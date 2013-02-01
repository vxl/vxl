#include "volm_io.h"
//:
// \file
#include <bkml/bkml_parser.h>
#include <bpgl/bpgl_camera_utils.h>
#include <bvgl/bvgl_labelme_parser.h>
#include <depth_map/depth_map_region_sptr.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_polygon.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <bsol/bsol_algs.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vcl_cassert.h>
#include <vsl/vsl_vector_io.h>

vcl_map<vcl_string, depth_map_region::orientation> create_orient_map()
{
  vcl_map<vcl_string, depth_map_region::orientation> m;
  m["vertical"] = depth_map_region::VERTICAL;
  m["horizontal"] = depth_map_region::HORIZONTAL;
  m["slanted_right"] = depth_map_region::SLANTED_RIGHT;
  m["slanted_left"] = depth_map_region::SLANTED_LEFT;
  m["porous"] = depth_map_region::POROUS;
  m["non_planar"] = depth_map_region::NON_PLANAR;
  return m;
}

// list of the possible values for indexed orientations from the reference world
vcl_map<int, vil_rgb<vxl_byte> > create_orient_colors()
{
  vcl_map<int, vil_rgb<vxl_byte> > m;
  m[0] = vil_rgb<vxl_byte>(255, 0, 0);  // no value
  m[100] = vil_rgb<vxl_byte>(255, 0, 0);  // no value
  m[1] = vil_rgb<vxl_byte>(0, 255, 0);  // horizontal surfaces (e.g. ground, water, etc.)
  m[2] = vil_rgb<vxl_byte>(0, 255, 255);  // vertical facing west
  m[3] = vil_rgb<vxl_byte>(0, 255, 155);  // vertical facing south west
  m[4] = vil_rgb<vxl_byte>(255, 255, 0);  // vertical facing south
  m[5] = vil_rgb<vxl_byte>(155, 255, 0);  // vertical facing south east
  m[6] = vil_rgb<vxl_byte>(155, 255, 155);  // vertical facing east
  m[7] = vil_rgb<vxl_byte>(155, 155, 155);  // vertical facing north east
  m[8] = vil_rgb<vxl_byte>(155, 0, 155);  // vertical facing north
  m[9] = vil_rgb<vxl_byte>(0, 155, 155);  // vertical facing north west
  return m;
}

vcl_map<int, vcl_pair<unsigned char, vil_rgb<vxl_byte> > > create_nlcd_map()
{
  vcl_map<int, vcl_pair<unsigned char, vil_rgb<vxl_byte> > > m;
  m[0] = vcl_pair<unsigned char, vil_rgb<vxl_byte> > (20, vil_rgb<vxl_byte>(255, 0, 0));  // open water
  m[11] = vcl_pair<unsigned char, vil_rgb<vxl_byte> > (0, vil_rgb<vxl_byte>(0, 0, 100));  // open water
  m[12] = vcl_pair<unsigned char, vil_rgb<vxl_byte> > (1, vil_rgb<vxl_byte>(255, 255, 255));  // perennial ice/snow
  m[21] = vcl_pair<unsigned char, vil_rgb<vxl_byte> > (2, vil_rgb<vxl_byte>(50, 0, 0));  // developed, open space
  m[22] = vcl_pair<unsigned char, vil_rgb<vxl_byte> > (3, vil_rgb<vxl_byte>(100, 0, 10));  // developed, low intensity
  m[23] = vcl_pair<unsigned char, vil_rgb<vxl_byte> > (3, vil_rgb<vxl_byte>(200, 0, 100));  // developed, medium intensity  (combined with prev class)
  m[24] = vcl_pair<unsigned char, vil_rgb<vxl_byte> > (4, vil_rgb<vxl_byte>(220, 0, 100));  // developed, high intensity
  m[31] = vcl_pair<unsigned char, vil_rgb<vxl_byte> > (5, vil_rgb<vxl_byte>(170, 170, 170));  // barren land/beach (rock/sand/clay)
  m[41] = vcl_pair<unsigned char, vil_rgb<vxl_byte> > (6, vil_rgb<vxl_byte>(0, 200, 0));  // deciduous forest
  m[42] = vcl_pair<unsigned char, vil_rgb<vxl_byte> > (7, vil_rgb<vxl_byte>(0, 250, 0));  // evergreen forest
  m[43] = vcl_pair<unsigned char, vil_rgb<vxl_byte> > (8, vil_rgb<vxl_byte>(0, 100, 0));  // mixed forest
  m[51] = vcl_pair<unsigned char, vil_rgb<vxl_byte> > (9, vil_rgb<vxl_byte>(10, 50, 0));  // dwarf scrub - alaska only
  m[52] = vcl_pair<unsigned char, vil_rgb<vxl_byte> > (9, vil_rgb<vxl_byte>(10, 50, 0));  // shrub/scrub
  m[71] = vcl_pair<unsigned char, vil_rgb<vxl_byte> > (10, vil_rgb<vxl_byte>(0, 100, 20)); // grassland/herbaceous
  m[72] = vcl_pair<unsigned char, vil_rgb<vxl_byte> > (10, vil_rgb<vxl_byte>(0, 150, 10)); // sedge/herbaceous
  m[73] = vcl_pair<unsigned char, vil_rgb<vxl_byte> > (10, vil_rgb<vxl_byte>(0, 170, 10)); // lichens
  m[74] = vcl_pair<unsigned char, vil_rgb<vxl_byte> > (10, vil_rgb<vxl_byte>(0, 200, 10)); // moss
  m[81] = vcl_pair<unsigned char, vil_rgb<vxl_byte> > (11, vil_rgb<vxl_byte>(0, 120, 120)); // pasture hay
  m[82] = vcl_pair<unsigned char, vil_rgb<vxl_byte> > (12, vil_rgb<vxl_byte>(210, 105, 30)); // cultivated crops
  m[90] = vcl_pair<unsigned char, vil_rgb<vxl_byte> > (13, vil_rgb<vxl_byte>(176, 196, 222)); // woody wetland -- marina
  m[95] = vcl_pair<unsigned char, vil_rgb<vxl_byte> > (13, vil_rgb<vxl_byte>(176, 196, 255)); // Emergent Herbaceous Wetlands
  return m;
}

