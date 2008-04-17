// This is brl/bseg/bvxm/bvxm_voxel_world.cxx
#include "bvxm_voxel_world.h"
//:
// \file

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_camera.h>
#include <vul/vul_file.h>

#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>

#include "bvxm_voxel_grid.h"
#include "bvxm_voxel_traits.h"
#include "bvxm_lidar_processor.h"
#include "bvxm_world_params.h"
#include "bvxm_voxel_slab.h"
#include "bvxm_image_metadata.h"
#include "bvxm_util.h"
#include "bvxm_lidar_camera.h"


//: Destructor
bvxm_voxel_world::~bvxm_voxel_world()
{
}


//: equality operator
bool bvxm_voxel_world::operator == (bvxm_voxel_world const& that) const
{
  return true;
}

//: less than operator
bool bvxm_voxel_world::operator < (bvxm_voxel_world const& that) const
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
vcl_ostream&  operator<<(vcl_ostream& s, bvxm_voxel_world const& vox_world)
{
  bvxm_world_params_sptr params = vox_world.get_params();
  s << "bvxm_voxel_world : " << params->num_voxels().x() << " x " << params->num_voxels().y() << " x " << params->num_voxels().z() << vcl_endl;
  return s;
}


//: save the occupancy grid in a ".raw" format readable by Drishti volume rendering software
bool bvxm_voxel_world::save_occupancy_raw(vcl_string filename)
{
  vcl_fstream ofs(filename.c_str(),vcl_ios::binary | vcl_ios::out);
  if (!ofs.is_open()) {
    vcl_cerr << "error opening file " << filename << " for write!\n";
    return false;
  }
  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  // write header
  unsigned char data_type = 0; // 0 means unsigned byte

  bvxm_voxel_grid<ocp_datatype> *ocp_grid =
    dynamic_cast<bvxm_voxel_grid<ocp_datatype>*>(get_grid<OCCUPANCY>(0).ptr());

  vxl_uint_32 nx = ocp_grid->grid_size().x();
  vxl_uint_32 ny = ocp_grid->grid_size().y();
  vxl_uint_32 nz = ocp_grid->grid_size().z();

  ofs.write(reinterpret_cast<char*>(&data_type),sizeof(data_type));
  ofs.write(reinterpret_cast<char*>(&nx),sizeof(nx));
  ofs.write(reinterpret_cast<char*>(&ny),sizeof(ny));
  ofs.write(reinterpret_cast<char*>(&nz),sizeof(nz));

  // write data
  // iterate through slabs and fill in memory array
  char *ocp_array = new char[nx*ny*nz];

  bvxm_voxel_grid<ocp_datatype>::iterator ocp_it = ocp_grid->begin();
  for (unsigned k=0; ocp_it != ocp_grid->end(); ++ocp_it, ++k) {
    vcl_cout << '.';
    for (unsigned i=0; i<(*ocp_it).nx(); ++i) {
      for (unsigned j=0; j < (*ocp_it).ny(); ++j) {
        ocp_array[i*ny*nz + j*nz + k] = (unsigned char)((*ocp_it)(i,j) * 255.0);;
      }
    }
  }
  vcl_cout << vcl_endl;
  ofs.write(reinterpret_cast<char*>(ocp_array),sizeof(unsigned char)*nx*ny*nz);

  ofs.close();

  delete[] ocp_array;

  return true;
}

//: save the edge probability grid in a ".raw" format readable by Drishti volume rendering software
bool bvxm_voxel_world::save_edges_raw(vcl_string filename)
{
  vcl_fstream ofs(filename.c_str(),vcl_ios::binary | vcl_ios::out);
  if (!ofs.is_open()) {
    vcl_cerr << "error opening file " << filename << " for write!\n";
    return false;
  }
  typedef bvxm_voxel_traits<EDGES>::voxel_datatype edges_datatype;

  // write header
  unsigned char data_type = 0; // 0 means unsigned byte

  bvxm_voxel_grid<edges_datatype> *edges_grid =
    dynamic_cast<bvxm_voxel_grid<edges_datatype>*>(get_grid<EDGES>(0).ptr());

  vxl_uint_32 nx = edges_grid->grid_size().x();
  vxl_uint_32 ny = edges_grid->grid_size().y();
  vxl_uint_32 nz = edges_grid->grid_size().z();

  ofs.write(reinterpret_cast<char*>(&data_type),sizeof(data_type));
  ofs.write(reinterpret_cast<char*>(&nx),sizeof(nx));
  ofs.write(reinterpret_cast<char*>(&ny),sizeof(ny));
  ofs.write(reinterpret_cast<char*>(&nz),sizeof(nz));

  // write data
  // iterate through slabs and fill in memory array
  char *edges_array = new char[nx*ny*nz];

  bvxm_voxel_grid<edges_datatype>::iterator edges_it = edges_grid->begin();
  for (unsigned k=0; edges_it != edges_grid->end(); ++edges_it, ++k) {
    vcl_cout << '.';
    for (unsigned i=0; i<(*edges_it).nx(); ++i) {
      for (unsigned j=0; j < (*edges_it).ny(); ++j) {
        edges_array[i*ny*nz + j*nz + k] = (unsigned char)((*edges_it)(i,j) * 255.0);;
      }
    }
  }
  vcl_cout << vcl_endl;
  ofs.write(reinterpret_cast<char*>(edges_array),sizeof(unsigned char)*nx*ny*nz);

  ofs.close();

  delete[] edges_array;

  return true;
}

