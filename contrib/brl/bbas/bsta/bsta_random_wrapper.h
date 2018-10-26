// This is brl/bbas/bsta/bsta_random_wrapper.h
#ifndef bsta_random_wrapper_h_
#define bsta_random_wrapper_h_
//:
// \file
// \brief A simple wrapper class around vnl_random, needed to be used across python processes
// \author Ozge C. Ozcanli
// \date February 08, 2012
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vsl/vsl_binary_io.h>

#include <vnl/vnl_random.h>

class bsta_random_wrapper : public vbl_ref_count
{
 public:
   bsta_random_wrapper(unsigned long seed) : rng_(seed) {}
   bsta_random_wrapper() = default;

   vnl_random rng_;
};

typedef vbl_smart_ptr<bsta_random_wrapper> bsta_random_wrapper_sptr;


//--- IO read/write for sptrs--------------------------------------------------
//: Binary write boxm2_scene scene to stream
void vsl_b_write(vsl_b_ostream& os, bsta_random_wrapper const& scene);
void vsl_b_write(vsl_b_ostream& os, const bsta_random_wrapper* &p);
void vsl_b_write(vsl_b_ostream& os, bsta_random_wrapper_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, bsta_random_wrapper_sptr const& sptr);

//: Binary load boxm2_scene scene from stream.
void vsl_b_read(vsl_b_istream& is, bsta_random_wrapper &scene);
void vsl_b_read(vsl_b_istream& is, bsta_random_wrapper* p);
void vsl_b_read(vsl_b_istream& is, bsta_random_wrapper_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, bsta_random_wrapper_sptr const& sptr);

#endif // bsta_random_wrapper_h_
