//:
// \file
#include <bprb/bprb_func_process.h>

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_ctime.h>
#include <vcl_cstdio.h> // for std::fclose()
#include <vcl_cassert.h>

#include <vgl/vgl_polygon.h>
#include <vgl/io/vgl_io_polygon.h>

#include <vpl/vpl.h>

#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_convert.h>

#include <bmdl/bmdl_mesh.h>

#include <imesh/imesh_vertex.h>
#include <imesh/imesh_fileio.h>
#include <imesh/imesh_detection.h>
#include <imesh/imesh_operations.h>
#include <imesh/algo/imesh_operations.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vpgl/vpgl_lvcs_sptr.h>
#include <vpgl/vpgl_lvcs.h>
#include <vul/vul_file.h>

#if HAS_ZLIB
#include <minizip/zip.h>
#endif

#define WRITEBUFFERSIZE (16384)


void update_mesh_coord(imesh_mesh& imesh, vpgl_geo_camera* cam)
{
  imesh_vertex_array<3>& vertices = imesh.vertices<3>();
  for (unsigned v=0; v<vertices.size(); v++) {
    unsigned int x = (unsigned int)vertices(v,0); // explicit cast from double
    unsigned int y = (unsigned int)vertices(v,1);
    unsigned int z = (unsigned int)vertices(v,2);
    vpgl_lvcs_sptr lvcs = cam->lvcs();
    double lon, lat, elev;
    cam->img_to_wgs(x, y, z, lon, lat, elev);
    vertices[v][0] = lon;
    vertices[v][1] = lat;
    vertices[v][2] = elev;
  }
}