//: remove all voxel data from disk - use with caution!
bool bvxm_voxel_world::clean_grids()
{
  // look for existing grids in the directory
  vcl_string storage_directory = params_->model_dir();

  vcl_stringstream grid_glob;
  grid_glob << storage_directory << "/*.vox";
  bool result = vul_file::delete_file_glob(grid_glob.str().c_str());

  grid_map_.clear();

  return result;
}

// Update a voxel grid with data from lidar/camera pair
bool bvxm_voxel_world::update_lidar(bvxm_image_metadata const& observation)
{
  vil_image_view<float> dummy;
  vil_image_view<bool> mask;
  return this->update_lidar_impl(observation, false, dummy, false, mask);
}


// Update a voxel grid with data from lidar/camera pair and return probability density of pixel values.
bool bvxm_voxel_world::update_lidar(bvxm_image_metadata const& observation,
                                    vil_image_view<float> &pix_prob_density, vil_image_view<bool> &mask)
{
  // check image sizes
  if ( (observation.img->ni() != pix_prob_density.ni()) || (observation.img->nj() != pix_prob_density.nj()) ) {
    vcl_cerr << "error: metadata image size does not match probability image size.\n";
  }
  if ( (observation.img->ni() != mask.ni()) || (observation.img->nj() != mask.nj()) ) {
    vcl_cerr << "error: metadata image size does not match mask image size.\n";
  }
  return this->update_lidar_impl(observation, true, pix_prob_density, true, mask);
}

// Update voxel grid with data from LIDAR image/camera pair and return probability density of pixel values.
bool bvxm_voxel_world::update_lidar_impl(bvxm_image_metadata const& metadata,
                                         bool return_prob,
                                         vil_image_view<float> &pix_prob_density,
                                         bool return_mask,
                                         vil_image_view<bool> &mask)
{


  typedef bvxm_voxel_traits<LIDAR>::voxel_datatype obs_datatype;
  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  vpgl_camera_double_sptr dummy_cam = metadata.camera;
  vcl_cout << dummy_cam << vcl_endl;

  //typedef bvxm_voxel_traits<LIDAR>::lidar_processor lidar_processor;
  bvxm_lidar_processor lidar_processor(10);

  // parameters
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
      compute_plane_image_H(metadata.camera,z,Hp2i,Hi2p);
      H_plane_to_img.push_back(Hp2i);
      H_img_to_plane.push_back(Hi2p);
    }
  }

  // convert image to a voxel_slab
  bvxm_voxel_slab<obs_datatype> image_slab(metadata.img->ni(), metadata.img->nj(), 1);
  if (!bvxm_util::img_to_slab(metadata.img,image_slab)) {
    vcl_cerr << "error converting image to voxel slab of observation type for bvxm_voxel_type: LIDAR" << vcl_endl;
    return false;
  }

