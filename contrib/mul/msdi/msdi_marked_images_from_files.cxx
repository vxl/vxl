//:
// \file
// \author Tim Cootes
// \brief Iterator for images and points stored in files

#include <iostream>
#include <string>
#include <iterator>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include "msdi_marked_images_from_files.h"
#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_load.h>
#include <vimt/vimt_convert.h>

#include <mbl/mbl_parse_colon_pairs_list.h>


//: Return string (e.g. "Raw") for given state
std::string msdi_string_from_state(msdi_reflection_state s)
{
  switch (s)
  {
    case Raw: return std::string("Raw");
    case ReflectOnly: return std::string("ReflectOnly");
    case OnlyReflectIm: return std::string("OnlyReflectIm");
    case ReflectSym: return std::string("ReflectSym");
    case ReflectAsymRawPts: return std::string("ReflectAsymRawPts");
    case ReflectAsymRefPts: return std::string("ReflectAsymRefPts");
  }
  return std::string("Invalid");
}

//: Convert string to state, returning true if valid string supplied.
bool msdi_state_from_string(const std::string& str, msdi_reflection_state& state)
{
  if (str=="Raw")          { state=Raw; return true; }
  if (str=="ReflectOnly")  { state=ReflectOnly; return true; }
  if (str=="OnlyReflectIm"){ state=OnlyReflectIm; return true; }
  if (str=="ReflectSym")   { state=ReflectSym; return true; }
  if (str=="ReflectAsymRawPts")  { state=ReflectAsymRawPts; return true; }
  if (str=="ReflectAsymRefPts")  { state=ReflectAsymRefPts; return true; }

  state=Raw;
  // No valid string available
  return false;
}

msdi_marked_images_from_files::msdi_marked_images_from_files()
  : ref_state_(Raw),grey_only_(true),unit_scaling_(1000.0f),
    image_ok_(false),image_pyr_ok_(false),
    points_ok_(false),load_as_float_(false),index_(0)
{
  pyr_builder_.set_min_size(24,24);
  points_are_reflected_=false;
}

//: Construct to use the external images and points of given type
msdi_marked_images_from_files::msdi_marked_images_from_files(
                     const std::string& image_dir,
                     const std::vector<std::string>& image_names,
                     const std::string& points_dir,
                     const std::vector<std::string>& points_names)
  : grey_only_(true),unit_scaling_(1000.0f),index_(0)
{
  set(image_dir,image_names, points_dir, points_names);
}

//: Initialise with directories and filenames
void msdi_marked_images_from_files::set(const std::string& image_dir,
                                        const std::vector<std::string>& image_names,
                                        const std::string& points_dir,
                                        const std::vector<std::string>& points_names,
                                        bool load_as_float)
{
  assert(image_names.size()==points_names.size());
  image_dir_   = image_dir;
  image_name_  = image_names;
  points_dir_  = points_dir;
  points_name_ = points_names;

  grey_only_=true;
  load_as_float_ = load_as_float;
  ref_state_=Raw;
  reset();
}
//: Initialise with directories and filenames
void msdi_marked_images_from_files::set(const std::string& image_dir,
                                        const std::vector<std::string>& image_names,
                                        const std::string& points_dir,
                                        bool load_as_float)
{
  unsigned n=image_names.size();
  std::vector<std::string> points_names(n);

  for (unsigned i=0;i<n;++i)
  {
    points_names[i]=image_names[i]+".pts";
  }

  set(image_dir,image_names, points_dir, points_names, load_as_float);
}

//=======================================================================
// Destructor
//=======================================================================

msdi_marked_images_from_files::~msdi_marked_images_from_files() = default;

