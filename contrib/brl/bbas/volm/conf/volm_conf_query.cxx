#include <cmath>
#include <iostream>
#include <algorithm>
#include <volm/conf/volm_conf_query.h>
//:
// \file
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bpgl/bpgl_camera_utils.h>
#include <vul/vul_file.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vsph/vsph_spherical_coord.h>
#include <vsph/vsph_sph_point_3d.h>
#include <vgl/vgl_polygon_scan_iterator.h>

// default constructor
volm_conf_query::volm_conf_query()
{
  ni_ = 0;  nj_ = 0;  ncam_ = 0;  nobj_ = 0;  nref_ = 0;
  dm_ = nullptr;
  cameras_.clear();  camera_strings_.clear();
  ref_obj_name_.clear();  conf_objects_.clear();
  tol_in_pixel_ = 0;
}

// create query from labeled depth map scene
volm_conf_query::volm_conf_query(const volm_camera_space_sptr& cam_space, const depth_map_scene_sptr& depth_scene, int const& tol_in_pixel)
{
  tol_in_pixel_ = tol_in_pixel;
  dm_ = depth_scene;
  ni_ = dm_->ni();  nj_ = dm_->nj();
  altitude_ = cam_space->altitude();
  // create reference objects from depth scene
  bool success = this->parse_ref_object(dm_);
  assert(success && "volm_conf_query: parse reference object list from depth scene failed");
  nref_ = (unsigned)this->ref_obj_name_.size();
  std::cout << nref_ << " reference configurational objects are loaded: ";
  for (unsigned i = 0; i < nref_; i++)
    std::cout << ref_obj_name_[i] << ' ';
  std::cout << '\n';
  // create cameras
  success = this->create_perspective_cameras(cam_space);
  assert(success && "volm_conf_query: construct perspective cameras from camera space failed");
  ncam_ = (unsigned)this->cameras_.size();
  std::cout << ncam_ << " cameras are created: " << std::endl;
  // construct configurational object from 3d polygons
  success  = this->create_conf_object();
  assert(success && "volm_conf_query: construct configurational objects failed");
}

bool volm_conf_query::parse_ref_object(const depth_map_scene_sptr& dm)
{
  ref_obj_name_.clear();
  if (dm == nullptr)
    return false;
  // note that sky object should never be reference object
  if (dm->ground_plane().size())
    for (unsigned i = 0; i < dm->ground_plane().size(); i++)
      if ( dm->ground_plane()[i]->is_ref() && dm->ground_plane()[i]->active())
        ref_obj_name_.push_back(dm->ground_plane()[i]->name());
  if (dm->scene_regions().size())
    for (unsigned i = 0; i < dm->scene_regions().size(); i++)
      if ( dm->scene_regions()[i]->is_ref() && dm->scene_regions()[i]->active())
        ref_obj_name_.push_back(dm->scene_regions()[i]->name());
  if (ref_obj_name_.empty())
    return false;
  return true;
}

bool volm_conf_query::create_perspective_cameras(const volm_camera_space_sptr& cam_space)
{
  // iterate over valid cameras in the camera space
  // Note that ground plane construct has been applied on camera space
  cameras_.clear();  camera_strings_.clear();  camera_angles_.clear();
  std::vector<unsigned> const& valid_cams = cam_space->valid_indices();
  for (unsigned int valid_cam : valid_cams) {
    vpgl_perspective_camera<double> cam = cam_space->camera(valid_cam);
    std::string cam_str = cam_space->get_string(valid_cam);
    cameras_.push_back(cam);
    camera_strings_.push_back(cam_str);
    camera_angles_.push_back(cam_space->camera_angles(valid_cam));
  }
  return true;
}


