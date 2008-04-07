// This is brl/bseg/bvxm/bvxm_voxel_world.h
#ifndef bvxm_voxel_world_h_
#define bvxm_voxel_world_h_
//:
// \file
// \brief A representation of the world using voxel grids of occupancy probability and an appearance model.
// \author Daniel Crispell (dec@lems.brown.edu)
// \date 1/22/2008
//
// The world has the ability to store voxel grids of any type defined in bvxm_voxel_traits.h.
// These grids are accessable through the get_grid() method.
// A voxel_type which is an appearance model type must have an associated appearance model processor class.
// The appearance model processor type needs to have the following methods:
//
//
//   bvxm_voxel_slab<float> prob_density(bvxm_voxel_slab<APM_PROC::apm_datatype> const& appearance,
//                                       bvxm_voxel_slab<APM_PROC::obs_datatype> const& observation);
//
//   bvxm_voxel_slab<float> prob_range(bvxm_voxel_slab<APM_PROC::apm_datatype> const& appearance,
//                                     bvxm_voxel_slab<APM_PROC::obs_datatype> const& observation,
//                                     bvxm_voxel_slab<float> pix_range);
//
//   bool update(bvxm_voxel_slab<APM_PROC::apm_datatype> appearance,
//               bvxm_voxel_slab<APM_PROC::obs_datatype> const& observation,
//               bvxm_voxel_slab<float> const& weights);
//
//   bvxm_voxel_slab<APM_PROC::obs_datatype> expected_color(bvxm_voxel_slab<APM_PROC::apm_datatype> const& appearance);
//
// \verbatim
//  Modifications:
//   Ozge C Ozcanli - 2/20/2008 - added  the method:
//           bool mixture_of_gaussians_image(bvxm_image_metadata const& camera,
//                                           bvxm_voxel_slab<apm_datatype> &mog_image);
//
//   Isabel Restrepo - 2/23/2008
//               -Changed class to support different VOXEL_GRID_TYPES simultaneously.
//                 Thus, the calss is not templated anymore but rather the indiviual functions.
//               - There is no need for bvxm_world_base, hence this class is not subclassed form it anymore
//               - Subclassed form vbl_ref_count
//
//   Ozge C Ozcanli - 2/27/2008 - made get_grid method public
//
//   Ibrahim Eden - 03/06/2008 - added the method:
//           bool expected_edge_image(bvxm_image_metadata const& camera,vil_image_view_base_sptr &expected);
//
//   Ibrahim Eden - 03/07/2008 - added the method:
//           bool update_edges(bvxm_image_metadata const& metadata);
//
//   Ibrahim Eden - 03/28/2008 - added the method:
//           bool save_edges_raw(vcl_string filename);
// \endverbatim
//
////////////////////////////////////////////////////////////////////////////////

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_ref_count.h>

#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>

#include <vil/vil_image_view.h>
#include <vpgl/vpgl_camera.h>

#include "bvxm_image_metadata.h"
#include "bvxm_mog_grey_processor.h"
#include "bvxm_voxel_grid.h"
#include "bvxm_voxel_traits.h"
#include "bvxm_world_params.h"
#include "bvxm_util.h"

// These includes are for the implementations of the templated methods,
// which should be moved from the header file if possible.
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_utility.h>
#include <vil/vil_image_view_base.h>
#include <vul/vul_file_iterator.h>

#define debug false

class bvxm_voxel_world: public vbl_ref_count
{
 public:

  //: default constructor
  bvxm_voxel_world() {}

  //: construct world with parameters
  bvxm_voxel_world(bvxm_world_params_sptr params) { params_ = params; }

  //: destructor
  ~bvxm_voxel_world();

  //: update voxel grid with data from image/camera pair. Based on algorithm published in Pollard + Mundy 06.
  template<bvxm_voxel_type APM_T>
  bool update(bvxm_image_metadata const& observation, unsigned bin_index = 0);

  //: update voxel grid wtih data from image/camera pair and return pixel probability densities.
  // Based on algorithm published in Pollard and Mundy 06.
  // The probability density of observing each pixel in the observation is returned in pixel_prob_density, which should be allocated by the caller.
  template<bvxm_voxel_type APM_T>
  bool update(bvxm_image_metadata const& observation, vil_image_view<float> &pixel_prob_density, vil_image_view<bool> &mask, unsigned bin_index = 0);
  
  //: update voxel grid wtih data from LIDAR/camera pair                   
  bool update_lidar(bvxm_image_metadata const& observation);
 
  //: update voxel grid wtih data from LIDAR/camera pair and return pixel probability densities.
  bool update_lidar(bvxm_image_metadata const& observation, vil_image_view<float> &pixel_prob_density, vil_image_view<bool> &mask);

  //: generate the expected image from the specified viewpoint. the expected image and mask should be allocated by the caller.
  template<bvxm_voxel_type APM_T>
  bool expected_image(bvxm_image_metadata const& camera,
                      vil_image_view_base_sptr &expected,
                      vil_image_view<float> &mask, unsigned bin_index = 0);
                      
  //: update voxel grid for edges with data from image/camera pair and return the edge probability density of pixel values
  bool update_edges(bvxm_image_metadata const& metadata);

  //: generate the expected edge image from the specified viewpoint. the expected image should be allocated by the caller.
  bool expected_edge_image(bvxm_image_metadata const& camera,vil_image_view_base_sptr &expected);

  //: probability that the observed pixels were _not_ produced by a voxel in the grid.
  // The range determines how much tolerance to allow the (continuous) pixel values in order to convert from a density to a discrete probability.
  // Default value of 0.008 is approximately two 8-bit levels in either direction (assuming intensity is normalized 0-1)
  template<bvxm_voxel_type APM_T>
  bool inv_pixel_range_probability(bvxm_image_metadata const& observation,
                                   vil_image_view<float> &inv_prob,
                                   unsigned bin_index = 0,
                                   float pixel_range = 0.008f);

  //: for each pixel, return the sum along the corresponding ray of voxels that the observation was produced by the voxel.
  // Based on algorithm published in Pollard + Mundy 06.
  // The returned values are approximate samples of a probability density, with the pixel values being the independent value.
  template<bvxm_voxel_type APM_T>
  bool pixel_probability_density(bvxm_image_metadata const& observation,
                                 vil_image_view<float> &pixel_probability,
                                 unsigned bin_index = 0);

  //: generate the mixture of gaussians slab from the specified viewpoint. the slab should be allocated by the caller.
  template<bvxm_voxel_type APM_T>
  bool mixture_of_gaussians_image(bvxm_image_metadata const& camera,
                                  bvxm_voxel_slab_base_sptr& mog_image,
                                  unsigned bin_index = 0);

  //: return the original image, viewed from a new viewpoint
  template<bvxm_voxel_type APM_T>
  bool virtual_view(bvxm_image_metadata const& original_view,
                    const vpgl_camera_double_sptr virtual_camera,
                    vil_image_view_base_sptr &virtual_view,
                    vil_image_view<float> &vis_prob, unsigned bin_index = 0);

  //: return a planar approximation to the world
  vgl_plane_3d<double> fit_plane();

  //: get the world parameters
  bvxm_world_params_sptr get_params() const { return params_; }

  //: set the world parameters
  void set_params(bvxm_world_params_sptr params){ params_ = params;}

  // === Operators that allow voxel world to be placed in a brdb database ===

  //: equality operator
  bool operator == (bvxm_voxel_world const& that) const;

