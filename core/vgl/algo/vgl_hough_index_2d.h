#ifndef vgl_hough_index_2d_h_
#define vgl_hough_index_2d_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief Hough transform for fast queries on line sets
//
// vgl_hough_index_2d is a class for indexing 2-d lines and
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
//  O is the origin position vector and M is the line midposize_t.
//  Note that N = (-Sin(theta), Cos(theta)).
//  The values of theta and r for a vsol_line_2d are computed by
//  the method ::array_loc(..).
//  If the line is translated by (tx, ty), then the r for the translated
//  line is given by r' = r - Sin(theta)tx + Cos(theta)ty.
//  The value of r for a given translation is computed by ::trans_loc(..).
//
//  The lines are stored in a two dimensional array of r_dim_ by th_dim_ bins
//  of vector<vgl_line_2d<T> >. The theta dimension of the array
//  is typically 180/5, or 36. The r dimension is defined by the diagonal
//  of the bounding box of the space containing the lines.  The resolution in
//  r is assumed to be 1.  It is necessary to specify the bounding box
//  of the line coordinate space at construction time, so that new lines
//  entered size_to the index do not exceed array bounds.
//
//  internally, the r space is defined on an origin at the center of the
//  bounding box diagonal. This choice minimizes the error in normal distance
//  due to variations in line orientation.
//
//  Typical usage:
// - Create a new index for a 100x100 coordinate space with 5 degree angular res.
//   \code
//      vgl_hough_index_2d<T> hindex(0.0, 0.0, 100.0, 100.0, 180.0, 5.0)
//   \endcode
// - Add a line to the index
//   \code
//      ...
//      vgl_line_segment_2d<T> l(p1, p2);
//      hindex.insert(l);
//   \endcode
// - find collinear lines (lines in a 2-d region in Hough space centered on the
//                         parameters defined by line l)
//   \code
//      ...
//      std::vector<vgl_line_segment_2d<T> > lines;
//      hindex.lines_in_interval(lines, l, 1.0, 5.0);//dr = 1.0, dtheta = 5.0
//                                                   //i.e. +- 1.0 and +- 5.0
//   \endcode
// - find lines at a particular orientation
//   \code
//      hindex.parallel_lines(lines, 45.0, 5.0); //Lines parallel to 45 deg.
//                                               //+- 5 deg.
//   \endcode
//
// \author J.L. Mundy December 1997
// \date   ported to VXL April 11, 2003
// \verbatim
//  Modifications
//   10-sep-2004 Peter Vanroose Added copy ctor with explicit vbl_ref_count init
//   May 5, 2018 J.L. Mundy moved to vgl/algo
// \endverbatim
//-----------------------------------------------------------------------------
#include <iostream>
#include <vector>
#include <vcl_compiler.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/vgl_box_2d.h>

template <class T>
class vgl_hough_index_2d 
{
  // PUBLIC SIZE_TERFACE----------------------------------------------------------

 public:

  // Constructors/initializers/Destructors-------------------------------------

  vgl_hough_index_2d(const size_t r_dimension, const size_t theta_dimension);
  vgl_hough_index_2d(const T x0, const T y0,
                     const T xsize, const T ysize,
                     const T angle_range = T(180),
                     const T angle_increment = T(5));

  vgl_hough_index_2d(vgl_box_2d<T> const & box,
                     const T angle_range = T(180),
                     const T angle_increment= T(5.0));

  vgl_hough_index_2d(vgl_hough_index_2d const& i)
    : xo_(i.xo_), yo_(i.yo_),
      xsize_(i.xsize_), ysize_(i.ysize_),
      angle_range_(i.angle_range_), angle_increment_(i.angle_increment_),
      r_dim_(i.r_dim_), th_dim_(i.th_dim_), index_(i.index_) {}
  
  ~vgl_hough_index_2d(){}

  // Data Access---------------------------------------------------------------

  T xsize() const {return xsize_;}
  T ysize() const {return ysize_;}

  size_t get_r_dimension() const {return r_dim_;}
  size_t get_theta_dimension() const {return th_dim_;}

  //: Get the vgl_hough_index_2d array location of a line segment
  void array_loc(vgl_line_segment_2d<T> const& line, T& r, T& theta);
  void array_loc(vgl_line_segment_2d<T> const& line, size_t& r, size_t& theta);

  //: r Location for a translated line position
  size_t trans_loc(const size_t transx, const size_t transy,
                const size_t ry, const size_t theta);

  //: Get line count at a particular location in vgl_hough_index_2d space
  size_t count(const size_t r, const size_t theta);

