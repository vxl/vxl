#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>

#include "open_cl_test_data.h"
#include "online_update_test_manager.h"
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_loc_code.h>
#include <boxm/boxm_apm_traits.h>
#include <vul/vul_timer.h>
#include <vbl/vbl_array_2d.h>
#include <vil/vil_convert.h>
#include <boxm/algo/rt/boxm_update_image_functor.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <boxm/algo/boxm_init_scene.h>

template<unsigned n>
static bool near_eq(vnl_vector_fixed<float, n> v1,
                    vnl_vector_fixed<float, n> v2, float tol)
{
  float d = (v1-v2).magnitude();
  if (d>tol)
    return false;
  return true;
}


vpgl_camera_double_sptr test_camera()
{
    vnl_matrix_fixed<double, 3, 3> mk(0.0);
    mk[0][0]=990.0; mk[0][2]=4.0;
    mk[1][1]=990.0; mk[1][2]=4.0; mk[2][2]=8.0/7.0;
    vpgl_calibration_matrix<double> K(mk);
    vnl_matrix_fixed<double, 3, 3> mr(0.0);
    mr[0][0]=1.0; mr[1][1]=-1.0; mr[2][2]=-1.0;
    vgl_rotation_3d<double> R(mr);
    vgl_vector_3d<double> t(-0.5,0.5,100);
    vpgl_camera_double_sptr cam = new vpgl_perspective_camera<double>(K,R,t);
    return cam;
}
static void test_update_upto_pass_4()
{
  bool good = true;
  vcl_string root_dir = testlib_root_dir();
  online_update_test_manager<boxm_sample<BOXM_APM_MOG_GREY> >* updt_mgr =
    online_update_test_manager<boxm_sample<BOXM_APM_MOG_GREY> >::instance();

  vgl_box_3d<double> box;
  box.add(vgl_point_3d<double>(0.0,0.0,0.0));
  box.add(vgl_point_3d<double>(1.0,1.0,1.0));
  boxm_block<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > * block =
    new boxm_block<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > >(box);
  block->set_tree(open_cl_test_data::tree<boxm_sample<BOXM_APM_MOG_GREY> >());

  vpgl_camera_double_sptr cam = test_camera();

  vil_image_view<float> img(8,8);
  for (unsigned j =0; j<8; ++j)
    for (unsigned i =0; i<8; ++i)
      img(i,j) = static_cast<float>((i+j+1)/15.0f);
  updt_mgr->set_bundle_ni(2);
  updt_mgr->set_bundle_nj(2);
  updt_mgr->set_block_items(block,cam,img);
  if (!updt_mgr->setup_norm_data(true, 0.5f, 0.25f))
    return;
  updt_mgr->process_block(4);
  vcl_vector<vnl_vector_fixed<float, 16> > tree_data = updt_mgr->tree_data();
  vcl_vector<vnl_vector_fixed<float, 4> > tree_aux_data = updt_mgr->aux_data();
#if 0
  vcl_cout << "Cell data -->\n";
  for (unsigned i = 0; i<tree_data.size(); ++i)
    vcl_cout << tree_data[i] << '\n' << tree_aux_data[i] << "\n\n";
#endif
  float dblk[] = {1.38629f, 0.533333f, 0.1f, 1.0f, 1.0f, 0.0f,
                  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f};
  vnl_vector_fixed<float, 16> tdata(dblk);
  float ablk[] = {1.0f, 0.533333f, 1.23052f, 0.249998f};
  vnl_vector_fixed<float, 4> adata(ablk);
  good = near_eq<16>(tree_data[0],tdata, 0.0001f);
  good = good && near_eq<4>(tree_aux_data[0],adata, 0.0001f);
  TEST("All passes except update and refine", good, true);
}
static void test_update_upto_pass_5()
{
  bool good = true;
  vcl_string root_dir = testlib_root_dir();
  online_update_test_manager<boxm_sample<BOXM_APM_MOG_GREY> >* updt_mgr =
    online_update_test_manager<boxm_sample<BOXM_APM_MOG_GREY> >::instance();

  vgl_box_3d<double> box;
  box.add(vgl_point_3d<double>(0.0,0.0,0.0));
  box.add(vgl_point_3d<double>(1.0,1.0,1.0));
  boxm_block<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > * block =
    new boxm_block<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > >(box);
  block->set_tree(open_cl_test_data::tree<boxm_sample<BOXM_APM_MOG_GREY> >());

  vpgl_camera_double_sptr cam = test_camera();

  vil_image_view<float> img(8,8);
  for (unsigned j =0; j<8; ++j)
    for (unsigned i =0; i<8; ++i)
      img(i,j) = static_cast<float>((i+j+1)/15.0f);
  updt_mgr->set_bundle_ni(2);
  updt_mgr->set_bundle_nj(2);
  updt_mgr->set_block_items(block,cam,img);
  if (!updt_mgr->setup_norm_data(true, 0.5f, 0.25f))
    return;
  updt_mgr->process_block(5);
  vcl_vector<vnl_vector_fixed<float, 16> > tree_data = updt_mgr->tree_data();
  vcl_vector<vnl_vector_fixed<float, 4> > tree_aux_data = updt_mgr->aux_data();
#if 0
  vcl_cout << "Cell data -->\n";
  for (unsigned i = 0; i<tree_data.size(); ++i)
    vcl_cout << tree_data[i] << '\n' << tree_aux_data[i] << "\n\n";
#endif
  float dblk0[] = {1.7058496f, 0.53333318f, 0.063200004f, 1.0f, 2.0f, 0.0f,
                  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.2499964f, 0.0f, 0.0f, 0.0f};
  vnl_vector_fixed<float, 16> tdata0(dblk0);
  float dblk1[] = {0.470925f, 0.533333f, 0.1f, 0.800001f, 1.0f, 0.8f, 0.09f, 0.199999f, 1.0f, 0.0f, 0.0f, 0.0f, 1.25f, 0.0f, 0.0f, 0.0f};
  vnl_vector_fixed<float, 16> tdata1(dblk1);
  good = near_eq<16>(tree_data[0],tdata0, 0.0001f);
  TEST("Upto Pass5 updated an existing component", good, true);
  good = near_eq<16>(tree_data[8],tdata1, 0.0001f);
  TEST("Upto Pass5 created a new component", good, true);

}
//: this camera is looking down and is placed far away so the camera is almost orthographic.

