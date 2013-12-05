//:
// \file
#include "bkml_parser.h"
//
// \brief Parses the kml configuration file for bwm tool.
// \verbatim
//  Modifications
//   2012-09-10 Yi Dong - Modified to parser the polygon and path(LineString) coordinates stored in kml
// \endverbatim
//
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h>
#include <vcl_cstring.h>
#include <vcl_cstddef.h> // for std::size_t

// --------------
// --- PARSER ---
// --------------
template <typename T>
void convert(const char* t, T& d)
{
  vcl_stringstream strm(t);
  strm >> d;
}

bkml_parser::bkml_parser()
{
  init_params();
}

void bkml_parser::init_params()
{
  heading_ = 0.0;
  tilt_ = 0.0;
  roll_ = 0.0;
  right_fov_ = 0.0;
  top_fov_ = 0.0;
  near_ = 0.0;
  heading_dev_ = 0.0;
  tilt_dev_ = 0.0;
  roll_dev_ = 0.0;
  right_fov_dev_ = 0.0;
  top_fov_dev_ = 0.0;
}

void
bkml_parser ::cdataHandler(vcl_string name, vcl_string data)
{
}

void
bkml_parser::handleAtts(const XML_Char** /*atts*/)
{
}


void
bkml_parser::startElement(const char* name, const char** atts)
{
  if (vcl_strcmp(name, KML_LON_TAG) == 0) {
    last_tag = KML_LON_TAG;
  }
  else if (vcl_strcmp(name, KML_LAT_TAG) == 0) {
    last_tag = KML_LAT_TAG;
  }
  else if (vcl_strcmp(name, KML_ALT_TAG) == 0) {
    last_tag = KML_ALT_TAG;
  }
  else if (vcl_strcmp(name, KML_HEAD_TAG) == 0 ) {
    last_tag = KML_HEAD_TAG;
  }
  else if (vcl_strcmp(name, KML_HEAD_DEV_TAG) == 0 ) {
    last_tag = KML_HEAD_DEV_TAG;
  }
  else if (vcl_strcmp(name, KML_TILT_TAG) == 0 ) {
    last_tag = KML_TILT_TAG;
  }
  else if (vcl_strcmp(name, KML_TILT_DEV_TAG) == 0 ) {
    last_tag = KML_TILT_DEV_TAG;
  }
  else if (vcl_strcmp(name, KML_ROLL_TAG) == 0 ) {
    last_tag = KML_ROLL_TAG;
  }
  else if (vcl_strcmp(name, KML_ROLL_DEV_TAG) == 0 ) {
    last_tag = KML_ROLL_DEV_TAG;
  }
  else if (vcl_strcmp(name, KML_RFOV_TAG) == 0) {
    last_tag = KML_RFOV_TAG;
  }
  else if (vcl_strcmp(name, KML_RFOV_DEV_TAG) == 0 ) {
    last_tag = KML_RFOV_DEV_TAG;
  }
  else if (vcl_strcmp(name, KML_TFOV_TAG) == 0) {
    last_tag = KML_TFOV_TAG;
  }
  else if (vcl_strcmp(name, KML_TFOV_DEV_TAG) == 0) {
    last_tag = KML_TFOV_DEV_TAG;
  }
  else if (vcl_strcmp(name, KML_NEAR_TAG) == 0) {
    last_tag = KML_NEAR_TAG;
  }
  else if (vcl_strcmp(name, KML_CORD_TAG) == 0 && (cord_tag_ != "") ) {
    last_tag = KML_CORD_TAG;
  }
  else if (vcl_strcmp(name, KML_POLYOB_TAG) == 0) {
    cord_tag_ = KML_POLYOB_TAG;
  }
  else if (vcl_strcmp(name, KML_POLYIB_TAG) == 0) {
    cord_tag_ = KML_POLYIB_TAG;
  }
  else if (vcl_strcmp(name, KML_LINE_TAG) == 0) {
    cord_tag_ = KML_LINE_TAG;
  }
  else if (vcl_strcmp(name, KML_POINT_TAG) == 0) {
    cord_tag_ = KML_POINT_TAG;
  }
  /*else if (vcl_strcmp(name, KML_COORDS_TAG) == 0) {
  last_tag = KML_COORDS_TAG;
  }
  else if (vcl_strcmp(name, KML_PLACEMARK_NAME_TAG) == 0) {
  last_tag = KML_PLACEMARK_NAME_TAG;
  }*/
}


void
bkml_parser::endElement(const char* name)
{
}