#ifdef debug
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

  vcl_cout << "Pass 1: " << vcl_endl;

  // get ocuppancy probability grid
  bvxm_voxel_grid_base_sptr ocp_grid_base = this->get_grid<OCCUPANCY>(0);
  bvxm_voxel_grid<ocp_datatype> *ocp_grid  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(ocp_grid_base.ptr());


  bvxm_voxel_grid<ocp_datatype>::const_iterator ocp_slab_it = ocp_grid->begin();

  bvxm_voxel_grid<float>::iterator preX_slab_it = preX.begin();
  bvxm_voxel_grid<float>::iterator PLvisX_slab_it = PLvisX.begin();

  for (unsigned k_idx=0; k_idx<(unsigned)grid_size.z(); ++k_idx, ++ocp_slab_it, ++preX_slab_it, ++PLvisX_slab_it)
  {
    vcl_cout << '.';

    // backproject image onto voxel plane
    bvxm_util::warp_slab_bilinear(image_slab, H_plane_to_img[k_idx], frame_backproj);

#ifdef debug
    vcl_stringstream ss;
    ss << "./frame_backproj_" << k_idx <<".tiff";
    bvxm_util::write_slab_as_image(frame_backproj,ss.str());
#endif
    // transform preX to voxel plane for this level
    bvxm_util::warp_slab_bilinear(preX_accum, H_plane_to_img[k_idx], *preX_slab_it);
    // transform visX to voxel plane for this level
    bvxm_util::warp_slab_bilinear(visX_accum, H_plane_to_img[k_idx], visX);

    // initialize PLvisX with PL(X)
    vgl_point_3d<float> local_xyz = voxel_index_to_xyz(0,0,k_idx);
    bvxm_voxel_slab<float> PL = lidar_processor.prob_density(local_xyz.z(),frame_backproj);   

    // now multiply by visX
    bvxm_util::multiply_slabs(visX,PL,*PLvisX_slab_it);

    //Is this needed?
    // update appearance model, using PX*visX as the weights
    bvxm_util::multiply_slabs(visX,*ocp_slab_it,PXvisX);

    // multiply to get PLPX
    bvxm_util::multiply_slabs(PL,*ocp_slab_it,PLPX);
#ifdef debug
    vcl_stringstream ss1, ss2;
    ss1 << "PL_" << k_idx <<".tiff";
    ss2 <<"PX_" << k_idx <<".tiff";
    bvxm_util::write_slab_as_image(PL,ss1.str());
    bvxm_util::write_slab_as_image(*ocp_slab_it,ss2.str());
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
#ifdef debug
    vcl_stringstream plpx, vis, prex;
    plpx << "PLPX_" << k_idx <<".tiff";
    vis << "visX_" << k_idx <<".tiff";
    prex << "preX_" << k_idx <<".tiff";
    bvxm_util::write_slab_as_image(PLPX_img,plpx.str());
    bvxm_util::write_slab_as_image(visX_accum,vis.str());
    bvxm_util::write_slab_as_image(preX_accum,prex.str());
#endif
    // scale and offset voxel probabilities to get (1-P(X))
    // transform (1-P(X)) to image plane to accumulate visX for next level
    bvxm_util::warp_slab_bilinear(*ocp_slab_it, H_img_to_plane[k_idx], PX_img);

    if (return_mask){
      bvxm_util::add_slabs(PX_img,mask_slab,mask_slab);
    }

    // note: doing scale and offset in image domain so invalid PLxels become 1.0 and dont affect visX
    bvxm_voxel_slab<float>::iterator PX_img_it = PX_img.begin();
    visX_accum_it = visX_accum.begin();
    for (; visX_accum_it != visX_accum.end(); ++visX_accum_it, ++PX_img_it) {
      *visX_accum_it *= (1 - *PX_img_it);
    }
  }
  // now traverse a second time, computing new P(X) along the way.

  bvxm_voxel_slab<float> preX_accum_vox(grid_size.x(),grid_size.y(),1);
  bvxm_voxel_slab<float> visX_accum_vox(grid_size.x(),grid_size.y(),1);

#ifdef debug
  vcl_stringstream vis2, prex2;
  vis2 << "visX2_"  <<".tiff";
  prex2 << "preX2_" <<".tiff";
  bvxm_util::write_slab_as_image(visX_accum,vis2.str());
  bvxm_util::write_slab_as_image(preX_accum,prex2.str());
