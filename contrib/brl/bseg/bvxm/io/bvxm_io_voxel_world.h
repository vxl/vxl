#ifndef bvxm_io_voxel_world_h_
#define bvxm_io_voxel_world_h_
//:
// \file

#include <iostream>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_world_params.h>


//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, bvxm_voxel_world const &world);

//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, bvxm_voxel_world &world);

void vsl_print_summary(std::ostream &os, const bvxm_voxel_world &world);

void vsl_b_read(vsl_b_istream& is, bvxm_voxel_world* p);

void vsl_b_write(vsl_b_ostream& os, const bvxm_voxel_world* &p);

void vsl_print_summary(std::ostream& os, const bvxm_voxel_world* &p);

#endif
