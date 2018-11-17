#include <iostream>
#include <fstream>
#include <sstream>
#include "bvxm_edge_ray_processor.h"
//:
// \file
#include <sdet/sdet_img_edge.h>
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_voxel_traits.h>
#include <bvxm/bvxm_world_params.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_voxel_slab.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_util.h>
#include <bsta/bsta_histogram.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_closest_point.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_quaternion.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static const bool edge_debug = false;
static const bool print_arrays = false;
static std::string base = "";
#if 0
static std::ofstream dos((base + "bproj_planes_44_24.txt").c_str());
#endif

// initialize the voxel grid for edges
bool bvxm_edge_ray_processor::init_edges(unsigned scale)
{
  if (world_->num_observations<EDGES>(0,scale)!=0) {
    return false;
  }

  // datatype for current appearance model
  typedef bvxm_voxel_traits<EDGES>::voxel_datatype edges_datatype;

  bvxm_world_params_sptr params = world_->get_params();
  vgl_vector_3d<unsigned int> grid_size = params->num_voxels(scale);

  // get edge probability grid
  bvxm_voxel_grid_base_sptr edges_voxel_base = world_->get_grid<EDGES>(0, scale);
  auto *edges_voxel  = static_cast<bvxm_voxel_grid<edges_datatype>*>(edges_voxel_base.ptr());

  // Pass 1: to build the marginal
  std::cout << "Initializing the voxel world:" << std::endl;
  bvxm_voxel_grid<edges_datatype>::iterator edges_voxel_it = edges_voxel->begin();
  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++edges_voxel_it)
  {
    std::cout << '.';
    if ( (edges_voxel_it == edges_voxel->end()) ) {
      std::cerr << "error: reached end of grid slabs at z = " << z << ".  nz = " << grid_size.z() << '\n';
      return false;
    }

    bvxm_voxel_slab<edges_datatype>::iterator edges_voxel_it_it = (*edges_voxel_it).begin();

    for (; edges_voxel_it_it != (*edges_voxel_it).end(); ++edges_voxel_it_it) {
      (*edges_voxel_it_it) = 0.0f;
    }
  }
  std::cout << "\nDone\n";

  return true;
}

// update voxel grid for edges with data from image/camera pair and return the edge probability density of pixel values
bool bvxm_edge_ray_processor::update_edges(bvxm_image_metadata const& metadata, unsigned scale)
{
  if (world_->num_observations<EDGES>(0,scale)==0) {
    this->init_edges(scale);
  }

  // datatype for current appearance model
  typedef bvxm_voxel_traits<EDGES>::voxel_datatype edges_datatype;
  bvxm_world_params_sptr params = world_->get_params();
  vgl_vector_3d<unsigned int> grid_size = params->num_voxels(scale);

  // compute homographies from voxel planes to image coordinates and vise-versa.
  std::vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  std::vector<vgl_h_matrix_2d<double> > H_img_to_plane;
  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    for (unsigned z=0; z < (unsigned)grid_size.z(); ++z)
    {
      world_->compute_plane_image_H(metadata.camera,z,Hp2i,Hi2p,scale);
      H_plane_to_img.push_back(Hp2i);
      H_img_to_plane.push_back(Hi2p);
    }
  }

  // convert image to a voxel_slab
  bvxm_voxel_slab<edges_datatype> image_voxel(grid_size.x(),grid_size.y(),1);
  bvxm_voxel_slab<edges_datatype> image_image(metadata.img->ni(), metadata.img->nj(), 1);
  if (!bvxm_util::img_to_slab(metadata.img,image_image)) {
    std::cerr << "error converting image to voxel slab of observation type for bvxm_voxel_type " << (int)EDGES << '\n';
    return false;
  }

  // get edge probability grid
  bvxm_voxel_grid_base_sptr edges_voxel_base = world_->get_grid<EDGES>(0, scale);
  auto *edges_voxel  = static_cast<bvxm_voxel_grid<edges_datatype>*>(edges_voxel_base.ptr());

  // Pass 1: to build the marginal
  std::cout << "Pass 1 of 1:" << std::endl;
  bvxm_voxel_grid<edges_datatype>::iterator edges_voxel_it = edges_voxel->begin();
  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++edges_voxel_it)
  {
    std::cout << '.';
    if ( (edges_voxel_it == edges_voxel->end()) ) {
      std::cerr << "error: reached end of grid slabs at z = " << z << ".  nz = " << grid_size.z() << '\n';
      return false;
    }

    bvxm_util::warp_slab_bilinear(image_image,H_plane_to_img[z],image_voxel);

    bvxm_voxel_slab<edges_datatype>::iterator image_voxel_it = image_voxel.begin();
    bvxm_voxel_slab<edges_datatype>::iterator edges_voxel_it_it = (*edges_voxel_it).begin();

    for (; image_voxel_it != image_voxel.end(); ++image_voxel_it, ++edges_voxel_it_it) {
      (*edges_voxel_it_it) = (*edges_voxel_it_it) + (*image_voxel_it);
    }
  }
  std::cout << std::endl;

  world_->increment_observations<EDGES>(0,scale);

  return true;
}

bool bvxm_edge_ray_processor::update_edges_with_Lidar_surface(bvxm_image_metadata const& metadata, unsigned scale)
{
  if (world_->num_observations<EDGES>(0,scale)==0) {
    this->init_edges(scale);
  }

  // datatype for current appearance model
  typedef bvxm_voxel_traits<EDGES>::voxel_datatype edges_datatype;
  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;
  bvxm_world_params_sptr params = world_->get_params();
  vgl_vector_3d<unsigned int> grid_size = params->num_voxels(scale);

  // compute homographies from voxel planes to image coordinates and vise-versa.
  std::vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  std::vector<vgl_h_matrix_2d<double> > H_img_to_plane;
  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    for (unsigned z=0; z < (unsigned)grid_size.z(); ++z)
    {
      world_->compute_plane_image_H(metadata.camera,z,Hp2i,Hi2p,scale);
      H_plane_to_img.push_back(Hp2i);
      H_img_to_plane.push_back(Hi2p);
    }
  }

  // convert image to a voxel_slab
  bvxm_voxel_slab<edges_datatype> image_voxel(grid_size.x(),grid_size.y(),1);
  bvxm_voxel_slab<edges_datatype> image_image(metadata.img->ni(), metadata.img->nj(), 1);
  if (!bvxm_util::img_to_slab(metadata.img,image_image)) {
    std::cerr << "error converting image to voxel slab of observation type for bvxm_voxel_type " << (int)EDGES << '\n';
    return false;
  }

  // get edge probability grid
  bvxm_voxel_grid_base_sptr edges_voxel_base = world_->get_grid<EDGES>(0, scale);
  auto *edges_voxel  = static_cast<bvxm_voxel_grid<edges_datatype>*>(edges_voxel_base.ptr());
  //: get lidar surface grid
  bvxm_voxel_grid_base_sptr voxel_base = world_->get_grid<OCCUPANCY>(0, scale);
  auto *surface_voxel  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(voxel_base.ptr());

  // Pass 1: to build the marginal
  std::cout << "Pass 1 of 1:" << std::endl;
  bvxm_voxel_grid<edges_datatype>::iterator edges_voxel_it = edges_voxel->begin();
  bvxm_voxel_grid<ocp_datatype>::iterator voxel_it = surface_voxel->begin();
  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++edges_voxel_it,++voxel_it)
  {
    std::cout << '.';
    if ( (edges_voxel_it == edges_voxel->end()) ) {
      std::cerr << "error: reached end of grid slabs at z = " << z << ".  nz = " << grid_size.z() << '\n';
      return false;
    }

    bvxm_util::warp_slab_bilinear(image_image,H_plane_to_img[z],image_voxel);

    bvxm_voxel_slab<edges_datatype>::iterator image_voxel_it = image_voxel.begin();
    bvxm_voxel_slab<edges_datatype>::iterator edges_voxel_it_it = (*edges_voxel_it).begin();
    bvxm_voxel_slab<edges_datatype>::iterator voxel_it_it = (*voxel_it).begin();

    for (; image_voxel_it != image_voxel.end(); ++image_voxel_it, ++edges_voxel_it_it,++voxel_it_it) {
      (*edges_voxel_it_it) = (*edges_voxel_it_it) + (*image_voxel_it)*((*voxel_it_it)*2-1);
    }
  }
  std::cout << std::endl;

  world_->increment_observations<EDGES>(0,scale);

  return true;
}