//: Reflect current points
void msdi_marked_images_from_files::reflect_points()
{
  if (points_.size()==0) return;

  // Need image to calculate reflection plane
  if (!image_ok_) get_image();

  // Image voxels will be reflected about line x=0.5(ni-1).
  double ax=0.5*(image().image_size()[0]-1);

  if (ref_point_index_.size()==0)
  {
    // Set 1-1 matching
    ref_point_index_.resize(points_.size());
    for (unsigned i=0;i<points_.size();++i)
      ref_point_index_[i]=i;
  }

  // Project points to image frame, reflect, then project back.
  points_.transform_by(image().world2im());  // Transform to image
  msm_points ref_points;
  msm_reflect_shape_along_x(points_,ref_point_index_,ref_points,ax);
  points_=ref_points;
  points_.transform_by(image().world2im().inverse()); // Map back to world.

  points_are_reflected_=true;
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
  // Number of images returned for each image listed:
  unsigned s=1;
  if (ref_state_==ReflectSym || ref_state_==ReflectAsymRawPts || ref_state_==ReflectAsymRefPts) s=2;

  return s*image_name_.size();
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
  image_is_reflected_=(ref_state_==ReflectOnly || ref_state_==OnlyReflectIm );
  points_are_reflected_=false;
}

//=======================================================================
//: Move to next example
//=======================================================================
bool msdi_marked_images_from_files::next()
{
  if (ref_state_==ReflectAsymRawPts && !image_is_reflected_)
  {
    // Current image is valid, so next is reflected version.
    if (!image_ok_) get_image();
    reflect_image();

    points_ok_=false;  // Force loading in of reflected set
    return true;
  }

  if (ref_state_==ReflectAsymRefPts && !image_is_reflected_)
  {
    // Current image is valid, so next is reflected version.
    if (!image_ok_) get_image();
    reflect_image();

    points_ok_=false;  // Force loading in of reflected set
    reflect_points();
    return true;
  }

  if (ref_state_==ReflectSym && !image_is_reflected_)
  {
    // Current image is valid, so next is reflected version.
    if (!image_ok_) get_image();

    // Reflect the current image.
    reflect_image();

    reflect_points();  // Reflect and re-number points
    return true;
  }

  if (index_+1>=(int)image_name_.size()) return false;

  ++index_;  // Move to next image
  if (ref_state_==ReflectOnly ||
      ref_state_==ReflectSym  ||
      ref_state_==ReflectAsymRawPts ||
      ref_state_==ReflectAsymRefPts)
    image_is_reflected_=false;

  image_ok_=false;
  image_pyr_ok_=false;
  points_ok_=false;
  points_are_reflected_=false;

  return true;
}


void msdi_marked_images_from_files::reflect_image()
{
  if (load_as_float_)
  {
    vil_image_view<float> ref_im = vil_flip_lr(float_image_.image());
    float_image_.image()=ref_im;
  }
  else
  {
    vil_image_view<vxl_byte> ref_im = vil_flip_lr(image_.image());
    image_.image()=ref_im;
  }
  image_is_reflected_=true;
  image_pyr_ok_=false;
}

//: Load in current image and generate suitable pyramid
void msdi_marked_images_from_files::get_image()
{
  // Read in the image
  std::string image_path = image_dir_ + "/" + image_name_[index_];

  if (!load_as_float_)
  {
    vimt_load_to_byte(image_path, image_, unit_scaling_);

    if (image_.image().size()==0)
    {
      std::cerr<<"Empty image!\n";
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
      std::cerr<<"Empty image!\n";
    }

    if (grey_only_ && float_image_.image().nplanes()>1)
    {
      vil_image_view<float> grey_image;
      vil_convert_planes_to_grey(float_image_.image(),grey_image);
      float_image_.image() = grey_image;
    }
  }

  image_is_reflected_=false;

  if (ref_state_==OnlyReflectIm || ref_state_==ReflectOnly)
  {
    reflect_image();
  }

  image_ok_=true;

}

void msdi_marked_images_from_files::get_points()
{
  std::string ref_prefix;

  // ReflectAsymRawPts state uses separate points files for reflected points:
  if (ref_state_==ReflectAsymRawPts && image_is_reflected_) ref_prefix=ref_prefix_;
  if (ref_state_==ReflectAsymRefPts && image_is_reflected_) ref_prefix=ref_prefix_;

  // Read in the points
  std::string points_path = points_dir_ + "/" + ref_prefix + points_name_[index_];
  if (!points_.read_text_file(points_path))
  {
    std::cerr<<"msdi_marked_images_from_files::get_points()"
              ": Unable to read points from "<<points_path << '\n';
  }
  else
    points_ok_=true;

  points_are_reflected_=false;

  if (ref_state_==ReflectOnly) reflect_points();
  if (ref_state_==ReflectAsymRefPts && image_is_reflected_) reflect_points();
}