bool volm_conf_query::create_conf_object()
{
  conf_objects_.clear();
  // loop over each calibrated camera to construct the list of configurational objects
  for (unsigned cam_id =0;  cam_id < ncam_;  cam_id++)
  {
    std::cout << "camera: " << this->camera_strings_[cam_id] << std::endl;
    vpgl_perspective_camera<double> pcam = cameras_[cam_id];
    // create a map of volm_conf_object
    std::map<std::string, volm_conf_object_sptr> conf_object;
    std::map<std::string, std::pair<unsigned, unsigned> > conf_pixels;
    // only consider non-planar objects
    std::vector<depth_map_region_sptr> regions = dm_->scene_regions();
    for (unsigned r_idx = 0; r_idx < regions.size(); r_idx++)
    {
      if (!regions[r_idx]->active()) {
        std::cout << "\t\t region " << r_idx << " " << regions[r_idx]->name() << " is not active, IGNORED" << std::endl;
        continue;
      }
      //std::cout << "\t\t projecting " << regions[r_idx]->name() << "..." << std::flush << std::endl;
      vgl_polygon<double> poly = bsol_algs::vgl_from_poly(regions[r_idx]->region_2d());
      float theta = -1.0f, dist = -1.0f, height = -1.0f;
      unsigned i, j;
      // project all ground vertices on the polygon to 3-d world points if the vertex is under the horizon
      this->project(pcam, poly, dist, theta, i, j);
      // here to use the distance from depth map scene
      dist   = regions[r_idx]->min_depth();
      height = regions[r_idx]->height();
      //std::cout << "\t\t min_dist: " << dist << ", phi: " << theta << ", pixel: " << i << "x" << j << std::flush << std::endl;
      if (theta < 0)
        continue;
      // create a configurational object for it
      unsigned char land_id = regions[r_idx]->land_id();
      volm_conf_object_sptr conf_obj = new volm_conf_object(theta, dist, height, land_id);
      //std::cout << "conf_obj: ";  conf_obj->print(std::cout);
      conf_object.insert(std::pair<std::string, volm_conf_object_sptr>(regions[r_idx]->name(), conf_obj));
      std::pair<unsigned, unsigned> tmp_pair(i,j);
      conf_pixels.insert(std::pair<std::string, std::pair<unsigned, unsigned> >(regions[r_idx]->name(), tmp_pair));
    }
    // update the configurational object for current camera
    conf_objects_.push_back(conf_object);
    conf_objects_pixels_.push_back(conf_pixels);
  }

  if (tol_in_pixel_ == 0)
  {
    for (unsigned cam_id =0;  cam_id < ncam_;  cam_id++)
    {
      std::map<std::string, volm_conf_object_sptr> conf_objs = conf_objects_[cam_id];
      std::map<std::string, std::pair<unsigned, unsigned> > conf_pixels = conf_objects_pixels_[cam_id];
      std::map<std::string, std::pair<float, float> > conf_dist_tol;
      for (auto & conf_pixel : conf_pixels)
      {
        float min_dist = conf_objs[conf_pixel.first]->dist()*0.8;
        float max_dist = conf_objs[conf_pixel.first]->dist()*1.2;
        conf_dist_tol.insert(std::pair<std::string, std::pair<float, float> >(conf_pixel.first, std::pair<float, float>(min_dist, max_dist)));
      }
      conf_objects_d_tol_.push_back(conf_dist_tol);
    }
  }
  else
  {
    // calculate the distance tolerance for each configuration object
    int nbrs4_delta[4][2] = {  {-1*tol_in_pixel_,    tol_in_pixel_},
                               {   tol_in_pixel_,    tol_in_pixel_},
                               {   tol_in_pixel_, -1*tol_in_pixel_},
                               {-1*tol_in_pixel_, -1*tol_in_pixel_}
                            };
    unsigned num_nbrs = 4;
    for (unsigned cam_id =0;  cam_id < ncam_;  cam_id++)
    {
      vpgl_perspective_camera<double> pcam = cameras_[cam_id];
      std::map<std::string, volm_conf_object_sptr> conf_objs = conf_objects_[cam_id];
      std::map<std::string, std::pair<unsigned, unsigned> > conf_pixels = conf_objects_pixels_[cam_id];
      std::map<std::string, std::pair<float, float> > conf_dist_tol;
      for (auto & conf_pixel : conf_pixels)
      {
        unsigned i = conf_pixel.second.first;  unsigned j = conf_pixel.second.second;
        float min_dist = conf_objs[conf_pixel.first]->dist();
        float max_dist = min_dist;
        for (unsigned k = 0; k < num_nbrs; k++) {
          int nbr_i = i + nbrs4_delta[k][0];
          int nbr_j = j + nbrs4_delta[k][1];
          float dist, phi;
          this->project(pcam, nbr_i, nbr_j, dist, phi);
          if (dist < 0 || phi < 0)
            continue;
          if (dist <= min_dist)  min_dist = dist;
          if (dist >= max_dist)  max_dist = dist;
          //std::cout << "\t pixel " << nbr_i << ", " << nbr_j << ", dist: " << dist << ", min_dist: " << min_dist << ", max_dist: " << max_dist << std::endl;
        }
        conf_dist_tol.insert(std::pair<std::string, std::pair<float, float> >(conf_pixel.first, std::pair<float, float>(min_dist, max_dist)));
      }
      conf_objects_d_tol_.push_back(conf_dist_tol);
    }
  }

  return true;
}

