// This is core/vgl/vgl_polygon_scan_iterator.hxx
#ifndef vgl_polygon_scan_iterator_hxx_
#define vgl_polygon_scan_iterator_hxx_
//:
// \file

#include <cstring>
#include <cmath>
#include <iostream>
#include <algorithm>
#include "vgl_polygon_scan_iterator.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// It used to be necessary to add 0.5 to the scanline coordinates
// obtained from a vgl_polygon_scan_iterator. Presumably this had
// something to do with pixels and rendering them, but that issue is
// irrelevant to a polygon_scan_iterator.
//
// I think it is clear what a vgl_polygon_scan_iterator should do: tell
// me which points inside my polygon have integer coordinates.
//
// If you cannot live without a polygon_scan_iterator which offsets
// things by 0.5, make a new class called something like
// \code
//   vgl_polygon_scan_iterator_which_adds_one_half
// \endcode
// and change the value of vgl_polygon_scan_iterator_offset back to 0.5
// for that class.
//
// fsm
//

static const float vgl_polygon_scan_iterator_offset = 0.0f; // was 0.5f;

// find minimum of a and b
#undef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))

// find maximum of a and b
#undef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))

template <class T>
struct compare_vertind
{
  compare_vertind(typename vgl_polygon<T>::sheet_t* chs) : chs_(chs)
  {}

  inline bool operator()(const typename vgl_polygon_scan_iterator<T>::vertind &u,
                         const typename vgl_polygon_scan_iterator<T>::vertind &v) const
  {
    return chs_[u.chainnum][u.vertnum].y() < chs_[v.chainnum][v.vertnum].y();
  }

  typename vgl_polygon<T>::sheet_t* chs_;
};

template <class T>
struct compare_crossedges
{
  inline bool operator()(const typename vgl_polygon_scan_iterator<T>::crossedge &u,
                         const typename vgl_polygon_scan_iterator<T>::crossedge &v) const
  {
    return u.x < v.x;
  }
};

template <class T>
inline static void local_qsort(typename vgl_polygon_scan_iterator<T>::vertind* yverts, int numverts, vgl_polygon<T>& p)
{
  std::sort(yverts, yverts + numverts, compare_vertind<T>(&p[0]));
}

template <class T>
inline static void local_qsort(typename vgl_polygon_scan_iterator<T>::crossedge* crossedges, int numcrossedges)
{
  std::sort(crossedges, crossedges + numcrossedges, compare_crossedges<T>());
}

//===============================================================
// Destructor
//===============================================================
template <class T>
vgl_polygon_scan_iterator<T>::~vgl_polygon_scan_iterator()
{
  delete [] crossedges;
  delete [] yverts;
}

//===============================================================
// Constructor - polygon & boundary flag
//===============================================================
template <class T>
vgl_polygon_scan_iterator<T>::vgl_polygon_scan_iterator(vgl_polygon<T> const& face,
                                                        bool boundaryp):
  poly_(face)
{
  boundp = boundaryp;
  have_window = false;
  init();
}

//===============================================================
// Constructor - polygon, boundary flag and viewing area
//===============================================================
template <class T>
vgl_polygon_scan_iterator<T>::vgl_polygon_scan_iterator(vgl_polygon<T> const& face, bool boundaryp, vgl_box_2d<T> const& window):
  poly_(face)
{
  boundp = boundaryp;
  have_window = true;
  win = window;
  init();
}

