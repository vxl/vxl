// This is core/vgl/vgl_polygon.h
#ifndef vgl_polygon_h_
#define vgl_polygon_h_
//:
// \file
// \author awf@robots.ox.ac.uk
// \date   02 Apr 2000
//
// \verbatim
//  Modifications
//   Binary IO added and documentation tidied up NPC, 20/03/01
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
//   Nov.2003 - Peter Vanroose - made vgl_polygon a templated class and added lost of documentation
//   Nov.2003 - Peter Vanroose - added constructor (to replace new_polygon from test_driver)
//   May.2009 - Matt Leotta - added a function to find self-intersections
// \endverbatim

#include <iosfwd>
#include <utility>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "vgl_point_2d.h" // needed for std::vector instantiations

//: Store a polygon.
// May have holes or multiple sections.  The polygon is stored as a list
// of "sheets", each sheet is a list of 2d points.
// Iterate through all points using
//
// for (unsigned int s = 0; s < polygon.num_sheets(); ++s)
//   for (unsigned int p = 0; p < polygon[s].size(); ++p)
//     do_something(polygon[s][p].x(), polygon[s][p].y());
//
//  Note: area is not defined on the polygon class to keep a clean interface
//  see vgl_area<T>
template <class T>
class vgl_polygon
{
 public:
  typedef vgl_point_2d<T> point_t;

  typedef std::vector<point_t> sheet_t;

  // Constructors/Destructor---------------------------------------------------

  //: Default constructor - constructs an empty polygon with no sheets
  vgl_polygon() = default;

  //: Construct an empty polygon, setting the number of (empty) sheets
  explicit vgl_polygon(unsigned int nr_sheets) : sheets_(nr_sheets) {}

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
  //  Note: n_sheets is only there to distinguish this from the next constructor for VC6
  //  which seems to have a problem.
  explicit vgl_polygon(sheet_t const& points, unsigned n_sheets=1) : sheets_(n_sheets,points) {}

  //: Construct by specifying all of its sheets
  explicit vgl_polygon(std::vector<sheet_t>  sheets) : sheets_(std::move(sheets)) {}

  // Copy constructor
  vgl_polygon(vgl_polygon const& a) : sheets_(a.sheets_) {}

  // Destructor
  ~vgl_polygon() = default;

  //: Returns true if \a p(x,y) is inside the polygon, else false
  bool contains(point_t const& p) const { return contains(p.x(),p.y()); }

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

  inline unsigned int num_sheets() const { return (unsigned int)(sheets_.size()); }

  inline unsigned int num_vertices() const {
    unsigned int c=0;
    for (unsigned int i=0;i<num_sheets();++i) c += (unsigned int)(sheets_[i].size());
    return c;
  }
  //: Get the ith sheet
  inline sheet_t& operator[](int i) { return sheets_[i]; }

  //: Get the ith sheet
  inline sheet_t const& operator[](int i) const { return sheets_[i]; }

  //: Pretty print
  std::ostream& print(std::ostream&) const;

  //: read this polygon from ascii stream
  std::istream& read(std::istream&);
 protected:

  // Data Members--------------------------------------------------------------
  std::vector<sheet_t> sheets_;
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

//: Compute all self-intersections between all edges on all sheets.
// \returns three arrays \a e1, \a e2, and \a ip of equal size.
// Corresponding elements from these arrays describe an intersection.
// e1[k].first is the sheet index containing edge (e1[k].second, e1[k].second+1)
// involved in the k-th intersection.  Similarly, e2[k] indexes the other
// edge involved in the k-th intersection.  The corresponding intersection
// point is returned in ip[k].
template <class T>
void vgl_selfintersections(vgl_polygon<T> const& p,
                           std::vector<std::pair<unsigned,unsigned> >& e1,
                           std::vector<std::pair<unsigned,unsigned> >& e2,
                           std::vector<vgl_point_2d<T> >& ip);

// turn the first sheet into counterclockwise polygon
template <class T>
vgl_polygon<T> vgl_reorient_polygon(vgl_polygon<T> const &p);

template <class T>
bool vgl_polygon_sheet_is_counter_clockwise(std::vector<vgl_point_2d<T> > verts);


// \relatesalso vgl_polygon
template <class T>
std::ostream& operator<< (std::ostream& os, vgl_polygon<T> const& p) { return p.print(os); }

template <class T>
std::istream& operator>> (std::istream& is, vgl_polygon<T>& p) { return p.read(is); }

#define VGL_POLYGON_INSTANTIATE(T) extern "please include vgl/vgl_polygon.hxx instead"

#endif // vgl_polygon_h_