// calculate the minimum distance, the average angle values from the label boundary and the pixel associated with it
void volm_conf_query::project(vpgl_perspective_camera<double> const& cam,
                              vgl_polygon<double> const& poly,
                              float& min_dist, float& phi, unsigned& i, unsigned& j)
{
  min_dist = -1.0f;  phi = -1.0f;  i = 0;  j=0;
  // only consider the first sheet
  unsigned n_vertices = poly[0].size();
  std::map<float, float> pt_pairs;
  std::map<float, std::pair<unsigned, unsigned> > pt_pixels;
  // calculate the angle value from all polygons
  std::set<float> dist_values;
  std::vector<float> phi_values;
  std::vector<std::pair<double, double> > pixel_values;
  for (unsigned v_idx = 0; v_idx < n_vertices; v_idx++)
  {
    double x = poly[0][v_idx].x();
    double y = poly[0][v_idx].y();
    if (x < 0 || x >= ni_ || y < 0 || y >= nj_)
      continue;
    float dist, phi;
    this->project(cam, x, y, dist, phi);
    if (dist < 0)
      continue;
    dist_values.insert(dist);
    phi_values.push_back(phi);
    pixel_values.emplace_back(x,y);
  }
  if (phi_values.empty())  // no vertices projects to ground
    return;
  min_dist = *(dist_values.begin());
  // calculate the average angle
  float phi_value = 0.0f;
  double xi = 0.0, yi = 0.0;
  for (float pi : phi_values)
    phi_value += pi;
  for (auto & pixel_value : pixel_values) {
    xi += pixel_value.first;
    yi += pixel_value.second;
  }
  phi_value /= phi_values.size();
  xi /= pixel_values.size();
  yi /= pixel_values.size();

  phi = phi_value;
  i = (unsigned)(std::floor(xi+0.5));
  j = (unsigned)(std::floor(yi+0.5));
  return;

# if 0
  for (unsigned v_idx = 0; v_idx < n_vertices; v_idx++) {
    double x = poly[0][v_idx].x();
    double y = poly[0][v_idx].y();
    float dist, phi;
    this->project(cam, x, y, dist, phi);
    if (dist < 0)
      continue;
    pt_pairs.insert(std::pair<float, float>(dist, phi));
#if 0
    std::cout << "\t\tpixel: " << x << "x" << y << " is under horizon, has ray " << cp
             << " and spherical coords: " << sp << " dist: " << dist << ", theta: " << sp.phi_ << std::flush << std::endl;
#endif


    pt_pixels.insert(std::pair<float, std::pair<unsigned, unsigned> >(dist, std::pair<unsigned, unsigned>((unsigned)x,(unsigned)y)));
  }
  if (pt_pairs.empty())  // no vertices projects to ground
    return;
  min_dist = pt_pairs.begin()->first;
  phi = pt_pairs[min_dist];

  i = pt_pixels[min_dist].first;
  j = pt_pixels[min_dist].second;
  //std::cout << "\t\tmin_dist: " << min_dist << ", phi: " << phi << ", pixel: " << i << "x" << j << std::flush << std::endl;
  return;
#endif

}

void volm_conf_query::project(vpgl_perspective_camera<double> const& cam,
                              double const& pixel_i, double const& pixel_j,
                              float& dist, float& phi)
{
  dist = -1.0; phi = -1.0;
  vgl_line_2d<double> horizon = bpgl_camera_utils::horizon(cam);
  if (pixel_i < 0 || pixel_i >= ni_ || pixel_j < 0 || pixel_j >= nj_)
    return;
  double yl = line_coord(horizon, pixel_i);
  if (pixel_j < yl)  // given img point is above the horizon
    return;
  // obtain the back project ray to calculate the distance
  vgl_ray_3d<double> ray = cam.backproject(pixel_i, pixel_j);
  // obtain the angular relative to camera x axis
  vgl_point_3d<double> cp(ray.direction().x(), ray.direction().y(), ray.direction().z());
  vsph_spherical_coord sph_coord;
  vsph_sph_point_3d sp;
  sph_coord.spherical_coord(cp, sp);
  // calculate the distance
  dist = std::tan(vnl_math::pi - sp.theta_)*altitude_;
  phi = sp.phi_;
  return;
}

