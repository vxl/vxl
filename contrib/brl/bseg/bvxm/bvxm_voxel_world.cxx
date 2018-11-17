// This is brl/bseg/bvxm/bvxm_voxel_world.cxx
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "bvxm_voxel_world.h"
//:
// \file

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_2d.h>

#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_camera.h>
#include <vul/vul_file.h>

#include <vnl/vnl_math.h>

#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>

#include <vil/algo/vil_median.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/vil_math.h>
#include <vil/algo/vil_threshold.h>
#include <vil/algo/vil_gauss_filter.h>

#include <vnl/algo/vnl_chi_squared.h>

#include "grid/bvxm_voxel_grid.h"
#include "bvxm_voxel_traits.h"
#include "bvxm_lidar_processor.h"
#include "bvxm_world_params.h"
#include "grid/bvxm_voxel_slab.h"
#include "bvxm_image_metadata.h"
#include "bvxm_util.h"
#include <vpgl/file_formats/vpgl_geo_camera.h>
//#define DEBUG

//: Destructor
bvxm_voxel_world::~bvxm_voxel_world()
= default;


//: equality operator
bool bvxm_voxel_world::operator == (bvxm_voxel_world const& /*that*/) const
{
  return true;
}

//: less than operator
bool bvxm_voxel_world::operator < (bvxm_voxel_world const& /*that*/) const
{
  return false;
}


vgl_plane_3d<double> bvxm_voxel_world::fit_plane()
{
  // for now, just return plane of bottom layer. can do something smarter with occupancy probs later.
  vgl_point_3d<double> corner(params_->corner().x(),params_->corner().y(),params_->corner().z());
  vgl_vector_3d<double> normal(0,0,1.0);

  return vgl_plane_3d<double>(normal,corner);
}

//: output description of voxel world to stream.
std::ostream&  operator<<(std::ostream& s, bvxm_voxel_world const& vox_world)
{
  bvxm_world_params_sptr params = vox_world.get_params();
  s << "bvxm_voxel_world : " << params->num_voxels().x() << " x " << params->num_voxels().y() << " x " << params->num_voxels().z() << std::endl;
  return s;
}

//: save the occupancy grid as an 8-bit 3-d vff image
bool bvxm_voxel_world::save_occupancy_vff(const std::string& filename,unsigned scale_idx)
{
  // open file for binary writing
  std::fstream ofs(filename.c_str(),std::ios::binary | std::ios::out);
  if (!ofs.is_open()) {
    std::cerr << "error opening file " << filename << " for write!\n";
    return false;
  }
  bvxm_world_params_sptr params = this->get_params();

  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  bvxm_voxel_grid<ocp_datatype> *ocp_grid =
    dynamic_cast<bvxm_voxel_grid<ocp_datatype>*>(get_grid<OCCUPANCY>(0,scale_idx).ptr());

  vxl_uint_32 nx = ocp_grid->grid_size().x();
  vxl_uint_32 ny = ocp_grid->grid_size().y();
  vxl_uint_32 nz = ocp_grid->grid_size().z();

  // write header
  std::stringstream header;
  header << "ncaa\n"
         << "title=bvxm occupancy probabilities;\n"
         << "rank=3;\n"
         << "type=raster;\n"
         << "format=slice;\n"
         << "bits=8;\n"
         << "bands=1;\n"
         << "extent=" << nx << ' ' << ny << ' ' << nz << ";\n"
         << "size=" << nx << ' ' << ny << ' ' << nz << ";\n"
         << "aspect=1.0 1.0 1.0;\n"
         << "origin=0 0 0;\n"
         << "rawsize=" << nx*ny*nz << ";\n\f\n";

  std::string header_string = header.str();
  unsigned header_len = header_string.size();

  ofs.write(header_string.c_str(),header_len);

  // write data
  // iterate through slabs and fill in memory array
  char *ocp_array = new char[nx*ny*nz];

  bvxm_voxel_grid<ocp_datatype>::iterator ocp_it = ocp_grid->begin();
  for (unsigned k=nz-1; ocp_it != ocp_grid->end(); ++ocp_it, --k) {
    std::cout << '.';
    for (unsigned i=0; i<(*ocp_it).nx(); ++i) {
      for (unsigned j=0; j < (*ocp_it).ny(); ++j) {
        ocp_array[k*nx*ny + j*nx + i] = (unsigned char)((*ocp_it)(i,j) * 255.0);;
      }
    }
  }
  std::cout << std::endl;
  ofs.write(reinterpret_cast<char*>(ocp_array),sizeof(unsigned char)*nx*ny*nz);

  ofs.close();

  delete[] ocp_array;

  return true;
}

//: remove all voxel data from disk - use with caution!
bool bvxm_voxel_world::clean_grids()
{
  // look for existing grids in the directory
  std::string storage_directory = params_->model_dir();

  std::stringstream grid_glob;
  grid_glob << storage_directory << "/*.vox";
  bool result = vul_file::delete_file_glob(grid_glob.str().c_str());

  grid_map_.clear();

  return result;
}

#if 0 // the following three methods commented out:
// Update a voxel grid with data from lidar/camera pair
bool bvxm_voxel_world::update_lidar(bvxm_image_metadata const& observation, unsigned scale)
{
  vil_image_view<float> dummy;
  vil_image_view<bool> mask;
  return this->update_lidar_impl(observation, false, dummy, false, mask, scale);
}


// Update a voxel grid with data from lidar/camera pair and return probability density of pixel values.
bool bvxm_voxel_world::update_lidar(bvxm_image_metadata const& observation,
                                    vil_image_view<float> &pix_prob_density, vil_image_view<bool> &mask, unsigned scale)
{
  // check image sizes
  if ( (observation.img->ni() != pix_prob_density.ni()) || (observation.img->nj() != pix_prob_density.nj()) ) {
    std::cerr << "error: metadata image size does not match probability image size.\n";
  }
  if ( (observation.img->ni() != mask.ni()) || (observation.img->nj() != mask.nj()) ) {
    std::cerr << "error: metadata image size does not match mask image size.\n";
  }
  return this->update_lidar_impl(observation, true, pix_prob_density, true, mask, scale);
}