bool bvxm_edge_ray_processor::expected_edge_image(bvxm_image_metadata const& camera,vil_image_view_base_sptr &expected, float n_normal, unsigned scale)
{
  // datatype for current appearance model
  typedef bvxm_voxel_traits<EDGES>::voxel_datatype edges_datatype;

  // extract global parameters
  bvxm_world_params_sptr params = world_->get_params();
  vgl_vector_3d<unsigned int> grid_size = params->num_voxels(scale);

  // compute homographies from voxel planes to image coordinates and vise-versa.
  std::vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  std::vector<vgl_h_matrix_2d<double> > H_img_to_plane;
  for (unsigned z=0; z < (unsigned)grid_size.z(); ++z)
  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    world_->compute_plane_image_H(camera.camera,z,Hp2i,Hi2p, scale);
    H_plane_to_img.push_back(Hp2i);
    H_img_to_plane.push_back(Hi2p);
  }

  // allocate some images
  bvxm_voxel_slab<edges_datatype> edges_image(expected->ni(),expected->nj(),1);

  bvxm_voxel_slab<edges_datatype> expected_edge_image(expected->ni(),expected->nj(),1);
  expected_edge_image.fill(0.0f);

  // get edges probability grid
  bvxm_voxel_grid_base_sptr edges_voxel_base = world_->get_grid<EDGES>(0, scale);
  auto *edges_voxel  = static_cast<bvxm_voxel_grid<edges_datatype>*>(edges_voxel_base.ptr());

  bvxm_voxel_grid<edges_datatype>::iterator edges_voxel_it(edges_voxel->begin());

  std::cout << "Generating Expected Edge Image:" << std::endl;
  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++edges_voxel_it) {
    std::cout << '.';

    bvxm_util::warp_slab_bilinear((*edges_voxel_it), H_img_to_plane[z], edges_image);

    bvxm_voxel_slab<edges_datatype>::iterator edges_image_it = edges_image.begin();
    bvxm_voxel_slab<edges_datatype>::iterator expected_edge_image_it = expected_edge_image.begin();

    for (; expected_edge_image_it != expected_edge_image.end(); ++expected_edge_image_it, ++edges_image_it) {
      (*expected_edge_image_it) = std::max((*expected_edge_image_it),(*edges_image_it));
    }
  }
  std::cout << std::endl;

  int dof = (int)world_->num_observations<EDGES>(0,scale)-1;
  bvxm_voxel_slab<edges_datatype>::iterator expected_edge_image_it = expected_edge_image.begin();
  float eei_min = std::numeric_limits<float>::max();
  float eei_max = std::numeric_limits<float>::min();
  for (; expected_edge_image_it != expected_edge_image.end(); ++expected_edge_image_it) {
    (*expected_edge_image_it) = sdet_img_edge::convert_edge_statistics_to_probability((*expected_edge_image_it),n_normal,dof);
    eei_min = std::min(eei_min,*expected_edge_image_it);
    eei_max = std::max(eei_max,*expected_edge_image_it);
  }

  if (eei_min<eei_max) {
    for (; expected_edge_image_it != expected_edge_image.end(); ++expected_edge_image_it) {
      *expected_edge_image_it = (*expected_edge_image_it-eei_min)/(eei_max/eei_min);
    }
  }

  // convert back to vil_image_view
  bvxm_util::slab_to_img(expected_edge_image, expected);

  return true;
}

//: generate the expected edge image from the specified viewpoint. the expected image should be allocated by the caller.
//  return the height of the voxel where the expected edge image probability is greatest
bool bvxm_edge_ray_processor::expected_edge_image_and_heights(bvxm_image_metadata const& camera,vil_image_view_base_sptr &expected, vil_image_view_base_sptr &height_img, float n_normal, unsigned scale)
{
    // datatype for current appearance model
  typedef bvxm_voxel_traits<EDGES>::voxel_datatype edges_datatype;

  // extract global parameters
  bvxm_world_params_sptr params = world_->get_params();
  vgl_vector_3d<unsigned int> grid_size = params->num_voxels(scale);

  // compute homographies from voxel planes to image coordinates and vise-versa.
  std::vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  std::vector<vgl_h_matrix_2d<double> > H_img_to_plane;
  for (unsigned z=0; z < (unsigned)grid_size.z(); ++z)
  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    world_->compute_plane_image_H(camera.camera,z,Hp2i,Hi2p, scale);
    H_plane_to_img.push_back(Hp2i);
    H_img_to_plane.push_back(Hi2p);
  }

  // allocate some images
  bvxm_voxel_slab<edges_datatype> edges_image(expected->ni(),expected->nj(),1);

  bvxm_voxel_slab<edges_datatype> expected_edge_image(expected->ni(),expected->nj(),1);
  expected_edge_image.fill(0.0f);

  bvxm_voxel_slab<float> z_image(expected->ni(), expected->nj(),1);
  z_image.fill(0.0f);

  // get edges probability grid
  bvxm_voxel_grid_base_sptr edges_voxel_base = world_->get_grid<EDGES>(0, scale);
  auto *edges_voxel  = static_cast<bvxm_voxel_grid<edges_datatype>*>(edges_voxel_base.ptr());

  bvxm_voxel_grid<edges_datatype>::iterator edges_voxel_it(edges_voxel->begin());

  std::cout << "Generating Expected Edge Image:" << std::endl;
  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++edges_voxel_it) {
    std::cout << '.';

    bvxm_util::warp_slab_bilinear((*edges_voxel_it), H_img_to_plane[z], edges_image);

    bvxm_voxel_slab<edges_datatype>::iterator edges_image_it = edges_image.begin();
    bvxm_voxel_slab<edges_datatype>::iterator expected_edge_image_it = expected_edge_image.begin();
    bvxm_voxel_slab<float>::iterator z_image_it = z_image.begin();

    for (; expected_edge_image_it != expected_edge_image.end(); ++expected_edge_image_it, ++edges_image_it, ++z_image_it) {
      //(*expected_edge_image_it) = std::max((*expected_edge_image_it),(*edges_image_it));
      if ((*expected_edge_image_it) < (*edges_image_it)) {
        (*expected_edge_image_it) = (*edges_image_it);
        (*z_image_it) = (float)z;
      }
    }
  }
  std::cout << std::endl;

  int dof = (int)world_->num_observations<EDGES>(0,scale)-1;
  bvxm_voxel_slab<edges_datatype>::iterator expected_edge_image_it = expected_edge_image.begin();
  float eei_min = std::numeric_limits<float>::max();
  float eei_max = std::numeric_limits<float>::min();
  for (; expected_edge_image_it != expected_edge_image.end(); ++expected_edge_image_it) {
    (*expected_edge_image_it) = sdet_img_edge::convert_edge_statistics_to_probability((*expected_edge_image_it),n_normal,dof);
    eei_min = std::min(eei_min,*expected_edge_image_it);
    eei_max = std::max(eei_max,*expected_edge_image_it);
  }

  if (eei_min<eei_max) {
    for (; expected_edge_image_it != expected_edge_image.end(); ++expected_edge_image_it) {
      *expected_edge_image_it = (*expected_edge_image_it-eei_min)/(eei_max/eei_min);
    }
  }

  // convert back to vil_image_view
  bvxm_util::slab_to_img(expected_edge_image, expected);
  bvxm_util::slab_to_img(z_image, height_img);

  return true;
}


