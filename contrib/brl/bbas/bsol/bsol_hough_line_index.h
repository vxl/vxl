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
#ifndef bsol_hough_line_index_h_
#define bsol_hough_line_index_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief Hough transform for fast queries on line sets
//
// bsol_hough_line_index is a class for indexing 2-d lines and
// tangent vectors in the space of orientation and distance to the origin.
//
// The line index space is defined as follows:
// \verbatim
//                            o
//                           /
//                 line --> /\                  .
//                         /  |theta
//    <-------------------o--/----------------->
// \endverbatim
//
//  The angle, theta, corresponding to the line tangent direction
//  ranges from 0 to 180 degrees.  The perpendicular distance
//  to the origin, r, is defined as:
// \verbatim
//                   .           /
//                       .      / Line
//                 Line /   .  /
//                 Normal     /^ .
//                           /      r
//                          /           .
//                                         x Origin
// \endverbatim
//  r can be expressed as  r = N.(M - O)  where N is the line normal,
//  O is the origin position vector and M is the line midpoint.
//  Note that N = (-Sin(theta), Cos(theta)).
//  The values of theta and r for a vsol_line_2d are computed by
//  the method ::array_loc(..).
//  If the line is translated by (tx, ty), then the r for the translated
//  line is given by r' = r - Sin(theta)tx + Cos(theta)ty.
//  The value of r for a given translation is computed by ::trans_loc(..).
//
//  The lines are stored in a two dimensional array of r_dim_ by th_dim_ bins
//  of vector<vsol_line_2d_sptr>. The theta dimension of the array
//  is typically 180/5, or 36. The r dimension is defined by the diagonal
//  of the bounding box of the space containing the lines.  The resolution in
//  r is assumed to be 1.  It is necessary to specify the bounding box
//  of the line coordinate space at construction time, so that new lines
//  entered into the index do not exceed array bounds.
//
//  Internally, the r space is defined on an origin at the center of the
//  bounding box diagonal. This choice minimizes the error in normal distance
//  due to variations in line orientation.
//
//  Typical usage:
// - Create a new index for a 100x100 coordinate space with 5 degree angular res.
//   \code
//      index = new bsol_hough_line_index(0.0, 0.0, 100.0, 100.0, 180.0, 5.0)
//   \endcode
// - Add a line to the index
//   \code
//      ...
//      vsol_line_2d_sptr l(p1, p2);
//      index->index(l);
//   \endcode
// - find collinear lines (lines in a 2-d region in Hough space centered on the
//                         parameters defined by line l)
//   \code
//      ...
//      vcl_vector<vsol_line_2d_sptr> lines;
//      index->lines_in_interval(lines, l, 1.0, 5.0);//dr = 1.0, dtheta = 5.0
//                                                   //i.e. +- 1.0 and +- 5.0
//   \endcode
// - find lines at a particular orientation
//   \code
//      index->parallel_lines(lines, 45.0, 5.0); //Lines parallel to 45 deg.
//                                               //+- 5 deg.
//   \endcode
//
// \author J.L. Mundy December 1997
// \date   ported to VXL April 11, 2003
//
// \verbatim
//  Modifications
//   10-sep-2004 Peter Vanroose Added copy ctor with explicit vbl_ref_count init
// \endverbatim
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_bounding_box.h>
#include <vbl/vbl_array_2d.h>
#include <vsol/vsol_line_2d_sptr.h>

class bsol_hough_line_index :  public vbl_ref_count
{
  // PUBLIC INTERFACE----------------------------------------------------------

 public:

  // Constructors/initializers/Destructors-------------------------------------

  bsol_hough_line_index(const int r_dimension, const int theta_dimension);
  bsol_hough_line_index(const float x0, const float y0,
                        const float xsize, const float ysize,
                        const float angle_range=180.0,
                        const float angle_increment=5.0);

  bsol_hough_line_index(vbl_bounding_box<double, 2> const & box,
                        const float angle_range=180.0,
                        const float angle_increment=5.0);

  bsol_hough_line_index(bsol_hough_line_index const& i)
    : vbl_ref_count(), xo_(i.xo_), yo_(i.yo_),
      xsize_(i.xsize_), ysize_(i.ysize_),
      angle_range_(i.angle_range_), angle_increment_(i.angle_increment_),
      r_dim_(i.r_dim_), th_dim_(i.th_dim_), index_(i.index_) {}

