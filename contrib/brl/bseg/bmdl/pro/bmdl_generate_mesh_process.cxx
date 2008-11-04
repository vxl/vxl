#include "bmdl_generate_mesh_process.h"
//:
// \file

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_string.h>

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

#include <vpgl/bgeo/bgeo_lvcs_sptr.h>
#include <vpgl/bgeo/bgeo_lvcs.h>

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
    vcl_cout << "bmdl_generate_mesh_process -- Label image is not set!\n";
    return false;
  }

  // height image
  brdb_value_t<vil_image_view_base_sptr>* input2 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[2].ptr());
  vil_image_view_base_sptr height_img = input2->value();

  if (!height_img) {
    vcl_cout << "bmdl_generate_mesh_process -- Label image is not set!\n";
    return false;
  }

  // ground image
  brdb_value_t<vil_image_view_base_sptr>* input3 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[3].ptr());
  vil_image_view_base_sptr ground_img = input3->value();

  if (!ground_img) {
    vcl_cout << "bmdl_generate_mesh_process -- Label image is not set!\n";
    return false;
  }

  brdb_value_t<vcl_string>* input4 =
    static_cast<brdb_value_t< vcl_string>* >(input_data_[4].ptr());
  vcl_string file_mesh = input4->value();

  brdb_value_t<vpgl_camera_double_sptr>* input5 =
    static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(input_data_[5].ptr());
  vpgl_camera_double_sptr camera = input5->value();

  if (camera->type_name().compare("vpgl_geo_camera") != 0) {
    vcl_cout << "bmdl_generate_mesh_process -- Camera input should be type of lidar!\n";
    return false;
  }

  vpgl_geo_camera* lidar_cam = static_cast<vpgl_geo_camera*>(camera.ptr());

  generate_mesh(file_poly, label_img, height_img, ground_img, file_mesh, lidar_cam);

  return true;
}

bool
bmdl_generate_mesh_process::generate_mesh(vcl_string fpath_poly,
                                          vil_image_view_base_sptr label_img,
                                          vil_image_view_base_sptr height_img,
                                          vil_image_view_base_sptr ground_img,
                                          vcl_string fpath_mesh,
                                          vpgl_geo_camera* const lidar_cam)
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
  generate_kml_collada(fpath_mesh, mesh, lidar_cam);

  return true;
}

void bmdl_generate_mesh_process::update_mesh_coord(imesh_mesh& imesh,
                                                   vpgl_geo_camera* cam)
{
  imesh_vertex_array<3>& vertices = imesh.vertices<3>();
  for (unsigned v=0; v<vertices.size(); v++) {
    double x = vertices(v,0);
    double y = vertices(v,1);
    double z = vertices(v,2);
    bgeo_lvcs_sptr lvcs = cam->lvcs();
    double lon, lat, elev;
    cam->img_to_wgs(x, y, z, lon,lat, elev);
    vertices[v][0] = lon;
    vertices[v][1] = lat;
    vertices[v][2] = elev;
  }
}

void bmdl_generate_mesh_process::generate_kml(vcl_string& kml_filename,
                                              imesh_mesh& mesh,
                                              vpgl_geo_camera* lidar_cam)
{
  vcl_ofstream os(kml_filename.c_str());

  os << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
     << "<kml xmlns=\"http://earth.google.com/kml/2.1\">\n"
     << "<Document>\n"
     << "  <name>" << vul_file::strip_directory(kml_filename.c_str()) << "</name>\n"
     << "  <open>1</open>\n"
     << "  <Placemark>\n"
  // << "    <name>" << model_name.c_str() << "</name>\n"
     << "    <visibility>1</visibility>\n"
     << "    <MultiGeometry>\n";

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

  os << "    </MultiGeometry>\n"
     << "  </Placemark>\n"
     << "</Document>\n"
     << "</kml>\n";

  os.close();
  imesh_write_obj("meshes.obj", mesh);
}

