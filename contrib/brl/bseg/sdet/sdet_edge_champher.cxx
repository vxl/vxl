//:
// \file
#include "sdet_edge_champher.h"

//--------------------------------------------------------------
//
// Constructor and Destructor Functions
//
//--------------------------------------------------------------

//:  Constructor
sdet_edge_champher::sdet_edge_champher(vbl_array_2d<vtol_edge_2d_sptr>& edges)
{
  xsize_ = edges.cols();
  ysize_ = edges.rows();
  //note the max value of unsigned char is -1
  distance_ = vbl_array_2d<unsigned char>(ysize_, xsize_, (unsigned char)-1 );
  edges_ = vbl_array_2d<vtol_edge_2d_sptr>(ysize_, xsize_, nullptr);
  // Extract the edgel data and write to the distance and orientation images
  this->initialize_arrays(edges);
  // Do the Chamfer 3-4 filtering
  this->chamfer_34();
}


//: Destructor
sdet_edge_champher::~sdet_edge_champher()
= default;


//-----------------------------------------------------------------------------
//
//: Copies the edge array and sets the distance at each edge location to 0
//   Note that vbl_array has r, c -> y, x  index order
//-----------------------------------------------------------------------------
void
sdet_edge_champher::initialize_arrays(vbl_array_2d<vtol_edge_2d_sptr>& edges)
{
  for (int x=0; x<xsize_; x++)
    for (int y=0; y<ysize_; y++)
    {
      if ( edges.get(y,x) !=nullptr ) distance_[y][x] = 0;
      edges_[y][x] = edges.get(y,x);
    }
}

//:
//-----------------------------------------------------------------------------
//
// Performs 3-4 Chamfer filtering on the distance_ image, and carries with
// it information about the orientations.
//
//-----------------------------------------------------------------------------
void sdet_edge_champher::chamfer_34()
{
  this->forward_chamfer();
  this->backward_chamfer();
  this->compute_real_distances();
}

//-----------------------------------------------------------------------------
//
//: Determines the minimum of five ints.
//
//-----------------------------------------------------------------------------
int sdet_edge_champher::minimum_5(int a, int b, int c, int d, int e)
{
  if ( (a<=b) && (a<=c) && (a<=d) && (a<=e) )
    return 1;
  else if ( (b<=c) && (b<=d) && (b<=e) )
    return 2;
  else if ( (c<=d) && (c<=e) )
    return 3;
  else if ( d<=e )
    return 4;
  else
    return 5;
}

//-----------------------------------------------------------------------------
//
//: Performs a forward chamfer convolution on the distance_ and _edge images
//
//-----------------------------------------------------------------------------
void sdet_edge_champher::forward_chamfer()
{
  int i,j,val;

  for (j=1;j<xsize_-1;j++)
    for (i=1;i<ysize_-1;i++)
    {
      val =
        minimum_5(distance_[i-1][j-1]+4,distance_[i-1][j]+3,
                  distance_[i-1][j+1]+4,distance_[i][j-1]+3,
                  distance_[i][j]);
      switch (val)
      {
       case 1:
        distance_[i][j] = distance_[i-1][j-1]+4;
        edges_[i][j] = edges_[i-1][j-1];
        break;

       case 2:
        distance_[i][j] = distance_[i-1][j]+3;
        edges_[i][j] = edges_[i-1][j];
        break;

       case 3:
        distance_[i][j] = distance_[i-1][j+1]+4;
        edges_[i][j] = edges_[i-1][j+1];
        break;

       case 4:
        distance_[i][j] = distance_[i][j-1]+3;
        edges_[i][j] = edges_[i][j-1];
        break;

       default:
        break;
      }
    }
}

//-----------------------------------------------------------------------------
//
//: Performs a backward chamfer convolution on the distance_ and edges_ images
//
//-----------------------------------------------------------------------------
void sdet_edge_champher::backward_chamfer()
{
  int i,j,val;

  for (j=xsize_-2;j>0;j--)
    for (i=ysize_-2;i>0;i--)
    {
      val = minimum_5(distance_[i][j],distance_[i][j+1]+3,distance_[i+1][j-1]+4,
                      distance_[i+1][j]+3,distance_[i+1][j+1]+4 );
      switch (val)
      {
       case 2:
        distance_[i][j] = distance_[i][j+1]+3;
        edges_[i][j] = edges_[i][j+1];
        break;

       case 3:
        distance_[i][j] = distance_[i+1][j-1]+4;
        edges_[i][j] = edges_[i+1][j-1];
        break;

       case 4:
        distance_[i][j] = distance_[i+1][j]+3;
        edges_[i][j] = edges_[i+1][j];
        break;

       case 5:
        distance_[i][j] = distance_[i+1][j+1]+4;
        edges_[i][j] = edges_[i+1][j+1];
        break;

       default:
        break;
      }
    }
}
//:
//-----------------------------------------------------------------------------
//
// The Chamfer 3-4 masks compute distance which are three times the actual
// distance. Scale all values by three in the distance_ image.
//
//-----------------------------------------------------------------------------
void sdet_edge_champher::compute_real_distances()
{
  int x,y;

  for (x=0;x<xsize_;x++)
    for (y=0;y<ysize_;y++)
      distance_[y][x] = (unsigned char) distance_[y][x] / 3;
}
