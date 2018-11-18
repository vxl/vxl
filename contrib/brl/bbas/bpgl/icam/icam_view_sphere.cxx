#include "icam_view_sphere.h"
//:
// \file
#include <vpgl/vpgl_perspective_camera.h>
#include <vsph/vsph_camera_bounds.h>

icam_view_sphere::icam_view_sphere(vgl_box_3d<double> bb, double radius)
: view_sphere_(nullptr), ICAM_LOCAL_MIN_THRESH_(100)
{
  // create the view sphere
  view_sphere_ = new vsph_view_sphere<vsph_view_point<icam_view_metadata> > (bb, radius);
}

void icam_view_sphere::create_view_points(double cap_angle, double view_angle, unsigned ni, unsigned nj)
{
  // create the view points
  view_sphere_->add_uniform_views(cap_angle, view_angle,ni,nj);
}

//: returns the cameras of the view points, associated with the view point id
void icam_view_sphere::cameras(std::map<unsigned, vpgl_camera_double_sptr> &cameras)
{
  auto it=view_sphere_->begin();
  while (it != view_sphere_->end()) {
    vsph_view_point<icam_view_metadata> vp = it->second;
    unsigned id = it->first;
    cameras[id] = vp.camera();
    it++;
  }
}

void icam_view_sphere::set_cameras(std::map<unsigned, vpgl_camera_double_sptr> const &cameras)
{
  auto it=view_sphere_->begin();
  auto cam_it=cameras.begin();
  while (it != view_sphere_->end() && cam_it != cameras.end()) {
    vpgl_camera_double_sptr cam = cam_it->second;
    it->second.set_camera(cam);
    auto* pers_cam = static_cast<vpgl_perspective_camera<double>*>(cam.as_pointer());
    vgl_homg_point_3d<double> center = pers_cam->camera_center();
    vsph_spherical_coord coord;
    vsph_sph_point_3d sp;
    coord.spherical_coord(center, sp);
    it->second.set_view_point(sp);
    it++;
    cam_it++;
  }
}

//: sets the images and depth images, associated with the view point id
void icam_view_sphere::set_images(std::map<unsigned, std::string>& images,
                                  std::map<unsigned, std::string>& depth_images)
{
  auto it_imgs=images.begin();
  while (it_imgs != images.end()) {
    unsigned uid = it_imgs->first;
    if (images[uid].size()>0) {
      // make sure that there is a corresponding depth image
      if (depth_images.find(uid) != depth_images.end()) {
        vsph_view_point<icam_view_metadata>* vp;
        if (view_sphere_->view_point(uid, vp)) {
          // get the camera
          vpgl_camera_double_sptr camera=vp->camera();
          auto* cam = dynamic_cast<vpgl_perspective_camera<double>*> (camera.as_pointer());
          if (cam) {
            icam_view_metadata* data = new icam_view_metadata(images[uid],depth_images[uid]);
            vp->set_metadata(data);
          }
        }
      }
      else
        std::cout << "icam_view_sphere::set_images -- ERROR! There is a missing depth image for image id=" << uid << std::endl;
    }
    it_imgs++;
  }
}

void icam_view_sphere::register_image(vil_image_view<float> const& dest_img,
                                      icam_minimizer_params const& params)
{
  // try to find the best camera at each view point and at the end we will
  // have errors to compare on the view sphere
  auto it=view_sphere_->begin();
  unsigned index = 0;
  while (it != view_sphere_->end()) {
    vsph_view_point<icam_view_metadata> vp = it->second;
    icam_view_metadata* data=vp.metadata();
    if (data) {
      vpgl_camera_double_sptr camera=vp.camera();
      std::cout << "Evaluating viewpoint " << index << '\n';
      data->register_image(dest_img, camera, params);
    }
    it++; index++;
  }

  std::vector<vsph_view_point<icam_view_metadata> > local_min;
  find_local_minima(local_min);
#if 0
  std::vector<vsph_view_point<icam_view_metadata> > local_min;
  vsph_view_point<icam_view_metadata>* vp;
  view_sphere_->view_point(85,vp);
  vpgl_perspective_camera<double>* cam = (vpgl_perspective_camera<double>*)vp->camera().as_pointer();
  std::cout << *cam;
  local_min.push_back(*vp);
  //vsph_view_point<icam_view_metadata>* vp;
  view_sphere_->view_point(87,vp);
  cam = (vpgl_perspective_camera<double>*)vp->camera().as_pointer();
  std::cout << *cam;
  local_min.push_back(*vp);
  //vsph_view_point<icam_view_metadata>* vp;
  view_sphere_->view_point(88,vp);
  cam = (vpgl_perspective_camera<double>*)vp->camera().as_pointer();
  std::cout << *cam;
  local_min.push_back(*vp);
#endif
  double cam_cost=1e99; // will become min cost, so initialise with high value
  for (unsigned i=0; i<local_min.size(); i++) {
    std::cout << "Local MINIMA " << i << "--" << local_min[i].view_point() << std::endl;
    auto* gt_cam =
      dynamic_cast<vpgl_perspective_camera<double>* >(ground_truth_cam_.as_pointer());
    if (gt_cam) {
      vpgl_perspective_camera<double>* cam = (vpgl_perspective_camera<double>*)local_min[i].camera().as_pointer();
      vgl_rotation_3d<double> rel_rot;
      vgl_vector_3d<double> rel_trans;
      vsph_camera_bounds::relative_transf(*gt_cam, *cam,rel_rot,rel_trans);
      std::cout <<"***************************************\n"
               << "Rel Rot=" << rel_rot << '\n'
               << "Rel trans=" << rel_trans << '\n'
               <<"***************************************" << std::endl;
    }
    icam_view_metadata* md = local_min[i].metadata();
    md->refine_camera(dest_img, local_min[i].camera(),params);
    if (md->cost() < cam_cost) {
      cam_cost = md->cost();
    }
  }
}

