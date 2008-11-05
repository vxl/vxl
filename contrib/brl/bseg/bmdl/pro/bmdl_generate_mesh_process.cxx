#include "bmdl_generate_mesh_process.h"
//:
// \file

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_ctime.h>

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
#include <imesh/algo/imesh_operations.h>

#include <vpgl/bgeo/bgeo_lvcs_sptr.h>
#include <vpgl/bgeo/bgeo_lvcs.h>

#include <vul/vul_file.h>

//#if HAS_ZLIB
//#include <minizip/zip.h>
//#endif

#define WRITEBUFFERSIZE (16384)

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


//: Write the KML file for wrapping a COLLADA mesh
// \a lookat and \a location are (long,lat,alt) vectors
// \a orientation is a (heading,tilt,roll) vector
void bmdl_generate_mesh_process::
write_kml_collada_wrapper(vcl_ostream& os, 
                          const vcl_string& object_name,
                          const vgl_point_3d<double>& lookat,
                          const vgl_point_3d<double>& location,
                          const vgl_point_3d<double>& orientation,
                          const vcl_string& filename)
{
  os.precision(16);
  os << "<?xml version='1.0' encoding='UTF-8'?>\n";
  os << "<kml xmlns='http://earth.google.com/kml/2.1'>\n";
  os << "<Folder>\n";
  os << "  <name>" << object_name << "</name>\n";
  os << "  <description><![CDATA[Created with <a href=\"http://sourceforge.vxl.net\">VXL</a>]]></description>\n";
  os << "  <DocumentSource>Brown University imesh library</DocumentSource>\n";
  os << "  <visibility>1</visibility>\n";
  os << "  <LookAt>\n";
  os << "    <heading>0</heading>\n";
  os << "    <tilt>45</tilt>\n";
  os << "    <longitude>" << lookat.x() << "</longitude>\n";
  os << "    <latitude>" << lookat.y() << "</latitude>\n";
  os << "    <altitude>" << lookat.z() << "</altitude>\n";
  os << "    <range>200</range>\n";
  os << "    <altitudeMode>absolute</altitudeMode>\n";
  os << "  </LookAt>\n";
  os << "  <Folder>\n";
  os << "    <name>Tour</name>\n";
  os << "    <Placemark>\n";
  os << "      <name>Camera</name>\n";
  os << "      <visibility>1</visibility>\n";
  os << "    </Placemark>\n";
  os << "  </Folder>\n";
  os << "  <Placemark>\n";
  os << "    <name>Model</name>\n";
  os << "    <description><![CDATA[]]></description>\n";
  os << "    <Style id='default'>\n";
  os << "    </Style>\n";
  os << "    <Model>\n";
  os << "      <altitudeMode>relativeToGround</altitudeMode>\n";
  os << "      <Location>\n";
  os << "        <longitude>" << location.x()  << "</longitude>\n";
  os << "        <latitude>" << location.y()  << "</latitude>\n";
  os << "        <altitude>" << location.z() << "</altitude>\n"; 
  os << "      </Location>\n";
  os << "      <Orientation>\n";
  os << "        <heading>" << orientation.x() << "</heading>\n";
  os << "        <tilt>" << orientation.y() << "</tilt>\n";
  os << "        <roll>" << orientation.z() << "</roll>\n";
  os << "      </Orientation>\n";
  os << "      <Scale>\n";
  os << "        <x>1.0</x>\n";
  os << "        <y>1.0</y>\n"; 
  os << "        <z>1.0</z>\n";
  os << "      </Scale>\n";
  os << "      <Link>\n";
  os << "        <href>" << filename << "</href>\n";
  os << "      </Link>\n";
  os << "    </Model>\n";
  os << "  </Placemark>\n";
  os << "</Folder>\n";
  os << "</kml>\n";
}

  
void bmdl_generate_mesh_process::generate_kml_collada(vcl_string& kmz_dir,
                                                      imesh_mesh& mesh,
                                                      vpgl_geo_camera* lidar_cam)
{
  if (kmz_dir == "") {
    vcl_cerr << "Error: no filename selected.\n";
    return;
  }
  
  if (!vul_file::is_directory(kmz_dir)) {
    vcl_cerr << "Error: Select a directory name.\n";
    return;
  }
  
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
      if (verts[v][2] < minz){
        minz = verts[v][2];
      }
      meanx += verts[v][0];
      meany += verts[v][1];
    }
    meanx /= verts.size();
    meany /= verts.size();
    
    double lon, lat, elev;
    lidar_cam->img_to_wgs(meanx, -meany, minz, lon,lat, elev);
    
    vcl_stringstream ss;
    ss << "structure_" << i;
    vcl_string kml_fname = kmz_dir + "/" + ss.str() + ".kml";
    vcl_string dae_fname = kmz_dir + "/" + ss.str() + ".dae";
    
    vcl_string zip_fname = kmz_dir + "/" + ss.str() + ".kmz";
    zipFile zipf = zipOpen(zip_fname.c_str(), APPEND_STATUS_CREATE);
    
    vcl_ofstream os (dae_fname.data());
    imesh_write_kml_collada(os, building);
    os.close();
    zip_kmz(zipf, dae_fname.c_str());
    
    vcl_ofstream oskml(kml_fname.data());
    vgl_point_3d<double> lookat(lon, lat, elev);
    vgl_point_3d<double> location(origin_lon, origin_lat, -minz);
    vgl_point_3d<double> orientation(lox,loy,theta);
    write_kml_collada_wrapper(oskml,ss.str(),lookat,location,orientation,
                              vul_file::strip_directory(dae_fname));
    oskml.close();
    
    zipf = zipOpen(zip_fname.c_str(), APPEND_STATUS_ADDINZIP);
    zip_kmz(zipf, kml_fname.c_str());
  }

}


