#include <bocl/bocl_cl.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

//executable args
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>

//boxm2 scene stuff
#include <boxm2/io/boxm2_nn_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/cpp/boxm2_cpp_processor.h>
#include <boxm2/cpp/boxm2_cpp_update_process.h>


//brdb stuff
#include <brdb/brdb_value.h>

int main(int argc,  char** argv)
{
  vcl_cout<<"Boxm2 CPP UPDATE(and render) "<<vcl_endl;
  vul_arg<vcl_string> camdir("-camdir", "camera filename", "");
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg<vcl_string> imgdir("-imgdir", "input image", "");

  // need this on some toolkit implementations to get the window up.
  vul_arg_parse(argc, argv);

  boxm2_scene_sptr scene = new boxm2_scene(scene_file());
  brdb_value_sptr brdb_scene_sptr = new brdb_value_t<boxm2_scene_sptr>(scene);
  // get relevant blocks
  boxm2_nn_cache cache( scene.ptr() );
  boxm2_cpp_processor cpp_pro;
  cpp_pro.init();
  cpp_pro.set_scene(scene.ptr());

  boxm2_cpp_update_process cpp_update;
  cpp_update.init();
  cpp_update.set_cache(&cache);

  vcl_string camglob=camdir()+"/*.txt";
  vcl_string imgglob=imgdir()+"/*.*g";

  vul_file_iterator file_it(camglob.c_str());
  vul_file_iterator img_file_it(imgglob.c_str());
  vcl_vector<vcl_string> cam_files;
  vcl_vector<vcl_string> img_files;
  while (file_it && img_file_it) {
    vcl_string camName(file_it());
    vcl_string imgName(img_file_it());
    cam_files.push_back(camName);
    img_files.push_back(imgName);
    ++file_it; ++img_file_it;
  }
  vcl_sort(cam_files.begin(), cam_files.end());
  vcl_sort(img_files.begin(), img_files.end());

  if (cam_files.size() != img_files.size()) {
    vcl_cerr<<"Image files and cam files not one to one\n";
    return -1;
  }

  for(unsigned i=0;i<cam_files.size();i++)
  {
      //load camera from file
      vcl_ifstream ifs(cam_files[i].c_str());
      vpgl_perspective_camera<double>* pcam =new vpgl_perspective_camera<double>;
      if (!ifs.is_open()) {
          vcl_cerr << "Failed to open file " << cam_files[i] << '\n';
          return -1;
      }
      else  {
          ifs >> *pcam;
      }
      vpgl_camera_double_sptr cam = pcam;
      brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(cam);

      //load image from file
      vil_image_view_base_sptr loaded_image = vil_load(img_files[i].c_str());
      vil_image_view<float>* floatimg = new vil_image_view<float>(loaded_image->ni(), loaded_image->nj(), 1);
      if (vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(loaded_image.ptr()))
      {
          vil_convert_stretch_range_limited(*img_byte, *floatimg, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
      }
      else {
          vcl_cerr << "Failed to load image " << img_files[i]<< vcl_endl;
          return -1;
      }

      //create input image buffer
      brdb_value_sptr brdb_inimg = new brdb_value_t<vil_image_view_base_sptr>(floatimg);

      vcl_vector<brdb_value_sptr> input;
      input.push_back(brdb_scene_sptr);
      input.push_back(brdb_cam);
      input.push_back(brdb_inimg);

      //init output vector
      vcl_vector<brdb_value_sptr> output;

      vul_timer t;
      t.mark();
      cpp_pro.run(&cpp_update, input, output);
      vcl_cout<<"Time taken is :" <<t.all()<<vcl_endl;
  }  
  cpp_pro.finish();

  //save blocks and data to disk for debugging
  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator iter;
  for(iter = blocks.begin(); iter != blocks.end(); ++iter)
  { 
    boxm2_block_id id = iter->first; 
    boxm2_sio_mgr::save_block(scene->data_path(), cache.get_block(id)); 
    boxm2_sio_mgr::save_block_data(scene->data_path(), id, cache.get_data<BOXM2_ALPHA>(id) );
    boxm2_sio_mgr::save_block_data(scene->data_path(), id, cache.get_data<BOXM2_MOG3_GREY>(id) );
    boxm2_sio_mgr::save_block_data(scene->data_path(), id, cache.get_data<BOXM2_NUM_OBS>(id) );
  }
  return 0;
}