  //: less than operator
  bool operator < (bvxm_voxel_world  const& that) const;

  //: get a grid from the map: creates a new one if nothing exists at the specified index.
  template<bvxm_voxel_type VOX_T>
  bvxm_voxel_grid_base_sptr get_grid(unsigned bin_index);

  //: save the edge probability grid in a ".raw" format readable by Drishti volume rendering software
  bool save_edges_raw(vcl_string filename);

  //: save the occupancy grid in a ".raw" format readable by Drishti volume rendering software
  bool save_occupancy_raw(vcl_string filename);

  //: remove all voxel data from disk - use with caution!
  bool clean_grids();

  //: get the observation count of a voxel type at a specific bin
  template<bvxm_voxel_type VOX_T>
  unsigned int num_observations(unsigned int bin_idx = 0);

  //: increment the observation count of a voxel type at a specific bin
  template<bvxm_voxel_type VOX_T>
  void increment_observations( unsigned int bin_idx = 0);

 protected:

  //: appearance model voxel storage
  vcl_map<bvxm_voxel_type, vcl_map<unsigned int, bvxm_voxel_grid_base_sptr> > grid_map_;

  //: the world parameters
  bvxm_world_params_sptr params_;

 private:
  template <bvxm_voxel_type APM_T>
  bool update_impl(bvxm_image_metadata const& metadata,
                   bool return_prob, vil_image_view<float> &pix_prob_density,
                   bool return_mask, vil_image_view<bool> &mask, unsigned bin_index);

  
// Update voxel grid with data from LIDAR image/camera pair and return probability density of pixel values.
 bool bvxm_voxel_world::update_lidar_impl(bvxm_image_metadata const& metadata,
                                   bool return_prob,
                                   vil_image_view<float> &pix_prob_density,
                                   bool return_mask,
                                   vil_image_view<bool> &mask);
};


//: output description of voxel world to stream.
vcl_ostream&  operator<<(vcl_ostream& s, bvxm_voxel_world const& vox_world);


typedef vbl_smart_ptr<bvxm_voxel_world> bvxm_voxel_world_sptr;

//////////////////////////////////////////////////////////
// TODO: Move everything below here to .txx files -DEC
//////////////////////////////////////////////////////////

//: get the observation count of a voxel type at a specific bin
template<bvxm_voxel_type VOX_T>
unsigned int bvxm_voxel_world::num_observations(unsigned int bin_idx)
{
  // call get_grid so data will be loaded from disk if necessary.
  typedef typename bvxm_voxel_traits<VOX_T>::voxel_datatype vox_datatype;
  bvxm_voxel_grid<vox_datatype> *grid = static_cast<bvxm_voxel_grid<vox_datatype>*>(this->get_grid<VOX_T>(bin_idx).ptr());
  return grid->num_observations();
}


//: increment the observation count of a voxel type at a specific bin
template<bvxm_voxel_type VOX_T>
void bvxm_voxel_world::increment_observations(unsigned int bin_idx)
{
  // call get_grid so data will be loaded from disk if necessary.
  typedef typename bvxm_voxel_traits<VOX_T>::voxel_datatype vox_datatype;
  bvxm_voxel_grid<vox_datatype> *grid = static_cast<bvxm_voxel_grid<vox_datatype>*>(this->get_grid<VOX_T>(bin_idx).ptr());
  grid->increment_observations();
}


//: Returns the voxel_grid that corresponds to a given bvxm_voxel_type and a bin number
template<bvxm_voxel_type VOX_T>
bvxm_voxel_grid_base_sptr bvxm_voxel_world::get_grid(unsigned bin_index)
{
  //retrieve map for current bvxm_voxel_type
  //if no map found create a new one
  if (grid_map_.find(VOX_T) == grid_map_.end())
  {
    //create map
    vcl_map<unsigned, bvxm_voxel_grid_base_sptr> bin_map;

    // look for existing appearance model grids in the directory
    vgl_vector_3d<unsigned int> grid_size = params_->num_voxels();
    vcl_string storage_directory = params_->model_dir();

    vcl_stringstream grid_glob;
    vcl_string fname_prefix = bvxm_voxel_traits<VOX_T>::filename_prefix();
    grid_glob << storage_directory << '/' << fname_prefix << "*.vox";

    //insert grids
    for (vul_file_iterator file_it = grid_glob.str().c_str(); file_it; ++file_it) {
      vcl_string match_str = file_it.filename();
      unsigned idx_start = match_str.find_last_of('_') + 1;
      unsigned idx_end = match_str.find(".vox");
      vcl_stringstream idx_str;
      idx_str << match_str.substr(idx_start,idx_end - idx_start);
      int idx = -1;
      idx_str >> idx;
      if (idx < 0) {
        vcl_cerr << "error parsing filename " << file_it() << vcl_endl;
      } else {
        // create voxel grid and insert into map
        bvxm_voxel_grid_base_sptr grid = new bvxm_voxel_grid<typename bvxm_voxel_traits<VOX_T>::voxel_datatype>(file_it(),grid_size);
        bin_map.insert(vcl_make_pair((unsigned)idx, grid));
      }
    }

    grid_map_.insert(vcl_make_pair(VOX_T, bin_map));
  }

  //retrieve map containing voxel_grid
  vcl_map<unsigned, bvxm_voxel_grid_base_sptr> voxel_map = grid_map_[VOX_T];

  //retrieve voxel_grid for current bin
  //if no grid exists at bin location create one filled with default values
  if (voxel_map.find(bin_index) == voxel_map.end())
  {
    vgl_vector_3d<unsigned int> grid_size = params_->num_voxels();
    vcl_string storage_directory = params_->model_dir();

    vcl_stringstream apm_fname;
    vcl_string fname_prefix = bvxm_voxel_traits<VOX_T>::filename_prefix();
    apm_fname << storage_directory << '/' << fname_prefix << '_' << bin_index << ".vox";

    typedef typename bvxm_voxel_traits<VOX_T>::voxel_datatype voxel_datatype;
    bvxm_voxel_grid<voxel_datatype> *grid = new bvxm_voxel_grid<voxel_datatype>(apm_fname.str(),grid_size);

    // fill grid with default value
    if (!grid->initialize_data(bvxm_voxel_traits<VOX_T>::initial_val())){
      vcl_cerr << "error initializing voxel grid\n";
      return bvxm_voxel_grid_base_sptr(0);
    }

    //Insert voxel grid into map
    bvxm_voxel_grid_base_sptr grid_sptr = grid;
    grid_map_[VOX_T].insert(vcl_make_pair(bin_index, grid_sptr));
  }

  return grid_map_[VOX_T][bin_index];
}


// Update a voxel grid with data from image/camera pair
template <bvxm_voxel_type APM_T>
bool bvxm_voxel_world::update(bvxm_image_metadata const& observation, unsigned bin_index)
{
  vil_image_view<float> dummy;
  vil_image_view<bool> mask;
  return this->update_impl<APM_T>(observation, false, dummy, false, mask, bin_index);
}


// Update a voxel grid with data from image/camera pair and return probability density of pixel values.
template<bvxm_voxel_type APM_T>
bool bvxm_voxel_world::update(bvxm_image_metadata const& observation,
                              vil_image_view<float> &pix_prob_density, vil_image_view<bool> &mask, unsigned bin_index)
{
  // check image sizes
  if ( (observation.img->ni() != pix_prob_density.ni()) || (observation.img->nj() != pix_prob_density.nj()) ) {
    vcl_cerr << "error: metadata image size does not match probability image size.\n";
  }
  if ( (observation.img->ni() != mask.ni()) || (observation.img->nj() != mask.nj()) ) {
    vcl_cerr << "error: metadata image size does not match mask image size.\n";
  }
  return this->update_impl<APM_T>(observation, true, pix_prob_density, true, mask, bin_index);
}


