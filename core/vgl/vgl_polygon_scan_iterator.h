// This is core/vgl/vgl_polygon_scan_iterator.h
#ifndef vgl_polygon_scan_iterator_h
#define vgl_polygon_scan_iterator_h
//:
// \file
// \author Adapted from FillPolygon by J.L. Mundy
//
// \verbatim
//  Modifications
//   Binary IO added and documentation tidied up NPC, 20/03/01
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
//   Nov.2003 - Peter Vanroose - made vgl_polygon_scan_iterator a templated class
//   Apr.2004 - Peter Vanroose - corrected an earlier with_boundary fix in next()
// \endverbatim

#include <vgl/vgl_region_scan_iterator.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_box_2d.h>

//: Fill a polygonal face with interior scan lines
//  This class provides an iterator-style interface to polygon scan
//  conversion.  There are convenient constructors from vgl_polygon, and_
//  lists of floats.  An auxiliary clipping window can be specified by the
//  constructor argument, vgl_box_2d<T> win.
//
// Concave Polygon Scan Conversion
// by Paul Heckbert
// from "Graphics Gems", Academic Press, 1990
//
// Scan convert nvert-sided concave non-simple polygon
// with vertices at  (point[i].x, point[i].y)
// for i in [0..nvert-1] within the window win by
// calling spanproc for each visible span of pixels.
// Polygon can be clockwise or counterclockwise.
// Algorithm does uniform point sampling at pixel centers.
// Inside-outside test done by Jordan's rule: a point is considered inside if
// an emanating ray intersects the polygon an odd number of times.
//
// Note: The span limits, startx and endx,  are closed intervals.
// That is, you can use the endpoints of the span as valid interior points.
// Also, the initial and final y scan lines returned by the iterator
// are interior to the polygon.  The constructor argument, win, is a clipping
// window that is intersected with the polygonal region to determine the actual
// scanned area.
//
// Example usage:
// \code
//  vgl_polygon_scan_iterator<float> psi(mypoints);
//  psi.set_include_boundary(true); // optional flag, default is true
//  for (psi.reset(); psi.next(); ) {
//    int y = psi.scany();
//    for (int x = psi.startx(); x <= psi.endx(); ++x)
//         ....
//  }
// \endcode
template <class T>
class vgl_polygon_scan_iterator : public vgl_region_scan_iterator
{
  int boundp;       //!< boolean indicating if boundary should be included or not
  int xl;           //!< left bound of current span
  T fxl;            //!< left bound of current span (floating point value)
  int xr;           //!< right bound of current span
  T fxr;            //!< right bound of current span (floating point value)
  int k;            //!< current index of vertices ordered by increasing y
  int y0;           //!< bottommost scan line
  int y1;           //!< topmost scan line
  int y;            //!< current scan line
  T fy;             //!< floating point value of current scan line (i.e. T(y))
  int curcrossedge; //!< crossedge marking start of next scan segment
  vgl_box_2d<T> win;//!< clipping window
  bool have_window;

  vgl_polygon<T> poly_; //!< the polygon

 public:
  // Stores coordinates of a 2d point
  typedef typename vgl_polygon<T>::point_t Point2;

  // Constructors/Destructor---------------------------------------------------

  //: Construct with a polygon and bool indicating whether boundary included
  vgl_polygon_scan_iterator(vgl_polygon<T> const& face, bool boundaryp = true);

  //: Construct with a polygon, bool indicating whether boundary included and window (area visible)
  vgl_polygon_scan_iterator(vgl_polygon<T> const& face, bool boundaryp,
                            vgl_box_2d<T> const& window);

  //: Destructor
  ~vgl_polygon_scan_iterator();

  //Functions----------------------------------------------------------

  //: Resets iterator to first segment of first scan line
  void reset();

  //: Moves iterator to next segment
  bool next();

  //: Returns current scan line
  inline int scany() const { return y-1; }

  //: Returns start of current span
  inline int startx() const { return xl; }

  //: Returns end of current span
  inline int endx() const { return xr; }

  //: Returns start of current span (floating point value)
  inline T fstartx() const { return fxl; }

  //: Returns end of current span (floating point value)
  inline T fendx() const { return fxr; }

  //: Returns current scan line (floating point value)
  inline T fscany() const { return fy; }

  //: Vertex index - uniquely identifies a vertex in the array chains
  struct vertind {
    int chainnum; //!< which chain the vertex is part of
    int vertnum;  //!< which vertex in the chain
  };

  //: Describes an edge crossing the current scan line
  struct crossedge {
    T x;       //!< x coord of edge's intersection with current scanline
    T dx;      //!< change in x with respect to y
    vertind v; //!< edge goes from vertex v.vertnum to v.vertnum + 1
  };

// Internals ---------------------------------------------------------------

 private:

  vertind * yverts;       //!< array of all vertices ordered by y coordinate
  crossedge * crossedges; //!< array of edges crossing current scan line
  int numcrossedges;      //!< number of edges currently crossing scan line
  int numverts;           //!< total number of vertices comprising face

  // Returns x coord of vertex v
  inline T get_x(vertind v) const { return poly_[v.chainnum][v.vertnum].x(); }

  // Returns y coord of vertex v
  inline T get_y(vertind v) const { return poly_[v.chainnum][v.vertnum].y(); }

  // Returns vertex v
  inline Point2 get_pt( vertind v ) { return poly_[v.chainnum][v.vertnum]; }

  // assumes poly_, win, have_window, boundp are set
  void init();

  // Deletes edge (v,get_next_vert(v)) from crossedges array
  void delete_edge( vertind v );

  // Inserts edge (v,get_next_vert(v)) into crossedges array
  void insert_edge( vertind v );

  // Returns next vertex on chain
  void get_next_vert( vertind v, vertind & next );

  // Returns prev vertex on chain
  void get_prev_vert( vertind v, vertind & prev );

  // For debugging purposes
  void display_chains();
  void display_crossedges();
};

#define VGL_POLYGON_SCAN_ITERATOR_INSTANTIATE(T) extern "please include <vgl/vgl_polygon_scan_iterator.txx> instead"

#endif // vgl_polygon_scan_iterator_h