// Update voxel grid with data from LIDAR image/camera pair and return probability density of pixel values.
bool bvxm_voxel_world::update_lidar_impl(bvxm_image_metadata const& metadata,
                                         bool return_prob,
                                         vil_image_view<float> &pix_prob_density,
                                         bool return_mask,
                                         vil_image_view<bool> &mask, unsigned scale)
{
  typedef bvxm_voxel_traits<LIDAR>::voxel_datatype obs_datatype;
  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  vpgl_camera_double_sptr dummy_cam = metadata.camera;
  double lidar_pixel_size = 1.0;
  if (dummy_cam->type_name()=="vpgl_geo_camera"){
    vpgl_geo_camera* lcam = static_cast<vpgl_geo_camera*>(dummy_cam.ptr());
    std::cout << "Lidar Camera\n" << *lcam << std::endl;
    lidar_pixel_size = lcam->pixel_spacing();
  }
  //typedef bvxm_voxel_traits<LIDAR>::lidar_processor lidar_processor;
  bvxm_lidar_processor lidar_processor(10);

  // parameters
  vgl_vector_3d<unsigned int> grid_size = params_->num_voxels(scale);
  ocp_datatype min_vox_prob = params_->min_occupancy_prob();
  ocp_datatype max_vox_prob = params_->max_occupancy_prob();

  // compute homographies from voxel planes to image coordinates and vise-versa.
  std::vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  std::vector<vgl_h_matrix_2d<double> > H_img_to_plane;
  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    for (unsigned z=0; z < (unsigned)grid_size.z(); ++z)
    {
      compute_plane_image_H(metadata.camera,z,Hp2i,Hi2p, scale);
      H_plane_to_img.push_back(Hp2i);
      H_img_to_plane.push_back(Hi2p);
    }
  }

  // convert image to a voxel_slab
  bvxm_voxel_slab<obs_datatype> image_slab(metadata.img->ni(), metadata.img->nj(), 1);
  if (!bvxm_util::img_to_slab(metadata.img,image_slab)) {
    std::cerr << "error converting image to voxel slab of observation type for bvxm_voxel_type: LIDAR\n";
    return false;
  }

#ifdef DEBUG
  vil_save(*metadata.img, "lidar_img.tiff");
  bvxm_util::write_slab_as_image(image_slab,"lidar_slab.tiff");
#endif

  // temporary voxel grids to hold preX and PL*visX values
  bvxm_voxel_grid<float> preX(grid_size);
  bvxm_voxel_grid<float> PLvisX(grid_size);

  bvxm_voxel_slab<float> PLPX(grid_size.x(),grid_size.y(),1);
  bvxm_voxel_slab<float> PXvisX(grid_size.x(), grid_size.y(),1);

  bvxm_voxel_slab<float> preX_accum(image_slab.nx(),image_slab.ny(),1);
  bvxm_voxel_slab<float> visX_accum(image_slab.nx(),image_slab.ny(),1);
  bvxm_voxel_slab<float> img_scratch(image_slab.nx(),image_slab.ny(),1);
  bvxm_voxel_slab<float> PLPX_img(image_slab.nx(), image_slab.ny(),1);
  bvxm_voxel_slab<float> PX_img(image_slab.nx(), image_slab.ny(),1);
  bvxm_voxel_slab<float> mask_slab(image_slab.nx(), image_slab.ny(),1);

  preX_accum.fill(0.0f);
  visX_accum.fill(1.0f);

  // slabs for holding backprojections of visX
  bvxm_voxel_slab<float> visX(grid_size.x(),grid_size.y(),1);

  bvxm_voxel_slab<obs_datatype> frame_backproj(grid_size.x(),grid_size.y(),1);

  std::cout << "Pass 1:" << std::endl;

  // get occupancy probability grid
  bvxm_voxel_grid_base_sptr ocp_grid_base = this->get_grid<OCCUPANCY>(0,  scale);
  bvxm_voxel_grid<ocp_datatype> *ocp_grid  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(ocp_grid_base.ptr());


  bvxm_voxel_grid<ocp_datatype>::const_iterator ocp_slab_it = ocp_grid->begin();

  bvxm_voxel_grid<float>::iterator preX_slab_it = preX.begin();
  bvxm_voxel_grid<float>::iterator PLvisX_slab_it = PLvisX.begin();
#ifdef DEBUG
  double p_max = 0.0;