void generate_kml(vcl_string& kml_filename,
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


//: Write the KML file for wrapping a COLLADA mesh
// \a lookat and \a location are (long,lat,alt) vectors
// \a orientation is a (heading,tilt,roll) vector
void
write_kml_collada_wrapper(vcl_ostream& os,
                          const vcl_string& object_name,
                          const vgl_point_3d<double>& lookat,
                          const vgl_point_3d<double>& location,
                          const vgl_point_3d<double>& orientation,
                          const vcl_string& filename)
{
  os.precision(16);
  os << "<?xml version='1.0' encoding='UTF-8'?>\n"
     << "<kml xmlns='http://earth.google.com/kml/2.1'>\n"
     << "<Folder>\n"
     << "  <name>" << object_name << "</name>\n"
     << "  <description><![CDATA[Created with <a href=\"http://sourceforge.vxl.net\">VXL</a>]]></description>\n"
     << "  <DocumentSource>Brown University imesh library</DocumentSource>\n"
     << "  <visibility>1</visibility>\n"
     << "  <LookAt>\n"
     << "    <heading>0</heading>\n"
     << "    <tilt>45</tilt>\n"
     << "    <longitude>" << lookat.x() << "</longitude>\n"
     << "    <latitude>" << lookat.y() << "</latitude>\n"
     << "    <altitude>" << lookat.z() << "</altitude>\n"
     << "    <range>200</range>\n"
     << "    <altitudeMode>absolute</altitudeMode>\n"
     << "  </LookAt>\n"
     << "  <Folder>\n"
     << "    <name>Tour</name>\n"
     << "    <Placemark>\n"
     << "      <name>Camera</name>\n"
     << "      <visibility>1</visibility>\n"
     << "    </Placemark>\n"
     << "  </Folder>\n"
     << "  <Placemark>\n"
     << "    <name>Model</name>\n"
     << "    <description><![CDATA[]]></description>\n"
     << "    <Style id='default'>\n"
     << "    </Style>\n"
     << "    <Model>\n"
     << "      <altitudeMode>relativeToGround</altitudeMode>\n"
     << "      <Location>\n"
     << "        <longitude>" << location.x()  << "</longitude>\n"
     << "        <latitude>" << location.y()  << "</latitude>\n"
     << "        <altitude>" << location.z() << "</altitude>\n"
     << "      </Location>\n"
     << "      <Orientation>\n"
     << "        <heading>" << orientation.x() << "</heading>\n"
     << "        <tilt>" << orientation.y() << "</tilt>\n"
     << "        <roll>" << orientation.z() << "</roll>\n"
     << "      </Orientation>\n"
     << "      <Scale>\n"
     << "        <x>1.0</x>\n"
     << "        <y>1.0</y>\n"
     << "        <z>1.0</z>\n"
     << "      </Scale>\n"
     << "      <Link>\n"
     << "        <href>" << filename << "</href>\n"
     << "      </Link>\n"
     << "    </Model>\n"
     << "  </Placemark>\n"
     << "</Folder>\n"
     << "</kml>\n";
}
#if (HAS_ZLIB)
int zip_kmz(zipFile& zf, const char* filenameinzip)
{
  FILE * fin;
  int size_read;
  zip_fileinfo zi;
  unsigned long crcFile=0;
  int size_buf=0;
  void* buf=NULL;

  zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
  zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
  zi.dosDate = 0;
  zi.internal_fa = 0;
  zi.external_fa = 0;

  // Get time in seconds since Jan 1 1970
  vcl_time_t time_secs;
  vcl_time(&time_secs);

  // Convert time to struct tm form
  struct vcl_tm *time;
  time = vcl_localtime(&time_secs);
  zi.tmz_date.tm_sec  = time->tm_sec;
  zi.tmz_date.tm_min  = time->tm_min;
  zi.tmz_date.tm_hour = time->tm_hour;
  zi.tmz_date.tm_mday = time->tm_mday;
  zi.tmz_date.tm_mon  = time->tm_mon ;
  zi.tmz_date.tm_year = time->tm_year;

  int err = 0;
  int opt_compress_level=Z_DEFAULT_COMPRESSION;
  const char* password=0;

  err = zipOpenNewFileInZip3(zf,filenameinzip,&zi,
                             NULL,0,NULL,0,NULL ,
                             (opt_compress_level != 0) ? Z_DEFLATED : 0,
                             opt_compress_level,0,
                             -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
                             password,crcFile);

  if (err != ZIP_OK) {
    vcl_printf("error in opening %s in zipfile\n",filenameinzip);
    return 0;
  }
  else {
    fin = vcl_fopen(filenameinzip,"rb");
    if (fin==NULL) {
        err=ZIP_ERRNO;
        vcl_printf("error in opening %s for reading\n",filenameinzip);
        return 0;
    }
  }

  size_buf = WRITEBUFFERSIZE;
  buf = (void*)malloc(size_buf);
  if (buf==NULL) {
    vcl_printf("Error allocating memory\n");
    return ZIP_INTERNALERROR;
  }

  do {
    err = ZIP_OK;
    size_read = (int)vcl_fread(buf,1,size_buf,fin);
    if (size_read < size_buf)
      if (feof(fin)==0) {
      vcl_printf("error in reading %s\n",filenameinzip);
      err = ZIP_ERRNO;
    }

    if (size_read>0) {
      err = zipWriteInFileInZip (zf,buf,size_read);
      if (err<0) {
          vcl_printf("error in writing %s in the zipfile\n", filenameinzip);
      }
    }
  } while ((err == ZIP_OK) && (size_read>0));

  if (fin)
    vcl_fclose(fin);

  if (err<0)
    err=ZIP_ERRNO;
  else {
    err = zipCloseFileInZip(zf);
    if (err!=ZIP_OK)
      vcl_printf("error in closing %s in the zipfile\n", filenameinzip);
  }

  int errclose = zipClose(zf,NULL);

  if (errclose != ZIP_OK) {
    vcl_printf("error in closing zipfile\n");
    return 0;
  }
  free(buf);
  return ZIP_OK;
}
#endif

void generate_kml_collada(vcl_string& kmz_dir,
                          imesh_mesh& mesh,
                          vpgl_geo_camera* lidar_cam,
                          unsigned& num_of_buildings)
{
  if (kmz_dir == "") {
    vcl_cerr << "Error: no filename selected.\n";
    return;
  }

  if (!vul_file::is_directory(kmz_dir)) {
    vcl_cerr << "Error: Select a directory name.\n";
    return;
  }

  vul_file::change_directory(kmz_dir);
  double origin_lat = 0,origin_lon = 0, origin_elev = 0;
  double lox, loy,theta;
  lidar_cam->lvcs()->get_origin(origin_lat,origin_lon,origin_elev);
  lidar_cam->lvcs()->get_transform(lox, loy,theta);
  // write each building into kml
  if (!mesh.has_half_edges())
    mesh.build_edge_graph();

  imesh_half_edge_set he = mesh.half_edges();

  vcl_string model_name;

  vcl_vector<vcl_set<unsigned int> > cc = imesh_detect_connected_components(he);

  for (unsigned i=0; i<cc.size(); i++) {
    vcl_set<unsigned int> sel_faces = cc[i];
    imesh_mesh building(imesh_submesh_from_faces(mesh, sel_faces));
    imesh_triangulate_nonconvex(building);
    building.compute_face_normals();

    // find the mean (x,y) and minimum z coordinates
    double minz = vcl_numeric_limits<double>::infinity();
    double meanx = 0.0, meany = 0.0;
    const imesh_vertex_array<3>& verts = building.vertices<3>();
    for (unsigned int v=0; v<verts.size(); ++v) {
      if (verts[v][2] < minz) {
        minz = verts[v][2];
      }
      meanx += verts[v][0];
      meany += verts[v][1];
    }
    meanx /= verts.size();
    meany /= verts.size();

    double lon, lat, elev;
    //assert(meanx >= 0 && meany <= 0 && minz >= 0);
    lidar_cam->img_to_wgs((unsigned int)meanx, (unsigned int)(-meany), (unsigned int)minz, lon, lat, elev);

    vcl_stringstream ss;
    ss << "structure_" << i+num_of_buildings;
    vcl_string kml_fname = ss.str() + ".kml";
    vcl_string dae_fname = ss.str() + ".dae";

    vcl_ofstream os (dae_fname.data());
    imesh_write_kml_collada(os, building);
    os.close();

    vcl_ofstream oskml(kml_fname.data());
    vgl_point_3d<double> lookat(lon, lat, elev);
    vgl_point_3d<double> location(origin_lon, origin_lat, -minz);
    vgl_point_3d<double> orientation(lox,loy,theta);
    write_kml_collada_wrapper(oskml,ss.str(),lookat,location,orientation,
                              vul_file::strip_directory(dae_fname));
    oskml.close();

#if (HAS_ZLIB)
    vcl_string zip_fname = ss.str() + ".kmz";
    zipFile zipf = zipOpen(zip_fname.c_str(), APPEND_STATUS_CREATE);
    zip_kmz(zipf, dae_fname.c_str());
    zipf = zipOpen(zip_fname.c_str(), APPEND_STATUS_ADDINZIP);
    zip_kmz(zipf, kml_fname.c_str());
    vpl_unlink(kml_fname.c_str());
    vpl_unlink(dae_fname.c_str());
#endif
  }
  num_of_buildings += cc.size();
}


bool generate_mesh(vcl_string fpath_poly,
                   vil_image_view_base_sptr label_img,
                   vil_image_view_base_sptr height_img,
                   vil_image_view_base_sptr ground_img,
                   vcl_string fpath_mesh,
                   vpgl_geo_camera* const lidar_cam,
                   unsigned& num_of_buildings)
{
  if (label_img->pixel_format() != VIL_PIXEL_FORMAT_UINT_32) {
    vcl_cout << "bmdl_generate_mesh_process::the Label Image pixel format" << label_img->pixel_format() << " undefined" << vcl_endl;
    return false;
  }
  vil_image_view<vxl_uint_32> labels(label_img);

  vil_image_view<double> heights;
  if (height_img->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_convert_cast(vil_image_view<float>(*height_img), heights);
  }
  else if (height_img->pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) {
    heights = static_cast<vil_image_view<double> >(height_img.ptr());
  }
  else {
    vcl_cout << "bmdl_generate_mesh_process::the Height Image pixel format" << height_img->pixel_format() << " undefined" << vcl_endl;
    return false;
  }

  vil_image_view<double> ground;
  if (ground_img->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_convert_cast(vil_image_view<float>(*ground_img), ground);
  }
  else if (ground_img->pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) {
    ground = static_cast<vil_image_view<double> >(ground_img.ptr());
  }
  else {
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

  vcl_vector<bmdl_edge> edges;
  vcl_vector<bmdl_region> regions;
  unsigned int num_joints = bmdl_mesh::link_boundary_edges(labels, boundaries,
                                                           edges, regions);
  bmdl_mesh::simplify_edges(edges);
  imesh_mesh mesh;
  bmdl_mesh::mesh_lidar(edges, regions, num_joints, labels,
                        heights, ground, mesh);

  generate_kml_collada(fpath_mesh, mesh, lidar_cam, num_of_buildings);
  return true;
}

bool bmdl_generate_mesh_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 6) {
    vcl_cout << "lidar_roi_process: The input number should be 6" << vcl_endl;
    return false;
  }

  unsigned int i=0;
  vcl_string file_poly = pro.get_input<vcl_string>(i++);
  vil_image_view_base_sptr label_img = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr height_img = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr ground_img = pro.get_input<vil_image_view_base_sptr>(i++);
  vcl_string file_mesh = pro.get_input<vcl_string>(i++);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);

  if (!label_img) {
    vcl_cout << "bmdl_generate_mesh_process -- Label image is not set!\n";
    return false;
  }

  if (!height_img) {
    vcl_cout << "bmdl_generate_mesh_process -- Label image is not set!\n";
    return false;
  }

  if (!ground_img) {
    vcl_cout << "bmdl_generate_mesh_process -- Label image is not set!\n";
    return false;
  }

  if (camera->type_name().compare("vpgl_geo_camera") != 0) {
    vcl_cout << "bmdl_generate_mesh_process -- Camera input should be type of lidar!\n";
    return false;
  }

  vpgl_geo_camera* lidar_cam = static_cast<vpgl_geo_camera*>(camera.ptr());
  unsigned num_of_buildings=0;
  return generate_mesh(file_poly, label_img, height_img, ground_img, file_mesh, lidar_cam, num_of_buildings);
}

bool bmdl_generate_mesh_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string");
  input_types.push_back("vil_image_view_base_sptr");
  input_types.push_back("vil_image_view_base_sptr");
  input_types.push_back("vil_image_view_base_sptr");
  input_types.push_back("vcl_string");
  input_types.push_back("vpgl_camera_double_sptr");
  return pro.set_input_types(input_types);
}
