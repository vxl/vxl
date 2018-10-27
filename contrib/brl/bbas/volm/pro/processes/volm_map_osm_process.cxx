// This is brl/bbas/volm/pro/processes/volm_map_osm_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
//         Take an ortho image, ortho camera and a list of osm objects, map the roads onto the image
//
//
#include <bprb/bprb_parameters.h>
#include <vgl/vgl_polygon.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>
#include <volm/volm_osm_objects.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <volm/volm_io_tools.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vul/vul_file.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>
#include <bsol/bsol_algs.h>
#include <bkml/bkml_write.h>
#include <bkml/bkml_parser.h>
//:
//  Take an ortho image and its camera, a bin file with an osm object list, map the objects onto the image
bool volm_map_osm_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");  // ortho image - pass as converted to byte image
  input_types.emplace_back("vpgl_camera_double_sptr");  // ortho camera
  input_types.emplace_back("vcl_string");   // bin file with osm object list
  input_types.emplace_back("vcl_string");   // a binary file to store the projected line on image domain
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr"); // a color image with red channel the objects that are overlaid
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool volm_map_osm_process(bprb_func_process& pro)
{
  vsl_add_to_binary_loader(vsol_polygon_2d());
  vsl_add_to_binary_loader(vsol_polyline_2d());
  vsl_add_to_binary_loader(vsol_point_2d());
  if (!pro.verify_inputs()) {
    std::cout << "volm_map_osm_process: Input Error" << std::endl;
    return false;
  }

  // get the inputs
  vil_image_view_base_sptr img_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(1);
  std::string osm_file  = pro.get_input<std::string>(2);
  std::string out_bin_file = pro.get_input<std::string>(3);

  auto *geo_cam = dynamic_cast<vpgl_geo_camera*>(cam.ptr());
  if (!geo_cam) {
    std::cerr << "Cannot cast the input cam to a vpgl_geo_camera!\n";
    return false;
  }

  vil_image_view<vxl_byte> bimg(img_sptr);
  vil_image_view<vil_rgb<vxl_byte> > out_img(img_sptr->ni(), img_sptr->nj(), 1);
  for (unsigned i = 0; i < out_img.ni(); i++)
    for (unsigned j = 0; j < out_img.nj(); j++) {
      out_img(i, j).r = bimg(i,j);
      out_img(i, j).g = bimg(i,j);
      out_img(i, j).b = bimg(i,j);
    }

  // read the osm objects
  // load the volm_osm object
  volm_osm_objects osm_objs(osm_file);
  std::cout << " =========== Load volumetric open stree map objects... " << " ===============" << std::endl;
  std::cout << " \t number of roads in osm: " << osm_objs.num_roads() << std::endl;

  bool hit = false;
  std::vector<std::vector<std::pair<int, int> > > img_lines;
  std::vector<volm_osm_object_line_sptr> loc_lines = osm_objs.loc_lines();
  for (auto & loc_line : loc_lines) {
    std::vector<vgl_point_2d<double> > pts = loc_line->line();
    std::vector<std::pair<int, int> > img_line;
    for (auto & pt : pts) {
      double u, v;
      geo_cam->global_to_img(pt.x(), pt.y(), 0, u, v);
      auto uu = (unsigned)std::floor(u + 0.5f);
      auto vv = (unsigned)std::floor(v + 0.5f);
      if (uu < img_sptr->ni() && vv < img_sptr->nj()) {
        //out_img(uu, vv).r = 255;
        img_line.emplace_back(uu,vv);
        hit = true;
      }
    }
    if (img_line.size() > 0)
      img_lines.push_back(img_line);
  }
  std::cout << "number of img lines: " << img_lines.size() << std::endl;
  std::vector<vsol_spatial_object_2d_sptr> sos;
  if (hit) {
    for (unsigned i = 0; i < img_lines.size(); i++) {
      std::cout << "img line: " << i << " number of pts: " << img_lines[i].size() << " ";
      std::vector<vsol_point_2d_sptr> vertices;
      for (unsigned j = 0; j < img_lines.size(); j++)
        vertices.push_back(new vsol_point_2d(img_lines[i][j].first, img_lines[i][j].second));
      vsol_polyline_2d_sptr vsolp = new vsol_polyline_2d(vertices);
      sos.emplace_back(vsolp->cast_to_spatial_object());
      out_img(img_lines[i][0].first, img_lines[i][0].second).r = 255;
      for (unsigned j = 1; j < img_lines[i].size(); j++) {
        double prev_u = img_lines[i][j-1].first;
        double prev_v = img_lines[i][j-1].second;
        double dx = img_lines[i][j].first - img_lines[i][j-1].first;
        double dy = img_lines[i][j].second - img_lines[i][j-1].second;
        double ds = std::sqrt(dx*dx + dy*dy);
        std::cout << " ds: " << ds << " ";
        double cos = dx/ds; double sin = dy/ds;
        unsigned cnt = 1;
        while (ds > 0.1) {
          //out_img(prev_u + cnt*1*cos, prev_v + cnt*1*sin).r = 255;  // delta is 1 pixel

          auto uu = (unsigned)std::floor(prev_u + cnt*1*cos + 0.5f);
          auto vv = (unsigned)std::floor(prev_v + cnt*1*sin + 0.5f);
          if (uu < img_sptr->ni() && vv < img_sptr->nj())
            out_img(uu, vv).r = 255;  // delta is 1 pixel

          cnt++;
          ds -= 1;
        }
        std::cout << cnt << " pts in the image!\n";
      }
    }
  }

  // write out the binary file
  if (out_bin_file.compare("") != 0) {
    if (sos.size() != 0) {
      vsl_b_ofstream ostr(out_bin_file);
      if (!ostr)
        std::cerr << pro.name() << ": failed to open output stream " << out_bin_file << std::endl;
      else {
        vsl_b_write(ostr, sos);
        ostr.close();
      }
    }
  }

  vil_image_view_base_sptr out_img_sptr = new vil_image_view<vxl_byte>(out_img);

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_sptr);
  return hit;
}


