// This is brl/bseg/bvxm/pro/bvxm_roi_init_process.h
#ifndef bvxm_roi_init_process_h_
#define bvxm_roi_init_process_h_

//:
// \file
// \brief A class for clipping and image based on a 3D bounding box.
//        -  Input:
//             - Image path (string)
//             - bvxm_voxel_world_sptr
//
//        -  Output:
//             - modified rational camera "vpgl_camera_double_sptr"
//             - clipped image area (NITF) "vil_image_view_base_sptr"
//
//        -  Params:
//             -geographic uncertainty (error) in meters
//
// \author  Gamze D. Tunali
// \date    02/19/2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>

#include <vgl/vgl_point_3d.h>
#include <bgeo/bgeo_lvcs_sptr.h>
#include <bprb/bprb_process.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vil/vil_image_view.h>
#include <bvxm/bvxm_world_params.h>

class bvxm_roi_init_process : public bprb_process
{
 public:

   bvxm_roi_init_process();

  //: Copy Constructor (no local data)
  bvxm_roi_init_process(const bvxm_roi_init_process& other)
    : bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_roi_init_process(){};

  //: Clone the process
  virtual bvxm_roi_init_process* clone() const
    { return new bvxm_roi_init_process(*this); }

  vcl_string name(){return "bvxmRoiInitProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}

  bool roi_init(vcl_string const& image_path,
                vpgl_rational_camera<double>* camera,
                bvxm_world_params_sptr world_params,
                float uncertainty,
                vil_image_view<unsigned char>* nitf_image_unsigned_char,
                vpgl_local_rational_camera<double>& local_camera);
 private:

  //: projects the box on the image by taking the union of all the projected corners
  vgl_box_2d<double>* project_box(vpgl_rational_camera<double>* cam,
                                  bgeo_lvcs_sptr lvcs,
                                  vgl_box_3d<double> box,
                                  float r);

  //: return the corner point of a box (no particular order)
  //vcl_vector<vgl_point_3d<double> > corners_of_box_3d(vgl_box_3d<double> box);
};


#endif // bvxm_roi_init_process_h_
