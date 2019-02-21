// This is gcorr/sift/acal_tile_data_manager.h
#ifndef acal_tile_data_manager_h
#define acal_tile_data_manager_h

//:
// \file
/// \brief A class to manage tile metadata and file I/O
// \author J.L. Mundy
// \date January 10, 2018
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include <map>

#include <vpgl/vpgl_affine_camera.h>
#include <bpgl/acal/acal_metadata.h>
#include <bpgl/acal/acal_f_utils.h>


class acal_tile_data_manager
{
public:
  //: default constructor
  acal_tile_data_manager():
    image_format_ext_(".tif"),
    camera_format_ext_(".rpb"),
    fmatches_fname_("fmatches_F_filt.txt"),
    affine_cam_fname_("F_affine_cams.txt"),
    match_matrix_fname_("match_matrix.jpg"),
    affine_corrected_format_ext_("_affine_corrected.txt"),
    F_identity_fname_("F_matrix_identity_pairs.txt"),
    degen_ray_fname_("degenerate_ray_pairs.txt"),
    image_metadata_path_("images.json"),
    tile_metadata_path_("tiles.json"),
    geo_corr_metadata_path_("geo_corr_metadata.json"),
    tile_ptset_path_("dem_ptset.txt")
    {}

  //: constructor to specify just image type (image_format_ext = {.jpg, tif, .bmp, .gif, .png, .pnm, .psd})
  acal_tile_data_manager(std::string const& image_format_ext):
   image_format_ext_(image_format_ext),
   camera_format_ext_(".rpb"),
   fmatches_fname_("fmatches_F_filt.txt"),
   affine_cam_fname_("F_affine_cams.txt"),
   match_matrix_fname_("match_matrix.jpg"),
   affine_corrected_format_ext_("_affine_corrected.txt"),
   F_identity_fname_("F_matrix_identity_pairs.txt"),
   degen_ray_fname_("degenerate_ray_pairs.txt"),
   image_metadata_path_("images.json"),
   tile_metadata_path_("tiles.json"),
   geo_corr_metadata_path_("geo_corr_metadata.json"),
   tile_ptset_path_("dem_ptset.txt")
   {}

  //: constructor to specify all type and path values
  // image_format_ext   - the image type extension to control the type of image to be loaded
  // camera_format_ext  - the extension to specify a local rational camera file format (typically .rpb)
  // fmatches_fname     - the file name to store sift feature matches between image pairs
  // affine_cam_fname   - the file name to store affine camera approximations to each rational camera
  // match_matrix_fname - the file name to store the match incidence matrix in .jpg format
  // F_matrix_ident_pairs_fname  - the file name to store image ids that are identical according to F matrix elements
  // degen_ray_pairs_fname       - the file name to store image pair ids that degenerate according to ray intersection
  // affine_corrected_format_ext - the file name extenstion to be appended to an image name
  //                               to store the corrected affine camera in ascii format
  // NEEDS UPDATE TO INCLUDE ALL PATHS -- FIXME!!!
  acal_tile_data_manager(
      std::string const& image_format_ext, std::string const& camera_format_ext,
      std::string const& fmatches_fname = "fmatches_F_filt.txt",
      std::string const& affine_cam_fname = "F_affine_cams.txt",
      std::string const& match_matrix_fname = "match_matrix.jpg",
      std::string const& F_matrix_ident_pairs_fname = "F_matrix_identity_pairs.txt",
      std::string const& degen_ray_pairs_fname = "degenerate_ray_pairs.txt",
      std::string const& affine_corrected_format_ext = "_affine_corrected.txt"):
    image_format_ext_(image_format_ext),
    camera_format_ext_(camera_format_ext),
    fmatches_fname_(fmatches_fname),
    affine_cam_fname_(affine_cam_fname),
    match_matrix_fname_(match_matrix_fname),
    F_identity_fname_(F_matrix_ident_pairs_fname),
    degen_ray_fname_(degen_ray_pairs_fname),
    affine_corrected_format_ext_(affine_corrected_format_ext)
    {}

  //: alternative way to set image type  extension, e.g. ".tif"
  void set_image_format_ext(std::string const& format_ext){ image_format_ext_ = format_ext;}

  //: read iformation from the tile directories and from the metadata file
  bool init_tile_paths(std::string const& tile_dir, std::string metadata_dir, bool add_crop = true);

  std::map<size_t, std::string> image_paths(size_t tile_id);
  std::map<size_t, std::string> camera_paths(size_t tile_id);

  //:members
  acal_metadata meta_;
  std::map<std::string, size_t> meta_img_names_to_id_;
  std::string fmatches_fname_;
  std::string affine_cam_fname_;
  std::string match_matrix_fname_;
  std::string F_identity_fname_;
  std::string degen_ray_fname_;
  std::string tile_dir_;
  std::string image_format_ext_;
  std::string camera_format_ext_;
  std::string affine_corrected_format_ext_;
  std::string metadata_dir_;
  std::string image_metadata_path_;
  std::string tile_metadata_path_;
  std::string geo_corr_metadata_path_;
  std::string tile_ptset_path_;

  //       tile_id
  std::map<size_t, std::string> tile_dirs_;
  std::map<size_t, std::map<size_t, std::string> > tile_image_names_;
  std::map<size_t, vgl_box_3d<double> > tile_bounds_;

};
#endif
