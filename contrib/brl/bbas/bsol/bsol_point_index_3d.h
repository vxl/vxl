#ifndef bsol_point_index_3d_h_
#define bsol_point_index_3d_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief A distance index for 3-d vsol points
//
//  Indexes points in a 3-d array to support proximity queries.
//
// \verbatim
//  Modifications
//   Initial version March 1, 2004
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vsol/vsol_box_3d_sptr.h>
#include <vsol/vsol_point_3d_sptr.h>

class bsol_point_index_3d
{
 public:
  bsol_point_index_3d();
  bsol_point_index_3d(int nrows, int ncols, int nslabs,
                      vsol_box_3d_sptr const& bb);
  bsol_point_index_3d(int nrows, int ncols, int nslabs,
                      vcl_vector<vsol_point_3d_sptr> const& points);
  ~bsol_point_index_3d();
  //:accessors
  int ncols(){return ncols_;}
  int nrows(){return nrows_;}
  int nslabs(){return nrows_;}
  double row_spacing(){return row_spacing_;}
  double col_spacing(){return col_spacing_;}
  double slab_spacing(){return slab_spacing_;}
  //: origin of the index space
  void origin(double& x0, double& y0, double& z0);
  //: number of points in a cell at r, c, s
  int n_points(const int row, const int col, const int slab);
  //: number of points in a cell at x, y, z
  int n_points(const double x, const double y, const double z);
  //: total number of points in the index
  int n_points();

  //:the points in an index cell
  vcl_vector<vsol_point_3d_sptr> points(const int row, const int col,
                                        const int slab);
  //:all points in the index
  vcl_vector<vsol_point_3d_sptr> points();

  //: the box corresponding to a volume index cell, r, c, s
  vsol_box_3d_sptr index_cell(const int row, const int col, const int slab);

  //: the box corresponding to a volume index cell, x , y, z
  vsol_box_3d_sptr index_cell(const double x, const double y, const double z);

  //: the box corresponding to the bounds of all points in the index
  vsol_box_3d_sptr point_bounds();

  //:mutators
  bool add_point(vsol_point_3d_sptr const& p);
  bool add_points(vcl_vector<vsol_point_3d_sptr> const& points);

  //:mark as not in index, but point remains. Useful for matching
  bool mark_point(vsol_point_3d_sptr&  p);
  //:clear mark
  bool unmark_point(vsol_point_3d_sptr& p);

  //:Is a point marked
  bool marked(vsol_point_3d_sptr const& p);

  //spatial queries

  //:find a point in the index, based on pointer equivalence
  bool find_point(vsol_point_3d_sptr const& p);

  //:find the points within a specified box
  bool in_box(vsol_box_3d_sptr const& box,
              vcl_vector<vsol_point_3d_sptr>& points);

  //:find the points within a radius of a point, possibly none
  bool in_radius(const double radius, vsol_point_3d_sptr const& p,
                 vcl_vector<vsol_point_3d_sptr>& points);

  //:find the closest point to p within a radius, possibly none
  bool closest_in_radius(const double radius, vsol_point_3d_sptr const& p,
                         vsol_point_3d_sptr& point);

  //:remove all points from the index, bounds remain the same
  void clear();
  //:clear marks
  void clear_marks();
 private:
  //transform to index coords
  bool trans(const double x, const double y, const double z,
             int& row, int& col, int& slab);

  int nrows_;
  int ncols_;
  int nslabs_;
  double row_spacing_;//cell dimensions
  double col_spacing_;
  double slab_spacing_;

  vsol_box_3d_sptr b_box_;//bounding box for the array

  //  row         col       slab     points in cell
  vcl_vector<vcl_vector<vcl_vector<vcl_vector<vsol_point_3d_sptr> > > > point_array_;
};

#endif