vcl_map<vcl_string, depth_map_region::orientation> volm_orient_table::ori_id = create_orient_map();
vcl_map<int, vcl_pair<unsigned char, vil_rgb<vxl_byte> > > volm_nlcd_table::land_id = create_nlcd_map();
vcl_map<int, vil_rgb<vxl_byte> > volm_orient_table::ori_index_colors = create_orient_colors();

bool volm_io::read_camera(vcl_string kml_file,
                          unsigned const& ni, unsigned const& nj,
                          double& heading,   double& heading_dev,
                          double& tilt,      double& tilt_dev,
                          double& roll,      double& roll_dev,
                          double& top_fov,   double& top_fov_dev,
                          double& altitude, double& lat, double& lon)
{
  heading_dev = 0;
  tilt_dev = 0;
  roll_dev = 0;
  top_fov_dev = 0;
  bkml_parser* parser = new bkml_parser();
  vcl_FILE* xmlFile = vcl_fopen(kml_file.c_str(), "r");
  if (!xmlFile) {
    vcl_cerr << kml_file.c_str() << " ERROR in camera kml: can not open the given camera kml file.\n";
    delete parser;
    return false;
  }
  if (!parser->parseFile(xmlFile)) {
    vcl_cerr << XML_ErrorString(parser->XML_GetErrorCode()) << " at line "
             << parser->XML_GetCurrentLineNumber() << '\n';
    delete parser;
    return false;
  }

  lat = 0; lon = 0;
  if (parser->heading_)       heading = parser->heading_;
  if (parser->heading_dev_)   heading_dev = parser->heading_dev_;
  if (parser->tilt_)          tilt = parser->tilt_;
  if (parser->tilt_dev_)      tilt_dev = parser->tilt_dev_;
  if (parser->roll_)          roll = parser->roll_;
  if (parser->altitude_)      altitude = parser->altitude_;
  if (parser->latitude_)      lat = parser->latitude_;
  if (parser->longitude_)     lon = parser->longitude_;

  double dtor = vnl_math::pi_over_180;
  double ppu = 0.5*ni;
  double ppv = 0.5*nj;
  // check the consistency of input parameters
  if ( parser->right_fov_dev_ && !parser->right_fov_ ) {
    vcl_cerr << " ERROR in camera kml: deviation of right_fov is defined without given initial right_fov.\n";
    return false;
  }
  if ( parser->top_fov_dev_ && !parser->top_fov_) {
    vcl_cerr << " ERROR in camera kml: deviation of top_fov is defined without given initial top_fov.\n";
    return false;
  }

  // define the viewing volume
  if ( parser->right_fov_ && parser->top_fov_ ) {  // use averaged value to define top_fov_
    double tr = vcl_tan(parser->right_fov_*dtor), tt = vcl_tan(parser->top_fov_*dtor);
    double fr = ppu/tr, ft = ppv/tt;
    double f = 0.5*(fr+ft);
    top_fov = vcl_atan(0.5*nj/f)/dtor;
  }
  else if ( parser->right_fov_ ) {  // transfer right_fov to top_fov to serve query purpose
    double tr = vcl_tan(parser->right_fov_*dtor);
    double fr = ni/tr;
    top_fov = vcl_atan(nj/fr)/dtor;
  }
  else if ( parser->top_fov_ ) {   // use top_fov directly in query
    top_fov = parser->top_fov_;
  }
  // define the deviation of viewing volume
  if ( parser->right_fov_dev_ && parser->top_fov_dev_ ) {
    // If both given, use the one that gives larger focal range
    double trd = vcl_tan( dtor*(parser->right_fov_ + parser->right_fov_dev_) );
    double ttd = vcl_tan( dtor*(parser->top_fov_ + parser->top_fov_dev_) );
    double frd = ppu/trd, ftd = ppv/ttd;
    if (frd >= ftd)
      top_fov_dev = parser->top_fov_dev_;
    else
      top_fov_dev = parser->right_fov_dev_;
  }
  else if ( parser->right_fov_dev_ ) {
    // transfer right_fov_dev to top_fov_dev
    double trd = vcl_tan( dtor*(parser->right_fov_ + parser->right_fov_dev_) );
    top_fov_dev = vcl_atan(nj*trd/ni)/dtor - top_fov;
  }
  else if ( parser->top_fov_dev_ ) {
    // use top_fov
    top_fov_dev = parser->top_fov_dev_;
  }

  delete parser;
  return true;
}

