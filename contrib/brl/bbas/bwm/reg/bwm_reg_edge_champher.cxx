#include "bwm_reg_edge_champher.h"
//:
// \file
#include <vcl_cmath.h>
#include <vsol/vsol_point_2d.h>
#include <bsol/bsol_algs.h>
#include "bwm_reg_edge_champher.h"


//--------------------------------------------------------------
//
// Constructor and Destructor Functions
//
//--------------------------------------------------------------

//:  Default constructor
bwm_reg_edge_champher::bwm_reg_edge_champher()
{
  ncols_ = 0;
  nrows_ = 0;
  col_off_ = 0;
  row_off_ = 0;
  distance_ = vbl_array_2d<unsigned char>(0,0,0);
  edges_ = vbl_array_2d<vsol_digital_curve_2d_sptr>(0, 0, 0);
  sample_index_ = vbl_array_2d<unsigned>(0, 0, 0);
}

bwm_reg_edge_champher::
bwm_reg_edge_champher(unsigned col_off, unsigned row_off,
                      unsigned ncols, unsigned nrows,
                      vcl_vector<vsol_digital_curve_2d_sptr> const& edges)
{
  // need a boarder of 1 pixel around the edge of the champher array
  ncols_ = ncols+2;
  nrows_ = nrows+2;

  // the origin of the champher array (starting at 1,1) in image coordinates
  col_off_ = col_off;
  row_off_ = row_off;

  //note the max value of unsigned char is -1
  distance_ = vbl_array_2d<unsigned char>(nrows_, ncols_, (unsigned char)-1 );

  //pointers to the digital curves, initially null
  edges_ = vbl_array_2d<vsol_digital_curve_2d_sptr>(nrows_, ncols_, 0);

  //The index of the curve vertex inserted at a given cell
  sample_index_ = vbl_array_2d<unsigned>(nrows_, ncols_, 0);

  // Extract the vertex data and write to the arrays
  this->initialize_arrays(edges);

  // Do the Chamfer 3-4 filtering which sets distance to the nearest vertex
  // at each cell of the champer array. The active part of the
  // array is (1,1)<->(m-2, n-2).
  this->chamfer_34();
}

//: Destructor
bwm_reg_edge_champher::~bwm_reg_edge_champher()
{
}

//: For debugging purposes
void bwm_reg_edge_champher::print_distance()
{
  for (unsigned r = 1; r<nrows_-2; ++r)
  {
    for (unsigned c =1 ; c<ncols_-2;++c)
    {
      unsigned v = static_cast<unsigned>(distance_[r][c]);
      if (v>0)
        vcl_cout << 1 ;
      else
        vcl_cout << 0 ;
    }
    vcl_cout << '\n';
  }
  vcl_cout << vcl_flush;
}

void bwm_reg_edge_champher::print_full_distance()
{
  for (unsigned r = 1; r<nrows_-2; ++r)
  {
    for (unsigned c =1 ; c<ncols_-2;++c)
    {
      unsigned v = static_cast<unsigned>(distance_[r][c]);
      if (v>0)
        vcl_cout << v ;
      else
        vcl_cout << 0 ;
    }
    vcl_cout << '\n';
  }
  vcl_cout << vcl_flush;
}

//-----------------------------------------------------------------------------
//: Copies the edges into the array and sets the distance at each digital curve vertex location to 0.
//  (note: maybe should include points on the line joining vertices)
//
//-----------------------------------------------------------------------------
void bwm_reg_edge_champher::
initialize_arrays(vcl_vector<vsol_digital_curve_2d_sptr> const& edges)
{
  vcl_vector<vsol_digital_curve_2d_sptr>::const_iterator cit = edges.begin();
  for (; cit != edges.end(); ++cit)
    for (unsigned i = 0; i<(*cit)->size(); ++i)
    {
      vsol_point_2d_sptr p = (*cit)->point(i);
      double c = p->x(), r = p->y();
      c -= col_off_; r -= row_off_;
      if (c<0||r<0) continue;
      if (c>ncols_-2||r>nrows_-2) continue;
      unsigned ic = static_cast<unsigned>(vcl_floor(c)),
        ir = static_cast<unsigned>(vcl_floor(r));
      distance_[ir+1][ic+1] = 0;
      sample_index_[ir+1][ic+1] = i;
      edges_[ir+1][ic+1] = *cit;
    }
}

//-----------------------------------------------------------------------------
//: Performs 3-4 Chamfer filtering on the distance_ image
//
//-----------------------------------------------------------------------------
void bwm_reg_edge_champher::chamfer_34()
{
  this->forward_chamfer();
  this->backward_chamfer();
  this->compute_real_distances();
}