//:
//  Take an ortho 'segmented' image and its camera, a bin file with an osm object list, map the objects onto the image
//  mark the road segments as road, red in the output image
//  also take a height image, mark anything above the roads around them as 'building'
//  set the blue channel with respect to the height of the building
bool volm_map_segments_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");  // ortho image - pass as color image where each segment has a unique color
  input_types.emplace_back("vpgl_camera_double_sptr");  // ortho camera
  input_types.emplace_back("vcl_string");   // bin file with osm object list
  input_types.emplace_back("vil_image_view_base_sptr");  // ortho height image
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr"); // a color image with output segments
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool volm_map_segments_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 4) {
    std::cout << "volm_map_segments_process: The number of inputs should be 4" << std::endl;
    return false;
  }

  // get the inputs
  vil_image_view_base_sptr img_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(1);
  std::string osm_file  = pro.get_input<std::string>(2);
  vil_image_view_base_sptr input_height_image_sptr = pro.get_input<vil_image_view_base_sptr>(3);

  std::cout << "!!!!!!!!!!! input height img ni: " << input_height_image_sptr->ni() << "  nj: " << input_height_image_sptr->nj() << std::endl;
  vil_image_view<float> height_image = vil_convert_cast(float(), input_height_image_sptr);

  auto *geo_cam = dynamic_cast<vpgl_geo_camera*>(cam.ptr());
  if (!geo_cam) {
    std::cerr << "Cannot cast the input cam to a vpgl_geo_camera!\n";
    return false;
  }

  std::cout << "constructing segment map..!\n";
  vil_image_view<vil_rgb<vxl_byte> > img(img_sptr);  // input segments
  std::map<std::pair<unsigned char, std::pair<unsigned char, unsigned char> >, std::pair<std::vector<std::pair<int, int> >, vil_rgb<vxl_byte> > > segment_map;
  std::map<std::pair<unsigned char, std::pair<unsigned char, unsigned char> >, std::pair<std::vector<std::pair<int, int> >, vil_rgb<vxl_byte> > >::iterator iter;

  for (unsigned i = 0; i < img.ni(); i++)
    for (unsigned j = 0; j < img.nj(); j++) {
      std::pair<unsigned char, std::pair<unsigned char, unsigned char> > seg_color(img(i,j).r, std::pair<unsigned char, unsigned char>(img(i,j).g, img(i,j).b));
      iter = segment_map.find(seg_color);
      if (iter != segment_map.end())
        iter->second.first.emplace_back(i,j);
      else {
        std::vector<std::pair<int, int> > tmp;
        tmp.emplace_back(i,j);
        std::pair<std::vector<std::pair<int, int> >, vil_rgb<vxl_byte> > p(tmp, vil_rgb<vxl_byte>(255, 255, 255));
        segment_map[seg_color] = p;
      }
    }
  std::cout << "there are " << segment_map.size() << " unique segments in the image!\n";

  vil_image_view<vil_rgb<vxl_byte> > out_img(img_sptr->ni(), img_sptr->nj(), 1);
  out_img.fill(vil_rgb<unsigned char>(0,0,0));

  vil_rgb<vxl_byte> road_color(255, 0, 0);

  // read the osm objects
  // load the volm_osm object
  volm_osm_objects osm_objs(osm_file);
  std::cout << " =========== Load volumetric open street map objects... " << " ===============" << std::endl;
  std::cout << " \t number of roads in osm: " << osm_objs.num_roads() << std::endl;

  bool hit = false;
  std::vector<std::vector<std::pair<int, int> > > img_lines;
  std::vector<volm_osm_object_line_sptr> loc_lines = osm_objs.loc_lines();
  for (auto & loc_line : loc_lines) {
    std::vector<vgl_point_2d<double> > pts = loc_line->line();
    std::vector<std::pair<int, int> > img_line;
    for (auto & pt : pts) {
      double u, v;
      geo_cam->global_to_img(pt.x(), pt.y(), 0, u, v);
      unsigned uu = std::floor(u + 0.5f);
      unsigned vv = std::floor(v + 0.5f);
      if (uu < img_sptr->ni() && vv < img_sptr->nj()) {
        img_line.emplace_back(uu,vv);
        hit = true;
      }
    }
    if (img_line.size() > 0)
      img_lines.push_back(img_line);
  }
  std::cout << "number of img lines: " << img_lines.size() << std::endl;
  if (hit) {
    std::vector<float> road_heights;

    for (unsigned i = 0; i < img_lines.size(); i++) {
      std::cout << "img line: " << i << " number of pts: " << img_lines[i].size() << " ";
      // find the segment color of this pixel
      vil_rgb<vxl_byte> segment_color = img(img_lines[i][0].first, img_lines[i][0].second);
      // make the output color of this group, the road color: red
      std::pair<unsigned char, std::pair<unsigned char, unsigned char> > seg_color(segment_color.r, std::pair<unsigned char, unsigned char>(segment_color.g, segment_color.b));
      segment_map[seg_color].second = road_color;
      if (height_image(img_lines[i][0].first, img_lines[i][0].second) > 0)
        road_heights.push_back(height_image(img_lines[i][0].first, img_lines[i][0].second));
      for (unsigned j = 1; j < img_lines[i].size(); j++) {
        double prev_u = img_lines[i][j-1].first;
        double prev_v = img_lines[i][j-1].second;
        double dx = img_lines[i][j].first - img_lines[i][j-1].first;
        double dy = img_lines[i][j].second - img_lines[i][j-1].second;
        double ds = std::sqrt(dx*dx + dy*dy);
        std::cout << " ds: " << ds << " ";
        double cos = dx/ds; double sin = dy/ds;
        unsigned cnt = 1;
        while (ds > 0.1) {
          auto uu = (unsigned)std::floor(prev_u + cnt*1*cos + 0.5f);
          auto vv = (unsigned)std::floor(prev_v + cnt*1*sin + 0.5f);
          if (uu < img_sptr->ni() && vv < img_sptr->nj()) {
            vil_rgb<vxl_byte> segment_color = img(uu, vv);
            std::pair<unsigned char, std::pair<unsigned char, unsigned char> > seg_color(segment_color.r, std::pair<unsigned char, unsigned char>(segment_color.g, segment_color.b));
            segment_map[seg_color].second = road_color;
            if (height_image(uu,vv) > 0)
              road_heights.push_back(height_image(uu,vv));
          }
          cnt++;
          ds -= 1;
        }
        std::cout << cnt << " pts in the image!\n";
      }
    }

    // find the height of each segment
    std::map<std::pair<unsigned char, std::pair<unsigned char, unsigned char> >, float > segment_height_map;
    std::map<std::pair<unsigned char, std::pair<unsigned char, unsigned char> >, float >::iterator height_iter;

    for (iter = segment_map.begin(); iter != segment_map.end(); iter++) {
      std::vector<std::pair<int, int> >& tmp = iter->second.first;
      if (tmp.size() < 1) {
        segment_height_map[iter->first] = -1;
        continue;
      }
      // find the median height of its pixels
      std::vector<float> heights;
      for (auto & i : tmp) {
        if (height_image(i.first, i.second) > 0)
          heights.push_back(height_image(i.first, i.second));
      }
      if (heights.size() <= 0)
        continue;
      std::sort(heights.begin(), heights.end());
      int med = (int)std::floor(heights.size()/2.0);
      float median_height = heights[med];
      segment_height_map[iter->first] = median_height;
      /*if (iter->second.second == road_color) {
        road_heights.push_back(median_height);
      }*/
    }
    std::sort(road_heights.begin(), road_heights.end());
    std::cout << "road heights: " << std::endl;
    for (float road_height : road_heights)
      std::cout << road_height << " ";
    std::cout << "\n";
    int med = (int)std::floor(road_heights.size()/2.0);
    std::cout << "road heights size: " << road_heights.size() << " med: " << med << std::endl; std::cout.flush();
    std::cout << "median road height: " << road_heights[med] << std::endl; std::cout.flush();

    // declare all the segments higher than the median road height as a building
    for (height_iter = segment_height_map.begin(); height_iter != segment_height_map.end(); height_iter++) {
      if (height_iter->second > road_heights[med]) {
        iter = segment_map.find(height_iter->first);
        if (iter != segment_map.end() && iter->second.second != road_color) {
          unsigned char h = height_iter->second > 255 ? (unsigned char)255 : (unsigned char)height_iter->second;
          vil_rgb<vxl_byte> b_color(0, 0, h);
          segment_map[height_iter->first].second = b_color;
        }
      }
    }

    // recolor the output image
    for (iter = segment_map.begin(); iter != segment_map.end(); iter++) {
      std::vector<std::pair<int, int> >& tmp = iter->second.first;
      for (auto & i : tmp) {
        out_img(i.first, i.second) = iter->second.second;
      }
    }

  }

  vil_image_view_base_sptr out_img_sptr = new vil_image_view<vxl_byte>(out_img);

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_sptr);
  return hit;
}



