#include "volm_query.h"
//:
// \file
#include <bpgl/bpgl_camera_utils.h>
#include <volm/volm_spherical_container.h>
#include <bsol/bsol_algs.h>
#include <vil/vil_save.h>
#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <vcl_algorithm.h>

#define TOL -1E-8

volm_query::volm_query(vcl_string const& cam_kml_file,
                       vcl_string const& label_xml_file,
                       volm_spherical_container_sptr const& sph,
                       volm_spherical_shell_container_sptr const& sph_shell,
                       bool const& use_default)
  : use_default_(use_default), sph_depth_(sph), sph_(sph_shell)
{
  query_points_ = sph_->cart_points();
  query_size_ = (unsigned)query_points_.size();

  // load the labelme xml to define depth_map_scene and associated default value of camera parameters
  dm_ = new depth_map_scene;
  volm_io::read_labelme(label_xml_file, dm_, img_category_);
  ni_ = dm_->ni();
  nj_ = dm_->nj();
  // set the default value based on img_category_ ( temporary having "desert" and "coast"
  if (img_category_ == "desert") {
    head_ = 0.0;      head_d_ = 180.0; head_inc_ = 2.0;
    tilt_ = 90.0;     tilt_d_ = 0.0; tilt_inc_ = 2.0;
    roll_ = 0.0;      roll_d_ = 0.0; roll_inc_ = 1.0;
    tfov_ = 5.0;      tfov_d_ = 5.0; tfov_inc_ = 2.0;
  }
  else if (img_category_ == "coast") {
    // temporary use desert default
    head_ = 0.0;      head_d_ = 180.0; head_inc_ = 2.0;
    tilt_ = 90.0;     tilt_d_ = 0.0; tilt_inc_ = 2.0;
    roll_ = 0.0;      roll_d_ = 1.0; roll_inc_ = 1.0;
    tfov_ = 5.0;      tfov_d_ = 30.0; tfov_inc_ = 2.0;
  }
  else {
    // undefined img category, use desert default
    head_ = 0.0;      head_d_ = 180.0; head_inc_ = 2.0;
    tilt_ = 90.0;     tilt_d_ = 20.0; tilt_inc_ = 2.0;
    roll_ = 0.0;      roll_d_ = 1.0; roll_inc_ = 1.0;
    tfov_ = 5.0;      tfov_d_ = 30.0;  tfov_inc_ = 2.0;
  }
  double lat, lon;
  // load the camera kml, fetch the camera value and deviation
  volm_io::read_camera(cam_kml_file, ni_, nj_, head_, head_d_, tilt_, tilt_d_, roll_, roll_d_, tfov_, tfov_d_, altitude_, lat, lon);

  // create camera hypotheses
  this->create_cameras();
  // generate polygon vector based on defined order
  this->generate_regions();
  // start to ingest query, with min_dist and order implemented
  this->query_ingest();
}

