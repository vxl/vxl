//:
// \file
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <cstddef>
#include "volm_candidate_region_parser.h"
//
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <typename T>
void convert(const char* t, T& d)
{
  std::stringstream strm(t);
  strm >> d;
}

volm_candidate_region_parser::volm_candidate_region_parser()
{
  init_params();
}

void volm_candidate_region_parser::init_params()
{
  longitude_ = 0.0;  latitude_ = 0.0;  altitude_ = 0.0;
  heading_ = 0.0;  heading_dev_ = 0.0;  tilt_ = 0.0;  tilt_dev_ = 0.0;
  top_fov_ = 0.0;  top_fov_dev_ = 0.0;  roll_ = 0.0;  roll_dev_ = 0.0;
  right_fov_ = 0.0;  right_fov_dev_ = 0.0;
  near_ = 0.0;
  polyouter_.clear();  polyinner_.clear();
  linecords_.clear();
  points_.clear();
  last_tag = "";
  cord_tag_ = "";
  current_name_ = "";
}

void volm_candidate_region_parser::cdataHandler(const std::string&  /*name*/, const std::string&  /*data*/)
{
}

void volm_candidate_region_parser::handleAtts(const XML_Char** /*atts*/)
{
}

void volm_candidate_region_parser::startElement(const XML_Char* name, const XML_Char**  /*atts*/)
{
  if (std::strcmp(name, KML_LON_TAG) == 0)
    last_tag = KML_LON_TAG;
  else if (std::strcmp(name, KML_LAT_TAG) == 0)
    last_tag = KML_LAT_TAG;
  else if (std::strcmp(name, KML_ALT_TAG) == 0)
    last_tag = KML_ALT_TAG;
  else if (std::strcmp(name, KML_HEAD_TAG) == 0)
    last_tag = KML_HEAD_TAG;
  else if (std::strcmp(name,KML_HEAD_DEV_TAG) == 0)
    last_tag = KML_HEAD_DEV_TAG;
  else if (std::strcmp(name, KML_TFOV_TAG) == 0)
    last_tag = KML_TFOV_TAG;
  else if (std::strcmp(name, KML_TFOV_DEV_TAG) == 0)
    last_tag = KML_TFOV_DEV_TAG;
  else if (std::strcmp(name, KML_RFOV_TAG) == 0)
    last_tag = KML_RFOV_TAG;
  else if (std::strcmp(name, KML_RFOV_DEV_TAG) == 0)
    last_tag = KML_RFOV_DEV_TAG;
  else if (std::strcmp(name, KML_ROLL_TAG) == 0)
    last_tag = KML_ROLL_TAG;
  else if (std::strcmp(name, KML_ROLL_DEV_TAG) == 0)
    last_tag = KML_ROLL_DEV_TAG;
  else if (std::strcmp(name, KML_TILT_TAG) == 0)
    last_tag = KML_TILT_TAG;
  else if (std::strcmp(name, KML_TILT_DEV_TAG) == 0)
    last_tag = KML_TILT_DEV_TAG;
  else if (std::strcmp(name, KML_NEAR_TAG) == 0)
    last_tag = KML_NEAR_TAG;
  else if (std::strcmp(name, KML_CORD_TAG) == 0 && cord_tag_ != "")
    last_tag = KML_CORD_TAG;
  else if (std::strcmp(name, KML_PLACEMARK_NAME_TAG) == 0)
    last_tag = KML_PLACEMARK_NAME_TAG;
  else if (std::strcmp(name, KML_POLYOB_TAG) == 0)
    cord_tag_ = KML_POLYOB_TAG;
  else if (std::strcmp(name, KML_POLYIB_TAG) == 0)
    cord_tag_ = KML_POLYIB_TAG;
  else if (std::strcmp(name, KML_LINE_TAG) == 0)
    cord_tag_ = KML_LINE_TAG;
  else if (std::strcmp(name, KML_POINT_TAG) == 0)
    cord_tag_ = KML_POINT_TAG;
}

