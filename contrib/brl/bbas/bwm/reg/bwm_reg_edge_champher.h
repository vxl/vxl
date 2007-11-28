#ifndef bwm_reg_edge_champher_h_
#define bwm_reg_edge_champher_h_

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
// Adapted again 
// J.L. Mundy Nov. 26, 2007
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vbl/vbl_array_2d.h>
#include <vnl/vnl_numeric_traits.h>
#include <vsol/vsol_digital_curve_2d.h>
class bwm_reg_edge_champher
{
  // PUBLIC INTERFACE----------------------------------------------------------

 public:

  // Constructors/Initializers/Destructors-------------------------------------

  bwm_reg_edge_champher(unsigned ncols, unsigned nrows,
                        vcl_vector<vsol_digital_curve_2d_sptr> const& edges);
  ~bwm_reg_edge_champher();

  // Data Access---------------------------------------------------------------

  //
  inline float distance(unsigned col, unsigned row) {
    // ncols_ = number of columns; nrows_ = number of rows
    if ( (col<ncols_-1) && (row<nrows_-1) )
        return (float) distance_[row+1][col+1];
    else
        return vnl_numeric_traits<float>::maxval;
    }

  inline vsol_digital_curve_2d_sptr image_edge(unsigned col, unsigned row) {
    // ncols_ = number of columns; nrows_ = number of rows
    if ( (col<ncols_-1) && (row<nrows_-1) )
        return edges_[row+1][col+1];
    else
        return 0;
    }

  inline unsigned sample_index(unsigned col, unsigned row) {
    // ncols_ = number of columns; nrows_ = number of rows
    if (  (col<ncols_-1) && (row<nrows_-1) )
        return sample_index_[row+1][col+1];
    else
        return 0;
    }

  // Data Control--------------------------------------------------------------

  // Utility Methods-----------------------------------------------------------
  void distance_with_edge_masked(unsigned col, unsigned row, vsol_digital_curve_2d_sptr& e);

 protected:
  // INTERNALS-----------------------------------------------------------------
  void initialize_arrays(vcl_vector<vsol_digital_curve_2d_sptr> const& edges);
  void chamfer_34();
  unsigned minimum_5(unsigned,unsigned,unsigned,unsigned,unsigned);
  void forward_chamfer();
  void backward_chamfer();
  void compute_real_distances();


  // Data Members--------------------------------------------------------------

 private:

//the dimensions of the champher array, 2 greater than the actual space
  unsigned ncols_,nrows_; 

  // The distance image
  vbl_array_2d<unsigned char> distance_;

  // Pointers to the nearest edgel curve for each pixel;
  vbl_array_2d<vsol_digital_curve_2d_sptr> edges_;

  // The sample index of the edgel curve at a given cell
  vbl_array_2d<unsigned> sample_index_;

};

#endif