//: save the edge probability grid as an 8-bit 3-d vff image
bool bvxm_edge_ray_processor::save_edges_vff(const std::string& filename,unsigned scale)
{
  // open file for binary writing
  std::fstream ofs(filename.c_str(),std::ios::binary | std::ios::out);
  if (!ofs.is_open()) {
    std::cerr << "error opening file " << filename << " for write!\n";
    return false;
  }
  bvxm_world_params_sptr params = world_->get_params();

  typedef bvxm_voxel_traits<EDGES>::voxel_datatype edges_datatype;

  bvxm_voxel_grid<edges_datatype> *edges_grid =
    dynamic_cast<bvxm_voxel_grid<edges_datatype>*>(world_->get_grid<EDGES>(0,scale).ptr());

  vxl_uint_32 nx = edges_grid->grid_size().x();
  vxl_uint_32 ny = edges_grid->grid_size().y();
  vxl_uint_32 nz = edges_grid->grid_size().z();

  // write header
  std::stringstream header;
  header << "ncaa\n"
         << "title=bvxm edge probabilities;\n"
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
  char *edges_array = new char[nx*ny*nz];

  bvxm_voxel_grid<edges_datatype>::iterator edges_it = edges_grid->begin();
  for (unsigned k=nz-1; edges_it != edges_grid->end(); ++edges_it, --k) {
    std::cout << '.';
    for (unsigned i=0; i<(*edges_it).nx(); ++i) {
      for (unsigned j=0; j < (*edges_it).ny(); ++j) {
        edges_array[k*nx*ny + j*nx + i] = (unsigned char)((*edges_it)(i,j) * 255.0);;
      }
    }
  }
  std::cout << std::endl;
  ofs.write(reinterpret_cast<char*>(edges_array),sizeof(unsigned char)*nx*ny*nz);

  ofs.close();

  delete[] edges_array;

  return true;
}

//: save the edge probability grid in a ".raw" format readable by Drishti volume rendering software
bool bvxm_edge_ray_processor::save_edges_raw(const std::string& filename, float n_normal, unsigned scale)
{
  std::fstream ofs(filename.c_str(),std::ios::binary | std::ios::out);
  if (!ofs.is_open()) {
    std::cerr << "error opening file " << filename << " for write!\n";
    return false;
  }
  typedef bvxm_voxel_traits<EDGES>::voxel_datatype edges_datatype;

  // write header
  unsigned char data_type = 0; // 0 means unsigned byte

  bvxm_voxel_grid<edges_datatype> *edges_grid =
    dynamic_cast<bvxm_voxel_grid<edges_datatype>*>(world_->get_grid<EDGES>(0,scale).ptr());

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

  int dof = (int)world_->num_observations<EDGES>(0,scale)-1;

  std::cout << "Saving edges to RAW file:" << std::endl;
  bvxm_voxel_grid<edges_datatype>::iterator edges_it = edges_grid->begin();
  for (unsigned k=0; edges_it != edges_grid->end(); ++edges_it, ++k) {
    std::cout << '.';
    for (unsigned i=0; i<(*edges_it).nx(); ++i) {
      for (unsigned j=0; j < (*edges_it).ny(); ++j) {
        edges_array[i*ny*nz + j*nz + k] = (unsigned char)(255.0*sdet_img_edge::convert_edge_statistics_to_probability((*edges_it)(i,j),n_normal,dof));
      }
    }
  }
  std::cout << std::endl;
  ofs.write(reinterpret_cast<char*>(edges_array),sizeof(unsigned char)*nx*ny*nz);

  ofs.close();

  delete[] edges_array;

  return true;
}

