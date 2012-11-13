#include "volm_query.h"
//:
// \file
#include <bpgl/bpgl_camera_utils.h>
#include <volm/volm_spherical_container.h>
#include <bsol/bsol_algs.h>
#include <vil/vil_save.h>
#include <volm/volm_io.h>

#define TOL -1E-8

volm_query::volm_query(vcl_string const& cam_kml_file, 
             vcl_string const& label_xml_file, 
             volm_spherical_container_sptr const& sph,
             volm_spherical_shell_container_sptr const& sph_shell)
  : sph_depth_(sph), sph_(sph_shell)
{
  query_points_ = sph_->cart_points();
  query_size_ = (unsigned)query_points_.size();

  // load the labelme xml to define depth_map_scene and associated default value of camera parameters
  dm_ = new depth_map_scene;
  volm_io::read_labelme(label_xml_file, dm_, img_category_);
  ni_ = dm_->ni();
  nj_ = dm_->nj();
  // set the default value based on img_category_ ( temporary having "desert" and "coast"
  if(img_category_ == "desert") {
    head_ = 0.0;      head_d_ = 180.0;
    tilt_ = 90.0;     tilt_d_ = 0.0;
    roll_ = 0.0;      roll_d_ = 0.0;
    tfov_ = 5.0;      tfov_d_ = 30.0;
  }else if(img_category_ == "coast") {
    // temporary use desert default
    head_ = 0.0;      head_d_ = 180.0;
    tilt_ = 90.0;     tilt_d_ = 20.0;
    roll_ = 0.0;      roll_d_ = 0.0;
    tfov_ = 5.0;      tfov_d_ = 30.0;
  }else{
    // undefined img category, use desert default
    head_ = 0.0;      head_d_ = 180.0;
    tilt_ = 90.0;     tilt_d_ = 20.0;
    roll_ = 0.0;      roll_d_ = 0.0;
    tfov_ = 5.0;      tfov_d_ = 30.0;
  }

  // load the camera kml, fetch the camera value and deviation
  volm_io::read_camera(cam_kml_file, ni_, nj_, head_, head_d_, tilt_, tilt_d_, roll_, roll_d_, tfov_, tfov_d_);
  
  // create camera hypotheses
  this->create_cameras();
  // generate polygon vector based on defined order
  this->generate_depth_poly_order();
  // start to ingest query, with min_dist and order implemented
  this->query_ingest();
  
}

void volm_query::create_cameras()
{
  // set up the camera parameter arrays and constuct vector of cameras
  top_fov_.push_back(tfov_); // top viewing ranges from 1 to 89
  for(unsigned i = 1; i <= tfov_d_; i++) {
    double right = tfov_ + i, left = tfov_ - i;
    if(right > 89)  right = 89;
    if(left  < 1)   left = 1;
    top_fov_.push_back(right);  top_fov_.push_back(left);
  }

  headings_.push_back(head_);
  for(unsigned i = 1; i <= head_d_; i++) {   // heading ranges from 0 to 360
    double right = head_ + i, left = head_ - i;
    if(right > 360) right = right - 360;
    if(left < 0)    left = left + 360;
    headings_.push_back(right);  headings_.push_back(left);
  }

  tilts_.push_back(tilt_);  // tilt ranges from 0 to 180
  for(unsigned i = 1; i <= tilt_d_; i++) {
    double right = tilt_ + i, left = tilt_ - i;
    if(right > 180) right = 180;
    if(left < 0)    left = 0;
    tilts_.push_back(right);  tilts_.push_back(left);
  }

  rolls_.push_back(roll_); // roll ranges from -180 to 180 in kml, how about in camera_from_kml ?? (need to check...)
  for(unsigned i = 1; i <= roll_d_; i++) {
    double right = roll_ + i , left = roll_ - i;
    if(right > 180) right = right - 360;
    if(left < -180) left = left + 360;
    rolls_.push_back(roll_ + i);  rolls_.push_back(roll_ - i);
  }
  // construct cameras
  for(unsigned i = 0; i < tilts_.size(); i++)
    for(unsigned j = 0; j < rolls_.size(); j++)
      for(unsigned k = 0; k < top_fov_.size(); k++)
        for(unsigned h = 0; h < headings_.size(); h++){
          double tilt = tilts_[i], roll = rolls_[j], top_f = top_fov_[k], head = headings_[h];
          double dtor = vnl_math::pi_over_180;
          double ttr = vcl_tan(top_f*dtor);
          double right_f = vcl_atan(ni_*ttr/nj_)/dtor;
          vpgl_perspective_camera<double> cam = bpgl_camera_utils::camera_from_kml((double)ni_, (double)nj_, right_f, top_f, 0.0, head, tilt, roll);
          cameras_.push_back(cam);
        }
}

