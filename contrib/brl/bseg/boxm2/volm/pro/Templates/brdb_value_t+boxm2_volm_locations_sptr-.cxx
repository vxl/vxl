#include <brdb/brdb_value.txx>
#include <boxm2/volm/boxm2_volm_locations_sptr.h>

BRDB_VALUE_INSTANTIATE(boxm2_volm_loc_hypotheses_sptr, "boxm2_volm_loc_hypotheses_sptr");
void vsl_b_read(vsl_b_istream&, boxm2_volm_loc_hypotheses_sptr&) {}
void vsl_b_write(vsl_b_ostream&, boxm2_volm_loc_hypotheses_sptr const&) {}
