// This is brl/bseg/bmdl/pro/bmdl_lidar_roi_process.h
#ifndef bmdl_lidar_roi_process_h_
#define bmdl_lidar_roi_process_h_
//:
// \file
// \brief A class for clipping a lidar image based on a 2D bounding box.
//        -  Input:
//             - First return path "string"
//             - Last return path "string"
//             - min_lat "float"
//             - min_lon "float"
//             - max_lat "float"
//             - max_lon "float"
//        -  Output:
//             - first return roi "vil_image_view_base_sptr"
//             - last return roi  "vil_image_view_base_sptr"
//             - lvcs "bgeo_lvcs_sptr"
//
//        -  Params:
//
//
// \author  Gamze D. Tunali
// \date    10/23/2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>

#include <bprb/bprb_process.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_resource_sptr.h>
#include <vpgl/algo/vpgl_geo_camera.h>

class bmdl_lidar_roi_process : public bprb_process
{
 public:

   bmdl_lidar_roi_process();

  //: Copy Constructor (no local data)
  bmdl_lidar_roi_process(const bmdl_lidar_roi_process& other)
    : bprb_process(*static_cast<const bprb_process*>(&other)){}

  ~bmdl_lidar_roi_process(){}

  //: Clone the process
  virtual bmdl_lidar_roi_process* clone() const
    { return new bmdl_lidar_roi_process(*this); }

  vcl_string name(){return "bmdlLidarRoiProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}

 private:

   bool lidar_roi(vil_image_resource_sptr lidar_first,
                  vil_image_resource_sptr lidar_last,
                  float min_lat, float min_lon, 
                  float max_lat, float max_lon,
                  vil_image_view_base_sptr& first_roi,
                  vil_image_view_base_sptr& last_roi,
                  vpgl_geo_camera*& lvcs);

  //: projects the box on the image by taking the union of all the projected corners
  bool comp_trans_matrix(double sx1, double sy1, double sz1,
                         vcl_vector<vcl_vector<double> > tiepoints,
                         vnl_matrix<double>& trans_matrix);
};

#endif // bmdl_lidar_roi_process_h_
