#ifdef __GNUC__
#pragma implementation
#endif

//:
//  \file

#include "ClosestImagePointFinder.h"
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_functional.h>
#include <vcl_utility.h>

#include <vgl/vgl_box_2d.h>
#include <vnl/vnl_math.h>

#include <mvl/HomgInterestPointSet.h>

class vcl_multimap_double_int : public vcl_multimap<double, int, vcl_less <double> > {
  typedef vcl_multimap<double, int, vcl_less <double> > base;
public:
  iterator insert(double key, int value);
  void clear();
};

vcl_multimap_double_int::iterator vcl_multimap_double_int::insert(double key, int value)
{
  //vcl_cerr << " ins \t" << value << "\t" << key << vcl_endl;
  return base::insert(vcl_pair<const double, int>(key, value));
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

ClosestImagePointFinder::~ClosestImagePointFinder()
{
  delete y2i_;
}

void ClosestImagePointFinder::get_all_within_search_region(double cx, double cy, double w, double h, vcl_vector<int>* out)
{
  get_all_within_search_region(vgl_box_2d<double>(cx - w, cx + w, cy - h, cy + h), out);
}

void ClosestImagePointFinder::get_all_within_search_region(vgl_box_2d<double> const& disparity_bounds, vcl_vector<int>* out)
{
  // Look at `point2's between y0 and y1
  vcl_multimap_double_int::iterator potential = y2i_->lower_bound(disparity_bounds.min_y());
  vcl_multimap_double_int::iterator end =       y2i_->upper_bound(disparity_bounds.max_y() + 1);

  //vcl_cerr << "map:";
  //for (vcl_multimap_double_int::iterator p = y2i_->begin(); p != y2i_->end(); ++p)
  //  vcl_cerr << " "<< (*p).second << "[" << (*p).first << "]";
  //vcl_cerr << vcl_endl;

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
  vcl_multimap_double_int::iterator potential = y2i_->lower_bound(disparity_bounds.min_y());
  vcl_multimap_double_int::iterator end =       y2i_->upper_bound(disparity_bounds.max_y() + 1);

  double orig_mindist_sq = mindist_sq;

  _last_index = -1;
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
        _last_index = point2_index;
      }
    }
  }
  _last_d2 = mindist_sq;
  _last_inrange = inrange;
  if (out)
    *out = _last_index;

  return inrange;
}

int ClosestImagePointFinder::get_closest_within_region(double cx, double cy, double w, double h, int* out)
{
  double d = vnl_math_max(w,h);
  return get_closest_within_region(cx, cy, w, h, out, d*d);
}

//: Returns number that were within range.
int ClosestImagePointFinder::get_closest_within_distance(double cx, double cy, double r, int* out)
{
  return get_closest_within_region(cx, cy, r*2, r*2, out, r*r);
}