void volm_query::create_cameras()
{
  vcl_vector<double> top_fov_;
  vcl_vector<double> headings_;
  vcl_vector<double> tilts_;
  vcl_vector<double> rolls_;
  // set up the camera parameter arrays and construct vector of cameras
  if (use_default_)
  {
    if (img_category_ == "desert") {
      double stock[] = {3.0,  4.0, 5.0,
                        12.0, 17.0, 18.0, 19.0,
                        20.0, 24.0};
      if (ni_ >= nj_) {  // landscape
        top_fov_.insert(top_fov_.end(), stock, stock + 9);
      }
      else {             // protrait
        double dtor = vnl_math::pi_over_180;
        for (unsigned i = 0; i < 9; i++) {
          double tr = vcl_tan(stock[i]*dtor);
          double top = vcl_atan(nj_*tr/ni_)/dtor;
          top_fov_.push_back(top);
        }
      }
      vcl_cout << " NOTE: default top field of view is used:\n" << "\t[ ";
      for (unsigned i = 0; i < 9; i++)
        vcl_cout << top_fov_[i] << ' ';
      vcl_cout << ']' << vcl_endl;
    }
    else if (img_category_ == "coast") {
      double stock[] = {3.0, 4.0, 5.0, 9.0,
                        15.0, 16.0,
                        20.0, 21.0, 22.0, 23.0, 24.0};
      if (ni_ >= nj_) {   // landscape
        top_fov_.insert(top_fov_.end(), stock, stock + 11);
      }
      else {             // protrait
        double dtor = vnl_math::pi_over_180;
        for (unsigned i = 0; i < 11; i++) {
          double tr = vcl_tan(stock[i]*dtor);
          double top = vcl_atan(nj_*tr/ni_)/dtor;
          top_fov_.push_back(top);
        }
      }
      vcl_cout << " NOTE: default top field of view is used:\n" << "\t[ ";
      for (unsigned i = 0; i < 11; i++)
        vcl_cout << top_fov_[i] << ' ';
      vcl_cout << ']' << vcl_endl;
    }
	  else {
      double stock[] = {3.0,  4.0, 5.0,
                        12.0, 17.0, 18.0,19.0,
                       20.0, 24.0};
      top_fov_.insert(top_fov_.end(), stock, stock + 9);
      vcl_cout << " NOTE: default top field of view is used:\n" << "\t[ ";
      for (unsigned i = 0; i < 9; i++)
        vcl_cout << top_fov_[i] << ' ';
      vcl_cout << ']' << vcl_endl;
    }
  }
  else {
    if (tfov_ < 10)      tfov_inc_ = 1.0;
    else if (tfov_ > 20) tfov_inc_ = 4.0;
    else                 tfov_inc_ = 2.0;
    top_fov_.push_back(tfov_);    // top viewing ranges from 1 to 89
    for (double i = tfov_inc_; i <= tfov_d_; i+=tfov_inc_) {
      double right = tfov_ + i, left = tfov_ - i;
      if (right > 89)  right = 89;
      if (left  < 1)   left = 1;
      top_fov_.push_back(right);  top_fov_.push_back(left);
    }
  }

  headings_.push_back(head_);
  for (double i = head_inc_; i <= head_d_; i+=head_inc_) {   // heading ranges from 0 to 360
    double right = head_ + i, left = head_ - i;
    if (right > 360) right = right - 360;
    if (left < 0)    left = left + 360;
    headings_.push_back(right);  headings_.push_back(left);
  }

  tilts_.push_back(tilt_);   // tilt ranges from 0 to 180
  for (double i =  tilt_inc_; i <= tilt_d_; i+= tilt_inc_) {
    double right = tilt_ + i, left = tilt_ - i;
    if (right > 180) right = 180;
    if (left < 0)    left = 0;
    tilts_.push_back(right);  tilts_.push_back(left);
  }

  rolls_.push_back(roll_);    // roll ranges from -180 to 180 in kml, how about in camera_from_kml ?? (need to check...)
  for (double i = roll_inc_; i <= roll_d_; i+=roll_inc_) {
    double right = roll_ + i , left = roll_ - i;
    if (right > 180) right = right - 360;
    if (left < -180) left = left + 360;
    rolls_.push_back(roll_ + i);  rolls_.push_back(roll_ - i);
  }
  // construct cameras
  for (unsigned i = 0; i < tilts_.size(); ++i)
    for (unsigned j = 0; j < rolls_.size(); ++j)
      for (unsigned k = 0; k < top_fov_.size(); ++k)
        for (unsigned h = 0; h < headings_.size(); ++h) {
          double tilt = tilts_[i], roll = rolls_[j], top_f = top_fov_[k], head = headings_[h];
          double dtor = vnl_math::pi_over_180;
          double ttr = vcl_tan(top_f*dtor);
          double right_f = vcl_atan(ni_*ttr/nj_)/dtor;
          vpgl_perspective_camera<double> cam = bpgl_camera_utils::camera_from_kml((double)ni_, (double)nj_, right_f, top_f, altitude_, head, tilt, roll);
          // check whether current camera is consistent with defined ground plane
          bool success = true;
          if (dm_->ground_plane().size()) {
            for (unsigned i = 0; (success && i < dm_->ground_plane().size()); i++) {
              success = dm_->ground_plane()[i]->region_ground_2d_to_3d(cam);
              //vcl_cout << "checking ground plane consistency for: " << dm_->ground_plane()[i]->name() << " min depth is: " << dm_->ground_plane()[i]->min_depth();
              //success ? vcl_cout << " consistent!\n" : vcl_cout << " not_consistent!\n";
            }
            if (success) {
              cameras_.push_back(cam);
              camera_strings_.push_back(bpgl_camera_utils::get_string((double)ni_, (double)nj_, right_f, top_f, 0.0, head, tilt, roll));
            }
            else
            {
#ifdef DEBUG
              vcl_cout << "WARNING: following camera hypothesis is NOT consistent with defined ground plane in the query image and ignored\n"
                       << " \t heading = " << head << ", tilt = " << tilt << ", roll = " << roll << ", top_fov = " << top_f << ", right_fov = " << right_f << vcl_endl;
#endif
            }
          }
        }
}

