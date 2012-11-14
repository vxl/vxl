#include "volm_io.h"
//:
// \file
#include <bkml/bkml_parser.h>
#include <bpgl/bpgl_camera_utils.h>
#include <bvgl/bvgl_labelme_parser.h>
#include <depth_map/depth_map_region_sptr.h>
#include <vgl/vgl_plane_3d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <bsol/bsol_algs.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>

//:
// \file
#include <vcl_cassert.h>

bool volm_io::read_camera(vcl_string kml_file, unsigned const& ni, unsigned const& nj,
                                               double& heading,   double& heading_dev,
                                               double& tilt,      double& tilt_dev,
                                               double& roll,      double& roll_dev,
                                               double& top_fov,   double& top_fov_dev)
{
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

  if(parser->heading_)       heading = parser->heading_;
  if(parser->heading_dev_)   heading_dev = parser->heading_dev_;
  if(parser->tilt_)          tilt = parser->tilt_;
  if(parser->tilt_dev_)      tilt_dev = parser->tilt_dev_;
  if(parser->roll_)          roll = parser->roll_;
  
  double dtor = vnl_math::pi_over_180;
  double ppu = 0.5*ni;
  double ppv = 0.5*nj;
  // check the consitency of input parameters
  if( parser->right_fov_dev_ && !parser->right_fov_ ){
    vcl_cerr << " ERROR in camera kml: deviation of right_fov is defined without given inital right_fov.\n";
    return false;
  }
  if( parser->top_fov_dev_ && !parser->top_fov_) {
    vcl_cerr << " ERROR in camera kml: deviation of top_fov is defined without given inital top_fov.\n";
    return false;
  }

  // define the viewing volume
  if( parser->right_fov_ && parser->top_fov_ ) {  // use averaged value to define top_fov_
    double tr = vcl_tan(parser->right_fov_*dtor), tt = vcl_tan(parser->top_fov_*dtor);
    double fr = ppu/tr, ft = ppv/tt;
    double f = 0.5*(fr+ft);
    top_fov = vcl_atan(0.5*nj/f)/dtor;
  }else if( parser->right_fov_ ){  // transfer right_fov to top_fov to serve query purpose
    double tr = vcl_tan(parser->right_fov_*dtor);
    double fr = ni/tr;
    top_fov = vcl_atan(nj/fr)/dtor;
  }else if( parser->top_fov_ ) {   // use top_fov directly in query 
    top_fov = parser->top_fov_;
  }

  // define the deviation of viewing volume
  if( parser->right_fov_dev_ && parser->top_fov_dev_ ){
    // If both given, use the one that gives larger focal range
    double trd = vcl_tan( dtor*(parser->right_fov_ + parser->right_fov_dev_) );
    double ttd = vcl_tan( dtor*(parser->top_fov_ + parser->top_fov_dev_) );
    double frd = ppu/trd, ftd = ppv/ttd;
    if(frd >= ftd)
      top_fov_dev = parser->top_fov_dev_;
    else
      top_fov_dev = parser->right_fov_dev_;
  }else if( parser->right_fov_dev_ ) {
    // transfer right_fov_dev to top_fov_dev
    double trd = vcl_tan( dtor*(parser->right_fov_ + parser->right_fov_dev_) );
    top_fov_dev = vcl_atan(nj*trd/ni)/dtor - top_fov;
  }else if( parser->top_fov_dev_ ) {
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
  if (polys.size() != object_names.size() || !parser.image_ni() || !parser.image_nj() || parser.image_category() == "" ) {
    vcl_cerr << " ERROR in labelme xml file: defined names and shapes do not match\n";
    return false;
  }
  // load the image category
  img_category = parser.image_category();
  if( img_category != "desert" && img_category != "coast" ){
    vcl_cout << " image_category is " << img_category << vcl_endl;
    vcl_cerr << " WARNING in lableme xml file: undefined img_category found\n";
  }

  // load the image size
  unsigned ni = parser.image_ni();
  unsigned nj = parser.image_nj();
  depth_scene->set_image_size(nj, ni);
  // push the depth_map_region into depth_scene in the order of defined order in xml
  for (unsigned i = 0; i < polys.size(); i++) {
    vsol_polygon_2d_sptr poly = bsol_algs::poly_from_vgl(polys[i]);
    if(object_types[i] == "sky") {
      depth_scene->set_sky(poly);
    } else {
      // check object type to define the region_normal
      if(object_types[i] == "hotel" || object_types[i] == "building") {
        double min_depth = parser.obj_mindists()[i];
        double max_depth = parser.obj_maxdists()[i];
        vgl_vector_3d<double> vp(1.0, 0.0, 0.0);
        depth_scene->add_region(poly, vp, min_depth, max_depth, object_names[i], depth_map_region::VERTICAL, object_orders[i]);
      }else if(object_types[i] == "road" || object_types[i] == "beach" || object_types[i] == "water" || object_types[i] == "desert" || object_types[i] == "flat") {
        double min_depth = parser.obj_mindists()[i];
        double max_depth = parser.obj_maxdists()[i];
        vgl_vector_3d<double> gp(0.0, 0.0, 1.0);//z axis is the plane normal temporary, flat object can have different normal values
        depth_scene->add_region(poly, gp, min_depth, max_depth, object_names[i], depth_map_region::GROUND_PLANE, object_orders[i]);
      }else {
        double min_depth = parser.obj_mindists()[i];
        double max_depth = parser.obj_maxdists()[i];
        vgl_vector_3d<double> np(1.0, 1.0, 1.0); 
        depth_scene->add_region(poly, np, min_depth, max_depth, object_names[i], depth_map_region::NOT_DEF, object_orders[i]);
      }
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
    default:
      file << "Unidentified status code!\n";
      vcl_cerr << "Unidentified status code!\n";
      break;
  }
  file << "<code>\n" << status_code << "\n</code>\n";
  file << "<log>\n" << log_message << "\n</log>\n";
  file << "</status>\n";
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
  if (status_code == volm_io::MATCHER_EXE_FINISHED) 
    return true;
  return false;
}


bool volm_io::write_log(vcl_string out_folder, vcl_string log)
{
  vcl_ofstream file;
  vcl_string out_file = out_folder + "/log.xml";
  file.open (out_file.c_str());
  file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<status>\n";
  file << "<log>\n"<<log<<"</log>\n";
  file.close();
  return true;
}

bool volm_io::write_composer_log(vcl_string out_folder, vcl_string log)
{
  vcl_ofstream file;
  vcl_string out_file = out_folder + "/composer_log.xml";
  file.open (out_file.c_str());
  file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<status>\n";
  file << "<log>\n"<<log<<"</log>\n";
  file.close();
  return true;
}