//:
//  Take a cropped satellite image with cropped RPC camera (local_rational_camera), an ortho height map and its camera to read the heights of OSM objects, and map the objects onto the satellite image
//
bool volm_map_osm_onto_image_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");  // satellite image
  input_types.emplace_back("vpgl_camera_double_sptr");   // local rational camera
  input_types.emplace_back("vil_image_view_base_sptr");  // ortho image - float values: absolute heights
  input_types.emplace_back("vpgl_camera_double_sptr");  // ortho camera
  input_types.emplace_back("vcl_string");   // bin file with osm object list
  input_types.emplace_back("vcl_string");   // name of band where the osm data will be put
  input_types.emplace_back("bool");         // option to project OSM road
  input_types.emplace_back("bool");         // option to project OSM buildings
  input_types.emplace_back("vcl_string");   // if passed then also output a binary file with the mapped objects in image coordinates saved as a vector of vsol_spatial_object_2d_sptr
  input_types.emplace_back("vcl_string");   // if passed then also output a ortho kml file with all projected OSM objects
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr"); // a color image with red channel the objects that are overlaid
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool volm_map_osm_onto_image_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs()) {
    std::cout << pro.name() << ": Wrong Input!!!" << std::endl;
    return false;
  }
  vsl_add_to_binary_loader(vsol_polygon_2d());
  vsl_add_to_binary_loader(vsol_polyline_2d());
  vsl_add_to_binary_loader(vsol_point_2d());
  // get the inputs
  vil_image_view_base_sptr sat_img_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  vpgl_camera_double_sptr sat_cam = pro.get_input<vpgl_camera_double_sptr>(1);

  vil_image_view_base_sptr height_img_sptr = pro.get_input<vil_image_view_base_sptr>(2);
  vpgl_camera_double_sptr ortho_cam = pro.get_input<vpgl_camera_double_sptr>(3);
  std::string osm_file  = pro.get_input<std::string>(4);
  std::string band_name = pro.get_input<std::string>(5);
  bool is_region = pro.get_input<bool>(6);
  bool is_line   = pro.get_input<bool>(7);
  std::string bin_filename = pro.get_input<std::string>(8);
  std::string out_kml_file = pro.get_input<std::string>(9);
  std::vector<vsol_spatial_object_2d_sptr> sos;
  std::vector<vgl_polygon<double> >  projected_regions;
  std::vector<std::vector<vgl_point_2d<double> > > projected_roads;

  auto *geo_cam = dynamic_cast<vpgl_geo_camera*>(ortho_cam.ptr());
  if (!geo_cam) {
    std::cerr << pro.name() << ": cannot cast the input cam to a vpgl_geo_camera!\n";
    return false;
  }

  if (band_name.compare("r") != 0 && band_name.compare("g") != 0 && band_name.compare("b") != 0) {
    std::cerr << pro.name() << ": unknown image band " << band_name << " only r, g, b allowed!\n";
    return false;
  }


  auto* cam_local_rat = dynamic_cast<vpgl_local_rational_camera<double>*>(sat_cam.ptr());
  //vpgl_rational_camera<double>* cam_local_rat = dynamic_cast<vpgl_rational_camera<double>*>(sat_cam.ptr());
  if (!cam_local_rat) {
    std::cerr << pro.name() << ": cannot cast the input satellite cam to a local rational camera\n";
    return false;
  }

  vil_image_view<vxl_byte> bimg(sat_img_sptr);
  vil_image_view<float> height_img(height_img_sptr);

  vil_image_view<vil_rgb<vxl_byte> > out_img(sat_img_sptr->ni(), sat_img_sptr->nj(), 1);
  for (unsigned i = 0; i < out_img.ni(); i++)
    for (unsigned j = 0; j < out_img.nj(); j++) {
      out_img(i, j).r = bimg(i,j);
      out_img(i, j).g = bimg(i,j);
      out_img(i, j).b = bimg(i,j);
    }

  // read the osm objects
  // load the volm_osm object
  volm_osm_objects osm_objs(osm_file);
  std::cout << " =========== Load volumetric open street map objects... " << " ===============" << std::endl;

  // project OSM regions (building specifically) onto image
  bool hit = false;
  if (is_region)
  {
    std::cout << " \t number of regions in osm: " << osm_objs.num_regions() << std::endl;

    unsigned num_regions = osm_objs.num_regions();
    std::vector<vgl_polygon<double> > loc_regions;
    for (unsigned r_idx = 0; r_idx < num_regions; r_idx++) {
      unsigned char curr_id = osm_objs.loc_polys()[r_idx]->prop().id_;
      if (curr_id == volm_osm_category_io::volm_land_table_name["building"].id_)
        loc_regions.emplace_back(osm_objs.loc_polys()[r_idx]->poly()[0]);
    }

    std::vector<std::vector<vgl_point_2d<double> > > img_polys;

    for (unsigned i = 0; i < loc_regions.size(); i++) {
      std::vector<vgl_point_2d<double> > pts = loc_regions[i][0];
      //pts.push_back(loc_regions[i][0][0]);
      std::vector<vgl_point_2d<double> > img_poly;
      std::vector<vgl_point_2d<double> > loc_poly;
      std::vector<vsol_point_2d_sptr> vsol_pts;
      for (auto & pt : pts) {
        double u, v;
        geo_cam->global_to_img(pt.x(), pt.y(), 0, u, v);
        auto uu = (unsigned)std::floor(u + 0.5f);
        auto vv = (unsigned)std::floor(v + 0.5f);
        if (uu < height_img_sptr->ni() && vv < height_img_sptr->nj()) {
          double elev = height_img(uu, vv);
          // now find where it projects in the satellite image
          // project to local coords of local_rational_camera first
          double loc_x, loc_y, loc_z;
          cam_local_rat->lvcs().global_to_local(pt.x(), pt.y(), elev, vpgl_lvcs::wgs84, loc_x, loc_y, loc_z);
          double iu, iv;
          cam_local_rat->project(loc_x, loc_y, loc_z, iu, iv);
          double iuu = iu + 0.5;
          double ivv = iv + 0.5;
          if (iuu >= 0 && ivv >= 0 && iuu < sat_img_sptr->ni() && ivv < sat_img_sptr->nj()) {
            std::cout << "line " << i << ": pt [" << pt.x() << ',' << pt.y() << ',' << elev << " --> " << iuu << ',' << ivv << std::endl;
            img_poly.emplace_back(iuu,ivv);
            loc_poly.emplace_back(pt.x(), pt.y());
            vsol_pts.push_back(new vsol_point_2d(iuu, ivv));
            hit = true;
          }
        }
      }
      if (img_poly.size() > 0) {
        img_polys.push_back(img_poly);
        vgl_polygon<double> project_poly(loc_poly);
        projected_regions.push_back(project_poly);
        if (vsol_pts.size() > 2) {
          vsol_polygon_2d_sptr vsolp = new vsol_polygon_2d(vsol_pts);
          sos.emplace_back(vsolp->cast_to_spatial_object());
        }
      }
    }
    if (img_polys.size() > 0)
      hit = true;
    if (hit) {
      for (auto & i : img_polys) {
        std::vector<std::pair<int, int> > img_line;
        for (unsigned k = 0; k < i.size(); k++) {
          int iuu = (int)std::floor(i[k].x());
          int ivv = (int)std::floor(i[k].y());
          img_line.emplace_back(iuu,ivv);
        }
        img_line.emplace_back(std::floor(i[0].x()),std::floor(i[0].y()));
        out_img(img_line[0].first, img_line[0].second).r = 255;
        for (unsigned j = 1; j < img_line.size(); j++) {
          double prev_u = img_line[j-1].first;
          double prev_v = img_line[j-1].second;
          double dx = img_line[j].first - img_line[j-1].first;
          double dy = img_line[j].second - img_line[j-1].second;
          double ds = std::sqrt(dx*dx + dy*dy);
          std::cout << " ds: " << ds << " ";
          double cos = dx/ds; double sin = dy/ds;
          unsigned cnt = 1;
          std::vector<vgl_point_2d<double> > line_img;
          while (ds > 0.1) {
            //out_img(prev_u + cnt*1*cos, prev_v + cnt*1*sin).r = 255;  // delta is 1 pixel
            double uu = prev_u + cnt*1*cos + 0.5f;
            double vv = prev_v + cnt*1*sin + 0.5f;
            if (uu >= 0 && vv >= 0 && uu < sat_img_sptr->ni() && vv < sat_img_sptr->nj()) {
              line_img.emplace_back(uu,vv);
              //if (band_name.compare("r") == 0)      out_img(uu, vv).r = 255;
              //else if(band_name.compare("g") == 0)  out_img(uu, vv).g = 255;
              //else if(band_name.compare("b") == 0)  out_img(uu, vv).b = 255;
            }
            cnt++;
            ds -= 1;
          }
          // expand the line to a region with certain width
          double width = 3.0;
          vgl_polygon<double> img_poly;
          volm_io_tools::expend_line(line_img, width, img_poly);
          vgl_polygon_scan_iterator<double> it(img_poly, true);
          for (it.reset(); it.next();  ) {
            int y = it.scany();
            for (int x = it.startx(); x <= it.endx(); ++x) {
              if ( x >= 0 && y >= 0 && x < (int)out_img.ni() && y < (int)out_img.nj()) {
                if (band_name.compare("r") == 0)      out_img(x, y).r = 255;
                else if(band_name.compare("g") == 0)  out_img(x, y).g = 255;
                else if(band_name.compare("b") == 0)  out_img(x, y).b = 255;
              }
            }
          }
        }
      }
    }
  }

  // project OSM lines onto the image if necessary
  if (is_line)
  {
    std::cout << " \t number of roads in osm: " << osm_objs.num_roads() << std::endl;
    hit = false;
    std::vector<std::vector<std::pair<int, int> > > img_lines;
    std::vector<volm_osm_object_line_sptr> loc_lines = osm_objs.loc_lines();
    for (unsigned i = 0; i < loc_lines.size(); i++)
    {
      std::vector<vgl_point_2d<double> > pts = loc_lines[i]->line();
      std::vector<std::pair<int, int> > img_line;
      std::vector<vgl_point_2d<double> > loc_line;
      std::vector<vsol_point_2d_sptr> vsol_pts;
      for (auto & pt : pts) {
        double u, v;
        geo_cam->global_to_img(pt.x(), pt.y(), 0, u, v);
        auto uu = (unsigned)std::floor(u + 0.5f);
        auto vv = (unsigned)std::floor(v + 0.5f);
        if (uu < height_img_sptr->ni() && vv < height_img_sptr->nj()) {
          //out_img(uu, vv).r = 255;
          double elev = height_img(uu, vv);

          // now find where it projects in the satellite image
          // project to local coords of local_rational_camera first
          double loc_x, loc_y, loc_z;
          cam_local_rat->lvcs().global_to_local(pt.x(), pt.y(), elev, vpgl_lvcs::wgs84, loc_x, loc_y, loc_z);
          double iu, iv;
          cam_local_rat->project(loc_x, loc_y, loc_z, iu, iv);
          auto iuu = (unsigned)std::floor(iu + 0.5f);
          auto ivv = (unsigned)std::floor(iv + 0.5f);
          if (iuu < sat_img_sptr->ni() && ivv < sat_img_sptr->nj()) {
            std::cout << "line " << i << ": pt [" << pt.x() << ',' << pt.y() << ',' << elev << " --> " << iuu << ',' << ivv << std::endl;
            loc_line.push_back(pt);
            img_line.emplace_back(iuu,ivv);
            vsol_pts.push_back(new vsol_point_2d(iuu, ivv));
          }
        }
      }
      if (img_line.size() > 2) {
        img_lines.push_back(img_line);
        // add the line into project line
        projected_roads.push_back(loc_line);
        vsol_polyline_2d_sptr vsoll = new vsol_polyline_2d(vsol_pts);
        sos.emplace_back(vsoll->cast_to_spatial_object());
      }
    }
    std::cout << "number of img lines: " << img_lines.size() << std::endl;
    if (img_lines.size() > 0)
      hit = true;
    if (hit) {
      for (unsigned i = 0; i < img_lines.size(); i++) {
        std::cout << "img line: " << i << " number of pts: " << img_lines[i].size() << " ";
        out_img(img_lines[i][0].first, img_lines[i][0].second).r = 255;
        for (unsigned j = 1; j < img_lines[i].size(); j++) {
          double prev_u = img_lines[i][j-1].first;
          double prev_v = img_lines[i][j-1].second;
          double dx = img_lines[i][j].first - img_lines[i][j-1].first;
          double dy = img_lines[i][j].second - img_lines[i][j-1].second;
          double ds = std::sqrt(dx*dx + dy*dy);
          std::cout << " ds: " << ds << " ";
          double cos = dx/ds; double sin = dy/ds;
          unsigned cnt = 1;
          std::vector<vgl_point_2d<double> > line_img;
          while (ds > 0.1) {
            //out_img(prev_u + cnt*1*cos, prev_v + cnt*1*sin).r = 255;  // delta is 1 pixel

            double uu = prev_u + cnt*1*cos + 0.5f;
            double vv = prev_v + cnt*1*sin + 0.5f;
            if (uu >= 0 && vv >= 0 && uu < sat_img_sptr->ni() && vv < sat_img_sptr->nj())
              line_img.emplace_back(uu,vv);
            cnt++;
            ds -= 1;
          }
          if (line_img.empty())
            continue;
          // expand the line to a region with certain width
          double width = 2.0;
          vgl_polygon<double> img_poly;
          volm_io_tools::expend_line(line_img, width, img_poly);
          vgl_polygon_scan_iterator<double> it(img_poly, true);
          for (it.reset(); it.next();  ) {
            int y = it.scany();
            for (int x = it.startx(); x <= it.endx(); ++x) {
              if ( x >= 0 && y >= 0 && x < (int)out_img.ni() && y < (int)out_img.nj()) {
                if (band_name.compare("r") == 0)      out_img(x, y).r = 255;
                else if(band_name.compare("g") == 0)  out_img(x, y).g = 255;
                else if(band_name.compare("b") == 0)  out_img(x, y).b = 255;
              }
            }
          }
          std::cout << cnt << " pts in the image!\n";
        }
      }
    }
  }

  vil_image_view_base_sptr out_img_sptr = new vil_image_view<vxl_byte>(out_img);

  // write the binary file if required
  if (bin_filename.compare("") != 0) {
    if (sos.size() == 0)
      std::cerr << " There are no vsol spatial objects in the vector!\n";
    else {
      vsl_b_ofstream ostr(bin_filename);
      if (!ostr)
        std::cerr << "Failed to open output stream " << bin_filename << std::endl;
      else {
        std::cout << "there are " << sos.size() << " vsol objects, writing to binary file: " << bin_filename << std::endl;
        vsl_b_write(ostr, sos);
        ostr.close();
      }
    }
  }
  // write the kml
  if (out_kml_file.compare("") != 0)
  {
    std::ofstream ofs(out_kml_file.c_str());
    if (!ofs)
      std::cerr << pro.name() << ": failed to open output stream " << out_kml_file << std::endl;
    else {
      bkml_write::open_document(ofs);
      for (unsigned i = 0; i < projected_roads.size(); i++) {
        std::stringstream name;
        name << "road" << i;
        bkml_write::write_path(ofs, projected_roads[i], name.str(), name.str(), 1.0, 2.0, 1.0, 255, 0, 0);
      }
      for (unsigned i = 0; i < projected_regions.size(); i++) {
        std::stringstream name;
        name << "region" << i;
        bkml_write::write_polygon(ofs, projected_regions[i], name.str(), name.str(), 1.0, 3.0, 0.45, 0, 255, 0);
      }
      bkml_write::close_document(ofs);
      ofs.close();
    }
  }

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_sptr);
  return hit;
}


