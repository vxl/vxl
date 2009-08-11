#include "bvxm_edge_ray_processor.h"
//:
// \file
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_voxel_traits.h>
#include <bvxm/bvxm_world_params.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_voxel_slab.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_util.h>
#include <bvxm/bvxm_edge_util.h>

// initialize the voxel grid for edges
bool bvxm_edge_ray_processor::init_edges(unsigned scale)
{
  if (world_->num_observations<EDGES>(0,scale)!=0){
    return false;
  }

  // datatype for current appearance model
  typedef bvxm_voxel_traits<EDGES>::voxel_datatype edges_datatype;

  bvxm_world_params_sptr params = world_->get_params();
  vgl_vector_3d<unsigned int> grid_size = params->num_voxels(scale);

  // get edge probability grid
  bvxm_voxel_grid_base_sptr edges_voxel_base = world_->get_grid<EDGES>(0, scale);
  bvxm_voxel_grid<edges_datatype> *edges_voxel  = static_cast<bvxm_voxel_grid<edges_datatype>*>(edges_voxel_base.ptr());

  // Pass 1: to build the marginal
  vcl_cout << "Initializing the voxel world:" << vcl_endl;
  bvxm_voxel_grid<edges_datatype>::iterator edges_voxel_it = edges_voxel->begin();
  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++edges_voxel_it)
  {
    vcl_cout << '.';
    if ( (edges_voxel_it == edges_voxel->end()) ) {
      vcl_cerr << "error: reached end of grid slabs at z = " << z << ".  nz = " << grid_size.z() << '\n';
      return false;
    }

    bvxm_voxel_slab<edges_datatype>::iterator edges_voxel_it_it = (*edges_voxel_it).begin();

    for (; edges_voxel_it_it != (*edges_voxel_it).end(); ++edges_voxel_it_it) {
      (*edges_voxel_it_it) = 0.0f;
    }
  }
  vcl_cout << "\nDone\n";

  return true;
}
// update voxel grid for edges with data from image/camera pair and return the edge probability density of pixel values
bool bvxm_edge_ray_processor::update_edges(bvxm_image_metadata const& metadata, unsigned scale)
{
  if (world_->num_observations<EDGES>(0,scale)==0){
    this->init_edges(scale);
  }

  // datatype for current appearance model
  typedef bvxm_voxel_traits<EDGES>::voxel_datatype edges_datatype;
  bvxm_world_params_sptr params = world_->get_params();
  vgl_vector_3d<unsigned int> grid_size = params->num_voxels(scale);

  // compute homographies from voxel planes to image coordinates and vise-versa.
  vcl_vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  vcl_vector<vgl_h_matrix_2d<double> > H_img_to_plane;
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
    vcl_cerr << "error converting image to voxel slab of observation type for bvxm_voxel_type " << (int)EDGES << '\n';
    return false;
  }

  // get edge probability grid
  bvxm_voxel_grid_base_sptr edges_voxel_base = world_->get_grid<EDGES>(0, scale);
  bvxm_voxel_grid<edges_datatype> *edges_voxel  = static_cast<bvxm_voxel_grid<edges_datatype>*>(edges_voxel_base.ptr());

  // Pass 1: to build the marginal
  vcl_cout << "Pass 1 of 1:" << vcl_endl;
  bvxm_voxel_grid<edges_datatype>::iterator edges_voxel_it = edges_voxel->begin();
  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++edges_voxel_it)
  {
    vcl_cout << '.';
    if ( (edges_voxel_it == edges_voxel->end()) ) {
      vcl_cerr << "error: reached end of grid slabs at z = " << z << ".  nz = " << grid_size.z() << '\n';
      return false;
    }

    bvxm_util::warp_slab_bilinear(image_image,H_plane_to_img[z],image_voxel);

    bvxm_voxel_slab<edges_datatype>::iterator image_voxel_it = image_voxel.begin();
    bvxm_voxel_slab<edges_datatype>::iterator edges_voxel_it_it = (*edges_voxel_it).begin();

    for (; image_voxel_it != image_voxel.end(); ++image_voxel_it, ++edges_voxel_it_it) {
      (*edges_voxel_it_it) = (*edges_voxel_it_it) + (*image_voxel_it);
    }
  }
  vcl_cout << vcl_endl;

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
  vcl_vector<vgl_h_matrix_2d<double> > H_plane_to_img;
  vcl_vector<vgl_h_matrix_2d<double> > H_img_to_plane;
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
  bvxm_voxel_grid<edges_datatype> *edges_voxel  = static_cast<bvxm_voxel_grid<edges_datatype>*>(edges_voxel_base.ptr());

  bvxm_voxel_grid<edges_datatype>::iterator edges_voxel_it(edges_voxel->begin());

  vcl_cout << "Generating Expected Edge Image:" << vcl_endl;
  for (unsigned z=0; z<(unsigned)grid_size.z(); ++z, ++edges_voxel_it) {
    vcl_cout << '.';

    bvxm_util::warp_slab_bilinear((*edges_voxel_it), H_img_to_plane[z], edges_image);

    bvxm_voxel_slab<edges_datatype>::iterator edges_image_it = edges_image.begin();
    bvxm_voxel_slab<edges_datatype>::iterator expected_edge_image_it = expected_edge_image.begin();

    for (; expected_edge_image_it != expected_edge_image.end(); ++expected_edge_image_it, ++edges_image_it) {
      (*expected_edge_image_it) = vnl_math_max((*expected_edge_image_it),(*edges_image_it));
    }
  }
  vcl_cout << vcl_endl;

  int dof = (int)world_->num_observations<EDGES>(0,scale)-1;
  bvxm_voxel_slab<edges_datatype>::iterator expected_edge_image_it = expected_edge_image.begin();
  for (; expected_edge_image_it != expected_edge_image.end(); ++expected_edge_image_it) {
    (*expected_edge_image_it) = bvxm_edge_util::convert_edge_statistics_to_probability((*expected_edge_image_it),n_normal,dof);
  }

  // convert back to vil_image_view
  bvxm_util::slab_to_img(expected_edge_image, expected);

  return true;
}

