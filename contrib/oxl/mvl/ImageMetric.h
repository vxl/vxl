// This is oxl/mvl/ImageMetric.h
#ifndef ImageMetric_h_
#define ImageMetric_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Converting between image and metric coordinates
//
//    ImageMetric is the baseclass for classes that define how points in image
//    coordinates are converted to conditioned frames.  For simple systems this
//    will represent the mapping of the image plane to the unit square, but for
//    a fully calibrated camera, for example, this might include corrections for
//    radial lens distortion and other nonlinear effects.
//
//    The default implementation in this baseclass simply assumes an identity
//    mapping between the two coordinate systems.
//
//    A general convention that is sometimes useful is that points in homogeneous
//    coordinates have been conditioned, while nonhomogeneous primitives remain in
//    image coordinates.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 17 Aug 96
//
// \verbatim
//  Modifications
//   22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_fwd.h>
#include <vgl/vgl_fwd.h>
#include <vcl_iosfwd.h>
class HomgPoint2D;
class HomgLineSeg2D;
class HomgLine2D;
class FMatrix;

class ImageMetric
{
 public:
  // Constructors/Destructors--------------------------------------------------
  ImageMetric() {}
  virtual ~ImageMetric() {}
  //ImageMetric(ImageMetric const& that); - use default
  //ImageMetric& operator=(ImageMetric const& that); - use default

  virtual vgl_homg_point_2d<double> homg_to_imagehomg(vgl_homg_point_2d<double> const&) const;
  virtual vgl_homg_point_2d<double> imagehomg_to_homg(vgl_homg_point_2d<double> const&) const;

  virtual HomgPoint2D homg_to_imagehomg(const HomgPoint2D&) const;
  virtual HomgPoint2D imagehomg_to_homg(const HomgPoint2D&) const;

  // The following virtuals may be overridden if desired.
  // By default they are implemented in terms of the previous two
  virtual vgl_point_2d<double> homg_to_image(vgl_homg_point_2d<double> const&) const;
  virtual vnl_double_2 homg_to_image(const HomgPoint2D&) const;

  virtual vgl_homg_point_2d<double> image_to_homg(vgl_point_2d<double> const&) const;
  virtual HomgPoint2D image_to_homg(const vnl_double_2&) const;
  virtual HomgPoint2D image_to_homg(double x, double y) const;

  virtual vgl_homg_line_2d<double> homg_to_image_line(vgl_homg_line_2d<double> const&) const;
  virtual vgl_homg_line_2d<double> image_to_homg_line(vgl_homg_line_2d<double> const&) const;

  virtual HomgLine2D homg_to_image_line(const HomgLine2D&) const;
  virtual HomgLine2D image_to_homg_line(const HomgLine2D&) const;

  virtual HomgLineSeg2D image_to_homg_line(const HomgLineSeg2D&) const;
  virtual vgl_line_segment_2d<double> image_to_homg_line(vgl_line_segment_2d<double> const& l) const;
  virtual HomgLineSeg2D homg_line_to_image(const HomgLineSeg2D&) const;
  virtual vgl_line_segment_2d<double> homg_line_to_image(vgl_line_segment_2d<double> const& l) const;

  virtual double perp_dist_squared(const HomgPoint2D&, const HomgLine2D&) const;
  virtual double perp_dist_squared(vgl_homg_point_2d<double> const&,
                                   vgl_homg_line_2d<double> const&) const;
  virtual HomgPoint2D perp_projection(const HomgLine2D & l, const HomgPoint2D & p) const;
  virtual vgl_homg_point_2d<double> perp_projection(vgl_homg_line_2d<double> const& l,
                                                    vgl_homg_point_2d<double> const& p) const;
  virtual double distance_squared(const HomgPoint2D&, const HomgPoint2D&) const;
  virtual double distance_squared(const HomgLineSeg2D& segment, const HomgLine2D& line) const;// ca_distance_squared_lineseg_to_line
  virtual double distance_squared(const vgl_homg_point_2d<double>&, const vgl_homg_point_2d<double>&) const;
  virtual double distance_squared(vgl_line_segment_2d<double> const& segment,
                                  vgl_homg_line_2d<double> const& line) const;

  virtual bool is_within_distance(const HomgPoint2D&, const HomgPoint2D&, double distance) const;
  virtual bool is_within_distance(vgl_homg_point_2d<double> const&,
                                  vgl_homg_point_2d<double> const&,
                                  double distance) const;

  // Data Access---------------------------------------------------------------
  virtual vnl_double_3x3 get_C() const;
  virtual vnl_double_3x3 get_C_inverse() const;

  virtual bool is_linear() const;
  virtual bool can_invert_distance() const;
  virtual double image_to_homg_distance(double image_distance) const;
  virtual double homg_to_image_distance(double image_distance) const;

  virtual vcl_ostream& print(vcl_ostream& s) const;

  // Data Control--------------------------------------------------------------

  // Static functions to condition/decondition image relations
  static FMatrix decondition(const FMatrix& F, const ImageMetric* c1, const ImageMetric* c2);

  inline friend vcl_ostream& operator<<(vcl_ostream& o, const ImageMetric& m) { return m.print(o); }
};

#endif // ImageMetric_h_
