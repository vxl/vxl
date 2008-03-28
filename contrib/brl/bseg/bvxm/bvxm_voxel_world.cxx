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

#include "bvxm_voxel_grid.h"
#include "bvxm_voxel_traits.h"
#include "bvxm_world_params.h"
#include "bvxm_voxel_slab.h"
#include "bvxm_image_metadata.h"
#include "bvxm_util.h"


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
      bvxm_util::compute_plane_image_H(metadata.camera,params_,z,Hp2i,Hi2p);
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
    bvxm_util::compute_plane_image_H(camera.camera,params_,z,Hp2i,Hi2p);
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