//:
//  Take a cropped satellite image with cropped RPC camera (local_rational_camera), an ortho height map and its camera to read the heights of OSM objects, and map the objects onto the satellite image
//
bool volm_map_osm_onto_image_process2_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");  // satellite image
  input_types.emplace_back("vpgl_camera_double_sptr");   // local rational camera
  input_types.emplace_back("vil_image_view_base_sptr");  // ortho image - float values: absolute heights
  input_types.emplace_back("vpgl_camera_double_sptr");   // ortho camera
  input_types.emplace_back("vcl_string");                // bin file with osm object list // if it exists things are appended to this file!
  input_types.emplace_back("vcl_string");                // OSM category name
  input_types.emplace_back("vcl_string");                // output a binary file with the mapped objects saved as a vector of vsol_spatial_object_2d_sptr
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr"); // a RBG image patch that labels the OSM category
  output_types.emplace_back("vil_image_view_base_sptr"); // a binary mask patch that contains all ables from OSM category
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool volm_map_osm_onto_image_process2(bprb_func_process& pro)
{
  if (!pro.verify_inputs()) {
    std::cout << "volm_map_osm_onto_image_process2: The number of inputs should be 7" << std::endl;
    return false;
  }
  vsl_add_to_binary_loader(vsol_polygon_2d());
  vsl_add_to_binary_loader(vsol_polyline_2d());
  vsl_add_to_binary_loader(vsol_point_2d());

  // get the inputs
  vil_image_view_base_sptr sat_img_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  vpgl_camera_double_sptr sat_cam = pro.get_input<vpgl_camera_double_sptr>(1);

  vil_image_view_base_sptr height_img_sptr = pro.get_input<vil_image_view_base_sptr>(2);
  vpgl_camera_double_sptr ortho_cam = pro.get_input<vpgl_camera_double_sptr>(3);
  std::string osm_file  = pro.get_input<std::string>(4);
  std::string osm_category_name = pro.get_input<std::string>(5);
  unsigned osm_id = 0;
  auto iter = volm_osm_category_io::volm_land_table_name.find(osm_category_name);
  if (iter == volm_osm_category_io::volm_land_table_name.end()) {
    std::cerr << "string: " << osm_category_name << " is not a valid category name listed in volm_osm_category_io::volm_land_table_name!\n";
    return false;
  } else
    osm_id = iter->second.id_;

  std::string bin_filename = pro.get_input<std::string>(6);

  std::vector<vsol_spatial_object_2d_sptr> sos;

  auto *geo_cam = dynamic_cast<vpgl_geo_camera*>(ortho_cam.ptr());
  if (!geo_cam) {
    std::cerr << pro.name() << ": cannot cast the input cam to a vpgl_geo_camera!\n";
    return false;
  }

  auto* cam_local_rat = dynamic_cast<vpgl_local_rational_camera<double>*>(sat_cam.ptr());
  //vpgl_rational_camera<double>* cam_local_rat = dynamic_cast<vpgl_rational_camera<double>*>(sat_cam.ptr());
  if (!cam_local_rat) {
    std::cerr << pro.name() << ": cannot cast the input satellite cam to a local rational camera\n";
    return false;
  }

  vil_image_view<vxl_byte> bimg(sat_img_sptr);
  vil_image_view<float> height_img(height_img_sptr);

  bool hit = false;
  // read the osm objects
  // load the volm_osm object
  volm_osm_objects osm_objs(osm_file);
  std::cout << " =========== Load volumetric open street map objects... " << " ===============" << std::endl;

  // project all OSM regions with the specified name
  unsigned num_regions = osm_objs.num_regions();

  std::vector<vgl_polygon<double> > loc_regions;
  for (unsigned r_idx = 0; r_idx < num_regions; r_idx++) {
    unsigned char curr_id = osm_objs.loc_polys()[r_idx]->prop().id_;
    if (curr_id == osm_id)
      loc_regions.emplace_back(osm_objs.loc_polys()[r_idx]->poly()[0]);
  }

  std::vector<std::vector<vgl_point_2d<double> > > img_polys;

  std::cout << "project OSM regions with name " << osm_category_name << " onto image..." << std::endl;
  for (auto & loc_region : loc_regions) {
    std::vector<vgl_point_2d<double> > pts = loc_region[0];
    //pts.push_back(loc_regions[i][0][0]);
    std::vector<vgl_point_2d<double> > img_poly;

    std::vector<vsol_point_2d_sptr> vsol_pts;
    for (auto & pt : pts) {
      double u, v;
      geo_cam->global_to_img(pt.x(), pt.y(), 0, u, v);
      auto uu = (unsigned)std::floor(u + 0.5f);
      auto vv = (unsigned)std::floor(v + 0.5f);
      if (uu < height_img_sptr->ni() && vv < height_img_sptr->nj()) {
        double elev = height_img(uu, vv);
        // now find where it projects in the satellite image
        // project to local coords of local_rational_camera first
        double loc_x, loc_y, loc_z;
        cam_local_rat->lvcs().global_to_local(pt.x(), pt.y(), elev, vpgl_lvcs::wgs84, loc_x, loc_y, loc_z);
        double iu, iv;
        cam_local_rat->project(loc_x, loc_y, loc_z, iu, iv);
        double iuu = iu + 0.5;
        double ivv = iv + 0.5;
        if (iuu >= 0 && ivv >= 0 && iuu < sat_img_sptr->ni() && ivv < sat_img_sptr->nj()) {
          //std::cout << "line " << i << ": pt [" << pts[j].x() << ',' << pts[j].y() << ',' << elev << " --> " << iuu << ',' << ivv << std::endl;
          img_poly.emplace_back(iuu,ivv);
          vsol_pts.push_back(new vsol_point_2d(iuu, ivv));
        }
      }
    }
    if (img_poly.size() > 0) {
      img_polys.push_back(img_poly);
      if (vsol_pts.size() > 2) {
        vsol_polygon_2d_sptr vsolp = new vsol_polygon_2d(vsol_pts);
        sos.emplace_back(vsolp->cast_to_spatial_object());
      }
    }
  }

  std::cout << "project OSM lines with name " << osm_category_name << " onto image..." << std::endl;
  // project all OSM lines with the specified name
  std::vector<std::vector<std::pair<int, int> > > img_lines;
  std::vector<volm_osm_object_line_sptr> loc_lines = osm_objs.loc_lines();
  for (auto & loc_line : loc_lines) {
    if (loc_line->prop().id_ != osm_id)
      continue;

    std::vector<vgl_point_2d<double> > pts = loc_line->line();
    std::vector<std::pair<int, int> > img_line;
    std::vector<vsol_point_2d_sptr> vsol_pts;
    for (auto & pt : pts) {
      double u, v;
      geo_cam->global_to_img(pt.x(), pt.y(), 0, u, v);
      auto uu = (unsigned)std::floor(u + 0.5f);
      auto vv = (unsigned)std::floor(v + 0.5f);
      if (uu < height_img_sptr->ni() && vv < height_img_sptr->nj()) {
        //out_img(uu, vv).r = 255;
        double elev = height_img(uu, vv);

        // now find where it projects in the satellite image
        // project to local coords of local_rational_camera first
        double loc_x, loc_y, loc_z;
        cam_local_rat->lvcs().global_to_local(pt.x(), pt.y(), elev, vpgl_lvcs::wgs84, loc_x, loc_y, loc_z);
        double iu, iv;
        cam_local_rat->project(loc_x, loc_y, loc_z, iu, iv);
        auto iuu = (unsigned)std::floor(iu + 0.5f);
        auto ivv = (unsigned)std::floor(iv + 0.5f);
        if (iuu < sat_img_sptr->ni() && ivv < sat_img_sptr->nj()) {
          //std::cout << "line " << i << ": pt [" << pts[j].x() << ',' << pts[j].y() << ',' << elev << " --> " << iuu << ',' << ivv << std::endl;
          img_line.emplace_back(iuu,ivv);
        }
      }
    }
    if (img_line.size() > 1) {
      //std::cout << "road " << i << " has " << img_line.size() << " points" << std::endl;
      // expend the line to a polygon
      double width = loc_line->prop().width_;
      if (width == 0)  width = 3.0;
      width += 2.0;
      vgl_polygon<double> img_poly;
      std::vector<vgl_point_2d<double> > line_img;
      for (unsigned j = 1; j < img_line.size(); j++) {
        double prev_u = img_line[j-1].first;
        double prev_v = img_line[j-1].second;
        double dx = img_line[j].first - img_line[j-1].first;
        double dy = img_line[j].second - img_line[j-1].second;
        double ds = std::sqrt(dx*dx + dy*dy);
        double cos = dx/ds; double sin = dy/ds;
        unsigned cnt = 1;
        while (ds > 0.1) {
          double uu = prev_u + cnt*1*cos + 0.5f;
          double vv = prev_v + cnt*1*sin + 0.5f;
          if (uu >= 0 && vv >= 0 && uu < sat_img_sptr->ni() && vv < sat_img_sptr->nj())
            line_img.emplace_back(uu,vv);
          cnt++;
          ds -= 3;
        }
      }
      if (line_img.empty())
        continue;
      // expand the line to a region with certain width
      volm_io_tools::expend_line(line_img, width, img_poly);
      unsigned num_verts = img_poly[0].size();
      for (unsigned v_idx = 0; v_idx < num_verts; v_idx++)
        vsol_pts.push_back(new vsol_point_2d(img_poly[0][v_idx].x(), img_poly[0][v_idx].y()));
      if (vsol_pts.size() > 2) {
        vsol_polygon_2d_sptr vsolp = new vsol_polygon_2d(vsol_pts);
        sos.emplace_back(vsolp->cast_to_spatial_object());
      }
    }
  }

  // project all OSM points with the specified name
  std::cout << "project OSM points with name " << osm_category_name << " onto image..." << std::endl;
  std::cout << " \t number of points in osm: " << osm_objs.num_locs() << std::endl;
  unsigned num_points = osm_objs.num_locs();
  std::vector<std::pair<unsigned, unsigned> > img_pts;
  std::vector<volm_osm_object_point_sptr> loc_pts = osm_objs.loc_pts();

  for (auto & loc_pt : loc_pts) {
    unsigned char curr_id = loc_pt->prop().id_;
    if (curr_id == osm_id) {
      vgl_point_2d<double> pt = loc_pt->loc();
      double u, v;
      geo_cam->global_to_img(pt.x(), pt.y(), 0, u, v);
      auto uu = (unsigned)std::floor(u + 0.5f);
      auto vv = (unsigned)std::floor(v + 0.5f);
      if (uu < height_img_sptr->ni() && vv < height_img_sptr->nj()) {
        double elev = height_img(uu, vv);
        //now find where it projects in the satellite image
        //project to local coords of local_rational_camera first
        double loc_x, loc_y, loc_z;
        cam_local_rat->lvcs().global_to_local(pt.x(), pt.y(), elev, vpgl_lvcs::wgs84, loc_x, loc_y, loc_z);
        double iu, iv;
        cam_local_rat->project(loc_x, loc_y, loc_z, iu, iv);
        auto iuu = (unsigned)std::floor(iu + 0.5f);
        auto ivv = (unsigned)std::floor(iv + 0.5f);
        if (iuu < sat_img_sptr->ni() && ivv < sat_img_sptr->nj()) {
          // make it a 2x2 region with this point at the center
          std::vector<vsol_point_2d_sptr> vsol_pts;
          vsol_pts.push_back(new vsol_point_2d(iu-1, iv-1));
          vsol_pts.push_back(new vsol_point_2d(iu-1, iv+1));
          vsol_pts.push_back(new vsol_point_2d(iu+1, iv+1));
          vsol_pts.push_back(new vsol_point_2d(iu+1, iv-1));
          vsol_polygon_2d_sptr vsolp = new vsol_polygon_2d(vsol_pts);
          sos.emplace_back(vsolp->cast_to_spatial_object());
        }
      }
    }
  }

  // prepare the binary file, append to its content if it already exists
  if (sos.size() == 0)
    std::cerr << " There are no vsol spatial objects in the vector!\n";
  vsl_b_ifstream istr(bin_filename);
  std::vector<vsol_spatial_object_2d_sptr> sos_in;
  if (! !istr) {
    vsl_b_read(istr, sos_in);
    istr.close();
  }
  vsl_b_ofstream ostr(bin_filename);
  if (!ostr)
    std::cerr << "Failed to open output stream " << bin_filename << std::endl;
  else {
    std::cout << "there are " << sos.size() << " vsol objects, appending to binary file: " << bin_filename
              << " which currently has: " << sos_in.size() << " objects" << std::endl;
    for (const auto & ii : sos_in)
      sos.push_back(ii);
    vsl_b_write(ostr, sos);
    ostr.close();
  }

  // prepare the image patch
  vil_image_view<vil_rgb<vxl_byte> > out_img(sat_img_sptr->ni(), sat_img_sptr->nj(), 1);
  for (unsigned i = 0; i < out_img.ni(); i++)
    for (unsigned j = 0; j < out_img.nj(); j++) {
      out_img(i, j).r = bimg(i,j);
      out_img(i, j).g = bimg(i,j);
      out_img(i, j).b = bimg(i,j);
    }
  vil_image_view<vxl_byte> out_mask(sat_img_sptr->ni(), sat_img_sptr->nj(), 1);
  out_mask.fill(0);
  std::vector<vgl_polygon<double> > sos_poly;
  for (auto & so : sos) {
    auto* poly = static_cast<vsol_polygon_2d*>(so.ptr());
    vgl_polygon<double> vpoly; vpoly.new_sheet();
    unsigned nverts = poly->size();
    for (unsigned i = 0; i < nverts; i++) {
      vsol_point_2d_sptr v = poly->vertex(i);
      vpoly.push_back(v->x(), v->y());
    }
    sos_poly.push_back(vpoly);
  }

  if (sos_poly.size() == 0)
    std::cerr << " There are no vsol spatial objects in the vector!\n";
  else {
    std::cout << "there are " << sos_poly.size() << " vsol objects put into the image patch " << std::endl;
    for (const auto & i : sos_poly) {
      vgl_polygon_scan_iterator<double> it(i, true);
      for (it.reset(); it.next(); ) {
        int y = it.scany();
        for (int x = it.startx(); x <= it.endx(); ++x)
          if ( x >= 0 && y >= 0 && x < (int)out_img.ni() && y < (int)out_img.nj()) {
            out_img(x,y).r = 255;
            out_mask(x, y) = 255;
          }
      }
    }
  }

  // write out a text file (csv format) to store the outlines of landmarks
  // format: number_of_points,x,y,x,y,x,y,...
  std::string txt_file = vul_file::strip_extension(bin_filename) + ".csv";
  std::ofstream ofs(txt_file.c_str());
  for (auto & i : sos_poly) {
    unsigned n_verts = i[0].size();
    ofs << n_verts;
    for (unsigned v_idx = 0; v_idx < n_verts; v_idx++) {
      auto uu = (unsigned)std::floor(i[0][v_idx].x() + 0.5);
      auto vv = (unsigned)std::floor(i[0][v_idx].y() + 0.5);
      if (uu < out_img.ni() && vv < out_img.nj())
        ofs << ',' << uu << ',' << vv;
    }
    ofs << '\n';
  }
  ofs.close();
  vil_image_view_base_sptr out_img_sptr  = new vil_image_view<vil_rgb<vxl_byte> >(out_img);
  vil_image_view_base_sptr out_mask_sptr = new vil_image_view<vxl_byte>(out_mask);
  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_sptr);
  pro.set_output_val<vil_image_view_base_sptr>(1, out_mask_sptr);
  return true;
}


