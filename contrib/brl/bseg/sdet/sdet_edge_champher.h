#ifndef _sdet_edge_champher_h
#define _sdet_edge_champher_h

//-----------------------------------------------------------------------------
//
// \file
// \author J.L. Mundy
// \brief computes a distance transform for edge maps
//
// Uses the 3-4 chamfer distance transform on an array of edge pointers
// to determine the distance from a point to the nearest edge.  The 
// edge pointer at a given location can be accessed.
//
// \verbatim
// Adapted from the original by Charlie Rothwell - 4/5/95
// J.L. Mundy Nov.24, 2002
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vbl/vbl_array_2d.h>
#include <vnl/vnl_numeric_traits.h>
#include <vtol/vtol_edge_2d_sptr.h>
class sdet_edge_champher 
{

  // PUBLIC INTERFACE----------------------------------------------------------

public:

  // Constructors/Initializers/Destructors-------------------------------------
  
  sdet_edge_champher(vbl_array_2d<vtol_edge_2d_sptr>& edges);
  ~sdet_edge_champher();

  // Data Access---------------------------------------------------------------

  inline float distance(int x, int y) {
	int i = x;
	int j = y;
	if( (i>=0) && (i<_xsize) && (j>=0) && (j<_ysize) )
	    return( (float) _distance[i][j] );
	else
	    return(vnl_numeric_traits<float>::maxval);
    }

  inline vtol_edge_2d_sptr image_edge(int x, int y) {
    int i = x;
    int j = y;
	if( (i>=0) && (i<_xsize) && (j>=0) && (j<_ysize) )
	    return( _edges[i][j] );
	else
	    return(0);
    }

  // Data Control--------------------------------------------------------------

  // Utility Methods-----------------------------------------------------------
  void distance_with_edge_masked(int x, int y, vtol_edge_2d_sptr& e);
  // INTERNALS-----------------------------------------------------------------
  void initialize_arrays(vbl_array_2d<vtol_edge_2d_sptr>& edges);
  void chamfer_34();
  int minimum_5(int,int,int,int,int);
  void forward_chamfer();
  void backward_chamfer();
  void compute_real_distances();
protected:

  // Data Members--------------------------------------------------------------

private:

  // Various pieces of image info
  int _xsize,_ysize;

  // The distance image
  vbl_array_2d<unsigned char> _distance;
  // Pointers to the nearest Edge for each pixel;
  vbl_array_2d<vtol_edge_2d_sptr> _edges;

  // The index of the digital curve element at a given pixel
  int **_index;
};

#endif