void volm_query::generate_regions()
{
  // generate the map of the depth_map_region based on their order
  depth_regions_ = dm_->scene_regions();
  vcl_sort(depth_regions_.begin(), depth_regions_.end(), compare_order());
  unsigned size = (unsigned)dm_->scene_regions().size();
  d_threshold_ = 20000.0;
  for (unsigned i = 0; i < size; i++) {
    //depth_regions_[(dm_->scene_regions())[i]->order()] = (dm_->scene_regions())[i];
    if (d_threshold_ < (dm_->scene_regions())[i]->max_depth())
      d_threshold_ = (dm_->scene_regions())[i]->max_depth();
  }
  if (dm_->ground_plane().size()) {
    for (unsigned i = 0; i < dm_->ground_plane().size(); i++) {
      if (d_threshold_ < dm_->ground_plane()[i]->max_depth())
        d_threshold_ = dm_->ground_plane()[i]->max_depth();
    }
  }
}

bool volm_query::query_ingest()
{
  for (unsigned i = 0; i < cameras_.size(); i++) {
    vcl_vector<unsigned char> min_dist_layer;
    vcl_vector<unsigned char> max_dist_layer;
    vcl_vector<unsigned char> order_layer;
    vpgl_perspective_camera<double> cam = cameras_[i];
    // create an depth image for current camera
    dm_->set_camera(cam);
    vil_image_view<float> depth_img = dm_->depth_map("ground_plane", 0, d_threshold_);
    unsigned count = 0;
    for (unsigned p_idx = 0; p_idx < query_size_; p_idx++) {
      vgl_point_3d<double> qp(query_points_[p_idx].x(), query_points_[p_idx].y(), query_points_[p_idx].z()+altitude_);
      unsigned char min_dist, order, max_dist;
      // check whether the point is behind the camera
      if (cam.is_behind_camera(vgl_homg_point_3d<double>(qp))) {
        min_dist_layer.push_back((unsigned char)255);
        max_dist_layer.push_back((unsigned char)255);
        order_layer.push_back((unsigned char)255);
      }
      else {
        double u, v;
        cam.project(qp.x(), qp.y(), qp.z(), u, v);
        // compare (u, v) with depth_map_scene, return min_dist
        if ( u > (double)ni_ || v > (double)nj_ || u < TOL || v < TOL) {   // container point qp is outside camera viewing volume
          min_dist_layer.push_back((unsigned char)255);
          max_dist_layer.push_back((unsigned char)255);
          order_layer.push_back((unsigned char)255);
        }
        else {
          min_dist = this->fetch_depth(u, v, order, max_dist, depth_img);
          min_dist_layer.push_back(min_dist);
          max_dist_layer.push_back(max_dist);
          order_layer.push_back(order);
          if ((unsigned)min_dist != 255)
            count++;
        }
      }
    } // loop over rays for current camera
    min_dist_.push_back(min_dist_layer);
    max_dist_.push_back(max_dist_layer);
    order_.push_back(order_layer);
    ray_count_.push_back(count);
  } // loop over cameras
  return true;
}

