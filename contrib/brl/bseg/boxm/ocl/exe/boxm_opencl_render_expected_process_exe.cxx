#include <vul/vul_arg.h>

#include <boxm/ocl/pro/boxm_ocl_register.h>
#include <boxm/pro/boxm_register.h>
#include <bpro/core/vpgl_pro/vpgl_register.h>
#include <bpro/core/vil_pro/vil_register.h>
#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
int main(int argc, char ** argv)
{
  vcl_cout<<"Render Expected Image "<<vcl_endl;
  vul_arg<vcl_string> cam_file("-cam", "camera filename", "");
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg<vcl_string> img_outfile("-img", "image filename", "");
  vul_arg<unsigned> ni("-ni", "Width of image", 640);
  vul_arg<unsigned> nj("-nj", "Height of image", 480);
  vul_arg<unsigned> option("-option", "Option", 0);

  vul_arg_parse(argc, argv);

  boxm_ocl_register::register_process();
  boxm_register::register_process();
  vil_register::register_process();
  vpgl_register::register_process();

  boxm_ocl_register::register_datatype();
  boxm_register::register_datatype();
  vil_register::register_datatype();
  vpgl_register::register_datatype();

  brdb_value_sptr brdb_scene_file = new brdb_value_t<vcl_string>(scene_file());
  bool good = bprb_batch_process_manager::instance()->init_process("boxmCreateSceneProcess");
  good = good && bprb_batch_process_manager::instance()->set_input(0, brdb_scene_file);
  good = good && bprb_batch_process_manager::instance()->run_process();
  unsigned  id_scene;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_scene);

  brdb_value_sptr brdb_cam_file = new brdb_value_t<vcl_string>(cam_file());
  good = bprb_batch_process_manager::instance()->init_process("vpglLoadPerspectiveCameraProcess");
  good = good && bprb_batch_process_manager::instance()->set_input(0, brdb_cam_file);
  good = good && bprb_batch_process_manager::instance()->run_process();
  unsigned  id_cam;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_cam);

  brdb_value_sptr brdb_ni = new brdb_value_t<unsigned>(ni());
  brdb_value_sptr brdb_nj = new brdb_value_t<unsigned>(nj());
  brdb_value_sptr brdb_black_background = new brdb_value_t<bool>(0);

  if (option()==0)
    good = bprb_batch_process_manager::instance()->init_process("boxmOclRenderExpectedProcess");
  if (option()==1)
    good = bprb_batch_process_manager::instance()->init_process("boxmOclRayBundleExpectedProcess");
  if (option()==2)
    good = bprb_batch_process_manager::instance()->init_process("boxmOclStackRenderExpectedProcess");
  if (option()==3)
    good = bprb_batch_process_manager::instance()->init_process("boxmOclRenderAllBlocksExpectedProcess");

  good = good && bprb_batch_process_manager::instance()->set_input_from_db(0, id_scene);
  good = good && bprb_batch_process_manager::instance()->set_input_from_db(1, id_cam);
  good = good && bprb_batch_process_manager::instance()->set_input(2, brdb_ni);
  good = good && bprb_batch_process_manager::instance()->set_input(3, brdb_nj);
  good = good && bprb_batch_process_manager::instance()->set_input(4, brdb_black_background);
  good = good && bprb_batch_process_manager::instance()->run_process();
  unsigned  id_img;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_img);
  if (img_outfile()!="")
  {
    brdb_value_sptr brdb_img_file = new brdb_value_t<vcl_string>(img_outfile());
    good = bprb_batch_process_manager::instance()->init_process("vilSaveImageViewProcess");
    good = good && bprb_batch_process_manager::instance()->set_input_from_db(0, id_img);
    good = good && bprb_batch_process_manager::instance()->set_input(1, brdb_img_file);
    good = good && bprb_batch_process_manager::instance()->run_process();
  }
}