// Update voxel grid with data from image/camera pair and return probability density of pixel values.
template<bvxm_voxel_type APM_T>
bool bvxm_voxel_world::update_impl(bvxm_image_metadata const& metadata,
                                   bool return_prob,
                                   vil_image_view<float> &pix_prob_density,
                                   bool return_mask,
                                   vil_image_view<bool> &mask, unsigned bin_index)
{
  // datatype for current appearance model
  typedef typename bvxm_voxel_traits<APM_T>::voxel_datatype apm_datatype;
  // datatype of the pixels that the processor operates on.
  typedef typename bvxm_voxel_traits<APM_T>::obs_datatype obs_datatype;
  typedef typename bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  // the appearance model processor
  typename bvxm_voxel_traits<APM_T>::appearance_processor apm_processor;

  vgl_vector_3d<unsigned int> grid_size = params_->num_voxels();
  ocp_datatype min_vox_prob = params_->min_occupancy_prob();
  ocp_datatype max_vox_prob = params_->max_occupancy_prob();

  // compute homographies from voxel planes to image coordinates and vise-versa.
  vcl_vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  vcl_vector<vgl_h_matrix_2d<double> > H_img_to_plane;
  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    for (unsigned z=0; z < (unsigned)grid_size.z(); ++z)
    {
      bvxm_util::compute_plane_image_H(metadata.camera,params_,z,Hp2i,Hi2p);
      H_plane_to_img.push_back(Hp2i);
      H_img_to_plane.push_back(Hi2p);
    }
  }

  // convert image to a voxel_slab
  bvxm_voxel_slab<obs_datatype> image_slab(metadata.img->ni(), metadata.img->nj(), 1);
  if (!bvxm_util::img_to_slab(metadata.img,image_slab)) {
    vcl_cerr << "error converting image to voxel slab of observation type for bvxm_voxel_type:" << APM_T << vcl_endl;
    return false;
  }

  // temporary voxel grids to hold preX and PI*visX values
  bvxm_voxel_grid<float> preX(grid_size);
  bvxm_voxel_grid<float> PIvisX(grid_size);

  bvxm_voxel_slab<float> PIPX(grid_size.x(),grid_size.y(),1);
  bvxm_voxel_slab<float> PXvisX(grid_size.x(), grid_size.y(),1);

  bvxm_voxel_slab<float> preX_accum(image_slab.nx(),image_slab.ny(),1);
  bvxm_voxel_slab<float> visX_accum(image_slab.nx(),image_slab.ny(),1);
  bvxm_voxel_slab<float> img_scratch(image_slab.nx(),image_slab.ny(),1);
  bvxm_voxel_slab<float> PIPX_img(image_slab.nx(), image_slab.ny(),1);
  bvxm_voxel_slab<float> PX_img(image_slab.nx(), image_slab.ny(),1);
  bvxm_voxel_slab<float> mask_slab(image_slab.nx(), image_slab.ny(),1);

  preX_accum.fill(0.0f);
  visX_accum.fill(1.0f);

  // slabs for holding backprojections of visX
  bvxm_voxel_slab<float> visX(grid_size.x(),grid_size.y(),1);

  bvxm_voxel_slab<obs_datatype> frame_backproj(grid_size.x(),grid_size.y(),1);

  vcl_cout << "Pass 1: " << vcl_endl;

  // get ocuppancy probability grid
  bvxm_voxel_grid_base_sptr ocp_grid_base = this->get_grid<OCCUPANCY>(0);
  bvxm_voxel_grid<ocp_datatype> *ocp_grid  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(ocp_grid_base.ptr());

  //get appereance model grid
  bvxm_voxel_grid_base_sptr apm_grid_base = this->get_grid<APM_T>(bin_index);
  bvxm_voxel_grid<apm_datatype> *apm_grid  = static_cast<bvxm_voxel_grid<apm_datatype>*>(apm_grid_base.ptr());

  typename bvxm_voxel_grid<ocp_datatype>::const_iterator ocp_slab_it = ocp_grid->begin();
  typename bvxm_voxel_grid<apm_datatype>::iterator apm_slab_it = apm_grid->begin();
  typename bvxm_voxel_grid<float>::iterator preX_slab_it = preX.begin();
  typename bvxm_voxel_grid<float>::iterator PIvisX_slab_it = PIvisX.begin();

  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++ocp_slab_it, ++apm_slab_it, ++preX_slab_it, ++PIvisX_slab_it)
  {
    vcl_cout << '.';

    if ( (ocp_slab_it == ocp_grid->end()) || (apm_slab_it == apm_grid->end()) ) {
      vcl_cerr << "error: reached end of grid slabs at z = " << z << ".  nz = " << grid_size.z() << vcl_endl;
      return false;
    }

    // backproject image onto voxel plane
    bvxm_util::warp_slab_bilinear(image_slab, H_plane_to_img[z], frame_backproj);
#ifdef DEBUG
    bvxm_util::write_slab_as_image(frame_backproj,"c:/research/registration/output/frame_backproj.tiff");
#endif
    // transform preX to voxel plane for this level
    bvxm_util::warp_slab_bilinear(preX_accum, H_plane_to_img[z], *preX_slab_it);
    // transform visX to voxel plane for this level
    bvxm_util::warp_slab_bilinear(visX_accum, H_plane_to_img[z], visX);

    // initialize PIvisX with PI(X)
    bvxm_voxel_slab<float> PI = apm_processor.prob_density(*apm_slab_it, frame_backproj);

    // now multiply by visX
    bvxm_util::multiply_slabs(visX,PI,*PIvisX_slab_it);

    // update appearance model, using PX*visX as the weights
    bvxm_util::multiply_slabs(visX,*ocp_slab_it,PXvisX);
    apm_processor.update(*apm_slab_it, frame_backproj, PXvisX);

    // multiply to get PIPX
    bvxm_util::multiply_slabs(PI,*ocp_slab_it,PIPX);
#ifdef DEBUG
    bvxm_util::write_slab_as_image(PI,"PI.tiff");
    bvxm_util::write_slab_as_image(*ocp_slab_it,"PX.tiff");
#endif
    // warp PIPX back to image domain
    bvxm_util::warp_slab_bilinear(PIPX, H_img_to_plane[z], PIPX_img);

    // multiply PIPX by visX and add to preX_accum
    bvxm_voxel_slab<float>::iterator PIPX_img_it = PIPX_img.begin();
    bvxm_voxel_slab<float>::iterator visX_accum_it = visX_accum.begin();
    bvxm_voxel_slab<float>::iterator preX_accum_it = preX_accum.begin();

    for (; preX_accum_it != preX_accum.end(); ++preX_accum_it, ++PIPX_img_it, ++visX_accum_it) {
      *preX_accum_it += (*PIPX_img_it) * (*visX_accum_it);
    }
#ifdef DEBUG
    bvxm_util::write_slab_as_image(PIPX_img,"PIPX_img.tiff");
    bvxm_util::write_slab_as_image(visX_accum,"visX_accum.tiff");
    bvxm_util::write_slab_as_image(preX_accum,"preX_accum.tiff");
#endif
    // scale and offset voxel probabilities to get (1-P(X))
    // transform (1-P(X)) to image plane to accumulate visX for next level
    bvxm_util::warp_slab_bilinear(*ocp_slab_it, H_img_to_plane[z], PX_img);

    if (return_mask){
      bvxm_util::add_slabs(PX_img,mask_slab,mask_slab);
    }

    // note: doing scale and offset in image domain so invalid pixels become 1.0 and dont affect visX
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
  bvxm_util::write_slab_as_image(visX_accum,"visX_accum.tiff");
  bvxm_util::write_slab_as_image(preX_accum,"preX_accum.tiff");
#endif
  vcl_cout << vcl_endl << "Pass 2: " << vcl_endl;
  PIvisX_slab_it = PIvisX.begin();
  preX_slab_it = preX.begin();
  bvxm_voxel_grid<ocp_datatype>::iterator ocp_slab_it2 = ocp_grid->begin();
  for (unsigned z = 0; z < (unsigned)grid_size.z(); ++z, ++PIvisX_slab_it, ++preX_slab_it, ++ocp_slab_it2) {
    vcl_cout << '.';

    // transform preX_sum to current level
    bvxm_util::warp_slab_bilinear(preX_accum, H_plane_to_img[z], preX_accum_vox);

    // transform visX_sum to current level
    bvxm_util::warp_slab_bilinear(visX_accum, H_plane_to_img[z], visX_accum_vox);

    const float preX_sum_thresh = 0.01f;

    bvxm_voxel_slab<float>::const_iterator preX_it = preX_slab_it->begin(), PIvisX_it = PIvisX_slab_it->begin(), preX_sum_it = preX_accum_vox.begin(), visX_sum_it = visX_accum_vox.begin();
    bvxm_voxel_slab<float>::iterator PX_it = ocp_slab_it2->begin();

    for (; PX_it != ocp_slab_it2->end(); ++PX_it, ++preX_it, ++PIvisX_it, ++preX_sum_it, ++visX_sum_it) {
      // if preX_sum is zero at the voxel, no ray passed through the voxel (out of image)
      if (*preX_sum_it > preX_sum_thresh) {
        float multiplier = (*PIvisX_it + *preX_it) / *preX_sum_it;
        // leave out normalization for now - results seem a little better without it.  -DEC
        float ray_norm = 1 - *visX_sum_it; //normalize based on probability that a surface voxel is located along the ray. This was not part of the original Pollard + Mundy algorithm.
        *PX_it *= multiplier * ray_norm;
      }
      if (*PX_it < min_vox_prob)
        *PX_it = min_vox_prob;
      if (*PX_it > max_vox_prob)
        *PX_it = max_vox_prob;
    }
  }
  vcl_cout << vcl_endl << "done." << vcl_endl;

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

  // increment the observation count
  this->increment_observations<APM_T>(bin_index);

  return true;
}