unsigned char volm_query::fetch_depth(double const& u, double const& v, unsigned char& order, unsigned char& max_dist, vil_image_view<float> const& depth_img)
{
  unsigned char min_dist;
  // check other objects before ground,
  // e.g.,  for overlap region of a building and ground, building is on the ground and it is must closer than the ground
  if (depth_regions_.size()) {
    for (unsigned i = 0; i < depth_regions_.size(); i++) {
      vgl_polygon<double> poly = bsol_algs::vgl_from_poly(depth_regions_[i]->region_2d());
      if (poly.contains(u,v)) {
        double min_depth = depth_regions_[i]->min_depth();
        if (min_depth < sph_depth_->min_voxel_res())
          min_dist = (unsigned char)255;
        else
          min_dist = sph_depth_->get_depth_interval(min_depth);
        double max_depth = depth_regions_[i]->max_depth();
        if (max_depth < sph_depth_->min_voxel_res())
          max_dist = (unsigned char)255;
        else
          max_dist = sph_depth_->get_depth_interval(max_depth);
        order = (unsigned char)depth_regions_[i]->order();
        return min_dist;
      }
    }
  }
  // ground plane
  if (dm_->ground_plane().size()) {
    for (unsigned i = 0; i < dm_->ground_plane().size(); i++) {
      vgl_polygon<double> vgl_ground = bsol_algs::vgl_from_poly((dm_->ground_plane()[i])->region_2d());
      if (vgl_ground.contains(u,v)) {
        min_dist = sph_depth_->get_depth_interval(depth_img((int)u,(int)v));
        max_dist = (unsigned char)255;
        order = (unsigned char)(dm_->ground_plane()[i])->order();
        return min_dist;
      }
    }
  }
  // sky last since all objects should be closer than sky
  if (dm_->sky().size()) {
    for (unsigned i = 0; i < dm_->sky().size(); i++) {
      vgl_polygon<double> vgl_sky = bsol_algs::vgl_from_poly((dm_->sky()[i])->region_2d());
      if (vgl_sky.contains(u,v)) {
        max_dist = (unsigned char)254;
        order = (dm_->sky()[i])->order();
        return (unsigned char)254;
      }
    }
  }
  // the image points (u,v) is not inside any defined objectes
  max_dist = (unsigned char)255;
  order = (unsigned char)255;
  return (unsigned char)255;
}

void volm_query::draw_template(vcl_string const& vrml_fname)
{
  // write the header and shell container first
  sph_->draw_template(vrml_fname);
  // write rays
  this->draw_rays(vrml_fname);
  // write the camera
  unsigned cam_num = (unsigned)cameras_.size();
  for (unsigned i = 0; i < cam_num; i++) {
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    if (i%3 == 0)
      r = 1.0f;
    else if (i%3 == 1)
      g = 1.0f;
    else {
      r = 1.0f; g = 1.0f;
    }
    this->draw_viewing_volume(vrml_fname, cameras_[i], r, g, b);
  }
}

void volm_query::draw_rays(vcl_string const& fname)
{
  vcl_ofstream ofs(fname.c_str(), vcl_ios::app);
  double len = 400.0;
  vgl_point_3d<double> ori(0.0,0.0,0.0);
  for (unsigned i=0; i<query_size_; i++) {
    vgl_ray_3d<double> ray(ori, query_points_[i]);
    bvrml_write::write_vrml_cylinder(ofs, ori, ray.direction(), (float)3.0, (float)len, 0.0f, 0.0f, 0.0f, 1);
  }
}