bool bvxm_edge_ray_processor::
init_von_mises_edge_tangents(bvxm_image_metadata const& metadata0,
                             bvxm_image_metadata const& metadata1,
                             double initial_sd_ratio,
                             double initial_kappa,
                             unsigned scale)
{
  typedef bvxm_voxel_traits<TANGENT_POS>::voxel_datatype pos_dist_t;
  typedef bvxm_voxel_traits<TANGENT_POS>::obs_type pos_t;
  typedef bvxm_voxel_traits<TANGENT_DIR>::voxel_datatype dir_dist_t;
  typedef bvxm_voxel_traits<TANGENT_DIR>::obs_type dir_t;
  typedef bvxm_voxel_traits<TANGENT_DIR>::math_type math_t;
  // grid size
  bvxm_world_params_sptr params = world_->get_params();
  vgl_vector_3d<unsigned int> grid_size = params->num_voxels(scale);
  auto nx = static_cast<unsigned>(grid_size.x()),
    ny = static_cast<unsigned>(grid_size.y()), nz = static_cast<unsigned>(grid_size.z());
  double radius = 0.866*params->voxel_length()*initial_sd_ratio;
  if (edge_debug)
    std::cout << "Initializing a world " << nx << 'x'
             << ny << 'x' << nz << '\n';
  bvxm_voxel_grid_base_sptr tangent_pos_base = world_->get_grid<TANGENT_POS>(0, scale);
  auto *pos_dist_grid  =
    static_cast<bvxm_voxel_grid<pos_dist_t>*>(tangent_pos_base.ptr());
  // 3-d tangent direction grid distributions
  bvxm_voxel_grid_base_sptr tangent_dir_base = world_->get_grid<TANGENT_DIR>(0, scale);
  auto *dir_dist_grid  =
    static_cast<bvxm_voxel_grid<dir_dist_t>*>(tangent_dir_base.ptr());

  if (world_->num_observations<TANGENT_POS>(0,scale)!=0) {
    bvxm_voxel_grid<pos_dist_t>::iterator pos_dist_it=pos_dist_grid->begin();
    for (unsigned z = 0; z<nz; ++z, ++pos_dist_it) {
      // go through the grid and reset
      for (unsigned r = 0; r<ny; ++r)
        for (unsigned c = 0; c<nx; ++c) {
          ((*pos_dist_it)(c, r)).set_mean(pos_t(static_cast<math_t>(0)));
          ((*pos_dist_it)(c, r)).set_var(static_cast<math_t>(radius*radius));
          ((*pos_dist_it)(c, r)).num_observations = static_cast<math_t>(0);
        }
    }
    world_->zero_observations<TANGENT_POS>(0,scale);
  }
  if (world_->num_observations<TANGENT_DIR>(0,scale)!=0) {
    bvxm_voxel_grid<dir_dist_t>::iterator dir_dist_it=dir_dist_grid->begin();
    for (unsigned z = 0; z<(unsigned)grid_size.z(); ++z, ++dir_dist_it) {
      // go through the grid and reset
      for (unsigned r = 0; r<ny; ++r)
        for (unsigned c = 0; c<nx; ++c) {
          ((*dir_dist_it)(c, r)).set_mean(dir_t(static_cast<math_t>(0)));
          ((*dir_dist_it)(c, r)).set_kappa(static_cast<math_t>(initial_kappa));
          ((*dir_dist_it)(c, r)).num_observations = static_cast<math_t>(0);
          ((*dir_dist_it)(c, r)).vector_sum = dir_t(static_cast<math_t>(0));
        }
    }
    world_->zero_observations<TANGENT_DIR>(0,scale);
  }
  // extract the tangent-point images
  auto* tan_image0 =
    static_cast<vil_image_view<float>*>(metadata0.img.ptr());
  auto* tan_image1 =
    static_cast<vil_image_view<float>*>(metadata1.img.ptr());

  // ray processor for update and utilities
  bvxm_von_mises_tangent_processor<float> tan_proc;

  // compute homographies from voxel planes to image coordinates and vise-versa.
  std::vector<vgl_h_matrix_2d<double> > H_plane_to_img0,H_plane_to_img1;
  std::vector<vgl_h_matrix_2d<double> > H_img_to_plane0,H_img_to_plane1;
  {
    vgl_h_matrix_2d<double> Hp2i0, Hi2p0,Hp2i1, Hi2p1;
    for (unsigned z=0; z < (unsigned)grid_size.z(); ++z)
    {
      world_->compute_plane_image_H(metadata0.camera,z,Hp2i0,Hi2p0,scale);
      world_->compute_plane_image_H(metadata1.camera,z,Hp2i1,Hi2p1,scale);
      H_plane_to_img0.push_back(Hp2i0);
      H_img_to_plane0.push_back(Hi2p0);
      H_plane_to_img1.push_back(Hp2i1);
      H_img_to_plane1.push_back(Hi2p1);
    }
  }
  // get projective cameras (warning! needs to be upgraded for RPC -- later)
  auto* cam0 =
    static_cast<vpgl_proj_camera<double>*>(metadata0.camera.ptr());
  auto* cam1 =
    static_cast<vpgl_proj_camera<double>*>(metadata1.camera.ptr());

  // extract slabs from tangent-point images
  unsigned ni0 = metadata0.img->ni(), nj0 = metadata0.img->nj();
  unsigned ni1 = metadata1.img->ni(), nj1 = metadata1.img->nj();
  bvxm_voxel_slab<float> image_a_slab0(ni0, nj0 , 1);
  bvxm_voxel_slab<float> image_b_slab0(ni0, nj0 , 1);
  bvxm_voxel_slab<float> image_c_slab0(ni0, nj0 , 1);
  for (unsigned j = 0; j<nj0; ++j)
    for (unsigned i = 0; i<ni0; ++i) {
      image_a_slab0(i,j)=(*tan_image0)(i,j,0);
      image_b_slab0(i,j)=(*tan_image0)(i,j,1);
      image_c_slab0(i,j)=(*tan_image0)(i,j,2);
    }
  bvxm_voxel_slab<float> image_a_slab1(ni1, nj1 , 1);
  bvxm_voxel_slab<float> image_b_slab1(ni1, nj1 , 1);
  bvxm_voxel_slab<float> image_c_slab1(ni1, nj1 , 1);
  for (unsigned j = 0; j<nj1; ++j)
    for (unsigned i = 0; i<ni1; ++i) {
      image_a_slab1(i,j)=(*tan_image1)(i,j,0);
      image_b_slab1(i,j)=(*tan_image1)(i,j,1);
      image_c_slab1(i,j)=(*tan_image1)(i,j,2);
    }

  // slabs for later backprojection
  bvxm_voxel_slab<float> bproj_image_a_slab0(nx, ny,1);
  bvxm_voxel_slab<float> bproj_image_b_slab0(nx, ny,1);
  bvxm_voxel_slab<float> bproj_image_c_slab0(nx, ny,1);
  bvxm_voxel_slab<float> bproj_image_a_slab1(nx, ny,1);
  bvxm_voxel_slab<float> bproj_image_b_slab1(nx, ny,1);
  bvxm_voxel_slab<float> bproj_image_c_slab1(nx, ny,1);
  bvxm_voxel_slab<pos_t> voxel_pos_slab(nx, ny,1);
  bvxm_voxel_slab<dir_t> voxel_dir_slab(nx, ny,1);
  bvxm_voxel_slab<bool> voxel_flag_slab(nx, ny,1);

  // 3-d tangent position grid distributions

  if (edge_debug)
    std::cout << "Initializing the tangent world:" << std::endl;
  bvxm_voxel_grid<pos_dist_t>::iterator pos_dist_it = pos_dist_grid->begin();
  bvxm_voxel_grid<dir_dist_t>::iterator dir_dist_it = dir_dist_grid->begin();
  for (unsigned z=0; z<(unsigned)grid_size.z();++z,++pos_dist_it,++dir_dist_it)
  {
    if (edge_debug)
      std::cout << "processing slab " << z << '\n';
    if ( (pos_dist_it == pos_dist_grid->end())||
         (dir_dist_it == dir_dist_grid->end()) ) {
      std::cerr << "error: reached end of tangent slabs at z = " << z << ".  nz = " << grid_size.z() << '\n';
      return false;
    }
    voxel_pos_slab.fill(pos_t(0.0f));
    voxel_dir_slab.fill(dir_t(0.0f));
    voxel_flag_slab.fill(false);
    // backproject image line coefficients
    bvxm_util::warp_slab_nearest_neighbor(image_a_slab0, H_plane_to_img0[z],
                                          bproj_image_a_slab0);
    bvxm_util::warp_slab_nearest_neighbor(image_b_slab0, H_plane_to_img0[z],
                                          bproj_image_b_slab0);
    bvxm_util::warp_slab_nearest_neighbor(image_c_slab0, H_plane_to_img0[z],
                                          bproj_image_c_slab0);
    if (true||edge_debug) {
      bvxm_util::write_slab_as_image(bproj_image_a_slab0,
                                     base+"Img0a.tiff");
      bvxm_util::write_slab_as_image(bproj_image_b_slab0,
                                     base+"Img0b.tiff");
      bvxm_util::write_slab_as_image(bproj_image_c_slab0,
                                     base+"Img0c.tiff");
    }
    if (edge_debug&&print_arrays) {
      std::cout.precision(2);
      std::cout << "back proj view 0\n";
      for (unsigned j = 0; j<ny; j++) {
        for (unsigned i = 0; i<nx; i++)
          std::cout << bproj_image_a_slab0(i,j) << ' ';
        std::cout << '\n';
      }
    }
    bvxm_util::warp_slab_nearest_neighbor(image_a_slab1, H_plane_to_img1[z],
                                          bproj_image_a_slab1);
    bvxm_util::warp_slab_nearest_neighbor(image_b_slab1, H_plane_to_img1[z],
                                          bproj_image_b_slab1);
    bvxm_util::warp_slab_nearest_neighbor(image_c_slab1, H_plane_to_img1[z],
                                          bproj_image_c_slab1);
    if (true||edge_debug) {
      bvxm_util::write_slab_as_image(bproj_image_a_slab1,
                                     base+"Img1a.tiff");
      bvxm_util::write_slab_as_image(bproj_image_b_slab1,
                                     base+"Img1b.tiff");
      bvxm_util::write_slab_as_image(bproj_image_c_slab1,
                                     base+"Img1c.tiff");
    }
    if (edge_debug&&print_arrays) {
      std::cout << "back proj view 1\n";
      for (unsigned j = 0; j<ny; j++) {
        for (unsigned i = 0; i<nx; i++)
          std::cout << bproj_image_a_slab1(i,j) << ' ';
        std::cout << '\n';
      }
    }
    bsta_histogram<double> h(0.0, 2.0, 10);
    // jointly iterate through the slabs looking for intersecting rays
    unsigned n_init_trials = 0;
    unsigned n_init_hits = 0;
    for (unsigned r=0; r<ny; ++r)
      for (unsigned c=0; c<nx; ++c)
      {
        float img_a0 = bproj_image_a_slab0(c, r);
        float img_b0 = bproj_image_b_slab0(c, r);
        float img_a1 = bproj_image_a_slab1(c, r);
        float img_b1 = bproj_image_b_slab1(c, r);
        // valid line normal coefficients can't be smaller than -1
        if (img_a0!=-2.0f && img_b0!=-2.0f && img_a1!=-2.0f && img_b1!=-2.0f)
        {
          // found a voxel where rays from both tangent images intersect
          // compute 3-d sub-voxel position and direction of the corresponding
          // 3-d line
          float img_c0 = bproj_image_c_slab0(c, r);
          float img_c1 = bproj_image_c_slab1(c, r);
          vgl_infinite_line_3d<float> line_3d;
          if (!bvxm_von_mises_tangent_processor<float>::
              tangent_3d_from_2d(img_a0, img_b0, img_c0, *cam0,
                                 img_a1, img_b1, img_c1, *cam1,
                                 line_3d))
            continue;
          // check if intersecting line passes through the voxel
          // the voxel corners
          vgl_point_3d<float> local_xyz_00 =
            world_->voxel_index_to_xyz(c, r, z,scale);
          vgl_point_3d<float> local_xyz_11 =
            world_->voxel_index_to_xyz(c+1, r+1, z+1,scale);

          // The voxel diagonal radius
          math_t diag =
            static_cast<math_t>((local_xyz_11-local_xyz_00).length()/2.0);

          // define the 3-d box corresponding to the voxel
          vgl_box_3d<float> b;
          b.add(local_xyz_00); b.add(local_xyz_11);
          if (edge_debug)
            std::cout << "\n===>checking\n" << b << '\n';
          n_init_trials++;
          // check intersection
#if 0 // LJM mod
          vgl_point_3d<float> ip0, ip1;
          if (!vgl_intersection<float>(b, line_3d, ip0, ip1))
            continue;
#endif
          // get the closest point on the line as the display origin
          vgl_point_3d<math_t> cc = b.centroid();
          vgl_point_3d<math_t> pc = vgl_closest_point<math_t>(line_3d, cc);
          math_t dist = static_cast<math_t>((pc-cc).length());
          h.upcount(dist, 1.0);
          if (dist>diag)
            continue;
          n_init_hits++;
          // intersection line does pass through the voxel
          vgl_vector_2d<float> x0 = line_3d.x0();
          vgl_vector_3d<float> dir = line_3d.direction();
          (voxel_pos_slab(c, r))[0]=x0.x();
          (voxel_pos_slab(c, r))[1]=x0.y();
          (voxel_dir_slab(c, r))[0]=dir.x();
          (voxel_dir_slab(c, r))[1]=dir.y();
          (voxel_dir_slab(c, r))[2]=dir.z();
          if (edge_debug) {
            std::cout << "\n===>does intersect\n" << x0 << '\n'
                     << dir << '\n';
          }
          // a flag to indicate that the voxel contains update information
          voxel_flag_slab(c, r) = true;
        }
      }
    if (edge_debug) {
      std::cout << "Number of init trials: " << n_init_trials << '\n'
               << "Number of init intersections: " << n_init_hits << '\n'
               << "Distance histogram\n";
      h.print();
    }

    if (!tan_proc.update(*dir_dist_it, *pos_dist_it,
                        voxel_dir_slab, voxel_pos_slab, voxel_flag_slab))
      return false;
    if (edge_debug&&print_arrays) {
      std::cout << "grid contents after initialization\n";
      for (unsigned r=0; r<ny; ++r) {
        for (unsigned c=0; c<nx; ++c) {
          pos_dist_t pos_dist = (*pos_dist_it)(c,r);
          dir_dist_t dir_dist = (*dir_dist_it)(c,r);
          std::cout << '(' << pos_dist.num_observations << ' '
                   << dir_dist.num_observations << ") ";
        }
        std::cout << '\n';
      }
    }
  }
  world_->increment_observations<TANGENT_POS>(0,scale);
  world_->increment_observations<TANGENT_DIR>(0,scale);
  return true;
}