bool volm_io::read_labelme(vcl_string xml_file, depth_map_scene_sptr& depth_scene, vcl_string& img_category)
{
  bvgl_labelme_parser parser(xml_file);
  vcl_vector<vgl_polygon<double> > polys = parser.polygons();
  vcl_vector<vcl_string>& object_names = parser.obj_names();
  vcl_vector<vcl_string>& object_types = parser.obj_types();
  vcl_vector<int>& object_orders = parser.obj_depth_orders();
  vcl_vector<float>& object_mindist = parser.obj_mindists();
  vcl_vector<float>& object_maxdist = parser.obj_maxdists();
  vcl_vector<vcl_string>& object_orient = parser.obj_orientations();
  vcl_vector<unsigned>& object_nlcd = parser.obj_nlcd_ids();
  if (polys.size() != object_names.size()   ||
      polys.size() != object_orient.size()  ||
      polys.size() != object_nlcd.size()    ||
      polys.size() != object_types.size()   ||
      polys.size() != object_mindist.size() ||
      polys.size() != object_orders.size()     ) {
    vcl_cerr << " ERROR in labelme xml file: imcomplete object properties defination, check object attributes\n";
    return false;
  }
  if (!parser.image_ni() || !parser.image_nj()) {
    vcl_cerr << " ERROR in labelme xml file: Missing image size information, check <nrows> and <ncols>\n";
    return false;
  }
  if (parser.image_category() == "") {
    vcl_cerr << " ERROR in labelme xml file: Missing image category information, check <ImageCategory>\n";
    return false;
  }
  // load the image category
  img_category = parser.image_category();
  if ( img_category != "desert" && img_category != "coast" ) {
    vcl_cout << " image_category is " << img_category << vcl_endl;
    vcl_cerr << " WARNING in labelme xml file: undefined img_category found\n";
  }
  // load the image size
  unsigned ni = parser.image_ni();
  unsigned nj = parser.image_nj();
  depth_scene->set_image_size(nj, ni);

  // push the depth_map_region into depth_scene in the order of defined order in xml
  for (unsigned i = 0; i < polys.size(); i++) {
    vsol_polygon_2d_sptr poly = bsol_algs::poly_from_vgl(polys[i]);
    // SKY region
    if (object_types[i] == "sky") {
      depth_scene->add_sky(poly, object_orders[i], object_names[i]);
    }
    // GROUND region
    else if (object_types[i] == "road" || object_types[i] == "beach" || object_types[i] == "desert" || object_types[i] == "flat"
             || object_types[i] == "ground" || object_types[i] == "water" || object_types[i] == "ocean" )
    {
      double min_depth = object_mindist[i], max_depth = object_maxdist[i];
      if (min_depth < 4) {  // define it as ground plane
        depth_scene->add_ground(poly, min_depth, max_depth, object_orders[i], object_names[i], object_nlcd[i]);
      }
      else {
        vgl_vector_3d<double> gp(0.0,0.0,1.0);
        depth_scene->add_region(poly, gp, min_depth, max_depth, object_names[i],
                                volm_orient_table::ori_id[object_orient[i]], object_orders[i], object_nlcd[i]);
      }
    }
    // non-sky/non-ground region
    else {
      double min_depth = parser.obj_mindists()[i], max_depth = parser.obj_maxdists()[i];
      vgl_vector_3d<double> np; // surface normal
      if (object_orient[i] == "horizontal")
        np.set(0.0, 0.0, 1.0);
      else if (object_orient[i] == "vertical")
        np.set(1.0, 1.0, 0.0);
      else
        np.set(1.0, 1.0, 1.0);
      depth_scene->add_region(poly, np, min_depth, max_depth, object_names[i],
                              volm_orient_table::ori_id[object_orient[i]], object_orders[i], object_nlcd[i]);
    }
  }
  return true;
}

