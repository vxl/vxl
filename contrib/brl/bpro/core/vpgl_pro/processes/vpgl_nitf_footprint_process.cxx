// This is brl/bpro/core/vpgl_pro/processes/vpgl_nitf_footprint_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vpgl/file_formats/vpgl_nitf_rational_camera.h>
#include <vul/vul_file.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>

//: initialization
bool vpgl_nitf_footprint_process_cons(bprb_func_process& pro)
{
  //this process takes 2 inputs:
  // 1: (vcl_string) the filename containing a list of images to evaluate
  // 2: (vcl_string) the filename of the kml file to write footprints to

  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string");
  input_types.push_back("vcl_string");

  return pro.set_input_types(input_types);
}


// write kml header
void kml_init(vcl_ofstream &ofs)
{
  ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      << "<kml xmlns=\"http://www.opengis.net/kml/2.2\"><Document>\n"
      << vcl_endl;
  return;
}


void kml_poly(vcl_ofstream &ofs, vcl_string name, vcl_string description, vnl_double_2 ul, vnl_double_2 ur, vnl_double_2 ll, vnl_double_2 lr)
{
  ofs << "<Placemark>\n"
      << "  <name>" << name << "</name>\n"
      << "  <description>" << description << "</description>\n"
      << "  <Style>\n"
      << "    <PolyStyle>\n"
      << "      <colorMode>random</colorMode>\n"
      << "      <color>ffffffff</color>\n"
      << "      <fill>0</fill>\n"
      << "    </PolyStyle>\n"
      << "    <LineStyle>\n"
      << "      <colorMode>random</colorMode>\n"
      << "      <color>ffffffff</color>\n"
      << "      <width>3</width>\n"
      << "    </LineStyle>\n"
      << "  </Style>\n"
      << "  <Polygon>\n"
      << "   <tessellate>1</tessellate>\n"
      << "   <outerBoundaryIs>\n"
      << "     <LinearRing>\n"
      << "       <coordinates>\n"
      << "          "
      << ul[1] << ',' << ul[0] << ",0  "
      << ur[1] << ',' << ur[0] << ",0  "
      << lr[1] << ',' << lr[0] << ",0  "
      << ll[1] << ',' << ll[0] << ",0  "
      << ul[1] << ',' << ul[0] << ",0\n"
      << "       </coordinates>\n"
      << "     </LinearRing>\n"
      << "   </outerBoundaryIs>\n"
      << "  </Polygon>\n"
      << "</Placemark>\n" << vcl_endl;

  return;
}

void kml_finish(vcl_ofstream &ofs)
{
  ofs << "</Document></kml>" << vcl_endl;
}

//: Execute the process
bool vpgl_nitf_footprint_process(bprb_func_process& pro)
{
  if (pro.n_inputs() != 2) {
    vcl_cout << "vpgl_nitf_footprint_process: Number of inputs is " << pro.n_inputs() << ", should be 2" << vcl_endl;
    return false;
  }

  // get the inputs
  vcl_string in_img_list = pro.get_input<vcl_string>(0);
  vcl_string footprint_filename = pro.get_input<vcl_string>(1);

  vcl_ofstream ofs(footprint_filename.c_str());
  if (!ofs.good()) {
    vcl_cerr << "ERROR: vpgl_nitf_footprint_process : Error opening " << footprint_filename << " for write.\n";
    return false;
  }

  kml_init(ofs);

  vcl_ifstream ifs(in_img_list.c_str());
  vcl_string nitf_path;

  while (ifs.good()) {
    ifs >> nitf_path;

    vil_image_resource_sptr image =
        vil_load_image_resource(nitf_path.c_str());
    if (!image)
    {
      vcl_cout << "NITF image load failed in vpgl_nitf_footprint_process\n";
      return false;
    }

    vcl_string format = image->file_format();
    vcl_string prefix = format.substr(0,4);

    if (prefix != "nitf")
    {
      vcl_cout << "source image is not NITF in vpgl_nitf_footprint_process\n";
      return false;
    }

    //cast to an nitf2_image
    vil_nitf2_image *nitf_image = static_cast<vil_nitf2_image*>(image.ptr());
    vpgl_nitf_rational_camera *nitf_cam=new vpgl_nitf_rational_camera(nitf_image, true);

    vnl_double_2 ul = nitf_cam->upper_left();
    vnl_double_2 ur = nitf_cam->upper_right();
    vnl_double_2 ll = nitf_cam->lower_left();
    vnl_double_2 lr = nitf_cam->lower_right();

    vcl_string nitf_id = vul_file::strip_directory(nitf_path);
    vcl_string desc = nitf_path + " footprint";

    kml_poly(ofs, nitf_id, desc, ul, ur, ll, lr);
  }
  kml_finish(ofs);

  return true;
}


//: initialization
bool vpgl_geo_footprint_process_cons(bprb_func_process& pro)
{
  //this process takes 4 inputs:
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vpgl_camera_double_sptr");  // as read from a geotiff image header or from a tfw file
  input_types.push_back("vcl_string");               // geotiff file, to determine image size
  input_types.push_back("vcl_string");               // the filename of the kml file to write footprints to
  input_types.push_back("bool");                     // put the tags do initialize and end the document, if writing only one file these are needed, otherwise add manually to the file
  return pro.set_input_types(input_types);
}

//: Execute the process
bool vpgl_geo_footprint_process(bprb_func_process& pro)
{
  if (pro.n_inputs() != 4) {
    vcl_cout << "vpgl_geo_footprint_process: Number of inputs is " << pro.n_inputs() << ", should be 2" << vcl_endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(0);
  vcl_string geotiff_filename = pro.get_input<vcl_string>(1);
  vil_image_resource_sptr img_res = vil_load_image_resource(geotiff_filename.c_str());
  vcl_string footprint_filename = pro.get_input<vcl_string>(2);
  bool init_finish = pro.get_input<bool>(3);
  unsigned ni = img_res->ni(); unsigned nj = img_res->nj();

  vcl_ofstream ofs(footprint_filename.c_str(), vcl_ios::app);
  if (!ofs.good()) {
    vcl_cerr << "ERROR: vpgl_geo_footprint_process : Error opening " << footprint_filename << " for write.\n";
    return false;
  }

  if (init_finish)
    kml_init(ofs);

  vpgl_geo_camera* geocam=dynamic_cast<vpgl_geo_camera*> (cam.ptr());
  if (!geocam) {
    vcl_cerr << "In vpgl_geo_footprint_process() - input camera is not valid!\n";
    return false;
  }
  
  double lon, lat, elev;
  geocam->img_to_wgs(0,0,0,lon,lat,elev);
  vnl_double_2 ul; ul[0] = lat; ul[1] = lon;

  geocam->img_to_wgs(0,nj,0,lon,lat,elev);
  vnl_double_2 ur; ur[0] = lat; ur[1] = lon;
  
  geocam->img_to_wgs(ni,0,0,lon,lat,elev);
  vnl_double_2 ll; ll[0] = lat; ll[1] = lon; 
  
  geocam->img_to_wgs(ni,nj,0,lon,lat,elev);
  vnl_double_2 lr; lr[0] = lat; lr[1] = lon;

  vcl_string g_id = vul_file::strip_directory(geotiff_filename);
  vcl_string desc = geotiff_filename + " footprint";

  kml_poly(ofs, g_id, desc, ul, ur, ll, lr);
  
  if (init_finish)
    kml_finish(ofs);

  return true;
}