//:
//  Take an ortho image with vpgl_geo_camera (no need for height map) and map the objects onto the image
//
bool volm_map_osm_onto_image_process3_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");  // ortho image
  input_types.emplace_back("vpgl_camera_double_sptr");  // ortho camera
  input_types.emplace_back("vcl_string");   // bin file with osm object list // if it exists things are appended to this file!
  input_types.emplace_back("vcl_string");         // OSM category name
  input_types.emplace_back("vcl_string");   // output a binary file with the mapped objects in image coordinates saved as a vector of vsol_spatial_object_2d_sptr
  std::vector<std::string> output_types;
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool volm_map_osm_onto_image_process3(bprb_func_process& pro)
{
  if (pro.n_inputs() < 5) {
    std::cout << "volm_map_osm_onto_image_process3: The number of inputs should be 5" << std::endl;
    return false;
  }

  // get the inputs
  vil_image_view_base_sptr img_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  vpgl_camera_double_sptr ortho_cam = pro.get_input<vpgl_camera_double_sptr>(1);

  std::string osm_file  = pro.get_input<std::string>(2);
  std::string osm_category_name = pro.get_input<std::string>(3);
  unsigned osm_id = 0;
  auto iter = volm_osm_category_io::volm_land_table_name.find(osm_category_name);
  if (iter == volm_osm_category_io::volm_land_table_name.end()) {
    std::cerr << "string: " << osm_category_name << " is not a valid category name listed in volm_osm_category_io::volm_land_table_name!\n";
    return false;
  } else
    osm_id = iter->second.id_;

  std::string bin_filename = pro.get_input<std::string>(4);

  std::vector<vsol_spatial_object_2d_sptr> sos;

  auto *geo_cam = dynamic_cast<vpgl_geo_camera*>(ortho_cam.ptr());
  if (!geo_cam) {
    std::cerr << pro.name() << ": cannot cast the input cam to a vpgl_geo_camera!\n";
    return false;
  }

  vil_image_view<vxl_byte> bimg(img_sptr);

  bool hit = false;
  // read the osm objects
  // load the volm_osm object
  volm_osm_objects osm_objs(osm_file);
  std::cout << " =========== Load volumetric open street map objects... " << " ===============" << std::endl;

  // project all OSM regions with the specified name
  unsigned num_regions = osm_objs.num_regions();

  std::vector<vgl_polygon<double> > loc_regions;
  for (unsigned r_idx = 0; r_idx < num_regions; r_idx++) {
    unsigned char curr_id = osm_objs.loc_polys()[r_idx]->prop().id_;
    if (curr_id == osm_id)
      loc_regions.emplace_back(osm_objs.loc_polys()[r_idx]->poly()[0]);
  }

  std::vector<std::vector<vgl_point_2d<double> > > img_polys;

  for (auto & loc_region : loc_regions) {
    std::vector<vgl_point_2d<double> > pts = loc_region[0];
    //pts.push_back(loc_regions[i][0][0]);
    std::vector<vgl_point_2d<double> > img_poly;

    std::vector<vsol_point_2d_sptr> vsol_pts;
    for (auto & pt : pts) {
      double u, v;
      geo_cam->global_to_img(pt.x(), pt.y(), 0, u, v);
      int uu = (int)std::floor(u + 0.5f);
      int vv = (int)std::floor(v + 0.5f);
      if (uu >= 0 && vv >= 0 && uu < img_sptr->ni() && vv < img_sptr->nj()) {
        img_poly.emplace_back(uu,vv);
        vsol_pts.push_back(new vsol_point_2d(uu, vv));
        hit = true;
      }
    }
    if (img_poly.size() > 0) {
      img_polys.push_back(img_poly);
      if (vsol_pts.size() > 2) {
        vsol_polygon_2d_sptr vsolp = new vsol_polygon_2d(vsol_pts);
        sos.emplace_back(vsolp->cast_to_spatial_object());
      }
    }
  }


  // project all OSM lines with the specified name
  std::vector<std::vector<std::pair<int, int> > > img_lines;
  std::vector<volm_osm_object_line_sptr> loc_lines = osm_objs.loc_lines();
  for (auto & loc_line : loc_lines) {
    if (loc_line->prop().id_ != osm_id)
      continue;

    std::vector<vgl_point_2d<double> > pts = loc_line->line();
    std::vector<std::pair<int, int> > img_line;
    std::vector<vsol_point_2d_sptr> vsol_pts;
    for (auto & pt : pts) {
      double u, v;
      geo_cam->global_to_img(pt.x(), pt.y(), 0, u, v);
      int uu = (int)std::floor(u + 0.5f);
      int vv = (int)std::floor(v + 0.5f);
      if (uu >= 0 && vv >= 0 && uu < img_sptr->ni() && vv < img_sptr->nj()) {
        img_line.emplace_back(uu,vv);
        vsol_pts.push_back(new vsol_point_2d(uu, vv));
        hit = true;
      }
    }
    if (img_line.size() > 0) {
      // trace back the points to make it a polygon (its not a good idea to connect end point to the first point since roads curve and then the poly includes buildings, etc.)
      for (int i = vsol_pts.size()-1; i >=0; i--)
        vsol_pts.push_back(vsol_pts[i]);
      if (vsol_pts.size() > 2) {
        vsol_polygon_2d_sptr vsolp = new vsol_polygon_2d(vsol_pts);
        sos.emplace_back(vsolp->cast_to_spatial_object());
      }
    }
  }

  // project all OSM points with the specified name
  std::cout << " \t number of points in osm: " << osm_objs.num_locs() << std::endl;
  unsigned num_points = osm_objs.num_locs();
  std::vector<std::pair<unsigned, unsigned> > img_pts;
  std::vector<volm_osm_object_point_sptr> loc_pts = osm_objs.loc_pts();

  for (auto & loc_pt : loc_pts) {
    unsigned char curr_id = loc_pt->prop().id_;
    if (curr_id == osm_id) {
      vgl_point_2d<double> pt = loc_pt->loc();
      double u, v;
      geo_cam->global_to_img(pt.x(), pt.y(), 0, u, v);
      int uu = (int)std::floor(u + 0.5f);
      int vv = (int)std::floor(v + 0.5f);
      if (uu >= 0 && vv >= 0 && uu < img_sptr->ni() && vv < img_sptr->nj()) {
        hit = true;
        // make it a 2x2 region with this point at the center
        std::vector<vsol_point_2d_sptr> vsol_pts;
        vsol_pts.push_back(new vsol_point_2d(uu-1, vv-1));
        vsol_pts.push_back(new vsol_point_2d(uu-1, vv+1));
        vsol_pts.push_back(new vsol_point_2d(uu+1, vv+1));
        vsol_pts.push_back(new vsol_point_2d(uu+1, vv-1));
        vsol_polygon_2d_sptr vsolp = new vsol_polygon_2d(vsol_pts);
        sos.emplace_back(vsolp->cast_to_spatial_object());
      }
    }
  }

  // prepare the binary file, append to its content if it already exists
  if (sos.size() == 0)
    std::cerr << " There are no vsol spatial objects in the vector!\n";
  else {
    vsl_b_ifstream istr(bin_filename);
    std::vector<vsol_spatial_object_2d_sptr> sos_in;
    if (! !istr) {
      vsl_b_read(istr, sos_in);
      istr.close();
    }

    vsl_b_ofstream ostr(bin_filename);
    if (!ostr)
      std::cerr << "Failed to open output stream " << bin_filename << std::endl;
    else {
      std::cout << "there are " << sos.size() << " vsol objects, appending to binary file: " << bin_filename << " which currently has: " << sos_in.size() << " objects" << std::endl;
      for (const auto & ii : sos_in)
        sos.push_back(ii);
      vsl_b_write(ostr, sos);
      ostr.close();
    }
  }

  return hit;
}