void volm_query::draw_viewing_volume(vcl_string const& fname, vpgl_perspective_camera<double> cam, float r, float g, float b)
{
  vcl_ofstream ofs(fname.c_str(), vcl_ios::app);
  // reset the center back to zero
  cam.set_camera_center(vgl_point_3d<double>(0.0,0.0,0.0) );
  //bvrml_write::write_vrml_cylinder(ofs, cam.get_camera_center(), cam.principal_axis(),0.2f, (double)(conf_focal_ + init_focal_),r,g,b);

  // create the viewing volume by rays
  vgl_ray_3d<double> rtl = cam.backproject_ray(vgl_point_2d<double>(0.0, 0.0));
  vgl_ray_3d<double> rtr = cam.backproject_ray(vgl_point_2d<double>((double)ni_, 0.0));
  vgl_ray_3d<double> rll = cam.backproject_ray(vgl_point_2d<double>(0.0, (double)nj_));
  vgl_ray_3d<double> rlr = cam.backproject_ray(vgl_point_2d<double>((double)ni_, (double)nj_));
  // calculate a scaling factor
  double scale = 0.5;
  double focal = (cam.get_calibration()).focal_length();
  double depth = focal * scale;
  double dist = depth / focal * 0.5 * vcl_sqrt(4*focal*focal + ni_*ni_ + nj_*nj_);
  // get image corner point
  vgl_point_3d<double> ptl = cam.get_camera_center() + dist*rtl.direction();
  vgl_point_3d<double> ptr = cam.get_camera_center() + dist*rtr.direction();
  vgl_point_3d<double> pll = cam.get_camera_center() + dist*rll.direction();
  vgl_point_3d<double> plr = cam.get_camera_center() + dist*rlr.direction();
  // draw the boundary face
  ofs << "Shape {\n"
      << " appearance Appearance {\n"
      << "   material Material\n"
      << "    {\n"
      << "      diffuseColor " << r << ' ' << g << ' ' << b << '\n'
      << "      transparency " << 0 << '\n'
      << "    }\n"
      << "  }\n"
      << " geometry IndexedFaceSet {\n"
      << "   coord Coordinate {\n"
      << "      point [\n"
      << "        " << cam.get_camera_center().x() << ' '
      << "        " << cam.get_camera_center().y() << ' '
      << "        " << cam.get_camera_center().z() << ",\n"
      << "        " << ptl.x() << ' ' << ptl.y() << ' ' << ptl.z() << ",\n"
      << "        " << ptr.x() << ' ' << ptr.y() << ' ' << ptr.z() << ",\n"
      << "        " << pll.x() << ' ' << pll.y() << ' ' << pll.z() << ",\n"
      << "        " << plr.x() << ' ' << plr.y() << ' ' << plr.z() << '\n'
      << "      ]\n"
      << "    }\n"
      << "    coordIndex [\n"
      << "  0, 1, 2, -1,\n"
      << "  0, 2, 4, -1,\n"
      << "  0, 3, 4, -1,\n"
      << "  0, 1, 3, -1,\n"
      << "    ]\n"
      << "  }\n"
      << "}\n";
  // draw image boundary
  vgl_line_segment_3d<double> top(ptl,ptr);
  vgl_line_segment_3d<double> left(ptl,pll);
  vgl_line_segment_3d<double> right(plr,ptr);
  vgl_line_segment_3d<double> bottom(plr,pll);
  ofs.close();
}

