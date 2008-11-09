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
#include <vcl_iosfwd.h>

#include <bprb/bprb_process.h>
#include <vil/vil_image_view_base.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>

#include <imesh/imesh_mesh.h>
#include <vgl/vgl_point_3d.h>

#if (HAS_ZLIB)
#include <minizip/zip.h>
#endif

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
                     vpgl_geo_camera* const lidar_cam);

  void update_mesh_coord(imesh_mesh& imesh, vpgl_geo_camera* cam);

  //: Write the KML file for wrapping a COLLADA mesh
  // \a lookat and \a location are (long,lat,alt) vectors
  // \a orientation is a (heading,tilt,roll) vector
  void write_kml_collada_wrapper(vcl_ostream& os,
                                 const vcl_string& object_name,
                                 const vgl_point_3d<double>& lookat,
                                 const vgl_point_3d<double>& location,
                                 const vgl_point_3d<double>& orientation,
                                 const vcl_string& filename);

  void generate_kml(vcl_string& kml_filename, imesh_mesh& mesh, vpgl_geo_camera* lidar_cam);

  void generate_kml_collada(vcl_string& kmz_dir, imesh_mesh& mesh, vpgl_geo_camera* lidar_cam);

#if (HAS_ZLIB)
  int zip_kmz(zipFile& zf, const char* filenameinzip);
#endif
};

#endif // bmdl_generate_mesh_process_h_
