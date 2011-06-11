#include "icam_ocl_view_sphere.h"
#include "icam_ocl_view_metadata.h"
//:
// \file

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_camera_bounds.h>

//: sets the images and depth images, associated with the view point id
void icam_ocl_view_sphere::set_images(vcl_map<unsigned, vcl_string>& images,
                                      vcl_map<unsigned, vcl_string>& depth_images)
{
  vcl_map<unsigned, vcl_string>::iterator it_imgs=images.begin();
  vcl_map<unsigned, vcl_string>::iterator it_depths=depth_images.begin();
  while (it_imgs != images.end()) {
    unsigned uid = it_imgs->first;
    if (images[uid].size()>0) {
      // make sure that there is a corresponding depth image
      if (depth_images.find(uid) != depth_images.end()) {
        vsph_view_point<icam_view_metadata>* vp;
        if (view_sphere_->view_point(uid, vp)) {
          // get the camera
          vpgl_camera_double_sptr camera=vp->camera();
          vpgl_perspective_camera<double>* cam = dynamic_cast<vpgl_perspective_camera<double>*> (camera.as_pointer());
          if (cam) {
            icam_view_metadata* data = new icam_ocl_view_metadata(images[uid],depth_images[uid]);
            vp->set_metadata(data);
          }
        }
      }
      else
        vcl_cout << "icam_ocl_view_sphere::set_images -- ERROR! There is a missing depth image for image id=" << uid << vcl_endl;
    }
    it_imgs++;
  }
}

void vsl_b_read(vsl_b_istream &is, icam_ocl_view_sphere &sp)
{
  sp.b_read(is);
}

void vsl_b_write(vsl_b_ostream &os, icam_ocl_view_sphere const& sp)
{
  sp.b_write(os);
}

void vsl_b_read(vsl_b_istream &is, icam_ocl_view_sphere* sp)
{
  sp->b_read(is);
}

void vsl_b_write(vsl_b_ostream &os, const icam_ocl_view_sphere* sp)
{
  sp->b_write(os);
}

