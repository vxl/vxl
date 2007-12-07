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
//  Modifications
//   J.L. Mundy Nov.24, 2002 - Adapted from original by Charlie Rothwell, 4/5/95
//   J.L. Mundy Nov.26, 2007 - Adapted again
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vbl/vbl_array_2d.h>
#include <vnl/vnl_numeric_traits.h>
#include <vsol/vsol_digital_curve_2d.h>

class bwm_reg_edge_champher
{
  // PUBLIC INTERFACE----------------------------------------------------------

 public:

  // Constructors/Initializers/Destructors-------------------------------------

  bwm_reg_edge_champher();
  bwm_reg_edge_champher(unsigned col_off, unsigned row_off,
                        unsigned ncols, unsigned nrows,
                        vcl_vector<vsol_digital_curve_2d_sptr> const& edges);
  ~bwm_reg_edge_champher();

  // Data Access---------------------------------------------------------------

  //: the champher distance at the specified image col and row
  // (col, row) are in image coordinates, not champher array coordinates
  inline float distance(unsigned col, unsigned row)
  {
    // ncols_ = number of columns; nrows_ = number of rows
    col-=col_off_; row-=row_off_;
    if ( (col<ncols_-1) && (row<nrows_-1) )
      return (float) distance_[row+1][col+1];
    else {
      vcl_cout << "failed at(" << col+col_off_ << ' ' << row+row_off_ << '\n';
      return vnl_numeric_traits<float>::maxval;
    }
  }

  //: the digital curve that contributed a point at (col, row)
  inline vsol_digital_curve_2d_sptr image_edge(unsigned col, unsigned row)
  {
    // ncols_ = number of columns; nrows_ = number of rows
    col-=col_off_; row-=row_off_;
    if ( (col<ncols_-1) && (row<nrows_-1) )
      return edges_[row+1][col+1];
    else
      return 0;
  }

  //: the index of the vertex of the digital curve inserted at (col, row)
  inline unsigned sample_index(unsigned col, unsigned row)
  {
    // ncols_ = number of columns; nrows_ = number of rows
    col-=col_off_; row-=row_off_;
    if (  (col<ncols_-1) && (row<nrows_-1) )
      return sample_index_[row+1][col+1];
    else
      return 0;
  }

  // Debug Methods------------------------------------------------------------
  void print_distance();

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
  //:the dimensions of the champher array, 2 greater than the image region
  unsigned ncols_,nrows_;

  //:the origin of the champher region in the original image coordinate system
  unsigned col_off_, row_off_;

  //: The distance image
  vbl_array_2d<unsigned char> distance_;

  //: Pointers to the edgel curve at a champher cell (null if no curve)
  vbl_array_2d<vsol_digital_curve_2d_sptr> edges_;

  //: The sample index of the edgel curve at a given cell
  vbl_array_2d<unsigned> sample_index_;
};

#endif
