// This is brl/bbas/volm/desc/volm_desc.h
#ifndef volm_desc_h_
#define volm_desc_h_
//:
// \file
// \brief  A base class to represent of volumetric descriptor which constructs a histogram
//  to represent the scene observed from a hypothesized location
//
// \author Ozge C. Ozcanli
// \date May 29, 2013
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <bsta_histogram.h>
#include <bsta_histogram.txx>
#include <vsl/vsl_binary_io.h>

class volm_desc;
typedef vbl_smart_ptr<volm_desc> volm_desc_sptr;

class volm_desc : public vbl_ref_count
{
public:
  // Default constructor
  volm_desc() {}

  // Destructor
  virtual ~volm_desc() {};

  //: return the name of the descriptor
  virtual vcl_string name() { return "volm_desc base"; }

  //: return the number of bins of the histogram
  unsigned int nbins() { return h_.nbins(); }

  //: pretty print (prevent creation of base class by setting = 0)
  virtual void print() const = 0;

  //: general similarity matric for histogram
  virtual float similarity(volm_desc_sptr other);

  //: visualization using bsvg
  void visualize(vcl_string outfile, unsigned char const& y_max = 10) const;

  //: return the value of bin
  unsigned char count(unsigned const& i) const { return h_.counts(i); }

  //: Accerror to the value at bin i in the histogram
  unsigned char operator[] (unsigned int i) const { return h_.counts(i);}

   // ===========  binary I/O ================

  //: version
  unsigned version() const {return 1;}

  //: binary IO write
  virtual void b_write(vsl_b_ostream& os);

  //: binary IO read
  virtual void b_read(vsl_b_istream& is);

protected:
  //: name of the descriptor
  vcl_string name_;

  //: number of bins of the histogram
  unsigned int nbins_;

  //: histogram
  bsta_histogram<unsigned char> h_;
};

#endif  // volm_desc_h_
