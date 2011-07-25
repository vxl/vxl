#include <brdb/brdb_value.txx>
#include <bsta/bsta_joint_histogram_3d_sptr.h>

BRDB_VALUE_INSTANTIATE(bsta_joint_histogram_3d_base_sptr,"bsta_joint_histogram_3d_base_sptr");
// dummy instantiation for the following two:
void vsl_b_read(vsl_b_istream&, bsta_joint_histogram_3d_base_sptr&) {}
void vsl_b_write(vsl_b_ostream&, bsta_joint_histogram_3d_base_sptr const&) {}
