#ifndef vgl_polygon_h_
#define vgl_polygon_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vgl_polygon
// .INCLUDE vgl/vgl_polygon.h
// .FILE vgl_polygon.cxx
// .SECTION Author
//    awf@robots.ox.ac.uk
// Created: 02 Apr 00

#include <vcl/vcl_vector.h>
#include <vgl/vgl_point_2d.h>

//: Store a polygon.
// May have holes or multiple sections.  The polygon is stored as a list 
// of "sheets", each sheet is a list of 2d points.
// Iterate through all points using
//
// for(int s = 0; s < polygon.num_sheets(); ++s)
//   for(int p = 0; p < polygon[s].size(); ++p)
//     do_something(polygon[s][p].x(), polygon[s][p].y());
//
class vgl_polygon {
public:
  typedef vgl_point_2d<float> point_t;

  typedef vcl_vector<point_t> sheet_t;

  // Constructors/Destructors--------------------------------------------------
  vgl_polygon();
  vgl_polygon(int num_sheets);
  vgl_polygon(float const* x, float const* y, int n);
  vgl_polygon(vcl_vector<point_t> const& points);
  vgl_polygon(vcl_vector<sheet_t> const& sheets);
  // vgl_polygon(vgl_polygon const&);
  
  bool contains(float x, float y);

  // creation
  void new_sheet();
  void push_back(float x, float y);
  void push_back(point_t const&);
  
  void push_back(sheet_t const&);
  
  int num_sheets() const { return sheets_.size(); }
  
  sheet_t & operator[](int i) { return sheets_[i]; }
  sheet_t const& operator[](int i) const { return sheets_[i]; }
  
protected:
  // Data Members--------------------------------------------------------------
  vcl_vector<sheet_t> sheets_;
  
  // Helpers-------------------------------------------------------------------
};

#endif // vgl_polygon_h_
