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

//: A base class for arbitrary 2D images
//  world2im() gives transformation from world to image co-ordinates
class vimt_image_2d : public vimt_image
{
 protected:
  vimt_transform_2d world2im_;

  vimt_image_2d(const vimt_transform_2d& w2i) : world2im_(w2i) {}

 public:
  //: Dflt ctor
  vimt_image_2d() = default;

  //: Destructor
  virtual ~vimt_image_2d() = default;

  //: Return dimensionality of image
  virtual unsigned n_dims() const { return 2; }

  //: Return vector indicating size of image in pixels
  //  2D image is v[0] x v[1]
  //  Somewhat inefficient: Only use when you absolutely have to.
  //  Usually one only needs to know the size once one knows the exact type.
  virtual std::vector<unsigned> image_size() const;

  //: Return 2 element vector indicating the size of a pixel
  //  Somewhat inefficient: Only use when you absolutely have to.
  //  Usually one only needs to know the size once one knows the exact type.
  virtual std::vector<double> pixel_size() const;

  //: Return vectors defining bounding box containing image in world co-ords
  //  Somewhat inefficient: Only use when you absolutely have to.
  //  Usually one only needs to know the size once one knows the exact type.
  virtual void world_bounds(std::vector<double>& b_lo,
                            std::vector<double>& b_hi) const;

  //: Current world-to-image transformation
  const vimt_transform_2d& world2im() const { return world2im_; }

  //: Current world-to-image transformation
  vimt_transform_2d& world2im() { return world2im_; }

  //: Set world-to-image transformation
  // \deprecated in favour of non-const world2im()
  void set_world2im(const vimt_transform_2d& w2i) { world2im_ = w2i ;}

  //: Baseclass view of image
  virtual const vil_image_view_base& image_base() const = 0;

  //: Name of the class
  virtual std::string is_a() const { return "vimt_image_2d"; }

  //: Does the name of the class match the argument?
  virtual bool is_class(std::string const& s) const
  { return s=="vimt_image_2d" || vimt_image::is_class(s); }
};


//: Related Functions

//: Return bounding box containing input image in world co-ords, but more conveniently as a box
// This may be more convenient than the similar class method in generic vector form,
// as the latter is for a general number of dimensions
vgl_box_2d<double> world_bounding_box(const vimt_image_2d& img);


//: Translate the image so that its centre is at the origin of the world coordinate system.
void vimt_centre_image_at_origin(vimt_image_2d& image);


//: Calculate the pixel dimensions from the image transform
vgl_vector_2d<double> vimt_pixel_size_from_transform(const vimt_image_2d& image);


#endif // vimt_image_2d_h_
