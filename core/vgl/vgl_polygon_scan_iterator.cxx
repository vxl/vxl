// This is vxl/vgl/vgl_polygon_scan_iterator.cxx

//:
// \file

//--------------------------------------------------------------
//
// Class : vgl_polygon_scan_iterator
//
//--------------------------------------------------------------

#include "vgl_polygon_scan_iterator.h"

#include <vcl_list.h>
#include <vcl_cstdio.h>
#include <vcl_cstring.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>

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
//   vgl_polygon_scan_iterator_which_adds_one_half
// and change the value of fsm_OFFSET back to 0.5 for that class.
//
// fsm@robots.ox.ac.uk
//

//#define fsm_OFFSET 0.5
#define fsm_OFFSET 0.0

// find minimum of a and b
#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

// find maximum of a and b
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

//JLM Dec. 95: Changed name to avoid conflict with defintion in Basics/types.h
typedef int (* Callback2)(void const*, void const*);

// file global used by compare functions of qsort
static vgl_polygon::sheet_t* chs;

//===============================================================
// comparison routines for qsort
//===============================================================
static int compare_vertind(vgl_polygon_scan_iterator::vertind *u,
                           vgl_polygon_scan_iterator::vertind *v)
{
  return ( chs[u->chainnum][u->vertnum].y() <= chs[v->chainnum][v->vertnum].y() ) ? -1 : 1;
}

static int compare_crossedges(vgl_polygon_scan_iterator::crossedge *u,
                              vgl_polygon_scan_iterator::crossedge *v)
{
  return u->x <= v->x ? -1 : 1;
}

//===============================================================
// Print routine for vertind
//===============================================================
void vgl_polygon_scan_iterator::vertind::display( char const* str)
{
  vcl_cout << str << " chainnum = " << chainnum << "   vertnum = " << vertnum << vcl_endl;
}


//===============================================================
// Destructor
//===============================================================
vgl_polygon_scan_iterator::~vgl_polygon_scan_iterator()
{
  delete [] crossedges;
  delete [] yverts;
}

