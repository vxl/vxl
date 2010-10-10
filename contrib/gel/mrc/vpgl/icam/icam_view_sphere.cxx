#include "icam_view_sphere.h"
//:
// \file
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_camera_bounds.h>


icam_view_sphere::icam_view_sphere(vgl_box_3d<double> bb, double radius)
: view_sphere_(0)
{
  // create the view sphere
  view_sphere_ = new vsph_view_sphere<vsph_view_point<icam_view_metadata> > (bb, radius);
}

void icam_view_sphere::create_view_points(double cap_angle, double view_angle)
{
  // create the view points
  view_sphere_->add_uniform_views(cap_angle, view_angle);
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

//: sets the images and depth images, associated with the view point id
void icam_view_sphere::set_images(vcl_map<unsigned, vil_image_view<double> > images,
                                  vcl_map<unsigned,vil_image_view<double> > depth_images)
{
  vcl_map<unsigned, vil_image_view<double> >::iterator it_imgs=images.begin();
  vcl_map<unsigned, vil_image_view<double> >::iterator it_depths=depth_images.begin();
  while (it_imgs != images.end()) {
    unsigned uid = it_imgs->first;
    // make sure that there is a corresponding depth image
    if (depth_images.find(uid) != depth_images.end()) {
      vsph_view_point<icam_view_metadata> vp;
      if (view_sphere_->view_point(uid, vp)) {
        // get the camera
        vpgl_camera_double_sptr camera=vp.camera();
        vpgl_perspective_camera<double>* cam = dynamic_cast<vpgl_perspective_camera<double>*> (camera.as_pointer());
        if (cam) {
          vnl_matrix_fixed<double, 3, 3> K = cam->get_calibration().get_matrix();
          vgl_rotation_3d<double> rot;
          vgl_vector_3d<double> t(0,0,0);
          icam_depth_transform trans(K, depth_images[uid], rot, t);
          icam_view_metadata* data = new icam_view_metadata(images[uid],depth_images[uid],trans);
          vp.set_metadata(data);
        }
      }
    }
    else
      vcl_cout << "icam_view_sphere::set_images -- ERROR! There is a missing depth image for image id=" << uid << vcl_endl;
  }
}

void icam_view_sphere::register_image(vil_image_view<double> const& source_img)
{
  // try to find the best camera at each view point and at the end we will
  // have errors to compare on the view sphere
  vsph_view_sphere<vsph_view_point<icam_view_metadata> >::iterator it=view_sphere_->begin();
  while (it != view_sphere_->end()) {
    vsph_view_point<icam_view_metadata> vp = it->second;
    icam_view_metadata* data=vp.metadata();
    data->register_image(source_img);
  }
}

void icam_view_sphere::find_local_minima(vcl_vector<vsph_view_point<icam_view_metadata> >& local_minima)
{
  // go through all the viewpoints to see if it is a local maxima
  vsph_view_sphere<vsph_view_point<icam_view_metadata> >::iterator it=view_sphere_->begin();
  while (it != view_sphere_->end()) {
    vsph_view_point<icam_view_metadata> vp = it->second;
    unsigned vp_uid = it->first;
    double error=vp.metadata()->error();

    // find the closest neighbors errors
    vcl_vector<vsph_view_point<icam_view_metadata> > neighbors;
    view_sphere_->find_neighbors(vp_uid, neighbors);
    // compare the errors with the neighbors
    bool smallest=true;
    for (unsigned i=0; i<neighbors.size(); i++) {
      vsph_view_point<icam_view_metadata> vp = neighbors[i];
      icam_view_metadata* data = vp.metadata();
      if (data->error() < error) {
        smallest=false;
      }
    }

    if (smallest) {
      local_minima.push_back(vp);
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