double volm_conf_query::line_coord(vgl_line_2d<double> const& line, double const& x)
{
  if (line.b() == 0)
    return 0.0;
  else
    return -line.a()/line.b()*x - line.c()/line.b();
}


bool volm_conf_query::visualize_ref_objs(std::string const& in_file, std::string const& out_folder)
{
  if (!vul_file::exists(in_file))
    return false;
  vil_image_view<vil_rgb<vxl_byte> > src_img = vil_load(in_file.c_str());
  std::vector<depth_map_region_sptr> regions = dm_->scene_regions();

  for (unsigned cam_id = 0; cam_id < ncam_; cam_id++) {
    vil_image_view<vil_rgb<vxl_byte> > img;
    img.deep_copy(src_img);
    // plot horizontal line
    vgl_line_2d<double> h_line = bpgl_camera_utils::horizon(cameras_[cam_id]);
    std::vector<vgl_point_2d<double> > h_line_pixels;
    h_line_pixels.clear();
    for (unsigned x = 0; x < ni_; x++) {
      double y = (std::floor)(line_coord(h_line, x));
      h_line_pixels.emplace_back((double)x, y);
    }
    this->plot_line_into_image(img, h_line_pixels, 0, 0, 0, 6);
    // plot the non-ground depth map scenes first
    for (auto & region : regions) {
      // not plot the object that is not projected based on current camera
      if (conf_objects_[cam_id].find(region->name()) == conf_objects_[cam_id].end())
        continue;
      vgl_polygon<double> poly = bsol_algs::vgl_from_poly(region->region_2d());
      poly[0].push_back(poly[0][0]);
      unsigned char r,g,b;
      double width;
      if (std::find(ref_obj_name_.begin(), ref_obj_name_.end(), region->name()) == ref_obj_name_.end()) {
        r = volm_osm_category_io::volm_land_table[region->land_id()].color_.r;
        g = volm_osm_category_io::volm_land_table[region->land_id()].color_.g;
        b = volm_osm_category_io::volm_land_table[region->land_id()].color_.b;
        width = 5.0;
      } else {
        r = 255; g = 0; b = 0; width = 7.0;
      }
      //std::cout << "object: " << regions[i]->name() << ", land: " << (int)regions[i]->land_id() << ", color: " << (int)r << "," << (int)g << "," << (int)b << std::endl;
      this->plot_line_into_image(img, poly[0],r,g,b,width);
    }
    // plot the configurational object
    for (auto mit = conf_objects_pixels_[cam_id].begin(); mit != conf_objects_pixels_[cam_id].end(); ++mit ) {
      unsigned char land_id = conf_objects_[cam_id][mit->first]->land();
      unsigned char r,g,b;
      double width;
      if (std::find(ref_obj_name_.begin(), ref_obj_name_.end(), mit->first) == ref_obj_name_.end()) {
        r = volm_osm_category_io::volm_land_table[land_id].color_.r;
        g = volm_osm_category_io::volm_land_table[land_id].color_.g;
        b = volm_osm_category_io::volm_land_table[land_id].color_.b;
        width = 20.0;
      }else {
        r = 255; g = 255; b = 255; width = 30.0;
      }
      this->plot_dot_into_image(img, vgl_point_2d<double>((double)mit->second.first, (double)mit->second.second),r,g,b,width);
    }
    std::string filename = vul_file::strip_extension(vul_file::strip_directory(in_file));
    std::string out_file = out_folder + "/" + filename + "_" + camera_strings_[cam_id] + ".tif";
    vil_save(img, out_file.c_str());
  }
  return true;

}