#endif
  //The default values for the LIDAR Gaussian error ellipsoid
  // X-Y standard deviation set to 1/Sqrt(2) pixel spacing (arbitrary)
  // standard deviation in z measured from actual Buckeye data (0.03 m)
  double pix_sd = lidar_pixel_size*vnl_math::sqrt1_2;
  float xy_var = static_cast<float>(pix_sd*pix_sd);
  // The vector of spherical Gaussian variances
  vnl_vector_fixed<float,3> vars(xy_var, xy_var, 0.0009f);

  for (unsigned k_idx=0; k_idx<(unsigned)grid_size.z(); ++k_idx, ++ocp_slab_it, ++preX_slab_it, ++PLvisX_slab_it)
  {
    std::cout << k_idx << std::endl;

    // backproject image onto voxel plane
    bvxm_util::warp_slab_bilinear(image_slab, H_plane_to_img[k_idx], frame_backproj);

#ifdef DEBUG
    std::stringstream ss;
    ss << "./frame_backproj_" << k_idx <<".tiff";
    bvxm_util::write_slab_as_image(frame_backproj,ss.str());
#endif
    // transform preX to voxel plane for this level
    bvxm_util::warp_slab_bilinear(preX_accum, H_plane_to_img[k_idx], *preX_slab_it);
    // transform visX to voxel plane for this level
    bvxm_util::warp_slab_bilinear(visX_accum, H_plane_to_img[k_idx], visX);

    // initialize PLvisX with PL(X)

    bvxm_voxel_slab<float> PL(frame_backproj.nx(), frame_backproj.ny(), frame_backproj.nz());
    PL.fill(0.0);
    vil_image_view_base_sptr lidar = metadata.img;

    vgl_point_3d<float> local_xyz = voxel_index_to_xyz(0, 0, k_idx,scale);

    for (unsigned i_idx=0; i_idx<frame_backproj.nx(); i_idx++) {
      for (unsigned j_idx=0; j_idx<frame_backproj.ny(); j_idx++) {
        std::vector<vgl_homg_point_2d<double> > vp(4);
        int i = i_idx+1;
        int j = j_idx-1;
        vp[0] = vgl_homg_point_2d<double>(i, j);
        vp[1] = vgl_homg_point_2d<double>(i+1, j);
        vp[2] = vgl_homg_point_2d<double>(i, j+1);
        vp[3] = vgl_homg_point_2d<double>(i+1, j+1);

        vgl_h_matrix_2d<double> h_max = H_plane_to_img[k_idx];
        vgl_h_matrix_2d<double> h_min;
        if (k_idx == (unsigned)grid_size.z()-1)
          h_min = H_plane_to_img[k_idx];
        else
          h_min = H_plane_to_img[k_idx+1];
        vgl_box_2d<double> lidar_roi;

        for (unsigned i=0; i<4; i++) {
          vgl_homg_point_2d<double> img_pos_h_min = h_min*vp[i];
          vgl_point_2d<double> img_pos_min(img_pos_h_min);
          lidar_roi.add(img_pos_min);
        }

        float p = lidar_processor.prob_density(lidar, local_xyz.z(), vars, lidar_roi, params_->voxel_length(scale));

#ifdef DEBUG
        if (p > p_max) {
          p_max = p;
          std::cout << "-------------max_p=" << p << std::endl;
        }
        if (p >1.0) {
          std::cout << "ERROR!" << std::endl;
          p=max_vox_prob;
        }
#endif
        PL(i_idx, j_idx) = p;
      }
    }

    // now multiply by visX
    bvxm_util::multiply_slabs(visX,PL,*PLvisX_slab_it);

    //Is this needed?
    // update appearance model, using PX*visX as the weights
    bvxm_util::multiply_slabs(visX,*ocp_slab_it,PXvisX);

    // multiply to get PLPX
    bvxm_util::multiply_slabs(PL,*ocp_slab_it,PLPX);
#ifdef DEBUG
    std::stringstream ss1, ss2, ss3;
    ss1 << "PL_" << k_idx <<".tiff";
    ss2 <<"PX_" << k_idx <<".tiff";
    ss3 << "PL_P" << k_idx <<".tiff";
    bvxm_util::write_slab_as_image(PL,ss1.str());
    bvxm_util::write_slab_as_image(*ocp_slab_it,ss2.str());
    //bvxm_util::write_slab_as_image(PL_p,ss3.str());
#endif
    // warp PLPX back to image domain
    bvxm_util::warp_slab_bilinear(PLPX, H_img_to_plane[k_idx], PLPX_img);

    // multiply PLPX by visX and add to preX_accum
    bvxm_voxel_slab<float>::iterator PLPX_img_it = PLPX_img.begin();
    bvxm_voxel_slab<float>::iterator visX_accum_it = visX_accum.begin();
    bvxm_voxel_slab<float>::iterator preX_accum_it = preX_accum.begin();

    for (; preX_accum_it != preX_accum.end(); ++preX_accum_it, ++PLPX_img_it, ++visX_accum_it) {
      *preX_accum_it += (*PLPX_img_it) * (*visX_accum_it);
    }
#ifdef DEBUG
    std::stringstream plpx, vis, prex;
    plpx << "PLPX_" << k_idx <<".tiff";
    vis  << "visX_" << k_idx <<".tiff";
    prex << "preX_" << k_idx <<".tiff";
    bvxm_util::write_slab_as_image(PLPX_img,plpx.str());
    bvxm_util::write_slab_as_image(visX_accum,vis.str());
    bvxm_util::write_slab_as_image(preX_accum,prex.str());
#endif
    // scale and offset voxel probabilities to get (1-P(X))
    // transform (1-P(X)) to image plane to accumulate visX for next level
    bvxm_util::warp_slab_bilinear(*ocp_slab_it, H_img_to_plane[k_idx], PX_img);

    if (return_mask) {
      bvxm_util::add_slabs(PX_img,mask_slab,mask_slab);
    }

    // note: doing scale and offset in image domain so invalid pixels become 1.0 and don't affect visX
    bvxm_voxel_slab<float>::iterator PX_img_it = PX_img.begin();
    visX_accum_it = visX_accum.begin();
    for (; visX_accum_it != visX_accum.end(); ++visX_accum_it, ++PX_img_it) {
      *visX_accum_it *= (1 - *PX_img_it);
    }
  }
  // now traverse a second time, computing new P(X) along the way.

  bvxm_voxel_slab<float> preX_accum_vox(grid_size.x(),grid_size.y(),1);
  bvxm_voxel_slab<float> visX_accum_vox(grid_size.x(),grid_size.y(),1);

#ifdef DEBUG
  std::stringstream vis2, prex2;
  vis2  << "visX2_.tiff";
  prex2 << "preX2_.tiff";
  bvxm_util::write_slab_as_image(visX_accum,vis2.str());
  bvxm_util::write_slab_as_image(preX_accum,prex2.str());
#endif

  std::cout << "\nPass 2:" << std::endl;
  PLvisX_slab_it = PLvisX.begin();
  preX_slab_it = preX.begin();
  bvxm_voxel_grid<ocp_datatype>::iterator ocp_slab_it2 = ocp_grid->begin();
  for (unsigned k_idx = 0; k_idx < (unsigned)grid_size.z(); ++k_idx, ++PLvisX_slab_it, ++preX_slab_it, ++ocp_slab_it2) {
    std::cout << '.';

    // transform preX_sum to current level
    bvxm_util::warp_slab_bilinear(preX_accum, H_plane_to_img[k_idx], preX_accum_vox);

    // transform visX_sum to current level
    bvxm_util::warp_slab_bilinear(visX_accum, H_plane_to_img[k_idx], visX_accum_vox);

    const float preX_sum_thresh = 0.0f;

    bvxm_voxel_slab<float>::const_iterator preX_it = preX_slab_it->begin(), PLvisX_it = PLvisX_slab_it->begin(), preX_sum_it = preX_accum_vox.begin(), visX_sum_it = visX_accum_vox.begin();
    bvxm_voxel_slab<float>::iterator PX_it = ocp_slab_it2->begin();

    for (; PX_it != ocp_slab_it2->end(); ++PX_it, ++preX_it, ++PLvisX_it, ++preX_sum_it, ++visX_sum_it) {
      // if preX_sum is zero at the voxel, no ray passed through the voxel (out of image)
      if (*preX_sum_it > preX_sum_thresh) {
        float multiplier = (*PLvisX_it + *preX_it) / *preX_sum_it;
        // leave out normalization for now - results seem a little better without it.  -DEC
        //float ray_norm = 1 - *visX_sum_it; //normalize based on probability that a surface voxel is located along the ray. This was not part of the original Pollard + Mundy algorithm.
        *PX_it *= multiplier; // * ray_norm;
      }
      if (*PX_it < min_vox_prob)
        *PX_it = min_vox_prob;
      if (*PX_it > max_vox_prob)
        *PX_it = max_vox_prob;
    }
  }
  std::cout << "\ndone." << std::endl;

  if (return_prob) {
    // fill pixel_probabilities with preX_accum
    vil_image_view<float>::iterator pix_prob_it = pix_prob_density.begin();
    bvxm_voxel_slab<float>::const_iterator preX_accum_it = preX_accum.begin();

    for (; pix_prob_it != pix_prob_density.end(); ++pix_prob_it, ++preX_accum_it) {
      *pix_prob_it = *preX_accum_it;
    }
  }

  if (return_mask) {
    // fill mask values
    vil_image_view<bool>::iterator mask_it = mask.begin();
    bvxm_voxel_slab<float>::const_iterator mask_slab_it = mask_slab.begin();

    for (; mask_it != mask.end(); ++mask_it, ++mask_slab_it) {
      *mask_it = (*mask_slab_it > 0);
    }
  }