// process to render an mask image that represent the OSM polygons stored in a kml file
namespace volm_render_kml_polygon_mask_process_globals
{
  unsigned n_inputs_ = 7;
  unsigned n_outputs_ = 1;

  bool contains(vgl_polygon<double> const& poly, double const& ptx, double const& pty);
}

bool volm_render_kml_polygon_mask_process_globals::contains(vgl_polygon<double> const& poly, double const& ptx, double const& pty)
{
  // when poly sheets overlap, the poly.contain method will return false for pts located inside the overlapped region
  // this function will returns true if the given point is inside any single sheet of the polygon
  unsigned num_sheet = poly.num_sheets();
  for (unsigned i = 0; i < num_sheet; i++) {
    vgl_polygon<double> single_sheet_poly(poly[i]);
    if (single_sheet_poly.contains(ptx, pty))
      return true;
  }
  return false;
}

bool volm_render_kml_polygon_mask_process_cons(bprb_func_process& pro)
{
  using namespace volm_render_kml_polygon_mask_process_globals;
  // this process takes 5 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";  // input mask byte image to be write/modify
  input_types_[1] = "double";                    // lower left lon
  input_types_[2] = "double";                    // lower left lat
  input_types_[3] = "double";                    // upper right lon
  input_types_[4] = "double";                    // upper right lat
  input_types_[5] = "vcl_string";                // polygon kml file
  input_types_[6] = "unsigned";                  // pixel value
  // this process takes 1 outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr";  // output geo camera for the image
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_render_kml_polygon_mask_process(bprb_func_process& pro)
{
  using namespace volm_render_kml_polygon_mask_process_globals;
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!\n";
    return false;
  }
  // get inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr in_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  auto ll_lon = pro.get_input<double>(in_i++);
  auto ll_lat = pro.get_input<double>(in_i++);
  auto ur_lon = pro.get_input<double>(in_i++);
  auto ur_lat = pro.get_input<double>(in_i++);
  std::string poly_kml_file = pro.get_input<std::string>(in_i++);
  auto mask_value = pro.get_input<unsigned>(in_i++);

  // load the image
  auto* image = dynamic_cast<vil_image_view<vxl_byte>*>(in_img_sptr.ptr());
  if (!image) {
    std::cerr << pro.name() << ": Unsupported image pixel type -- " << in_img_sptr->pixel_format() << ", only byte image is supported!\n";
    return false;
  }
  // load the polygon
  if (!vul_file::exists(poly_kml_file)) {
    std::cerr << pro.name() << ": can not find polygon kml file " << poly_kml_file << "!\n";
    return false;
  }
  vgl_polygon<double> poly = bkml_parser::parse_polygon(poly_kml_file);
  std::cout << poly.num_sheets() << " sheets are loaded" << std::endl;

  // compute the geo camera from image size
  unsigned ni = image->ni();
  unsigned nj = image->nj();

  double scale_x =  (ur_lon - ll_lon)/ni;
  double scale_y = -(ur_lat - ll_lat)/nj;
  vnl_matrix<double> trans_matrix(4,4,0.0);
  trans_matrix[0][0] = scale_x;
  trans_matrix[1][1] = scale_y;
  trans_matrix[0][3] = ll_lon;
  trans_matrix[1][3] = ur_lat;
  vpgl_lvcs_sptr lvcs_dummy = new vpgl_lvcs;
  vpgl_geo_camera* out_cam = new vpgl_geo_camera(trans_matrix, lvcs_dummy);
  out_cam->set_scale_format(true);

  // convert the input polygon to image domain
  std::vector<vgl_polygon<double> > img_poly;
  unsigned n_sheets = poly.num_sheets();
  for (unsigned s_idx = 0; s_idx < n_sheets; s_idx++) {
    unsigned n_vertices = poly[s_idx].size();
    vgl_polygon<double> single_sheet_poly;
    single_sheet_poly.new_sheet();
    for (unsigned v_idx = 0; v_idx < n_vertices; v_idx++) {
      double lon = poly[s_idx][v_idx].x();
      double lat = poly[s_idx][v_idx].y();
      double u, v;
      out_cam->global_to_img(lon, lat, 0.0, u, v);
      double ii = std::floor(u+0.5);
      double jj = std::floor(v+0.5);
      single_sheet_poly.push_back(ii, jj);
    }
    img_poly.push_back(single_sheet_poly);
  }

  // fill the image
  for (const auto & ii : img_poly)
  {
    vgl_polygon_scan_iterator<double> psi(ii);
    for (psi.reset(); psi.next(); ) {
      int j = psi.scany();
      for (int i = psi.startx(); i <= psi.endx(); i++) {
        if (i < 0 || j < 0 || i >= ni || j >= nj)
          continue;
        (*image)(i,j) = (unsigned char)(mask_value);
      }
    }
  }

  // output
  pro.set_output_val<vpgl_camera_double_sptr>(0, out_cam);

  return true;
}
