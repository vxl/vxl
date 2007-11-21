#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "bwm_site_process.h"

#include "bwm/algo/bwm_algo.h"
#include "bwm/io/bwm_io_structs.h"

#include <vul/vul_file.h>
#include <vpgl/vpgl_rational_camera.h>

void bwm_site_process::RunBackgroundTask()
{
  if (!site_)
    return;

  vcl_vector<vcl_string> files = site_->image_paths_;
  vcl_vector<bool> pyr = site_->pyr_;
  vcl_vector<bool> act = site_->act_;
  vcl_vector<vcl_string> levels = site_->pyr_levels_;
  vcl_vector<vcl_pair<vcl_string, vcl_string> > obj_paths = site_->objects_;
  vcl_string site_dir = site_->path_;
  vcl_string site_name = site_->name_;

  for (unsigned i=0; i<files.size(); i++) {
    vcl_string img = files[i];
    if (img.size() > 0) {
      if (vul_file::exists(img)) {
        vcl_string img_path;
        vcl_string base = vul_file::basename(img);
        vcl_string ext = vul_file::extension(img);
        base = vul_file::strip_extension(base);
        vcl_string new_site = site_dir + "\\" + base;             
        vul_file::make_directory(new_site);
        vcl_string cam_img_file = "";

        // the image is already a pyramid
        if (pyr[i]) {
          if (!vul_file::is_directory(img)) 
            vcl_cout << "Error: the pyramid should be a directory not an image" << vcl_endl;
          else {
            img_path = img;

            // find the image under this folder to be able to compute nitf camera from
            cam_img_file = img + "//" + base + "_0.nitf";
            if (!vul_file::exists(cam_img_file))
              vcl_cout << "The image file [" << cam_img_file << "] cannot be found!" << vcl_endl;
          } 
        }

        // if the image is not a pyramid, create one
        else if (!pyr[i]) { // means create a pyramid for this image
          // check if it is a dir or file, it should be file
          if (!vul_file::is_directory(img)) {

            // 1. create a folder in the site dir
            vcl_string new_img_file = base + "_0" + ext;
            img_path = new_site + "\\" + new_img_file;
            cam_img_file = img_path;
            //2. copy the image
            if (system(NULL)) {
              printf ("Copying the image to the site...\n");
              vcl_string command = "copy "+ img + " " + img_path;
              system(command.data());
            }
            
            // 4. create the pyramid            
            printf ("Checking if processor is available...");
            if (system(NULL)) {
              printf ("Executing pyramid creation...\n");
              vcl_string command = "C:\\lems\\vxl\\build\\contrib\\brl\\bseg\\sbin\\release\\generate_tiff_rset.exe "+
                new_site + " " + ext.substr(1, ext.size()-1) + " " + levels[i];
              system(command.data());
              // set the image path to the directory where pyramid resides
              img_path = new_site;
            }
          }
        }

        // 3. extract the camera
        vpgl_rational_camera<double> *cam = bwm_algo::extract_nitf_camera(cam_img_file);
        vcl_string cam_path="";

        // create the tableau elements
        if (cam) {
          vcl_string cam_dir = new_site + "\\camera";
          cam_path = cam_dir+"\\"+base+".RPB";
          vul_file::make_directory(cam_dir);
          cam->save(cam_path);
          bwm_io_tab_config_cam* cam_tab = new bwm_io_tab_config_cam("CameraTableau",
              base, act[i], img_path, cam_path, "rational"); 
          site_->tableaus_.push_back(cam_tab);  
        } else {
          bwm_io_tab_config_img* img_tab = new bwm_io_tab_config_img("ImageTableau",
           base, act[i], img_path);
        }    
      }
    }
  }


  vcl_string site_xml = site_dir + "\\" + site_name + ".xml";
  vcl_ofstream s(site_xml.data());
  site_->x_write(s);
  return;
}