#ifdef DEBUG
  bvxm_util::write_slab_as_image(preX_accum,"prob.tiff");
#endif

  //Check:
  // increment the observation count
  //this->increment_observations<APM_T>(bin_index);

  return true;
}
#endif // 0

bool bvxm_voxel_world::update_edges_lidar(vil_image_view_base_sptr& lidar_height,
                                          vil_image_view_base_sptr& lidar_edges,
                                          vil_image_view_base_sptr& lidar_edges_prob,
                                          vpgl_camera_double_sptr& camera,
                                          unsigned scale)
{
  typedef bvxm_voxel_traits<EDGES>::voxel_datatype edges_datatype;

  //typedef bvxm_voxel_traits<LIDAR>::lidar_processor lidar_processor;
  bvxm_lidar_processor lidar_processor(10);

  // parameters
  vgl_vector_3d<unsigned int> grid_size = params_->num_voxels(scale);

  // compute homographies from voxel planes to image coordinates and vise-versa.
  std::vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  std::vector<vgl_h_matrix_2d<double> > H_img_to_plane;
  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    for (unsigned z=0; z < (unsigned)grid_size.z(); ++z)
    {
      compute_plane_image_H(camera,z,Hp2i,Hi2p,scale);
      H_plane_to_img.push_back(Hp2i);
      H_img_to_plane.push_back(Hi2p);
    }
  }

  // convert image to a voxel_slab
  bvxm_voxel_slab<float> lidar_height_slab(lidar_height->ni(), lidar_height->nj(), 1);
  bvxm_voxel_slab<float> lidar_edges_slab(lidar_edges->ni(), lidar_edges->nj(), 1);
  bvxm_voxel_slab<float> lidar_edges_prob_slab(lidar_edges_prob->ni(), lidar_edges_prob->nj(), 1);
  if ((!bvxm_util::img_to_slab(lidar_height,lidar_height_slab)) ||
      (!bvxm_util::img_to_slab(lidar_edges,lidar_edges_slab)) ||
      (!bvxm_util::img_to_slab(lidar_edges_prob,lidar_edges_prob_slab))) {
    std::cerr << "error converting image to voxel slab of observation type for bvxm_voxel_type: LIDAR\n";
    return false;
  }

  bvxm_voxel_slab<float> lidar_height_backproj(grid_size.x(),grid_size.y(),1);
  bvxm_voxel_slab<float> lidar_edges_backproj(grid_size.x(),grid_size.y(),1);
  bvxm_voxel_slab<float> lidar_edges_prob_backproj(grid_size.x(),grid_size.y(),1);

  // get edges probability grid
  bvxm_voxel_grid_base_sptr edges_grid_base = this->get_grid<EDGES>(0,scale);
  auto *edges_grid  = static_cast<bvxm_voxel_grid<edges_datatype>*>(edges_grid_base.ptr());

  bvxm_voxel_grid<edges_datatype>::iterator edges_slab_it = edges_grid->begin();

  for (unsigned k_idx=0; k_idx<(unsigned)grid_size.z(); ++k_idx, ++edges_slab_it)
  {
    std::cout << k_idx << std::endl;

    // backproject image onto voxel plane
    bvxm_util::warp_slab_bilinear(lidar_height_slab, H_plane_to_img[k_idx], lidar_height_backproj);
    bvxm_util::warp_slab_bilinear(lidar_edges_slab, H_plane_to_img[k_idx], lidar_edges_backproj);
    bvxm_util::warp_slab_bilinear(lidar_edges_prob_slab, H_plane_to_img[k_idx], lidar_edges_prob_backproj);

    bvxm_voxel_slab<float> lidar_prob(lidar_height_backproj.nx(), lidar_height_backproj.ny(), lidar_height_backproj.nz());
    lidar_prob.fill(0.0);

    vnl_vector_fixed<float,3> sigmas(0.5f,0.5f,0.0009f);
    vgl_point_3d<float> local_xyz = voxel_index_to_xyz(0, 0, k_idx,scale);

    for (unsigned i_idx=0; i_idx<lidar_height_backproj.nx(); i_idx++) {
      for (unsigned j_idx=0; j_idx<lidar_height_backproj.ny(); j_idx++) {
        std::vector<vgl_homg_point_2d<double> > vp(4);
        int i = i_idx+1;
        int j = j_idx-1;
        vp[0] = vgl_homg_point_2d<double>(i, j);
        vp[1] = vgl_homg_point_2d<double>(i+1, j);
        vp[2] = vgl_homg_point_2d<double>(i, j+1);
        vp[3] = vgl_homg_point_2d<double>(i+1, j+1);

        vgl_h_matrix_2d<double> h_max = H_plane_to_img[k_idx];
        vgl_h_matrix_2d<double> h_min;
        if (k_idx == (unsigned)grid_size.z()-1)
          h_min = H_plane_to_img[k_idx];
        else
          h_min = H_plane_to_img[k_idx+1];
        vgl_box_2d<double> lidar_roi;

        for (unsigned i=0; i<4; i++) {
          vgl_homg_point_2d<double> img_pos_h_min = h_min*vp[i];
          vgl_point_2d<double> img_pos_min(img_pos_h_min);
          lidar_roi.add(img_pos_min);
        }

        lidar_prob(i_idx, j_idx) = lidar_processor.prob_density(lidar_height, local_xyz.z(), sigmas, lidar_roi, params_->voxel_length(scale));
      }
    }

    bvxm_voxel_slab<float>::iterator lidar_prob_it = lidar_prob.begin();
    bvxm_voxel_slab<float>::iterator edges_slab_it_it = (*edges_slab_it).begin();

    bvxm_voxel_slab<float>::iterator lidar_edges_backproj_it = lidar_edges_backproj.begin();
    bvxm_voxel_slab<float>::iterator lidar_edges_prob_backproj_it = lidar_edges_prob_backproj.begin();

    for (; lidar_prob_it != lidar_prob.end(); ++lidar_prob_it, ++edges_slab_it_it, ++lidar_edges_backproj_it, ++lidar_edges_prob_backproj_it) {
      (*edges_slab_it_it) = 0.1f + 0.8f*(*lidar_prob_it)*(*lidar_edges_backproj_it);
    }
  }

  this->increment_observations<EDGES>(0,scale);

  return true;
}
//: generate a heightmap from the viewpoint of a virtual camera
// The pixel values are the z values of the most likely voxel intercepted by the corresponding camera ray
bool bvxm_voxel_world::heightmap(const vpgl_camera_double_sptr& virtual_camera, vil_image_view<unsigned> &heightmap, vil_image_view<float> &conf_map, unsigned scale_idx)
{
  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  // extract global parameters
  vgl_vector_3d<unsigned int> grid_size = params_->num_voxels(scale_idx);


  // compute homographies from voxel planes to image coordinates and vise-versa.
  std::vector<vgl_h_matrix_2d<double> > H_plane_to_virtual_img;
  std::vector<vgl_h_matrix_2d<double> > H_virtual_img_to_plane;

  for (unsigned z=0; z < (unsigned)grid_size.z(); ++z)
  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    compute_plane_image_H(virtual_camera,z,Hp2i,Hi2p,scale_idx);
    H_plane_to_virtual_img.push_back(Hp2i);
    H_virtual_img_to_plane.push_back(Hi2p);
  }

  // allocate some images
  bvxm_voxel_slab<float> visX_accum_virtual(heightmap.ni(), heightmap.nj(),1);
  bvxm_voxel_slab<float> heightmap_rough(heightmap.ni(),heightmap.nj(),1);
  bvxm_voxel_slab<float> max_prob_image(heightmap.ni(), heightmap.nj(), 1);
  bvxm_voxel_slab<ocp_datatype> slice_prob_img(heightmap.ni(),heightmap.nj(),1);

  heightmap_rough.fill((float)grid_size.z());
  visX_accum_virtual.fill(1.0f);
  max_prob_image.fill(0.0f);

  // get occupancy probability grid
  bvxm_voxel_grid_base_sptr ocp_grid_base = this->get_grid<OCCUPANCY>(0,scale_idx);
  auto *ocp_grid  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(ocp_grid_base.ptr());

  bvxm_voxel_grid<ocp_datatype>::const_iterator ocp_slab_it = ocp_grid->begin();

  std::cout << "generating height map from virtual camera:" << std::endl;
  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++ocp_slab_it) {
    std::cout << '.';

    // compute PXvisX for virtual camera and update visX
    bvxm_util::warp_slab_bilinear(*ocp_slab_it,H_virtual_img_to_plane[z],slice_prob_img);
    bvxm_voxel_slab<ocp_datatype>::const_iterator PX_it = slice_prob_img.begin();
    bvxm_voxel_slab<float>::iterator max_it = max_prob_image.begin(), visX_it = visX_accum_virtual.begin();
    bvxm_voxel_slab<float>::iterator hmap_it = heightmap_rough.begin();

    for (; hmap_it != heightmap_rough.end(); ++hmap_it, ++PX_it, ++max_it, ++visX_it) {
      float PXvisX = (*visX_it) * (*PX_it);
      //float PXvisX = *PX_it;
      if (PXvisX > *max_it) {
        *max_it = PXvisX;
        *hmap_it = (float)z;
      }
      // update virtual visX
      *visX_it *= (1.0f - *PX_it);
    }
  }
  std::cout << std::endl;