void volm_query::draw_polygon(vil_image_view<vil_rgb<vxl_byte> >& img, vgl_polygon<double> const& poly, unsigned char const& depth)
{
  for (unsigned pi = 0; pi < poly.num_sheets(); pi++) {
    for (unsigned vi = 0; vi < poly[pi].size(); vi++) {
      vgl_point_2d<double> s = poly[pi][vi];
      vgl_point_2d<double> e;
      if (vi < poly[pi].size()-1)  e = poly[pi][vi+1];
      else  e = poly[pi][0];
      double k;
      if (e.x() == s.x()) k = 10000;
      else k = (e.y()-s.y())/(e.x()-s.x());
      double b = s.y() - k * s.x();
      if (vcl_sqrt(k*k) < 1) {// loop x
        if (s.x() <= e.x()) {
          for (unsigned xi = (unsigned)s.x(); xi <= (unsigned)e.x(); xi++) {
            unsigned xj = (unsigned)(k*xi+b);
            if (  !(xi < 0 || xj < 0 || xi >= ni_ || xj >= nj_) ) {
              img(xi,xj).r = bvrml_color::heatmap_classic[(int)depth][0];
              img(xi,xj).g = bvrml_color::heatmap_classic[(int)depth][1];
              img(xi,xj).b = bvrml_color::heatmap_classic[(int)depth][2];
            }
          }
        }
        else {
          for (unsigned xi = (unsigned)e.x(); xi <= (unsigned)s.x(); xi++) {
            unsigned xj = (unsigned)(k*xi+b);
            if (  !(xi < 0 || xj < 0 || xi >= ni_ || xj >= nj_) ) {
              img(xi,xj).r = bvrml_color::heatmap_classic[(int)depth][0];
              img(xi,xj).g = bvrml_color::heatmap_classic[(int)depth][1];
              img(xi,xj).b = bvrml_color::heatmap_classic[(int)depth][2];
            }
          }
        }
      }
      else {
        if (s.y() <= e.y()) {
          for (unsigned xj = (unsigned)s.y(); xj <= (unsigned)e.y(); xj++) {
            unsigned xi = (unsigned)((xj-b)/k);
            if (  !(xi < 0 || xj < 0 || xi >= ni_ || xj >= nj_) ) {
              img(xi,xj).r = bvrml_color::heatmap_classic[(int)depth][0];
              img(xi,xj).g = bvrml_color::heatmap_classic[(int)depth][1];
              img(xi,xj).b = bvrml_color::heatmap_classic[(int)depth][2];
            }
          }
        }
        else {
          for (unsigned xj = (unsigned)e.y(); xj <= (unsigned)s.y(); xj++) {
            unsigned xi = (unsigned)((xj-b)/k);
            if (  !(xi < 0 || xj < 0 || xi >= ni_ || xj >= nj_) ) {
              img(xi,xj).r = bvrml_color::heatmap_classic[(int)depth][0];
              img(xi,xj).g = bvrml_color::heatmap_classic[(int)depth][1];
              img(xi,xj).b = bvrml_color::heatmap_classic[(int)depth][2];
            }
          }
        }
      }
    }
  }
}

void volm_query::draw_dot(vil_image_view<vil_rgb<vxl_byte> >& img,
                          vgl_point_3d<double> const& world_point,
                          unsigned char const& depth,
                          vpgl_perspective_camera<double> const& cam)
{
  int dot_size = ( img.ni() < img.nj() ) ? (int)(0.007*ni_) : (int)(0.007*nj_);
  double u, v;
  cam.project(world_point.x(), world_point.y(), world_point.z()+altitude_, u, v);
  int cx = (int)u;
  int cy = (int)v;
  for (int i = -dot_size; i < dot_size; i++)
    for (int j = -dot_size; j < dot_size; j++) {
      int x = cx + i ; int y = cy + j;
      if ( !(x < 0 || y < 0 || x >= (int)img.ni() || y >= (int)img.ni()) ) {
        if (depth == 254) { // special color for sky
          img((unsigned)x,(unsigned)y).r = 255;
          img((unsigned)x,(unsigned)y).g = 255;
          img((unsigned)x,(unsigned)y).b = 255;
        } else {
          img((unsigned)x,(unsigned)y).r = bvrml_color::heatmap_classic[(int)depth][0];
          img((unsigned)x,(unsigned)y).g = bvrml_color::heatmap_classic[(int)depth][1];
          img((unsigned)x,(unsigned)y).b = bvrml_color::heatmap_classic[(int)depth][2];
        }
      }
    }
}

