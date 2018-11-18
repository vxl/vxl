//:
// \file
// \brief  executable to create a mask in the size of a given ortho tiff by projecting specified osm objects
//
// \author Ozge C. Whiting
// \date Feb 6, 2017
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <volm/volm_geo_index2.h>
#include <volm/volm_osm_objects.h>
#include <volm/volm_category_io.h>
#include <volm/volm_tile.h>
#include <volm/volm_io.h>
#include <volm/volm_io_tools.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/vgl_box_2d.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vpgl/vpgl_utm.h>
#include <volm/volm_osm_parser.h>
#include <vcl_where_root_dir.h>
#include <bkml/bkml_write.h>
#include <bkml/bkml_parser.h>
#include <vul/vul_file_iterator.h>
#include <brip/brip_line_generator.h>

#include <utility>

bool find_key_value_from_tags(std::string const& key, std::string const& value,
                              std::vector<std::pair<std::string, std::string> >& tags,
                              std::string& name)
{
  bool found = false;
  if (key == "all" && value == "all") {
    name = "all=all";
    return true;
  }
  for (auto vit = tags.begin(); ( vit != tags.end() && !found); ++vit)
  {
    if (key.compare("all") == 0 && value.compare(vit->second) == 0)
    {  found = true;  name = vit->first + "=" + vit->second; }
    else if (key.compare(vit->first) == 0 && value.compare("all") == 0)
    {  found = true;  name = vit->first + "=" + vit->second; }
    else if (key.compare(vit->first) == 0 && value.compare(vit->second) == 0)
    {  found = true;  name = vit->first + "=" + vit->second; }
  }
  return found;
}

