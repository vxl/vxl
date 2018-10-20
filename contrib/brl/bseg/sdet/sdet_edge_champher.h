#ifndef sdet_edge_champher_h_
#define sdet_edge_champher_h_

//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief computes a distance transform for edge maps
//
// Uses the 3-4 chamfer distance transform on an array of edge pointers
// to determine the distance from a point to the nearest edge.  The
// edge pointer at a given location can be accessed.
//
// \verbatim
// Modifications
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
    // xsize_ = number of columns; ysize_ = number of rows
    if ( (i>=0) && (i<xsize_) && (j>=0) && (j<ysize_) )
        return (float) distance_[j][i];
    else
        return vnl_numeric_traits<float>::maxval;
    }

  inline vtol_edge_2d_sptr image_edge(int x, int y) {
    int i = x;
    int j = y;
    // xsize_ = number of columns; ysize_ = number of rows
    if ( (i>=0) && (i<xsize_) && (j>=0) && (j<ysize_) )
        return edges_[j][i];
    else
        return nullptr;
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
  int xsize_,ysize_;

  // The distance image
  vbl_array_2d<unsigned char> distance_;
  // Pointers to the nearest Edge for each pixel;
  vbl_array_2d<vtol_edge_2d_sptr> edges_;

  // The index of the digital curve element at a given pixel
  int **index_;
};

#endif