#define HMAP_DEBUG
#ifdef  DEBUG
  bvxm_util::write_slab_as_image(heightmap_rough,"./heightmap_rough.tiff");
#endif
  // now clean up height map

  // convert confidence and heightmap to vil images
  //vil_image_view<float>* conf_img = new vil_image_view<float>(heightmap.ni(),heightmap.nj());
  //vil_image_view_base_sptr conf_img_sptr = conf_img;
  vil_image_view_base_sptr conf_img_sptr = new vil_image_view<float>(conf_map);
  bvxm_util::slab_to_img(max_prob_image,conf_img_sptr);

  auto* heightmap_rough_img = new vil_image_view<float>(heightmap.ni(),heightmap.nj());
  vil_image_view_base_sptr heightmap_rough_img_sptr = heightmap_rough_img;
  bvxm_util::slab_to_img(heightmap_rough,heightmap_rough_img_sptr);
  /*
  // first, median filter heightmap
  vil_image_view<float> heightmap_med_img(heightmap.ni(),heightmap.nj());
  std::vector<int> strel_vec;
  for (int i=-medfilt_halfsize; i <= medfilt_halfsize; ++i)
    strel_vec.push_back(i);
  vil_structuring_element strel(strel_vec,strel_vec);
  vil_median(*heightmap_rough_img,heightmap_med_img,strel);

  // detect inliers as points which don't vary drastically from the median image
  vil_image_view<float> med_abs_diff(heightmap.ni(),heightmap.nj());
  vil_math_image_abs_difference(heightmap_med_img,*heightmap_rough_img,med_abs_diff);
  vil_image_view<bool> inliers(heightmap.ni(),heightmap.nj());
  vil_threshold_below(med_abs_diff,inliers,med_diff_thresh);

  std::cout << "smoothing height map: ";
  vil_image_view<float> heightmap_filtered_img(heightmap.ni(),heightmap.nj(),1);
  vil_image_view<bool> conf_mask(heightmap.ni(),heightmap.nj());
  // threshold confidence
  vil_threshold_above(*conf_img,conf_mask,conf_thresh);

#ifdef HMAP_DEBUG
  vil_save(*conf_img,"./heightmap_conf.tiff");
  vil_save(heightmap_med_img,"./heightmap_med.tiff");
#endif

  // initialize with rough heightmap
  vil_image_view<float>::const_iterator hmap_rough_it = heightmap_rough_img->begin();
  vil_image_view<float>::iterator hmap_filt_it = heightmap_filtered_img.begin();
  for (; hmap_filt_it != heightmap_filtered_img.end(); ++hmap_filt_it, ++hmap_rough_it) {
    *hmap_filt_it = (float)(*hmap_rough_it);
  }

  for (unsigned i=0; i< n_smooth_iterations; ++i) {
    std::cout << '.';
    // smooth heightmap
    vil_gauss_filter_2d(heightmap_filtered_img, heightmap_filtered_img, 1.0, 2, vil_convolve_constant_extend);
    // reset values we are confident in
    vil_image_view<bool>::const_iterator mask_it = conf_mask.begin(), inlier_it = inliers.begin();
    vil_image_view<float>::const_iterator hmap_med_it = heightmap_med_img.begin();
    hmap_filt_it = heightmap_filtered_img.begin();
    for (; hmap_filt_it != heightmap_filtered_img.end(); ++hmap_filt_it, ++hmap_med_it, ++mask_it, ++inlier_it) {
      if (*mask_it && *inlier_it) {
        *hmap_filt_it = (float)(*hmap_med_it);
      }
    }
  }
  std::cout << std::endl;

  // finally, median filter final heightmap
  vil_image_view<float> heightmap_filtered_med(heightmap.ni(),heightmap.nj());
  vil_median(heightmap_filtered_img,heightmap_filtered_med,strel);

#ifdef HMAP_DEBUG
  vil_save(heightmap_filtered_med,"./heightmap_filtered.tiff");
#endif

  // convert back to unsigned
  vil_image_view<unsigned>::iterator hmap_it = heightmap.begin();
  hmap_filt_it = heightmap_filtered_med.begin();
  for (; hmap_it != heightmap.end(); ++hmap_filt_it, ++hmap_it) {
    *hmap_it = (unsigned)(*hmap_filt_it); // should we do some rounding here?
  }
  */

  // convert back to unsigned
  vil_image_view<float>::const_iterator hmap_rough_it = heightmap_rough_img->begin();
  vil_image_view<unsigned>::iterator hmap_it = heightmap.begin();
  for (; hmap_rough_it != heightmap_rough_img->end(); ++hmap_it, ++hmap_rough_it) {
    *hmap_it = (unsigned)(*hmap_rough_it);
  }
  return true;
}