//: update a von_mises edge tangent world
bool bvxm_edge_ray_processor::
update_von_mises_edge_tangents(bvxm_image_metadata const& metadata,
                               double /*x0_interval*/,
                               double /*cone_angle*/,
                               unsigned scale)
{
  // extract the tangent-point images
  auto* tan_image =
    static_cast<vil_image_view<float>*>(metadata.img.ptr());

  // grid size
  bvxm_world_params_sptr params = world_->get_params();
  vgl_vector_3d<unsigned int> grid_size = params->num_voxels(scale);
#if 0 // unused variable
  // voxel diagonal radius
  double radius = 0.866*params->voxel_length()*x0_interval;
#endif // 0

  // ray processor for update and utilities
  bvxm_von_mises_tangent_processor<float> tan_proc;

  // compute homographies from voxel planes to image coordinates and vise-versa.
  std::vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  std::vector<vgl_h_matrix_2d<double> > H_img_to_plane;
  {
    vgl_h_matrix_2d<double> Hp2i, Hi2p;
    for (unsigned z=0; z < (unsigned)grid_size.z(); ++z)
    {
      world_->compute_plane_image_H(metadata.camera,z,Hp2i,Hi2p,scale);
      H_plane_to_img.push_back(Hp2i);
      H_img_to_plane.push_back(Hi2p);
    }
  }
  // get projective cameras (warning! needs to be upgraded for RPC -- later)
  auto* cam =
    static_cast<vpgl_proj_camera<double>*>(metadata.camera.ptr());

  // datatype for current disributions
  typedef bvxm_voxel_traits<TANGENT_POS>::voxel_datatype pos_dist_t;
  typedef bvxm_voxel_traits<TANGENT_POS>::obs_type pos_t;
  typedef bvxm_voxel_traits<TANGENT_DIR>::voxel_datatype dir_dist_t;
  typedef bvxm_voxel_traits<TANGENT_DIR>::obs_type dir_t;
  typedef bvxm_voxel_traits<TANGENT_DIR>::math_type math_t;

  // extract tangent line slabs from tangent-point image
  unsigned ni = metadata.img->ni(), nj = metadata.img->nj();
  bvxm_voxel_slab<math_t> image_a_slab(ni, nj , 1);
  bvxm_voxel_slab<math_t> image_b_slab(ni, nj , 1);
  bvxm_voxel_slab<math_t> image_c_slab(ni, nj , 1);
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i) {
      image_a_slab(i,j)=(*tan_image)(i,j,0);
      image_b_slab(i,j)=(*tan_image)(i,j,1);
      image_c_slab(i,j)=(*tan_image)(i,j,2);
    }

  auto nx = static_cast<unsigned>(grid_size.x()),
           ny = static_cast<unsigned>(grid_size.y());

  // slabs for later backprojection
  // tangent slabs
  bvxm_voxel_slab<math_t> bproj_image_a_slab(nx, ny,1);
  bvxm_voxel_slab<math_t> bproj_image_b_slab(nx, ny,1);
  bvxm_voxel_slab<math_t> bproj_image_c_slab(nx, ny,1);
  // von mises distribution slabs
  bvxm_voxel_slab<pos_t> voxel_pos_slab(nx, ny,1);
  bvxm_voxel_slab<dir_t> voxel_dir_slab(nx, ny,1);
  // active voxel flag
  bvxm_voxel_slab<bool> voxel_flag_slab(nx, ny,1);

  // 3-d tangent position grid distributions
  bvxm_voxel_grid_base_sptr tangent_pos_base = world_->get_grid<TANGENT_POS>(0, scale);
  auto *pos_dist_grid  =
    static_cast<bvxm_voxel_grid<pos_dist_t>*>(tangent_pos_base.ptr());
  // 3-d tangent direction grid distributions
  bvxm_voxel_grid_base_sptr tangent_dir_base = world_->get_grid<TANGENT_DIR>(0, scale);
  auto *dir_dist_grid  =
    static_cast<bvxm_voxel_grid<dir_dist_t>*>(tangent_dir_base.ptr());

  if (edge_debug)
    std::cout << "Updating the tangent world:" << std::endl;
  bvxm_voxel_grid<pos_dist_t>::iterator pos_dist_it = pos_dist_grid->begin();
  bvxm_voxel_grid<dir_dist_t>::iterator dir_dist_it = dir_dist_grid->begin();
  for (unsigned z=0; z<(unsigned)grid_size.z();++z,++pos_dist_it,++dir_dist_it)
  {
    if (edge_debug)
      std::cout << "processing slab " << z << '\n';
    if ( (pos_dist_it == pos_dist_grid->end())||
         (dir_dist_it == dir_dist_grid->end()) ) {
      std::cerr << "error: reached end of tangent slabs at z = " << z << ".  nz = " << grid_size.z() << '\n';
      return false;
    }
    voxel_pos_slab.fill(pos_t(0.0f));
    voxel_dir_slab.fill(dir_t(0.0f));
    voxel_flag_slab.fill(false);
    // backproject image line coefficients
    bvxm_util::warp_slab_nearest_neighbor(image_a_slab, H_plane_to_img[z],
                                          bproj_image_a_slab);
    bvxm_util::warp_slab_nearest_neighbor(image_b_slab, H_plane_to_img[z],
                                          bproj_image_b_slab);
    bvxm_util::warp_slab_nearest_neighbor(image_c_slab, H_plane_to_img[z],
                                          bproj_image_c_slab);
    if (edge_debug&&print_arrays) {
      std::cout.precision(2);
      std::cout << "back proj view\n";
      for (unsigned j = 0; j<ny; j++) {
        for (unsigned i = 0; i<nx; i++)
          std::cout << bproj_image_a_slab(i,j) << ' ';
        std::cout << '\n';
      }
    }
    bvxm_voxel_slab<pos_dist_t>& pos_dist_slab = (*pos_dist_it);
    bvxm_voxel_slab<dir_dist_t>& dir_dist_slab = (*dir_dist_it);

    unsigned n_update_trials = 0;
    unsigned n_update_hits = 0;
    for (unsigned r=0; r<ny; ++r)
      for (unsigned c=0; c<nx; ++c)
      {
        math_t img_a = bproj_image_a_slab(c, r);
        math_t img_b = bproj_image_b_slab(c, r);
        pos_dist_t pos_dist = pos_dist_slab(c,r);
        dir_dist_t dir_dist = dir_dist_slab(c,r);
        // valid line normal coefficients can't be smaller than -1
        if (img_a!=-2.0f && img_b!=-2.0f &&
            pos_dist.num_observations > 0 && dir_dist.num_observations > 0)
        {
          // found a voxel which intersects the 3-d tangent plane
          // backprojected from the image tangent
          // compute the 3-d sub-voxel tangent line position and direction
          math_t img_c = bproj_image_c_slab(c, r);
          vgl_infinite_line_3d<math_t> line_3d;
          if (!bvxm_von_mises_tangent_processor<math_t>::
              pos_dir_from_image_tangent(img_a, img_b, img_c, *cam,
                                         pos_dist, dir_dist,
                                         line_3d))
            continue;
          // JLM =============Debug===================
          if (r == 44 && c == 24) {
#if 0
            pos_t pm = pos_dist.mean();
            math_t psd = std::sqrt(pos_dist.var());
            dir_t dm = dir_dist.mean();
            math_t ka = dir_dist.kappa();
            dos << pm[0] << ' ' << pm[1] << ' ' << psd << ' '
                << dm[0] << ' ' << dm[1] << ' ' << dm[2] << ' '
                << ka << '\n' << std::flush;
#endif
            vgl_homg_line_2d<double> img_l(img_a, img_b, img_c);
            vgl_plane_3d<double> pl = cam->backproject(img_l);
            double pa = pl.a(), pb = pl.b(), pc = pl.c();
#if 0
            double pd = pl.d();
            double nm = std::sqrt(pa*pa + pb*pb + pc*pc);
            pa/=nm;
            pb/=nm;
            pc/=nm;
            pd/=nm;
            dos << pa << ' ' << pb << ' ' << pc << ' ' << pd
                << '\n' << std::flush;
#endif
          }
          //========================================
          // check if intersecting line passes through the voxel
          // the voxel corners
          vgl_point_3d<math_t> local_xyz_00 =
            world_->voxel_index_to_xyz(c, r, z,scale);
          vgl_point_3d<math_t> local_xyz_11 =
            world_->voxel_index_to_xyz(c+1, r+1, z+1,scale);
          // The voxel diagonal radius
          math_t diag =
            static_cast<math_t>((local_xyz_11-local_xyz_00).length()/2.0);
          // define the 3-d box corresponding to the voxel
          vgl_box_3d<math_t> b;
          b.add(local_xyz_00); b.add(local_xyz_11);
          if (edge_debug)
            std::cout << "\n===>checking\n" << b << '\n';
          n_update_trials++;
          vgl_point_3d<math_t> cc = b.centroid();
          vgl_point_3d<math_t> pc = vgl_closest_point<math_t>(line_3d, cc);
          math_t dist = static_cast<math_t>((pc-cc).length());
          if (dist>diag) continue;
#if 0 // jlm debug
          // check intersection
          vgl_point_3d<math_t> ip0, ip1;
          if (!vgl_intersection<math_t>(b, line_3d, ip0, ip1))
            continue;
#endif
          n_update_hits++;
          if (edge_debug) {
            vgl_vector_2d<math_t> x0 = line_3d.x0();
            vgl_vector_3d<math_t> dir = line_3d.direction();
            std::cout << "\n===>does intersect\n" << x0 << '\n'
                     << dir << '\n';
          }
          // intersection line does pass through the voxel
          // update the distribution position mean and sample line means
          // to match
          vgl_vector_2d<math_t> x0 = line_3d.x0();
          vgl_vector_3d<math_t> dir = line_3d.direction();
          (voxel_pos_slab(c, r))[0]=x0.x();
          (voxel_pos_slab(c, r))[1]=x0.y();
          (voxel_dir_slab(c, r))[0]=dir.x();
          (voxel_dir_slab(c, r))[1]=dir.y();
          (voxel_dir_slab(c, r))[2]=dir.z();
          // a flag to indicate that the voxel contains update information
          voxel_flag_slab(c, r) = true;
        }
      }
    if (edge_debug) {
      std::cout << "ntrials for update: " << n_update_trials << '\n'
               << "nhits for update: " << n_update_hits << '\n';
    }
    if (!tan_proc.update(*dir_dist_it, *pos_dist_it,
                         voxel_dir_slab, voxel_pos_slab, voxel_flag_slab))
      return false;
  }
  world_->increment_observations<TANGENT_POS>(0,scale);
  world_->increment_observations<TANGENT_DIR>(0,scale);
  return true;
}

