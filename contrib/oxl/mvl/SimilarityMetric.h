// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
//-*- c++ -*-------------------------------------------------------------------
#ifndef SimilarityMetric_h_
#define SimilarityMetric_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : SimilarityMetric
//
// .SECTION Description
//    An ImageMetric that simply scales and translates.  Most often used
//    to condition points by transforming the image centre to the origin,
//    and scaling so that the diagonal has length 2.
//
// .NAME        SimilarityMetric - Scale + translate ImageMetric
// .LIBRARY     MViewBasics
// .HEADER	MultiView Package
// .INCLUDE     mvl/SimilarityMetric.h
// .FILE        SimilarityMetric.h
// .FILE        SimilarityMetric.C
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 04 Feb 97
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_double_3x3.h>
#include <mvl/ImageMetric.h>
#include <vcl/vcl_iosfwd.h>

class Image;

class SimilarityMetric : public ImageMetric {
public:
  // Constructors/Destructors--------------------------------------------------
  
  SimilarityMetric();
  SimilarityMetric(int xsize, int ysize);
  SimilarityMetric(double cx, double cy, double scale);

  //  SimilarityMetric(const Image* ); -- don't want a dependency on ImageClasses
  // SimilarityMetric(const SimilarityMetric& that); - use default
  ~SimilarityMetric();
  // SimilarityMetric& operator=(const SimilarityMetric& that); - use default

  void set_from_rectangle(int xsize, int ysize);
  void set_center_and_scale(double cx, double cy, double scale);
  void scale_matrices(double s);

  // Operations----------------------------------------------------------------
  virtual HomgPoint2D image_to_homg(const vnl_double_2&);
  virtual HomgPoint2D image_to_homg(double x, double y);
  virtual vnl_double_2 homg_to_image(const HomgPoint2D&);
  virtual HomgPoint2D imagehomg_to_homg(const HomgPoint2D&);
  virtual HomgPoint2D homg_to_imagehomg(const HomgPoint2D&);

  virtual double perp_dist_squared(HomgPoint2D const& p, HomgLine2D const& l);
  virtual double distance_squared(HomgPoint2D const&, HomgPoint2D const&);

  virtual bool is_linear() const { return true; }
  virtual const vnl_matrix<double>& get_C() const { return cond_matrix; }
  virtual const vnl_matrix<double>& get_C_inverse() const { return inv_cond_matrix; }

  virtual bool can_invert_distance() const;
  virtual double image_to_homg_distance(double image_distance) const;
  virtual double homg_to_image_distance(double image_distance) const;

  // virtual bool can_invert_distance() const { return true; }

  // Data Control--------------------------------------------------------------
  ostream& print(ostream&) const;
  void print() const;
  void print(char* msg) const;

protected:
  // Data Members--------------------------------------------------------------
  double _centre_x;
  double _centre_y;
  double _inv_scale;
  double _scale;

  vnl_double_3x3 cond_matrix;
  vnl_double_3x3 inv_cond_matrix;
  
private:
  // Helpers-------------------------------------------------------------------
  void make_matrices();
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS SimilarityMetric.