void volm_candidate_region_parser::endElement(const XML_Char*  /*name*/)
{
}

void volm_candidate_region_parser::charData(const XML_Char* s, int len)
{
  const int leadingSpace = skipWhiteSpace(s);
  if (len == 0 || len <= leadingSpace)
    return; // called with whitespace between elements
  if (last_tag == KML_LON_TAG) {
    std::stringstream str;
    for (int i = 0; i < len; i++)
      str << s[i];
    str >> longitude_;
    last_tag = "";
  }
  else if (last_tag == KML_LAT_TAG) {
    std::stringstream str;
    for (int i = 0; i < len; i++)
      str << s[i];
    str >> latitude_;
    last_tag = "";
  }
  else if (last_tag == KML_ALT_TAG) {
    std::stringstream str;
    for (int i = 0; i < len; i++)
      str << s[i];
    str >> altitude_;
    last_tag = "";
  }
  else if (last_tag == KML_PLACEMARK_NAME_TAG) {
    std::stringstream str;
    for (int i = 0; i < len; i++)
      str << s[i];
    current_name_ = str.str();
    last_tag = "";
  }
  else if (last_tag == KML_HEAD_TAG) {
    std::stringstream str;
    for (int i = 0; i < len; i++)
      str << s[i];
    str >> heading_;
    last_tag = "";
  }
  else if (last_tag == KML_HEAD_DEV_TAG) {
    std::stringstream str;
    for (int i = 0; i < len; i++)
      str << s[i];
    str >> heading_dev_;
    last_tag = "";
  }
  else if (last_tag == KML_TILT_TAG) {
    std::stringstream str;
    for (int i = 0; i < len; i++)
      str << s[i];
    str >> tilt_;
    last_tag = "";
  }
  else if (last_tag == KML_TILT_DEV_TAG) {
    std::stringstream str;
    for (int i = 0; i < len; i++)
      str << s[i];
    str >> tilt_dev_;
    last_tag = "";
  }
  else if (last_tag == KML_ROLL_TAG) {
    std::stringstream str;
    for (int i = 0; i < len; i++)
      str << s[i];
    str >> roll_;
    last_tag = "";
  }
  else if (last_tag == KML_ROLL_DEV_TAG) {
    std::stringstream str;
    for (int i = 0; i < len; i++)
      str << s[i];
    str >> roll_dev_;
    last_tag = "";
  }
  else if (last_tag == KML_TFOV_TAG) {
    std::stringstream str;
    for (int i = 0; i < len; i++)
      str << s[i];
    str >> top_fov_;
    last_tag = "";
  }
  else if (last_tag == KML_TFOV_DEV_TAG) {
    std::stringstream str;
    for (int i = 0; i < len; i++)
      str << s[i];
    str >> top_fov_dev_;
    last_tag = "";
  }
  else if (last_tag == KML_RFOV_TAG) {
    std::stringstream str;
    for (int i = 0; i < len; i++)
      str << s[i];
    str >> right_fov_;
    last_tag = "";
  }
  else if (last_tag == KML_RFOV_DEV_TAG) {
    std::stringstream str;
    for (int i = 0; i < len; i++)
      str << s[i];
    str >> right_fov_dev_;
    last_tag = "";
  }
  else if (last_tag == KML_NEAR_TAG) {
    std::stringstream str;
    for (int i = 0; i < len; i++)
      str << s[i];
    str >> near_;
    last_tag = "";
  }
  else if (last_tag == KML_CORD_TAG) {
    // get the coordinate value
    std::stringstream str;
    double x,y,z;
    std::string str_s;
    str_s = s;
    std::size_t cord_end = str_s.find(KML_POLYCORE_END_TAG);

    if (str_s[len] == '<')
      cord_end = len;
    else {
      while (str_s[cord_end] != '\n')
        cord_end--;
      while (str_s[cord_end-1] == ' ')
        cord_end--;
    }
    for (unsigned int i=0; i<cord_end; ++i)
      str << s[i];
    std::vector<vgl_point_3d<double> > poly_verts;
    while (!str.eof()) {
      str >> x;  str.ignore();
      str >> y;  str.ignore();
      str >> z;  str.ignore(128, ' ');
      vgl_point_3d<double> vpt(x, y, z);
      // check whether same point exists inside the point list already
      if (std::find(poly_verts.begin(), poly_verts.end(), vpt) == poly_verts.end())
        poly_verts.push_back(vpt);
    }
    // put the coordinates value into different polygons

    if (cord_tag_ == KML_POLYOB_TAG) {
      polyouter_[current_name_].push_back(poly_verts);
      region_name_ = current_name_;
    }
    else if (cord_tag_ == KML_POLYIB_TAG) {
      if (region_name_.compare("") == 0)
        region_name_ = current_name_;
      polyinner_[region_name_].push_back(poly_verts);
    }
    else if (cord_tag_ == KML_LINE_TAG) {
      linecords_[current_name_].push_back(poly_verts);
    }
    else if (cord_tag_ == KML_POINT_TAG) {
      points_[current_name_].push_back(poly_verts[0]);
    }
    else {
      std::cout << "WARNING: unknown shape tag: " << cord_tag_ << " (name: " << current_name_ << ") won't be parsed" << std::endl;
    }
    last_tag = "";
    cord_tag_ = "";
    current_name_ = "";
  }
}