template<bvxm_voxel_type APM_T>
bool bvxm_voxel_world::expected_image(bvxm_image_metadata const& camera,
                                      vil_image_view_base_sptr &expected,
                                      vil_image_view<float> &mask,
                                      unsigned bin_index)
{
  // datatype for current appearance model
  typedef typename bvxm_voxel_traits<APM_T>::voxel_datatype apm_datatype;
  typedef typename bvxm_voxel_traits<APM_T>::obs_datatype obs_datatype;
  typedef typename bvxm_voxel_traits<APM_T>::obs_mathtype obs_mathtype;
  typedef typename bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  // the appearance model processor
  typename bvxm_voxel_traits<APM_T>::appearance_processor apm_processor;

  // extract global parameters
  vgl_vector_3d<unsigned int> grid_size = params_->num_voxels();

  // compute homographies from voxel planes to image coordinates and vise-versa.
  vcl_vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  vcl_vector<vgl_h_matrix_2d<double> > H_img_to_plane;
  for (unsigned z=0; z < (unsigned)grid_size.z(); ++z)
  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    bvxm_util::compute_plane_image_H(camera.camera,params_,z,Hp2i,Hi2p);
    H_plane_to_img.push_back(Hp2i);
    H_img_to_plane.push_back(Hi2p);
  }

  // allocate some images
  bvxm_voxel_slab<obs_datatype> expected_slab(expected->ni(),expected->nj(),1);
  bvxm_voxel_slab<obs_datatype> expected_slice_img(expected->ni(), expected->nj(),1);
  bvxm_voxel_slab<float> slice_ocp_img(expected->ni(),expected->nj(),1);
  bvxm_voxel_slab<float> PXvisX_accum(expected->ni(), expected->nj(),1);
  bvxm_voxel_slab<float> visX_accum(expected->ni(), expected->nj(),1);

  PXvisX_accum.fill(0.0f);
  visX_accum.fill(1.0f);
  obs_datatype data(obs_mathtype(0));
  expected_slab.fill(data);

  // get ocuppancy probability grid
  bvxm_voxel_grid_base_sptr ocp_grid_base = this->get_grid<OCCUPANCY>(0);
  bvxm_voxel_grid<ocp_datatype> *ocp_grid  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(ocp_grid_base.ptr());

  //get appereance model grid
  bvxm_voxel_grid_base_sptr apm_grid_base = this->get_grid<APM_T>(bin_index);
  bvxm_voxel_grid<apm_datatype> *apm_grid  = static_cast<bvxm_voxel_grid<apm_datatype>*>(apm_grid_base.ptr());

  typename bvxm_voxel_grid<ocp_datatype>::const_iterator ocp_slab_it = ocp_grid->begin();
  typename bvxm_voxel_grid<apm_datatype>::const_iterator apm_slab_it = apm_grid->begin();

  vcl_cout << "Generating Expected Image: " << vcl_endl;
  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++ocp_slab_it, ++apm_slab_it) {
    vcl_cout << '.';

    // get expected observation
    bvxm_voxel_slab<obs_datatype> expected_slice = apm_processor.expected_color(*apm_slab_it);
    // and project to image plane
    bvxm_util::warp_slab_bilinear(expected_slice, H_img_to_plane[z], expected_slice_img);

    // warp slice_probability to image plane
    bvxm_util::warp_slab_bilinear(*ocp_slab_it, H_img_to_plane[z], slice_ocp_img);

    typename bvxm_voxel_slab<obs_datatype>::const_iterator I_it = expected_slice_img.begin();
    typename bvxm_voxel_slab<ocp_datatype>::const_iterator PX_it = slice_ocp_img.begin();
    typename bvxm_voxel_slab<obs_datatype>::iterator out_it = expected_slab.begin();
    typename bvxm_voxel_slab<float>::iterator visX_it = visX_accum.begin(), W_it = PXvisX_accum.begin();

    for (; out_it != expected_slab.end(); ++I_it, ++PX_it, ++out_it, ++visX_it, ++W_it) {
      float w = *PX_it * *visX_it;
      *W_it += w;
      *out_it += *I_it * w;
      // update visX for next level
      *visX_it *= (1.0f - *PX_it);
    }
  }
  vcl_cout << vcl_endl;

  typename bvxm_voxel_slab<obs_datatype>::iterator out_it = expected_slab.begin();
  typename bvxm_voxel_slab<float>::const_iterator W_it = PXvisX_accum.begin();
  // normalize expected image by weight sum
  for (; out_it != expected_slab.end(); ++out_it, ++W_it) {
    if (*W_it > 0)
      *out_it /= *W_it;
  }

  // convert back to vil_image_view
  bvxm_util::slab_to_img(expected_slab, expected);

  // convert PXvisX_accum to mask
  bvxm_voxel_slab<bool> mask_slab(PXvisX_accum.nx(),PXvisX_accum.ny(),1);
  bvxm_util::threshold_slab_above(PXvisX_accum,0.001f,mask_slab);
  vil_image_view<float>::iterator mask_img_it = mask.begin();
  bvxm_voxel_slab<bool>::iterator mask_slab_it = mask_slab.begin();
  mask.fill(0.0f);
  for (; mask_img_it != mask.end(); ++mask_img_it, ++mask_slab_it) {
    if (*mask_slab_it)
      *mask_img_it = 1.0f;
  }

  return true;
}

