#include "bvxm_io_voxel_world.h"
//:
// \file
#include <vsl/vsl_binary_io.h>
#include "../bvxm_world_params.h"


//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, bvxm_voxel_world const &world)
{
  // just save world parameters
  bvxm_world_params_sptr params = world.get_params();
  params->b_write(os);
}


//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, bvxm_voxel_world &world)
{
  // read world parameters
  bvxm_world_params_sptr params = world.get_params();
  params->b_read(is);
}

void vsl_print_summary(std::ostream &os, const bvxm_voxel_world &world)
{
  bvxm_world_params_sptr params = world.get_params();
  os << *params;
}

void vsl_b_read(vsl_b_istream& is, bvxm_voxel_world* p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    p = new bvxm_voxel_world();
    vsl_b_read(is, *p);
  }
  else
    p = nullptr;
}

void vsl_b_write(vsl_b_ostream& os, const bvxm_voxel_world* &p)
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

void vsl_print_summary(std::ostream& os, const bvxm_voxel_world* &p)
{
  if (p==nullptr)
    os << "NULL PTR";
  else {
    os << "T: ";
    vsl_print_summary(os, *p);
  }
}
