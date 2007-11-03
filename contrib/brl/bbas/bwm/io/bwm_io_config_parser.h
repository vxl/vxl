#ifndef bwm_io_config_parser_h_
#define bwm_io_config_parser_h_

#include <expatpp/expatpplib.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_cstdio.h>
#include <vcl_cassert.h>
#include <vcl_vector.h>
#include <vcl_utility.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>

class bwm_io_tab_config 
{
public:
  bwm_io_tab_config(vcl_string type, vcl_string tab_name) : type_name(type), name(tab_name) {}
  vcl_string name;
  vcl_string type_name;
};

struct bwm_io_tab_config_img : public bwm_io_tab_config {
  bwm_io_tab_config_img(vcl_string type, vcl_string name, vcl_string path) 
    : bwm_io_tab_config(type, name), img_path(path) {}
  vcl_string img_path;
};

  
struct bwm_io_tab_config_cam : public bwm_io_tab_config {
  bwm_io_tab_config_cam(vcl_string type, vcl_string name, vcl_string i_path, vcl_string c_path, vcl_string c_type) 
    : bwm_io_tab_config(type, name), img_path(i_path), cam_path(c_path), cam_type(c_type) {}
  vcl_string img_path;
  vcl_string cam_path;
  vcl_string cam_type; 
};

struct bwm_io_tab_config_coin3d: public bwm_io_tab_config {
  bwm_io_tab_config_coin3d(vcl_string type, vcl_string name, vcl_string c_path, vcl_string c_type) 
    : bwm_io_tab_config(type, name), cam_path(c_path), cam_type(c_type) {}
  vcl_string cam_path;
  vcl_string cam_type; 
};

struct bwm_io_tab_config_proj2d: public bwm_io_tab_config {
  bwm_io_tab_config_proj2d(vcl_string type, vcl_string name, vcl_string p_type, vcl_string c_path, vcl_string c_type, vcl_string coin3d) 
    : bwm_io_tab_config(type, name), proj2d_type(p_type), cam_path(c_path), cam_type(c_type), coin3d_tab_name(coin3d) {}
  vcl_string cam_path;
  vcl_string cam_type; 
  vcl_string proj2d_type;
  vcl_string coin3d_tab_name;
};

class bwm_io_config_parser : public expatpp {
public:
  
  bwm_io_config_parser(void);
  ~bwm_io_config_parser(void){};
  vcl_vector<bwm_io_tab_config* > tableau_config() { return tableaus_; }
  vcl_vector<vcl_vector<vcl_pair<vcl_string, vsol_point_2d> > > correspondences() { return corresp_; }
  vcl_string corresp_mode() {return corr_mode_; }
  vcl_vector<vsol_point_3d> corresp_world_pts() {return corresp_world_pts_; }

private:
  virtual void startElement(const XML_Char* name, const XML_Char** atts);
  virtual void endElement(const XML_Char* name);
  virtual void charData(const XML_Char* s, int len);

  void handleAtts(const XML_Char** atts);
  void cdataHandler(vcl_string name, vcl_string data);
  void WriteIndent();
  void init_params();

  //Data
  int mDepth;
  vcl_string cdata;
  vcl_string last_tag;

  vcl_vector<bwm_io_tab_config* > tableaus_;
  vcl_string name_;
  vcl_string image_path_;
  vcl_string camera_path_;
  vcl_string camera_type_;
  vcl_string proj2d_type_;
  vcl_string coin3d_name_;

  // correspondence related parameters
  vcl_string corr_mode_;
  vcl_string corr_cam_tab_;
  double X_, Y_, Z_;
  vcl_vector<vcl_vector<vcl_pair<vcl_string, vsol_point_2d> > > corresp_;
  vcl_vector<vsol_point_3d> corresp_world_pts_;
  vcl_vector<vcl_pair<vcl_string, vsol_point_2d> > corresp_elm_;

  void trim_string(vcl_string& s);
};

#endif
