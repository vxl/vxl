#include "sdet_edge_champher.h"
#include <gevd/gevd_pixel.h>
//:
//--------------------------------------------------------------
//
// Constructor and Destructor Functions
//
//--------------------------------------------------------------

//:  Constructor
sdet_edge_champher::sdet_edge_champher(vbl_array_2d<vtol_edge_2d_sptr>& edges)
{
  _xsize = edges.cols();
  _ysize = edges.rows();
  //note the max value of unsigned char is -1
  _distance = vbl_array_2d<unsigned char>(_ysize, _xsize, (unsigned char)-1 );
  _edges = vbl_array_2d<vtol_edge_2d_sptr>(_ysize, _xsize, 0);
  // Extract the edgel data and write to the distance and orientation images
  this->initialize_arrays(edges);
  // Do the Chamfer 3-4 filtering
  this->chamfer_34();
}


//: Destructor
sdet_edge_champher::~sdet_edge_champher()
{
}


//:
//-----------------------------------------------------------------------------
//
//  Copies the edge array and sets the distance at each edge location to 0
//   Note that vbl_array has r, c -> y, x  index order
//-----------------------------------------------------------------------------
void 
sdet_edge_champher::initialize_arrays(vbl_array_2d<vtol_edge_2d_sptr>& edges)
{
  for(int x=0; x<_xsize; x++)
    for(int y=0; y<_ysize; y++)
      {
        _distance[y][x] = 0;
				_edges[y][x] = edges.get(y,x);
	    }
}

//:
//-----------------------------------------------------------------------------
//
// Performs 3-4 Chamfer filtering on the _distance image, and carries with
// it information about the orientations.
// 
//-----------------------------------------------------------------------------
void sdet_edge_champher::chamfer_34()
{
  this->forward_chamfer();
  this->backward_chamfer();
  this->compute_real_distances();
}

//:
//-----------------------------------------------------------------------------
//
// Determines the minimum of five ints.
//
//-----------------------------------------------------------------------------
int sdet_edge_champher::minimum_5(int a, int b, int c, int d, int e)
{
  if( (a<=b) && (a<=c) && (a<=d) && (a<=e) )
    return(1);
  else if( (b<=c) && (b<=d) && (b<=e) )
    return(2);
  else if( (c<=d) && (c<=e) )
    return(3);
  else if( d<=e )
    return(4);
  else
    return(5);
}

//:
//-----------------------------------------------------------------------------
//
// Performs a forward chamfer convolution on the _distance and _edge images
//
//-----------------------------------------------------------------------------
void sdet_edge_champher::forward_chamfer()
{
  int i,j,val;

  for(j=1;j<_xsize-1;j++)
    for(i=1;i<_ysize-1;i++)
	    {
        val = 
          minimum_5(_distance[i-1][j-1]+4,_distance[i-1][j]+3,
                    _distance[i-1][j+1]+4,_distance[i][j-1]+3,
                    _distance[i][j]);
        switch (val)
          {
          case 1:
            _distance[i][j] = _distance[i-1][j-1]+4;
            _edges[i][j] = _edges[i-1][j-1];
            break;
               
          case 2:
            _distance[i][j] = _distance[i-1][j]+3;
            _edges[i][j] = _edges[i-1][j];
            break;
               
          case 3:
            _distance[i][j] = _distance[i-1][j+1]+4;
            _edges[i][j] = _edges[i-1][j+1];
            break;
               
          case 4:
            _distance[i][j] = _distance[i][j-1]+3;
            _edges[i][j] = _edges[i][j-1];
            break;
               
          case 5:
            break;
          }
	    }
}

//:               
//-----------------------------------------------------------------------------
//
// -- Performs a backward chamfer convolution on the _distance and _edge images
//
//-----------------------------------------------------------------------------
void sdet_edge_champher::backward_chamfer()
{
  int i,j,val;

  for(j=_xsize-2;j>0;j--)
    for(i=_ysize-2;i>0;i--)
	    {
        val = minimum_5(_distance[i][j],_distance[i][j+1]+3,_distance[i+1][j-1]+4,
                        _distance[i+1][j]+3,_distance[i+1][j+1]+4 );
        switch (val)
          {
          case 1:
            break;
               
          case 2:
            _distance[i][j] = _distance[i][j+1]+3;
            _edges[i][j] = _edges[i][j+1];
            break;
               
          case 3:
            _distance[i][j] = _distance[i+1][j-1]+4;
            _edges[i][j] = _edges[i+1][j-1];
            break;
               
          case 4:
            _distance[i][j] = _distance[i+1][j]+3;
            _edges[i][j] = _edges[i+1][j];
            break;
               
          case 5:
            _distance[i][j] = _distance[i+1][j+1]+4;
            _edges[i][j] = _edges[i+1][j+1];
            break;
          }
	    }
}
//:
//-----------------------------------------------------------------------------
//
// The Chamfer 3-4 masks compute distance which are three times the actual
// distance. Scale all values by three in the _distance image.
// 
//-----------------------------------------------------------------------------
void sdet_edge_champher::compute_real_distances()
{
  int x,y;
    
  for(x=0;x<_xsize;x++)
    for(y=0;y<_ysize;y++)
	    _distance[y][x] = (unsigned char) _distance[y][x] / 3;
}
