#include "icam_view_sphere.h"
//:
// \file
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_camera_bounds.h>
#include <vil/vil_save.h>

icam_view_sphere::icam_view_sphere(vgl_box_3d<double> bb, double radius)
: view_sphere_(0), ICAM_LOCAL_MIN_THRESH_(100)
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
void icam_view_sphere::cameras(vcl_map<unsigned, vpgl_camera_double_sptr> &cameras)
{
  vsph_view_sphere<vsph_view_point<icam_view_metadata> >::iterator it=view_sphere_->begin();
  while (it != view_sphere_->end()) {
    vsph_view_point<icam_view_metadata> vp = it->second;
    unsigned id = it->first;
    cameras[id] = vp.camera();
    it++;
  }
}

void icam_view_sphere::set_cameras(vcl_map<unsigned, vpgl_camera_double_sptr> const &cameras)
{
  vsph_view_sphere<vsph_view_point<icam_view_metadata> >::iterator it=view_sphere_->begin();
  vcl_map<unsigned, vpgl_camera_double_sptr>::const_iterator cam_it=cameras.begin();
  while (it != view_sphere_->end() && cam_it != cameras.end()) {
    vpgl_camera_double_sptr cam = cam_it->second;
    it->second.set_camera(cam);
    vpgl_perspective_camera<double>* pers_cam = static_cast<vpgl_perspective_camera<double>*>(cam.as_pointer());
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
void icam_view_sphere::set_images(vcl_map<unsigned, vil_image_view<float>*>& images,
                                  vcl_map<unsigned,vil_image_view<double>*>& depth_images)
{
  vcl_map<unsigned, vil_image_view<float>*>::iterator it_imgs=images.begin();
  vcl_map<unsigned, vil_image_view<double>*>::iterator it_depths=depth_images.begin();
  while (it_imgs != images.end()) {
    unsigned uid = it_imgs->first;
    if (images[uid]) {
      // make sure that there is a corresponding depth image
      if (depth_images.find(uid) != depth_images.end()) {
        vsph_view_point<icam_view_metadata>* vp;
        if (view_sphere_->view_point(uid, vp)) {
          // get the camera
          vpgl_camera_double_sptr camera=vp->camera();
          vpgl_perspective_camera<double>* cam = dynamic_cast<vpgl_perspective_camera<double>*> (camera.as_pointer());
          if (cam) {
            vnl_matrix_fixed<double, 3, 3> K = cam->get_calibration().get_matrix();
            vgl_rotation_3d<double> rot=cam->get_rotation();
            vgl_vector_3d<double> t=cam->get_translation();
            icam_depth_transform dt(K, *depth_images[uid], rot, t);
            icam_view_metadata* data = new icam_view_metadata(*images[uid],*depth_images[uid],dt);
            vp->set_metadata(data);
          }
        }
      }
      else
        vcl_cout << "icam_view_sphere::set_images -- ERROR! There is a missing depth image for image id=" << uid << vcl_endl;
    }
    it_imgs++;
  }
}

void icam_view_sphere::register_image(vil_image_view<float> const& dest_img)
{
  // try to find the best camera at each view point and at the end we will
  // have errors to compare on the view sphere
  vsph_view_sphere<vsph_view_point<icam_view_metadata> >::iterator it=view_sphere_->begin();
  while (it != view_sphere_->end()) {
    vsph_view_point<icam_view_metadata> vp = it->second;
    icam_view_metadata* data=vp.metadata();
    if (data)
      data->register_image(dest_img);
    it++;
  }

  vcl_vector<vsph_view_point<icam_view_metadata> > local_min;
  find_local_minima(local_min);
#if 0
  vsph_view_point<icam_view_metadata>* vp;
  view_sphere_->view_point(142,vp);
  vpgl_perspective_camera<double>* cam = (vpgl_perspective_camera<double>*)vp->camera().as_pointer();
  vcl_cout << *cam;
  local_min.push_back(*vp);
#endif
  double cam_error=1e10;
  unsigned idx=-1;
  for (unsigned i=0; i<local_min.size(); i++) {
    vcl_cout << "Local MINIMA " << i << "--" << local_min[i].view_point() << vcl_endl;
    icam_view_metadata* md = local_min[i].metadata();
    md->compute_camera();
    if (md->error() < cam_error) {
      cam_error = md->error();
      idx = i;
    }
  }
}

void icam_view_sphere::find_local_minima(vcl_vector<vsph_view_point<icam_view_metadata> >& local_minima)
{
  // go through all the viewpoints to see if it is a local maxima
  vsph_view_sphere<vsph_view_point<icam_view_metadata> >::iterator it=view_sphere_->begin();
  while (it != view_sphere_->end()) {
    vsph_view_point<icam_view_metadata> vp = it->second;
    unsigned vp_uid = it->first;
    if (!vp.metadata()) {
      it++;
      continue;
    }
    double error=vp.metadata()->error();

    // find the closest neighbors' errors
    vcl_vector<vsph_view_point<icam_view_metadata> > neighbors;
    view_sphere_->find_neighbors(vp_uid, neighbors);
    // compare the errors with the neighbors
    bool smallest=true;
    double smallest_diff=1e308;
    for (unsigned i=0; i<neighbors.size(); i++) {
      vsph_view_point<icam_view_metadata> vp = neighbors[i];
      icam_view_metadata* data = vp.metadata();
      if (data) {
        if (data->error() < error) {
          smallest=false;
        }
        else {
          double diff = data->error()-error;
          if (smallest_diff > diff)
            smallest_diff = diff;
        }
      }
    }
    if (smallest) { // && smallest_diff > ICAM_LOCAL_MIN_THRESH_
      // the smallest should be really different, and much smaller than the neighborhood
      vcl_cout << " Selected-->" << vp_uid << " how far?=" << smallest_diff << vcl_endl;
      local_minima.push_back(vp);
    }
    it++;
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
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, icam_view_sphere&)\n"
               << "           Unknown version number "<< version << '\n';
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
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

void vsl_b_read(vsl_b_istream &is, icam_view_sphere_sptr &sp)
{
  sp->b_read(is);
}

void vsl_b_write(vsl_b_ostream &os, icam_view_sphere_sptr const& sp)
{
  sp->b_write(os);
}
