#ifndef bvxm_io_voxel_world_h_
#define bvxm_io_voxel_world_h_


#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>
#include "../bvxm_voxel_world.h"
#include "../bvxm_world_params.h"


//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, bvxm_voxel_world const &world);

//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, bvxm_voxel_world &world);

void vsl_print_summary(vcl_ostream &os, const bvxm_voxel_world &world);

void vsl_b_read(vsl_b_istream& is, bvxm_voxel_world* p);

void vsl_b_write(vsl_b_ostream& os, const bvxm_voxel_world* &p);
 
void vsl_print_summary(vcl_ostream& os, const bvxm_voxel_world* &p);

#endif

