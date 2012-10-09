#include "volm_io.h"
#include <bkml/bkml_parser.h>
#include <bpgl/bpgl_camera_utils.h>
#include <bvgl/bvgl_labelme_parser.h>
#include <depth_map/depth_map_region_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <bsol/bsol_algs.h>
#include <vcl_iomanip.h>
#include <vcl_iostream.h>

//:
// \file
#include <vcl_cassert.h>

bool volm_io::read_camera(vcl_string kml_file, vpgl_perspective_camera<double>& cam, unsigned ni, unsigned nj) {
  bkml_parser* parser = new bkml_parser();
  vcl_FILE* xmlFile = vcl_fopen(kml_file.c_str(), "r");
  if (!xmlFile) {
    vcl_cerr << kml_file.c_str() << " error on opening\n";
    delete parser;
    return false;
  }
  if (!parser->parseFile(xmlFile)) {
    vcl_cerr << XML_ErrorString(parser->XML_GetErrorCode()) << " at line "
             << parser->XML_GetCurrentLineNumber() << '\n';

    delete parser;
    return false;
  }
  cam = bpgl_camera_utils::camera_from_kml((double)ni, (double)nj, parser->right_fov_, parser->top_fov_, parser->altitude_, parser->heading_, parser->tilt_, parser->roll_);   
    
  delete parser;
  return true;
}

bool volm_io::read_labelme(vcl_string xml_file, vpgl_perspective_camera<double> const& cam, depth_map_scene_sptr& depth_scene)
{
  try {
    bvgl_labelme_parser parser(xml_file);
    vcl_vector<vgl_polygon<double> > polys = parser.polygons();
    vcl_vector<vcl_string> objects = parser.obj_names();
    vcl_cout << " read " << polys.size() << " polygons for " << objects.size() << " objects from labelme file: " << xml_file << vcl_endl;
    for (unsigned i = 0; i < objects.size(); i++) {
      vcl_cout << "obj name: "<< objects[i] << vcl_endl;
    }
    for (unsigned i = 0; i < parser.obj_mindists().size(); i++) {
      vcl_cout << "obj dist: "<< parser.obj_mindists()[i] << vcl_endl;
    }
    for (unsigned i = 0; i < parser.obj_depth_orders().size(); i++) {
      vcl_cout << "obj depth order: "<< parser.obj_depth_orders()[i] << vcl_endl;
    }
    if (polys.size() != objects.size()) 
      throw -1;
    /*
    depth_map_region_sptr sky, ground_plane;
    vcl_vector<depth_map_region_sptr> regions;
    for (unsigned i = 0; i < polys.size(); i++) {
      vsol_polygon_2d_sptr poly = bsol_algs::poly_from_vgl(polys[i]);
      depth_map_region_sptr reg = new depth_map_region(poly, plane, 
      depth_map_region(vsol_polygon_2d_sptr const& region, 
                   vgl_plane_3d<double> const& region_plane,
                   double min_depth, double max_depth,
                   vcl_string name,
                   depth_map_region::orientation orient); 
    }
    /*
     depth_map_scene(unsigned ni, unsigned nj,
                  vcl_string const& image_path,
                  vpgl_perspective_camera<double> const& cam,
                  depth_map_region_sptr const& ground_plane,
                  depth_map_region_sptr const& sky,
                  vcl_vector<depth_map_region_sptr> const& scene_regions);
    */
  } catch (int) {
    return false;
  }
  return true;
}

bool volm_io::write_status(vcl_string out_folder, int status_code, int percent)
{
  vcl_ofstream file;
  vcl_string out_file = out_folder + "/status.xml";
  file.open (out_file.c_str());
  file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  file << "<status>\n";
  switch (status_code) {
    case volm_io::CAM_FILE_IO_ERROR:
      file << "Camera FILE IO Error\n<percent>0</percent>\n"; break;
    case volm_io::EXE_ARGUMENT_ERROR:
      file << "Error in executable arguments\n<percent>0</percent>\n"; break;
    case volm_io::SUCCESS:
      file << "Completed Successfuly\n<percent>100</percent>\n"; break;
    case volm_io::EXE_STARTED:
      file << "Exe Started\n<percent>0</percent>\n"; break;
    case volm_io::EXE_RUNNING:
      file << "Exe Running\n<percent>\n" << percent << "\n</percent>\n"; break;
    case volm_io::LABELME_FILE_IO_ERROR:
      file << "LABELME FILE IO Error\n<percent>0</percent>\n"; break;
    default:
      file << "Unidentified status code!\n"; 
      vcl_cerr << "Unidentified status code!\n"; 
      break;
  }
  file << "</status>\n";
  file.close();
  return true;
}


vcl_vector<volm_tile> volm_tile::generate_p1_tiles()
{
  vcl_vector<volm_tile> p1_tiles;
  p1_tiles.push_back(volm_tile(37, 118, 'N', 'W'));
  p1_tiles.push_back(volm_tile(37, 119, 'N', 'W'));
  p1_tiles.push_back(volm_tile(38, 118, 'N', 'W'));
  p1_tiles.push_back(volm_tile(38, 119, 'N', 'W'));
  p1_tiles.push_back(volm_tile(30, 82, 'N', 'W'));
  p1_tiles.push_back(volm_tile(31, 81, 'N', 'W'));
  p1_tiles.push_back(volm_tile(31, 82, 'N', 'W'));
  p1_tiles.push_back(volm_tile(32, 80, 'N', 'W'));
  p1_tiles.push_back(volm_tile(32, 81, 'N', 'W'));
  p1_tiles.push_back(volm_tile(33, 78, 'N', 'W'));
  p1_tiles.push_back(volm_tile(33, 79, 'N', 'W'));
  p1_tiles.push_back(volm_tile(33, 80, 'N', 'W'));
  p1_tiles.push_back(volm_tile(34, 77, 'N', 'W'));
  p1_tiles.push_back(volm_tile(34, 78, 'N', 'W'));
  p1_tiles.push_back(volm_tile(34, 79, 'N', 'W'));
  p1_tiles.push_back(volm_tile(35, 76, 'N', 'W'));
  p1_tiles.push_back(volm_tile(35, 77, 'N', 'W'));
  p1_tiles.push_back(volm_tile(36, 76, 'N', 'W'));
  return p1_tiles;
}
 
vcl_string volm_tile::get_string()
{
  vcl_stringstream str;
  str << hemisphere_ << lat_ << direction_ << vcl_setfill('0') << vcl_setw(3) << lon_;
  return str.str(); 
}