//-----------------------------------------------------------------------------
//: Determines the minimum of five ints.
//
//-----------------------------------------------------------------------------
unsigned bwm_reg_edge_champher::
minimum_5(unsigned a, unsigned b, unsigned c, unsigned d, unsigned e)
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
//: Performs a forward chamfer convolution on the distance_,  edge_ and sample_index_ arrays.
//  That is, distance_[row][col]) is with respect to a particular digital curve
//  vertex and that curve and its index are propagated to the cell at (col, row)
//
//-----------------------------------------------------------------------------
void bwm_reg_edge_champher::forward_chamfer()
{
  unsigned col,row,val;

  for (row=1;row<ncols_-1;row++)
    for (col=1;col<nrows_-1;col++)
    {
      val =
        minimum_5(distance_[col-1][row-1]+4,distance_[col-1][row]+3,
                  distance_[col-1][row+1]+4,distance_[col][row-1]+3,
                  distance_[col][row]);
        switch (val)
          {
          case 1:
            distance_[col][row] = distance_[col-1][row-1]+4;
            edges_[col][row] = edges_[col-1][row-1];
            sample_index_[col][row] = sample_index_[col-1][row-1];
            break;

          case 2:
            distance_[col][row] = distance_[col-1][row]+3;
            edges_[col][row] = edges_[col-1][row];
            sample_index_[col][row] = sample_index_[col-1][row];
            break;

          case 3:
            distance_[col][row] = distance_[col-1][row+1]+4;
            edges_[col][row] = edges_[col-1][row+1];
            sample_index_[col][row] = sample_index_[col-1][row+1];
            break;

          case 4:
            distance_[col][row] = distance_[col][row-1]+3;
            edges_[col][row] = edges_[col][row-1];
            sample_index_[col][row] = sample_index_[col][row-1];
            break;

          case 5:
            break;
          }
    }
}

//-----------------------------------------------------------------------------
//: Performs a backward chamfer propagation on the distance_, edges_ and sample_index_ arrays
//
//-----------------------------------------------------------------------------
void bwm_reg_edge_champher::backward_chamfer()
{
  unsigned col,row,val;

  for (row=ncols_-2;row>0;row--)
    for (col=nrows_-2;col>0;col--)
    {
        val = minimum_5(distance_[col][row], distance_[col][row+1]+3,
                        distance_[col+1][row-1]+4, distance_[col+1][row]+3,
                        distance_[col+1][row+1]+4 );
        switch (val)
          {
          case 1:
            break;

          case 2:
            distance_[col][row] = distance_[col][row+1]+3;
            edges_[col][row] = edges_[col][row+1];
            sample_index_[col][row] = sample_index_[col][row+1];
            break;

          case 3:
            distance_[col][row] = distance_[col+1][row-1]+4;
            edges_[col][row] = edges_[col+1][row-1];
            sample_index_[col][row] = sample_index_[col+1][row-1];
            break;

          case 4:
            distance_[col][row] = distance_[col+1][row]+3;
            edges_[col][row] = edges_[col+1][row];
            sample_index_[col][row] = sample_index_[col+1][row];
            break;

          case 5:
            distance_[col][row] = distance_[col+1][row+1]+4;
            edges_[col][row] = edges_[col+1][row+1];
            sample_index_[col][row] = sample_index_[col+1][row+1];
            break;
          }
    }
}

//-----------------------------------------------------------------------------
//: The Chamfer 3-4 masks compute distance which are three times the actual distance.
// Scale all values by three in the distance_ image.
//
//-----------------------------------------------------------------------------
void bwm_reg_edge_champher::compute_real_distances()
{
  unsigned row, col;

  for (col=1;col<ncols_-1;col++)
    for (row=1;row<nrows_-1;row++)
      distance_[row][col] = (unsigned char) distance_[row][col] / 3;
}


bool bwm_reg_edge_champher::match_tangent(unsigned col, unsigned row,
                                          double dx, double dy,
                                          double angle_tolerance)
{
  //get the curve responsible for the nearest distance
  vsol_digital_curve_2d_sptr i_edge = this->image_edge(col, row);
  //get the sample on the curve responsible
  unsigned i_index = this->sample_index(col, row);
  //compute the indexed curve tangent
  double idx=0, idy=0;
  bsol_algs::tangent(i_edge, i_index, idx, idy);
  //compute the dot product of the tangents
  double dot = idx*dx + idy*dy;
  double i_mag = vcl_sqrt(idx*idx +idy*idy);
  double mag = vcl_sqrt(dx*dx +dy*dy);
  dot /= i_mag; dot /= mag;
  //compute the angle between the tangents
  double angle = vcl_acos(dot);
  //determine match
  if(vcl_fabs(angle)> angle_tolerance)
    return false;
  return true;
}
