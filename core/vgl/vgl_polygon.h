// This is ./vxl/vgl/vgl_polygon.h
#ifndef vgl_polygon_h_
#define vgl_polygon_h_
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \author awf@robots.ox.ac.uk
// \date   02 Apr 00
//
// \verbatim
// Modifications:
// Binary IO added and documentation tidied up NPC, 20/03/01
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
// \endverbatim

#include <vcl_iosfwd.h>
#include <vgl/vgl_point_2d.h> // needed for vcl_vector instantiations
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_utility.h>

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

  // Constructors/Destructor---------------------------------------------------

  //: Default constructor
  vgl_polygon();

  //: Construct setting number of sheets or regions
  explicit vgl_polygon(int num_sheets);

  //: Construct setting the points in the first sheet.
  // n is the number of points
  vgl_polygon(float const* x, float const* y, int n);

  //: Construct setting the points in the first sheet.
  // n is the number of points
  vgl_polygon(vcl_pair<float, float> const p[], int n);

  //: Construct setting the points in the first sheet.
  // n is the number of points
  vgl_polygon(vcl_pair<double, double> const p[], int n);

  //: Construct setting single sheet of points
  explicit vgl_polygon(sheet_t const& points);

  //: Construct setting a number of sheets
  explicit vgl_polygon(vcl_vector<sheet_t> const& sheets);

  //: Copy constructor
  vgl_polygon(vgl_polygon const&);

  //: Destructor
  ~vgl_polygon();

  //: Returns true if \a x,y is inside the polyon, else false
  bool contains(float x, float y) const;

  // creation

  //: Set the number of sheets to zero
  void clear();

  //: Add a new sheet to the polygon
  void new_sheet();

  //: Add a new point to the current sheet
  void push_back(float x, float y);

  //: Add a new point to the current sheet
  void push_back(point_t const&);

  //: Add a pre-existing sheet to the polygon
  void push_back(sheet_t const&);

  inline int num_sheets() const { return sheets_.size(); }

  //: Get the ith sheet
  inline sheet_t & operator[](int i) { return sheets_[i]; }

  //: Get the ith sheet
  inline sheet_t const& operator[](int i) const { return sheets_[i]; }

  //: Pretty print
  vcl_ostream& print(vcl_ostream&) const;

protected:

  // Data Members--------------------------------------------------------------
  vcl_vector<sheet_t> sheets_;
};

//: A commonly required polygon representation.
struct vgl_polygon_sheet_as_array {
  int n;
  float* x;
  float* y;

  //: Constructor
  vgl_polygon_sheet_as_array(vgl_polygon::sheet_t const& p);

  //: Destructor
  ~vgl_polygon_sheet_as_array();
};

vcl_ostream& operator<< (vcl_ostream& os, vgl_polygon const& p);

#endif // vgl_polygon_h_
