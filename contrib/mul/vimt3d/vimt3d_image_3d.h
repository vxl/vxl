#ifndef vimt3d_image_3d_h_
#define vimt3d_image_3d_h_
//:
//  \file
//  \brief A base class for arbitrary 3D images+transform
//  \author Tim Cootes

#include <vimt/vimt_image.h>
#include <vimt3d/vimt3d_transform_3d.h>
#include <vil3d/vil3d_image_view_base.h>

class mil_transform_2d;

//: A base class for arbitrary 3D images
//  world2im() gives transformation from world to image co-ordinates
class vimt3d_image_3d : public vimt_image {
 protected:
  vimt3d_transform_3d world2im_;

  vimt3d_image_3d(const vimt3d_transform_3d& w2i) : world2im_(w2i) {};

 public:
  //: Dflt ctor
  vimt3d_image_3d() {};

  //: Destructor
  virtual ~vimt3d_image_3d() {};

    //: Return dimensionality of image
  virtual unsigned n_dims() const { return 3; }

    //: Return 3 element vector indicating size of image in pixels
    //  3D image is v[0] x v[1] x v[2]
    //  Somewhat inefficient: Only use when you absolutely have to.
    //  Usually one only needs to know the size once one knows the exact type.
  virtual vcl_vector<unsigned> image_size() const;

    //: Return 3D vectors defining bounding box containing image in world co-ords
  virtual void world_bounds(vcl_vector<double>& b_lo,
                            vcl_vector<double>& b_hi) const;


  //: Current world-to-image transformation
  const vimt3d_transform_3d& world2im() const { return world2im_; }

  //: Set world-to-image transformation
  void set_world2im(const vimt3d_transform_3d& w2i) { world2im_ = w2i ;}

  //: Baseclass view of image
  virtual const vil3d_image_view_base& image_base() const = 0;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const{ return s=="vimt3d_image_3d"; }
};

#endif // vimt3d_image_3d_h_