bool volm_io::write_status(vcl_string out_folder, int status_code, int percent, vcl_string log_message)
{
  vcl_ofstream file;
  vcl_string out_file = out_folder + "/status.xml";
  file.open (out_file.c_str());
  file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<status>\n";
  switch (status_code) {
    case volm_io::CAM_FILE_IO_ERROR:
      file << "Camera FILE IO Error\n<percent>0</percent>\n"; break;
    case volm_io::EXE_ARGUMENT_ERROR:
      file << "Error in executable arguments\n<percent>0</percent>\n"; break;
    case volm_io::SUCCESS:
      file << "Completed Successfully\n<percent>100</percent>\n"; break;
    case volm_io::MATCHER_EXE_STARTED:
      file << "Matcher Exe Started\n<percent>0</percent>\n"; break;
    case volm_io::MATCHER_EXE_FINISHED:
      file << "Matcher Exe Finished, composer starting..\n<percent>90</percent>\n"; break;
    case volm_io::COMPOSE_STARTED:
      file << "Composing output tiles\n<percent>90</percent>\n"; break;
    case volm_io::EXE_RUNNING:
      file << "Matcher Exe Running\n<percent>\n" << percent << "\n</percent>\n"; break;
    case volm_io::LABELME_FILE_IO_ERROR:
      file << "LABELME FILE IO Error\n<percent>0</percent>\n"; break;
    case volm_io::COMPOSE_HALT:
      file << "COMPOSER waiting for matcher to complete\n<percent>90</percent>\n"; break;
    case volm_io::EXE_STARTED:
      file << "PREP exe starterd\n<percent>0</percent>\n"; break;
    default:
      file << "Unidentified status code!\n";
      vcl_cerr << "Unidentified status code!\n";
      break;
  }
  file << "<code>\n" << status_code << "\n</code>\n"
       << "<log>\n" << log_message << "\n</log>\n"
       << "</status>\n";
  file.close();
  return true;
}

//: return true if MATCHER_EXE_FINISHED, otherwise return false
bool volm_io::check_matcher_status(vcl_string out_folder)
{
  vcl_ifstream ifs;
  vcl_string file = out_folder + "/status.xml";
  ifs.open(file.c_str());
  char buf[10000];
  ifs.getline(buf, 10000);
  ifs.close();
  vcl_stringstream str(buf);
  vcl_string dummy;
  str >> dummy;
  while (dummy.compare("<code>") != 0)
    str >> dummy;
  int status_code;
  str >> status_code;
  return status_code == volm_io::MATCHER_EXE_FINISHED;
}


bool volm_io::write_log(vcl_string out_folder, vcl_string log)
{
  vcl_ofstream file;
  vcl_string out_file = out_folder + "/log.xml";
  file.open (out_file.c_str());
  file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<status>\n"
       << "<log>\n"<<log<<"</log>\n";
  file.close();
  return true;
}

