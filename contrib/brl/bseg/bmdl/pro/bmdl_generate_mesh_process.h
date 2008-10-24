// This is brl/bseg/bmdl/pro/bmdl_generate_mesh_process.h
#ifndef bmdl_generate_mesh_process_h_
#define bmdl_generate_mesh_process_h_
//:
// \file
// \brief A class for clipping and image based on a 3D bounding box.
//        -  Input:
//             - file path for polygons (binary format)
//             - label image "vil_image_view_base_sptr"
//             - height image "vil_image_view_base_sptr"
//             - ground image "vil_image_view_base_sptr"
//             - file path for output meshes (binary format)
//             - lidar Camera "vil_camera_double_sptr"
//
//        -  Output:
//             - 
//        -  Params:
//
//
// \author  Gamze D. Tunali
// \date    10/14/2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>

#include <bprb/bprb_process.h>
#include <vil/vil_image_view_base.h>
#include <vpgl/vpgl_lidar_camera.h>

#include <imesh/imesh_mesh.h>

class bmdl_generate_mesh_process : public bprb_process
{
 public:

   bmdl_generate_mesh_process();

   //: Copy Constructor (no local data)
   bmdl_generate_mesh_process(const bmdl_generate_mesh_process& other)
    : bprb_process(*static_cast<const bprb_process*>(&other)){}

  ~bmdl_generate_mesh_process(){}

  //: Clone the process
  virtual bmdl_generate_mesh_process* clone() const
    { return new bmdl_generate_mesh_process(*this); }

  vcl_string name(){return "bmdlGenerateMeshProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}

 private:

   bool generate_mesh(vcl_string fpath_poly, vil_image_view_base_sptr label_img,
                      vil_image_view_base_sptr height_img, 
                      vil_image_view_base_sptr ground_img,
                      vcl_string fpath_mesh, 
                      vpgl_lidar_camera* const lidar_cam);

   void update_mesh_coord(imesh_mesh& imesh, vpgl_lidar_camera* cam);

   void generate_kml(vcl_string& kml_filename, imesh_mesh& mesh, vpgl_lidar_camera* lidar_cam);
};

#endif // bmdl_generate_mesh_process_h_
