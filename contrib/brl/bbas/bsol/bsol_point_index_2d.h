#ifndef bsol_point_index_2d_h_
#define bsol_point_index_2d_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief A distance index for 2-d vsol points
//
//  Indexes points in a 2-d array to support proximity and matching queries
//
// \verbatim
//  Modifications
//   Initial version February 24, 2004
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>

class bsol_point_index_2d
{
  bsol_point_index_2d() {} //not meaningful
 public:
  bsol_point_index_2d(int nrows, int ncols, vsol_box_2d_sptr const& bb);
  bsol_point_index_2d(int nrows, int ncols,
                      vcl_vector<vsol_point_2d_sptr> const& points);
  ~bsol_point_index_2d();
  //:accessors
  int ncols() { return ncols_; }
  int nrows() { return nrows_; }
  double row_spacing() { return row_spacing_; }
  double col_spacing() { return col_spacing_; }

  //: origin of the index space
  void origin(double& x0, double& y0);

  //: number of points in a cell at r, c
  int n_points(const int row, const int col);
  //: number of points in a cell at x, y
  int n_points(const double x, const double y);
  //: total number of points in the index
  int n_points();

  //:the points in an index cell
  vcl_vector<vsol_point_2d_sptr> points(const int row, const int col);

  //:all points in the index
  vcl_vector<vsol_point_2d_sptr> points();

  //: the box corresponding to an index cell, r, c
  vsol_box_2d_sptr index_cell(const int row, const int col);

  //: the box corresponding to an index cell, x , y
  vsol_box_2d_sptr index_cell(const double x, const double y);

  //: the box corresponding to the bounds of all points in the index
  vsol_box_2d_sptr point_bounds();

  //:mutators
  bool add_point(vsol_point_2d_sptr const& p);
  bool add_points(vcl_vector<vsol_point_2d_sptr> const& points);

  //:mark as not in index, but point remains. Useful for matching
  bool mark_point(vsol_point_2d_sptr&  p);
  //:clear mark
  bool unmark_point(vsol_point_2d_sptr& p);
  //:Is a point marked
  bool marked(vsol_point_2d_sptr const& p);

  //spatial queries

  //:find a point in the index
  bool find_point(vsol_point_2d_sptr const& p);

  //:find the points within a radius of a point, possibly none
  bool in_radius(const double radius, vsol_point_2d_sptr const& p,
                 vcl_vector<vsol_point_2d_sptr>& points);

  //:find the closest point to p within a radius, possibly none
  bool closest_in_radius(const double radius, vsol_point_2d_sptr const& p,
                         vsol_point_2d_sptr& point);

  //:remove all points from the index, bounds remain the same
  void clear();

  //:clear marks
  void clear_marks();

 private:
  bool trans(const double x, const double y,  //transform to index coords
             int& row, int& col);

  int nrows_;
  int ncols_;
  double row_spacing_;//cell dimensions
  double col_spacing_;

  vsol_box_2d_sptr b_box_;//bounding box for the array

  //  row         col            points in cell
  vcl_vector<vcl_vector<vcl_vector<vsol_point_2d_sptr> > > point_array_;
};

#endif
