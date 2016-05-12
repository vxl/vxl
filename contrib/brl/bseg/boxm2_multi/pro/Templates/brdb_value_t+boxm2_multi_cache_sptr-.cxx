#include <brdb/brdb_value.hxx>
#include <boxm2_multi/boxm2_multi_cache.h>
#include <vbl/vbl_smart_ptr.hxx>

void vsl_b_read(vsl_b_istream&, boxm2_multi_cache_sptr&) {}
void vsl_b_write(vsl_b_ostream&, boxm2_multi_cache_sptr const&) {}

VBL_SMART_PTR_INSTANTIATE(boxm2_multi_cache);
BRDB_VALUE_INSTANTIATE(boxm2_multi_cache_sptr, "boxm2_multi_cache_sptr");
