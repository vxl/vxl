#include "vgl_polygon_scan_iterator.h"

#include <vcl/vcl_list.h>
#include <vcl/vcl_cstdio.h>
#include <vcl/vcl_cstring.h>
#include <vcl/vcl_cmath.h>
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_cstdlib.h>

#include <vgl/vgl_box_2d.h>


#ifndef MIN
#define MIN(a,b)	(((a)<(b))?(a):(b))	
#endif

/* find maximum of a and b */
#ifndef MAX
#define MAX(a,b)	(((a)>(b))?(a):(b))
#endif

//
//JLM Dec. 95: Changed name to avoid conflict with defintion in Basics/types.h
typedef int (* Callback2)(void const*, void const*);

// file global used by compare functions of qsort
static vgl_polygon::sheet_t* chs;

// comparison routines for qsort 
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

void vgl_polygon_scan_iterator::vertind::display( char const* str)
{  
  cout << str << " chainnum = " << chainnum << "   vertnum = " << vertnum << endl << flush;
}


//---------------------------------------------------------------------
//  
//
vgl_polygon_scan_iterator::~vgl_polygon_scan_iterator()
{
  delete [] crossedges;
  delete [] yverts;
}

vgl_polygon_scan_iterator::vgl_polygon_scan_iterator(vgl_polygon const& face, 
						     bool boundaryp VCL_DEFAULT_VALUE(true)):
  poly_(face)
{
  boundp = boundaryp;
  have_window = false;
  init();
}

vgl_polygon_scan_iterator::vgl_polygon_scan_iterator(vgl_polygon const& face, bool boundaryp, vgl_box_2d<float> const& window):
  poly_(face)
{
  boundp = boundaryp;
  have_window = true;
  win = window;
  init();
}

//--------------------------------------------------------------------
// -- Initializes data structures necessary for the scan line
//    conversion.  These initializations are common to all 3
//    constructors.
//
void vgl_polygon_scan_iterator::init()
{
  // count total numverts
  numverts = 0;
  for(int s = 0; s < poly_.num_sheets(); ++s)
    numverts += poly_[s].size();

  int numchains = poly_.num_sheets();
  // return if no vertices in face
  if ( numverts == 0 ) return;  

  // create array for storing edges crossing current scan line
  crossedges = new crossedge[ numverts ];

  // create y-sorted array of vertices
  yverts = new vertind[ numverts ];
  int i = 0, j, h;
  for ( j = 0; j < numchains; j++ )
    for ( h = 0; h < poly_[ j ].size(); h++ )
      {
	yverts[ i ].chainnum = j;
	yverts[ i ].vertnum = h;
	i++;
      }
  if ( i != numverts ) 
    cout << "Error:  i does not equal numverts!" << endl << flush;
  
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
 	y0 = (int)MAX(win.get_min_y(), floor( miny - .5));
       else
 	y0 = (int)MAX(win.get_min_y(), ceil( miny - .5));

       if(boundp)
 	y1 = (int)MIN(win.get_max_y()-1, ceil( maxy - .5));
       else
 	y1 = (int)MIN(win.get_max_y()-1, floor( maxy - .5));
     }
   else
     {
       if(boundp)
 	y0 = (int)floor( miny - .5);
       else
       	y0 = (int)ceil( miny - .5);

       if(boundp)
 	y1 = (int)ceil( maxy - .5);
       else
	y1 = (int)floor(  maxy - .5);
    }
}

//--------------------------------------------------------------------
// -- Deletes edge (v,get_next_vert(v)) from the crossedge array
//
void vgl_polygon_scan_iterator::delete_edge( vertind v )
{
    int j;
    for ( j = 0; ( j < numcrossedges ) && 
                 ( !( ( crossedges[j].v.chainnum == v.chainnum ) &&
                      ( crossedges[j].v.vertnum == v.vertnum ) )); j++ );

    // edge not in cross edge list; happens at win->y0
    if ( j >= numcrossedges ) return;	

    numcrossedges--;
    memcpy( &crossedges[j], &crossedges[j+1], 
            (numcrossedges-j)*sizeof( crossedges[0] ));
}

//--------------------------------------------------------------------
// -- Inserts edge (v,get_next_vert(v)) into the crossedge array
//
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
     crossedges[numcrossedges].x = dx * ( fy + 0.5 - p.y() ) + p.x();
     crossedges[numcrossedges].v = v;
     numcrossedges++;
}

//--------------------------------------------------------------------
// -- Resets the iterator so that when next() is called, it will
//    store the first scan segment
//
void vgl_polygon_scan_iterator::reset()
{
  y = y0;               // first interior scan line
  numcrossedges = 0;	// start with empty crossingedges list 
  curcrossedge = 0;     // crossedge marking first scan segment
  k = 0;	        // yverts[k] is next vertex to process
  xl = 0;               // Arbitrary values
  xr = 0;
  fxl = 0;
  fxr = 0;
}