void update_with_opencl(vcl_vector<vnl_vector_fixed<float, 16> > & updated_tree)
{
    //: prepare the scene.
    boxm_scene<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY > > > 
        s(vgl_point_3d<double>(0.0,0.0,0.0),vgl_vector_3d<double>(1.0,1.0,1.0),vgl_vector_3d<unsigned>(1,1,1));
    s.set_appearance_model(BOXM_APM_MOG_GREY);
    s.set_octree_levels(3,3);
    s.set_path("./","oneblock");
    s.set_pinit(0.01);

    s.clean_scene();
    boxm_init_scene<BOXM_APM_MOG_GREY>(s);
    vpgl_camera_double_sptr cam=test_camera();

    vil_image_view<float> img(8,8);
    for (unsigned j =0; j<8; ++j)
        for (unsigned i =0; i<8; ++i)
            img(i,j) = static_cast<float>((i+j+1)/15.0f);

   online_update_test_manager<boxm_sample<BOXM_APM_MOG_GREY> >* updt_mgr =
       online_update_test_manager<boxm_sample<BOXM_APM_MOG_GREY> >::instance();

   s.load_block(vgl_point_3d<int>(0,0,0));
   boxm_block<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > * block =s.get_active_block();
   updt_mgr->set_bundle_ni(2);
   updt_mgr->set_bundle_nj(2);
   updt_mgr->set_block_items(block,cam,img);
   if (!updt_mgr->setup_norm_data(true, 0.5f, 0.25f))
       return ;
   updt_mgr->process_block(5);
   updated_tree = updt_mgr->tree_data();
   
    s.write_active_block();
    s.clean_scene();

}
void update_with_boxm_cpp(vcl_vector<vnl_vector_fixed<float, 16> > & updated_tree)
{
    boxm_scene<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY > > > 
        s(vgl_point_3d<double>(0.0,0.0,0.0),vgl_vector_3d<double>(1.0,1.0,1.0),vgl_vector_3d<unsigned>(1,1,1));
    s.set_appearance_model(BOXM_APM_MOG_GREY);
    s.set_octree_levels(3,3);
    s.set_path("./","oneblock");
    s.set_pinit(0.01);
    s.clean_scene();
    boxm_init_scene<BOXM_APM_MOG_GREY>(s);
    
    vpgl_camera_double_sptr cam=test_camera();

    vil_image_view<float> img(8,8);
    for (unsigned j =0; j<8; ++j)
        for (unsigned i =0; i<8; ++i)
            img(i,j) = static_cast<float>((i+j+1)/15.0f);

    boxm_update_image_rt<short, boxm_sample<BOXM_APM_MOG_GREY> >(s,cam,img,false);

    s.load_block(vgl_point_3d<int>(0,0,0));
    boxm_block<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > * block =s.get_active_block();

    boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > * tree=block->get_tree();
    vcl_vector<vnl_vector_fixed<int, 4> > cell_array;
    boxm_ocl_convert<boxm_sample<BOXM_APM_MOG_GREY> >::copy_to_arrays(tree->root(),cell_array,updated_tree,0);

    s.clean_scene();

}

static void test_update_with_cpp_implementation()
{
    vcl_vector<vnl_vector_fixed<float, 16> > tree_data_opencl;
    vcl_vector<vnl_vector_fixed<float, 16> > tree_data_boxm_cpp;

    update_with_boxm_cpp(tree_data_boxm_cpp);
    update_with_opencl(tree_data_opencl);

    TEST("Tree size is equal", tree_data_boxm_cpp.size(), tree_data_opencl.size());

    bool flag=true;
    if(tree_data_opencl.size()==tree_data_boxm_cpp.size())
        for(unsigned i=0;i<tree_data_opencl.size();i++)
            if(!near_eq<16>(tree_data_opencl[i],tree_data_boxm_cpp[i],0.2))
                flag=false;

    TEST("GPU update matches the c++ update", flag, true);
}


static void test_update()
{
    test_update_upto_pass_4();
    test_update_upto_pass_5();
    test_update_with_cpp_implementation();

}
TESTMAIN(test_update);