bool volm_io::write_composer_log(vcl_string out_folder, vcl_string log)
{
  vcl_ofstream file;
  vcl_string out_file = out_folder + "/composer_log.xml";
  file.open (out_file.c_str());
  file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<status>\n"
       << "<log>\n"<<log<<"</log>\n";
  file.close();
  return true;
}

//: piecewise linear s.t. [1,127) -> [0,t), [127,255] -> [t,1]
float volm_io::scale_score_to_0_1(unsigned char pix_value, float threshold)
{
  if (pix_value < 127)
    return ((float)pix_value/127)*threshold;
  else
    return ((float)(pix_value-127)/128)*(1-threshold) + threshold;
}

//: piecewise linear s.t. [0,t) -> [1,127), [t,1] -> [127,255]"
unsigned char volm_io::scale_score_to_1_255(float threshold, float score)
{
  if (score < threshold)
    return (unsigned char) ((score/threshold)*127);
  else
    return (unsigned char) (((score-threshold)/(1-threshold))*128 + 127);
}

//: piecewise linear s.t. [0,t) -> [1,63), [t,1] -> [63,127]"
unsigned char volm_io::scale_score_to_1_127(float threshold, float score)
{
  if (score < threshold)
    return (unsigned char) ((score/threshold)*63);
  else
    return (unsigned char) (((score-threshold)/(1-threshold))*64 + 63);
}

bool operator>(const vcl_pair<float, volm_rationale>& a, const vcl_pair<float, volm_rationale>& b)
{  return a.first>b.first; }

bool volm_rationale::write_top_matches(vcl_multiset<vcl_pair<float, volm_rationale>, std::greater<vcl_pair<float, volm_rationale> > >& top_matches, vcl_string& filename)
{
  vcl_ofstream ofs(filename.c_str());
  if (!ofs.is_open()) {
    vcl_cerr << " cannot open: " << filename << " for write!\n";
    return false;
  }
  vcl_multiset<vcl_pair<float, volm_rationale>, std::greater<vcl_pair<float, volm_rationale> > >::iterator iter;
  ofs << top_matches.size() << vcl_endl;
  for (iter = top_matches.begin(); iter != top_matches.end(); iter++) {
    ofs << iter->first << ' ' << iter->second.lat << ' ' << iter->second.lon << ' ' << iter->second.elev << ' ' << iter->second.index_id << ' ' << iter->second.cam_id << '\n'
        << iter->second.index_file << '\n'
        << iter->second.score_file << vcl_endl;
  }
  ofs.close();
  return true;
}

bool volm_rationale::read_top_matches(vcl_multiset<vcl_pair<float, volm_rationale>, std::greater<vcl_pair<float, volm_rationale> > >& top_matches, vcl_string& filename)
{
  vcl_ifstream ifs(filename.c_str());
  if (!ifs.is_open()) {
    vcl_cerr << " cannot open: " << filename << vcl_endl;
    return false;
  }
  unsigned cnt = 0;
  ifs >> cnt;
  for (unsigned i = 0; i < cnt; i++) {
    float score;
    ifs >> score;
    volm_rationale r;
    ifs >> r.lat >> r.lon >> r.elev >> r.index_id >> r.cam_id;
    ifs >> r.index_file >> r.score_file;
    top_matches.insert(vcl_pair<float, volm_rationale>(score, r));
  }
  ifs.close();
  return true;
}

// x is lon, y is lat
void volm_io::read_polygons(vcl_string poly_file, vgl_polygon<double>& out)
{
  vcl_ifstream ifs(poly_file.c_str());
  unsigned np, nvert;
  double x,y;

  ifs >> np;
  for (unsigned i = 0; i < np; i++) {
    ifs >> nvert;
    if (nvert < 3) {
      for (unsigned j = 0; j < nvert; j++) {
        ifs >> x; ifs >> y;
      }
      continue; // do not insert as a sheet
    }
    out.new_sheet();

    for (unsigned j = 0; j < nvert; j++) {
      ifs >> x; ifs >> y;
      vgl_point_2d<double> pt(x,y);
      out[i].push_back(pt);
    }
    unsigned cnt = 0;
    for (int j = (int)nvert-1; j > 0; j--)
      if (out[i][j] == out[i][0])
        cnt++;
    if (cnt > 0) {
      out[i].erase(out[i].begin()+nvert-cnt, out[i].end());
    }
  }
  assert(np == out.num_sheets());
}