#endif

  vcl_cout << vcl_endl << "Pass 2: " << vcl_endl;
  PLvisX_slab_it = PLvisX.begin();
  preX_slab_it = preX.begin();
  bvxm_voxel_grid<ocp_datatype>::iterator ocp_slab_it2 = ocp_grid->begin();
  for (unsigned k_idx = 0; k_idx < (unsigned)grid_size.z(); ++k_idx, ++PLvisX_slab_it, ++preX_slab_it, ++ocp_slab_it2) {
    vcl_cout << '.';

    // transform preX_sum to current level
    bvxm_util::warp_slab_bilinear(preX_accum, H_plane_to_img[k_idx], preX_accum_vox);

    // transform visX_sum to current level
    bvxm_util::warp_slab_bilinear(visX_accum, H_plane_to_img[k_idx], visX_accum_vox);

    const float preX_sum_thresh = 0.01f;

    bvxm_voxel_slab<float>::const_iterator preX_it = preX_slab_it->begin(), PLvisX_it = PLvisX_slab_it->begin(), preX_sum_it = preX_accum_vox.begin(), visX_sum_it = visX_accum_vox.begin();
    bvxm_voxel_slab<float>::iterator PX_it = ocp_slab_it2->begin();

    for (; PX_it != ocp_slab_it2->end(); ++PX_it, ++preX_it, ++PLvisX_it, ++preX_sum_it, ++visX_sum_it) {
      // if preX_sum is zero at the voxel, no ray passed through the voxel (out of image)
      if (*preX_sum_it > preX_sum_thresh) {
        float multiplier = (*PLvisX_it + *preX_it) / *preX_sum_it;
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

#ifdef debug
  bvxm_util::write_slab_as_image(preX_accum,"prob.tiff");
#endif

  //Check:
  // increment the observation count
  //this->increment_observations<APM_T>(bin_index);

  return true;
}


// update voxel grid for edges with data from image/camera pair and return the edge probability density of pixel values
bool bvxm_voxel_world::update_edges(bvxm_image_metadata const& metadata)
{
  // datatype for current appearance model
  typedef bvxm_voxel_traits<EDGES>::voxel_datatype edges_datatype;

  vgl_vector_3d<unsigned int> grid_size = params_->num_voxels();

  // compute homographies from voxel planes to image coordinates and vise-versa.
  vcl_vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  vcl_vector<vgl_h_matrix_2d<double> > H_img_to_plane;
  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    for (unsigned z=0; z < (unsigned)grid_size.z(); ++z)
    {
      compute_plane_image_H(metadata.camera,z,Hp2i,Hi2p);
      H_plane_to_img.push_back(Hp2i);
      H_img_to_plane.push_back(Hi2p);
    }
  }

  // convert image to a voxel_slab
  bvxm_voxel_slab<edges_datatype> image_slab(metadata.img->ni(), metadata.img->nj(), 1);
  if (!bvxm_util::img_to_slab(metadata.img,image_slab)) {
    vcl_cerr << "error converting image to voxel slab of observation type for bvxm_voxel_type:" << EDGES << vcl_endl;
    return false;
  }

  bvxm_voxel_slab<edges_datatype> frame_backproj(grid_size.x(),grid_size.y(),1);

  // get edge probability grid
  bvxm_voxel_grid_base_sptr edges_grid_base = this->get_grid<EDGES>(0);
  bvxm_voxel_grid<edges_datatype> *edges_grid  = static_cast<bvxm_voxel_grid<edges_datatype>*>(edges_grid_base.ptr());
  bvxm_voxel_grid<edges_datatype>::iterator edges_slab_it = edges_grid->begin();

  vcl_cout << "Updating Voxels for the Edge Model: " << vcl_endl;

  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++edges_slab_it)
  {
    vcl_cout << '.';
    if ( (edges_slab_it == edges_grid->end()) ) {
      vcl_cerr << "error: reached end of grid slabs at z = " << z << ".  nz = " << grid_size.z() << vcl_endl;
      return false;
    }

    // backproject image onto voxel plane
    bvxm_util::warp_slab_bilinear(image_slab, H_plane_to_img[z], frame_backproj);

    bvxm_voxel_slab<edges_datatype>::iterator frame_backproj_it = frame_backproj.begin();
    bvxm_voxel_slab<edges_datatype>::iterator edges_slab_it_it = (*edges_slab_it).begin();

    for (; frame_backproj_it != frame_backproj.end(); ++frame_backproj_it, ++edges_slab_it_it) {
      (*edges_slab_it_it) = (*edges_slab_it_it)*(*frame_backproj_it);
    }
  }
  vcl_cout << vcl_endl;
  return true;
}

