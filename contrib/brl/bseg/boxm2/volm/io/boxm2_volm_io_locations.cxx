#include "boxm2_volm_io_locations.h"
//:
// \file
#include <vsl/vsl_binary_io.h>
#include "../boxm2_volm_locations.h"
#include <volm/volm_tile.h>


//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, boxm2_volm_loc_hypotheses const &loc_hyp)
{
  loc_hyp.b_write(os);
}


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, boxm2_volm_loc_hypotheses &loc_hyp)
{
  loc_hyp.b_read(is);
}

void vsl_print_summary(std::ostream &os, const boxm2_volm_loc_hypotheses & /*loc_hyp*/)
{
  // not yet implemented
  os << "vsl_print_summary for boxm2_volm_loc_hypotheses Not Implemented" << std::endl;
}

void vsl_b_read(vsl_b_istream& is, boxm2_volm_loc_hypotheses* p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    p = new boxm2_volm_loc_hypotheses(volm_tile());
    vsl_b_read(is, *p);
  }
  else
    p = nullptr;
}

void vsl_b_write(vsl_b_ostream& os, const boxm2_volm_loc_hypotheses* &p)
{
  if (p==nullptr)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*p);
  }
}

void vsl_print_summary(std::ostream& os, const boxm2_volm_loc_hypotheses* &p)
{
  if (p==nullptr)
    os << "NULL PTR";
  else {
    os << "T: ";
    vsl_print_summary(os, *p);
  }
}
