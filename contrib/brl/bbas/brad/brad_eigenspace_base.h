// This is brl/bbas/brad/brad_eigenspace_base.h
#ifndef brad_eigenspace_base_h_
#define brad_eigenspace_base_h_
//:
// \file
// \brief Abstract base class for brad_eigenspace
// \author Joseph Mundy
// \date July 2, 2011
//
// \verbatim
// \endverbatim

#include <vcl_string.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_compiler.h>
#include <vsl/vsl_binary_io.h>
#include <brad/brad_eigenspace_sptr.h>
class brad_eigenspace_base : public vbl_ref_count
{
 public:
  //: Default constructor
  brad_eigenspace_base(){}

  virtual ~brad_eigenspace_base() {}

  virtual vcl_string feature_vector_type(){return "unknown\n";}
};

void vsl_b_write(vsl_b_ostream &os, const brad_eigenspace_sptr& /*eptr*/);

void vsl_b_read(vsl_b_istream &is, brad_eigenspace_sptr& /*eptr*/);


#endif // brad_eigenspace_base_h_
