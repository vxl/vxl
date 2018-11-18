#ifndef msdi_marked_images_h_
#define msdi_marked_images_h_

//:
// \file
// \author Tim Cootes
// \brief Base for iterator which returns images+points

class vimt_image_pyramid;
class vimt_image_2d;
class msm_points;

#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Base for iterator which returns images+points
class msdi_marked_images
{
 public:
  virtual ~msdi_marked_images() = default;

  //: Move to start of data
  virtual void reset()=0;

  //: Move to next item.  Return true until reach end of items
  virtual bool next()=0;

  //: Return number of examples this will provide
  virtual unsigned size() const=0;

  //: Return current image
  virtual const vimt_image_2d& image() = 0;

  //: Return current image pyramid
  virtual const vimt_image_pyramid& image_pyr() = 0;

  //: points for the current image
  virtual const msm_points& points() = 0;

    //: Return name for current image
  virtual std::string image_name() const = 0;

    //: Return name for current points
  virtual std::string points_name() const = 0;

};


#endif // msdi_marked_images_h_
