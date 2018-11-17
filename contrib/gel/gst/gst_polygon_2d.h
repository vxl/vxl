// This is gel/gst/gst_polygon_2d.h
#ifndef gst_polygon_2d_h_
#define gst_polygon_2d_h_
//:
// \file
// \author crossge@crd.ge.com

#include <iostream>
#include <iosfwd>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>
#include "gst_edge_2d_sptr.h"

class gst_polygon_2d : public vbl_ref_count
{
 public:

  // constructors and destructors
  gst_polygon_2d() = default;
  ~gst_polygon_2d() override = default;
  // copy constructor - compiler-provided one sets ref_count to nonzero which is wrong -PVr
  gst_polygon_2d(gst_polygon_2d const& p) : vbl_ref_count(), edges_(p.edges_) {}

  // getters and setters
  unsigned int size() const { return edges_.size(); }
  gst_edge_2d_sptr operator[](unsigned int i) const { return i<size() ? edges_[i] : (gst_edge_2d_sptr)nullptr; }
  void add(gst_edge_2d_sptr edge) { edges_.push_back(edge); }

  //: check closure of edges
  //   returns false if the edges are not closed or
  //                 if there is more than one possible cycle
  bool check_validity() const;

  // useful computational accessors
  bool inside(const double x, const double y) const;
  bool inside(const gst_vertex_2d_sptr& v) const;

  double get_centroid_x() const;
  double get_centroid_y() const;

  double area() const;

  // output
  friend std::ostream &operator<<( std::ostream &os, gst_polygon_2d &p);

 protected:
  //: unordered, but oriented, list of edges
  std::vector<gst_edge_2d_sptr> edges_;
};

std::ostream &operator<<( std::ostream &os, gst_polygon_2d &p);

#endif // gst_polygon_2d_h_
