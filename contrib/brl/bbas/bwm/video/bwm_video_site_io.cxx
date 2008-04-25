//:
// \file
// \brief Parses the configuration file for correspondences
//
#include "bwm_video_site_io.h"
#include "bwm_video_corr.h"
#include <vsl/vsl_basic_xml_element.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h>
#include <vcl_cstring.h>
// --------------
// --- PARSER ---
// --------------
template <typename T>
void convert(const char* t, T& d)
{
  vcl_stringstream strm(t);
  strm >> d;
}

bwm_video_site_io::bwm_video_site_io()
{
  init_params();
}

void bwm_video_site_io::init_params()
{
  name_ = "";
  site_dir_ = "";
  video_path_ = "";
  camera_path_ = "";
  corr_ = 0;
}

void bwm_video_site_io::clear()
{
  name_ = "";
  site_dir_ = "";
  video_path_ = "";
  camera_path_ = "";
  corr_ = 0;
  corrs_.clear();
}

bool bwm_video_site_io::open(vcl_string const& xml_path)
{
  if (xml_path.size() == 0)
    return false;
  this->clear();
  vcl_FILE* xmlFile = vcl_fopen(xml_path.c_str(), "r");
  if (!xmlFile){
    vcl_cerr << xml_path << " error on opening\n";
    return false;
  }
  if (!this->parseFile(xmlFile)) {
    vcl_cerr << XML_ErrorString(this->XML_GetErrorCode()) << " at line "
             << this->XML_GetCurrentLineNumber() << vcl_endl;
    return false;
  }
  return true;
}

void
bwm_video_site_io ::cdataHandler(vcl_string name, vcl_string data)
{

  // clean up the empty chars before and after the file paths
  trim_string(data);
  if (name.compare(VIDEO_PATH_TAG) == 0 ) {
    video_path_.assign(data);
  } else if (name.compare(CAMERA_PATH_TAG) == 0 ) {
    camera_path_.assign(data);
  } else if (name.compare(SITE_DIR_TAG) == 0 ) {
    site_dir_.assign(data);
    cdata = "";
  }
}
void
bwm_video_site_io::handleAtts(const XML_Char** atts)
{
}

void
bwm_video_site_io::startElement(const char* name, const char** atts)
{
  if ((vcl_strcmp(name,VIDEO_SITE)== 0)){
    if (vcl_strcmp(atts[0], "name") == 0)
      convert(atts[1], name_);
  }
  else if (vcl_strcmp(name, VIDEO_PATH_TAG) == 0) {
    if (vcl_strcmp(atts[0], "path") == 0)
      convert(atts[1], video_path_);
  }
  else if (vcl_strcmp(name, CAMERA_PATH_TAG) == 0) {
    if (vcl_strcmp(atts[0], "path") == 0)
      convert(atts[1], camera_path_);
  }
  else if (vcl_strcmp(name, SITE_DIR_TAG) == 0) {
    if (vcl_strcmp(atts[0], "path") == 0)
      convert(atts[1], site_dir_);
  }
  else if (vcl_strcmp(name, CORRESPONDENCES_TAG) == 0) {
    corrs_.clear();
  }
  else if (vcl_strcmp(name, CORRESP_TAG) == 0) {
    corr_ = new bwm_video_corr();
  }
  else if (vcl_strcmp(name, CORRESP_WORLD_PT_TAG) == 0) {
    double X = 0, Y = 0, Z = 0;
    bool success = true;
    if (vcl_strcmp(atts[0], "X") == 0)
      convert(atts[1], X);
    else success = false;
    if (vcl_strcmp(atts[2], "Y") == 0)
      convert(atts[3], Y);
    else success = false;
    if (vcl_strcmp(atts[4], "Z") == 0)
      convert(atts[5], Z);
    else success = false;
    if (success)
      corr_->set_world_pt(vgl_point_3d<double>(X, Y, Z));
  }
  else if (vcl_strcmp(name, CORR_ELE) == 0) {
    unsigned frame=0;
    double u = 0, v = 0;
    bool success = true;
    if (vcl_strcmp(atts[0], "fr") == 0)
      convert(atts[1], frame);
    else success = false;
    if (vcl_strcmp(atts[2], "u") == 0)
      convert(atts[3], u);
    else success = false;
    if (vcl_strcmp(atts[4], "v") == 0)
      convert(atts[5], v);
    else success = false;
    if (success)
      corr_->add(frame, vgl_point_2d<double>(u, v));
  }
}

void
bwm_video_site_io::endElement(const char* name)
{
  // first check if the last element has some cdata
  if (cdata.size() > 0) {
    cdataHandler(name, cdata);
    cdata= "";
  }

  if (vcl_strcmp(name, CORRESP_TAG) == 0){
    corrs_.push_back(corr_);
  }
  else if (vcl_strcmp(name, CORRESPONDENCES_TAG) == 0) {
    //nothing for now
  }
  else if (vcl_strcmp(name, VIDEO_SITE) == 0) {
    //nothing for now
  }
}

void bwm_video_site_io::charData(const XML_Char* s, int len)
{
  const int leadingSpace = skipWhiteSpace(s);
  if (len==0 || len<=leadingSpace)
    return;  // called with whitespace between elements

  vcl_putchar('(');
  vcl_fwrite(s, len, 1, stdout);
  puts(")");
  cdata.append(s, len);
}

void bwm_video_site_io::trim_string(vcl_string& s)
{
  int i = s.find_first_not_of(" ");
  int j = s.find_last_not_of(" ");
  vcl_string t = s.substr(i,j-i+1);
  s = t;
}

//
// =========  xwrite section ========
//

void bwm_video_site_io::
set_corrs(vcl_vector<bwm_video_corr_sptr> const& corrs)
{
  corrs_ = corrs;
}

void bwm_video_site_io::set_video_path(vcl_string const& video_path)
{
  video_path_ = video_path;
}

void bwm_video_site_io::set_camera_path(vcl_string const& camera_path)
{
  camera_path_ = camera_path;
}

void bwm_video_site_io::x_write(vcl_string const& xml_path)
{
  vcl_ofstream os(xml_path.c_str());
  if (!os)
    return;
  vsl_basic_xml_element vsite(VIDEO_SITE);
  vsite.add_attribute("name", name_);
  vsite.x_write_open(os);

  vsl_basic_xml_element vpath(VIDEO_PATH_TAG);
  vpath.add_attribute("path", video_path_);
  vpath.x_write(os);

  vsl_basic_xml_element cpath(CAMERA_PATH_TAG);
  cpath.add_attribute("path", camera_path_);
  cpath.x_write(os);

  vsl_basic_xml_element sdir(SITE_DIR_TAG);
  sdir.add_attribute("path", site_dir_);
  sdir.x_write(os);

  //write the correspondences
  vsl_basic_xml_element corrs(CORRESPONDENCES_TAG);
  corrs.x_write_open(os);
  vcl_vector<bwm_video_corr_sptr >::iterator cit = corrs_.begin();
  for (; cit != corrs_.end(); ++cit)
    (*cit)->x_write(os);
  corrs.x_write_close(os);
  vsite.x_write_close(os);
}