#if 0
bool volm_conf_query::generate_top_views(std::string const& out_folder, std::string const& filename_pre)
{
  // ensure a maximum image size
  unsigned half_ni = 0;  unsigned half_nj = 0;
  for (unsigned cam_id = 0; cam_id < ncam_; cam_id++)
  {
    std::map<std::string, volm_conf_object_sptr> conf_obj = conf_objects_[cam_id];
    for (std::map<std::string, volm_conf_object_sptr>::iterator mit = conf_obj.begin();  mit != conf_obj.end();  ++mit) {
      float x = mit->second->dist() * std::cos(mit->second->theta());
      float y = mit->second->dist() * std::sin(mit->second->theta());
      if (half_ni < std::ceil(x))  half_ni = std::ceil(x);
      if (half_nj < std::ceil(y))  half_nj = std::ceil(y);
      //std::cout << "obj " << mit->first << " has distance " << mit->second->dist() << " and angle " << mit->second->theta()
      //         << ", pixel " << std::ceil(x) << "x" << std::ceil(y)
      //         << ", img size" << 2*half_ni << "x" << 2*half_nj << std::endl;
    }
  }
  unsigned ni,nj;
  ni = 2*half_ni;
  nj = 2*half_nj;
  for (unsigned cam_id = 0; cam_id < ncam_; cam_id++)
  {
    std::string cam_string = camera_strings_[cam_id];
    vil_image_view<vil_rgb<vxl_byte> > img(ni, nj);
    img.fill(vil_rgb<vxl_byte>(127,127,127));
    // perform the coordinate transformation (putting camera center to the image center)
    float xo = half_ni, yo = half_nj;
    // plot camera center
    this->plot_dot_into_image(img, vgl_point_2d<double>(xo,yo), 0, 0, 0, 5.0);

    std::map<std::string, volm_conf_object_sptr> conf_obj = conf_objects_[cam_id];
    for (std::map<std::string, volm_conf_object_sptr>::iterator mit = conf_obj.begin();  mit != conf_obj.end();  ++mit) {
      float xc = mit->second->dist() * std::cos(mit->second->theta());
      float yc = mit->second->dist() * std::sin(mit->second->theta());
      unsigned char r, g, b;  double width;
      unsigned char land_id = mit->second->land();
      if (std::find(ref_obj_name_.begin(), ref_obj_name_.end(), mit->first) == ref_obj_name_.end()) {
        r = volm_osm_category_io::volm_land_table[land_id].color_.r;
        g = volm_osm_category_io::volm_land_table[land_id].color_.g;
        b = volm_osm_category_io::volm_land_table[land_id].color_.b;
        width = 10.0;
      }else {
        r = 255; g = 255; b = 255; width = 25.0;
      }
      //std::cout << "(" << xc << "," << yc << "), (" << xo << "," << yo << ") --> (" << xc+xo << "," << yo-yc << ")" << std::endl;
      this->plot_dot_into_image(img, vgl_point_2d<double>(xc+xo,yo-yc), r, g, b, width);
    }
    std::string out_file = out_folder + "/" + filename_pre + "_" + cam_string + ".tif";
    //std::cout << "save image to " << out_file << std::endl;
    vil_save(img, out_file.c_str());
  }
  return true;
}
#endif

void volm_conf_query::plot_line_into_image(vil_image_view<vil_rgb<vxl_byte> >& image,
                                           std::vector<vgl_point_2d<double> > const& line,
                                           unsigned char const& r, unsigned char const& g, unsigned char const& b,
                                           double const& width)
{
  vgl_polygon<double> img_poly;
  volm_io_tools::expend_line(line, width, img_poly);
  vgl_polygon_scan_iterator<double> it(img_poly, true);
  for (it.reset(); it.next();  ) {
    int y = it.scany();
    for (int x = it.startx(); x <= it.endx(); ++x) {
      if ( x >= 0 && y >= 0 && x < (int)image.ni() && y < (int)image.nj()) {
        image(x, y) = vil_rgb<vxl_byte>(r, g, b);
      }
    }
  }
}

void volm_conf_query::plot_dot_into_image(vil_image_view<vil_rgb<vxl_byte> >& image,
                                          vgl_point_2d<double> const& pt,
                                          unsigned char const& r, unsigned char const& g, unsigned char const& b,
                                          double const& radius)
{
  vgl_polygon<float> img_poly;
  img_poly.new_sheet();
  std::set<float> angles;
  for (unsigned i = 0; i <= 36; i++)
    angles.insert(i*10.0f);
  angles.insert(45.0f);  angles.insert(135.0f);  angles.insert(225.0f);  angles.insert(315.0f);
  for (const auto & angle : angles) {
    float x = pt.x() + radius*std::cos(angle*vnl_math::pi_over_180);
    float y = pt.y() + radius*std::sin(angle*vnl_math::pi_over_180);
    img_poly.push_back(x, y);
  }
  vgl_polygon_scan_iterator<float> it(img_poly, true);
  for (it.reset(); it.next();  ) {
    int y = it.scany();
    for (int x = it.startx(); x <= it.endx(); ++x) {
      if ( x>=0 && y>=0 && x<(int)image.ni() && y<(int)image.nj())
        image(x,y) = vil_rgb<vxl_byte>(r, g, b);
    }
  }
}