  //: Insert a new line size_to the index
  bool insert(vgl_line_segment_2d<T> const& line);

  //: Insert a unique new line size_to the index
  bool insert_new(vgl_line_segment_2d<T> const& line);

  //: find if a line is in the index
  bool find(vgl_line_segment_2d<T> const& line);

  //: find a line at a specific r and theta
  bool find(T r, T theta, vgl_line_segment_2d<T>& line);

  //: remove a line
  bool remove(vgl_line_segment_2d<T> const& line);

  //: Lines in a line index bin at r and theta bin indices.
  void lines_at_index(const size_t r, const size_t theta,
                      std::vector<vgl_line_segment_2d<T> >& lines);

  std::vector<vgl_line_segment_2d<T> > lines_at_index(const size_t r,
                                                    const size_t theta);

  //: Lines in a tolerance box around the r and theta of a given line.
  // r is in distance units and theta is in degrees.
  void lines_in_interval(vgl_line_segment_2d<T> const& l,
                         const T r_dist, const T theta_dist,
                         std::vector<vgl_line_segment_2d<T> > & lines);

  std::vector<vgl_line_segment_2d<T> >
    lines_in_interval(vgl_line_segment_2d<T> const & l,
                      const T r_dist,
                      const T theta_dist);

  //:Lines parallel to a given angle in degrees
  void parallel_lines(const T angle,
                      const T angle_dist,
                      std::vector<vgl_line_segment_2d<T> >& lines);

  std::vector<vgl_line_segment_2d<T> > parallel_lines(const T angle,
                                               const T angle_dist);

  //: Lines at an angle to a given line (angle is in degrees)
  void lines_at_angle(vgl_line_segment_2d<T> const &l,
                      const T angle, const T angle_dist,
                      std::vector<vgl_line_segment_2d<T> >& lines);

  std::vector<vgl_line_segment_2d<T> >
    lines_at_angle(vgl_line_segment_2d<T> const &l,
                   const T angle, const T angle_dist);

  //: Lines parallel to a given line with angle_dist in degrees
  void parallel_lines(vgl_line_segment_2d<T> const &l,
                      const T angle_dist,
                      std::vector<vgl_line_segment_2d<T> >& lines);

  std::vector<vgl_line_segment_2d<T> >
    parallel_lines(vgl_line_segment_2d<T> const &l,
                   const T angle_dist);

  //: Angle histogram - projection of hough space onto theta axis
  std::vector<size_t> angle_histogram();

  //: Dominant line directions found by non-maximum suppression 
  // that is peaks with count less than thresh are also suppressed
  size_t dominant_directions(const size_t thresh, const T angle_tol,
                             std::vector<size_t>& dirs);

  //: Dominant parallel line groups
  size_t dominant_line_groups(const size_t thresh, const T angle_tol,
                           std::vector<std::vector<vgl_line_segment_2d<T> > >& groups);

  //: hough line coordinates for dominant parallel groups
  size_t dominant_group_hough_coords(const size_t thresh, const T angle_tol,std::vector<std::vector<std::pair<T, T> > >& group_h_coords);

  // index satistics
  size_t max_count() const;
  size_t min_count() const;
  T average_count() const;
  //: all lines with cell count greater and equal to min_count
  void lines_with_cells_ge_count(const size_t min_count, std::vector<vgl_line_segment_2d<T> >& lines) const;

  // Data Control--------------------------------------------------------------

  void clear_index();

  // INTERNALS-----------------------------------------------------------------
  static T tangent_angle(vgl_line_segment_2d<T> const& line);
  std::vector<vgl_line_segment_2d<T> > lines() const{
    return lines_;
  }
 protected:
  //internal functions
  void init(const size_t r_dimension, const size_t theta_dimension);
  std::vector<size_t> non_maximum_suppress(const size_t radius,
                                           std::vector<size_t> const & bins);
  
  // Data Members--------------------------------------------------------------

 private:

  T xo_; //!< X Origin of the Cartesian Space
  T yo_; //!< Y Origin of the Cartesian Space

  T xsize_; //!< Dimensions of the Cartesian space
  T ysize_;

  T angle_range_; //!< Granularity of the line index
  T angle_increment_;

  size_t   r_dim_;  //!< The dimensions of the index space
  size_t   th_dim_;

  //: The array of lines. 
  //  This array index is stored in the hough array
  std::vector<vgl_line_segment_2d<T> > lines_;

  //: The hough space, a 2d array of line index values
  //     r          theta         indices
  std::vector<std::vector<std::vector<size_t> > > index_;
};

#endif