//: generate a heightmap from the viewpoint of a virtual camera
// The pixel values are the expected z values and variance along the corresponding camera ray
bool bvxm_voxel_world::heightmap_exp(const vpgl_camera_double_sptr& virtual_camera, vil_image_view<float> &heightmap, vil_image_view<float> &var, float& max_depth, unsigned scale_idx)
{
  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  // extract global parameters
  vgl_vector_3d<unsigned int> grid_size = params_->num_voxels(scale_idx);

  // compute homographies from voxel planes to image coordinates and vise-versa.
  std::vector<vgl_h_matrix_2d<double> > H_plane_to_virtual_img;
  std::vector<vgl_h_matrix_2d<double> > H_virtual_img_to_plane;

  for (unsigned z=0; z < (unsigned)grid_size.z(); ++z)
  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    compute_plane_image_H(virtual_camera,z,Hp2i,Hi2p,scale_idx);
    H_plane_to_virtual_img.push_back(Hp2i);
    H_virtual_img_to_plane.push_back(Hi2p);
  }

  // compute the height for initial pre_x, pre_y, pre_z
  bvxm_voxel_slab<float> x_slab(grid_size.x(),grid_size.y(),1);
  bvxm_voxel_slab<float> y_slab(grid_size.x(),grid_size.y(),1);
  bvxm_voxel_slab<float> z_slab(grid_size.x(),grid_size.y(),1);

  for (unsigned i = 0; i < grid_size.x(); i++)
    for (unsigned j = 0; j < grid_size.y(); j++) {
      vgl_point_3d<float> pt = this->voxel_index_to_xyz(i, j, -1, 0);
      x_slab(i, j) = pt.x();
      y_slab(i, j) = pt.y();
      z_slab(i, j) = pt.z();
    }

  vgl_h_matrix_2d<double> Hp2i, Hi2p;
  compute_plane_image_H(virtual_camera,-1,Hp2i,Hi2p,scale_idx);  // compute for z = -1 (slab above the world volume)

  // allocate some images
  bvxm_voxel_slab<float> visX_accum_virtual(heightmap.ni(), heightmap.nj(),1);
  bvxm_voxel_slab<float> depth(heightmap.ni(),heightmap.nj(),1);
  bvxm_voxel_slab<float> exp_depth(heightmap.ni(),heightmap.nj(),1);
  bvxm_voxel_slab<float> exp_depth_square(heightmap.ni(),heightmap.nj(),1);
  bvxm_voxel_slab<ocp_datatype> slice_prob_img(heightmap.ni(),heightmap.nj(),1);

  bvxm_voxel_slab<float> slab_x_virtual(heightmap.ni(), heightmap.nj(), 1);
  bvxm_voxel_slab<float> slab_y_virtual(heightmap.ni(), heightmap.nj(), 1);
  bvxm_voxel_slab<float> slab_z_virtual(heightmap.ni(), heightmap.nj(), 1);
  bvxm_voxel_slab<float> slab_x_pre_virtual(heightmap.ni(), heightmap.nj(), 1);
  bvxm_voxel_slab<float> slab_y_pre_virtual(heightmap.ni(), heightmap.nj(), 1);
  bvxm_voxel_slab<float> slab_z_pre_virtual(heightmap.ni(), heightmap.nj(), 1);

  // initialize the pres using z = -1 height
  bvxm_util::warp_slab_bilinear(x_slab,Hi2p,slab_x_pre_virtual);
  bvxm_util::warp_slab_bilinear(y_slab,Hi2p,slab_y_pre_virtual);
  bvxm_util::warp_slab_bilinear(z_slab,Hi2p,slab_z_pre_virtual);

  //heightmap_rough.fill((float)grid_size.z());
  visX_accum_virtual.fill(1.0f);
  depth.fill(0.0f);  // depth from the ceiling of the scene! so start with 0
  exp_depth.fill(0.0f);
  exp_depth_square.fill(0.0f);

  // get occupancy probability grid
  bvxm_voxel_grid_base_sptr ocp_grid_base = this->get_grid<OCCUPANCY>(0,scale_idx);
  auto *ocp_grid  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(ocp_grid_base.ptr());

  bvxm_voxel_grid<ocp_datatype>::const_iterator ocp_slab_it = ocp_grid->begin();

  std::cout << "generating depth map from virtual camera:" << std::endl;
  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++ocp_slab_it) {
    std::cout << '.';

    // compute PXvisX for virtual camera and update visX
    bvxm_util::warp_slab_bilinear(*ocp_slab_it,H_virtual_img_to_plane[z],slice_prob_img);

    // compute the current x,y,z
    for (unsigned i = 0; i < grid_size.x(); i++)
    for (unsigned j = 0; j < grid_size.y(); j++) {
      vgl_point_3d<float> pt = this->voxel_index_to_xyz(i, j, z, 0);
      x_slab(i, j) = pt.x();
      y_slab(i, j) = pt.y();
      z_slab(i, j) = pt.z();
    }
    bvxm_util::warp_slab_bilinear(x_slab,Hi2p,slab_x_virtual);
    bvxm_util::warp_slab_bilinear(y_slab,Hi2p,slab_y_virtual);
    bvxm_util::warp_slab_bilinear(z_slab,Hi2p,slab_z_virtual);


    // compute the current depths
    bvxm_voxel_slab<float>::iterator depth_it = depth.begin(), x_it = slab_x_virtual.begin(), y_it = slab_y_virtual.begin(), z_it = slab_z_virtual.begin();
    bvxm_voxel_slab<float>::iterator x_pre_it = slab_x_pre_virtual.begin(), y_pre_it = slab_y_pre_virtual.begin(), z_pre_it = slab_z_pre_virtual.begin();
    for (; depth_it != depth.end(); ++depth_it, ++x_it, ++y_it, ++z_it, ++x_pre_it, ++y_pre_it, ++z_pre_it) {
      float inc_x = *x_pre_it - *x_it;
      float inc_y = *y_pre_it - *y_it;
      float inc_z = *z_pre_it - *z_it;
      float d = std::sqrt(inc_x*inc_x + inc_y*inc_y + inc_z*inc_z);
      *depth_it += d;
      *x_pre_it = *x_it;
      *y_pre_it = *y_it;
      *z_pre_it = *z_it;
    }

    bvxm_voxel_slab<ocp_datatype>::const_iterator PX_it = slice_prob_img.begin();
    bvxm_voxel_slab<float>::iterator exp_depth_it = exp_depth.begin(), exp_depth_square_it = exp_depth_square.begin(), visX_it = visX_accum_virtual.begin();
    depth_it = depth.begin();

    for (; exp_depth_it != exp_depth.end(); ++exp_depth_it, ++PX_it, ++exp_depth_square_it, ++visX_it, ++depth_it) {
      float PXvisX = (*visX_it) * (*PX_it);
      *exp_depth_it += *depth_it * PXvisX;
      *exp_depth_square_it += *depth_it * *depth_it * PXvisX;

      // update virtual visX
      *visX_it *= (1.0f - *PX_it);
    }
  }
  std::cout << std::endl;

  // find the depth at the slab below the world volume
  for (unsigned i = 0; i < grid_size.x(); i++)
    for (unsigned j = 0; j < grid_size.y(); j++) {
      vgl_point_3d<float> pt = this->voxel_index_to_xyz(i, j, grid_size.z(), 0);
      x_slab(i, j) = pt.x();
      y_slab(i, j) = pt.y();
      z_slab(i, j) = pt.z();
    }

  bvxm_util::warp_slab_bilinear(x_slab,Hi2p,slab_x_virtual);
  bvxm_util::warp_slab_bilinear(y_slab,Hi2p,slab_y_virtual);
  bvxm_util::warp_slab_bilinear(z_slab,Hi2p,slab_z_virtual);

  // compute the current depths
  bvxm_voxel_slab<float>::iterator depth_it = depth.begin(), x_it = slab_x_virtual.begin(), y_it = slab_y_virtual.begin(), z_it = slab_z_virtual.begin();
  bvxm_voxel_slab<float>::iterator x_pre_it = slab_x_pre_virtual.begin(), y_pre_it = slab_y_pre_virtual.begin(), z_pre_it = slab_z_pre_virtual.begin();
  for (; depth_it != depth.end(); ++depth_it, ++x_it, ++y_it, ++z_it, ++x_pre_it, ++y_pre_it, ++z_pre_it) {
    float inc_x = *x_pre_it - *x_it;
    float inc_y = *y_pre_it - *y_it;
    float inc_z = *z_pre_it - *z_it;
    float d = std::sqrt(inc_x*inc_x + inc_y*inc_y + inc_z*inc_z);
    *depth_it += d;
  }

  // normalize the depths, the visibility at the end of the ray may not have diminished completely
  bvxm_voxel_slab<float>::iterator exp_depth_it = exp_depth.begin(), exp_depth_square_it = exp_depth_square.begin(), visX_it = visX_accum_virtual.begin();
  depth_it = depth.begin();
  vil_image_view<float>::iterator hmap_it = heightmap.begin(), var_it = var.begin();
  for (; exp_depth_it != exp_depth.end(); ++exp_depth_it, ++exp_depth_square_it, ++depth_it, ++visX_it, ++hmap_it, ++var_it) {
    *hmap_it = *exp_depth_it + *depth_it * *visX_it;
    *var_it = *exp_depth_square_it + *depth_it * *depth_it * *visX_it - (*exp_depth_it * *exp_depth_it);
  }
  // return the max depth for the ray (0,0)
  max_depth = *(depth.begin());

  return true;
}