void bkml_parser::charData(const XML_Char* s, int len)
{
  const int leadingSpace = skipWhiteSpace(s);
  if (len==0 || len<=leadingSpace)
    return;  // called with whitespace between elements
  if (last_tag == KML_LON_TAG) {
    vcl_stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> longitude_;
    last_tag = "";
  }
  else if (last_tag == KML_LAT_TAG) {
    vcl_stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> latitude_;
    last_tag = "";
  }
  /*else if (last_tag == KML_COORDS_TAG) {
    if (current_name_.compare("Camera Ground Truth") == 0) {
      vcl_stringstream str;
      for (int i =0; i<len; ++i) {
        if (s[i] == ',') 
          str << ' ';
        else
          str << s[i];
      }
      str >> longitude_ >> latitude_;
    }
    last_tag = "";
  }
  else if (last_tag == KML_PLACEMARK_NAME_TAG) {
    vcl_stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    current_name_ = str.str();
    last_tag = "";
  }*/
  else if (last_tag == KML_ALT_TAG) {
    vcl_stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> altitude_;
    last_tag = "";
  }
  else if (last_tag == KML_HEAD_TAG) {
    vcl_stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> heading_;
    last_tag = "";
  }
  else if (last_tag == KML_HEAD_DEV_TAG) {
    vcl_stringstream str;
    for (int i = 0; i < len; i++)
      str << s[i];
    str >> heading_dev_;
    last_tag = "";
  }
  else if (last_tag == KML_TILT_TAG) {
    vcl_stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> tilt_;
    last_tag = "";
  }
  else if (last_tag == KML_TILT_DEV_TAG) {
    vcl_stringstream str;
    for (int i = 0; i < len; i++)
      str << s[i];
    str >> tilt_dev_;
    last_tag = "";
  }
  else if (last_tag == KML_ROLL_TAG) {
    vcl_stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> roll_;
    last_tag = "";
  }
  else if (last_tag == KML_ROLL_DEV_TAG) {
    vcl_stringstream str;
    for (int i = 0; i < len; i++)
      str << s[i];
    str >> roll_dev_;
    last_tag = "";
  }
  else if (last_tag == KML_RFOV_TAG) {
    vcl_stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> right_fov_;
    last_tag = "";
  }
  else if (last_tag == KML_RFOV_DEV_TAG) {
    vcl_stringstream str;
    for (int i = 0; i < len; i++)
      str << s[i];
    str >> right_fov_dev_;
    last_tag = "";
  }
  else if (last_tag == KML_TFOV_TAG) {
    vcl_stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> top_fov_;
    last_tag = "";
  }
  else if (last_tag == KML_TFOV_DEV_TAG) {
    vcl_stringstream str;
    for (int i = 0; i < len; ++i)
      str << s[i];
    str >> top_fov_dev_;
    last_tag = "";
  }
  else if (last_tag == KML_NEAR_TAG) {
    vcl_stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> near_;
    last_tag = "";
  }
  else if (last_tag == KML_CORD_TAG) {
    vcl_stringstream str;
    double x,y,z;
    vcl_string str_s;
    str_s = s;
    vcl_size_t cord_end = str_s.find(KML_POLYCORE_END_TAG);
    if (str_s[len] == '<')
      cord_end = len;
    else {
      while (str_s[cord_end] != '\n')
        cord_end--;
      while (str_s[cord_end-1] == ' ')
        cord_end--;
      if ((int)cord_end > len)
        len = (int)cord_end;
    }
    
    for (unsigned int i=0; i<cord_end; ++i)
      str << s[i];
    vcl_vector<vgl_point_3d<double> > poly_verts;
    while (!str.eof()) {
      str >> x;
      str.ignore();
      str >> y;
      str.ignore();
      str >> z;
      str.ignore(128, ' ');
      vgl_point_3d<double> vpt(x,y,z);
      poly_verts.push_back(vpt);
      //if (cord_tag_ == KML_POLYOB_TAG)
      //  poly_verts.push_back(vpt);
      //else if (cord_tag_ == KML_POLYIB_TAG)
      //  poly_verts.push_back(vpt);
      //else if (cord_tag_ == KML_LINE_TAG)
      //  poly_verts.push_back(vpt);
      //else {
      //  vcl_cout << "WARNING: shape tag can not be recognized (not LineString or Polygon)" << vcl_endl;
      //}
    }
    if (cord_tag_ == KML_POLYOB_TAG)
      polyouter_.push_back(poly_verts);
    else if (cord_tag_ == KML_POLYOB_TAG)
      polyinner_.push_back(poly_verts);
    else if (cord_tag_ == KML_LINE_TAG)
      linecord_.push_back(poly_verts);
    else if (cord_tag_ == KML_POINT_TAG)
      points_.push_back(poly_verts[0]);
    else
      vcl_cout << "WARNING: shape tag can not be recognized (not LineString, Point or Polygon, nothing will be parserd)" << vcl_endl;
    last_tag = "";
    cord_tag_ = "";
  }
}

void bkml_parser::trim_string(vcl_string& s)
{
  int i = (int)s.find_first_not_of(" ");
  int j = (int)s.find_last_not_of(" ");
  vcl_string t = s.substr(i,j-i+1);
  s = t;
}