//===============================================================
// Init - data structures necessary for the scan line
//    conversion.  These initializations are common to all 3
//    constructors.
//===============================================================
template <class T>
void vgl_polygon_scan_iterator<T>::init()
{
  // count total numverts
  numverts = 0;
  for (unsigned int s = 0; s < poly_.num_sheets(); ++s)
    numverts += int(poly_[s].size());

  unsigned int numchains = poly_.num_sheets();
  // return if no vertices in face
  if ( numverts == 0 ) {
    // Make a call to next() return false.
    y0 = 0;
    y1 = -1;
    crossedges = nullptr;
    yverts = nullptr;
    return;
  }

  // create array for storing edges crossing current scan line
  crossedges = new crossedge[ numverts ];

  // create y-sorted array of vertices
  yverts = new vertind[ numverts ];
  int i = 0;
  for (unsigned int j = 0; j < numchains; j++ )
    for (unsigned int h = 0; h < poly_[ j ].size(); h++ )
    {
      yverts[ i ].chainnum = j;
      yverts[ i ].vertnum = h;
      i++;
    }
  if ( i != numverts )
    std::cout << "Error:  i does not equal numverts!\n";

  // sort vertices by y coordinate
  local_qsort<T>(yverts, numverts, poly_);

  T miny, maxy;   // min and max y coordinate of vertices
  miny = get_y( yverts[ 0 ] );
  maxy = get_y( yverts[ numverts - 1 ] );

  // set y0 and y1 to bottommost and topmost scan lines
  if (have_window)
  {
    if (boundp)
      y0 = (int)MAX(win.min_y(), std::floor( miny - vgl_polygon_scan_iterator_offset));
    else
      y0 = (int)MAX(win.min_y(), std::ceil( miny - vgl_polygon_scan_iterator_offset));

    if (boundp)
      y1 = (int)MIN(win.max_y()-1, std::ceil( maxy - vgl_polygon_scan_iterator_offset));
    else
      y1 = (int)MIN(win.max_y()-1, std::floor( maxy - vgl_polygon_scan_iterator_offset));
  }
  else
  {
    if (boundp)
      y0 = (int)std::floor( miny - vgl_polygon_scan_iterator_offset);
    else
      y0 = (int)std::ceil( miny - vgl_polygon_scan_iterator_offset);

    if (boundp)
      y1 = (int)std::ceil( maxy - vgl_polygon_scan_iterator_offset);
    else
      y1 = (int)std::floor(  maxy - vgl_polygon_scan_iterator_offset);
  }
}

//===============================================================
// Deletes edge (v,get_next_vert(v)) from the crossedge array
//===============================================================
template <class T>
void vgl_polygon_scan_iterator<T>::delete_edge( vertind v )
{
    int j;
    for ( j = 0; j < numcrossedges &&
                 !( crossedges[j].v.chainnum == v.chainnum &&
                    crossedges[j].v.vertnum  == v.vertnum ); ++j )
      /*nothing*/;

    // edge not in cross edge list; happens at win->y0
    if ( j >= numcrossedges ) return;

    numcrossedges--;
    std::memmove(&crossedges[j], &crossedges[j+1],
                (numcrossedges-j)*sizeof( crossedges[0] ));
}

//===============================================================
// Inserts edge (v,get_next_vert(v)) into the crossedge array
//===============================================================
template <class T>
void vgl_polygon_scan_iterator<T>::insert_edge( vertind v )
{
  vertind nextvert;
  T dx;
  Point2 p, q;

  get_next_vert( v, nextvert );
  if ( get_y( v ) < get_y( nextvert ) )
  {
    p = get_pt( v );
    q = get_pt( nextvert );
  }
  else
  {
    p = get_pt( nextvert );
    q = get_pt( v );
  }

  // initialize x position at intersection of edge with scanline y
  crossedges[numcrossedges].dx = dx = (q.x() - p.x()) / (q.y() - p.y());
  crossedges[numcrossedges].x = dx * ( fy + vgl_polygon_scan_iterator_offset - p.y() ) + p.x();
  crossedges[numcrossedges].v = v;
  numcrossedges++;
}

//===============================================================
// Resets the iterator so that when next() is called, it will
//    store the first scan segment
//===============================================================
template <class T>
void vgl_polygon_scan_iterator<T>::reset()
{
  y = y0;               // first interior scan line
  numcrossedges = 0;    // start with empty crossingedges list
  curcrossedge = 0;     // crossedge marking first scan segment
  k = 0;                // yverts[k] is next vertex to process
  xl = 0;               // Arbitrary values
  xr = 0;
  fxl = 0;
  fxr = 0;
}

