//:
// \file
// \author Vishal Jain
// \date 26-Jan-2011

#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>

#include <boct/boct_bit_tree.h>

#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_block_metadata.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boxm2/cpp/algo/boxm2_update_functions.h>
#include <boxm2/cpp/algo/boxm2_render_functions.h>
#include <vil/vil_transform.h>

vpgl_camera_double_sptr test_camera_cone()
{
  vnl_matrix_fixed<double, 3, 3> mk(0.0);
  mk[0][0]=990.0; mk[0][2]=4.0;
  mk[1][1]=990.0; mk[1][2]=4.0; mk[2][2]=8.0/7.0;
  vpgl_calibration_matrix<double> K(mk);
  vnl_matrix_fixed<double, 3, 3> mr(0.0);
  mr[0][0]=1.0; mr[1][1]=-1.0; mr[2][2]=-1.0;
  vgl_rotation_3d<double> R(mr);
  vgl_point_3d<double> t(0.5,0.5,100);

  std::cout<<mk<<mr<<t;
  vpgl_camera_double_sptr cam = new vpgl_perspective_camera<double>(K,t,R);
  return cam;
}


void test_cone_update()
{
  // manufacture scene information
  boxm2_scene_sptr scene = new boxm2_scene();
  scene->set_local_origin( vgl_point_3d<double>(0,0,0) );

  // set scene block information
  std::map<boxm2_block_id, boxm2_block_metadata> blocks;
  boxm2_block_id id(0,0,0);
  boxm2_block_metadata data(id,
                            vgl_point_3d<double>(0,0,0),
                            vgl_vector_3d<double>(1.0/8.0, 1.0/8.0, 1.0/8.0),
                            vgl_vector_3d<unsigned>(8,8,1),
                            1, 1, 100,
                            0.01); // Pinit starts out at 0 (will create 0 alphas?
  blocks[id] = data;
  scene->set_blocks(blocks);

  // list of appearance models/observation models used by this scene
  std::vector<std::string> appearances;
  appearances.push_back(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  scene->set_appearances(appearances);

  // grab boxm2_scene_info
  boxm2_scene_info* info = scene->get_blk_metadata(id);

  // instantiate a cache to grab the first block
  boxm2_lru_cache::create(scene);
  boxm2_block* blk = boxm2_cache::instance()->get_block(scene,id);
  // set block data to be black and white
  boxm2_data_base * alph = boxm2_cache::instance()->get_data_base(scene,id,boxm2_data_traits<BOXM2_GAMMA>::prefix());
  boxm2_data<BOXM2_GAMMA>    * alpha_data_ = new boxm2_data<BOXM2_GAMMA>(alph->data_buffer(),alph->buffer_length(),alph->block_id());
  boxm2_data_base * mog  = boxm2_cache::instance()->get_data_base(scene,id,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  boxm2_data<BOXM2_MOG3_GREY>* mog3_data_  = new boxm2_data<BOXM2_MOG3_GREY>(mog->data_buffer(),mog->buffer_length(),mog->block_id());

  typedef vnl_vector_fixed<vxl_byte, 16> uchar16;

  // fill in the tree with synthetic data
  vnl_vector_fixed<vxl_byte,8> empty_app;
  empty_app[0]=0; empty_app[3]=0; empty_app[6]=0;
  empty_app[1]=0;  empty_app[4]=0; empty_app[7]=0;
  empty_app[2]=0; empty_app[5]=0;

  vnl_vector_fixed<vxl_byte,8> white_app;
  white_app[0]=255; white_app[3]=0; white_app[6]=0;
  white_app[1]=25;  white_app[4]=0; white_app[7]=0;
  white_app[2]=255; white_app[5]=0;

  vnl_vector_fixed<vxl_byte,8> black_app;
  black_app[0]=0; black_app[3]=0; black_app[6]=0;
  black_app[1]=25;  black_app[4]=0; black_app[7]=0;
  black_app[2]=255; black_app[5]=0;

  for (int x=0; x<8; ++x) {
    for (int y=0; y<4; ++y) {
      // load current block/tree
      uchar16 tree = blk->trees()(x,y,0);
      boct_bit_tree bit_tree( (unsigned char*)tree.data_block(), info->root_level+1);
      int data_ptr = bit_tree.get_data_ptr();

      alpha_data_->data()[data_ptr] = 1000;
      mog3_data_->data()[data_ptr] = empty_app;
    }
  }
  for (int x=0; x<8; ++x) {
    for (int y=4; y<8; ++y) {
      // load current block/tree
      uchar16 tree = blk->trees()(x,y,0);
      boct_bit_tree bit_tree( (unsigned char*)tree.data_block(), info->root_level+1);
      int data_ptr = bit_tree.get_data_ptr();

      alpha_data_->data()[data_ptr] = 1000;
      mog3_data_->data()[data_ptr] = empty_app;// boxm2_data<BOXM2_MOG3_GREY>::datatype((vxl_byte) 0);
    }
  }

  // generate test camera
  vpgl_camera_double_sptr cam = test_camera_cone();

  // create input image for update
  unsigned ni=8, nj=8;
  vil_image_view<float> input_img(ni,nj);
  for (unsigned i=0;i<ni;i++)
    for (unsigned j=0;j<nj;j++)
      input_img(i,j)=(float)i/8;
    bool test_success=true;
  // run render process
  test_success= test_success && boxm2_update_cone_image(scene,"boxm2_mog3_grey","boxm2_num_obs",cam, &input_img,  ni, nj) ;


  TEST("Cone Update function Call works" ,test_success,true);
}


TESTMAIN(test_cone_update);