template<bvxm_voxel_type APM_T>
bool bvxm_voxel_world::inv_pixel_range_probability(bvxm_image_metadata const& observation,
                                                   vil_image_view<float> &inv_prob,
                                                   unsigned bin_index, float pixel_range)
{
  // datatype for current appearance model
  typedef typename bvxm_voxel_traits<APM_T>::voxel_datatype apm_datatype;
  typedef typename bvxm_voxel_traits<APM_T>::obs_datatype obs_datatype;
  typedef typename bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  typename bvxm_voxel_traits<APM_T>::appearance_processor apm_processor;

  bvxm_world_params_sptr params = params_;

  // check image sizes
  if ( (observation.img->ni() != inv_prob.ni()) || (observation.img->nj() != inv_prob.nj()) ) {
    vcl_cerr << "error: observation image size does not match input image size. " << vcl_endl;
  }

  vgl_vector_3d<unsigned int> grid_size = params->num_voxels();
  ocp_datatype min_vox_prob = params->min_occupancy_prob();
  ocp_datatype max_vox_prob = params->max_occupancy_prob();

  // compute homographies from voxel planes to image coordinates and vise-versa.
  vcl_vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  vcl_vector<vgl_h_matrix_2d<double> > H_img_to_plane;
  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    for (unsigned z=0; z < (unsigned)grid_size.z(); ++z)
    {
      bvxm_util::compute_plane_image_H(observation.camera,params,z,Hp2i,Hi2p);
      H_plane_to_img.push_back(Hp2i);
      H_img_to_plane.push_back(Hi2p);
    }
  }

  // convert image to a voxel_slab
  bvxm_voxel_slab<obs_datatype> image_slab(observation.img->ni(), observation.img->nj(), 1);
  bvxm_util::img_to_slab(observation.img,image_slab);

  bvxm_voxel_slab<float> preX, visX, slice_prob, PIPX;

  bvxm_voxel_slab<obs_datatype> frame_backproj(grid_size.x(),grid_size.y(),1);

  bvxm_voxel_slab<float> visX_accum(observation.img->ni(),observation.img->nj(),1);
  visX_accum.fill(1.0f);
  inv_prob.fill(1.0f);
  bvxm_voxel_slab<float> PIPX_image(observation.img->ni(),observation.img->nj(),1);
  bvxm_voxel_slab<float> slice_prob_image(observation.img->ni(),observation.img->nj(),1);

  vcl_cout << "Computing inverse probability of frame +- range: ";

  // get ocuppancy probability grid
  bvxm_voxel_grid_base_sptr ocp_grid_base = this->get_grid<OCCUPANCY>(0);
  bvxm_voxel_grid<ocp_datatype> *ocp_grid  = static_cast<bvxm_voxel_grid<ocp_datatype> >(ocp_grid_base.ptr());

  //get appereance model grid
  bvxm_voxel_grid_base_sptr apm_grid_base = this->get_grid<APM_T>(bin_index);
  bvxm_voxel_grid<apm_datatype> *apm_grid  = static_cast<bvxm_voxel_grid<apm_datatype> >(apm_grid_base.ptr());

  typename bvxm_voxel_grid<ocp_datatype>::const_iterator ocp_slab_it = ocp_grid->begin();
  typename bvxm_voxel_grid<apm_datatype>::iterator apm_slab_it = apm_grid->begin();

  for (int z=0; z<grid_size.z(); ++z, ++ocp_slab_it, ++apm_slab_it)
  {
    vcl_cout << '.';

    if ( (ocp_slab_it == ocp_grid->end()) || (apm_slab_it == apm_grid->end()) ) {
      vcl_cerr << "error: reached end of grid slabs at z = " << z << ".  nz = " << grid_size.z() << vcl_endl;
      return false;
    }

    // backproject image onto voxel plane
    bvxm_util::warp_slab_bilinear(image_slab, H_plane_to_img[z], frame_backproj);

    // create min and max observations: for now, just use same range for every pixel
    bvxm_voxel_slab<obs_datatype> obs_min(frame_backproj.nx(),frame_backproj.ny(),frame_backproj.nz());
    bvxm_voxel_slab<obs_datatype> obs_max(frame_backproj.ny(),frame_backproj.ny(),frame_backproj.nz());
    typename bvxm_voxel_slab<obs_datatype>::const_iterator backproj_it = frame_backproj.begin();
    typename bvxm_voxel_slab<obs_datatype>::iterator max_it = obs_max.begin(), min_it = obs_min.begin();
    for (; backproj_it != frame_backproj.end(); ++backproj_it, ++max_it, ++min_it) {
      *max_it = *backproj_it + pixel_range;
      *min_it = *backproj_it - pixel_range;
    }

    // transform visX to voxel plane for this level
    bvxm_util::warp_slab_bilinear(visX_accum, H_plane_to_img[z], visX);

    // initialize PIPX with PI
    PIPX = apm_processor.prob_range(*apm_slab_it, obs_min, obs_max);
    // and multiply with PX
    bvxm_util::multiply_slabs(*ocp_slab_it,PIPX,PIPX);

    // now transform to image plane
    bvxm_util::warp_slab_bilinear(PIPX, H_img_to_plane[z], PIPX_image);
    bvxm_util::warp_slab_bilinear(*ocp_slab_it,H_img_to_plane[z], slice_prob_image);

    // update pixel probabilities for this level
    bvxm_voxel_slab<ocp_datatype>::const_iterator PX_it = slice_prob_image.begin();
    bvxm_voxel_slab<float>::const_iterator PIPX_it = PIPX_image.begin();
    bvxm_voxel_slab<float>::iterator visX_it = visX_accum.begin(), prob_it = inv_prob.begin();
    for (; prob_it != inv_prob.end(); ++prob_it, ++PIPX_it, ++visX_it, ++PX_it) {
      float PIPXvisX = *PIPX_it * *visX_it;
      *prob_it *= (1 - PIPXvisX);
      *visX_it *= (1 - *PX_it);
    }
  }
  vcl_cout << vcl_endl;

  return true;
}

