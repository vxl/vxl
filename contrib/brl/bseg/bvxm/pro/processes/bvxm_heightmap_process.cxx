// This is brl/bseg/bvxm/pro/processes/bvxm_heightmap_process.cxx
#include "bvxm_heightmap_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bvxm/bvxm_voxel_world.h>
#include <vil/vil_image_view.h>

//: set input and output types
bool bvxm_heightmap_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_heightmap_process_globals;
  // This process has 4 inputs:
  //input[0]: The camera to render the heightmap from
  //input[1]: number of pixels (x)
  //input[2]: number of pixels (y)
  //input[3]: The voxel world
  //input[4]: The option to generate a negated depth map, i.e., an absolute height map
  std::vector<std::string> input_types_(n_inputs_);
  int i=0;
  input_types_[i++] = "vpgl_camera_double_sptr";  // camera
  input_types_[i++] = "unsigned";
  input_types_[i++] = "unsigned";
  input_types_[i++] = "bvxm_voxel_world_sptr";    // voxel_world
  input_types_[i++] = "bool";
  if (!pro.set_input_types(input_types_))
    return false;

  //output
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  return pro.set_output_types(output_types_);
}

// generates a height map from a given camera viewpoint
bool bvxm_heightmap_process(bprb_func_process& pro)
{
  using namespace bvxm_heightmap_process_globals;

  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get the inputs:

  // camera
  unsigned i = 0;
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);

  // npixels x
  auto npixels_x = pro.get_input<unsigned>(i++);

  // npixels y
  auto npixels_y = pro.get_input<unsigned>(i++);

  //voxel_world
  bvxm_voxel_world_sptr voxel_world =  pro.get_input<bvxm_voxel_world_sptr>(i++);

  //option to negate depth map
  bool is_negate = pro.get_input<bool>(i++);

   //check inputs validity
  if (!camera) {
    std::cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }
   //check inputs validity
  if (!voxel_world) {
    std::cout << pro.name() <<" :--  Input 3  is not valid!\n";
    return false;
  }

  auto *hmap = new vil_image_view<unsigned>(npixels_x, npixels_y, 1);
  auto *conf_map = new vil_image_view<float>(npixels_x, npixels_y, 1);

  voxel_world->heightmap(camera, *hmap, *conf_map);

  if (is_negate) {
    // generate absolute height map by negating depth map with scene floor + scene floor height
    bvxm_world_params_sptr params = voxel_world->get_params();
    vgl_box_3d<double> box = params->world_box_local();
    vpgl_lvcs_sptr lvcs = params->lvcs();
    // locate the absolute elevation of the point where the ray tracing stops, that is, one slab below the lowest world slab
    vgl_point_3d<float> ray_trace_end = voxel_world->voxel_index_to_xyz(0, 0, params->num_voxels().z()+1, 0);
    double lon, lat, base_elev;
    lvcs->local_to_global(ray_trace_end.x(), ray_trace_end.y(), ray_trace_end.z(), vpgl_lvcs::wgs84, lon, lat, base_elev);
    // obtain the scene height
    float h = box.depth();
    std::cout << "Using scene height: " << h << " and scene floor elevation: " << base_elev << " to negate the depth map!\n";
    unsigned ni = hmap->ni();
    unsigned nj = hmap->nj();
    unsigned np = hmap->nplanes();
    auto* negated_hmap = new vil_image_view<float>(ni, nj, np);
    negated_hmap->fill(0.0f);
    for (unsigned i = 0; i < ni; i++)
      for (unsigned j = 0; j < nj; j++)
        for (unsigned k = 0; k < np; k++)
          (*negated_hmap)(i,j,k) = h - (*hmap)(i,j,k) + base_elev;
    // store output
    pro.set_output_val<vil_image_view_base_sptr>(0, negated_hmap);

    return true;
  }

  //store output
  pro.set_output_val<vil_image_view_base_sptr>(0, hmap);

  return true;
}
