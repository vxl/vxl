// This is oxl/mvl/ClosestImagePointFinder.h
#ifndef ClosestImagePointFinder_h_
#define ClosestImagePointFinder_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief HomgInterestPointSet closest points
//
//    ClosestImagePointFinder allows fast access to closest-point
//    operations on a HomgInterestPointSet.
//
// \author  Andrew W. Fitzgibbon, Oxford RRG, 21 Jan 97
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vnl/vnl_vector.h>
#include <vgl/vgl_fwd.h> // for vgl_box_2d

class HomgInterestPointSet;
class vcl_multimap_double_int;
class HomgPoint2D;

class ClosestImagePointFinder
{
 public:
  // Constructors/Destructors--------------------------------------------------

  ClosestImagePointFinder(const HomgInterestPointSet& corners);
  ClosestImagePointFinder(const vcl_vector<HomgPoint2D>& corners);
  ~ClosestImagePointFinder();

  // ClosestImagePointFinder(const ClosestImagePointFinder& that); - use default
  // ClosestImagePointFinder& operator=(const ClosestImagePointFinder& that); - use default

  // Operations----------------------------------------------------------------
  void get_all_within_search_region(double cx, double cy, double w, double h, vcl_vector<int>* out_indices);
  void get_all_within_search_region(vgl_box_2d<double> const& region, vcl_vector<int>* out_indices);

  int get_closest_within_region(double cx, double cy, double w, double h, int* out_index = 0);
  int get_closest_within_distance(double cx, double cy, double r, int* out_index = 0);

  // Data Access---------------------------------------------------------------
  double get_last_squared_distance() const { return _last_d2; }
  int get_last_num_candidates() const { return _last_inrange; }
  int get_last_match_index() const { return _last_index; }
  double get_last_x() const { return px_[_last_index]; }
  double get_last_y() const { return py_[_last_index]; }

 protected:
  // Data Members--------------------------------------------------------------

  int get_closest_within_region(double cx, double cy, double w, double h, int* out_index, double mindist_sq);

  vnl_vector<double> px_;
  vnl_vector<double> py_;
  vcl_multimap_double_int* y2i_;
  double _last_d2;
  int _last_inrange;
  int _last_index;
};

#endif // ClosestImagePointFinder_h_
