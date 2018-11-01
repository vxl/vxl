
#include <sstream>
#include <iostream>
#include <cstdlib>
#include "bwm_site_process.h"
//
#include <bwm/algo/bwm_algo.h>
#include <bwm/io/bwm_site.h>
#include <bwm/io/bwm_io_structs.h>

#include <vul/vul_file.h>
#include <vil/vil_image_list.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vgui/vgui_error_dialog.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

double bwm_site_process::version_num = 0;

template<typename T> std::string toString(const T& t)
{
  std::stringstream strm;

  strm << std::fixed << t;
  std::string str(strm.str());
  return str;
}

std::string escape_space(std::string str)
{
  std::string s = "";
  for (unsigned i=0; i<str.size(); i++) {
   if (str[i] == ' ')
     s += "^ ";
   else if (str[i] == '\\')
     s += "\\";
   else
     s += str[i];
  }
  return s;
}

std::string make_quoted(std::string str)
{
  std::string::size_type idx = str.find_first_of("\"", 0);
  if (idx == std::string::npos) {
    idx = str.find_first_of("\\", 0);
    str.insert(idx, "\"");
    str.append("\"");
  }
  return str;
}

void bwm_site_process::RunBackgroundTask()
{
  if (!site_)
    return;

  std::vector<std::string> files = site_->image_paths_;
  std::vector<bool> pyr = site_->pyr_;
  std::vector<bool> act = site_->act_;
  std::vector<std::string> levels = site_->pyr_levels_;
  std::vector<std::pair<std::string, std::string> > obj_paths;
  site_->objects(obj_paths);
  std::string site_dir = site_->path_;
  std::string site_name = site_->name_;
  std::string pyr_exe = site_->pyr_exe_path_;

  // check if pyramid exe is valid
  if (!vul_file::exists(pyr_exe)) {
    vgui_error_dialog("Pyramid exacutable path is not valid");
    return;
  }

  // delete the folders that are not in the site anymore
  for (unsigned i=0; i<site_->remove_.size(); i++)
  {
    unsigned idx = site_->remove_[i];
    if (idx < site_->tableaus_.size())
    {
      // remove the folder first
      std::string img_path = "";
      std::string tab_type = site_->tableaus_[idx]->type_name;
      if (tab_type.compare(CAMERA_TABLEAU_TAG) == 0) {
        bwm_io_tab_config_cam* tab = (bwm_io_tab_config_cam*) site_->tableaus_[idx];
        img_path = tab->img_path;
      }
      else if (tab_type == IMAGE_TABLEAU_TAG) {
        bwm_io_tab_config_img* tab = (bwm_io_tab_config_img*) site_->tableaus_[idx];
        img_path = tab->img_path;
      }
      if (img_path.size() > 0) {
        // if it is under site dir delete it,
        // if not find the folder created for it and then delete it
        if (img_path.find(site_dir) == std::string::npos) {
          std::string base = vul_file::basename(img_path);
          base = vul_file::strip_extension(base);
          img_path = site_dir + "\\" + base;
        }
        if (std::system(NULL)) {
          std::cout << "Deleting the image from the site...\n";
          std::string command = "rd /q /s \"" + img_path + "\"";
          if (std::system(command.data()) != 0)
            std::cerr << "An error occurred while removing the folder\n";
        }
      }
      delete site_->tableaus_[idx];
      site_->tableaus_[idx] = 0;
    }
  }

  // clean up the deleted tableaux
  std::vector<bwm_io_tab_config*>::iterator it = site_->tableaus_.begin();
  while (it != site_->tableaus_.end()) {
    if (*it == 0) {
      site_->tableaus_.erase(it);
      it = site_->tableaus_.begin();
    }
    else {
      it++;
    }
  }
  site_->remove_.clear();

  // create the new site items
  for (unsigned i=0; i<files.size(); i++)
  {
    std::string img = files[i];
    if (img.size() > 0)
    {
      if (vul_file::exists(img))
      {
        std::string img_path;
        std::string base = vul_file::basename(img);
        std::string ext = vul_file::extension(img);
        base = vul_file::strip_extension(base);
        std::string new_site = site_dir + "\\" + base;
        vul_file::make_directory(new_site);
        std::string cam_img_file = "";

        // the image is already a pyramid
        if (!pyr[i])
        {
          if (!vul_file::is_directory(img))
            std::cerr << "Error: the pyramid should be a directory not an image\n";
          else {
            img_path = img;

            // find the image under this folder to be able to compute nitf camera from
            vil_image_list list(img.c_str());
            std::vector<std::string> f_list = list.files();
            bool found = false;
            for (unsigned f=0; f<f_list.size(); f++) {
              if (f_list[f].find_last_of("_0.") != std::string::npos) {
                cam_img_file = f_list[f];
                found = true;
                break;
              }
            }
            //cam_img_file = img + "\\" + base + "_0.nitf";
            //if (!vul_file::exists(cam_img_file))
            if (!found) {
              std::cerr << "The image file [" << cam_img_file << "] cannot be found!\n";
              continue;
            }
          }
        }

        // if the image is not a pyramid, create one
        else if (pyr[i]) // means create a pyramid for this image
        {
          // check if it is a dir or file, it should be file
          if (!vul_file::is_directory(img))
          {
            // 1. create a folder in the site dir
            std::string new_img_file = base + "_0" + ext;
            img_path = new_site + "\\" + new_img_file;
            cam_img_file = img_path;
            //2. copy the image
            if (std::system(NULL)) {
              std::cout << "Copying the image to the site...\n";
              std::string command = "copy \""+ img + "\" \"" + img_path + "\"";
              std::cout << '[' << command << ']' << std::endl;
              std::system(command.c_str());
            }

            // 4. create the pyramid
            std::cout << "Checking if processor is available...";
            if (std::system(NULL)) {
              std::cout << "Executing pyramid creation...\n";
              pyr_exe = make_quoted(pyr_exe);
              std::string command =  pyr_exe + " \"" + new_site + "\" " + ext.substr(1, ext.size()-1) + " " + levels[i];
              std::cout << '[' << command << ']' << std::endl;
              std::system(command.data());
              // set the image path to the directory where pyramid resides
              img_path = new_site;
            }
          }
        }

        // 3. extract the camera
        vpgl_rational_camera<double> *cam = bwm_algo::extract_nitf_camera(cam_img_file);
        std::string cam_path="";

        // create the tableau elements
        if (cam) {
          std::string cam_dir = new_site + "\\camera";
          cam_path = cam_dir+"\\"+base+".RPB";
          vul_file::make_directory(cam_dir);
          cam->save(cam_path);
          bwm_io_tab_config_cam* cam_tab = new bwm_io_tab_config_cam(base, act[i], img_path, cam_path, "rational");
          site_->tableaus_.push_back(cam_tab);
        }
        else
        {
#if 0
          bwm_io_tab_config_img* img_tab = new bwm_io_tab_config_img(IMAGE_TABLEAU_TAG,
                                                                     base, act[i], img_path);
          site_->tableaus_.push_back(img_tab);
#endif // 0
          // do not create a tableau for now
          std::cerr <<  "\nNITF camera is not successfully created, not creating the tableau!\n";
        }
      }
    }
  }

  // create a version number for the xml
  long time = timer_.real();
  std::string site_xml = site_dir + "\\" + site_name + "_v" + toString(time) + ".xml";
  std::ofstream s(site_xml.data());
  site_->x_write(s);
  vgui_error_dialog(("........SITE \"" + site_name + "\" IS READY TO USE.........").c_str());
  return;
}
