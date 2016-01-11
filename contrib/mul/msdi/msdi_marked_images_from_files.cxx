//:
// \file
// \author Tim Cootes
// \brief Iterator for images and points stored in files

#include <vcl_cassert.h>
#include <vcl_string.h>

#include "msdi_marked_images_from_files.h"
#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_load.h>
#include <vimt/vimt_convert.h>


msdi_marked_images_from_files::msdi_marked_images_from_files()
  : grey_only_(true),image_ok_(false),image_pyr_ok_(false),
    points_ok_(false),load_as_float_(false),unit_scaling_(1000.0f),index_(0)
{
  pyr_builder_.set_min_size(24,24);
}

//: Construct to use the external images and points of given type
msdi_marked_images_from_files::msdi_marked_images_from_files(
                     const vcl_string& image_dir,
                     const vcl_vector<vcl_string>& image_names,
                     const vcl_string& points_dir,
                     const vcl_vector<vcl_string>& points_names)
  : grey_only_(true),unit_scaling_(1000.0f),index_(0)
{
  set(image_dir,image_names, points_dir, points_names);
}

//: Initialise with directories and filenames
void msdi_marked_images_from_files::set(const vcl_string& image_dir,
                                        const vcl_vector<vcl_string>& image_names,
                                        const vcl_string& points_dir,
                                        const vcl_vector<vcl_string>& points_names,
                                        bool load_as_float)
{
  assert(image_names.size()==points_names.size());
  image_dir_   = image_dir;
  image_name_  = image_names;
  points_dir_  = points_dir;
  points_name_ = points_names;

  index_ = 0;
  image_ok_=false;
  image_pyr_ok_=false;
  points_ok_=false;
  grey_only_=true;
  load_as_float_ = load_as_float;
}
//: Initialise with directories and filenames
void msdi_marked_images_from_files::set(const vcl_string& image_dir,
                                        const vcl_vector<vcl_string>& image_names,
                                        const vcl_string& points_dir)
{
  unsigned n=image_names.size();
  vcl_vector<vcl_string> points_names(n);

  for (unsigned i=0;i<n;++i)
  {
    points_names[i]=image_names[i]+".pts";
  }

  set(image_dir,image_names, points_dir, points_names);
}

//=======================================================================
// Destructor
//=======================================================================

msdi_marked_images_from_files::~msdi_marked_images_from_files()
{
}

//: When true, all images converted to greyscale (1 plane) on loading
void msdi_marked_images_from_files::set_convert_to_greyscale(bool b)
{
  grey_only_=b;
}

//: Scaling required to convert from units in image to desired world units
// (e.g. 1000 for mm if image units are metres)
// Only used if load_as_float_ is true.
void msdi_marked_images_from_files::set_unit_scaling(float s)
{
  unit_scaling_=s;
}


unsigned msdi_marked_images_from_files::size() const
{
  return image_name_.size();
}

//: Return current image
const vimt_image_2d& msdi_marked_images_from_files::image()
{
  assert(index_ < (int)size());
  if (!image_ok_) get_image();

  if (!load_as_float_)
    return image_;
  else
    return float_image_;
}

  //: Return current image pyramid
const vimt_image_pyramid& msdi_marked_images_from_files::image_pyr()
{
  assert(index_ < (int)size());
  if (!image_ok_) get_image();
  if (!image_pyr_ok_)
  {
    if (!load_as_float_)
    { pyr_builder_.build(image_pyr_,image_); }
    else
    { float_pyr_builder_.build(float_image_pyr_,float_image_); }

    image_pyr_ok_=true;
  }

  if (!load_as_float_)
    return image_pyr_;
  else
    return float_image_pyr_;
}

const msm_points& msdi_marked_images_from_files::points()
{
  assert(index_ < (int)size());
  if (!points_ok_)
  {
    get_points();
  }
  return points_;
}


//=======================================================================
//: Set to first example
//=======================================================================
void msdi_marked_images_from_files::reset()
{
  if (index_==0) return;
  index_=0;
  image_ok_=false;
  image_pyr_ok_=false;
  points_ok_=false;
}

//=======================================================================
//: Move to next example
//=======================================================================
bool msdi_marked_images_from_files::next()
{
  if (index_+1>=(int)size()) return false;
  ++index_;
  image_ok_=false;
  image_pyr_ok_=false;
  points_ok_=false;

  return true;
}


//: Load in current image and generate suitable pyramid
void msdi_marked_images_from_files::get_image()
{
  // Read in the image
  vcl_string image_path = image_dir_ + "/" + image_name_[index_];

  if (!load_as_float_)
  {
    vimt_load_to_byte(image_path, image_, unit_scaling_);

    if (image_.image().size()==0)
    {
      vcl_cerr<<"Empty image!\n";
    }

    if (grey_only_ && image_.image().nplanes()>1)
    {
      vil_image_view<vxl_byte> grey_image;
      vil_convert_planes_to_grey(image_.image(),grey_image);
      image_.image() = grey_image;
    }
  }
  else
  {
    vimt_load(image_path.c_str(),float_image_,unit_scaling_);

    if (float_image_.image().size()==0)
    {
      vcl_cerr<<"Empty image!\n";
    }

    if (grey_only_ && float_image_.image().nplanes()>1)
    {
      vil_image_view<float> grey_image;
      vil_convert_planes_to_grey(float_image_.image(),grey_image);
      float_image_.image() = grey_image;
    }
  }

  image_ok_=true;
}

void msdi_marked_images_from_files::get_points()
{
  // Read in the points
  vcl_string points_path = points_dir_ + "/" + points_name_[index_];
  if (!points_.read_text_file(points_path))
  {
    vcl_cerr<<"msdi_marked_images_from_files::get_points()"
              ": Unable to read points from "<<points_path << '\n';
  }
  else
    points_ok_=true;
}

//: Return current image file name
vcl_string msdi_marked_images_from_files::image_name() const
{
  assert(index_ < (int)size());
  return image_name_[index_];
}

//: Return current points file name
vcl_string msdi_marked_images_from_files::points_name() const
{
  assert(index_ < (int)size());
  return points_name_[index_];
}