void bvxm_edge_ray_processor::
display_edge_tangent_world_vrml(std::string const& vrml_path)
{
  // open the file
  std::ofstream os(vrml_path.c_str());
  if (!os.is_open())
  {
    std::cerr << "In bvxm_edge_ray_processor::display_edge_tangent_world_vrml - "
             << " invalid path " << vrml_path << '\n';
    return;
  }
  // write the vrml header
  os << "#VRML V2.0 utf8\nBackground {\n  skyColor [ 0 0 0 ]\n  groundColor [ 0 0 0 ]\n}\n";
  // typedefs for tangent disributions and data
  typedef bvxm_voxel_traits<TANGENT_POS>::voxel_datatype pos_dist_t;
  typedef bvxm_voxel_traits<TANGENT_POS>::obs_type pos_t;
  typedef bvxm_voxel_traits<TANGENT_POS>::math_type math_t;
  typedef bvxm_voxel_traits<TANGENT_DIR>::voxel_datatype dir_dist_t;
  typedef bvxm_voxel_traits<TANGENT_DIR>::obs_type dir_t;
  unsigned scale = 0;
  // 3-d tangent position distribution grid
  bvxm_voxel_grid_base_sptr tangent_pos_base = world_->get_grid<TANGENT_POS>(0, scale);
  auto *pos_dist_grid  =
    static_cast<bvxm_voxel_grid<pos_dist_t>*>(tangent_pos_base.ptr());
  // 3-d tangent direction distribution grid
  bvxm_voxel_grid_base_sptr tangent_dir_base = world_->get_grid<TANGENT_DIR>(0, scale);
  auto *dir_dist_grid  =
    static_cast<bvxm_voxel_grid<dir_dist_t>*>(tangent_dir_base.ptr());

  // extract grid dimensions
  bvxm_world_params_sptr params = world_->get_params();
  vgl_vector_3d<unsigned int> grid_size = params->num_voxels(scale);
  double radius = params->voxel_length()*0.1;
  auto len = static_cast<math_t>(params->voxel_length());
  auto nx = static_cast<unsigned>(grid_size.x()),
                                      ny = static_cast<unsigned>(grid_size.y()),
                                      nz = static_cast<unsigned>(grid_size.z());
  double ssd = 0.0, skap = 0;
  double min_sd = 1000.0, max_sd = 0.0;
  double min_kappa = 20000.0, max_kappa = 0.0;
  unsigned sd_count = 0;
  unsigned kappa_count = 0;
  double min_nobs = 10.0e100, max_nobs = 0.0;
  double nobs = 0.0;
  unsigned max_nobs_r = 0, max_nobs_c = 0;
  vgl_point_3d<math_t> max_pc;
  bsta_histogram<double> h(0.0, 50.0, 50);
  bvxm_voxel_grid<pos_dist_t>::iterator pos_dist_it = pos_dist_grid->begin();
  bvxm_voxel_grid<dir_dist_t>::iterator dir_dist_it = dir_dist_grid->begin();
  for (unsigned z=0; z<nz; ++z,++pos_dist_it,++dir_dist_it)
  {
    if ( (pos_dist_it == pos_dist_grid->end())||
         (dir_dist_it == dir_dist_grid->end()) ) {
      std::cerr << "In bvxm_edge_ray_processor::display_edge_tangent_world_vrml- reached end of tangent slabs at z = " << z << ".  nz = " << grid_size.z() << '\n';
      return;
    }
    bvxm_voxel_slab<pos_dist_t>& pos_dist_slab = (*pos_dist_it);
    bvxm_voxel_slab<dir_dist_t>& dir_dist_slab = (*dir_dist_it);
    for (unsigned r=0; r<ny; ++r)
      for (unsigned c=0; c<nx; ++c)
      {
        pos_dist_t pos_dist = pos_dist_slab(c,r);
        dir_dist_t dir_dist = dir_dist_slab(c,r);
        double pnobs = pos_dist.num_observations;
        double dnobs = dir_dist.num_observations;
        if (pnobs > 0 && dnobs > 0)
        {
          h.upcount(pnobs,1.0);
          nobs += pnobs;
          if (pnobs<min_nobs)
            min_nobs = pnobs;
          // get the infinite 3-d line corresponding to the tangent dist
          pos_t pos_mean = pos_dist.mean();
          dir_t dir_mean = dir_dist.mean();
          double sd = std::sqrt(pos_dist.var());
          ssd += sd;
          if (sd<min_sd)
            min_sd = sd;
          if (sd>max_sd)
            max_sd = sd;
          double kap = dir_dist.kappa();
          if (kap<50000.0) {
            if (kap<min_kappa)
              min_kappa = kap;
            if (kap>max_kappa)
              max_kappa = kap;
            skap += kap;
            kappa_count++;
          }
          sd_count ++;
#if 0
          double red = 0.0418941/sd;
          double green = kap/49687.8;
          double blue = pnobs/5;
#endif
          double red = 1, green = 1, blue = 0;
          vgl_vector_2d<math_t> posv(pos_mean[0], pos_mean[1]);
          vgl_vector_3d<math_t> dirv(dir_mean[0],dir_mean[1], dir_mean[2]);
          vgl_infinite_line_3d<math_t> line_3d(posv, dirv);
          // get the current voxel corners
          vgl_point_3d<math_t> local_xyz_00 =
            world_->voxel_index_to_xyz(c, r, z,scale);
          vgl_point_3d<math_t> local_xyz_11 =
            world_->voxel_index_to_xyz(c+1, r+1, z+1,scale);
          // define the 3-d box corresponding to the voxel
          vgl_box_3d<math_t> b;
          b.add(local_xyz_00); b.add(local_xyz_11);
          // get the closest point on the line as the display origin
          vgl_point_3d<math_t> cc = b.centroid();
          vgl_point_3d<math_t> pc = vgl_closest_point<math_t>(line_3d, cc);
          // the tangent line cylinder rotation
          if (pnobs>max_nobs) {
            max_nobs = pnobs;
            max_nobs_r = r;
            max_nobs_c = c;
            max_pc = pc;
          }
          vnl_double_3 yaxis(0.0, 1.0, 0.0),
            pvec(dirv.x(), dirv.y(), dirv.z());
          vgl_rotation_3d<double> rot(yaxis, pvec);
          vnl_quaternion<double> q = rot.as_quaternion();
          vnl_double_3 axis = q.axis();
          double ang = q.angle();
          os <<  "Transform {\n"
             << " translation " << pc.x() << ' ' << pc.y() << ' ' << pc.z() << '\n'
             << " rotation " << axis[0] << ' ' << axis[1] << ' ' << axis[2] << ' ' <<  ang << '\n'
             << "children [\n Shape {\n appearance Appearance{\n"
             << "  material Material\n   {\n"
             << "     diffuseColor " << red << ' ' << green << ' ' << blue <<'\n'
             << "     transparency 0\n    }\n  }\n"
             << " geometry Cylinder\n{\n radius " << radius << '\n'
             << " height " << len << "\n }\n }\n]\n}\n";
        }
      }
  }

  os << "Transform {\n"
     << "translation " << max_pc.x()<< ' ' << max_pc.y()<< ' ' << max_pc.z() <<'\n'
     << "children [\nShape {\nappearance Appearance{\n"
     << "  material Material\n   {\n     diffuseColor 0 1 0\n"
     << "      transparency 0\n   }\n }\n"
     << "geometry Sphere\n{\n  radius .05\n   }\n  }\n ]\n}\n";
  os.close();
  if (edge_debug) {
    std::cout << "min sd " << min_sd << "mean sd " << ssd/sd_count
             << " max sd " << max_sd << '\n'
             << "min kappa " << min_kappa << "mean kappa " << skap/kappa_count
             << " max kappa " << max_kappa << '\n'
             << "min nobs " << min_nobs << "mean nobs " << nobs/sd_count << " max nobs " << max_nobs << '\n';
    h.print();
  }

  std::cout << "Max nobs, " << max_nobs << " at (" << max_nobs_c << ' '
           << max_nobs_r << ") 3d pos: " << max_pc << '\n';
}