  ~bsol_hough_line_index();

  // Data Access---------------------------------------------------------------

  float getxsize_() const {return xsize_;}
  float getysize_() const {return ysize_;}

  int get_r_dimension() const {return r_dim_;}
  int get_theta_dimension() const {return th_dim_;}

  //: Get the bsol_hough_line_index array location of a line segment
  void array_loc(vsol_line_2d_sptr const& line, float& r, float& theta);
  void array_loc(vsol_line_2d_sptr const& line, int& r, int& theta);

  //: r Location for a translated line position
  int trans_loc(const int transx, const int transy,
                const int ry, const int theta);

  //: Get line count at a particular location in bsol_hough_line_index space
  int count(const int r, const int theta);

  //: Insert a new line into the index
  bool index(vsol_line_2d_sptr const& line);

  //: Insert a unique new line into the index
  bool index_new(vsol_line_2d_sptr const& line);

  //: find if a line is in the index
  bool find(vsol_line_2d_sptr const& line);

  //: remove a line
  bool remove(vsol_line_2d_sptr const& line);

  //: Lines in a line index bin at integer r and theta bin indices.
  void lines_at_index(const int r, const int theta,
                      vcl_vector<vsol_line_2d_sptr>& lines);

  vcl_vector<vsol_line_2d_sptr > lines_at_index(const int r,
                                                const int theta);

  //: Lines in a tolerance box around the r and theta of a given line.
  // r is in distance units and theta is in degrees.
  void lines_in_interval(vsol_line_2d_sptr const& l,
                         const float r_dist, const float theta_dist,
                         vcl_vector<vsol_line_2d_sptr>& lines);

  vcl_vector<vsol_line_2d_sptr>
    lines_in_interval(vsol_line_2d_sptr const & l,
                      const float r_dist,
                      const float theta_dist);

  //:Lines parallel to a given angle in degrees
  void parallel_lines(const float angle,
                      const float angle_dist,
                      vcl_vector<vsol_line_2d_sptr>& lines);

  vcl_vector<vsol_line_2d_sptr> parallel_lines(const float angle,
                                               const float angle_dist);

  //: Lines at an angle to a given line (angle is in degrees)
  void lines_at_angle(vsol_line_2d_sptr const &l,
                      const float angle, const float angle_dist,
                      vcl_vector<vsol_line_2d_sptr >& lines);

  vcl_vector<vsol_line_2d_sptr>
    lines_at_angle(vsol_line_2d_sptr const &l,
                   const float angle, const float angle_dist);

  //: Lines parallel to a given line with angle_dist in degrees
  void parallel_lines(vsol_line_2d_sptr const &l,
                      const float angle_dist,
                      vcl_vector<vsol_line_2d_sptr>& lines);

  vcl_vector<vsol_line_2d_sptr>
    parallel_lines(vsol_line_2d_sptr const &l,
                   const float angle_dist);

  //: Angle histogram - projection of hough space onto theta axis
  vcl_vector<int> angle_histogram();

  //: Dominant line directions found by non-maximum suppression above thresh
  int dominant_directions(const int thresh, const float angle_tol,
                          vcl_vector<int>& dirs);

  //: Dominant parallel line groups
  int dominant_line_groups(const int thresh, const float angle_tol,
                           vcl_vector<vcl_vector<vsol_line_2d_sptr> >& groups);

  //: An image of the hough space
  vbl_array_2d<unsigned char> get_hough_image();

  // Data Control--------------------------------------------------------------

  void clear_index();

  // INTERNALS-----------------------------------------------------------------

 protected:
  //internal functions
  void init(const int r_dimension, const int theta_dimension);
  vcl_vector<int> non_maximum_suppress(const int radius,
                                       vcl_vector<int> const & bins);

  // Data Members--------------------------------------------------------------

 private:

  float xo_; //!< X Origin of the Cartesian Space
  float yo_; //!< Y Origin of the Cartesian Space

  float xsize_; //!< Dimensions of the Cartesian space
  float ysize_;

  float angle_range_; //!< Granularity of the line index
  float angle_increment_;

  int   r_dim_;  //!< The dimensions of the index space
  int   th_dim_;

  //: The index space for lines. An array of vectors of line indices
  vbl_array_2d<vcl_vector<vsol_line_2d_sptr>* > index_;
};

#endif