int bmdl_generate_mesh_process::zip_kmz(zipFile& zf, const char* filenameinzip)
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
    //filetime(filenameinzip,&zi.tmz_date,&zi.dosDate);

    int err = 0;
    int opt_compress_level=Z_DEFAULT_COMPRESSION;
    err = zipOpenNewFileInZip(zf,filenameinzip,&zi,
                     NULL,0,NULL,0,NULL,
                     (opt_compress_level != 0) ? Z_DEFLATED : 0,
                     opt_compress_level);


    /*err = zipOpenNewFileInZip3(zf,filenameinzip,&zi,
                     NULL,0,NULL,0,NULL ,
                     (opt_compress_level != 0) ? Z_DEFLATED : 0,
                     opt_compress_level,0,
                     -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
                     password,crcFile);*/

    if (err != ZIP_OK)
        printf("error in opening %s in zipfile\n",filenameinzip);
    else
    {
        
        fin = fopen(filenameinzip,"rb");
        if (fin==NULL)
        {
            err=ZIP_ERRNO;
            printf("error in opening %s for reading\n",filenameinzip);
        }
    }

    if (err == ZIP_OK)
        do
        {
            err = ZIP_OK;
            size_buf = WRITEBUFFERSIZE;
            buf = (void*)malloc(size_buf);
            if (buf==NULL)
            {
              printf("Error allocating memory\n");
              return ZIP_INTERNALERROR;
            }

            size_read = (int)fread(buf,1,size_buf,fin);
            if (size_read < size_buf)
                if (feof(fin)==0)
            {
                printf("error in reading %s\n",filenameinzip);
                err = ZIP_ERRNO;
            }

            if (size_read>0)
            {
                err = zipWriteInFileInZip (zf,buf,size_read);
                if (err<0)
                {
                    printf("error in writing %s in the zipfile\n",
                                     filenameinzip);
                }

            }
        } while ((err == ZIP_OK) && (size_read>0));

    if (fin)
        fclose(fin);

    if (err<0)
        err=ZIP_ERRNO;
    else
    {
        err = zipCloseFileInZip(zf);
        if (err!=ZIP_OK)
            printf("error in closing %s in the zipfile\n",
                        filenameinzip);
    }

    int errclose = zipClose(zf,NULL);
    if (errclose != ZIP_OK)
      printf("error in closing zipfile \n");
  return ZIP_OK;
}
