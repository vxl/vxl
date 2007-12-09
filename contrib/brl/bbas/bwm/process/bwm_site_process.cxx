#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "bwm_site_process.h"
#include "bwm/algo/bwm_algo.h"
#include "bwm/bwm_site.h"
#include "bwm/io/bwm_io_structs.h"

#include <vul/vul_file.h>
#include <vil/vil_image_list.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vgui/vgui_dialog_extensions.h>
#include <vgui/vgui_error_dialog.h>

double bwm_site_process::version_num = 0;

template<typename T> vcl_string toString(const T& t)
{
  vcl_stringstream strm;

  strm << vcl_fixed << t;
  vcl_string str(strm.str());
  return str;
}

vcl_string escape_space(vcl_string str)
{
  vcl_string s = "";
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

vcl_string make_quoted(vcl_string str)
{
  unsigned idx = str.find_first_of("\"", 0);
  if (idx == vcl_string::npos) {
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

  vcl_vector<vcl_string> files = site_->image_paths_;
  vcl_vector<bool> pyr = site_->pyr_;
  vcl_vector<bool> act = site_->act_;
  vcl_vector<vcl_string> levels = site_->pyr_levels_;
  vcl_vector<vcl_pair<vcl_string, vcl_string> > obj_paths;
  site_->objects(obj_paths);
  vcl_string site_dir = site_->path_;
  vcl_string site_name = site_->name_;
  vcl_string pyr_exe = site_->pyr_exe_path_;

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
      vcl_string img_path = "";
      vcl_string tab_type = site_->tableaus_[idx]->type_name;
      if (tab_type.compare(CAMERA_TABLEAU_TAG) == 0) {
        bwm_io_tab_config_cam* tab = (bwm_io_tab_config_cam*) site_->tableaus_[idx];
        img_path = tab->img_path;
      } else if (tab_type == IMAGE_TABLEAU_TAG) {
        bwm_io_tab_config_img* tab = (bwm_io_tab_config_img*) site_->tableaus_[idx];
        img_path = tab->img_path;
      }
      if (img_path.size() > 0) {
        // if it is under site dir delete it,
        // if not find the folder created for it and then delete it
        if (img_path.find(site_dir) == vcl_string::npos) {
          vcl_string base = vul_file::basename(img_path);
          base = vul_file::strip_extension(base);
          img_path = site_dir + "\\" + base;
        }
        if (system(NULL)) {
          vcl_cout << "Deleting the image from the site...\n";
          vcl_string command = "rd /q /s \"" + img_path + "\"";
          if (system(command.data()) != 0)
            vcl_cerr << "An error occured while removing the folder\n";
        }
      }
      delete site_->tableaus_[idx];
      site_->tableaus_[idx] = 0;
    }
  }

  // clean up the deleted tableaux
  vcl_vector<bwm_io_tab_config*>::iterator it = site_->tableaus_.begin();
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
    vcl_string img = files[i];
    if (img.size() > 0)
    {
      if (vul_file::exists(img))
      {
        vcl_string img_path;
        vcl_string base = vul_file::basename(img);
        vcl_string ext = vul_file::extension(img);
        base = vul_file::strip_extension(base);
        vcl_string new_site = site_dir + "\\" + base;
        vul_file::make_directory(new_site);
        vcl_string cam_img_file = "";

        // the image is already a pyramid
        if (!pyr[i])
        {
          if (!vul_file::is_directory(img))
            vcl_cerr << "Error: the pyramid should be a directory not an image\n";
          else {
            img_path = img;

            // find the image under this folder to be able to compute nitf camera from
            vil_image_list list(img.c_str());
            vcl_vector<vcl_string> f_list = list.files();
            bool found = false;
            for (unsigned f=0; f<f_list.size(); f++) {
              if (f_list[f].find_last_of("_0.") != vcl_string::npos) {
                cam_img_file = f_list[f];
                found = true;
                break;
              }
            }
            //cam_img_file = img + "\\" + base + "_0.nitf";
            //if (!vul_file::exists(cam_img_file))
            if (!found) {
              vcl_cerr << "The image file [" << cam_img_file << "] cannot be found!\n";
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
            vcl_string new_img_file = base + "_0" + ext;
            img_path = new_site + "\\" + new_img_file;
            cam_img_file = img_path;
            //2. copy the image
            if (system(NULL)) {
              vcl_cout << "Copying the image to the site...\n";
              vcl_string command = "copy \""+ img + "\" \"" + img_path + "\"";
              vcl_cout << '[' << command << ']' << vcl_endl;
              system(command.c_str());
            }

            // 4. create the pyramid
            vcl_cout << "Checking if processor is available...";
            if (system(NULL)) {
              vcl_cout << "Executing pyramid creation...\n";
              pyr_exe = make_quoted(pyr_exe);
              vcl_string command =  pyr_exe + " \"" + new_site + "\" " + ext.substr(1, ext.size()-1) + " " + levels[i];
              vcl_cout << '[' << command << ']' << vcl_endl;
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
          bwm_io_tab_config_cam* cam_tab = new bwm_io_tab_config_cam(CAMERA_TABLEAU_TAG,
              base, act[i], img_path, cam_path, "rational");
          site_->tableaus_.push_back(cam_tab);
        }
        else
        {
#if 0
          bwm_io_tab_config_img* img_tab = new bwm_io_tab_config_img(IMAGE_TABLEAU_TAG,
                                                                     base, act[i], img_path);
          site_->tableaus_.push_back(img_tab);
#endif // 0
          // do not create a tableu for now
          vcl_cerr <<  "\nNITF camera is not successfully created, not creating the tableau!\n";
        }
      }
    }
  }

  // create a version number for the xml
  long time = timer_.real();
  vcl_string site_xml = site_dir + "\\" + site_name + "_v" + toString(time) + ".xml";
  vcl_ofstream s(site_xml.data());
  site_->x_write(s);
  vgui_error_dialog(("........SITE \"" + site_name + "\" IS READY TO USE.........").c_str());
  return;
}
