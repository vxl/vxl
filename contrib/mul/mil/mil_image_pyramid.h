// This is mul/mil/mil_image_pyramid.h
#ifndef mil_image_pyramid_h_
#define mil_image_pyramid_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Pyramid of images of arbitrary type
// \author Tim Cootes

#include <vcl_vector.h>
#include <vcl_iosfwd.h>
class mil_image;

//: Pyramid of images of arbitrary type
class mil_image_pyramid
{
  vcl_vector<mil_image*> image_;
  double base_pixel_width_;
  double scale_step_;

  void deleteImages();
 public:
  //: Dflt ctor
  mil_image_pyramid();

  //: Copy ctor
  // Makes a shallow copy of each mil_image object, not of the
  // underlying data
  mil_image_pyramid(const mil_image_pyramid &);

  //: Destructor
  virtual ~mil_image_pyramid();

  //: Resize to n_levels pyramid
  //  Each level of which is a clone of im_type
  void resize(int n_levels, const mil_image& im_type);

  //: Lowest level (highest resolution image) of pyramid
  int lo() const;

  //: Highest level (lowest resolution image) of pyramid
  int hi() const;

  //: Number of levels
  int n_levels() const;

  //: Image at level L
  mil_image& operator()(int L);

  //: Image at level L
  const mil_image& operator()(int L) const;

  //: Copy operator
  // Makes a shallow copy of each mil_image object, not of the
  // underlying data
  const mil_image_pyramid& operator=(const mil_image_pyramid& that);

  //: Mean width (in world coordinates) of pixels at level zero
  double basePixelWidth() const;

  //: Scaling per level
  //  Pixels at level L have width
  //  basePixelWidth() * scaleStep()^L
  double scale_step() const;

  //: Access to image data
  //  Should only be used by pyramid builders
  vcl_vector<mil_image*>& data();

  //: Define pixel widths
  void setWidths(double base_pixel_width, double scale_step);

  void print_summary(vcl_ostream& os) const;

  //: Print whole of each image to os
  void print_all(vcl_ostream& os) const;
};

//: Convert an image pyramid into a flat image containing each layer.
// E.g. a three layer 2d image pyramid with scale factor 2.0
// would be converted to an image looking like
// \verbatim
// ________________
// |11111111222233|
// |111111112222  |
// |11111111      |
// |11111111      |
// ----------------
// \endverbatim
template <class T> void mil_image_pyramid_flatten(T& out, const mil_image_pyramid & in);

vcl_ostream& operator<<(vcl_ostream& os, const mil_image_pyramid& im_pyr);
vcl_ostream& operator<<(vcl_ostream& os, const mil_image_pyramid* im_pyr);
void vsl_print_summary(vcl_ostream& os, const mil_image_pyramid& im_pyr);
void vsl_print_summary(vcl_ostream& os, const mil_image_pyramid* im_pyr);

#define MIL_IMAGE_PYRAMID_INSTANTIATE(T) \
extern "please #include mil/mil_image_pyramid.txx instead"

#endif // mil_image_pyramid_h_
