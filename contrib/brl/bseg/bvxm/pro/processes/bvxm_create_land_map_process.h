// This is brl/bseg/bvxm/pro/processes/bvxm_create_land_map_process.h
#ifndef bvxm_create_land_map_process_h_
#define bvxm_create_land_map_process_h_
//:
// \file
// \brief A process that generate landmark maps for given bvxm_scene
//            - Input:
//                  * bvxm_voxel_world_sptr
//                  Geo cover images
//                  Urban landmark images
//                  Open Street Map (OSM) data
//                  option to ingest OSM data
//
// \author Yi Dong
// \date Sep 29, 2014
// \verbatim
//   Modifications
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_world_params.h>
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_lvcs_sptr.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <volm/volm_osm_parser.h>
#include <volm/volm_osm_objects.h>
#include <volm/volm_category_io.h>
#include <volm/volm_tile.h>
#include <volm/volm_io.h>
#include <volm/volm_io_tools.h>
#include <vpgl/vpgl_utm.h>

//: global variables and functions
namespace bvxm_create_land_map_process_globals
{
  constexpr unsigned n_inputs_ = 8;
  constexpr unsigned n_outputs_ = 1;
}

//: set input and outout types
bool bvxm_create_land_map_process_cons(bprb_func_process& pro);

//: actual process implementation
bool bvxm_create_land_map_process(bprb_func_process& pro);

#endif // bvxm_create_land_map_process_h_
