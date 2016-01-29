#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "test_utils.h"

#include <boxm2/boxm2_block.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/boxm2_blocks_vis_graph.h>
#include <vnl/vnl_vector_fixed.h>
#include <vpgl/vpgl_generic_camera.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_algorithm.h>
void create_scene(boxm2_scene_sptr & scene)
{
  // create block metadata
  double big_block_side = 2.0;

  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks;
  for( int i = 0; i < 5; i++ )
    {
    for( int j = 0; j < 5; j++ )
      {
      for( int k = 0; k < 2; k++ )
        {
        boxm2_block_id       id(i, j, k);
        boxm2_block_metadata data;
        data.id_ = id;
        data.local_origin_ = vgl_point_3d<double>(big_block_side * i, big_block_side * j, big_block_side * k);
        data.sub_block_dim_ = vgl_vector_3d<double>(.2, .2, .2);
        data.sub_block_num_ = vgl_vector_3d<unsigned>(10, 10, 10);
        data.init_level_ = 1;
        data.max_level_ = 4;
        data.max_mb_ = 400;
        data.p_init_ = .001;
        data.version_ = 2;
        // push it into the map
        blocks[id] = data;
        }
      }
    }
  // create scene
  scene->set_local_origin(vgl_point_3d<double>(0.0, 0.0, 0.0) );
  vpgl_lvcs lvcs;
  scene->set_lvcs(lvcs);
  scene->set_blocks(blocks);
}

void test_block_vis_graph()
{
  vbl_array_2d<vgl_ray_3d<double> > ray_array(2, 2);
  ray_array(0, 0) =   vgl_ray_3d<double>(vgl_point_3d<double>(0, 0, 100), vgl_vector_3d<double>(1, 1, -100) );
  ray_array(0, 1) =   vgl_ray_3d<double>(vgl_point_3d<double>(0, 0, 100), vgl_vector_3d<double>(3, 1, -100) );
  ray_array(1, 0) =   vgl_ray_3d<double>(vgl_point_3d<double>(0, 0, 100), vgl_vector_3d<double>(1, 3, -100) );
  ray_array(1, 1) =   vgl_ray_3d<double>(vgl_point_3d<double>(0, 0, 100), vgl_vector_3d<double>(3, 3, -100) );
  vpgl_generic_camera<double> gcam(ray_array);
  boxm2_scene_sptr            scene = new boxm2_scene();
  create_scene(scene);
  boxm2_block_vis_graph      g(scene->blocks(), gcam);
  vcl_vector<boxm2_block_id> ids = g.get_ordered_ids();
  vcl_cout << "Order is " << vcl_endl;
  for( unsigned i = 0; i < ids.size(); i++ )
    {
    vcl_cout << ids[i] << vcl_endl;
    }
}

TESTMAIN( test_block_vis_graph );