//===============================================================
// Constructor - polygon & boundary flag
//===============================================================
vgl_polygon_scan_iterator::vgl_polygon_scan_iterator(vgl_polygon const& face,
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
vgl_polygon_scan_iterator::vgl_polygon_scan_iterator(vgl_polygon const& face, bool boundaryp, vgl_box_2d<float> const& window):
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
void vgl_polygon_scan_iterator::init()
{
  // count total numverts
  numverts = 0;
  for(int s = 0; s < poly_.num_sheets(); ++s)
    numverts += poly_[s].size();

  int numchains = poly_.num_sheets();
  // return if no vertices in face
  if ( numverts == 0 ) {
    // Make a call to next() return false.
    y0 = 0;
    y1 = -1;
    crossedges = 0;
    yverts = 0;
    return;
  }

  // create array for storing edges crossing current scan line
  crossedges = new crossedge[ numverts ];

  // create y-sorted array of vertices
  yverts = new vertind[ numverts ];
  int i = 0;
  for (int j = 0; j < numchains; j++ )
    for (unsigned int h = 0; h < poly_[ j ].size(); h++ )
      {
        yverts[ i ].chainnum = j;
        yverts[ i ].vertnum = h;
        i++;
      }
  if ( i != numverts )
    vcl_cout << "Error:  i does not equal numverts!" << vcl_endl;

  // sort vertices by y coordinate
  chs = &poly_[0]; // a hack -- but apparently must do it to use qsort
  qsort(yverts, numverts, sizeof(yverts[0]), (Callback2)compare_vertind);

  float miny, maxy;   // min and max y coordinate of vertices
  miny = get_y( yverts[ 0 ] );
  maxy = get_y( yverts[ numverts - 1 ] );

  // set y0 and y1 to bottommost and topmost scan lines
  if(have_window)
  {
      if(boundp)
        y0 = (int)MAX(win.min_y(), vcl_floor( miny - fsm_OFFSET));
      else
        y0 = (int)MAX(win.min_y(), vcl_ceil( miny - fsm_OFFSET));

      if(boundp)
        y1 = (int)MIN(win.max_y()-1, vcl_ceil( maxy - fsm_OFFSET));
      else
        y1 = (int)MIN(win.max_y()-1, vcl_floor( maxy - fsm_OFFSET));
  }
  else
  {
      if(boundp)
        y0 = (int)vcl_floor( miny - fsm_OFFSET);
      else
        y0 = (int)vcl_ceil( miny - fsm_OFFSET);

      if(boundp)
        y1 = (int)vcl_ceil( maxy - fsm_OFFSET);
      else
        y1 = (int)vcl_floor(  maxy - fsm_OFFSET);
  }
}

//===============================================================
// Deletes edge (v,get_next_vert(v)) from the crossedge array
//===============================================================
void vgl_polygon_scan_iterator::delete_edge( vertind v )
{
    int j;
    for ( j = 0; ( j < numcrossedges ) &&
                 ( !( ( crossedges[j].v.chainnum == v.chainnum ) &&
                      ( crossedges[j].v.vertnum == v.vertnum ) )); j++ );

    // edge not in cross edge list; happens at win->y0
    if ( j >= numcrossedges ) return;

    numcrossedges--;
    vcl_memcpy( &crossedges[j], &crossedges[j+1],
            (numcrossedges-j)*sizeof( crossedges[0] ));
}

//===============================================================
// Inserts edge (v,get_next_vert(v)) into the crossedge array
//===============================================================
void vgl_polygon_scan_iterator::insert_edge( vertind v )
{
     vertind nextvert;
     float dx;
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
     crossedges[numcrossedges].x = dx * ( fy + fsm_OFFSET - p.y() ) + p.x();
     crossedges[numcrossedges].v = v;
     numcrossedges++;
}

//===============================================================
// Resets the iterator so that when next() is called, it will
//    store the first scan segment
//===============================================================
void vgl_polygon_scan_iterator::reset()
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
// Round the double to the neaest int
//===============================================================
static inline int irnd(double x)
{
  return (int) vcl_floor(x + 0.5); //(x + ((x<0) ? -0.5 : 0.5));
}

//===============================================================
// Moves iterator to the next scan segment.
// Scanline y is at y+fsm_OFFSET.
//
//??? Check vertices between previous scanline and current one, if any to simplify.
//??? If pt.y=y+0.5,  pretend it's above invariant: y-0.5 < pt[i].y <= y+.5.
//===============================================================
bool vgl_polygon_scan_iterator::next( )
{
  // Find next segment on current scan line
  if( curcrossedge < numcrossedges )
    {
      fxl = crossedges[curcrossedge].x;
      fxr = crossedges[curcrossedge+1].x;
      if(boundp)
        // left end of span with boundary
        xl = (int)vcl_floor( crossedges[curcrossedge].x - fsm_OFFSET);
      else
        // left end of span without boundary
        xl = (int)vcl_ceil( crossedges[curcrossedge].x - fsm_OFFSET);

      if( have_window && xl < irnd(win.min_x()) )
        {
          fxl = win.min_x();
          xl = irnd(fxl);
        }

      if ( boundp )
        //right end of span with boundary
        xr = (int)vcl_ceil( crossedges[curcrossedge+1].x - fsm_OFFSET);
      else
        // right end of span without boundary
        xr = (int)vcl_floor( crossedges[curcrossedge+1].x - fsm_OFFSET);

      if ( have_window && xr >= irnd(win.max_x()) )
        {
          fxr = win.max_x() -1;
          xr =  irnd(fxr);
        }

      // adjust the x coord so that it is the intersection of
      // the edge with the scan line one above current
      crossedges[curcrossedge].x += crossedges[curcrossedge].dx;
      crossedges[curcrossedge+1].x += crossedges[curcrossedge+1].dx;
      curcrossedge+=2;
      if (! ( xl <= xr ) )
        return next();
      else
        return true;
    }

  // All segments on current scan line have been exhausted.  Start
  // processing next scan line.
  vertind curvert, prevvert, nextvert;
  if ( y <= y1 )
    {
      fy = (float)y;
      for (; k<numverts && get_y(yverts[k]) <= (fy+fsm_OFFSET); k++)
      {
          curvert = yverts[ k ];

          // insert or delete edges (curvert, nextvert) and (prevvert, curvert)
          // from crossedges list if they cross scanline y
          get_prev_vert( curvert, prevvert );

          if ( get_y( prevvert ) <= (fy-fsm_OFFSET))  // old edge, remove from active list
            delete_edge( prevvert );
          else if ( get_y( prevvert ) > (fy+fsm_OFFSET))  // new edge, add to active list
            insert_edge( prevvert );

          get_next_vert( curvert, nextvert );

          if ( get_y( nextvert ) <= (fy-fsm_OFFSET))  // old edge, remove from active list
            delete_edge( curvert );
          else if ( get_y( nextvert ) > (fy+fsm_OFFSET))  // new edge, add to active list
            insert_edge( curvert );
      }

      // sort edges crossing scan line by their intersection with scan line
      qsort( crossedges, numcrossedges, sizeof crossedges[0],
             (Callback2)compare_crossedges);

      curcrossedge = 0; // Process the next set of horizontal spans
      y++;
      return next();
    }
  else
    return false;
}

//===============================================================
//: Returns the vertex following v in v's chain.
//  The vertex is returned through the parameter nextvert.
//  I get a syntax error when I tried to return an object of type vertind.
//  Compiler error says the default return type is int???
void vgl_polygon_scan_iterator::get_next_vert( vertind v, vertind & nextvert )
{
        nextvert = v;
        nextvert.vertnum += 1;
        if ( nextvert.vertnum == int(poly_[nextvert.chainnum].size()) )
            nextvert.vertnum = 0; // wrap around to first vertex
}

//: Returns the vertex preceeding v in v's chain.
//  The vertex is returned through the parameter prevvert.
//  I get a syntax error when I tried to return an object of type vertind.
//  Compiler error says the default return type is int???
void vgl_polygon_scan_iterator::get_prev_vert( vertind v, vertind & prevvert )
{
        prevvert = v;
        prevvert.vertnum = prevvert.vertnum - 1;
        if ( prevvert.vertnum == -1 )  // wrap around to last vertex
            prevvert.vertnum = poly_[prevvert.chainnum].size() - 1;
}

//===============================================================
// For debugging purposes.
//===============================================================
void vgl_polygon_scan_iterator::display_chains()
{
    vcl_cout << "Number of Chains: " << poly_.num_sheets() << vcl_endl;
    vcl_cout << "Number of Vertices: " << numverts << vcl_endl;
    for (int c = 0; c < poly_.num_sheets(); c++ )
    {
        vcl_cout << "---- Chain # " << c << " ----" << vcl_endl;
        vcl_cout << "  Length: " << poly_[ c ].size() << vcl_endl;
        for (unsigned int v = 0; v < poly_[ c ].size(); v++ )
        {
            vcl_cout << "  [ " << poly_[ c ][ v ].x()
            << " " << poly_[ c ][ v ].y() << " ]" << vcl_endl;
        }
    }
    vcl_cout << vcl_flush;
}

//===============================================================
// For debugging purposes.
//===============================================================
void vgl_polygon_scan_iterator::display_crossedges()
{
    int i;
    vcl_cout << "----- CROSSEDGES -----" << vcl_endl;
    vcl_cout << "numcrossedges: " << numcrossedges << vcl_endl;
    for ( i = 0; i< numcrossedges; i++ )
    {
        vcl_cout << "x = " << crossedges[i].x
             << "y = " << crossedges[i].dx;
        crossedges[i].v.display( "v: " );
    }
    vcl_cout << "---------------------" << vcl_endl;
    vcl_cout << vcl_flush;
}