void volm_query::generate_depth_poly_order()
{
  // generate the vector of 2d polygons for fetch minimum depth value, based on the depth_map_scene orders
  unsigned size = (unsigned)dm_->scene_regions().size();
  unsigned last = 0;
  for(unsigned i = 0; i < size; i++)
    for(unsigned j = 0; j < size; j++){
      if(dm_->scene_regions()[j]->order() == last){
        vgl_polygon<double> poly = bsol_algs::vgl_from_poly((dm_->scene_regions())[j]->region_2d());
        dm_poly_.push_back(poly);
        min_depths_.push_back((dm_->scene_regions())[j]->min_depth());
        max_depths_.push_back((dm_->scene_regions())[j]->max_depth());
        order_depths_.push_back((dm_->scene_regions())[j]->order());
        last++;
        break;
      }
    }
}

bool volm_query::query_ingest()
{
  for(unsigned i = 0; i < cameras_.size(); i++) {
    vcl_vector<unsigned char> min_dist_layer;
    vcl_vector<unsigned char> max_dist_layer;
    vcl_vector<unsigned char> order_layer;
    vpgl_perspective_camera<double> cam = cameras_[i];
    unsigned count = 0;
    for(unsigned p_idx = 0; p_idx < query_size_; p_idx++) {
      vgl_point_3d<double> qp = query_points_[p_idx];
      unsigned char min_dist, order, max_dist;
      // check whether the point is behind the camera
      if(cam.is_behind_camera(vgl_homg_point_3d<double>(qp))) {
        min_dist_layer.push_back((unsigned char)255);
        max_dist_layer.push_back((unsigned char)255);
        order_layer.push_back((unsigned char)255);
      }else{
        double u, v;
        cam.project(qp.x(), qp.y(), qp.z(), u, v);
        // compare (u, v) with depth_map_scene, return min_dist
        if( u > (double)ni_ || v > (double)nj_ || u < TOL || v < TOL) {   // container point qp is outside camera viewing volume
          min_dist_layer.push_back((unsigned char)255);
          max_dist_layer.push_back((unsigned char)255);
          order_layer.push_back((unsigned char)255);
        }else{
          min_dist = this->fetch_depth(u, v, order, max_dist);
          min_dist_layer.push_back(min_dist);
          max_dist_layer.push_back(max_dist);
          order_layer.push_back(order);
          if((unsigned)min_dist != 255)
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

#if 0
  double ppu = ni_/2, ppv = nj_/2;
  double rtod = vnl_math::deg_per_rad;
  // loop over all camera parameters
  for(unsigned i = 0; i < (Nf_+1); i++)
    for(unsigned j = 0; j < (Nh_+1); j++)
      for(unsigned k = 0; k < (Nt_+1); k++)
        for(unsigned l = 0; l < (Nr_+1); l++) {
          double focal = focals_[i];
          double heading = headings_[j];
          double tilt = tilts_[k];
          double roll = rolls_[l];
          double right_fov = vcl_atan(ppu/focal)*rtod;
          double top_fov = vcl_atan(ppv/focal)*rtod;
          
          vcl_vector<unsigned char> single_layer;
          // create camera hypothesis
          vpgl_perspective_camera<double> cam = bpgl_camera_utils::camera_from_kml((double)ni_, (double)nj_, right_fov, top_fov, 0.0, heading, tilt, roll);
          cameras_.push_back(cam);
          // ingetst depth_map scene to query_
          for(unsigned p_idx = 0; p_idx < query_size_; p_idx++) {
            vgl_point_3d<double> qp = query_points_[p_idx];
            // check whether the point is behind the camera
            if(cam.is_behind_camera(vgl_homg_point_3d<double>(qp))) {
              single_layer.push_back((unsigned char)255);
            }else{
              double u, v;
              cam.project(qp.x(), qp.y(), qp.z(), u, v);
              // compare (u, v) with depth_map_scene, return min_dist
              if( u > (double)ni_ || v > (double)nj_ || u < TOL || v < TOL)   // container point qp is outside camera viewing volume
                single_layer.push_back((unsigned char)255);
              else
                single_layer.push_back(this->fetch_depth(u,v));
            }
          }
          query_.push_back(single_layer);
        }
#endif
}

unsigned char volm_query::fetch_depth(double const& u, double const& v, unsigned char& order, unsigned char& max_dist)
{
  unsigned min_dist;
  // sky
  if(dm_->sky()){
    vgl_polygon<double> vgl_sky = bsol_algs::vgl_from_poly((dm_->sky())->region_2d());
    if(vgl_sky.contains(u,v)){
      max_dist = (unsigned char)254;
      order = (dm_->sky())->order();
      return (unsigned char)254;
    }
  }
  // other objects
  // use the order polygon vector, therefore the min_dist in query would be the min_dist of the smallest order object
  if(dm_poly_.size()) {
    for(unsigned i = 0 ; i < dm_poly_.size(); i++) {
      if(dm_poly_[i].contains(u,v)){
        double min_depth = min_depths_[i];
        if(min_depth == 0){
          min_dist = (unsigned char)255;
        }else{
          min_dist = sph_depth_->get_depth_interval(min_depth);
        }
        double max_depth = max_depths_[i];
        if(max_depth == 0){
          max_dist = (unsigned char)255;
        }else{
          max_dist = sph_depth_->get_depth_interval(max_depth);
        }
        order = (unsigned char)order_depths_[i];
        return min_dist;
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
  for(unsigned i = 0; i < cam_num; i++) {
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    if(i%3 == 0)
      r = 1.0f;
    else if(i%3 == 1)
      g = 1.0f;
    else{
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
  for(unsigned i=0; i<query_size_; i++){
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
//  if(conf_focal_)  scale = conf_focal_ / init_focal_;
// else  scale = 0.5;

  double focal = (cam.get_calibration()).focal_length();
  double depth = focal * scale;
  double dist = depth / focal * 0.5 * vcl_sqrt(4*focal*focal + ni_*ni_ + nj_*nj_);
//  bvrml_write::write_vrml_line(ofs, cam.get_camera_center(), cam.principal_axis(), (float)((conf_focal_ + init_focal_)*scale), 0.0f, 0.0f, 0.0f);
  // get image corner point
  vgl_point_3d<double> ptl = cam.get_camera_center() + dist*rtl.direction();
  vgl_point_3d<double> ptr = cam.get_camera_center() + dist*rtr.direction();
  vgl_point_3d<double> pll = cam.get_camera_center() + dist*rll.direction();
  vgl_point_3d<double> plr = cam.get_camera_center() + dist*rlr.direction();
  // draw the boundary face
  ofs << "Shape {\n"
      << " appearance Appearance{\n"
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
      << "        " << plr.x() << ' ' << plr.y() << ' ' << plr.z() << "\n"
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

void volm_query::draw_polygon(vil_image_view<vxl_byte>& img, vgl_polygon<double> const& poly)
{
  for(unsigned pi = 0; pi < poly.num_sheets(); pi++) {
    for(unsigned vi = 0; vi < poly[pi].size(); vi++) {
      vgl_point_2d<double> s = poly[pi][vi];
      vgl_point_2d<double> e;
      if(vi < poly[pi].size()-1)  e = poly[pi][vi+1];
      else  e = poly[pi][0];
      double k;
      if(e.x() == s.x()) k = 10000;
      else k = (e.y()-s.y())/(e.x()-s.x());
      double b = s.y() - k * s.x();
      if(vcl_sqrt(k*k) < 1) {// loop x
        if(s.x() <= e.x()){
          for(unsigned xi = (unsigned)s.x(); xi <= (unsigned)e.x(); xi++){
            unsigned xj = (unsigned)(k*xi+b);  
            if(  !(xi < 0 || xj < 0 || xi >= ni_ || xj >= nj_) )
              img(xi,xj) = 0;
          }
        }else{
          for(unsigned xi = (unsigned)e.x(); xi <= (unsigned)s.x(); xi++){
            unsigned xj = (unsigned)(k*xi+b);
            if(  !(xi < 0 || xj < 0 || xi >= ni_ || xj >= nj_) )
              img(xi,xj) = 0;
          }
        }
      }else{
        if(s.y() <= e.y()){
          for(unsigned xj = (unsigned)s.y(); xj <= (unsigned)e.y(); xj++){
            unsigned xi = (unsigned)((xj-b)/k);
            if(  !(xi < 0 || xj < 0 || xi >= ni_ || xj >= nj_) )
              img(xi,xj) = 0;
          }
        }else{
          for(unsigned xj = (unsigned)e.y(); xj <= (unsigned)s.y(); xj++){
            unsigned xi = (unsigned)((xj-b)/k);
            if(  !(xi < 0 || xj < 0 || xi >= ni_ || xj >= nj_) )
              img(xi,xj) = 0;
          }
        }
      }  
    }
  }
}

void volm_query::draw_dot(vil_image_view<vxl_byte>& img, vgl_point_3d<double> const& world_point, unsigned char const& depth, vpgl_perspective_camera<double> const& cam)
{
  double u, v;
  cam.project(world_point.x(), world_point.y(), world_point.z(), u, v);
  int cx = (int)u;
  int cy = (int)v;
  for(int i = -5; i < 6; i++)
    for(int j = -5; j < 6; j++){
      int x = cx + i ; int y = cy + j;
      if( !(x < 0 || y < 0 || x >= (int)ni_ || y >= (int)nj_) ){
        int grey = (int)(depth*20);
        if(grey > 255) grey = 255;
        img((unsigned)x,(unsigned)y) = (int)(depth*20);
      }
    }
}

void volm_query::draw_query_images(vcl_string const& out_dir)
{
  // create a png img associated with each camera hypothesis, containing the geometry defined 
  //  in depth_map_scene and the img points corresponding to points inside the container
  // loop over all camera
  for(unsigned i = 0; i < cameras_.size(); i++) {
    // create a vil_image_view<vxl_byte> for png query_img geometry
    vil_image_view<vxl_byte> img(ni_, nj_);
    img.fill(150);
    // draw the polygon
    if(dm_->sky()) {
      vgl_polygon<double> poly = bsol_algs::vgl_from_poly((dm_->sky())->region_2d());
      this->draw_polygon(img, poly);  
    }
    if(dm_->scene_regions().size()) {
      for(unsigned obj_i = 0; obj_i < dm_->scene_regions().size(); obj_i++) {
        vgl_polygon<double> poly = bsol_algs::vgl_from_poly((dm_->scene_regions())[obj_i]->region_2d());
        this->draw_polygon(img,poly);
      }
    }
    // draw the rays which penetrate through the depth_map_scene
    vcl_vector<unsigned char> single_layer = min_dist_[i];
    //vcl_vector<unsigned char> single_layer = order_[i];
    for(unsigned pidx = 0; pidx < query_size_; pidx++)
      if(single_layer[pidx] < 255){
        this->draw_dot(img, query_points_[pidx], single_layer[pidx], cameras_[i]);
      }
    // write into img
    vcl_stringstream s_idx;
    s_idx << i;
    vcl_string fs = out_dir + "query_img_" + s_idx.str() + ".png";
    char * fname = new char[fs.size() + 1];
    fname[fs.size()] = 0;
    vcl_memcpy(fname, fs.c_str(),fs.size());
    vil_save(img,fname);
    delete []fname;
  }
}

void volm_query::visualize_query(vcl_string const& prefix)
{
  // visualize the spherical shell by the query depth value -- used to compare with the generated index spherical shell
  // loop over all camera
  for(unsigned i = 0; i < cameras_.size(); i++) {
    vcl_vector<unsigned char> single_layer = min_dist_[i];
    vcl_stringstream str; 
    str << prefix << "_query_" << i << ".vrml";
    sph_->draw_template(str.str(), single_layer, 254);
  }
}