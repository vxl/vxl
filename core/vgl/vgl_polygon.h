// This is core/vgl/vgl_polygon.h
#ifndef vgl_polygon_h_
#define vgl_polygon_h_
//:
// \file
// \author awf@robots.ox.ac.uk
// \date   02 Apr 00
//
// \verbatim
//  Modifications
//   Binary IO added and documentation tidied up NPC, 20/03/01
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
//   Nov.2003 - Peter Vanroose - made vgl_polygon a templated class and added lost of documentation
//   Nov.2003 - Peter Vanroose - added constructor (to replace new_polygon from test_driver)
// \endverbatim

#include <vcl_iosfwd.h>
#include <vgl/vgl_point_2d.h> // needed for vcl_vector instantiations
#include <vcl_vector.h>

//: Store a polygon.
// May have holes or multiple sections.  The polygon is stored as a list
// of "sheets", each sheet is a list of 2d points.
// Iterate through all points using
//
// for (int s = 0; s < polygon.num_sheets(); ++s)
//   for (int p = 0; p < polygon[s].size(); ++p)
//     do_something(polygon[s][p].x(), polygon[s][p].y());
//
template <class T>
class vgl_polygon
{
 public:
  typedef vgl_point_2d<T> point_t;

  typedef vcl_vector<point_t> sheet_t;

  // Constructors/Destructor---------------------------------------------------

  //: Default constructor - constructs an empty polygon with no sheets
  vgl_polygon() {}

  //: Construct an empty polygon, setting the number of (empty) sheets
  explicit vgl_polygon(int num_sheets) : sheets_(num_sheets) {}

  //: Construct a single-sheet polygon from a list of n points.
  //  More sheets can be added later with the add_contour method.
  vgl_polygon(point_t const p[], int n);

  //: Construct a single-sheet polygon from a list of n points.
  //  More sheets can be added later with the add_contour method.
  vgl_polygon(T const* x, T const* y, int n);

  //: Construct a single-sheet polygon from a list of n points, given in (x,y) pairs.
  //  The x_y array should thus be of size 2*n !
  //  More sheets can be added later with the add_contour method.
  vgl_polygon(T const x_y[], int n);

  //: Construct a single-sheet polygon from a sheet, i.e., a vector of 2D points.
  explicit vgl_polygon(sheet_t const& points) : sheets_(1,points) {}

  //: Construct by specifying all of its sheets
  explicit vgl_polygon(vcl_vector<sheet_t> const& sheets) : sheets_(sheets) {}

  // Copy constructor
  vgl_polygon(vgl_polygon const& a) : sheets_(a.sheets_) {}

  // Destructor
  ~vgl_polygon() {}

  //: Returns true if \a p(x,y) is inside the polygon, else false
  bool contains(point_t const& p) { return contains(p.x(),p.y()); }

  bool contains(T x, T y) const;

  // creation

  //: Add a single sheet to this polygon, specified by the given list of n points.
  // This increments the number of sheets by one.
  void add_contour(point_t const p[], int n);

  //: Add a single sheet to this polygon, specified by the given list of n points.
  // This increments the number of sheets by one.
  void add_contour(T const* x, T const* y, int n);

  //: Add a single sheet to this polygon, specified by the given list of n (x,y) pairs.
  //  The x_y array should thus be of size 2*n !
  // This increments the number of sheets by one.
  void add_contour(T const x_y[], int n);

  //: Set the number of sheets to zero, so the polygon becomes empty
  void clear() { sheets_.resize(0); }

  //: Add a new (empty) sheet to the polygon.
  // This increments the number of sheets by one.
  void new_sheet() { sheets_.push_back(sheet_t()); }

  //: Add a new point to the last sheet
  void push_back(T x, T y);

  //: Add a new point to the last sheet
  void push_back(point_t const&);

  //: Add a pre-existing sheet to the polygon
  void push_back(sheet_t const& s) { sheets_.push_back(s); }

  inline unsigned int num_sheets() const { return sheets_.size(); }

  inline unsigned int num_vertices() const {
    unsigned int c=0; for (unsigned int i=0;i<num_sheets();++i) c += sheets_[i].size(); return c;
  }

  //: Get the ith sheet
  inline sheet_t& operator[](int i) { return sheets_[i]; }

  //: Get the ith sheet
  inline sheet_t const& operator[](int i) const { return sheets_[i]; }

  //: Pretty print
  vcl_ostream& print(vcl_ostream&) const;

 protected:

  // Data Members--------------------------------------------------------------
  vcl_vector<sheet_t> sheets_;
};

//: A commonly required (single-sheet) polygon representation.
template <class T>
struct vgl_polygon_sheet_as_array
{
  int n;
  T* x;
  T* y;

  //: Automatic constructor from a single-sheet polygon
  vgl_polygon_sheet_as_array(vgl_polygon<T> const& p);

  //: Automatic constructor from a polygon sheet
  vgl_polygon_sheet_as_array(typename vgl_polygon<T>::sheet_t const& p);

  //: Destructor
  ~vgl_polygon_sheet_as_array();
};

// \relates vgl_polygon
template <class T>
vcl_ostream& operator<< (vcl_ostream& os, vgl_polygon<T> const& p) { return p.print(os); }

#define VGL_POLYGON_INSTANTIATE(T) extern "please include vgl/vgl_polygon.txx instead"

#endif // vgl_polygon_h_