std::vector<vgl_point_3d<double> > volm_candidate_region_parser::parse_points(std::string const& kml_file, std::string const& name)
{
  std::vector<vgl_point_3d<double> > out;
  out.clear();
  auto* parser = new volm_candidate_region_parser();
  std::FILE* xmlFile = std::fopen(kml_file.c_str(), "r");
  if (!xmlFile) {
    std::cerr << "volm_candidate_region_parser: can not open kml file " << kml_file << '\n';
    delete parser;
    return out;
  }
  if (!parser->parseFile(xmlFile)) {
    std::cerr << "volm_candidate_region_parser: " << XML_ErrorString(parser->XML_GetErrorCode()) << " at line "
             <<  parser->XML_GetCurrentLineNumber() << '\n';
    delete parser;
    return out;
  }
  if (parser->points_.find(name) == parser->points_.end()) {
    std::cerr << "volm_candidate_region_parser: no point named " << name << ", empty point returned!\n";
    delete parser;
    return out;
  }
  else {
    out = parser->points_[name];
    delete parser;
    return out;
  }
}

std::vector<std::vector<vgl_point_3d<double> > > volm_candidate_region_parser::parse_lines(std::string const& kml_file, std::string const& name)
{
  std::vector<std::vector<vgl_point_3d<double> > > out_lines;
  out_lines.clear();
  auto* parser = new volm_candidate_region_parser();
  std::FILE* xmlFile = std::fopen(kml_file.c_str(), "r");
  if (!xmlFile) {
    std::cerr << "volm_candidate_region_parser: can not open kml file " << kml_file <<'\n';
    delete parser;
    return out_lines;
  }
  if (!parser->parseFile(xmlFile)) {
    std::cerr << "volm_candidate_region_parser: " << XML_ErrorString(parser->XML_GetErrorCode()) << " at line "
      << parser->XML_GetCurrentLineNumber() << '\n';
    delete parser;
    return out_lines;
  }
  if (parser->linecords_.find(name) == parser->linecords_.end()) {
    std::cerr << "volm_candidate_region_parser: no line named " << name << ", empty line returned!\n";
    delete parser;
    return out_lines;
  }
  else {
    out_lines = parser->linecords_[name];
    delete parser;
    return out_lines;
  }
}

