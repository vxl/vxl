#ifndef vimt_image_2d_h_
#define vimt_image_2d_h_

//: \file
//  \brief A base class for arbitrary 2D images+transform
//  \author Tim Cootes

#include <vimt/vimt_image.h>
#include <vimt/vimt_transform_2d.h>
#include <vil2/vil2_image_view_base.h>

class mil_transform_2d;

//: A base class for arbitrary 2D images
//  world2im() gives transformation from world to image co-ordinates
class vimt_image_2d : public vimt_image {
protected:
  vimt_transform_2d world2im_;

  vimt_image_2d(const vimt_transform_2d& w2i) : world2im_(w2i) {};

public:
  //: Dflt ctor
  vimt_image_2d() {};

  //: Destructor
  virtual ~vimt_image_2d() {};

  //: Current world-to-image transformation
  const vimt_transform_2d& world2im() const { return world2im_; }

  //: Set world-to-image transformation
  void set_world2im(const vimt_transform_2d& w2i) { world2im_ = w2i ;}

  //: Baseclass view of image
  virtual const vil2_image_view_base& image_base() const = 0;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const
    { return (s=="vimt_image_2d"); }
};

#endif // vimt_image_2d_h_
