/* \file contrib/brl/bwm/io/bwm_io_config_parser.cxx
  Parses the configuration file for bwm tool.
*/
#include "bwm_io_config_parser.h"
#include "bwm/bwm_site.h"

#include <vcl_cstdio.h>
#include <vcl_cassert.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>

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
  site_ = new bwm_site(); 
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
  // clean up the empty chars before and after the file paths
  trim_string(data);
  if (name.compare(IMAGE_PATH_TAG) == 0 ) {
    image_path_.assign(data);
  } else if (name.compare(CAMERA_PATH_TAG) == 0 ) {
    camera_path_.assign(data);
  } else if (name.compare(CORR_CAMERA_TAG) == 0 ) {
    corr_cam_tab_.assign(data);
  } else if (name.compare(OBJECT_TAG) == 0 ) {
    object_path_.assign(data);
  } else if (name.compare(SITE_HOME_TAG) == 0) {
    site_->path_.assign(data);
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
  
  if (strcmp(name, SITE_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (strcmp(atts[i], "name") == 0)
        convert(atts[i+1], site_->name_);
      /*else if (strcmp(atts[i], "path") == 0)
        convert(atts[i+1], site_->path_);*/
    }
  }
  else if ((strcmp(name,IMAGE_TABLEAU_TAG)== 0) ||
    (strcmp(name, CAMERA_TABLEAU_TAG) == 0) ||
    (strcmp(name,COIN3D_TABLEAU_TAG) == 0) || 
    (strcmp(name,PROJ2D_TABLEAU_TAG) == 0)) {
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (strcmp(atts[i], "name") == 0)
        convert(atts[i+1], name_);
      else if (strcmp(atts[i], "status") == 0) {
        vcl_string status;
        convert(atts[i+1], status);
        if (status.compare("active") == 0)
          status_ = true;
        else
          status_ = false;
      }
    }
  } else if (strcmp(name,CAMERA_PATH_TAG)== 0) {
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
  } else if (strcmp(name, OBJECT_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (strcmp(atts[i], "type") == 0)
        convert(atts[i+1], object_type_);
    }
  } else if (strcmp(name, LVCS_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (strcmp(atts[i], "lat") == 0)
        convert(atts[i+1], lat_);
      else if (strcmp(atts[i], "lon") == 0)
        convert(atts[i+1], lon_);
      else if (strcmp(atts[i], "elev") == 0)
        convert(atts[i+1], elev_);
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

   if (strcmp(name, IMAGE_TABLEAU_TAG) == 0) {
     bwm_io_tab_config_img* img = new bwm_io_tab_config_img(name, name_, status_, image_path_);
     site_->tableaus_.push_back(img);
     init_params();
   } else if (strcmp(name, CAMERA_TABLEAU_TAG) == 0) {
     bwm_io_tab_config_cam* cam = new bwm_io_tab_config_cam(name, name_, status_, image_path_, camera_path_, camera_type_);
     site_->tableaus_.push_back(cam);
     init_params();
   } else if (strcmp(name, COIN3D_TABLEAU_TAG) == 0) {
     bwm_io_tab_config_coin3d* coin3d = new bwm_io_tab_config_coin3d(name, name_, status_, camera_path_, camera_type_);
     site_->tableaus_.push_back(coin3d);
     init_params();
   } else if (strcmp(name, PROJ2D_TABLEAU_TAG) == 0) {
     bwm_io_tab_config_proj2d* proj2d = new bwm_io_tab_config_proj2d(name, name_, status_, proj2d_type_, camera_path_, camera_type_, coin3d_name_);
   } else if (strcmp(name, "corr_elm") == 0) {
     corresp_elm_.push_back(vcl_pair<vcl_string, vsol_point_2d> (corr_cam_tab_, vsol_point_2d(X_,Y_)));
     corr_cam_tab_ = "";
   } else if (strcmp(name, "corr_world_point") == 0) {
     corresp_world_pts_.push_back(vsol_point_3d(X_, Y_, Z_));
   } else if (strcmp(name, "correspondence") == 0) {
     corresp_.push_back(corresp_elm_);
     corresp_elm_.clear();
   } else if (strcmp(name, OBJECT_TAG) == 0) {
     vcl_pair<vcl_string, vcl_string> object(object_path_, object_type_);
     site_->objects_.push_back(object);
   } else if (strcmp(name, LVCS_TAG) == 0) {
     site_->lvcs_ = new vsol_point_3d(lat_, lon_, elev_);
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



