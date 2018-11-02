#ifndef mipa_block_normaliser_h_
#define mipa_block_normaliser_h_

//:
// \file
// \author Martin Roberts
// \brief Class to independently normalise sub-blocks with a region

#include <string>
#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_fwd.h>
#include <mbl/mbl_cloneable_ptr.h>
#include <mipa/mipa_vector_normaliser.h>
#include <mipa/mipa_identity_normaliser.h>

//: Independently normalise (non-overlapping) blocks within a region
//  (e.g. as in simplified R-HOG without overlap)

class mipa_block_normaliser : public mipa_vector_normaliser
{
 private:
  //: The actual normaliser.
  mbl_cloneable_nzptr<mipa_vector_normaliser> normaliser_;

 protected:
  //: Number of x cells  in region
  unsigned ni_region_;
  //: Number of y cells  in region
  unsigned nj_region_;

  //: Number of cells per block
  unsigned nc_per_block_;

  //: Number of histogram bins per cell
  unsigned nA_;

 public:

  mipa_block_normaliser():
    normaliser_(mipa_identity_normaliser()) {}

  //: The actual normaliser.
  const mipa_vector_normaliser& normaliser() const { return *normaliser_; }

  //: Set the actual normaliser.
  void set_normaliser(const mipa_vector_normaliser& norm) { normaliser_ = norm; }

  //: Apply transform independently to each chunk of v
  void normalise(vnl_vector<double>& v) const override;

  //: Name of the class
  std::string is_a() const override;

  //: Create a copy on the heap and return base class pointer
  mipa_vector_normaliser* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;

  //: Initialise from a text stream.
  // syntax
  // \verbatim
  // {
  //   normaliser: mipa_l2norm_vector_normaliser
  //   ni: 16
  //   nj: 16
  //   nA: 20;
  //   nc_per_block: 4
  // }
  // \endverbatim
  void config_from_stream(
    std::istream &is, const mbl_read_props_type &extra_props) override;

  void set_region(unsigned ni,unsigned nj)
      {ni_region_ = ni; nj_region_ = nj;}
  void set_nbins(unsigned nA)
      {nA_ = nA;}
  void set_nc_per_block(unsigned block_size)
      {nc_per_block_ = block_size;}

  unsigned nA() const {return nA_;}
  unsigned ni_region() const {return ni_region_;}
  unsigned nj_region() const {return nj_region_;}
  unsigned nc_per_block() const {return nc_per_block_;}
};

#endif // mipa_block_normaliser_h_