//===============================================================
// Round the double to the nearest int
//===============================================================
template <class T>
static inline int irnd(T x)
{
  return (int)std::floor(x + 0.5);
}

template <class T>
void vgl_polygon_scan_iterator<T>::get_crossedge_vertices(int * &chainnum, int * &vertnum, int & num_crossedges)
{
  num_crossedges=numcrossedges;
  int current=0;
  chainnum=new int[num_crossedges];
  vertnum=new int[num_crossedges];
  while ( current < numcrossedges )
  {
    chainnum[current] = crossedges[current].v.chainnum;
    vertnum[current] = crossedges[current].v.vertnum;
    current++;
  }
}
//===============================================================
// Moves iterator to the next scan segment.
// Scanline y is at y+vgl_polygon_scan_iterator_offset.
//
//??? Check vertices between previous scanline and current one, if any to simplify.
//??? If pt.y=y+0.5,  pretend it's above invariant: y-0.5 < pt[i].y <= y+.5.
//===============================================================
template <class T>
bool vgl_polygon_scan_iterator<T>::next( )
{
  do {
    // Find next segment on current scan line
    while ( curcrossedge < numcrossedges )
    {
      fxl = crossedges[curcrossedge].x;
      fxr = crossedges[curcrossedge+1].x;
      if (boundp)
        // left end of span with boundary
        xl = (int)std::floor( crossedges[curcrossedge].x - vgl_polygon_scan_iterator_offset);
      else
        // left end of span without boundary
        xl = (int)std::ceil( crossedges[curcrossedge].x - vgl_polygon_scan_iterator_offset);

      if ( have_window && xl < irnd(win.min_x()) )
      {
        fxl = win.min_x();
        xl = irnd(fxl);
      }

      if ( boundp )
        //right end of span with boundary
        xr = (int)std::ceil( crossedges[curcrossedge+1].x - vgl_polygon_scan_iterator_offset);
      else
        // right end of span without boundary
        xr = (int)std::floor( crossedges[curcrossedge+1].x - vgl_polygon_scan_iterator_offset);

      if ( have_window && xr >= irnd(win.max_x()) )
      {
        fxr = win.max_x() -1;
        xr =  irnd(fxr);
      }
#if 0 // TODO: added by Vishal Jain (Brown U) but breaks the tests - please fix!
      if (xr==crossedges[curcrossedge+1].x)
        --xr;
#endif // 0
      // adjust the x coord so that it is the intersection of
      // the edge with the scan line one above current
      crossedges[curcrossedge].x += crossedges[curcrossedge].dx;
      crossedges[curcrossedge+1].x += crossedges[curcrossedge+1].dx;
      curcrossedge+=2;
      if ( xl <= xr )
        return true;
    }

    // All segments on current scan line have been exhausted.  Start
    // processing next scan line.
    vertind curvert, prevvert, nextvert;
    if ( y > y1 )
      return false;
    else
    {
      // Current scan line is not the last one.
      bool not_last = true;

      // If boundary included and processing first or last scan line
      // floating point scan line must be taken as a min/max y coordinate
      // of the polygon. Otherwise these scan lines are not included because
      // of the earlier rounding (ceil/floor).
      if ( boundp ) {
        if ( y == y0 )
          fy = std::floor(get_y( yverts[ 0 ] ));
        else if ( y == y1 ) {
          fy = std::ceil(get_y( yverts[ numverts - 1 ] ));
          not_last = false;
        }
        else
          fy = T(y);
      }
      else
        fy = T(y);

      for (; k<numverts && get_y(yverts[k]) <= fy+vgl_polygon_scan_iterator_offset && not_last; k++)
      {
        curvert = yverts[ k ];

        // insert or delete edges (curvert, nextvert) and (prevvert, curvert)
        // from crossedges list if they cross scanline y
        get_prev_vert( curvert, prevvert );

        if ( get_y( prevvert ) <= fy-vgl_polygon_scan_iterator_offset)  // old edge, remove from active list
          delete_edge( prevvert );
        else if ( get_y( prevvert ) > fy+vgl_polygon_scan_iterator_offset)  // new edge, add to active list
          insert_edge( prevvert );

        get_next_vert( curvert, nextvert );

        if ( get_y( nextvert ) <= fy-vgl_polygon_scan_iterator_offset)  // old edge, remove from active list
          delete_edge( curvert );
        else if ( get_y( nextvert ) > fy+vgl_polygon_scan_iterator_offset)  // new edge, add to active list
          insert_edge( curvert );
      }

      // sort edges crossing scan line by their intersection with scan line
      local_qsort<T>(crossedges, numcrossedges);

      curcrossedge = 0; // Process the next set of horizontal spans
      y++;
    }
  } while ( true );
}