static inline int irnd(double x)
{
  return (int) (x + ((x<0) ? -.5 : .5));
}

//----------------------------------------------------------------------
// -- Moves iterator to the next scan segment.  Scanline y is at 
//    y+.5 in continuous coordinates check vertices between previous 
//    scanline and current one, if any to simplify. If pt.y=y+.5, 
//    pretend it's above invariant: y-.5 < pt[i].y <= y+.5.
//
bool vgl_polygon_scan_iterator::next( )
{
  // Find next segment on current scan line
  if( curcrossedge < numcrossedges )
    {
      fxl = crossedges[curcrossedge].x;
      fxr = crossedges[curcrossedge+1].x;
      if(boundp)
        // left end of span with boundary
	xl = (int)floor( crossedges[curcrossedge].x - .5); 
      else
        // left end of span without boundary
	xl = (int)ceil( crossedges[curcrossedge].x - .5);    

      if( have_window && xl < irnd(win.get_min_x()) ) 
	{
	  fxl = win.get_min_x();
	  xl = irnd(fxl);
	}
      
      if ( boundp )
        //right end of span with boundary
	xr = (int)ceil( crossedges[curcrossedge+1].x - .5);  
      else
        // right end of span without boundary
	xr = (int)floor( crossedges[curcrossedge+1].x - .5); 

      if ( have_window && xr >= irnd(win.get_max_x()) ) 
	{
	  fxr = win.get_max_x() -1;
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
      for (; k<numverts && get_y(yverts[k]) <= (fy+.5); k++) 
      {
	  curvert = yverts[ k ];	
	  
	  // insert or delete edges (curvert, nextvert) and (prevvert, curvert)
	  // from crossedges list if they cross scanline y
          get_prev_vert( curvert, prevvert );
    
	  if ( get_y( prevvert ) <= (fy-.5))  // old edge, remove from active list
	    delete_edge( prevvert );
	  else if ( get_y( prevvert ) > (fy+.5))  // new edge, add to active list
	    insert_edge( prevvert );

          get_next_vert( curvert, nextvert );

	  if ( get_y( nextvert ) <= (fy-.5))  // old edge, remove from active list
	    delete_edge( curvert );
	  else if ( get_y( nextvert ) > (fy+.5))  // new edge, add to active list
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

//----------------------------------------------------------------------
// -- Returns the vertex following v in v's chain.  The vertex
//    is returned through the parameter nextvert.  I get a syntax error
//    when I tried to return an object of type vertind.  Compiler error 
//    says the default return type is int???
//
void vgl_polygon_scan_iterator::get_next_vert( vertind v, vertind & nextvert ) 
{
        nextvert = v;
        nextvert.vertnum = nextvert.vertnum + 1;
        if ( nextvert.vertnum == poly_[nextvert.chainnum].size() )
            nextvert.vertnum = 0; // wrap around to first vertex        
}

//----------------------------------------------------------------------
// -- Returns the vertex preceeding v in v's chain.  The vertex
//    is returned through the parameter prevvert.  I get a syntax error
//    when I tried to return an object of type vertind.  Compiler error 
//    says the default return type is int???
//
void vgl_polygon_scan_iterator::get_prev_vert( vertind v, vertind & prevvert ) 
{
        prevvert = v;
        prevvert.vertnum = prevvert.vertnum - 1;
        if ( prevvert.vertnum == -1 )  // wrap around to last vertex
            prevvert.vertnum = poly_[prevvert.chainnum].size() - 1; 
}

// ---------------------------------------------------------------------------//
// For debugging purposes.
//
void vgl_polygon_scan_iterator::display_chains()
{
    int c, v;
    cout << "Number of Chains: " << poly_.num_sheets() << endl;
    cout << "Number of Vertices: " << numverts << endl;
    for ( c = 0; c < poly_.num_sheets(); c++ )
    {
        cout << "---- Chain # " << c << " ----" << endl;
        cout << "  Length: " << poly_[ c ].size() << endl;
        for ( v = 0; v < poly_[ c ].size(); v++ )
	{
            cout << "  [ " << poly_[ c ][ v ].x() 
                 << " " << poly_[ c ][ v ].y() << " ]" << endl; 
        }
    }
    cout << flush;
}

// ---------------------------------------------------------------------------//
// For debugging purposes.
//
void vgl_polygon_scan_iterator::display_crossedges()
{
    int i;
    cout << "----- CROSSEDGES -----" << endl;
    cout << "numcrossedges: " << numcrossedges << endl;
    for ( i = 0; i< numcrossedges; i++ )
    {
        cout << "x = " << crossedges[i].x
             << "y = " << crossedges[i].dx;
        crossedges[i].v.display( "v: " );
    }
    cout << "---------------------" << endl;
    cout << flush;
}