void icam_view_sphere::find_local_minima(std::vector<vsph_view_point<icam_view_metadata> >& local_minima)
{
  // go through all the viewpoints to see if it is a local maxima
  auto it=view_sphere_->begin();
  while (it != view_sphere_->end()) {
    vsph_view_point<icam_view_metadata> vp = it->second;
    unsigned vp_uid = it->first;
    if (!vp.metadata()) {
      it++;
      continue;
    }
    double cost=vp.metadata()->cost();

    // find the closest neighbors' errors
    std::vector<vsph_view_point<icam_view_metadata> > neighbors;
    view_sphere_->find_neighbors(vp_uid, neighbors);
    // compare the errors with the neighbors
    bool smallest=true;
    double smallest_diff=1e308;
    for (auto vp : neighbors) {
      icam_view_metadata* data = vp.metadata();
      if (data) {
        if (data->cost() < cost) {
          smallest=false;
        }
        else {
          double diff = data->cost()-cost;
          if (smallest_diff > diff)
            smallest_diff = diff;
        }
      }
    }
    if (smallest) { // && smallest_diff > ICAM_LOCAL_MIN_THRESH_
      // the smallest should be really different, and much smaller than the neighborhood
      std::cout << " Selected-->" << vp_uid << " how far?=" << smallest_diff << std::endl;
      local_minima.push_back(vp);
    }
    it++;
  }
}

void icam_view_sphere::camera_transf(vpgl_perspective_camera<double> const& cam)
{
  auto it=view_sphere_->begin();
  while (it != view_sphere_->end()) {
    vsph_view_point<icam_view_metadata> vp = it->second;
    if (!vp.metadata()) {
      it++;
      continue;
    }
    unsigned vp_uid = it->first;
    vpgl_perspective_camera<double>* vp_cam = static_cast<vpgl_perspective_camera<double>*>(vp.camera().as_pointer());
    vgl_rotation_3d<double> rel_rot;
    vgl_vector_3d<double> rel_trans;
    vsph_camera_bounds::relative_transf(cam, *vp_cam,rel_rot,rel_trans);
    std::cout <<"***************************************\n"
             << "VIEW POINT " << vp_uid << '\n'
             << "Rel Rot=" << rel_rot << '\n'
             << "Rel trans=" << rel_trans << '\n'
             <<"***************************************" << std::endl;
  }
}

//: the mapped source image and actual destination image at a level
void icam_view_sphere::mapped_image(unsigned viewpoint_id,
                                    vil_image_view<float> const& source_img,
                                    vgl_rotation_3d<double>& rot,
                                    vgl_vector_3d<double>& trans,
                                    unsigned level,
                                    vil_image_view<float>& act_dest,
                                    vil_image_view<float>& mapped_dest,
                                    icam_minimizer_params const& params)
{
  vsph_view_point<icam_view_metadata>* vp;
  if (view_sphere_->view_point(viewpoint_id, vp)) {
    icam_view_metadata* data=vp->metadata();
    if (data) {
      vpgl_perspective_camera<double>* vp_cam = static_cast<vpgl_perspective_camera<double>*>(vp->camera().as_pointer());
      data->mapped_image(source_img, vp_cam, rot, trans, level, params, act_dest, mapped_dest);
    }
  }
}

void icam_view_sphere::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short version;
  vsl_b_read(is, version);
  switch (version)
  {
    case 1:
    {
      vsph_view_sphere<vsph_view_point<icam_view_metadata> > view_sphere;
      vsl_b_read(is, view_sphere);
      if (view_sphere_)
        *view_sphere_ = view_sphere;
      else
        view_sphere_=new vsph_view_sphere<vsph_view_point<icam_view_metadata> >(view_sphere);
    }
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, icam_view_sphere&)\n"
               << "           Unknown version number "<< version << '\n';
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      break;
  }
}

void icam_view_sphere::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, *view_sphere_);
}

void vsl_b_read(vsl_b_istream &is, icam_view_sphere &sp)
{
  sp.b_read(is);
}

void vsl_b_write(vsl_b_ostream &os, icam_view_sphere const& sp)
{
  sp.b_write(os);
}

void vsl_b_read(vsl_b_istream &is, icam_view_sphere* sp)
{
  sp->b_read(is);
}

void vsl_b_write(vsl_b_ostream &os, const icam_view_sphere* sp)
{
  sp->b_write(os);
}