bool bvxm_voxel_world::expected_edge_image(bvxm_image_metadata const& camera,vil_image_view_base_sptr &expected)
{
  // datatype for current appearance model
  typedef bvxm_voxel_traits<EDGES>::voxel_datatype edges_datatype;

  // extract global parameters
  vgl_vector_3d<unsigned int> grid_size = params_->num_voxels();

  // compute homographies from voxel planes to image coordinates and vise-versa.
  vcl_vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  vcl_vector<vgl_h_matrix_2d<double> > H_img_to_plane;
  for (unsigned z=0; z < (unsigned)grid_size.z(); ++z)
  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    compute_plane_image_H(camera.camera,z,Hp2i,Hi2p);
    H_plane_to_img.push_back(Hp2i);
    H_img_to_plane.push_back(Hi2p);
  }

  // allocate some images
  bvxm_voxel_slab<edges_datatype> expected_edge_image(expected->ni(),expected->nj(),1);
  bvxm_voxel_slab<edges_datatype> slice_edges(expected->ni(),expected->nj(),1);

  expected_edge_image.fill(0.0f);

  // get edges probability grid
  bvxm_voxel_grid_base_sptr edges_grid_base = this->get_grid<EDGES>(0);
  bvxm_voxel_grid<edges_datatype> *edges_grid  = static_cast<bvxm_voxel_grid<edges_datatype>*>(edges_grid_base.ptr());

  bvxm_voxel_grid<edges_datatype>::const_iterator edges_slab_it(edges_grid->begin());

  vcl_cout << "Generating Expected Edge Image: " << vcl_endl;
  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++edges_slab_it) {
    vcl_cout << '.';
    // warp slice_probability to image plane
    bvxm_util::warp_slab_bilinear(*edges_slab_it, H_img_to_plane[z], slice_edges);

    bvxm_voxel_slab<edges_datatype>::const_iterator slice_edges_it = slice_edges.begin();
    bvxm_voxel_slab<edges_datatype>::iterator expected_edge_image_it = expected_edge_image.begin();

    for (; expected_edge_image_it != expected_edge_image.end(); ++slice_edges_it, ++expected_edge_image_it) {
      (*expected_edge_image_it) = vnl_math_max((*expected_edge_image_it),(*slice_edges_it));
    }
  }
  vcl_cout << vcl_endl;

  // convert back to vil_image_view
  bvxm_util::slab_to_img(expected_edge_image, expected);

  return true;
}

//: generate a heightmap from the viewpoint of a virtual camera
// The pixel values are the z values of the most likely voxel intercepted by the corresponding camera ray
bool bvxm_voxel_world::heightmap(vpgl_camera_double_sptr virtual_camera, vil_image_view<unsigned> &heightmap)
{
  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  // extract global parameters
  vgl_vector_3d<unsigned int> grid_size = params_->num_voxels();


  // compute homographies from voxel planes to image coordinates and vise-versa.
  vcl_vector<vgl_h_matrix_2d<double> > H_plane_to_virtual_img;
  vcl_vector<vgl_h_matrix_2d<double> > H_virtual_img_to_plane;

  for (unsigned z=0; z < (unsigned)grid_size.z(); ++z)
  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    compute_plane_image_H(virtual_camera,z,Hp2i,Hi2p);
    H_plane_to_virtual_img.push_back(Hp2i);
    H_virtual_img_to_plane.push_back(Hi2p);
  }

  // allocate some images
  bvxm_voxel_slab<float> visX_accum_virtual(heightmap.ni(), heightmap.nj(),1);
  bvxm_voxel_slab<unsigned> heightmap_rough(heightmap.ni(),heightmap.nj(),1);
  bvxm_voxel_slab<float> max_prob_image(heightmap.ni(), heightmap.nj(), 1);
  bvxm_voxel_slab<ocp_datatype> slice_prob_img(heightmap.ni(),heightmap.nj(),1);

  heightmap_rough.fill(grid_size.z());
  visX_accum_virtual.fill(1.0f);
  max_prob_image.fill(0.0f);

  // get ocuppancy probability grid
  bvxm_voxel_grid_base_sptr ocp_grid_base = this->get_grid<OCCUPANCY>(0);
  bvxm_voxel_grid<ocp_datatype> *ocp_grid  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(ocp_grid_base.ptr());

  bvxm_voxel_grid<ocp_datatype>::const_iterator ocp_slab_it = ocp_grid->begin();

  vcl_cout << "generating height map from virtual camera: " << vcl_endl;
  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++ocp_slab_it) {
    vcl_cout << '.';

    // compute PXvisX for virtual camera and update visX
    bvxm_util::warp_slab_bilinear(*ocp_slab_it,H_virtual_img_to_plane[z],slice_prob_img);
    bvxm_voxel_slab<ocp_datatype>::const_iterator PX_it = slice_prob_img.begin();
    bvxm_voxel_slab<float>::iterator max_it = max_prob_image.begin(), visX_it = visX_accum_virtual.begin();
    bvxm_voxel_slab<unsigned>::iterator hmap_it = heightmap_rough.begin();

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
  bvxm_voxel_slab<float> heightmap_filtered(heightmap.ni(),heightmap.nj(),1);
  bvxm_voxel_slab<bool> conf_mask(heightmap.ni(),heightmap.nj(),1);
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

  // convert back to unsigned
  vil_image_view<unsigned>::iterator hmap_it = heightmap.begin();
  hmap_filt_it = heightmap_filtered.begin();
  for (; hmap_it != heightmap.end(); ++hmap_filt_it, ++hmap_it) {
    *hmap_it = (unsigned)(*hmap_filt_it); // should we do some rounding here?
  }

  return true;
}

