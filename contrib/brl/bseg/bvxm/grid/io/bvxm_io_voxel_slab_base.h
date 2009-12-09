#ifndef bvxm_io_voxel_slab_base_h_
#define bvxm_io_voxel_slab_base_h_
//:
// \file

#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>
#include <bvxm/grid/bvxm_voxel_slab_base.h>

// Binary io, NOT IMPLEMENTED, signatures defined to use bvxm_voxel_slab_base as a brdb_value

//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, bvxm_voxel_slab_base const &world);

//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, bvxm_voxel_slab_base &sb);

void vsl_print_summary(vcl_ostream &os, const bvxm_voxel_slab_base &sb);

void vsl_b_read(vsl_b_istream& is, bvxm_voxel_slab_base* p);

void vsl_b_write(vsl_b_ostream& os, const bvxm_voxel_slab_base* &p);

void vsl_print_summary(vcl_ostream& os, const bvxm_voxel_slab_base* &p);

#endif