//===============================================================
//: Returns the vertex following v in v's chain.
//  The vertex is returned through the parameter nextvert.
//  I get a syntax error when I tried to return an object of type vertind.
//  Compiler error says the default return type is int???
template <class T>
void vgl_polygon_scan_iterator<T>::get_next_vert( vertind v, vertind & nextvert )
{
        nextvert = v;
        nextvert.vertnum += 1;
        if ( nextvert.vertnum == int(poly_[nextvert.chainnum].size()) )
            nextvert.vertnum = 0; // wrap around to first vertex
}

//: Returns the vertex preceding v in v's chain.
//  The vertex is returned through the parameter prevvert.
//  I get a syntax error when I tried to return an object of type vertind.
//  Compiler error says the default return type is int???
template <class T>
void vgl_polygon_scan_iterator<T>::get_prev_vert( vertind v, vertind & prevvert )
{
        prevvert = v;
        prevvert.vertnum = prevvert.vertnum - 1;
        if ( prevvert.vertnum == -1 )  // wrap around to last vertex
            prevvert.vertnum = int(poly_[prevvert.chainnum].size() - 1);
}

//===============================================================
// For debugging purposes.
//===============================================================
template <class T>
void vgl_polygon_scan_iterator<T>::display_chains()
{
    std::cout << "Number of Chains: " << poly_.num_sheets() << std::endl
             << "Number of Vertices: " << numverts << std::endl;
    for (unsigned int c = 0; c < poly_.num_sheets(); ++c )
    {
        std::cout << "---- Chain # " << c << " ----\n"
                 << "  Length: " << poly_[ c ].size() << std::endl;
        for (unsigned int v = 0; v < poly_[ c ].size(); v++ )
        {
            std::cout << "  [ " << poly_[ c ][ v ].x()
                     << ' ' << poly_[ c ][ v ].y() << " ]\n";
        }
    }
    std::cout << std::flush;
}

//===============================================================
// For debugging purposes.
//===============================================================
template <class T>
void vgl_polygon_scan_iterator<T>::display_crossedges()
{
    std::cout << "----- CROSSEDGES -----\n"
             << "numcrossedges: " << numcrossedges << std::endl;
    for (int i = 0; i< numcrossedges; i++ )
    {
        std::cout << "x = " << crossedges[i].x << '\n'
                 << "y = " << crossedges[i].dx << '\n'
                 << "v: chainnum=" << crossedges[i].v.chainnum
                 << ", vertnum=" << crossedges[i].v.vertnum << '\n';
    }
    std::cout << "---------------------\n" << std::flush;
}

#undef VGL_POLYGON_SCAN_ITERATOR_INSTANTIATE
#define VGL_POLYGON_SCAN_ITERATOR_INSTANTIATE(T) \
template class vgl_polygon_scan_iterator<T >

#endif // vgl_polygon_scan_iterator_hxx_