void volm_io::convert_polygons(vgl_polygon<double> const& in, vgl_polygon<float>& out)
{
  // convert poly
  for (unsigned i = 0; i < in.num_sheets(); i++)
    out.new_sheet();

  for (unsigned i = 0; i < in.num_sheets(); i++) {
    for (unsigned j = 0; j < in[i].size(); j++) {
      vgl_point_2d<float> pt((float)in[i][j].x(), (float)in[i][j].y());
      out[i].push_back(pt);
    }
  }
}

//: binary IO write
void volm_score::b_write(vsl_b_ostream& os)
{
  unsigned ver = this->version();
  vsl_b_write(os, ver);
  vsl_b_write(os, leaf_id_);
  vsl_b_write(os, hypo_id_);
  vsl_b_write(os, max_score_);
  vsl_b_write(os, max_cam_id_);
  vsl_b_write(os, cam_id_);
}

//: binary IO read
void volm_score::b_read(vsl_b_istream& is)
{
  unsigned ver;
  vsl_b_read(is, ver);
  if (ver == 1) {
    vsl_b_read(is, leaf_id_);
    vsl_b_read(is, hypo_id_);
    vsl_b_read(is, max_score_);
    vsl_b_read(is, max_cam_id_);
    vsl_b_read(is, cam_id_);
  }
  else
  {
    vcl_cerr << "I/O ERROR: volm_score::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }
}

int volm_io::read_gt_file(vcl_string gt_file, vcl_vector<vcl_pair<vgl_point_3d<double>, vcl_pair<vcl_string, vcl_string> > >& samples)
{
  vcl_ifstream ifs(gt_file.c_str());
  int cnt; ifs >> cnt;
  for (int j = 0; j < cnt; j++) {
    vcl_string name; ifs >> name; vcl_string type;
    double lat, lon, elev;
    ifs >> lat; ifs >> lon; ifs >> elev;
    ifs >> type;
    vgl_point_3d<double> pt(lon, lat, elev);
    samples.push_back(vcl_pair<vgl_point_3d<double>, vcl_pair<vcl_string, vcl_string> >(pt, vcl_pair<vcl_string, vcl_string>(name, type) ) );
  }
  ifs.close();
  return cnt;
}

void volm_score::write_scores(vcl_vector<volm_score_sptr>& scores, vcl_string const& file_name)
{
  vsl_b_ofstream ofs(file_name);
  vsl_b_write(ofs, (unsigned)(scores.size()));
  for (unsigned i = 0; i < scores.size(); i++)
    scores[i]->b_write(ofs);
  ofs.close();
}

void volm_score::read_scores(vcl_vector<volm_score_sptr>& scores, vcl_string const& file_name)
{
  vsl_b_ifstream ifs(file_name);
  unsigned size;
  vsl_b_read(ifs, size);
  scores.clear();
  for (unsigned i = 0; i < size; i++) {
    volm_score_sptr s = new volm_score;
    s->b_read(ifs);
    scores.push_back(s);
  }
  ifs.close();
}

void volm_io_expt_params::read_params(vcl_string params_file)
{
  vcl_ifstream ifs(params_file.c_str());
  vcl_string dummy;
  ifs >> dummy; ifs >> fov_inc;     vcl_cout << dummy << ' ' << fov_inc << ' ';
  ifs >> dummy; ifs >> tilt_inc;    vcl_cout << dummy << ' ' << tilt_inc << ' ';
  ifs >> dummy; ifs >> roll_inc;    vcl_cout << dummy << ' ' << roll_inc << ' ';
  ifs >> dummy; ifs >> head_inc;    vcl_cout << dummy << ' ' << head_inc << ' ';
  ifs >> dummy; ifs >> vmin;        vcl_cout << dummy << ' ' << vmin << ' ';
  ifs >> dummy; ifs >> solid_angle; vcl_cout << dummy << ' ' << solid_angle << ' ';
  ifs >> dummy; ifs >> dmax;        vcl_cout << dummy << ' ' << dmax << ' ';
  ifs >> dummy; ifs >> cap_angle;   vcl_cout << dummy << ' ' << cap_angle << ' ';
  ifs >> dummy; ifs >> point_angle; vcl_cout << dummy << ' ' << point_angle << ' ';
  ifs >> dummy; ifs >> top_angle;   vcl_cout << dummy << ' ' << top_angle << ' ';
  ifs >> dummy; ifs >> bottom_angle;vcl_cout << dummy << ' ' << bottom_angle << '\n';
}
