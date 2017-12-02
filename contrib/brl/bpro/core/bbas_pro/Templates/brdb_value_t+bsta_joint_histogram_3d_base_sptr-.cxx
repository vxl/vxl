#include <brdb/brdb_value.hxx>
#include <bsta/bsta_joint_histogram_3d_sptr.h>
#include <bsta/io/bsta_io_histogram.h>

BRDB_VALUE_INSTANTIATE(bsta_joint_histogram_3d_base_sptr,"bsta_joint_histogram_3d_base_sptr");
// dummy instantiation for the following two:
//void vsl_b_read(vsl_b_istream&, bsta_joint_histogram_3d_base_sptr&) {}
//void vsl_b_write(vsl_b_ostream&, bsta_joint_histogram_3d_base_sptr const&) {}
