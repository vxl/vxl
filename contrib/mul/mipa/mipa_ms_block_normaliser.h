#ifndef mipa_ms_block_normaliser_h_
#define mipa_ms_block_normaliser_h_

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
#include <mipa/mipa_block_normaliser.h>
#include <mipa/mipa_identity_normaliser.h>

//: Independently normalise (non-overlapping) blocks within a region
//(e.g. as in simplified R-HOG without overlap)

class mipa_ms_block_normaliser : public mipa_block_normaliser
{
  //: Number of SIFT-like scales at which the histogram cells are combined
  unsigned nscales_;

  //: Is there an overall histogram (over all cells) included at the end
  bool include_overall_histogram_;

 public:

  mipa_ms_block_normaliser():
      mipa_block_normaliser(),
      nscales_(1), include_overall_histogram_(false) {}

  mipa_ms_block_normaliser(const mipa_block_normaliser& lbn,
                           unsigned nscales,bool include_overall_histogram):
      mipa_block_normaliser(lbn),
      nscales_(nscales), include_overall_histogram_(include_overall_histogram) {}

  //: Apply transform independently to each block (at multi-scales)
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
  //   include_overall_histogram: true
  //   nscales: 2
  // }
  // \endverbatim
  void config_from_stream(
      std::istream &is, const mbl_read_props_type &extra_props) override;

  void set_nscales(unsigned n)
      {nscales_ = n;}

  void set_include_overall_histogram(bool bIn)
      {include_overall_histogram_ = bIn;}

  unsigned  nscales() const {return nscales_;}

  bool include_overall_histogram() const  {return include_overall_histogram_ ;}
};

#endif // mipa_ms_block_normaliser_h_
