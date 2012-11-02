#include "volm_query.h"
//:
// \file
#include <bpgl/bpgl_camera_utils.h>
#include <volm/volm_spherical_container.h>
#include <volm/volm_spherical_shell_container.h>
#include <bpgl/depth_map/depth_map_scene.h>
#include <bpgl/depth_map/depth_map_region.h>
#include <bsol/bsol_algs.h>
#include <vil/vil_save.h>

#define TOL -1E-8

volm_query::volm_query(depth_map_scene_sptr dm,
                       volm_spherical_container_sptr sph,
			           volm_spherical_shell_container_sptr container,
			           unsigned ni,         unsigned nj,
			           double init_focal,   double conf_focal,
			           double init_heading, double conf_heading,
			           double init_tilt,    double conf_tilt,
			           double init_roll,    double conf_roll,
					   double altitude)
{
  init_focal_ = init_focal;
  conf_focal_ = conf_focal;
  ni_ = ni;
  nj_ = nj;
  altitude_ = altitude;
  // construct camera hypothesis arrays given camera input (better be even number)
  focals_.push_back(init_focal);
  Nf_ = 0;
  if(conf_focal){
	Nf_ = 2;
    double d_focal = 2.0*conf_focal/Nf_;
	for(unsigned i = 1; i < Nf_; i++){
      focals_.push_back(init_focal + i*d_focal);  focals_.push_back(init_focal - i*d_focal);
	}
  }
  headings_.push_back(init_heading);
  Nh_ = 0;
  if(conf_heading){
    Nh_ = 2;
	double d_head = 2.0*conf_heading/Nh_;
	for(unsigned i = 1; i < Nh_; i++){
      double fowd = init_heading + i*d_head;
	  double bkwd = init_heading - i*d_head;
	  if(fowd > 360.0) fowd -= 360.0;
	  if(bkwd < 0.0)   bkwd += 360.0;
      headings_.push_back(fowd);  headings_.push_back(bkwd);
	}
  }
  tilts_.push_back(init_tilt);
  Nt_ = 0;
  if(conf_tilt){
    Nt_ = 2;
	double d_tilt = 2.0*conf_tilt/Nt_;
	for(unsigned i = 1; i < Nt_; i++) {
      double fowd = init_tilt + i*d_tilt;
	  double bkwd = init_tilt - i*d_tilt;
	  if(fowd > 360.0) fowd -= 360.0;
	  if(bkwd < 0.0)   bkwd += 360.0;
      tilts_.push_back(fowd);  tilts_.push_back(fowd);
	}
  }
  rolls_.push_back(init_roll);
  Nr_ = 0;
  if(conf_roll){
    Nr_ = 2;
	double d_roll = 2.0*conf_roll/Nr_;
	for(unsigned i = 1; i < Nr_; i++) {
      double fowd = init_roll + i*d_roll;
	  double bkwd = init_roll - i*d_roll;
	  if(fowd > 360.0) fowd -= 360.0;
	  if(bkwd < 0.0)   bkwd += 360.0;
      rolls_.push_back(fowd);  rolls_.push_back(bkwd);
	}
  }
  query_points_ = container->cart_points();
  query_size_ = (unsigned)query_points_.size();

  this->query_ingest(dm, sph);
}

bool volm_query::query_ingest(depth_map_scene_sptr const& dm, volm_spherical_container_sptr const& sph)
{
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
		  //: create camera hypothesis
	      vpgl_perspective_camera<double> cam = bpgl_camera_utils::camera_from_kml((double)ni_, (double)nj_, right_fov, top_fov, 0.0, heading, tilt, roll);
		  cameras_.push_back(cam);
		  //: ingetst depth_map scene to query_
          for(unsigned p_idx = 0; p_idx < query_size_; p_idx++) {
            vgl_point_3d<double> qp = query_points_[p_idx];
			//: check whether the point is behind the camera
			if(cam.is_behind_camera(vgl_homg_point_3d<double>(qp))) {
			  single_layer.push_back((unsigned char)255);
			}else{
              double u, v;
              cam.project(qp.x(), qp.y(), qp.z(), u, v);
			  //: compare (u, v) with depth_map_scene, return min_dist
			  if( u > (double)ni_ || v > (double)nj_ || u < TOL || v < TOL)   // container point qp is outside camera viewing volume
				single_layer.push_back((unsigned char)255);
			  else
                single_layer.push_back(this->fetch_depth(u,v,dm,sph));
			}
		  }
		  query_.push_back(single_layer);
		}
  return true;
}

unsigned char volm_query::fetch_depth(double const& u, double const& v, depth_map_scene_sptr const& dm, volm_spherical_container_sptr const& sph)
{
  
  //: sky
  if(dm->sky()){
	vgl_polygon<double> vgl_sky = bsol_algs::vgl_from_poly((dm->sky())->region_2d());
	if(vgl_sky.contains(u,v)){
	  return (unsigned char)254;
	}
  }
  //: other objects
  if(dm->scene_regions().size()) {
    for(unsigned i = 0; i < dm->scene_regions().size(); i++) {
      vgl_polygon<double> vgl_poly = bsol_algs::vgl_from_poly((dm->scene_regions())[i]->region_2d());
	  if(vgl_poly.contains(u,v)){
		double min_depth = (dm->scene_regions())[i]->min_depth();
		return sph->get_depth_interval(min_depth);
	  }
	}
  } 
  //: the image point (u,v) isn't inside any predefined depth region
  return (unsigned char)255;
}

void volm_query::draw_template(vcl_string const& vrml_fname, volm_spherical_shell_container_sptr const& sph_shell, depth_map_scene_sptr const& dm)
{
  //: write the header and shell container first
  sph_shell->draw_template(vrml_fname);
  //: write rays
  this->draw_rays(vrml_fname);
  //: write the camera
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
	this->draw_viewing_volume(vrml_fname, dm, cameras_[i], r, g, b);
  }
}

