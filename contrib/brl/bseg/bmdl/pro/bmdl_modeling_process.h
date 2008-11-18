// This is brl/bseg/bmdl/pro/bmdl_modeling_process.h
#ifndef bmdl_modeling_process_h_
#define bmdl_modeling_process_h_
//:
// \file
// \brief A class for clipping a lidar image based on a 2D bounding box.
//        -  Input:
//             - First return path "string"
//             - Last return path "string"
//             - ground image "string"
//             - output directory for kmz files "string"
//             - tile x dimension "unsigned"
//             - tile y dimension "unsigned"
//             - tile x overlapping "unsigned"
//             - tile y overlapping "unsigned"
//        -  Output:
//             -
//
//        -  Params:
//
//
// \author  Gamze D. Tunali
// \date    11/11/2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>

#include <bprb/bprb_process.h>
#include <vil/vil_image_resource_sptr.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>

class bmdl_modeling_process : public bprb_process
{
 public:

   bmdl_modeling_process();

  //: Copy Constructor (no local data)
  bmdl_modeling_process(const bmdl_modeling_process& other)
    : bprb_process(*static_cast<const bprb_process*>(&other)){}

  ~bmdl_modeling_process(){}

  //: Clone the process
  virtual bmdl_modeling_process* clone() const
    { return new bmdl_modeling_process(*this); }

  vcl_string name(){return "bmdlModelingProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}

 private:

   bool modeling(vil_image_resource_sptr lidar_first,
                 vil_image_resource_sptr lidar_last,
                 vil_image_resource_sptr ground,
                 vcl_string output_path,
                 unsigned x_dim, unsigned y_dim,
                 unsigned x_overlap, unsigned y_overlap,
                 float gthresh);
};

#endif // bmdl_modeling_process_h_
