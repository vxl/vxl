
//:
// \file
// \author Tim Cootes
// \brief Adaptor which generates reflected versions of images/points

#include <iostream>
#include <string>
#include <utility>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include "msdi_reflected_marked_images.h"
#include <vimt/vimt_image_2d_of.h>
#include <vil/vil_flip.h>
#include <msm/msm_reflect_shape.h>

//: Construct with external vectors of images and points
msdi_reflected_marked_images::msdi_reflected_marked_images(
                        msdi_marked_images& raw_data,
                        std::vector<unsigned>  sym_pts,
                        bool only_reflect)
  : marked_images_(raw_data),
    sym_pts_(std::move(sym_pts)),
    only_reflect_(only_reflect)
{
  pyr_builder_.set_min_size(24,24);
  reset();
}

//=======================================================================
// Destructor
//=======================================================================

msdi_reflected_marked_images::~msdi_reflected_marked_images() = default;

unsigned msdi_reflected_marked_images::size() const
{
  if (only_reflect_) return marked_images_.size();

  return marked_images_.size()*2;
}

//: Return current image
const vimt_image_2d& msdi_reflected_marked_images::image()
{
  if (!image_ok_) get_image();
  return image_;
}


  //: Return current image pyramid
const vimt_image_pyramid& msdi_reflected_marked_images::image_pyr()
{
  if (!image_ok_) get_image();
  if (!image_pyr_ok_)
  {
    pyr_builder_.build(image_pyr_,image_);
    image_pyr_ok_=true;
  }

  return image_pyr_;
}

const msm_points& msdi_reflected_marked_images::points()
{
  if (!points_ok_) get_points();
  return points_;
}

//=======================================================================
//: Set to first example
//=======================================================================
void msdi_reflected_marked_images::reset()
{
  marked_images_.reset();
  first_pass_=true;
  points_ok_=false;
  image_ok_=false;
  image_pyr_ok_=false;
}

//=======================================================================
//: Move to next example
//=======================================================================
bool msdi_reflected_marked_images::next()
{
  if (!marked_images_.next())
  {
    if (first_pass_ && !only_reflect_)
    {
      first_pass_=false;
      marked_images_.reset();
    }
    else return false;
  }
  points_ok_=false;
  image_ok_=false;
  image_pyr_ok_=false;

  return true;
}


//: Load in current image and generate suitable pyramid
void msdi_reflected_marked_images::get_image()
{
  assert(marked_images_.image().is_a()=="vimt_image_2d_of<vxl_byte>");
  const auto& b_im
    = static_cast<const vimt_image_2d_of<vxl_byte>&>(marked_images_.image());

  if (!first_pass_)
  {
    image_ = b_im;
    image_ok_=true;
    return;
  }

  // First pass through, so reflect the image
  image_.image()=vil_flip_lr(b_im.image());
  image_.set_world2im(b_im.world2im());
  image_ok_=true;
}

void msdi_reflected_marked_images::get_points()
{
  if (!first_pass_)
  {
    points_ = marked_images_.points();
    points_ok_=true;
    return;
  }

  // First pass through, so reflect the points about image midline.
  double ax = 0.5*(image().image_base().ni()-1);

  // Project points into image frame, reflect, then project back into world frame.
  msm_points im_points=marked_images_.points();
  im_points.transform_by(image().world2im());
  msm_reflect_shape_along_x(im_points,
                            sym_pts_,points_,ax);
  points_.transform_by(image().world2im().inverse());  // Map back to world co-ords
  points_ok_=true;
}


//: Return current image file name
std::string msdi_reflected_marked_images::image_name() const
{
  if (!first_pass_) return marked_images_.image_name();

  return "ref_"+marked_images_.image_name();
}

//: Return current points file name
std::string msdi_reflected_marked_images::points_name() const
{
  if (!first_pass_) return marked_images_.points_name();

  return "ref_"+marked_images_.points_name();
}
