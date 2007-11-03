/* \file contrib/brl/bwm/io/bwm_io_config_parser.cxx
  Parses the configuration file for bwm tool.
*/
#include "bwm_io_config_parser.h"

#include <vcl_cstdio.h>
#include <vcl_cassert.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>

//#include <vcl_fstream.h>

//-----
//--- PARSER --
//--
template <typename T>
void convert(const char* t, T& d)
{
  vcl_stringstream strm(t);

  strm >> d;

}

bwm_io_config_parser::bwm_io_config_parser() 
{
  init_params();
}


void bwm_io_config_parser::init_params()
{
  name_ = "";
  image_path_ = "";
  camera_path_ = "";
  camera_type_ = "";
}

void 
bwm_io_config_parser ::WriteIndent()
{
//for (int i = 0; i < mDepth; i++)
//  putchar('\t');
}
void 
bwm_io_config_parser ::cdataHandler(vcl_string name, vcl_string data)
{
  if (name.compare("imagePath") == 0 ) {
    image_path_.assign(data);
    trim_string(image_path_);
  } else if (name.compare("cameraPath") == 0 ) {
    camera_path_.assign(data);
    trim_string(camera_path_);
  } else if (name.compare("corr_camera_tab") == 0 ) {
    corr_cam_tab_.assign(data);
    trim_string(corr_cam_tab_);
  }
  
   cdata = ""; 
}

void 
bwm_io_config_parser::handleAtts(const XML_Char** atts)
{

}

void 
bwm_io_config_parser::startElement(const char* name, const char** atts)
{
 
  vcl_cout<< "element=" << name << vcl_endl; 
  
  if ((strcmp(name,"ImageTableau")== 0) ||
    (strcmp(name,"CameraTableau") == 0) ||
    (strcmp(name,"Coin3DTableau") == 0) || 
    (strcmp(name,"Proj2DTableau") == 0)) {
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (strcmp(atts[i], "name") == 0)
        convert(atts[i+1], name_);
    }
  } else if (strcmp(name,"cameraPath")== 0) {
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (strcmp(atts[i], "type") == 0)
        convert(atts[i+1], camera_type_);
    }
  } else if (strcmp(name, "correspondences") == 0) {
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (strcmp(atts[i], "mode") == 0)
        convert(atts[i+1], corr_mode_);
    }
  } else if (strcmp(name, "corr_point") == 0) {
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (strcmp(atts[i], "X") == 0)
        convert(atts[i+1], X_);
      else if (strcmp(atts[i], "Y") == 0)
        convert(atts[i+1], Y_);
    }
  } else if (strcmp(name, "corr_world_point") == 0) {
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (strcmp(atts[i], "X") == 0)
        convert(atts[i+1], X_);
      else if (strcmp(atts[i], "Y") == 0)
        convert(atts[i+1], Y_);
      else if (strcmp(atts[i], "Z") == 0)
        convert(atts[i+1], Z_);
    }
  }
}


void 
bwm_io_config_parser::endElement(const char* name)
{
   // first check if the last element has some cdata
   if (cdata.size() > 0) {
     cdataHandler(name, cdata);
     cdata= "";
   }

   if (strcmp(name, "ImageTableau") == 0) {
     bwm_io_tab_config_img* img = new bwm_io_tab_config_img(name, name_, image_path_);
     tableaus_.push_back(img);
     init_params();
   } else if (strcmp(name, "CameraTableau") == 0) {
     bwm_io_tab_config_cam* cam = new bwm_io_tab_config_cam(name, name_, image_path_, camera_path_, camera_type_);
     tableaus_.push_back(cam);
     init_params();
   } else if (strcmp(name, "Coin3DTableau") == 0) {
     bwm_io_tab_config_coin3d* coin3d = new bwm_io_tab_config_coin3d(name, name_, camera_path_, camera_type_);
     tableaus_.push_back(coin3d);
     init_params();
   } else if (strcmp(name, "Proj2DTableau") == 0) {
     bwm_io_tab_config_proj2d* proj2d = new bwm_io_tab_config_proj2d(name, name_, proj2d_type_, camera_path_, camera_type_, coin3d_name_);
   } else if (strcmp(name, "corr_elm") == 0) {
     corresp_elm_.push_back(vcl_pair<vcl_string, vsol_point_2d> (corr_cam_tab_, vsol_point_2d(X_,Y_)));
     corr_cam_tab_ = "";
   } else if (strcmp(name, "corr_world_point") == 0) {
     corresp_world_pts_.push_back(vsol_point_3d(X_, Y_, Z_));
   } else if (strcmp(name, "correspondence") == 0) {
     corresp_.push_back(corresp_elm_);
     corresp_elm_.clear();
   }
}

void bwm_io_config_parser::charData(const XML_Char* s, int len)
{
  const int leadingSpace = skipWhiteSpace(s);
  if (len==0 || len<=leadingSpace)
     return;  // called with whitespace between elements

  putchar('(');
  fwrite(s, len, 1, stdout);
  puts(")");
  cdata.append(s, len);
}

void bwm_io_config_parser::trim_string(vcl_string& s)
{
  int i = s.find_first_not_of(" ");
  int j = s.find_last_not_of(" ");
  vcl_string t = s.substr(i,j-i+1);
  s = t;
}



