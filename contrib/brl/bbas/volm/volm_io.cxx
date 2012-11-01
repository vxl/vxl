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

bool volm_io::read_camera(vcl_string kml_file, vpgl_perspective_camera<double>& cam, unsigned ni, unsigned nj)
{
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
  bvgl_labelme_parser parser(xml_file);
  vcl_vector<vgl_polygon<double> > polys = parser.polygons();
  vcl_vector<vcl_string>& objects = parser.obj_names();
  vcl_vector<vcl_string>& object_types = parser.obj_types();
#if 0
  vcl_cout << " read " << polys.size() << " polygons for " << objects.size() << " objects from labelme file: " << xml_file << vcl_endl;
  for (unsigned i = 0; i < objects.size(); ++i)
    vcl_cout << "obj name: "<< objects[i] << vcl_endl;
  for (unsigned i = 0; i < object_types.size(); i++)
    vcl_cout << "obj type: " << object_types[i] << vcl_endl;
  for (unsigned i = 0; i < parser.obj_mindists().size(); ++i)
    vcl_cout << "obj min_dist: "<< parser.obj_mindists()[i] << ", \t max_dist = " << parser.obj_maxdists()[i] <<  vcl_endl; 
  for (unsigned i = 0; i < parser.obj_depth_orders().size(); ++i)
    vcl_cout << "obj depth order: "<< parser.obj_depth_orders()[i] << vcl_endl;
#endif
  if (polys.size() != objects.size()) {
    return false;
  }
  //: add differnt depth region into depth_map_scene
  for (unsigned i = 0; i < polys.size(); i++) {
    vsol_polygon_2d_sptr poly = bsol_algs::poly_from_vgl(polys[i]);
    if(objects[i] == "sky") {
      depth_scene->set_sky(poly);
    } else {
      //: check object type to define the region_normal
      if(object_types[i] == "mountain") {
        double min_depth = parser.obj_mindists()[i];
        double max_depth = parser.obj_maxdists()[i];
		vgl_vector_3d<double> np(1.0, 1.0, 1.0); 
		depth_scene->add_region(poly, np, min_depth, max_depth, objects[i], depth_map_region::NOT_DEF);
      } else if(object_types[i] == "vertical") {
        double min_depth = parser.obj_mindists()[i];
        double max_depth = parser.obj_maxdists()[i];
		vgl_vector_3d<double> vp(1.0, 0.0, 0.0);
        depth_scene->add_region(poly, vp, min_depth, max_depth, objects[i], depth_map_region::VERTICAL);
      } else if(object_types[i] == "flat") {
       double min_depth = parser.obj_mindists()[i];
       double max_depth = parser.obj_maxdists()[i];
       vgl_vector_3d<double> gp(0.0, 0.0, 1.0);//z axis is the plane normal temporary, flat object can have different normal values
       depth_scene->add_region(poly, gp, min_depth, max_depth, objects[i], depth_map_region::FLAT);
      } else {
       return false;
      }
    }
  }
  return true;
}

bool volm_io::write_status(vcl_string out_folder, int status_code, int percent)
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


