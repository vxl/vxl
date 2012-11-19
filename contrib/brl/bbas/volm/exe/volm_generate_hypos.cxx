//:
// \file
// \brief executable to generate hypothesis locations using DEMs or LIDAR
// \author Ozge C. Ozcanli
// \date Nov 15, 2012

#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <volm/volm_loc_hyp.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vpgl/vpgl_utm.h>
#include <vpgl/vpgl_lvcs.h>
#include <bkml/bkml_parser.h>

#if 0
// generate gt hypos
int main(int argc,  char** argv)
{
  vul_arg<vcl_string> in_poly("-poly", "region polygon as kml, only the hypos inside this will be added", "");
  vul_arg<unsigned> inc_i("-inc_i", "increments in pixels in the DEM image", 1);
  vul_arg<unsigned> inc_j("-inc_j", "increments in pixels in the DEM image", 1);
  vul_arg<vcl_string> out_pre("-out_pre", "output file prefix", "");
  vul_arg_parse(argc, argv);

  vcl_cout << "argc: " << argc << vcl_endl;
  if (out_pre().compare("") == 0 || in_poly().compare("") == 0) {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vcl_cout << "will use increments " << inc_i() << " pixels along i and " << inc_j() << " pixels along j in the DEM image!\n";

  vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly());
  vcl_cout << "outer poly  has: " << poly[0].size() << vcl_endl;

  vpgl_lvcs_sptr dummy_lvcs = new vpgl_lvcs();

  volm_loc_hyp hyp;

#if 0 // add region 1 gt positions
  hyp.add(37.6763153076, -117.978889465, 1891.40698242);
  hyp.add(37.6763076782, -117.978881836, 1891.40698242);
  hyp.add(37.9629554749, -117.964019775, 1670.87597656);
  hyp.add(37.9623947144, -117.962753296, 1643.76293945);
  hyp.add(38.5223121643, -118.147850037, 1382.84594727);
  hyp.add(38.5202674866, -118.147697449, 1378.42004395);
  hyp.add(38.0597877502, -117.936508179, 1376.12097168);
  hyp.add(38.0598144531, -117.936523438, 1376.12097168);
  hyp.add(37.7081604004, -117.416557312, 1860.39599609);
  hyp.add(37.7084999084, -117.416664124, 1863.23205566);
  hyp.add(37.7082748413, -117.416511536, 1860.99194336);
#endif

  double lon2, lat2;
  cam->img_to_global(ni, nj, lon2, lat2);
      hyp.add(poly, img, cam, inc_i(), inc_j(), true, 'N', 'W');
    delete cam;
    vcl_cout << "after addition hyp has: " << hyp.size() << " elements\n";
  }
  vcl_stringstream str;
  str << out_pre() << '_' << inc_i() << '_' << inc_j() << ".bin";
  hyp.write_hypotheses(str.str());

  return volm_io::SUCCESS;
}
#endif


int main(int argc,  char** argv)
{
  vul_arg<vcl_string> in_folder("-in", "input folder to read files as .tif", "");
  vul_arg<vcl_string> in_poly("-poly", "region polygon as kml, only the hypos inside this will be added", "");
  vul_arg<unsigned> inc_i("-inc_i", "increments in pixels in the DEM image", 1);
  vul_arg<unsigned> inc_j("-inc_j", "increments in pixels in the DEM image", 1);
  vul_arg<vcl_string> out_pre("-out_pre", "output file prefix", "");
  vul_arg_parse(argc, argv);

  vcl_cout << "argc: " << argc << vcl_endl;
  if (in_folder().compare("") == 0 || out_pre().compare("") == 0 || in_poly().compare("") == 0) {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vcl_cout << "will use increments " << inc_i() << " pixels along i and " << inc_j() << " pixels along j in the DEM image!\n";

  vgl_polygon<double> poly = bkml_parser::parse_polygon(in_poly());
  vcl_cout << "outer poly  has: " << poly[0].size() << vcl_endl;

  vpgl_lvcs_sptr dummy_lvcs = new vpgl_lvcs();

  volm_loc_hyp hyp;

  vcl_string file_glob = in_folder() + "/*.tif";
  for (vul_file_iterator fn=file_glob; fn; ++fn) {
    vcl_string tiff_fname = fn();
    vcl_cout << tiff_fname << vcl_endl;

    vil_image_view_base_sptr img_sptr = vil_load(tiff_fname.c_str());
    vil_image_view<float> img(img_sptr);
    unsigned ni = img.ni(); unsigned nj = img.nj();
    vcl_cout << " tile size: "<< ni << " x " << nj << vcl_endl;

    // determine the translation matrix from the image file name and construct a geo camera
    vcl_string name = vul_file::strip_directory(tiff_fname);
    name = name.substr(name.find_first_of('_')+1, name.size());
    vcl_cout << "will determine transformation matrix from the file name: " << name << vcl_endl;
    vcl_string n = name.substr(name.find_first_of('N')+1, name.find_first_of('W'));
    float lon, lat, scale;
    vcl_stringstream str(n); str >> lat;
    n = name.substr(name.find_first_of('W')+1, name.find_first_of('_'));
    vcl_stringstream str2(n); str2 >> lon;
    n = name.substr(name.find_first_of('x')+1, name.find_last_of('.'));
    vcl_stringstream str3(n); str3 >> scale;
    vcl_cout << " lat: " << lat << " lon: " << lon << " WARNING: using same scale for both ni and nj: scale:" << scale << vcl_endl;

    // determine the upper left corner to use a vpgl_geo_cam, subtract from lat
    vcl_cout << "upper left corner in the image is: " << lat+scale << " N " << lon << " W\n"
             << "lower right corner in the image is: " << lat << " N " << lon-scale << " W" << vcl_endl;
    vnl_matrix<double> trans_matrix(4,4,0.0);
    trans_matrix[0][0] = -scale/ni; trans_matrix[1][1] = -scale/nj;
    trans_matrix[0][3] = lon; trans_matrix[1][3] = lat+scale;
    vpgl_geo_camera* cam = new vpgl_geo_camera(trans_matrix, dummy_lvcs);
    cam->set_scale_format(true);
    double lon2, lat2;
    cam->img_to_global(ni, nj, lon2, lat2);
    vpgl_utm utm; double x, y; int zone; utm.transform(lat2, -lon2, x, y, zone);
    vcl_cout << "lower right corner in the image given by geocam is: " << lat2 << " N " << lon2 << " W " << " zone: " << zone << vcl_endl;

    hyp.add(poly, img, cam, inc_i(), inc_j(), true, 'N', 'W');
    delete cam;
    vcl_cout << "after addition hyp has: " << hyp.size() << " elements\n";
  }
  vcl_stringstream str;
  str << out_pre() << '_' << inc_i() << '_' << inc_j() << ".bin";
  hyp.write_hypotheses(str.str());

  return volm_io::SUCCESS;
}