void bmdl_generate_mesh_process::generate_kml_collada(vcl_string& kmz_dir,
                                                      imesh_mesh& mesh,
                                                      vpgl_geo_camera* lidar_cam)
{

  double origin_lat = 0,origin_lon = 0, origin_elev = 0;
  double lox, loy,theta;
  lidar_cam->lvcs()->get_origin(origin_lat,origin_lon,origin_elev);
  lidar_cam->lvcs()->get_transform(lox, loy,theta);
  // write each building into kml
  if (!mesh.has_half_edges())
    mesh.build_edge_graph();

  imesh_half_edge_set he = mesh.half_edges();

  vcl_vector<imesh_mesh> buildings;
  // guess at ground height = lowest vertex
  double minz = 1e6;
  vcl_vector<vcl_set<unsigned int> > cc = imesh_detect_connected_components(he);

  for (unsigned i=0; i<cc.size(); i++) {
    vcl_set<unsigned int> sel_faces = cc[i];
    buildings.push_back(imesh_submesh_from_faces(mesh, sel_faces));
    imesh_triangulate(buildings.back());
    buildings.back().compute_face_normals();
    const imesh_face_array_base& faces = buildings.back().faces();
    const imesh_vertex_array_base& verts = buildings.back().vertices();

    for (unsigned int f=0; f<faces.size(); ++f) {
      for (unsigned int v=0; v<faces.num_verts(f); ++v) {
        unsigned int idx = faces(f,v);
        double vz = verts(idx, 2);
        if (vz < minz)
          minz = vz;
      }
    }
  }

  double ground_height = minz;
  vcl_string model_name;

  if (kmz_dir == "") {
    vcl_cerr << "Error: no filename selected.\n";
    return;
  }

  if (!vul_file::is_directory(kmz_dir)) {
    vcl_cerr << "Error: Select a directory name.\n";
    return;
  }

  int nobjects = 0;
  unsigned min_faces = 3;
      
  for (unsigned idx=0; idx<buildings.size(); idx++) {
    const imesh_mesh& building = buildings[idx];

    if ( building.num_faces() <  min_faces)
      // single mesh face is probably ground plane, which we do not want to render
      continue;

    vcl_stringstream ss;
    ss << "structure_" << idx;
    vcl_string kml_fname = kmz_dir + "/" + ss.str() + ".kml";
    vcl_string dae_fname = kmz_dir + "/" + ss.str() + ".dae";
    
    vcl_ofstream os (dae_fname.data());
    imesh_write_kml_collada(os, building);
    os.close();

    
    vcl_ofstream oskml(kml_fname.data());

    oskml << "<?xml version='1.0' encoding='UTF-8'?>\n";
    oskml << "<kml xmlns='http://earth.google.com/kml/2.1'>\n";
    oskml << "<Folder>\n";
    oskml << "  <name>" << ss.str() << "</name>\n";
    oskml << "  <description><![CDATA[Created with <a href=\"http://sourceforge.vxl.net\">VXL</a>]]></description>\n";
    oskml << "  <DocumentSource>Brown University imesh library</DocumentSource>\n";
    oskml << "  <visibility>1</visibility>\n";
    oskml << "  <LookAt>\n";
    oskml << "    <heading>0</heading>\n";
    oskml << "    <tilt>45</tilt>\n";
    oskml << "    <longitude>" << origin_lon << "</longitude>\n";
    oskml << "    <latitude>" << origin_lat << "</latitude>\n";
    oskml << "    <range>200</range>\n";
    oskml << "    <altitude>" << 0.0f << "</altitude>\n";
    oskml << "    <altitudeMode>absolute</altitudeMode>\n";//absolute
    oskml << "  </LookAt>\n";
    oskml << "  <Folder>\n";
    oskml << "    <name>Tour</name>\n";
    oskml << "    <Placemark>\n";
    oskml << "      <name>Camera</name>\n";
    oskml << "      <visibility>1</visibility>\n";
    oskml << "    </Placemark>\n";
    oskml << "  </Folder>\n";
    oskml << "  <Placemark>\n";
    oskml << "    <name>Model</name>\n";
    oskml << "    <description><![CDATA[]]></description>\n";
    oskml << "    <Style id='default'>\n";
    oskml << "    </Style>\n";
    oskml << "    <Model>\n";
    oskml << "      <altitudeMode>absolute</altitudeMode>\n";
    oskml << "      <Location>\n";
    oskml << "        <longitude>" << origin_lon  << "</longitude>\n";
    oskml << "        <latitude>" << origin_lat  << "</latitude>\n";
    oskml << "        <altitude>" << origin_elev - ground_height << "</altitude>\n"; 
    oskml << "      </Location>\n";
    oskml << "      <Orientation>\n";
    oskml << "        <heading>" << lox << "</heading>\n";
    oskml << "        <tilt>" << loy << "</tilt>\n";
    oskml << "        <roll>" << theta << "</roll>\n";
    oskml << "      </Orientation>\n";
    oskml << "      <Scale>\n";
    oskml << "        <x>1.0</x>\n";
    oskml << "        <y>-1.0</y>\n";  // y axis is in the opposite direction
    oskml << "        <z>1.0</z>\n";
    oskml << "      </Scale>\n";
    oskml << "      <Link>\n";
    oskml << "        <href>" << vul_file::strip_directory(dae_fname) << "</href>\n";
    oskml << "      </Link>\n";
    oskml << "    </Model>\n";
    oskml << "  </Placemark>\n";
    oskml << "</Folder>\n";
    oskml << "</kml>\n";
    oskml.close();
  }

}