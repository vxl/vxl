#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>

#include "open_cl_test_data.h"
#include <boxm/ocl/boxm_online_update_manager.h>
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_loc_code.h>
#include <vul/vul_timer.h>
#include <vbl/vbl_array_2d.h>


template <class T>
static void test_seg_len_obs()
{
 
}

static void test_update()
{
  bool good = true;
  vcl_string root_dir = testlib_root_dir();
  boxm_online_update_manager<float >* updt_mgr = boxm_online_update_manager<float >::instance();

  boxm_scene<boct_tree<short,float> > scene(vgl_point_3d<double>(0.0,0.0,0.0),vgl_vector_3d<double>(1,1,1),vgl_vector_3d<unsigned>(1,1,1));
  scene.set_octree_levels(3,2);
  
  scene.load_block(0,0,0);
  boxm_block<boct_tree<short,float> > * block=scene.get_active_block();
  block->set_tree(open_cl_test_data::tree<float>());
  
  vnl_matrix_fixed<double, 3, 3> mk(0.0);
  mk[0][0]=990.0; mk[0][2]=4.0;
  mk[1][1]=990.0; mk[1][2]=4.0; mk[2][2]=8.0/7.0;
  vpgl_calibration_matrix<double> K(mk);
  vnl_matrix_fixed<double, 3, 3> mr(0.0);
  mr[0][0]=1.0; mr[1][1]=-1.0; mr[2][2]=-1.0;
  vgl_rotation_3d<double> R(mr);
  vgl_vector_3d<double> t(-0.5,0.5,100);
  vpgl_camera_double_sptr cam = new vpgl_perspective_camera<double>(K,R,t);

  vil_image_view<float> img(8,8);
  img.fill(0.0f);
  updt_mgr->set_bundle_ni(2);
  updt_mgr->set_bundle_nj(2);

  updt_mgr->init_update(&scene,cam,img);

  updt_mgr->run_scene();

  float * image=updt_mgr->output_image();

  vcl_cout<<"Plane 0"<<vcl_endl;
  for(unsigned j=0;j<updt_mgr->wnj();j++)
  {
	  for(unsigned i=0;i<updt_mgr->wni();i++)	  
		  vcl_cout<<image[(j*updt_mgr->wni()+i)*4]<<" ";
	  vcl_cout<<vcl_endl;
  }
  vcl_cout<<"Plane 1"<<vcl_endl;
  for(unsigned j=0;j<updt_mgr->wnj();j++)
  {
	  for(unsigned i=0;i<updt_mgr->wni();i++)
		  vcl_cout<<image[(j*updt_mgr->wni()+i)*4+1]<<" ";
	  vcl_cout<<vcl_endl;
  }
  vcl_cout<<"Plane 2"<<vcl_endl;
  for(unsigned j=0;j<updt_mgr->wnj();j++)
  {
	  for(unsigned i=0;i<updt_mgr->wni();i++)
		  vcl_cout<<image[(j*updt_mgr->wni()+i)*4+2]<<" ";
	  vcl_cout<<vcl_endl;
  }


}

TESTMAIN(test_update);
