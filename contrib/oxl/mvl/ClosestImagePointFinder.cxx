// This is oxl/mvl/ClosestImagePointFinder.cxx
//:
//  \file

#include <vector>
#include <map>
#include <iostream>
#include <functional>
#include <utility>
#include "ClosestImagePointFinder.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vnl/vnl_math.h>

#include <mvl/HomgInterestPointSet.h>

class vcl_multimap_double_int : public std::multimap<double, int, std::less <double> >
{
  typedef std::multimap<double, int, std::less <double> > base;
 public:
  iterator insert(double key, int value);
  void clear();
};

vcl_multimap_double_int::iterator vcl_multimap_double_int::insert(double key, int value)
{
  //std::cerr << " ins \t" << value << '\t' << key << '\n';
  return base::insert(std::pair<const double, int>(key, value));
}

void vcl_multimap_double_int::clear() { base::erase(begin(), end()); }


//: Initialize to allow fast lookups of corners in the given set.
ClosestImagePointFinder::ClosestImagePointFinder(const HomgInterestPointSet& corners):
  px_(corners.size()),
  py_(corners.size())
{
  // Make a map of image2 y value to corner2
  y2i_ = new vcl_multimap_double_int;
  for (unsigned i = 0; i < corners.size(); ++i) {
    double x = corners.get_2d(i)[0];
    double y = corners.get_2d(i)[1];
    px_[i] = x;
    py_[i] = y;
    y2i_->insert(y, i);
  }
}

//: Initialize to allow fast lookups of corners in the given set.
ClosestImagePointFinder::ClosestImagePointFinder(std::vector<vgl_homg_point_2d<double> > const& corners)
 : px_(corners.size()), py_(corners.size())
{
  y2i_ = new vcl_multimap_double_int;
  for (unsigned i = 0; i < corners.size(); ++i)
  {
    px_[i] = corners[i].x();
    py_[i] = corners[i].y();
    y2i_->insert(py_[i], i);
  }
}

ClosestImagePointFinder::~ClosestImagePointFinder()
{
  delete y2i_;
}

void ClosestImagePointFinder::get_all_within_search_region(double cx, double cy, double w, double h, std::vector<int>* out)
{
  get_all_within_search_region(vgl_box_2d<double>(cx - w, cx + w, cy - h, cy + h), out);
}

void ClosestImagePointFinder::get_all_within_search_region(vgl_box_2d<double> const& disparity_bounds, std::vector<int>* out)
{
  // Look at `point2's between y0 and y1
  auto potential = y2i_->lower_bound(disparity_bounds.min_y());
  auto end =       y2i_->upper_bound(disparity_bounds.max_y() + 1);
#if 0
  std::cerr << "map:";
  for (vcl_multimap_double_int::iterator p = y2i_->begin(); p != y2i_->end(); ++p)
    std::cerr << ' '<< (*p).second << '[' << (*p).first << ']';
  std::cerr << '\n';
#endif // 0
  out->erase(out->begin(), out->end());
  for (; potential != end; ++potential) {
    int point2_index = (*potential).second;
    if (disparity_bounds.contains(px_[point2_index], py_[point2_index]))
      out->push_back(point2_index);
  }
}

//: Returns number that were within range.
int ClosestImagePointFinder::get_closest_within_region(double cx, double cy, double w, double h, int* out, double mindist_sq)
{
  // setup_checklist_disparity
  vgl_box_2d<double> disparity_bounds(cx - w, cx + w, cy - h, cy + h);

  // Look at `point2's between y0 and y1
  auto potential = y2i_->lower_bound(disparity_bounds.min_y());
  auto end =       y2i_->upper_bound(disparity_bounds.max_y() + 1);

  double orig_mindist_sq = mindist_sq;

  last_index_ = -1;
  int inrange = 0;
  for (; potential != end; ++potential) {
    int point2_index = (*potential).second;
    double x = px_[point2_index];
    double y = py_[point2_index];
    if (disparity_bounds.contains(x, y)) {
      double dx = x - cx;
      double dy = y - cy;
      double d2 = dx*dx + dy*dy;
      if (d2 < orig_mindist_sq)
        ++inrange;
      if (d2 < mindist_sq) {
        mindist_sq = d2;
        last_index_ = point2_index;
      }
    }
  }
  last_d2_ = mindist_sq;
  last_inrange_ = inrange;
  if (out)
    *out = last_index_;

  return inrange;
}

int ClosestImagePointFinder::get_closest_within_region(double cx, double cy, double w, double h, int* out)
{
  double d = std::max(w,h);
  return get_closest_within_region(cx, cy, w, h, out, d*d);
}

//: Returns number that were within range.
int ClosestImagePointFinder::get_closest_within_distance(double cx, double cy, double r, int* out)
{
  return get_closest_within_region(cx, cy, r*2, r*2, out, r*r);
}