template<bvxm_voxel_type APM_T>
bool bvxm_voxel_world::pixel_probability_density(bvxm_image_metadata const& observation,
                                                 vil_image_view<float> &pixel_probability, unsigned bin_index)
{
  // datatype for current appearance model
  typedef typename bvxm_voxel_traits<APM_T>::voxel_datatype apm_datatype;
  typedef typename bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;
  typedef typename bvxm_voxel_traits<APM_T>::obs_datatype obs_datatype;

  // the appearance model processor
  typename bvxm_voxel_traits<APM_T>::appearance_processor apm_processor;

  // check image sizes
  if ( (observation.img->ni() != pixel_probability.ni()) || (observation.img->nj() != pixel_probability.nj()) ) {
    vcl_cerr << "error: observation image size does not match input image size. " << vcl_endl;
  }

  vgl_vector_3d<unsigned int> grid_size = params_->num_voxels();

  // compute homographies from voxel planes to image coordinates and vise-versa.
  vcl_vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  vcl_vector<vgl_h_matrix_2d<double> > H_img_to_plane;
  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    for (unsigned z=0; z < (unsigned)grid_size.z(); ++z)
    {
      bvxm_util::compute_plane_image_H(observation.camera,params_,z,Hp2i,Hi2p);
      H_plane_to_img.push_back(Hp2i);
      H_img_to_plane.push_back(Hi2p);
    }
  }

  // convert image to a voxel_slab
  bvxm_voxel_slab<obs_datatype> image_slab(observation.img->ni(), observation.img->nj(), 1);
  bvxm_util::img_to_slab(observation.img,image_slab);

  bvxm_voxel_slab<float> preX(grid_size.x(),grid_size.y(),1);
  bvxm_voxel_slab<float> PIPX(grid_size.x(),grid_size.y(),1);

  bvxm_voxel_slab<float> preX_accum(image_slab.nx(),image_slab.ny(),1);
  bvxm_voxel_slab<float> visX_accum(image_slab.nx(),image_slab.ny(),1);
  bvxm_voxel_slab<float> img_scratch(image_slab.nx(),image_slab.ny(),1);
  bvxm_voxel_slab<float> PIPX_img(image_slab.nx(), image_slab.ny(),1);
  bvxm_voxel_slab<float> PX_img(image_slab.nx(), image_slab.ny(),1);

  preX_accum.fill(0.0f);
  visX_accum.fill(1.0f);

  // slabs for holding backprojections of visX
  bvxm_voxel_slab<float> visX(grid_size.x(),grid_size.y(),1);

  bvxm_voxel_slab<obs_datatype> frame_backproj(grid_size.x(),grid_size.y(),1);

  vcl_cout << "Pass 1: " << vcl_endl;

  // get ocuppancy probability grid
  bvxm_voxel_grid_base_sptr ocp_grid_base = this->get_grid<OCCUPANCY>(0);
  bvxm_voxel_grid<ocp_datatype> *ocp_grid  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(ocp_grid_base.ptr());

  //get appereance model grid
  bvxm_voxel_grid_base_sptr apm_grid_base = this->get_grid<APM_T>(bin_index);
  bvxm_voxel_grid<apm_datatype> *apm_grid  = static_cast<bvxm_voxel_grid<apm_datatype>*>(apm_grid_base.ptr());

  typename bvxm_voxel_grid<ocp_datatype>::const_iterator ocp_slab_it = ocp_grid->begin();
  typename bvxm_voxel_grid<apm_datatype>::iterator apm_slab_it = apm_grid->begin();

  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++ocp_slab_it, ++apm_slab_it)
  {
    vcl_cout << '.';

    if ( (ocp_slab_it == ocp_grid->end()) || (apm_slab_it == apm_grid->end()) ) {
      vcl_cerr << "error: reached end of grid slabs at z = " << z << ".  nz = " << grid_size.z() << vcl_endl;
      return false;
    }

    // backproject image onto voxel plane
    bvxm_util::warp_slab_bilinear(image_slab, H_plane_to_img[z], frame_backproj);

    // transform preX to voxel plane for this level
    bvxm_util::warp_slab_bilinear(preX_accum, H_plane_to_img[z], preX);
    // transform visX to voxel plane for this level
    bvxm_util::warp_slab_bilinear(visX_accum, H_plane_to_img[z], visX);

    // calculate PI(X)
    bvxm_voxel_slab<float> PI = apm_processor.prob_density(*apm_slab_it, frame_backproj);

    // multiply to get PIPX
    bvxm_util::multiply_slabs(PI,*ocp_slab_it,PIPX);
#ifdef DEBUG
    bvxm_util::write_slab_as_image(PI,"PI.tiff");
    bvxm_util::write_slab_as_image(*ocp_slab_it,"PX.tiff");
#endif
    // warp PIPX back to image domain
    bvxm_util::warp_slab_bilinear(PIPX, H_img_to_plane[z], PIPX_img);

    // multiply PIPX by visX and add to preX_accum
    bvxm_voxel_slab<float>::iterator PIPX_img_it = PIPX_img.begin();
    bvxm_voxel_slab<float>::iterator visX_accum_it = visX_accum.begin();
    bvxm_voxel_slab<float>::iterator preX_accum_it = preX_accum.begin();

    for (; preX_accum_it != preX_accum.end(); ++preX_accum_it, ++PIPX_img_it, ++visX_accum_it) {
      *preX_accum_it += (*PIPX_img_it) * (*visX_accum_it);
    }
#ifdef DEBUG
    bvxm_util::write_slab_as_image(PIPX_img,"PIPX_img.tiff");
    bvxm_util::write_slab_as_image(visX_accum,"visX_accum.tiff");
    bvxm_util::write_slab_as_image(preX_accum,"preX_accum.tiff");
#endif
    // scale and offset voxel probabilities to get (1-P(X))
    // transform (1-P(X)) to image plane to accumulate visX for next level
    bvxm_util::warp_slab_bilinear(*ocp_slab_it, H_img_to_plane[z], PX_img);

    // note: doing scale and offset in image domain so invalid pixels become 1.0 and dont affect visX
    bvxm_voxel_slab<float>::iterator PX_img_it = PX_img.begin();
    visX_accum_it = visX_accum.begin();
    for (; visX_accum_it != visX_accum.end(); ++visX_accum_it, ++PX_img_it) {
      *visX_accum_it *= (1 - *PX_img_it);
    }
  }

  vcl_cout << vcl_endl << "done." << vcl_endl;

  // fill pixel_probabilities with preX_accum
  vil_image_view<float>::iterator pix_prob_it = pixel_probability.begin();
  bvxm_voxel_slab<float>::const_iterator preX_accum_it = preX_accum.begin();
  for (; pix_prob_it != pixel_probability.end(); ++pix_prob_it, ++preX_accum_it) {
    *pix_prob_it = *preX_accum_it;
  }

  return true;
}