static void error(std::string log_file, const std::string& msg)
{
  std::cerr << msg;  volm_io::write_post_processing_log(std::move(log_file), msg);
}
using namespace std;
int main(int argc, char** argv)
{
  vul_arg<std::string> osm_file("-osm", "open street map xml file", ""); // open street map xml file
  vul_arg<std::string> key("-key", "the key of desired osm category, write \"all\" (default) to parse all categories","all");
  vul_arg<std::string> value("-value", "the value of desired osm category, write \"all\" (default) to parse all categories","all");
  vul_arg<std::string> geometry("-geo","the geometry of desired osm category, can be point/line/poly, write \"all\" (default) to parse all geometries","all");

  vul_arg<std::string> input_geotiff("-geotiff", "name of input geotiff file, the camera will be parsed from the header of this image", "");
  vul_arg<std::string> output_image("-out", "name of output image", "");
  vul_arg<unsigned> margin("-margin", "optionally put a margin as masked around the image", 0);
  vul_arg_parse(argc, argv);

  vil_image_resource_sptr loaded_image = vil_load_image_resource(input_geotiff().c_str());
  int ni = loaded_image->ni(); int nj = loaded_image->nj();
  cout << "ni: " << ni << " nj: " << nj << endl;
  vil_image_view<vil_rgb<vxl_byte> > output_img(ni, nj);
  vil_image_view<vil_rgb<vxl_byte> > output_img2(ni, nj);
  output_img.fill(vil_rgb<vxl_byte>(0,0,0));
  output_img2.fill(vil_rgb<vxl_byte>(0,0,0));

  vpgl_lvcs_sptr lvcs = new vpgl_lvcs;
  vpgl_geo_camera *cam;
  vpgl_geo_camera::init_geo_camera(loaded_image, lvcs, cam);
  //vpgl_geo_camera::init_geo_camera_from_filename(input_geotiff(), ni, nj, lvcs, cam);

  std::vector<vgl_point_2d<double> > osm_pts;
  std::vector<std::vector<std::pair<std::string, std::string> > > osm_pt_keys;
  std::vector<std::vector<vgl_point_2d<double> > > osm_lines;
  std::vector<std::vector<std::pair<std::string, std::string> > > osm_line_keys;
  std::vector<vgl_polygon<double> > osm_polys;
  std::vector<std::vector<std::pair<std::string, std::string> > > osm_poly_keys;

  std::cout << "Input osm file:  " << osm_file() << std::endl;
  std::cout << "START..." << std::endl;
  if (geometry().compare("point") == 0)
  {
    std::cout << " parsing all points having key \"" << key() << "\" and value \"" << value() << "\" in osm to kml..." << std::endl;
    volm_osm_parser::parse_points(osm_pts, osm_pt_keys, osm_file());
  }
  else if (geometry().compare("lines") == 0)
  {
    std::cout << " parsing all lines having key \"" << key() << "\" and value \"" << value() << "\" in osm to kml..." << std::endl;
    volm_osm_parser::parse_lines(osm_lines, osm_line_keys, osm_file());
  }
  else if (geometry().compare("poly") == 0)
  {
    std::cout << " parsing all regions having key \"" << key() << "\" and value \"" << value() << "\" in osm to kml..." << std::endl;
    volm_osm_parser::parse_polygons(osm_polys, osm_poly_keys, osm_file());
  }
  else {
    std::cout << " parsing location points having key \"" << key() << "\" and value \"" << value() << "\" in osm to kml..." << std::endl;
    volm_osm_parser::parse_points(osm_pts, osm_pt_keys, osm_file());
    std::cout << osm_pts.size() << " points are parsed from osm file" << std::flush << std::endl;
    std::cout << " parsing lines having key \"" << key() << "\" and value \"" << value() << "\" in osm to kml..." << std::endl;
    volm_osm_parser::parse_lines(osm_lines, osm_line_keys, osm_file());
    std::cout << osm_lines.size() << " lines are parsed from osm file" << std::flush << std::endl;
    std::cout << " parsing regions having key \"" << key() << "\" and value \"" << value() << "\" in osm to kml..." << std::endl;
    volm_osm_parser::parse_polygons(osm_polys, osm_poly_keys, osm_file());
    std::cout << osm_polys.size() << " regions are parsed from osm file" << std::flush << std::endl;

    auto num_lines = (unsigned)osm_lines.size();
    for (unsigned i = 0; i < num_lines; i++) {
      vector<pair<string, string> > curr_keys = osm_line_keys[i];
      string name, description;
      bool found = find_key_value_from_tags(key(), value(), curr_keys, name);
      if (found) { // mark these pixels in the image
        vector<vgl_point_2d<double> > line;
        for (auto & vit : osm_lines[i]) {
          double lon = vit.x(); double lat = vit.y();
          double u,v;
          cam->global_to_img(lon, lat, 0.0, u, v);

          if (u >= 0 && u < ni && v >= 0 && v < nj) {
            line.emplace_back(u,v);
            cout << u << " " << v << " " << endl;
          }
        }
        if (line.size() == 0)
          continue;
        /*
        vgl_polygon<double> img_poly;
        double width = 5;
        if (!volm_io_tools::expend_line(line, width, img_poly)) {
          cout << " expending osm line failed for " << name << endl; cout.flush();
          return false;
        }
        cout << "line size: " << line.size() << endl;

        //vgl_polygon_scan_iterator<double> it(img_poly, true);
        vgl_polygon_scan_iterator<double> it(img_poly, false);
        for (it.reset(); it.next();  ) {
        int y = it.scany();
        for (int x = it.startx(); x <= it.endx(); ++x) {
          if ( x >= 0 && y >= 0 && x < output_img.ni() && y < output_img.nj()) {
            output_img(x,y) = vil_rgb<vxl_byte>(255,255,255);
          }
        }
        }
        */


        vgl_polygon<double> img_poly;
        vector<vgl_point_2d<double> > sheet;
        vgl_point_2d<double> s = line[0];
        sheet.emplace_back(0, s.y());
        //sheet.push_back(vgl_point_2d<double>(s.x(), 0));
        for (int kk = 0; kk < line.size()-1; kk++) {
          vgl_point_2d<double> s, e;
          s = line[kk];  e = line[kk+1];

          bool init = true;
          auto xs = static_cast<float>(s.x()), ys = static_cast<float>(s.y());
          auto xe = static_cast<float>(e.x()), ye = static_cast<float>(e.y());
          float x=xs, y=ys;
          //generate the path between two pixels
          while (brip_line_generator::generate(init, xs, ys, xe, ye, x, y))
          {
            auto u = static_cast<unsigned>(x), v = static_cast<unsigned>(y);
            sheet.emplace_back(x,y);
            if ( u >= 0 && v >= 0 && u < output_img.ni() && v < output_img.nj()) {
              output_img(u,v) = vil_rgb<vxl_byte>(255,255,255);
            }
          }
        }

        vgl_point_2d<double> e = line[line.size()-1];
        sheet.emplace_back(e.x(),0);
        //sheet.push_back(vgl_point_2d<double>(0,e.y()));
        sheet.emplace_back(ni-1, nj-1);
        //sheet.push_back(vgl_point_2d<double>(0, nj));
        sheet.emplace_back(0, nj);
        //sheet.push_back(vgl_point_2d<double>(0, 0));
        img_poly.push_back(sheet);
        vgl_polygon_scan_iterator<double> it(img_poly, false);
        for (it.reset(); it.next();  ) {
        int y = it.scany();
        for (int x = it.startx(); x <= it.endx(); ++x) {
          if ( x >= 0 && y >= 0 && x < output_img.ni() && y < output_img.nj()) {
            output_img2(x,y) = vil_rgb<vxl_byte>(255,255,255);
          }
        }
        }


      }
    }
  }
  if (margin() > 0) {  // mark the margin as "not water"
    for (int i = 0; i < margin(); i++)
      for (int j = 0; j < output_img2.nj(); j++)
        output_img2(i,j) = vil_rgb<vxl_byte>(0,0,0);

    for (int i = output_img2.ni()-margin(); i < output_img2.ni(); i++)
      for (int j = 0; j < output_img2.nj(); j++)
        output_img2(i,j) = vil_rgb<vxl_byte>(0,0,0);

    for (int j = 0; j < margin(); j++)
      for (int i = 0; i < output_img2.ni(); i++)
        output_img2(i,j) = vil_rgb<vxl_byte>(0,0,0);

    for (int j = output_img2.nj()-margin(); j < output_img2.nj(); j++)
      for (int i = 0; i < output_img2.ni(); i++)
        output_img2(i,j) = vil_rgb<vxl_byte>(0,0,0);

  }

  // save the images
  vil_save(output_img, output_image().c_str());
  string name = output_image() + "_water_filled.png";
  vil_save(output_img2, name.c_str());

  return true;
}