vgl_point_3d<float> bvxm_voxel_world::voxel_index_to_xyz(unsigned vox_i, unsigned vox_j, unsigned vox_k)
{
  float vox_len = params_->voxel_length();
  vgl_vector_3d<unsigned> num_vox = params_->num_voxels();

  // corner in parameters refers to the bottom. we want the top since slice 0 is the top-most slice.
  vgl_point_3d<float> grid_origin = params_->corner() + vgl_vector_3d<float>(0.5f, 0.5f, vox_len*(num_vox.z() - 0.5f));
  vgl_vector_3d<float> step_i(vox_len, 0.0f, 0.0f);
  vgl_vector_3d<float> step_j(0.0f, vox_len, 0.0f);
  vgl_vector_3d<float> step_k(0.0f, 0.0f, -vox_len);

  return grid_origin + step_i*vox_i + step_j*vox_j + step_k*vox_k;
}

void bvxm_voxel_world::compute_plane_image_H(vpgl_camera_double_sptr const& cam, unsigned k_idx, vgl_h_matrix_2d<double> &H_plane_to_image, vgl_h_matrix_2d<double> &H_image_to_plane)
{

    vgl_vector_3d<unsigned int> grid_size = params_->num_voxels();
    
    vcl_vector<vgl_homg_point_2d<double> > voxel_corners_img;
    vcl_vector<vgl_homg_point_2d<double> > voxel_corners_vox;

    // create vectors containing four corners of grid, and their projections into the image
    double u=0, v=0;
    vgl_point_3d<float> corner_world;

    voxel_corners_vox.push_back(vgl_homg_point_2d<double>(0,0));
    corner_world = this->voxel_index_to_xyz(0,0,k_idx);
    cam->project(corner_world.x(),corner_world.y(),corner_world.z(),u,v);
    voxel_corners_img.push_back(vgl_homg_point_2d<double>(u,v));

    voxel_corners_vox.push_back(vgl_homg_point_2d<double>(grid_size.x()-1,0));
    corner_world = this->voxel_index_to_xyz(grid_size.x()-1,0,k_idx);
    cam->project(corner_world.x(),corner_world.y(),corner_world.z(),u,v);
    voxel_corners_img.push_back(vgl_homg_point_2d<double>(u,v));

    voxel_corners_vox.push_back(vgl_homg_point_2d<double>(grid_size.x()-1,grid_size.y()-1));
    corner_world = this->voxel_index_to_xyz(grid_size.x()-1,grid_size.y()-1,k_idx);
    cam->project(corner_world.x(),corner_world.y(),corner_world.z(),u,v);
    voxel_corners_img.push_back(vgl_homg_point_2d<double>(u,v));

    voxel_corners_vox.push_back(vgl_homg_point_2d<double>(0,(grid_size.y()-1)));
    corner_world = this->voxel_index_to_xyz(0,grid_size.y()-1,k_idx);
    cam->project(corner_world.x(),corner_world.y(),corner_world.z(),u,v);
    voxel_corners_img.push_back(vgl_homg_point_2d<double>(u,v));


    vgl_h_matrix_2d_compute_linear comp_4pt;
    if (!comp_4pt.compute(voxel_corners_img,voxel_corners_vox, H_image_to_plane)) {
      vcl_cerr << "ERROR computing homography from image to voxel slice. " << vcl_endl;
    }
    if (!comp_4pt.compute(voxel_corners_vox,voxel_corners_img, H_plane_to_image)) {
      vcl_cerr << "ERROR computing homography from voxel slice to image. " << vcl_endl;
    }
    return;
}