//: generate the mixture of gaussians slab from the specified viewpoint. the slab should be allocated by the caller.
template<bvxm_voxel_type APM_T>
bool bvxm_voxel_world::mixture_of_gaussians_image(bvxm_image_metadata const& observation,
                                                  bvxm_voxel_slab_base_sptr& mog_image, unsigned bin_index)
{
  // datatype for current appearance model
  typedef typename bvxm_voxel_traits<APM_T>::voxel_datatype apm_datatype; // datatype for current appearance model
  typedef typename bvxm_voxel_traits<APM_T>::obs_datatype obs_datatype;   // datatype of the pixels that the processor operates on.
  typedef typename bvxm_voxel_traits<APM_T>::obs_mathtype obs_mathtype;
  typedef typename bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  typename bvxm_voxel_traits<APM_T>::appearance_processor apm_processor;

  // extract global parameters
  vgl_vector_3d<unsigned int> grid_size = params_->num_voxels();

  // compute homographies from voxel planes to image coordinates and vise-versa.
  vcl_vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  vcl_vector<vgl_h_matrix_2d<double> > H_img_to_plane;
  for (unsigned z=0; z < (unsigned)grid_size.z(); ++z)
  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    bvxm_util::compute_plane_image_H(observation.camera,params_,z,Hp2i,Hi2p);
    H_plane_to_img.push_back(Hp2i);
    H_img_to_plane.push_back(Hi2p);
  }

  // allocate some images
  bvxm_voxel_slab<apm_datatype> mog_slab(observation.img->ni(),observation.img->nj(),1);
  bvxm_voxel_slab<obs_datatype> expected_slice_img(observation.img->ni(), observation.img->nj(),1);
  bvxm_voxel_slab<float> slice_ocp_img(observation.img->ni(),observation.img->nj(),1);
  bvxm_voxel_slab<float> visX_accum(observation.img->ni(), observation.img->nj(),1);

  visX_accum.fill(1.0f);
  mog_slab.fill(bvxm_voxel_traits<APM_T>::initial_val());

  // get ocuppancy probability grid
  bvxm_voxel_grid_base_sptr ocp_grid_base = this->get_grid<OCCUPANCY>(0);
  bvxm_voxel_grid<ocp_datatype> *ocp_grid  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(ocp_grid_base.ptr());

  //get appereance model grid
  bvxm_voxel_grid_base_sptr apm_grid_base = this->get_grid<APM_T>(bin_index);
  bvxm_voxel_grid<apm_datatype> *apm_grid  = static_cast<bvxm_voxel_grid<apm_datatype>*>(apm_grid_base.ptr());

  typename bvxm_voxel_grid<ocp_datatype>::const_iterator ocp_slab_it = ocp_grid->begin();
  typename bvxm_voxel_grid<apm_datatype>::const_iterator apm_slab_it = apm_grid->begin();

  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++ocp_slab_it, ++apm_slab_it)
  {
    // get expected observation
    bvxm_voxel_slab<obs_datatype> expected_slice = apm_processor.expected_color(*apm_slab_it);
    // and project to image plane
    bvxm_util::warp_slab_bilinear(expected_slice, H_img_to_plane[z], expected_slice_img);

    // warp slice_probability to image plane
    bvxm_util::warp_slab_bilinear(*ocp_slab_it, H_img_to_plane[z], slice_ocp_img);

    typename bvxm_voxel_slab<ocp_datatype>::const_iterator PX_it = slice_ocp_img.begin();
    bvxm_voxel_slab<float>::iterator visX_it = visX_accum.begin();

#if 0 // do the following update operation from Thom's code
    float hard_mult = 1;
    for ( unsigned v = 0; v < voxels.size(); v++ ){
      float this_color = voxels[v]->appearance->expected_color( light );
      if ( this_color >= 0 )
        mog->update( this_color, hard_mult*voxels[v]->occupancy_prob[0], light );
      hard_mult *= (1-voxels[v]->occupancy_prob[0]);
    }
#endif // 0
    bvxm_voxel_slab<float> w(observation.img->ni(), observation.img->nj(),1);
    w.fill(1.0f);
    bvxm_voxel_slab<float>::iterator w_it = w.begin();
    for (; w_it != w.end(); ++PX_it, ++visX_it, ++w_it) {
      *w_it *= *PX_it * *visX_it;
      *visX_it *= (1.0f - *PX_it);  // update visX for next level
    }

    if (!apm_processor.update(mog_slab, expected_slice_img, w)) {   // check "if (*I_it >= 0)" during update
      vcl_cout << "In bvxm_voxel_world<APM_T>::mixture_of_gaussians_image() -- problems in appearance update\n";
      return false;
    }
  }

  mog_image = new bvxm_voxel_slab<apm_datatype>(mog_slab);

  return true;
}

template<bvxm_voxel_type APM_T>
bool bvxm_voxel_world::virtual_view(bvxm_image_metadata const& original_view,
                                    const vpgl_camera_double_sptr virtual_camera,
                                    vil_image_view_base_sptr &virtual_view,
                                    vil_image_view<float> &vis_prob,
                                    unsigned bin_index)
{
  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;
  typedef typename bvxm_voxel_traits<APM_T>::obs_datatype obs_datatype;
  // extract global parameters
  vgl_vector_3d<unsigned int> grid_size = params_->num_voxels();

  // the appearance model processor
  typename bvxm_voxel_traits<APM_T>::appearance_processor apm_processor;

  // compute homographies from voxel planes to image coordinates and vise-versa.
  vcl_vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  vcl_vector<vgl_h_matrix_2d<double> > H_img_to_plane;
  vcl_vector<vgl_h_matrix_2d<double> > H_plane_to_virtual_img;
  vcl_vector<vgl_h_matrix_2d<double> > H_virtual_img_to_plane;
  vcl_vector<vgl_h_matrix_2d<double> > H_virtual_img_to_img;

  for (unsigned z=0; z < (unsigned)grid_size.z(); ++z)
  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    // real camera
    bvxm_util::compute_plane_image_H(original_view.camera,params_,z,Hp2i,Hi2p);
    H_plane_to_img.push_back(Hp2i);
    H_img_to_plane.push_back(Hi2p);
    // virtual camera
    bvxm_util::compute_plane_image_H(virtual_camera,params_,z,Hp2i,Hi2p);
    H_plane_to_virtual_img.push_back(Hp2i);
    H_virtual_img_to_plane.push_back(Hi2p);
    // image to image
    H_virtual_img_to_img.push_back(H_plane_to_img.back()*H_virtual_img_to_plane.back());
  }

  // allocate some images
  bvxm_voxel_slab<obs_datatype> virtual_view_slab(virtual_view->ni(),virtual_view->nj(),1);
  bvxm_voxel_slab<float> visX_accum_virtual(virtual_view->ni(), virtual_view->nj(),1);
  bvxm_voxel_slab<unsigned> heightmap_rough(virtual_view->ni(),virtual_view->nj(),1);
  bvxm_voxel_slab<float> max_prob_image(virtual_view->ni(), virtual_view->nj(), 1);
  bvxm_voxel_slab<float> heightmap_filtered(virtual_view->ni(),virtual_view->nj(),1);
  bvxm_voxel_slab<ocp_datatype> slice_prob_img(virtual_view->ni(),virtual_view->nj(),1);

  heightmap_rough.fill(grid_size.z());
  visX_accum_virtual.fill(1.0f);
  max_prob_image.fill(0.0f);

  // get ocuppancy probability grid
  bvxm_voxel_grid_base_sptr ocp_grid_base = this->get_grid<OCCUPANCY>(0);
  bvxm_voxel_grid<ocp_datatype> *ocp_grid  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(ocp_grid_base.ptr());

  bvxm_voxel_grid<ocp_datatype>::const_iterator ocp_slab_it = ocp_grid->begin();

  vcl_cout << "Pass 1 - generating height map from virtual camera: " << vcl_endl;
  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++ocp_slab_it) {
    vcl_cout << '.';

    // compute PXvisX for virtual camera and update visX
    bvxm_util::warp_slab_bilinear(*ocp_slab_it,H_virtual_img_to_plane[z],slice_prob_img);
    bvxm_voxel_slab<ocp_datatype>::const_iterator PX_it = slice_prob_img.begin();
    bvxm_voxel_slab<float>::iterator max_it = max_prob_image.begin(), visX_it = visX_accum_virtual.begin();
    bvxm_voxel_slab<unsigned>::iterator hmap_it = heightmap_rough.begin();
#ifdef DEBUG
    bvxm_util::write_slab_as_image(slice_prob_img,"c:/research/registration/output/slice_prob_img.tiff");
    bvxm_util::write_slab_as_image(visX_accum_virtual,"c:/research/registration/output/visX_accum_virtual.tiff");
#endif
    for (; hmap_it != heightmap_rough.end(); ++hmap_it, ++PX_it, ++max_it, ++visX_it) {
      float PXvisX = (*visX_it) * (*PX_it);
      if (PXvisX > *max_it) {
        *max_it = PXvisX;
        *hmap_it = z;
      }
      // update virtual visX
      *visX_it *= (1.0f - *PX_it);
    }
  }
  vcl_cout << vcl_endl;
