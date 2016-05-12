//This is brl/bseg/bvxm/pro/processes/bvxm_update_point_cloud_process.cxx
#include "bvxm_update_point_cloud_process.h"
//:
// \file

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vgl/vgl_point_3d.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>

struct compare_point_3d
{
  bool operator()(const vgl_point_3d<float> &a, const vgl_point_3d<float>  &b)
  {
      return a.z() > b.z();
  }
};

bool bvxm_update_point_cloud_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_update_point_cloud_process_globals;

  //process takes 4inputs
  //input[0]: filename for input point cloud (x y z)
  //input[1]: filename for the voxel grid
  //input[2]: spacing in x-y
  //input[3]: spacing in z
  //input[4]: use opinion ?
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";
  input_types_[1] = "bvxm_voxel_world_sptr";
  input_types_[2] = "float";
  input_types_[3] = "float";
  input_types_[4] = "bool";
  return pro.set_input_types(input_types_);
}

bool bvxm_update_point_cloud_process(bprb_func_process& pro)
{
  using namespace bvxm_update_point_cloud_process_globals;

  //check number of inputs
  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get inputs
  unsigned i = 0;
  std::string point_filename = pro.get_input<std::string>(i++);
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  /* float xy_spacing = */ pro.get_input<float>(i++);
  /* float z_spacing  = */ pro.get_input<float>(i++);
  bool use_opinion = pro.get_input<bool>(i++);

  std::ifstream ifile(point_filename.c_str());
  if (!ifile)
  {
    std::cout<<"Failed to open "<<point_filename<<std::endl;
    return false;
  }

  std::vector<vgl_point_3d<float> > point_cloud;
  float x,y,z;

  //: read the file to obtain min, max
  while (ifile)
  {
    ifile>>x>>y>>z;
    vgl_point_3d<float> p3d(x,y,z);
    point_cloud.push_back(p3d);
  }
  std::sort(point_cloud.begin(), point_cloud.end(), compare_point_3d());
  bool result=true;
  if (!use_opinion)
  {
    result =result && world->update_point_cloud<OCCUPANCY>(point_cloud);
  }
  else {
    std::cout << "Working with opinion!" << std::endl;
    result =result && world->update_point_cloud<OCCUPANCY_OPINION>(point_cloud);
  }

  return result;
}
