#ifndef vimt3d_image_3d_h_
#define vimt3d_image_3d_h_
//:
// \file
// \brief A base class for arbitrary 3D images+transform
// \author Tim Cootes

#include <vgl/vgl_box_3d.h>
#include <vimt/vimt_image.h>
#include <vimt3d/vimt3d_transform_3d.h>
#include <vil3d/vil3d_image_view_base.h>

//: A base class for arbitrary 3D images
//  world2im() gives transformation from world to image co-ordinates
class vimt3d_image_3d : public vimt_image
{
 protected:
  vimt3d_transform_3d world2im_;

  vimt3d_image_3d(const vimt3d_transform_3d& w2i) : world2im_(w2i) {}

 public:
  //: Dflt ctor
  vimt3d_image_3d() = default;

  //: Destructor
  ~vimt3d_image_3d() override = default;

  //: Return dimensionality of image
  unsigned n_dims() const override { return 3; }

  //: Return 3 element vector indicating size of image in pixels
  //  3D image is v[0] x v[1] x v[2]
  //  Somewhat inefficient: Only use when you absolutely have to.
  //  Usually one only needs to know the size once one knows the exact type.
  std::vector<unsigned> image_size() const override;

  //: Return 3D vectors defining bounding box containing image in world co-ords
  //  Somewhat inefficient: Only use when you absolutely have to.
  //  Usually one only needs to know the size once one knows the exact type.
  void world_bounds(std::vector<double>& b_lo,
                            std::vector<double>& b_hi) const override;

  //: Return 3 element vector indicating the size of a pixel
  //  Somewhat inefficient: Only use when you absolutely have to.
  //  Usually one only needs to know the size once one knows the exact type.
  std::vector<double> pixel_size() const override;

  //: Current world-to-image transformation
  const vimt3d_transform_3d& world2im() const { return world2im_; }

  //: Current world-to-image transformation
  vimt3d_transform_3d& world2im() { return world2im_; }

  //: Set world-to-image transformation
  // \deprecated in favour of non-const world2im()
  void set_world2im(const vimt3d_transform_3d& w2i) { world2im_ = w2i ;}

  //: Baseclass view of image
  virtual const vil3d_image_view_base& image_base() const = 0;

  //: Name of the class
  std::string is_a() const override { return "vimt3d_image_3d"; }

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override
  { return s=="vimt3d_image_3d" || vimt_image::is_class(s); }
};

// Related Functions

//: Return bounding box containing input image in world co-ords, but more conveniently as a box
// This may be more convenient than the similar class method in generic vector form,
// as the latter is for a general number of dimensions
vgl_box_3d<double> world_bounding_box(const vimt3d_image_3d& img);


//: Translate the image transform so that the image centre is at the world origin.
void vimt3d_centre_image_at_origin(vimt3d_image_3d& image);


//: Calculate the voxel dimensions from the image transform
vgl_vector_3d<double> vimt3d_voxel_size_from_transform(const vimt3d_image_3d& image);


#endif // vimt3d_image_3d_h_