#ifdef DEBUG
  bvxm_util::write_slab_as_image(heightmap_rough,"c:/research/registration/output/heightmap_rough.tiff");
#endif
  // now clean up height map
  unsigned n_smooth_iterations = 10;
  float conf_thresh = 0.05f;

  vcl_cout << "smoothing height map: ";
  bvxm_voxel_slab<bool> conf_mask(virtual_view->ni(),virtual_view->nj(),1);
  // threshold confidence
  bvxm_util::threshold_slab_above(max_prob_image, conf_thresh, conf_mask);

  // initialize with rough heightmap
  bvxm_voxel_slab<unsigned>::const_iterator hmap_rough_it = heightmap_rough.begin();
  bvxm_voxel_slab<float>::iterator hmap_filt_it = heightmap_filtered.begin();
  for (; hmap_filt_it != heightmap_filtered.end(); ++hmap_filt_it, ++hmap_rough_it) {
    *hmap_filt_it = (float)(*hmap_rough_it);
  }

  for (unsigned i=0; i< n_smooth_iterations; ++i) {
    vcl_cout << '.';
    // smooth heightmap
    bvxm_util::smooth_gaussian(heightmap_filtered, 1.0f, 1.0f);
    // reset values we are confident in
    bvxm_voxel_slab<bool>::const_iterator mask_it = conf_mask.begin();
    hmap_rough_it = heightmap_rough.begin();
    hmap_filt_it = heightmap_filtered.begin();
    for (; hmap_filt_it != heightmap_filtered.end(); ++hmap_filt_it, ++hmap_rough_it, ++mask_it) {
      if (*mask_it) {
        *hmap_filt_it = (float)(*hmap_rough_it);
      }
    }
  }
  vcl_cout << vcl_endl;

  // create virtual image based on smoothed height map
  bvxm_voxel_slab<obs_datatype> frame_virtual_proj(virtual_view->ni(),virtual_view->nj(),1);

  bvxm_voxel_slab<ocp_datatype> visX_accum(original_view.img->ni(),original_view.img->nj(),1);
  bvxm_voxel_slab<ocp_datatype> visX_accum_virtual_proj(virtual_view->ni(),virtual_view->nj(),1);
  visX_accum.fill(1.0f);
#ifdef DEBUG
  bvxm_util::write_slab_as_image(heightmap_filtered,"c:/research/registration/output/heightmap_filtered.tiff");
#endif
  vcl_cout <<"Pass 2 - generating virtual image: ";

  ocp_slab_it = ocp_grid->begin();
  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++ocp_slab_it) {
    vcl_cout << '.';

    // project image to virtual image
    bvxm_voxel_slab<obs_datatype> image_slab(original_view.img->ni(),original_view.img->nj(),1);
    bvxm_util::img_to_slab(original_view.img,image_slab);
    bvxm_util::warp_slab_bilinear(image_slab, H_virtual_img_to_img[z], frame_virtual_proj);

    // project visX_accum from image to virtual image
    bvxm_util::warp_slab_bilinear(visX_accum, H_virtual_img_to_img[z], visX_accum_virtual_proj);
#ifdef DEBUG
    bvxm_util::write_slab_as_image(visX_accum,"c:/research/registration/output/visX_accum.tiff");
    bvxm_util::write_slab_as_image(visX_accum_virtual,"c:/research/registration/output/visX_accum_virtual.tiff");
#endif
    // project slice probabilities into virtual camera
    bvxm_voxel_slab<float> slice_prob_vimg(virtual_view->ni(),virtual_view->nj(),1);
    bvxm_util::warp_slab_bilinear(*ocp_slab_it, H_virtual_img_to_plane[z], slice_prob_vimg);

    typename bvxm_voxel_slab<obs_datatype>::const_iterator frame_it = frame_virtual_proj.begin();
    typename bvxm_voxel_slab<obs_datatype>::iterator vframe_it = virtual_view_slab.begin();
    bvxm_voxel_slab<float>::const_iterator height_it = heightmap_filtered.begin();
    bvxm_voxel_slab<float>::const_iterator PX_vproj_it = slice_prob_vimg.begin();
    bvxm_voxel_slab<float>::iterator visX_accum_vproj_it = visX_accum_virtual_proj.begin();
    vil_image_view<float>::iterator vis_it = vis_prob.begin();

    for (; vframe_it != virtual_view_slab.end(); ++vframe_it, ++frame_it, ++height_it, ++vis_it, ++visX_accum_vproj_it, ++PX_vproj_it) {
      // fill in virtual image
      if ((unsigned)(*height_it) == z) {
        *vframe_it = *frame_it;
        *vis_it = (*visX_accum_vproj_it);// * (*PX_vproj_it);
      }
    }
    // project slice probabilities into real camera
    bvxm_util::warp_slab_bilinear(*ocp_slab_it, H_img_to_plane[z], slice_prob_img);

    // update visX_accum
    bvxm_voxel_slab<ocp_datatype>::iterator PX_it = slice_prob_img.begin(), visX_accum_it = visX_accum.begin();
    for (; visX_accum_it != visX_accum.end(); ++visX_accum_it, ++PX_it) {
      *visX_accum_it *= (1.0f - *PX_it);
    }
#ifdef DEBUG
    bvxm_util::write_slab_as_image(visX_accum,"c:/research/registration/output/visX_accum.tiff");
#endif
  }
  vcl_cout << vcl_endl;

  // mask out pixels whose rays did not intersct any voxels in the original and virtual frames
  vcl_cout << "Normalizing visibility probability. ";
  const float visX_thresh = 1.0f - params_->min_occupancy_prob();

  // set mask to 0 for all pixels whose corresponding pixel in the original image did not pass through a voxel.
  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z)
  {
    vcl_cout << '.';

    // project final visX_accum from image to virtual image
    bvxm_util::warp_slab_bilinear(visX_accum, H_virtual_img_to_img[z], visX_accum_virtual_proj);

    bvxm_voxel_slab<float>::const_iterator height_it = heightmap_filtered.begin();
    vil_image_view<float>::iterator vis_it = vis_prob.begin();
    bvxm_voxel_slab<ocp_datatype>::iterator visX_virtual_proj_it = visX_accum_virtual_proj.begin();

    for (; vis_it != vis_prob.end(); ++vis_it, ++visX_virtual_proj_it, ++height_it)
      if ((unsigned)(*height_it) == z) {
        if (*visX_virtual_proj_it > visX_thresh) {
          *vis_it = 0.0f;
        }
      }
  }
  // set mask to 0 for all pixels in virtual image that did not intersect a voxel.
  bvxm_voxel_slab<ocp_datatype>::iterator visX_accum_virtual_it = visX_accum_virtual.begin();
  vil_image_view<float>::iterator vis_it = vis_prob.begin();
  for (; vis_it != vis_prob.end(); ++vis_it, ++visX_accum_virtual_it) {
    if (*visX_accum_virtual_it > visX_thresh ) {
      *vis_it = 0.0f;
    }
  }

  bvxm_util::slab_to_img(virtual_view_slab,virtual_view);

  return true;
}

#endif // bvxm_voxel_world_h_
