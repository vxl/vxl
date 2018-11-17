#pragma once
// This is brl/bbas/volm/desc/volm_desc.h
#ifndef volm_desc_h_
#define volm_desc_h_
//:
// \file
// \brief  A base class to represent of volumetric descriptor which constructs a histogram
//  to represent the scene observed from a hypothesized location
//
// \author Yi Dong
// \date May 29, 2013
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <vector>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class volm_desc;
typedef vbl_smart_ptr<volm_desc> volm_desc_sptr;

class volm_desc : public vbl_ref_count
{
public:
  // Default constructor
  volm_desc() = default;

  //: construct the histogram from a vector
  volm_desc(std::vector<unsigned char> values)
    : h_(values)
  { name_ = "descriptor";  nbins_ = (unsigned)values.size(); }

  // Destructor
  ~volm_desc() override = default;;

  //: return the name of the descriptor
  virtual std::string name() { return "volm_desc base"; }

  //: return the number of bins of the histogram
  unsigned int nbins() { return nbins_; }

  //: Get total area under the histogram = total counts in histogram
  unsigned int get_area();

  //: pretty print (prevent creation of base class by setting = 0)
  virtual void print() const;

  //: general similarity metric for histogram
  virtual float similarity(volm_desc_sptr other);

  //: visualization using bsvg
  void visualize(const std::string& outfile, unsigned char const& y_max = 10) const;

  //: return the value of bin
  unsigned char count(unsigned const& i) const { return h_[i]; }

  //: Accessor to the value at bin i in the histogram
  unsigned char operator[] (unsigned int i) const { return h_[i];}

  void get_char_array(std::vector<unsigned char>& values) const;

  // ===========  binary I/O ================

  //: version
  virtual unsigned version() const { return 1; }

  //: binary IO write
  virtual void b_write(vsl_b_ostream& os);

  //: binary IO read
  virtual void b_read(vsl_b_istream& is);

protected:
  //: name of the descriptor
  std::string name_;

  //: number of bins of the histogram
  unsigned int nbins_;

  //: a simple 1D histogram
  std::vector<unsigned char> h_;
};

#endif  // volm_desc_h_