//: Return current image file name
std::string msdi_marked_images_from_files::image_name() const
{
  assert(index_ < (int)image_name_.size());
  return image_name_[index_];
}

//: Return current points file name
std::string msdi_marked_images_from_files::points_name() const
{
  if (index_>=(int)points_name_.size())
  {
    std::cerr<<"Attempt to read beyond end of name array."<<std::endl;
    abort();
  }

  if (image_is_reflected_)
  {
    if (ref_state_==ReflectAsymRawPts ||
        ref_state_==ReflectAsymRefPts ||
        ref_state_==ReflectSym)
    return ref_prefix_+points_name_[index_];
  }
  return points_name_[index_];
}

void msdi_marked_images_from_files::set_state(msdi_reflection_state s)
{
  ref_state_=s;
  reset();
}

//: Define prefix to be used for reflected points.
// Default is "ref_".  Only used for ReflectAsymRawPts state.
void msdi_marked_images_from_files::set_ref_prefix(const std::string& ref_prefix)
{
  ref_prefix_=ref_prefix;
}

//: Define point mapping for reflected points
//  rpi[i] is index in old list of reflected point i
//  Only used for ReflectSym state.
void msdi_marked_images_from_files::set_ref_point_index(const std::vector<unsigned>& rpi)
{
  ref_point_index_ = rpi;
}

//: Set up from information in props object
// Expects to find parameters for reflection_symmetry, reflection_state and ref_prefix.
void msdi_marked_images_from_files::set_reflection_state_from_props(mbl_read_props_type& props)
{
  std::string ref_sym_str=props.get_optional_property("reflection_symmetry","");
  ref_point_index_.resize(0);
  if (ref_sym_str!="" && ref_sym_str!="-")
  {
    std::stringstream ss(ref_sym_str);
    mbl_parse_int_list(ss, std::back_inserter(ref_point_index_),
                       unsigned());
  }

  // For backwards compatability
  bool only_reflect=vul_string_to_bool(props.get_optional_property("only_reflect","false"));

  ref_prefix_=props.get_optional_property("ref_prefix","ref-");
  if (ref_prefix_=="-") ref_prefix_="";  // Use "-" to mean empty string.

  std::string ref_state_str=props.get_optional_property("reflection_state","Undefined");
  if (ref_state_str=="Undefined")
  {
    // For backwards compatability.
    if (ref_point_index_.size()>0)
    {
      if (only_reflect) ref_state_=ReflectOnly;
      else              ref_state_=ReflectSym;
    }
    else
    {
      if (only_reflect) ref_state_=ReflectOnly;
      else              ref_state_=Raw;
    }
  }
  else
  {
    if (!msdi_state_from_string(ref_state_str,ref_state_))
    {
      throw (mbl_exception_parse_error("Unknown reflection_state: "+ref_state_str));
    }
  }

}

//: Set up from information in props object
// Expects to find parameters for images, image_dir, points_dir, and optionally
// reflection_symmetry, reflection_state and ref_prefix.
void msdi_marked_images_from_files::set_from_props(mbl_read_props_type& props)
{
  image_dir_=props.get_optional_property("image_dir","./");
  points_dir_=props.get_optional_property("points_dir","./");

  mbl_parse_colon_pairs_list(props.get_required_property("images"),
                             points_name_,image_name_);

  unsigned max_im_pyr_levels=vul_string_atoi(props.get_optional_property("max_im_pyr_levels","5"));
  pyr_builder().set_max_levels(max_im_pyr_levels);

  set_reflection_state_from_props(props);
}

//: Parse named text file to read in data
//  Throws a mbl_exception_parse_error if fails
void msdi_marked_images_from_files::read_from_file(const std::string& path)
{
  std::ifstream ifs(path.c_str());
  if (!ifs)
  {
    std::string error_msg = "Failed to open file: "+path;
    throw (mbl_exception_parse_error(error_msg));
  }

  mbl_read_props_type props = mbl_read_props_ws(ifs);

  set_from_props(props);
}
