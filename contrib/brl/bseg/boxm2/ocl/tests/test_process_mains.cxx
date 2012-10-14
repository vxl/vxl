//:
// \file
// \author Andy Miller
// \date 26-Oct-2010
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vcl_where_root_dir.h>
#include <vcl_algorithm.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <bocl/bocl_manager.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>

#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vnl/vnl_random.h>


//: Example c++ calls
void test_render_height_main(boxm2_scene_sptr& scene, bocl_device_sptr& device, boxm2_opencl_cache_sptr& opencl_cache)
{
  //////////////////////////////////////////////////////////////////////////////
  //Only has to be done once
  //
  //register data types and process functions
  DECLARE_FUNC_CONS(boxm2_ocl_render_expected_height_map_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_height_map_process, "boxm2OclRenderExpectedHeightMapProcess");
  REGISTER_DATATYPE(boxm2_opencl_cache_sptr);
  REGISTER_DATATYPE(boxm2_scene_sptr);
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(float);
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Has to be done on each render
  //
  //set up brdb_value_sptr arguments... (for generic passing)
  brdb_value_sptr brdb_device = new brdb_value_t<bocl_device_sptr>(device);
  brdb_value_sptr brdb_scene = new brdb_value_t<boxm2_scene_sptr>(scene);
  brdb_value_sptr brdb_opencl_cache = new brdb_value_t<boxm2_opencl_cache_sptr>(opencl_cache);

  //if scene has RGB data type, use color render process
  bool good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderExpectedHeightMapProcess");

  //set process args
  good = good && bprb_batch_process_manager::instance()->set_input(0, brdb_device) // device
              && bprb_batch_process_manager::instance()->set_input(1, brdb_scene)  //  scene
              && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache)
              && bprb_batch_process_manager::instance()->run_process();

  //The Height Map Process has 5 outputs -
  // 1. expected height (z image)
  // 2. variance in expected height
  // 3. x coord image
  // 4. y coord image
  // 5. prob image (likelihood depth is within the volume)
  vcl_vector< vil_image_view<float>* > out_imgs;
  vcl_vector<unsigned int> out_ids;
  for (int i=0; i<6; ++i) {
    unsigned int out_img = 0;
    good = good && bprb_batch_process_manager::instance()->commit_output(i, out_img);
    out_ids.push_back(out_img);
    brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, out_img);
    brdb_selection_sptr S = DATABASE->select("vil_image_view_base_sptr_data", Q);
    if (S->size()!=1) {
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " no selections\n";
    }
    brdb_value_sptr value;
    if (!S->get_value(vcl_string("value"), value)) {
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " didn't get value\n";
    }
    vil_image_view_base_sptr out_img_sptr = value->val<vil_image_view_base_sptr>();
    vil_image_view<float>* flt_ptr = (vil_image_view<float>*) out_img_sptr.ptr();
    out_imgs.push_back(flt_ptr);
  }

  vil_save(*out_imgs[0], "height.tiff");
  vil_save(*out_imgs[1], "var.tiff");
  vil_save(*out_imgs[2], "x_img.tiff");
  vil_save(*out_imgs[3], "y_img.tiff");
  vil_save(*out_imgs[4], "prob_img.tiff");
  vil_save(*out_imgs[5], "app_img.tiff");

  //EXAMPLE
  //clean up out images
  for (unsigned int i=0; i<out_ids.size(); ++i) {
    bprb_batch_process_manager::instance()->remove_data(out_ids[i]);
  }
}
//: Example c++ calls
void test_refine_main(boxm2_scene_sptr& scene, bocl_device_sptr& device, boxm2_opencl_cache_sptr& opencl_cache)
{
  //////////////////////////////////////////////////////////////////////////////
  //Only has to be done once
  //
  //register data types and process functions
  DECLARE_FUNC_CONS(boxm2_ocl_refine_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_refine_process, "boxm2OclRefineProcess");
  REGISTER_DATATYPE(boxm2_opencl_cache_sptr);
  REGISTER_DATATYPE(boxm2_scene_sptr);
  REGISTER_DATATYPE(float);
  //


  //set up brdb_value_sptr arguments... (for generic passing)
  brdb_value_sptr brdb_device = new brdb_value_t<bocl_device_sptr>(device);
  brdb_value_sptr brdb_scene = new brdb_value_t<boxm2_scene_sptr>(scene);
  brdb_value_sptr brdb_opencl_cache = new brdb_value_t<boxm2_opencl_cache_sptr>(opencl_cache);
  brdb_value_sptr brdb_thresh = new brdb_value_t<float>(0.3f);

  //if scene has RGB data type, use color render process
  bool good = true;
  good = bprb_batch_process_manager::instance()->init_process("boxm2OclRefineProcess");
  //set process args
  good = good && bprb_batch_process_manager::instance()->set_input(0, brdb_device) // device
              && bprb_batch_process_manager::instance()->set_input(1, brdb_scene)  //  scene
              && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache)
              && bprb_batch_process_manager::instance()->set_input(3, brdb_thresh)    // camera
               && bprb_batch_process_manager::instance()->run_process();


}
//: Example c++ calls
void test_render_main(boxm2_scene_sptr& scene, bocl_device_sptr& device, boxm2_opencl_cache_sptr& opencl_cache)
{
  //////////////////////////////////////////////////////////////////////////////
  //Only has to be done once
  //
  //register data types and process functions
  DECLARE_FUNC_CONS(boxm2_ocl_render_expected_color_process);
  DECLARE_FUNC_CONS(boxm2_ocl_render_expected_image_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_color_process, "boxm2OclRenderExpectedColorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_image_process, "boxm2OclRenderExpectedImageProcess");
  REGISTER_DATATYPE(boxm2_opencl_cache_sptr);
  REGISTER_DATATYPE(boxm2_scene_sptr);
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(float);
  //
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Has to be done on each render
  //
  //setup image size
  int ni=1280, nj=720;

  //create initial cam (or pass in your own
  double currInc = 45.0;
  double currRadius = scene->bounding_box().height(); //2.0f;
  double currAz = 0.0;
  vpgl_perspective_camera<double>* pcam;
  pcam = boxm2_util::construct_camera(currInc, currAz, currRadius, ni, nj,
                                      scene->bounding_box(), false);
  vpgl_camera_double_sptr cam = new vpgl_perspective_camera<double>(*pcam);

  //set up brdb_value_sptr arguments... (for generic passing)
  brdb_value_sptr brdb_device = new brdb_value_t<bocl_device_sptr>(device);
  brdb_value_sptr brdb_scene = new brdb_value_t<boxm2_scene_sptr>(scene);
  brdb_value_sptr brdb_opencl_cache = new brdb_value_t<boxm2_opencl_cache_sptr>(opencl_cache);
  brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(cam);
  brdb_value_sptr brdb_ni = new brdb_value_t<unsigned>(ni);
  brdb_value_sptr brdb_nj = new brdb_value_t<unsigned>(nj);

  //if scene has RGB data type, use color render process
  bool good = true;
  if (scene->has_data_type(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix()) )
    good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderExpectedColorProcess");
  else
    good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderExpectedImageProcess");

  //set process args
  good = good && bprb_batch_process_manager::instance()->set_input(0, brdb_device) // device
              && bprb_batch_process_manager::instance()->set_input(1, brdb_scene)  //  scene
              && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache)
              && bprb_batch_process_manager::instance()->set_input(3, brdb_cam)    // camera
              && bprb_batch_process_manager::instance()->set_input(4, brdb_ni)     // ni for rendered image
              && bprb_batch_process_manager::instance()->set_input(5, brdb_nj)     // nj for rendered image
              && bprb_batch_process_manager::instance()->run_process();

  //grab vil_image_view_base_sptr from process
  unsigned int out_img = 0;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, out_img);
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, out_img);
  brdb_selection_sptr S = DATABASE->select("vil_image_view_base_sptr_data", Q);
  if (S->size()!=1) {
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " no selections\n";
  }
  brdb_value_sptr value;
  if (!S->get_value(vcl_string("value"), value)) {
    vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " didn't get value\n";
  }
  vil_image_view_base_sptr out_img_sptr =value->val<vil_image_view_base_sptr>();
  vil_save(*out_img_sptr.ptr(), "test_output.tiff");
}