//: measure the average uncertainty along the rays
bool bvxm_voxel_world::uncertainty(const vpgl_camera_double_sptr& virtual_camera, vil_image_view<float> &uncertainty, unsigned scale_idx)
{
  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  // extract global parameters
  vgl_vector_3d<unsigned int> grid_size = params_->num_voxels(scale_idx);

  // compute homographies from voxel planes to image coordinates and vise-versa.
  std::vector<vgl_h_matrix_2d<double> > H_plane_to_virtual_img;
  std::vector<vgl_h_matrix_2d<double> > H_virtual_img_to_plane;

  for (unsigned z=0; z < (unsigned)grid_size.z(); ++z)
  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    compute_plane_image_H(virtual_camera,z,Hp2i,Hi2p,scale_idx);
    H_plane_to_virtual_img.push_back(Hp2i);
    H_virtual_img_to_plane.push_back(Hi2p);
  }

  // allocate some images
  bvxm_voxel_slab<float> visX_accum_virtual(uncertainty.ni(), uncertainty.nj(),1);
  bvxm_voxel_slab<float> uncer(uncertainty.ni(),uncertainty.nj(),1);
  bvxm_voxel_slab<float> uncer_cnt(uncertainty.ni(),uncertainty.nj(),1);
  bvxm_voxel_slab<ocp_datatype> slice_prob_img(uncertainty.ni(),uncertainty.nj(),1);

  visX_accum_virtual.fill(1.0f);
  uncer.fill(0.0f);  // depth from the ceiling of the scene! so start with 0
  uncer_cnt.fill(0.0f);

  // get occupancy probability grid
  bvxm_voxel_grid_base_sptr ocp_grid_base = this->get_grid<OCCUPANCY>(0,scale_idx);
  auto *ocp_grid  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(ocp_grid_base.ptr());

  bvxm_voxel_grid<ocp_datatype>::const_iterator ocp_slab_it = ocp_grid->begin();

  std::cout << "generating depth map from virtual camera:" << std::endl;
  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++ocp_slab_it) {
    std::cout << '.';

    // compute PXvisX for virtual camera and update visX
    bvxm_util::warp_slab_bilinear(*ocp_slab_it,H_virtual_img_to_plane[z],slice_prob_img);

    bvxm_voxel_slab<ocp_datatype>::const_iterator PX_it = slice_prob_img.begin();
    bvxm_voxel_slab<float>::iterator uncer_it = uncer.begin(), uncer_cnt_it = uncer_cnt.begin(), visX_it = visX_accum_virtual.begin();
    for (; uncer_it != uncer.end(); ++uncer_it, ++uncer_cnt_it, ++PX_it, ++visX_it) {
      float PX = (*PX_it);
      float PXminus = 1.0f-(*PX_it);
      float ratio = PX/PXminus;
      if (ratio > 1)
        ratio = 1.0f/ratio;
      //*uncer_it += ratio;
      //*uncer_cnt_it += 1.0f;
      *uncer_it += *visX_it * ratio;
      *uncer_cnt_it += *visX_it;

      // update virtual visX
      *visX_it *= (1.0f - *PX_it);
    }
  }
  std::cout << std::endl;

  // compute the average
  bvxm_voxel_slab<float>::iterator uncer_it = uncer.begin(), uncer_cnt_it = uncer_cnt.begin();
  vil_image_view<float>::iterator uncer_img_it = uncertainty.begin();
  for (; uncer_it != uncer.end(); ++uncer_it, ++uncer_cnt_it, ++uncer_img_it) {
    *uncer_img_it = *uncer_it / *uncer_cnt_it;
  }

  return true;
}