void bvxm_edge_ray_processor::
display_ground_truth(std::string const& gnd_truth_path,
                     std::string const& vrml_path)
{
  // open the files
  std::ifstream is(gnd_truth_path.c_str());
  if (!is.is_open())
  {
    std::cerr << "In bvxm_edge_ray_processor::display_ground_truth - "
             << " invalid input path " << vrml_path << '\n';
    return;
  }
  std::ofstream os(vrml_path.c_str());
  if (!os.is_open())
  {
    std::cerr << "In bvxm_edge_ray_processor::display_ground_truth - "
             << " invalid output path " << vrml_path << '\n';
    return;
  }
  // write the vrml header
  os << "#VRML V2.0 utf8\n"
     << "Background {\n"
     << "  skyColor [ 0 0 0 ]\n"
     << "  groundColor [ 0 0 0 ]\n"
     << "}\n";
  // read the file
  std::string temp;
  is >> temp;
  if (temp!="nlines:")
  {
    std::cerr << "In bvxm_edge_ray_processor::display_ground_truth - "
             << " parse of input failed\n";
    return;
  }
  unsigned nlines = 0;
  is >> nlines;
  for (unsigned i = 0; i<nlines; ++i) {
    vnl_double_3 v0, v1, dir, pc;
    is >> v0; is >> v1;
    pc = (v0+v1)/2.0;
    dir = v1-v0;
    double len = dir.magnitude();
    dir /= len;
    double red = 0, green = 1, blue = 0;
    double radius = 0.025;
    // the tangent line cylinder rotation
    vnl_double_3 yaxis(0.0, 1.0, 0.0);
    vgl_rotation_3d<double> rot(yaxis, dir);
    vnl_quaternion<double> q = rot.as_quaternion();
    vnl_double_3 axis = q.axis();
    double ang = q.angle();
    os <<  "Transform {\n"
       << " translation " << pc[0] << ' ' << pc[1] << ' ' << pc[2] << '\n'
       << " rotation " << axis[0] << ' ' << axis[1] << ' ' << axis[2] << ' ' <<  ang << '\n'
       << "children [\n Shape {\n appearance Appearance{\n"
       << "  material Material\n   {\n"
       << "     diffuseColor " << red << ' ' << green << ' ' << blue <<'\n'
       << "     transparency 0\n    }\n  }\n"
       << " geometry Cylinder\n{\n radius " << radius << '\n'
       << " height " << len << "\n }\n }\n]\n}\n";
  }
  os.close();
}