//: Example c++ calls
void test_update_main(boxm2_scene_sptr& scene, bocl_device_sptr& device, boxm2_opencl_cache_sptr& opencl_cache )
{
  //set image and camera directory
  vcl_string imgdir = "/home/acm/data/downtown/imgs";
  vcl_string camdir = "/home/acm/data/downtown/cams";
  vcl_vector<vcl_string> imgs = boxm2_util::images_from_directory(imgdir);
  vcl_vector<vcl_string> cams = boxm2_util::camfiles_from_directory(camdir);
  if (imgs.size() != cams.size()) {
    vcl_cout<<"num(cams) != num(imgs), returning!"<<vcl_endl;
    return;
  }

  //////////////////////////////////////////////////////////////////////////////
  //Only has to be done once
  //
  //register data types and process functions
  DECLARE_FUNC_CONS(boxm2_ocl_update_color_process);
  DECLARE_FUNC_CONS(boxm2_ocl_update_process);
  DECLARE_FUNC_CONS(boxm2_ocl_refine_process);
  DECLARE_FUNC_CONS(boxm2_ocl_filter_process);
  DECLARE_FUNC_CONS(boxm2_write_cache_process);

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_update_color_process, "boxm2OclUpdateColorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_update_process, "boxm2OclUpdateProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_refine_process, "boxm2OclRefineProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_filter_process, "boxm2OclFilterProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_write_cache_process, "boxm2WriteCacheProcess");

  REGISTER_DATATYPE(boxm2_cache_sptr);
  REGISTER_DATATYPE(boxm2_opencl_cache_sptr);
  REGISTER_DATATYPE(boxm2_scene_sptr);
  REGISTER_DATATYPE(bocl_mem_sptr);
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(float);
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Update loop
  //
  vnl_random random(9667566);

  int numLoops = vcl_min((int)imgs.size(), 5);
  for (int i=0; i<numLoops; ++i)
  {
    //grab frame
    int frame = random.lrand32(0, imgs.size()-1);
    vcl_cout<<"updating with image: "<<imgs[frame]<<" and "
            <<" cam: "<<cams[frame]<<vcl_endl;

    //: Load an image resource object from a file.
    vil_image_view_base_sptr in_im = vil_load(imgs[frame].c_str());
    vpgl_camera_double_sptr in_cam = boxm2_util::camera_from_file(cams[frame]);

    //set up brdb_value_sptr arguments...
    brdb_value_sptr brdb_device       = new brdb_value_t<bocl_device_sptr>(device);
    brdb_value_sptr brdb_scene        = new brdb_value_t<boxm2_scene_sptr>(scene);
    brdb_value_sptr brdb_opencl_cache = new brdb_value_t<boxm2_opencl_cache_sptr>(opencl_cache);
    brdb_value_sptr brdb_cam          = new brdb_value_t<vpgl_camera_double_sptr>(in_cam);
    brdb_value_sptr brdb_img          = new brdb_value_t<vil_image_view_base_sptr>(in_im);

    //if scene has RGB data type, use color render process
    bool good =
         scene->has_data_type(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix())
         ? bprb_batch_process_manager::instance()->init_process("boxm2OclUpdateColorProcess")
         : bprb_batch_process_manager::instance()->init_process("boxm2OclUpdateProcess");

    //set process args
    good = good
        && bprb_batch_process_manager::instance()->set_input(0, brdb_device) // device
        && bprb_batch_process_manager::instance()->set_input(1, brdb_scene)  //  scene
        && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache)
        && bprb_batch_process_manager::instance()->set_input(3, brdb_cam)    // camera
        && bprb_batch_process_manager::instance()->set_input(4, brdb_img)    // input image
        && bprb_batch_process_manager::instance()->run_process();
  }
}


void test_process_mains()
{
  // Create scene from file
  vcl_string scene_file = "e:/data/Tailwind1.5/Richmond1/geomodel/scene.xml";
  boxm2_scene_sptr scene = new boxm2_scene(scene_file);

  //make bocl manager (handles a lot of OpenCL stuff)
  bocl_manager_child_sptr mgr = bocl_manager_child::instance();
  bocl_device_sptr device = mgr->gpus_[0];

  //create cpu cache (lru), and create opencl_cache on the device
  boxm2_lru_cache::create(scene);
  boxm2_opencl_cache_sptr opencl_cache = new boxm2_opencl_cache(scene, device, 4);

  //run render and update mains
  test_render_main(scene, device, opencl_cache);
  test_refine_main(scene,device, opencl_cache);
  //test_update_main(scene, device, opencl_cache);
  //test_render_height_main(scene, device, opencl_cache);

  //print database
  bprb_batch_process_manager::instance()->print_db();
}

TESTMAIN( test_process_mains );
