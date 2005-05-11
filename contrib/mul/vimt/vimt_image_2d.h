#ifndef vimt_image_2d_h_
#define vimt_image_2d_h_
//:
// \file
// \brief A base class for arbitrary 2D images+transform
// \author Tim Cootes

#include <vimt/vimt_image.h>
#include <vimt/vimt_transform_2d.h>
#include <vil/vil_image_view_base.h>
#include <vgl/vgl_box_2d.h>
class mil_transform_2d;

//: A base class for arbitrary 2D images
//  world2im() gives transformation from world to image co-ordinates
class vimt_image_2d : public vimt_image
{
 protected:
  vimt_transform_2d world2im_;

  vimt_image_2d(const vimt_transform_2d& w2i) : world2im_(w2i) {};

 public:
  //: Dflt ctor
  vimt_image_2d() {};

  //: Destructor
  virtual ~vimt_image_2d() {};

  //: Return dimensionality of image
  virtual unsigned n_dims() const { return 2; }

  //: Return vector indicating size of image in pixels
  //  2D image is v[0] x v[1]
  //  Somewhat inefficient: Only use when you absolutely have to.
  //  Usually one only needs to know the size once one knows the exact type.
  virtual vcl_vector<unsigned> image_size() const;

  //: Return vectors defining bounding box containing image in world co-ords
  virtual void world_bounds(vcl_vector<double>& b_lo,
                            vcl_vector<double>& b_hi) const;

 //: Return bounding box containing image in world co-ords, but more conveniently as a box
  vgl_box_2d<double> world_bounding_box() const;
  
  //: Current world-to-image transformation
  const vimt_transform_2d& world2im() const { return world2im_; }

  //: Set world-to-image transformation
  void set_world2im(const vimt_transform_2d& w2i) { world2im_ = w2i ;}

  //: Baseclass view of image
  virtual const vil_image_view_base& image_base() const = 0;

  //: Name of the class
  virtual vcl_string is_a() const { return "vimt_image_2d"; }

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const
  { return s=="vimt_image_2d" || vimt_image::is_class(s); }
};

#endif // vimt_image_2d_h_
