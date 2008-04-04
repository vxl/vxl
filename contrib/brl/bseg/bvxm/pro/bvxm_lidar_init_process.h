// This is brl/bseg/bvxm/pro/bvxm_lidar_init_process.h
#ifndef bvxm_lidar_init_process_h_
#define bvxm_lidar_init_process_h_

//:
// \file
// \brief A class for clipping and image based on a 3D bounding box.
//        -  Input:
//             - First return path (string)
//             - Second return path (string)
//             - bvxm_voxel_world_sptr
//
//        -  Output:
//             - lidar pseudo camera "vpgl_camera_double_sptr"
//             - clipped image area (first ret) "vil_image_view_base_sptr"
//             - clipped image area (second ret) "vil_image_view_base_sptr"
//             - mask "vil_image_view_base_sptr"
//
//        -  Params:
//             - threshold for first and second return differences 
//
// \author  Gamze D. Tunali
// \date    04/02/2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <vcl_vector.h>

#include <vgl/vgl_point_3d.h>
#include <bgeo/bgeo_lvcs_sptr.h>
#include <bprb/bprb_process.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource_sptr.h>

#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_lidar_camera.h>

class bvxm_lidar_init_process : public bprb_process
{
 public:

   bvxm_lidar_init_process();

  //: Copy Constructor (no local data)
  bvxm_lidar_init_process(const bvxm_lidar_init_process& other)
    : bprb_process(*static_cast<const bprb_process*>(&other)){}

  ~bvxm_lidar_init_process(){}

  //: Clone the process
  virtual bvxm_lidar_init_process* clone() const
    { return new bvxm_lidar_init_process(*this); }

  vcl_string name(){return "bvxmLidarInitProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}

  bool lidar_init(vil_image_resource_sptr lidar, 
                  bvxm_world_params_sptr params,
                  vil_image_view_base_sptr& roi,
                  bvxm_lidar_camera*& camera);

 private:

  //: projects the box on the image by taking the union of all the projected corners
  bool comp_trans_matrix(double sx1, double sy1, double sz1,//vil_geotiff_header* gtif,
                         vcl_vector<vcl_vector<double> > tiepoints,
                         vnl_matrix<double>& trans_matrix);

  bool gen_mask(vil_image_view_base_sptr roi_first, 
                                       bvxm_lidar_camera* cam_first, 
                                       vil_image_view_base_sptr roi_second, 
                                       bvxm_lidar_camera* cam_second, 
                                       vil_image_view_base_sptr& mask,
                                       double thresh);

  //: return the corner point of a box (no particular order)
  //vcl_vector<vgl_point_3d<float> > corners_of_box_3d(vgl_box_3d<float> box);
};


#endif // bvxm_lidar_init_process_h_
