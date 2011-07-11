
//:
// \file
// \author Tim Cootes
// \brief Iterator for images and points stored in arrays

#include <vcl_cassert.h>
#include <vcl_string.h>
#include <vcl_sstream.h>

#include "msdi_array_of_marked_images.h"
#include <vimt/vimt_image_2d_of.h>
#include <vil/vil_convert.h>

msdi_array_of_marked_images::msdi_array_of_marked_images()
  : grey_only_(true),image_pyr_ok_(false),index_(0)
{
  pyr_builder_.set_min_size(24,24);
}

//: Construct with external vectors of images and points
msdi_array_of_marked_images::msdi_array_of_marked_images(
                 const vcl_vector<vil_image_view<vxl_byte> >& images,
                 const vcl_vector<msm_points>& points,
                 bool grey_only)
  : images_(0),points_(0),index_(0)
{
  set(images,points,grey_only);
}

//: Initialise with directories and filenames
void msdi_array_of_marked_images::set(
           const vcl_vector<vil_image_view<vxl_byte> >& images,
           const vcl_vector<msm_points>& points,
           bool grey_only)
{
  assert(images.size()==points.size());

  grey_only_=grey_only;
  images_=&images;
  points_=&points;

  index_ = 0;
  image_ok_=false;
  image_pyr_ok_=false;
}

//=======================================================================
// Destructor
//=======================================================================

msdi_array_of_marked_images::~msdi_array_of_marked_images()
{
}

unsigned msdi_array_of_marked_images::size() const
{
  assert(images_!=0);
  return images_->size();
}

//: Return current image
const vimt_image_2d& msdi_array_of_marked_images::image()
{
  assert(images_!=0);
  assert(index_ < (int)size());
  if (!image_ok_) get_image();
  return image_;
}


  //: Return current image pyramid
const vimt_image_pyramid& msdi_array_of_marked_images::image_pyr()
{
  assert(images_!=0);
  assert(index_ < (int)size());
  if (!image_ok_) get_image();
  if (!image_pyr_ok_)
  {
    pyr_builder_.build(image_pyr_,image_);
    image_pyr_ok_=true;
  }

  return image_pyr_;
}

const msm_points& msdi_array_of_marked_images::points()
{
  assert(points_!=0);
  assert(index_ < (int)size());
  return points_->operator[](index_);
}

//=======================================================================
//: Set to first example
//=======================================================================
void msdi_array_of_marked_images::reset()
{
  if (index_==0) return;
  index_=0;
  image_ok_=false;
  image_pyr_ok_=false;
}

//=======================================================================
//: Move to next example
//=======================================================================
bool msdi_array_of_marked_images::next()
{
  if (index_+1>=(int)size()) return false;
  index_++;
  image_ok_=false;
  image_pyr_ok_=false;

  return true;
}


//: Load in current image and generate suitable pyramid
void msdi_array_of_marked_images::get_image()
{
  image_.image() = images_->operator[](index_);
  if (image_.image().size()==0)
  {
    vcl_cerr<<"Empty image! "<<vcl_endl;
  }
  if (grey_only_ && image_.image().nplanes()>1)
  {
    vil_image_view<vxl_byte> grey_image;
    vil_convert_planes_to_grey(image_.image(),grey_image);
    image_.image() = grey_image;
  }
  image_ok_=true;
}

//: Return current image file name
vcl_string msdi_array_of_marked_images::image_name() const
{
  assert(index_ < (int)size());
  vcl_stringstream ss;
  ss<<"image"<<index_<<".png";
  return ss.str();
}

//: Return current points file name
vcl_string msdi_array_of_marked_images::points_name() const
{
  assert(index_ < (int)size());
  vcl_stringstream ss;
  ss<<"points"<<index_<<".pts";
  return ss.str();
}