vgl_polygon<double> volm_candidate_region_parser::parse_polygon(std::string const& kml_file, std::string const& name)
{
  vgl_polygon<double> out;
  out.clear();
  auto* parser = new volm_candidate_region_parser;
  std::FILE* xmlFile = std::fopen(kml_file.c_str(), "r");
  if (!xmlFile) {
    std::cerr << "volm_candidate_region_parser: can not open kml file " << kml_file << '\n';
    delete parser;
    return out;
  }
  if (!parser->parseFile(xmlFile)) {
    std::cerr << "volm_candidate_region_parser: " << XML_ErrorString(parser->XML_GetErrorCode()) << " at line "
             <<  parser->XML_GetCurrentLineNumber() << '\n';
    delete parser;
    return out;
  }
  if (parser->polyouter_.find(name) == parser->polyouter_.end()) {
    std::cerr << "volm_candidate_region_parser: no polygon region named " << name << ", empty polygon returned\n";
    delete parser;
    return out;
  }
  // create polygon
  std::vector<std::vector<vgl_point_3d<double> > > out_sheets = parser->polyouter_[name];
  unsigned num_sheet = out_sheets.size();
  for (unsigned i = 0; i < num_sheet; i++)
  {
    unsigned n_pts = out_sheets[i].size();
    out.new_sheet();
    for (unsigned k = 0; k < n_pts; k++)
      out.push_back(out_sheets[i][k].x(), out_sheets[i][k].y());
  }
  delete parser;
  return out;
}

// parse the polygon region given the name
// the first n_out sheets are the exterior boundary and the following n_in sheets are the interior boundary
vgl_polygon<double> volm_candidate_region_parser::parse_polygon_with_inner(std::string const& kml_file, std::string const& name,
                                                                           vgl_polygon<double>& outer, vgl_polygon<double>& inner,
                                                                           unsigned& n_out, unsigned& n_in)
{
  vgl_polygon<double> poly;
  poly.clear();
  auto* parser = new volm_candidate_region_parser;
  std::FILE* xmlFile = std::fopen(kml_file.c_str(), "r");
  if (!xmlFile) {
    std::cerr << "volm_candidate_region_parser: can not open kml file " << kml_file << '\n';
    delete parser;
    return poly;
  }
  if (!parser->parseFile(xmlFile)) {
    std::cerr << "volm_candidate_region_parser: " << XML_ErrorString(parser->XML_GetErrorCode()) << " at line "
             <<  parser->XML_GetCurrentLineNumber() << '\n';
    delete parser;
    return poly;
  }
  if (parser->polyouter_.find(name) == parser->polyouter_.end()) {
    std::cerr << "volm_candidate_region_parser: no polygon region named " << name << ", empty polygon returned\n";
    delete parser;
    return poly;
  }
  // create polygon
  outer.clear();
  inner.clear();
  // create the outer boundary polygon
  if (parser->polyouter_.find(name) != parser->polyouter_.end()) {
    std::vector<std::vector<vgl_point_3d<double> > > out_sheets = parser->polyouter_[name];
    unsigned int n_sheet = out_sheets.size();
    for (unsigned int i = 0; i < n_sheet; ++i) {
      unsigned int n_pts = out_sheets[i].size();
      outer.new_sheet();
      for (unsigned int k = 0; k < n_pts; ++k)
        outer.push_back(out_sheets[i][k].x(), out_sheets[i][k].y());
    }
  }
  // create the inner boundary polygon
  if (parser->polyinner_.find(name) != parser->polyinner_.end()) {
    std::vector<std::vector<vgl_point_3d<double> > > in_sheet = parser->polyinner_[name];
    unsigned n_sheet = in_sheet.size();
    for (unsigned i = 0; i < n_sheet; i++) {
      unsigned n_pts = in_sheet[i].size();
      inner.new_sheet();
      for (unsigned k = 0; k < n_pts; k++)
        inner.push_back(in_sheet[i][k].x(), in_sheet[i][k].y());
    }
  }
  // compose to form the polygon
  n_out = outer.num_sheets();
  n_in  = inner.num_sheets();
  for (unsigned s_idx = 0; s_idx < n_out; s_idx++)
    poly.push_back(outer[s_idx]);
  for (unsigned s_idx = 0; s_idx < n_in; s_idx++)
    poly.push_back(inner[s_idx]);
  delete parser;
  return poly;
}
