
//:
// \file
// \author Tim Cootes
// \brief Adaptor which steps through images and returns a subset of original points

#include <iostream>
#include <string>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include "msdi_points_subset.h"

//: Construct with external vectors of images and points
msdi_points_subset::msdi_points_subset(
                        msdi_marked_images& raw_data,
                        const std::vector<unsigned>& index)
  : marked_images_(raw_data), index_(index)
{
  points_.set_size(index.size());
  reset();
}

//=======================================================================
// Destructor
//=======================================================================

msdi_points_subset::~msdi_points_subset() = default;

unsigned msdi_points_subset::size() const
{
  return marked_images_.size();
}

//: Return current image
const vimt_image_2d& msdi_points_subset::image()
{
  return marked_images_.image();
}


  //: Return current image pyramid
const vimt_image_pyramid& msdi_points_subset::image_pyr()
{
  return marked_images_.image_pyr();
}

const msm_points& msdi_points_subset::points()
{
  if (!points_ok_)
  {
    // Get subset
    for (size_t i=0;i<index_.size();++i)
      points_.set_point(i,marked_images_.points()[index_[i]]);
    points_ok_=true;
  }
  return points_;
}

//=======================================================================
//: Set to first example
//=======================================================================
void msdi_points_subset::reset()
{
  marked_images_.reset();
  points_ok_=false;
}

//=======================================================================
//: Move to next example
//=======================================================================
bool msdi_points_subset::next()
{
  if (!marked_images_.next())
  {
    return false;
  }
  points_ok_=false;

  return true;
}

//: Return current image file name
std::string msdi_points_subset::image_name() const
{
  return marked_images_.image_name();
}

//: Return current points file name
std::string msdi_points_subset::points_name() const
{
  return marked_images_.points_name();
}
