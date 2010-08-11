//    Volume update and rendering sample
//    This sample loads a 3D volume
// Utilities, OpenCL and system includes
#include <GL/glew.h>
#include <boxm/ocl/boxm_ocl_scene.h>

//tableau includes
#include <boxm/ocl/view/boxm_ocl_update_tableau.h>
#include <boxm/ocl/view/boxm_ocl_update_tableau_sptr.h>

//camera image includes
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>

//vul includes
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>

//vgui includes
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_clear_tableau.h>
#include <vgui/vgui_easy3D_tableau.h>
#include <vgui/vgui_viewer3D_tableau.h>

#include <vcl_vector.h>

int main(int argc, char ** argv)
{
  //init vgui (should choose/determine toolkit)
  vgui::init(argc, argv);

  // handle arguments
  vcl_cout<<"OCL SCENE UPDATE"<<vcl_endl;
  vul_arg<vcl_string> cam_dir("-camdir", "camera directory", "");
  vul_arg<vcl_string> img_dir("-imgdir", "Image directory", "");
  vul_arg<vcl_string> scene_file("-scene", "scene filename", ""); //ocl_scene xml file
  vul_arg<vcl_string> camfile("-initcam", "initial camera filename", "");
  vul_arg<unsigned> ni("-ni", "Width of image", 640);
  vul_arg<unsigned> nj("-nj", "Height of image", 480);
  vul_arg<float> prob_thresh("-prob", "Refine probability", 0.3);
  vul_arg_parse(argc, argv);
  if (!vul_file::is_directory(cam_dir().c_str()))
    return -1;

  //create ocl_scene from xml file
  boxm_ocl_scene ocl_scene(scene_file());

  //create initial cam
  vcl_ifstream ifs(camfile().c_str());
  vpgl_perspective_camera<double>* pcam = new vpgl_perspective_camera<double>;
  if (!ifs.is_open()) {
    vcl_cerr << "Failed to open file " << camfile() << '\n';
    return -1;
  }
  else
    ifs >> *pcam;

  //get all of the cam and image files, sort them
  vcl_string camglob=cam_dir()+"/*.txt";
  vcl_string imgglob=img_dir()+"/*.jpg";
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


  //create and initialize update_tableau
  boxm_ocl_update_tableau_new update_tableau;
  GLboolean bGLEW = glewIsSupported("GL_VERSION_2_0  GL_ARB_pixel_buffer_object");

  update_tableau->init(&ocl_scene, ni(), nj(), pcam, cam_files, img_files,prob_thresh());

  vgui_window* win = vgui::produce_window(ni(), nj(), "OpenCl Volume Visualizer");
  win->get_adaptor()->set_tableau( update_tableau  ); 
  update_tableau->set_statusbar(win->get_statusbar());
  win->show();

  return  vgui::run();
}
