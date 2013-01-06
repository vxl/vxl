// This is brl/bseg/boxm2/volm/boxm2_volm_io.h
#ifndef boxm2_volm_io_h_
#define boxm2_volm_io_h_
//:
// \file
// \brief Binary I/O for boxm2/volm classes
// \author Ozge C. Ozcanli
// \date January 2, 2013
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "boxm2_volm_locations.h"
#include "boxm2_volm_locations_sptr.h"
#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>

//: Binary save to stream.
void vsl_b_write(vsl_b_ostream &os, const boxm2_volm_loc_hypotheses& m);

//: Binary load from stream.
void vsl_b_read(vsl_b_istream &is, boxm2_volm_loc_hypotheses& m);

//: Binary write to stream
void vsl_b_write(vsl_b_ostream& os, const boxm2_volm_loc_hypotheses* &p);
//: Binary write to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_volm_loc_hypotheses_sptr& sptr);
//: Binary write to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_volm_loc_hypotheses_sptr const& sptr);

//: Binary load from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_volm_loc_hypotheses* p);
//: Binary load from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_volm_loc_hypotheses_sptr& sptr);
//: Binary load from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_volm_loc_hypotheses_sptr const& sptr);


#endif // boxm2_volm_io_h_
