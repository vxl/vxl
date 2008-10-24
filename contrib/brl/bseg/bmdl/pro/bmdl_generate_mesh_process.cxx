#include "bmdl_generate_mesh_process.h"
//:
// \file

#include <vcl_iostream.h>

#include <vgl/vgl_polygon.h>
#include <vgl/io/vgl_io_polygon.h>

#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_convert.h>

#include <bprb/bprb_parameters.h>

#include <bmdl/bmdl_mesh.h>

#include <imesh/imesh_vertex.h>
#include <imesh/imesh_fileio.h>
#include <imesh/imesh_detection.h>
#include <imesh/imesh_operations.h>

#include <bgeo/bgeo_lvcs_sptr.h>
#include <bgeo/bgeo_lvcs.h>

#include <vul/vul_file.h>

bmdl_generate_mesh_process::bmdl_generate_mesh_process()
{
  //this process takes 1 input:
  input_data_.resize(6, brdb_value_sptr(0));
  input_types_.resize(6);

  int i=0;
  input_types_[i++] = "vcl_string";                 // file path (bin) for the polygons to be read
  input_types_[i++] = "vil_image_view_base_sptr";   // label image
  input_types_[i++] = "vil_image_view_base_sptr";   // height image
  input_types_[i++] = "vil_image_view_base_sptr";   // ground image
  input_types_[i++] = "vcl_string";                 // file path (ply2) for the meshes
  input_types_[i++] = "vpgl_camera_double_sptr";     // lidar camera 

  //output
  output_data_.resize(0,brdb_value_sptr(0));
  output_types_.resize(0);
}

bool bmdl_generate_mesh_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  // get the inputs:
  brdb_value_t<vcl_string>* input0 =
    static_cast<brdb_value_t< vcl_string>* >(input_data_[0].ptr());
  vcl_string file_poly = input0->value();

  // label image
  brdb_value_t<vil_image_view_base_sptr>* input1 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[1].ptr());
  vil_image_view_base_sptr label_img = input1->value();

  if (!label_img) {
    vcl_cout << "bmdl_classify_process -- Label image is not set!\n";
    return false;
  }

  // height image
  brdb_value_t<vil_image_view_base_sptr>* input2 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[2].ptr());
  vil_image_view_base_sptr height_img = input2->value();

  if (!height_img) {
    vcl_cout << "bmdl_classify_process -- Label image is not set!\n";
    return false;
  }

  // ground image
  brdb_value_t<vil_image_view_base_sptr>* input3 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[3].ptr());
  vil_image_view_base_sptr ground_img = input3->value();

  if (!ground_img) {
    vcl_cout << "bmdl_classify_process -- Label image is not set!\n";
    return false;
  }

  brdb_value_t<vcl_string>* input4 =
    static_cast<brdb_value_t< vcl_string>* >(input_data_[4].ptr());
  vcl_string file_mesh = input4->value();

  brdb_value_t<vpgl_camera_double_sptr>* input5 =
    static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(input_data_[5].ptr());
  vpgl_camera_double_sptr camera = input5->value();

  if (camera->type_name().compare("vpgl_lidar_camera") != 0) {
    vcl_cout << "bmdl_classify_process -- Camera input should be type of lidar!\n";
    return false;
  }

  vpgl_lidar_camera* lidar_cam = static_cast<vpgl_lidar_camera*>(camera.ptr());

  generate_mesh(file_poly, label_img, height_img, ground_img, file_mesh, lidar_cam);

  return true;
}

