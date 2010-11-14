#include "icam_ocl_view_sphere.h"
#include "icam_ocl_view_metadata.h"
//:
// \file
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_camera_bounds.h>
#include <vil/vil_save.h>


//: sets the images and depth images, associated with the view point id
void icam_ocl_view_sphere::set_images(vcl_map<unsigned, vil_image_view<float>*>& images,
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
            icam_view_metadata* data = new icam_ocl_view_metadata(*images[uid],*depth_images[uid],dt);

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



