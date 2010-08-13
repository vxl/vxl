//    Volume change detection
//    This sample loads a 3D volume
// Utilities, OpenCL and system includes
#include <GL/glew.h>
#include <boxm/ocl/boxm_ocl_scene.h>

//tableau includes
#include <boxm/ocl/view/boxm_ocl_change_detection_tableau.h>
#include <boxm/ocl/view/boxm_ocl_change_detection_tableau_sptr.h>

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
#include <bsta/bsta_histogram.h>

int main(int argc, char ** argv)
{
  //init vgui (should choose/determine toolkit)
  vgui::init(argc, argv);

  // handle arguments
  vcl_cout<<"OCL Change Detection "<<vcl_endl;
  vul_arg<vcl_string> cam_dir("-camdir", "camera directory", "");
  vul_arg<vcl_string> img_dir("-imgdir", "Image directory", "");
  vul_arg<vcl_string> scene_file("-scene", "scene filename", ""); //ocl_scene xml file
  vul_arg<vcl_string> hist_file("-foregroundhist", "foreground histogram", ""); //ocl_scene xml file
  vul_arg<vcl_string> save_img_dir("-saveimgdir", "Save image directory", ""); //ocl_scene xml file

  vul_arg_parse(argc, argv);
  if (!vul_file::is_directory(cam_dir().c_str()))
    return -1;

  //create ocl_scene from xml file
  boxm_ocl_scene ocl_scene(scene_file());

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
  boxm_ocl_change_detection_tableau_new change_detection_tableau;
  GLboolean bGLEW = glewIsSupported("GL_VERSION_2_0  GL_ARB_pixel_buffer_object");

  bsta_histogram<float> hist(0.0f,1.0f,20);

  vcl_ifstream ifile(hist_file().c_str());
  vcl_vector<float> pdf;
  if(!ifile)
  {
      vcl_cout<<"Failed to open a histogram: will assume uniform distriubtion"<<vcl_endl;
      for(unsigned i=0;i<hist.nbins();i++)
          pdf.push_back(1.0f);
  }
  else
  {
      hist.read(ifile);
      float area=hist.area();
      vcl_cout<<"Area = "<<area<<vcl_endl;
      pdf=hist.count_array();
      for(unsigned i=0;i<hist.nbins();i++)
      {
          pdf[i]=pdf[i]/area*20;
          vcl_cout<<pdf[i]<<" ";
      }
      ifile.close();
  }
  change_detection_tableau->init(&ocl_scene, cam_files, img_files,pdf,save_img_dir());

  return  vgui::run(change_detection_tableau, 640,480);
}