bool
bmdl_generate_mesh_process::generate_mesh(vcl_string fpath_poly,
                                          vil_image_view_base_sptr label_img,
                                          vil_image_view_base_sptr height_img,
                                          vil_image_view_base_sptr ground_img,
                                          vcl_string fpath_mesh,
                                          vpgl_lidar_camera* const lidar_cam)
{
  if (label_img->pixel_format() != VIL_PIXEL_FORMAT_UINT_32) {
    vcl_cout << "bmdl_generate_mesh_process::the Label Image pixel format" << label_img->pixel_format() << " undefined" << vcl_endl;
    return false;
  }
  vil_image_view<vxl_uint_32> labels(label_img);

  vil_image_view<double> heights;
  if (height_img->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_convert_cast(vil_image_view<float>(*height_img), heights);
  } else if (height_img->pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) {
    heights = static_cast<vil_image_view<double> >(height_img.ptr());
  } else {
    vcl_cout << "bmdl_generate_mesh_process::the Height Image pixel format" << height_img->pixel_format() << " undefined" << vcl_endl;
    return false;
  }

  vil_image_view<double> ground;
  if (ground_img->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_convert_cast(vil_image_view<float>(*ground_img), ground);
  } else if (ground_img->pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) {
    ground = static_cast<vil_image_view<double> >(ground_img.ptr());
  } else {
    vcl_cout << "bmdl_generate_mesh_process::the Ground Image pixel format" << ground_img->pixel_format() << " undefined" << vcl_endl;
    return false;
  }
  

  // read polygons
  vsl_b_ifstream os(fpath_poly);
  unsigned char ver; //version();
  vsl_b_read(os, ver);
  unsigned int size;
  vsl_b_read(os, size);
  vgl_polygon<double> polygon;
  vcl_vector<vgl_polygon<double> > boundaries;
  for (unsigned i = 0; i < size; i++) {
    vsl_b_read(os, polygon);
    boundaries.push_back(polygon);
  }

  imesh_mesh mesh;
  bmdl_mesh::mesh_lidar(boundaries , labels, heights, ground, mesh);

  generate_kml(fpath_mesh, mesh, lidar_cam);

  return true;
}

void bmdl_generate_mesh_process::update_mesh_coord(imesh_mesh& imesh, 
                                                   vpgl_lidar_camera* cam)
{
  imesh_vertex_array<3>& vertices = imesh.vertices<3>();
  
  for (unsigned v=0; v<vertices.size(); v++) {
    double x = vertices(v,0);
    double y = vertices(v,1);
    double z = vertices(v,2);
    bgeo_lvcs_sptr lvcs = cam->lvcs();
    double lon, lat, gz;
    lvcs->local_to_global(x,y,z,lvcs->get_cs_name(),lon,lat,gz);
    vertices[v][0] = lon;
    vertices[v][1] = lat;
    vertices[v][2] = gz;
  }
}

void bmdl_generate_mesh_process::generate_kml(vcl_string& kml_filename, 
                                              imesh_mesh& mesh,
                                              vpgl_lidar_camera* lidar_cam)
{
  vcl_ofstream os(kml_filename.c_str());

  os << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
  os << "<kml xmlns=\"http://earth.google.com/kml/2.1\">\n";
  os << "<Document>\n";
  os << "  <name>" << vul_file::strip_directory(kml_filename.c_str()) << "</name>\n";
  os << "  <open>1</open>\n";
  os << "  <Placemark>\n";
  //os << "    <name>" << model_name.c_str() << "</name>\n";
  os << "    <visibility>1</visibility>\n";
  os << "    <MultiGeometry>\n";

  // write each building into kml
  if (!mesh.has_half_edges())
    mesh.build_edge_graph();

  imesh_half_edge_set he = mesh.half_edges();
  vcl_vector<vcl_set<unsigned int> > cc = imesh_detect_connected_components(he);
  for (unsigned i=0; i<cc.size(); i++) {
    vcl_set<unsigned int> sel_faces = cc[i];
    imesh_mesh building = imesh_submesh_from_faces(mesh, sel_faces);
    update_mesh_coord(building, lidar_cam);
    imesh_write_kml(os, building);
  }

  os << "    </MultiGeometry>\n";
  os << "  </Placemark>\n";
  os << "</Document>\n";
  os << "</kml>\n";

  os.close();
  imesh_write_obj("meshes.obj", mesh);
}