void volm_query::draw_rays(vcl_string const& fname)
{
  vcl_ofstream ofs(fname.c_str(), vcl_ios::app);
  double len = 200.0;
  vgl_point_3d<double> ori(0.0,0.0,0.0);
  for(unsigned i=0; i<query_size_; i++){
    vgl_ray_3d<double> ray(ori, query_points_[i]);
	bvrml_write::write_vrml_cylinder(ofs, ori, ray.direction(), (float)0.4, (float)len, 0.0f, 0.0f, 0.0f, 1);
  }  
}

void volm_query::draw_viewing_volume(vcl_string const& fname, depth_map_scene_sptr const& dm, vpgl_perspective_camera<double> cam, float r, float g, float b)
{
  vcl_ofstream ofs(fname.c_str(), vcl_ios::app);
  //: reset the center back to zero
  cam.set_camera_center(vgl_point_3d<double>(0.0,0.0,0.0) );
  //bvrml_write::write_vrml_cylinder(ofs, cam.get_camera_center(), cam.principal_axis(),0.2f, (double)(conf_focal_ + init_focal_),r,g,b);
  
  //: create the viewing volume by rays
  vgl_ray_3d<double> rtl = cam.backproject_ray(vgl_point_2d<double>(0.0, 0.0));
  vgl_ray_3d<double> rtr = cam.backproject_ray(vgl_point_2d<double>((double)ni_, 0.0));
  vgl_ray_3d<double> rll = cam.backproject_ray(vgl_point_2d<double>(0.0, (double)nj_));
  vgl_ray_3d<double> rlr = cam.backproject_ray(vgl_point_2d<double>((double)ni_, (double)nj_));
  //: calculate a scaling factor
  double scale;
  if(conf_focal_)  scale = conf_focal_ / init_focal_;
  else  scale = 0.05;

  double focal = (cam.get_calibration()).focal_length();
  double depth = focal * scale;
  double dist = depth / focal * 0.5 * vcl_sqrt(4*focal*focal + ni_*ni_ + nj_*nj_);
  bvrml_write::write_vrml_line(ofs, cam.get_camera_center(), cam.principal_axis(), (float)((conf_focal_ + init_focal_)*scale), 0.0f, 0.0f, 0.0f);
  //: get image corner point
  vgl_point_3d<double> ptl = cam.get_camera_center() + dist*rtl.direction();
  vgl_point_3d<double> ptr = cam.get_camera_center() + dist*rtr.direction();
  vgl_point_3d<double> pll = cam.get_camera_center() + dist*rll.direction();
  vgl_point_3d<double> plr = cam.get_camera_center() + dist*rlr.direction();
  //: draw the boundary face
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
  //: draw image boundary
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
            unsigned xj = (unsigned)(k*xi+b);  img(xi,xj) = 0;
          }
        }else{
          for(unsigned xi = (unsigned)e.x(); xi <= (unsigned)s.x(); xi++){
            unsigned xj = (unsigned)(k*xi+b);  img(xi,xj) = 0;
          }
        }
      }else{
        if(s.y() <= e.y()){
          for(unsigned xj = (unsigned)s.y(); xj <= (unsigned)e.y(); xj++){
            unsigned xi = (unsigned)((xj-b)/k);  img(xi,xj) = 0;
          }
        }else{
          for(unsigned xj = (unsigned)e.y(); xj <= (unsigned)s.y(); xj++){
            unsigned xi = (unsigned)((xj-b)/k);  img(xi,xj) = 0;
          }
        }
      }		    
    }
  }
}

void volm_query::draw_dot(vil_image_view<vxl_byte>& img, vgl_point_3d<double> const& world_point, vpgl_perspective_camera<double> const& cam)
{
  double u, v;
  cam.project(world_point.x(), world_point.y(), world_point.z(), u, v);
  int cx = (int)u;
  int cy = (int)v;
  for(int i = -5; i < 6; i++)
    for(int j = -5; j < 6; j++){
      int x = cx + i ; int y = cy + j;
	  img((unsigned)x,(unsigned)y) = 0;
	}
}

void volm_query::draw_query_images(vcl_string const& out_dir, depth_map_scene_sptr const& dm)
{
  //: create a png img associated with each camera hypothesis, containing the geometry defined 
  //  in depth_map_scene and the img points corresponding to points inside the container
  
  //: loop over all camera
  for(unsigned i = 0; i < cameras_.size(); i++) {
    //: create a vil_image_view<vxl_byte> for png query_img geometry
	vil_image_view<vxl_byte> img(ni_, nj_);
    for(unsigned img_i = 0; img_i < ni_; img_i++) {
      for(unsigned img_j = 0; img_j < nj_; img_j++) {
        img(img_i, img_j) = 255;
	  }
	}
	//: draw the polygon
	if(dm->sky()) {
      vgl_polygon<double> poly = bsol_algs::vgl_from_poly((dm->sky())->region_2d());
	  
	  this->draw_polygon(img, poly);
	  
	}
	if(dm->scene_regions().size()) {
      for(unsigned obj_i = 0; obj_i < dm->scene_regions().size(); obj_i++) {
        vgl_polygon<double> poly = bsol_algs::vgl_from_poly((dm->scene_regions())[obj_i]->region_2d());
		this->draw_polygon(img,poly);
	  }
	}
	//: draw the rays which penetrate through the depth_map_scene
	vcl_vector<unsigned char> single_layer = query_[i];
	for(unsigned pidx = 0; pidx < query_size_; pidx++)
      if(single_layer[pidx] < 255)
        this->draw_dot(img, query_points_[pidx], cameras_[i]);
	//: write into img
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