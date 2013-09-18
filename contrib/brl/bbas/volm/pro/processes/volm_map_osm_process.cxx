// This is brl/bbas/volm/pro/processes/volm_map_osm_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
//         Take an ortho image, ortho camera and a list of osm objects, map the roads onto the image
//
//
#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <volm/volm_osm_objects.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>

//:
//  Take an ortho image and its camera, a bin file with an osm object list, map the objects onto the image
bool volm_map_osm_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");  // ortho image - pass as converted to byte image
  input_types.push_back("vpgl_camera_double_sptr");  // ortho camera
  input_types.push_back("vcl_string");   // bin file with osm object list
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr"); // a color image with red channel the objects that are overlaid
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool volm_map_osm_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 3) {
    vcl_cout << "volm_map_osm_process: The number of inputs should be 3" << vcl_endl;
    return false;
  }

  // get the inputs
  vil_image_view_base_sptr img_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(1);
  vcl_string osm_file  = pro.get_input<vcl_string>(2);

  vpgl_geo_camera *geo_cam = dynamic_cast<vpgl_geo_camera*>(cam.ptr());
  if (!geo_cam) {
    vcl_cerr << "Cannot cast the input cam to a vpgl_geo_camera!\n";
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
  vcl_cout << " =========== Load volumetric open stree map objects... " << " ===============" << vcl_endl;
  vcl_cout << " \t number of roads in osm: " << osm_objs.num_roads() << vcl_endl;
  
  bool hit = false;
  vcl_vector<vcl_vector<vcl_pair<int, int> > > img_lines;
  vcl_vector<volm_osm_object_line_sptr> loc_lines = osm_objs.loc_lines();
  for (unsigned i = 0; i < loc_lines.size(); i++) {
    vcl_vector<vgl_point_2d<double> > pts = loc_lines[i]->line();
    vcl_vector<vcl_pair<int, int> > img_line;
    for (unsigned j = 0; j < pts.size(); j++) {
      double u, v;
      geo_cam->global_to_img(pts[j].x(), pts[j].y(), 0, u, v);
      int uu = vcl_floor(u + 0.5f);
      int vv = vcl_floor(v + 0.5f);
      if (uu >= 0 && vv >= 0 && uu < img_sptr->ni() && vv < img_sptr->nj()) {
        //out_img(uu, vv).r = 255;
        img_line.push_back(vcl_pair<int, int>(uu,vv));
        hit = true;
      }
    }
    if (img_line.size() > 0)
      img_lines.push_back(img_line);
  }
  vcl_cout << "number of img lines: " << img_lines.size() << vcl_endl;
  if (hit) {
    for (unsigned i = 0; i < img_lines.size(); i++) {
      vcl_cout << "img line: " << i << " number of pts: " << img_lines[i].size() << " ";
      out_img(img_lines[i][0].first, img_lines[i][0].second).r = 255;
      for (unsigned j = 1; j < img_lines[i].size(); j++) {
        double prev_u = img_lines[i][j-1].first;
        double prev_v = img_lines[i][j-1].second;
        double dx = img_lines[i][j].first - img_lines[i][j-1].first;
        double dy = img_lines[i][j].second - img_lines[i][j-1].second;
        double ds = vcl_sqrt(dx*dx + dy*dy);
        vcl_cout << " ds: " << ds << " ";
        double cos = dx/ds; double sin = dy/ds;
        unsigned cnt = 1;
        while (ds > 0.1) {
          //out_img(prev_u + cnt*1*cos, prev_v + cnt*1*sin).r = 255;  // delta is 1 pixel
          
          int uu = vcl_floor(prev_u + cnt*1*cos + 0.5f);
          int vv = vcl_floor(prev_v + cnt*1*sin + 0.5f);
          if (uu >= 0 && vv >= 0 && uu < img_sptr->ni() && vv < img_sptr->nj()) 
            out_img(uu, vv).r = 255;  // delta is 1 pixel

          cnt++;
          ds -= 1;
        }
        vcl_cout << cnt << " pts in the image!\n";
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
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");  // ortho image - pass as color image where each segment has a unique color
  input_types.push_back("vpgl_camera_double_sptr");  // ortho camera
  input_types.push_back("vcl_string");   // bin file with osm object list
  input_types.push_back("vil_image_view_base_sptr");  // ortho height image 
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr"); // a color image with output segments
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool volm_map_segments_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 4) {
    vcl_cout << "volm_map_segments_process: The number of inputs should be 4" << vcl_endl;
    return false;
  }

  // get the inputs
  vil_image_view_base_sptr img_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(1);
  vcl_string osm_file  = pro.get_input<vcl_string>(2);
  vil_image_view_base_sptr input_height_image_sptr = pro.get_input<vil_image_view_base_sptr>(3);

  vcl_cout << "!!!!!!!!!!! input height img ni: " << input_height_image_sptr->ni() << "  nj: " << input_height_image_sptr->nj() << vcl_endl;
  vil_image_view<float> height_image = vil_convert_cast(float(), input_height_image_sptr);

  vpgl_geo_camera *geo_cam = dynamic_cast<vpgl_geo_camera*>(cam.ptr());
  if (!geo_cam) {
    vcl_cerr << "Cannot cast the input cam to a vpgl_geo_camera!\n";
    return false;
  }
  
  vcl_cout << "constructing segment map..!\n";
  vil_image_view<vil_rgb<vxl_byte> > img(img_sptr);  // input segments
  vcl_map<vcl_pair<unsigned char, vcl_pair<unsigned char, unsigned char> >, vcl_pair<vcl_vector<vcl_pair<int, int> >, vil_rgb<vxl_byte> > > segment_map;
  vcl_map<vcl_pair<unsigned char, vcl_pair<unsigned char, unsigned char> >, vcl_pair<vcl_vector<vcl_pair<int, int> >, vil_rgb<vxl_byte> > >::iterator iter;
  
  for (unsigned i = 0; i < img.ni(); i++)
    for (unsigned j = 0; j < img.nj(); j++) {
      vcl_pair<unsigned char, vcl_pair<unsigned char, unsigned char> > seg_color(img(i,j).r, vcl_pair<unsigned char, unsigned char>(img(i,j).g, img(i,j).b));
      iter = segment_map.find(seg_color);
      if (iter != segment_map.end())
        iter->second.first.push_back(vcl_pair<int, int>(i,j));
      else {
        vcl_vector<vcl_pair<int, int> > tmp;
        tmp.push_back(vcl_pair<int, int>(i,j));   
        vcl_pair<vcl_vector<vcl_pair<int, int> >, vil_rgb<vxl_byte> > p(tmp, vil_rgb<vxl_byte>(255, 255, 255));         
        segment_map[seg_color] = p;
      }
    }
  vcl_cout << "there are " << segment_map.size() << " unique segments in the image!\n";

  vil_image_view<vil_rgb<vxl_byte> > out_img(img_sptr->ni(), img_sptr->nj(), 1);
  out_img.fill(vil_rgb<unsigned char>(0,0,0));

  vil_rgb<vxl_byte> road_color(255, 0, 0);
    
  // read the osm objects
  // load the volm_osm object
  volm_osm_objects osm_objs(osm_file);
  vcl_cout << " =========== Load volumetric open street map objects... " << " ===============" << vcl_endl;
  vcl_cout << " \t number of roads in osm: " << osm_objs.num_roads() << vcl_endl;
  
  bool hit = false;
  vcl_vector<vcl_vector<vcl_pair<int, int> > > img_lines;
  vcl_vector<volm_osm_object_line_sptr> loc_lines = osm_objs.loc_lines();
  for (unsigned i = 0; i < loc_lines.size(); i++) {
    vcl_vector<vgl_point_2d<double> > pts = loc_lines[i]->line();
    vcl_vector<vcl_pair<int, int> > img_line;
    for (unsigned j = 0; j < pts.size(); j++) {
      double u, v;
      geo_cam->global_to_img(pts[j].x(), pts[j].y(), 0, u, v);
      int uu = vcl_floor(u + 0.5f);
      int vv = vcl_floor(v + 0.5f);
      if (uu >= 0 && vv >= 0 && uu < img_sptr->ni() && vv < img_sptr->nj()) {
        img_line.push_back(vcl_pair<int, int>(uu,vv));
        hit = true;
      }
    }
    if (img_line.size() > 0)
      img_lines.push_back(img_line);
  }
  vcl_cout << "number of img lines: " << img_lines.size() << vcl_endl;
  if (hit) {
    vcl_vector<float> road_heights;

    for (unsigned i = 0; i < img_lines.size(); i++) {
      vcl_cout << "img line: " << i << " number of pts: " << img_lines[i].size() << " ";
      // find the segment color of this pixel
      vil_rgb<vxl_byte> segment_color = img(img_lines[i][0].first, img_lines[i][0].second);
      // make the output color of this group, the road color: red
      vcl_pair<unsigned char, vcl_pair<unsigned char, unsigned char> > seg_color(segment_color.r, vcl_pair<unsigned char, unsigned char>(segment_color.g, segment_color.b));
      segment_map[seg_color].second = road_color;
      if (height_image(img_lines[i][0].first, img_lines[i][0].second) > 0)
        road_heights.push_back(height_image(img_lines[i][0].first, img_lines[i][0].second));
      for (unsigned j = 1; j < img_lines[i].size(); j++) {
        double prev_u = img_lines[i][j-1].first;
        double prev_v = img_lines[i][j-1].second;
        double dx = img_lines[i][j].first - img_lines[i][j-1].first;
        double dy = img_lines[i][j].second - img_lines[i][j-1].second;
        double ds = vcl_sqrt(dx*dx + dy*dy);
        vcl_cout << " ds: " << ds << " ";
        double cos = dx/ds; double sin = dy/ds;
        unsigned cnt = 1;
        while (ds > 0.1) {
          int uu = vcl_floor(prev_u + cnt*1*cos + 0.5f);
          int vv = vcl_floor(prev_v + cnt*1*sin + 0.5f);
          if (uu >= 0 && vv >= 0 && uu < img_sptr->ni() && vv < img_sptr->nj()) {
            vil_rgb<vxl_byte> segment_color = img(uu, vv);
            vcl_pair<unsigned char, vcl_pair<unsigned char, unsigned char> > seg_color(segment_color.r, vcl_pair<unsigned char, unsigned char>(segment_color.g, segment_color.b));
            segment_map[seg_color].second = road_color;
            if (height_image(uu,vv) > 0)
              road_heights.push_back(height_image(uu,vv));
          }
          cnt++;
          ds -= 1;
        }
        vcl_cout << cnt << " pts in the image!\n";
      }
    }

    // find the height of each segment
    vcl_map<vcl_pair<unsigned char, vcl_pair<unsigned char, unsigned char> >, float > segment_height_map;
    vcl_map<vcl_pair<unsigned char, vcl_pair<unsigned char, unsigned char> >, float >::iterator height_iter;
    
    for (iter = segment_map.begin(); iter != segment_map.end(); iter++) {
      vcl_vector<vcl_pair<int, int> >& tmp = iter->second.first;
      if (tmp.size() < 1) {
        segment_height_map[iter->first] = -1;
        continue;
      }
      // find the median height of its pixels
      vcl_vector<float> heights;
      for (unsigned i = 0; i < tmp.size(); i++) {
        if (height_image(tmp[i].first, tmp[i].second) > 0)
          heights.push_back(height_image(tmp[i].first, tmp[i].second));
      }
      if (heights.size() <= 0)
        continue;
      vcl_sort(heights.begin(), heights.end());
      int med = (int)vcl_floor(heights.size()/2.0);
      float median_height = heights[med];
      segment_height_map[iter->first] = median_height;
      /*if (iter->second.second == road_color) {
        road_heights.push_back(median_height);
      }*/
    }
    vcl_sort(road_heights.begin(), road_heights.end());
    vcl_cout << "road heights: " << vcl_endl;
    for (unsigned i = 0; i < road_heights.size(); i++)
      vcl_cout << road_heights[i] << " ";
    vcl_cout << "\n";
    int med = (int)vcl_floor(road_heights.size()/2.0);
    vcl_cout << "road heights size: " << road_heights.size() << " med: " << med << vcl_endl; vcl_cout.flush();
    vcl_cout << "median road height: " << road_heights[med] << vcl_endl; vcl_cout.flush();
  
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
      vcl_vector<vcl_pair<int, int> >& tmp = iter->second.first;
      for (unsigned i = 0; i < tmp.size(); i++) {
        out_img(tmp[i].first, tmp[i].second) = iter->second.second;
      }
    }

  }

  vil_image_view_base_sptr out_img_sptr = new vil_image_view<vxl_byte>(out_img);
  
  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_sptr);
  return hit;
}