vcl_vector<vgl_point_3d<double> > bkml_parser::parse_points(vcl_string kml_file)
{
  bkml_parser* parser = new bkml_parser();
  vcl_vector<vgl_point_3d<double> > out;
  vcl_FILE* xmlFile = vcl_fopen(kml_file.c_str(), "r");
  if (!xmlFile) {
    vcl_cerr << kml_file.c_str() << " error on opening the input kml file\n";
    delete parser;
    return out;
  }
  if (!parser->parseFile(xmlFile)) {
    vcl_cerr << XML_ErrorString(parser->XML_GetErrorCode()) << " at line "
             << parser->XML_GetCurrentLineNumber() << '\n';
    delete parser;
    return out;
  }
  // return the points retrieved from kml
  return parser->points_;
}

//: the first sheet contains the outer polygon, and the second sheet contains the inner polygon if any, saves 2d points, only lat, lon
vgl_polygon<double> bkml_parser::parse_polygon(vcl_string poly_kml_file)
{
  bkml_parser* parser = new bkml_parser();
  vgl_polygon<double> out;
  vcl_FILE* xmlFile = vcl_fopen(poly_kml_file.c_str(), "r");
  if (!xmlFile) {
    vcl_cerr << poly_kml_file.c_str() << " error on opening the input kml file\n";
    delete parser;
    return out;
  }
  if (!parser->parseFile(xmlFile)) {
    vcl_cerr << XML_ErrorString(parser->XML_GetErrorCode()) << " at line "
             << parser->XML_GetCurrentLineNumber() << '\n';
    delete parser;
    return out;
  }
  // create polygon from parser
  if (parser->polyouter_.empty()) {
    vcl_cerr << "input kml has no polygon outerboundary, return an empty polygon" << '\n';
    delete parser;
    vgl_polygon<double> out2;
    return out2;
  }
  for (unsigned sh_idx = 0; sh_idx < parser->polyouter_.size(); sh_idx++) {
    out.new_sheet();
    unsigned n_points = (unsigned)parser->polyouter_[sh_idx].size()-1;
    for (unsigned pt_idx = 0; pt_idx < n_points; pt_idx++) {
      out.push_back(parser->polyouter_[sh_idx][pt_idx].x(), parser->polyouter_[sh_idx][pt_idx].y());
    }
  }
  return out;
}

vgl_polygon<double> bkml_parser::parse_polygon_with_inner(vcl_string poly_kml_file, unsigned& n_out, unsigned& n_in)
{
  bkml_parser* parser = new bkml_parser();
  vgl_polygon<double> out;
  vcl_FILE* xmlFile = vcl_fopen(poly_kml_file.c_str(), "r");
  if (!xmlFile) {
    vcl_cerr << poly_kml_file.c_str() << " error on opening the input kml file\n";
    delete parser;
    return out;
  }
  if (!parser->parseFile(xmlFile)) {
    vcl_cerr << XML_ErrorString(parser->XML_GetErrorCode()) << " at line "
             << parser->XML_GetCurrentLineNumber() << '\n';
    delete parser;
    return out;
  }
  // create polygon from parser
  n_out = (unsigned)parser->polyouter_.size();
  n_in = (unsigned)parser->polyinner_.size();
  // load the outer boundary
  for (unsigned sh_idx = 0; sh_idx < n_out; sh_idx++) {
    out.new_sheet();
    unsigned n_points = (unsigned)parser->polyouter_[sh_idx].size() - 1;
    for (unsigned pt_idx = 0; pt_idx < n_points; pt_idx++)
      out.push_back(parser->polyouter_[sh_idx][pt_idx].x(), parser->polyouter_[sh_idx][pt_idx].y());
  }
  // load the inner boundary
  for (unsigned sh_idx = 0; sh_idx < n_in; sh_idx++) {
    out.new_sheet();
    unsigned n_points = (unsigned)parser->polyinner_[sh_idx].size() - 1;
    for (unsigned pt_idx = 0; pt_idx < n_points; pt_idx++)
      out.push_back(parser->polyinner_[sh_idx][pt_idx].x(), parser->polyinner_[sh_idx][pt_idx].y());
  }
  return out;
}

bool bkml_parser::parse_location_from_kml(vcl_string kml_file, double& lat, double& lon)
{
  bkml_parser* parser = new bkml_parser();
  vcl_FILE* xmlFile = vcl_fopen(kml_file.c_str(), "r");
  if(!xmlFile) {
    vcl_cerr << kml_file.c_str() << " error on opening the input kml file\n";
    delete parser;
    return false;
  }
  if(!parser->parseFile(xmlFile)){
    vcl_cerr << XML_ErrorString(parser->XML_GetErrorCode()) << " at line "
             << parser->XML_GetCurrentLineNumber() << '\n';
    delete parser;
    return false;
  }
  lat = parser->latitude_;
  lon = parser->longitude_;
  return true;
}