void volm_query::depth_rgb_image(vcl_vector<unsigned char> const& values,
                                 unsigned const& cam_id,
                                 vil_image_view<vil_rgb<vxl_byte> >& out_img)
{
  // draw depth_map polygons on the depth images
  if (dm_->sky().size()) {
    for (unsigned i = 0; i < dm_->sky().size(); i++) {
      vgl_polygon<double> poly = bsol_algs::vgl_from_poly((dm_->sky()[i])->region_2d());
      this->draw_polygon(out_img, poly, (unsigned char)254);
    }
  }
  if (dm_->ground_plane().size()) {
    unsigned ground_size = (unsigned)dm_->ground_plane().size();
    for (unsigned i = 0; i < ground_size; i++) {
      vgl_polygon<double> poly = bsol_algs::vgl_from_poly((dm_->ground_plane()[i])->region_2d());
      this->draw_polygon(out_img, poly, (unsigned char)1);
    }
  }
  if (dm_->scene_regions().size()) {
    unsigned region_size = (unsigned)dm_->scene_regions().size();
    for ( unsigned i = 0; i < region_size; i++) {
      vgl_polygon<double> poly = bsol_algs::vgl_from_poly((dm_->scene_regions())[i]->region_2d());
      double value = (dm_->scene_regions())[i]->min_depth();
      unsigned char depth = sph_depth_->get_depth_interval(value);
      this->draw_polygon(out_img, poly, depth);
    }
  }
  // draw the rays that current penetrate through the image
  for (unsigned pidx = 0; pidx < query_size_; pidx++) {
    if (values[pidx] < 255) 
      this->draw_dot(out_img, query_points_[pidx], values[pidx], cameras_[cam_id]);
   }
}

void volm_query::draw_query_image(unsigned cam_i, vcl_string const& out_name)
{
  // create an rgb image instance
  vil_image_view<vil_rgb<vxl_byte> > img(ni_, nj_);
  // initialize the image
  for (unsigned i = 0; i < ni_; i++)
    for (unsigned j = 0; j < nj_; j++) {
      img(i,j).r = (unsigned char)120;
      img(i,j).g = (unsigned char)120;
      img(i,j).b = (unsigned char)120;
    }
  // visualize the depth rgb image
  vcl_vector<unsigned char> current_query = min_dist_[cam_i];
  this->depth_rgb_image(current_query, cam_i, img);
  // save the images

  vil_save(img,out_name.c_str());
}

void volm_query::draw_query_images(vcl_string const& out_dir)
{
  // create a png img associated with each camera hypothesis, containing the geometry defined
  //  in depth_map_scene and the img points corresponding to points inside the container
  // loop over fist 100 camera
  unsigned img_num = (cameras_.size() > 100) ? 100 : (unsigned)cameras_.size();
  for (unsigned i = 0; i < img_num; i++) {
    vcl_stringstream s_idx;
    s_idx << i;
    vcl_string fs = out_dir + "query_img_" + s_idx.str() + ".png";
    this->draw_query_image(i, fs);
  }
}

void volm_query::visualize_query(vcl_string const& prefix)
{
  // visualize the spherical shell by the query depth value -- used to compare with the generated index spherical shell
  // loop over all camera
  for (unsigned i = 0; i < cameras_.size(); i++) {
    vcl_vector<unsigned char> single_layer = min_dist_[i];
    vcl_stringstream str;
    str << prefix << "_query_" << i << ".vrml";
    sph_->draw_template(str.str(), single_layer, 254);
  }
}
