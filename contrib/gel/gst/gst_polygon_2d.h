#ifndef gst_polygon_2d_h_
#define gst_polygon_2d_h_
#ifdef __GNUC__
#pragma interface "gst_polygon_2d"
#endif
/*
  crossge@crd.ge.com
*/

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_vector.h>
#include <vbl/vbl_ref_count.h>
#include <gst/gst_edge_2d_ref.h>

class gst_polygon_2d : public vbl_ref_count
{
public:

  // constructors and desctructors
  gst_polygon_2d() {}
  ~gst_polygon_2d() {}

  // getters and setters
  int size() const { return edges_.size(); }
  gst_edge_2d_ref operator[]( const int i) const { return edges_[i]; }
  void add( const gst_edge_2d_ref edge) { edges_.push_back( edge); }

  // check closure of edges
  //   returns false if the edges are not closed or
  //                 if there is more than one possible cycle
  bool check_validity() const;

  // useful computational accessors
  bool inside( const double x, const double y) const;
  bool inside( const gst_vertex_2d_ref v) const;

  // output
  friend ostream &operator<<( ostream &os, gst_polygon_2d &p);

protected:

  // unordered, but oriented, list of edges
  vcl_vector<gst_edge_2d_ref> edges_;

};

ostream &operator<<( ostream &os, gst_polygon_2d &p);

#endif
