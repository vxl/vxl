// This is brl/bseg/bvxm/pro/processes/bvxm_dem_to_xyz_process.h
#ifndef bvxm_dem_to_xyz_process_h_
#define bvxm_dem_to_xyz_process_h_
//:
// \file
// \brief A class that generate cropped x y z image for given bvxm_scene
//             -  Input:
//                    * bvxm_voxel_world_sprt
//                    DEM image
//                    geocam if available, otherwise camera will be constructed using info in geotiff dem header
//             -  Output:
//                    x, y, z image with scene resolution
//
// \author Yi Dong
// \date Jan 03, 2014
// \verbatim
//  Modifications
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_world_params.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vpgl/vpgl_generic_camera.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>


//: global variables and functions
namespace bvxm_dem_to_xyz_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 3;
}
//: set input and output types
bool bvxm_dem_to_xyz_process_cons(bprb_func_process& pro);

//: actual process implementation
bool bvxm_scene_kml_process(bprb_func_process& pro);

//: generate x y z images using all available DEM resource in folder (used to handle the situation where scene may overlap with multiple DEM images)
namespace bvxm_dem_to_xyz_process2_globales
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 3;
}

bool bvxm_dem_to_xyz_process2_cons(bprb_func_process& pro);

bool bvxm_dem_to_xyz_process2(bprb_func_process& pro);


#endif // bvxm_dem_to_xyz_process_h_
