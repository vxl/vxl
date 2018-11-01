// This is oxl/mvl/ClosestImagePointFinder.h
#ifndef ClosestImagePointFinder_h_
#define ClosestImagePointFinder_h_
//:
// \file
// \brief HomgInterestPointSet closest points
//
//    ClosestImagePointFinder allows fast access to closest-point
//    operations on a HomgInterestPointSet.
//
// \author  Andrew W. Fitzgibbon, Oxford RRG, 21 Jan 97
//
// \verbatim
//  Modifications
//   22 Jun 2003 - Peter Vanroose - added vgl_homg_point_2d interface
// \endverbatim
//-----------------------------------------------------------------------------

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>
#include <vgl/vgl_fwd.h>

class HomgInterestPointSet;
class vcl_multimap_double_int;
class HomgPoint2D;

class ClosestImagePointFinder
{
 public:
  // Constructors/Destructors--------------------------------------------------

  ClosestImagePointFinder(const HomgInterestPointSet& corners);
  ClosestImagePointFinder(std::vector<vgl_homg_point_2d<double> > const& corners);
  ClosestImagePointFinder(const std::vector<HomgPoint2D>& corners);
  ~ClosestImagePointFinder();

  // ClosestImagePointFinder(const ClosestImagePointFinder& that); - use default
  // ClosestImagePointFinder& operator=(const ClosestImagePointFinder& that); - use default

  // Operations----------------------------------------------------------------
  void get_all_within_search_region(double cx, double cy, double w, double h, std::vector<int>* out_indices);
  void get_all_within_search_region(vgl_box_2d<double> const& region, std::vector<int>* out_indices);

  int get_closest_within_region(double cx, double cy, double w, double h, int* out_index = nullptr);
  int get_closest_within_distance(double cx, double cy, double r, int* out_index = nullptr);

  // Data Access---------------------------------------------------------------
  double get_last_squared_distance() const { return last_d2_; }
  int get_last_num_candidates() const { return last_inrange_; }
  int get_last_match_index() const { return last_index_; }
  double get_last_x() const { return px_[last_index_]; }
  double get_last_y() const { return py_[last_index_]; }

 protected:
  // Data Members--------------------------------------------------------------

  int get_closest_within_region(double cx, double cy, double w, double h, int* out_index, double mindist_sq);

  vnl_vector<double> px_;
  vnl_vector<double> py_;
  vcl_multimap_double_int* y2i_;
  double last_d2_;
  int last_inrange_;
  int last_index_;
};

#endif // ClosestImagePointFinder_h_