//: save the edge probability grid as an 8-bit 3-d vff image
bool bvxm_edge_ray_processor::save_edges_vff(vcl_string filename,unsigned scale)
{
  // open file for binary writing
  vcl_fstream ofs(filename.c_str(),vcl_ios::binary | vcl_ios::out);
  if (!ofs.is_open()) {
    vcl_cerr << "error opening file " << filename << " for write!\n";
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
  vcl_stringstream header;
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

  vcl_string header_string = header.str();
  unsigned header_len = header_string.size();

  ofs.write(header_string.c_str(),header_len);

  // write data
  // iterate through slabs and fill in memory array
  char *edges_array = new char[nx*ny*nz];

  bvxm_voxel_grid<edges_datatype>::iterator edges_it = edges_grid->begin();
  for (unsigned k=nz-1; edges_it != edges_grid->end(); ++edges_it, --k) {
    vcl_cout << '.';
    for (unsigned i=0; i<(*edges_it).nx(); ++i) {
      for (unsigned j=0; j < (*edges_it).ny(); ++j) {
        edges_array[k*nx*ny + j*nx + i] = (unsigned char)((*edges_it)(i,j) * 255.0);;
      }
    }
  }
  vcl_cout << vcl_endl;
  ofs.write(reinterpret_cast<char*>(edges_array),sizeof(unsigned char)*nx*ny*nz);

  ofs.close();

  delete[] edges_array;

  return true;
}

//: save the edge probability grid in a ".raw" format readable by Drishti volume rendering software
bool bvxm_edge_ray_processor::save_edges_raw(vcl_string filename, float n_normal, unsigned scale)
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

  vcl_cout << "Saving edges to RAW file:" << vcl_endl;
  bvxm_voxel_grid<edges_datatype>::iterator edges_it = edges_grid->begin();
  for (unsigned k=0; edges_it != edges_grid->end(); ++edges_it, ++k) {
    vcl_cout << '.';
    for (unsigned i=0; i<(*edges_it).nx(); ++i) {
      for (unsigned j=0; j < (*edges_it).ny(); ++j) {
        edges_array[i*ny*nz + j*nz + k] = (unsigned char)(255.0*bvxm_edge_util::convert_edge_statistics_to_probability((*edges_it)(i,j),n_normal,dof));
      }
    }
  }
  vcl_cout << vcl_endl;
  ofs.write(reinterpret_cast<char*>(edges_array),sizeof(unsigned char)*nx*ny*nz);

  ofs.close();

  delete[] edges_array;

  return true;
}