vgl_point_3d<float> bvxm_voxel_world::voxel_index_to_xyz(unsigned vox_i, unsigned vox_j, int vox_k, unsigned scale_idx)
{
  float vox_len = params_->voxel_length(scale_idx);
  vgl_vector_3d<unsigned> num_vox = params_->num_voxels(scale_idx);

  // corner in parameters refers to the bottom. we want the top since slice 0 is the top-most slice.
  vgl_point_3d<float> grid_origin = params_->corner() + (vgl_vector_3d<float>(0.5f, 0.5f, (float(num_vox.z()) - 0.5f))*vox_len);
  vgl_vector_3d<float> step_i = vox_len*params_->base_x();
  vgl_vector_3d<float> step_j = vox_len*params_->base_y();
  vgl_vector_3d<float> step_k = (-vox_len)*params_->base_z();

  return grid_origin + step_i*vox_i + step_j*vox_j + step_k*vox_k;
}

void bvxm_voxel_world::compute_plane_image_H(vpgl_camera_double_sptr const& cam, int k_idx, vgl_h_matrix_2d<double> &H_plane_to_image, vgl_h_matrix_2d<double> &H_image_to_plane, unsigned scale_idx)
{
  vgl_vector_3d<unsigned int> grid_size = params_->num_voxels(scale_idx);

  std::vector<vgl_homg_point_2d<double> > voxel_corners_img;
  std::vector<vgl_homg_point_2d<double> > voxel_corners_vox;

  // create vectors containing four corners of grid, and their projections into the image
  double u=0, v=0;
  vgl_point_3d<float> corner_world;

  voxel_corners_vox.emplace_back(0,0);
  corner_world = this->voxel_index_to_xyz(0,0,k_idx,scale_idx);
  cam->project(corner_world.x(),corner_world.y(),corner_world.z(),u,v);
  voxel_corners_img.emplace_back(u,v);

  voxel_corners_vox.emplace_back(grid_size.x()-1,0);
  corner_world = this->voxel_index_to_xyz(grid_size.x()-1,0,k_idx,scale_idx);
  cam->project(corner_world.x(),corner_world.y(),corner_world.z(),u,v);
  voxel_corners_img.emplace_back(u,v);

  voxel_corners_vox.emplace_back(grid_size.x()-1,grid_size.y()-1);
  corner_world = this->voxel_index_to_xyz(grid_size.x()-1,grid_size.y()-1,k_idx,scale_idx);
  cam->project(corner_world.x(),corner_world.y(),corner_world.z(),u,v);
  voxel_corners_img.emplace_back(u,v);

  voxel_corners_vox.emplace_back(0,(grid_size.y()-1));
  corner_world = this->voxel_index_to_xyz(0,grid_size.y()-1,k_idx,scale_idx);
  cam->project(corner_world.x(),corner_world.y(),corner_world.z(),u,v);
  voxel_corners_img.emplace_back(u,v);


  vgl_h_matrix_2d_compute_linear comp_4pt;
  if (!comp_4pt.compute(voxel_corners_img,voxel_corners_vox, H_image_to_plane)) {
    std::cerr << "ERROR computing homography from image to voxel slice.\n";
    for (const auto & i : voxel_corners_img)
      std::cerr << i << std::endl;
    for (const auto & i : voxel_corners_vox)
      std::cerr << i << std::endl;
  }
  if (!comp_4pt.compute(voxel_corners_vox,voxel_corners_img, H_plane_to_image)) {
    std::cerr << "ERROR computing homography from voxel slice to image.\n";
  }
  return;
}
