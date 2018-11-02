// This is brl/bseg/boxm/algo/boxm_save_scene_raw_general.h
#ifndef boxm_save_scene_raw_general_h
#define boxm_save_scene_raw_general_h

//:
// \file
// \brief  Writes the 3D scene into a raw file by combining all the spatial data in a set of octrees.
//         The octree data mapped to a 3D regular grid and kept in binary files temporarily.
//         And then these sets of data combined and saved in a raw file.
//
// \author Gamze Tunali
// \date   September 9, 2009
// \verbatim
//  Modifications
//   February 17, 2011 Isabel Restrepo: Modified functions to use with more general scenes.
//                                      Double, float values are not converted to byte, nor assumptions are made. Values are saves as they are
// \endverbatim

#include <vector>
#include <new>
#include <iostream>
#include <sstream>
#include <boct/boct_tree.h>
#include <boxm/boxm_scene.h>
#include <boxm/sample/boxm_sample.h>
#include <boxm/util/boxm_cell_data_traits.h>
#include <boxm/basic/boxm_block_vis_graph_iterator.h>
#include <boxm/basic/boxm_cell_vis_graph_iterator.h>
#include <boxm/sample/algo/boxm_mog_grey_processor.h>
#include <boxm/algo/boxm_compute_scene_statistics.h>
#include <bsta/bsta_histogram.h>
#include <vbl/vbl_array_3d.hxx>
#include <vsl/vsl_binary_io.h>
#include <vpl/vpl.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <vul/vul_file.h>


//: Traits for saving grids of different datatypes to Drishti .raw file
template <class T>
class boxm_dristhi_traits;

//: Drishti datatypes for header:
// * 0 : unsigned byte - 1 byte per voxel
// * 1 : signed byte - 1 byte per voxel
// * 2 : unsigned short - 2 bytes per voxel
// * 3 : signed short - 2 bytes per voxel
// * 4 : integer - 4 bytes per voxel
// * 8 : float - 4 bytes per voxel

//: Drishti traits specializations. Please extent as needed

template <>
class boxm_dristhi_traits<float>
{
 public:
  static unsigned char dristhi_header() {return 8;}
};


template <>
class boxm_dristhi_traits<unsigned char>
{
 public:
  static unsigned char dristhi_header() {return 0;}
};

template <>
class boxm_dristhi_traits<char>
{
 public:
  static unsigned char dristhi_header() {return 1;}
};

template <>
class boxm_dristhi_traits<short>
{
 public:
  static unsigned char dristhi_header() {return 3;}
};

template <>
class boxm_dristhi_traits<unsigned int>
{
 public:
  static unsigned char dristhi_header() {return 4;}
};

template <>
class boxm_dristhi_traits<bsta_num_obs<bsta_gauss_sf1> >
{
 public:
  static unsigned char dristhi_header() {return 8;}
};


template <class T_loc>
void boxm_save_scene_raw_general(boxm_scene<boct_tree<T_loc, bool > > &scene,
                                 std::string filename,
                                 unsigned int resolution_level)
{
  unsigned long n_zeros = 0;
  unsigned long n_ones = 0;
  unsigned long n_zeros_level0 = 0;
  unsigned long n_ones_level0 = 0;

  typedef boct_tree<T_loc, bool > tree_type;

  // create an array for each block, and save in a binary file

  // number of cells for each block should be the same, because they have the
  // same tree max_level definitions
  unsigned int ncells = 0;
  boxm_block_iterator<tree_type> iter(&scene);

  // query the finest level of the tree and do not make the resolution
  // smaller than that
  unsigned int finest_level=scene.finest_level();

  if (resolution_level < finest_level)
    resolution_level=finest_level;

  iter.begin();
  while (!iter.end())
  {
    vgl_point_3d<int> idx = iter.index();
    scene.load_block(idx);
    boxm_block<tree_type>* block = scene.get_block(idx);
    vgl_box_3d<double> block_bb = block->bounding_box();
    tree_type* tree = block->get_tree();

    // get origin of block
    vgl_point_3d<double> min = block_bb.min_point();

    ncells = 1 << (tree->root_level() - (int)resolution_level);

    // assume that bounding box is a cube
    const double step_len = ((block_bb.max_x() - block_bb.min_x())/double(ncells));

    // origin should specify center of first cell
    vgl_point_3d<double> data_og(min.x() + (step_len/2.0), min.y() + (step_len/2.0), min.z() + (step_len/2.0));

    int data_size = ncells*ncells*ncells;
    unsigned char *data = nullptr;
    std::cout << "Data Size=" << data_size << std::endl;
    data = new (std::nothrow)unsigned char[data_size];

    if (data == nullptr) {
      std::cout << "boxm_save_block_raw: Could not allocate data!" << std::endl;
      return;
    }

    // init to zero
    for (unsigned char* dp = data; dp < data + ncells*ncells*ncells; ++dp) {
      *dp = (unsigned char)0.0;
    }

    double out_cell_norm_volume = (tree->number_levels() - (int)resolution_level + 1);
    out_cell_norm_volume = out_cell_norm_volume*out_cell_norm_volume*out_cell_norm_volume;

    std::vector<boct_tree_cell<T_loc, bool > *> cells = tree->leaf_cells();
    for (unsigned i=0; i<cells.size(); i++)
    {
      vgl_point_3d<double> node = tree->cell_bounding_box_local(cells[i]).min_point();

      unsigned char cell_val = (unsigned char)cells[i]->data()*255;
      if (cells[i]->data()) {
        n_ones++;
        if (cells[i]->get_code().level ==0)
          n_ones_level0++;
      }
      else {
        n_zeros++;
        if (cells[i]->get_code().level ==0)
          n_zeros_level0++;
      }

      unsigned int level = cells[i]->get_code().level;
      if (level == resolution_level) {
        // just copy value to output array
        int out_index=static_cast<int>(ncells-1-(node.z()/step_len) + (node.y()/step_len)*ncells + (node.x()/step_len)*ncells*ncells);

        if (out_index >= data_size)
          std::cout << "boxm_save_block_raw, array out of index! " << out_index << " -- " << data_size << std::endl;
        else
          data[out_index] = cell_val;
      }
      else if (level > resolution_level) {
        // cell is bigger than output cells.  copy value to all contained output cells.
        const unsigned int us_factor = 1 << (level - resolution_level);
        const unsigned int node_x_start = static_cast<unsigned int>(node.x()/step_len);
        const unsigned int node_y_start = static_cast<unsigned int>(node.y()/step_len);
        const unsigned int node_z_start = static_cast<unsigned int>(node.z()/step_len);
        for (unsigned int z=node_z_start; z<node_z_start+us_factor; ++z) {
          for (unsigned int y=node_y_start; y<node_y_start+us_factor; ++y) {
            for (unsigned int x=node_x_start; x<node_x_start+us_factor; ++x) {
              int out_index=ncells-1-z + y*ncells + x*ncells*ncells;

              if (out_index >= data_size)
                std::cout << "boxm_save_block_raw, array out of index! " << out_index << " -- " << data_size << std::endl;
              else
                data[out_index] = cell_val;
            }
          }
        }
      }
      else {
        // cell is smaller than output cells. increment output cell value
        unsigned int ds_factor = 1 << (resolution_level - level);
        double update_weight = 1.0 / double(ds_factor*ds_factor*ds_factor);
        const unsigned int node_x = static_cast<unsigned int>(node.x()/step_len);
        const unsigned int node_y = static_cast<unsigned int>(node.y()/step_len);
        const unsigned int node_z = static_cast<unsigned int>(node.z()/step_len);
        int out_index=ncells-1-node_z + node_y*ncells + node_x*ncells*ncells;

        data[out_index] += (unsigned char)(cell_val*update_weight);
      }
    }

    // write the data into a bin file, generate a file name
    std::stringstream strm;
    strm << filename<< idx.x() << '_' << idx.y() << '_' << idx.z() << ".bin";
    std::string fn(strm.str());
    vsl_b_ofstream os(fn.c_str(),std::ios::binary);
    if (!os.os().good()) {
      std::cerr << "error opening " << filename << " for write!\n";
      return;
    }

    vsl_b_write(os, (unsigned int) ncells);  // x dimension
    vsl_b_write(os, (unsigned int) ncells);  // y dimension
    vsl_b_write(os, (unsigned int) ncells);  // z dimension

    unsigned char* dp = data;
    for (; dp < data + data_size; ++dp) {
      unsigned char v = *dp;
      vsl_b_write(os, (unsigned char)v);
    }
    delete[] data;
    os.close();
    iter++;
  }

  scene.unload_active_blocks();

  assert(ncells > 0);

  // combine the blocks
  vgl_vector_3d<unsigned> dim = scene.world_dim();
  unsigned dimx = dim.x()*ncells;
  unsigned dimy = dim.y()*ncells;
  unsigned dimz = dim.z()*ncells;
  // we will read the data a column at a time and this is enough
  int row_data_size = ncells;
  unsigned char* read_data = new (std::nothrow) unsigned char[row_data_size];
  if (read_data == nullptr) {
    std::cout << "boxm_save_block_raw: Could not allocate read data!" << std::endl;
    return;
  }

  // open the binary files streams, saved earlier
  unsigned int nx,ny,nz;
  vbl_array_3d<vsl_b_ifstream*> streams(dim.x(), dim.y(), dim.z());
  for (unsigned z=0; z<dim.z(); z++) {
    for (unsigned y=0; y<dim.y(); y++) {
      for (unsigned x=0; x<dim.x(); x++) {
        vsl_b_ifstream*& s = streams(x,y,z);
        std::stringstream strm;
        strm << filename << x << '_' << y << '_' << z << ".bin";
        std::string fn(strm.str());
        s = new vsl_b_ifstream(fn.c_str(),std::ios::binary);
        vsl_b_read(*s, nx);
        vsl_b_read(*s, ny);
        vsl_b_read(*s, nz);
      }
    }
  }

  // start writing the raw header
  unsigned char data_type= 0;
  vxl_uint_32 nx_uint = dimx;
  vxl_uint_32 ny_uint = dimy;
  vxl_uint_32 nz_uint = dimz;
  std::ofstream os(filename.c_str(),std::ios::binary);
  if (!os.good()) {
    std::cerr << "error opening " << filename << " for write!\n";
    return;
  }

  os.write(reinterpret_cast<char*>(&data_type),sizeof(data_type));
  os.write(reinterpret_cast<char*>(&nx_uint),sizeof(nx_uint));
  os.write(reinterpret_cast<char*>(&ny_uint),sizeof(ny_uint));
  os.write(reinterpret_cast<char*>(&nz_uint),sizeof(nz_uint));

  // combine the column from streams to generate one raw file
  for (unsigned x=0; x<dim.x(); x++) {
    unsigned k=0;
    while (k<dim.y()*dim.z()*ncells*ncells*ncells) {
      for (unsigned y=0; y<dim.y(); y++) {
        unsigned j=0;
        while (j < ncells*ncells*dim.z()) {
          for (unsigned z=dim.z();z>0; z--) {
            vsl_b_ifstream* s=streams(x,y,z-1);
            s->is().read(reinterpret_cast<char*>(read_data), ncells*sizeof(unsigned char));
            os.write(reinterpret_cast<char*>(read_data),ncells*sizeof(unsigned char));
            j+=ncells;
            k+=ncells;
          }
        }
      }
    }
  }

  os.close();
  delete[] read_data;

  // remove the intermediate bin files
  for (unsigned z=0; z<dim.z(); z++) {
    for (unsigned y=0; y<dim.y(); y++) {
      for (unsigned x=0; x<dim.x(); x++) {
        vsl_b_ifstream*& s = streams(x,y,z);
        s->close();
        std::stringstream strm;
        strm << filename << x << '_' << y << '_' << z << ".bin";
        std::string fn(strm.str());
        std::cout << "Deleting " << fn << std::endl;
        vpl_unlink(fn.data());
        delete s;
      }
    }
  }

  std::cout << "Number of true cells : " << n_ones << "\n Number of false cells: " << n_zeros << std::endl;
  std::cout << "Number of true cells at level 0 : " << n_ones_level0 << "\n Number of false cells at level 0: " << n_zeros_level0 << std::endl;

  return;
}


template <class T_loc, class T_data>
void boxm_save_scene_raw_general(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                                 std::string filename,
                                 unsigned int resolution_level)
{
  std::cout << " Using boxm_save_scene_raw_general\n";
  bsta_histogram<float> hist;
  boxm_compute_scene_statistics(&scene, hist);
  hist.pretty_print();

  std::stringstream ss;
  ss << vul_file::strip_extension(filename) << "_hist_plot.txt" ;
  std::ofstream hist_arrays_ofs((ss.str()).c_str());
  hist.print_to_arrays(hist_arrays_ofs);
  hist_arrays_ofs.close();

  typedef boct_tree<T_loc, T_data > tree_type;

  // create an array for each block, and save in a binary file

  // number of cells for each block should be the same, because they have the
  // same tree max_level definitions
  unsigned int ncells = 0;
  boxm_block_iterator<tree_type> iter(&scene);

  // query the finest level of the tree and do not make the resolution
  // smaller than that
  unsigned int finest_level=scene.finest_level();

  if (resolution_level < finest_level)
    resolution_level=finest_level;

  iter.begin();
  while (!iter.end())
  {
    vgl_point_3d<int> idx = iter.index();
    scene.load_block(idx);
    boxm_block<tree_type>* block = scene.get_block(idx);
    vgl_box_3d<double> block_bb = block->bounding_box();
    tree_type* tree = block->get_tree();

    // get origin of block
    vgl_point_3d<double> min = block_bb.min_point();

    ncells = 1 << (tree->root_level() - (int)resolution_level);

    // assume that bounding box is a cube
    const double step_len = ((block_bb.max_x() - block_bb.min_x())/double(ncells));

    // origin should specify center of first cell
    vgl_point_3d<double> data_og(min.x() + (step_len/2.0), min.y() + (step_len/2.0), min.z() + (step_len/2.0));

    int data_size = ncells*ncells*ncells;
    T_data *data = nullptr;
    std::cout << "Data Size=" << data_size << std::endl;
    data = new (std::nothrow)T_data[data_size];

    if (data == nullptr) {
      std::cout << "boxm_save_block_raw: Could not allocate data!" << std::endl;
      return;
    }

    // init to zero
    for (T_data* dp = data; dp < data + ncells*ncells*ncells; ++dp) {
      *dp = (T_data)0.0;
    }

    double out_cell_norm_volume = (tree->number_levels() - (int)resolution_level + 1);
    out_cell_norm_volume = out_cell_norm_volume*out_cell_norm_volume*out_cell_norm_volume;

    std::vector<boct_tree_cell<T_loc, T_data > *> cells = tree->leaf_cells();
    for (unsigned i=0; i<cells.size(); i++)
    {
      vgl_point_3d<double> node = tree->cell_bounding_box_local(cells[i]).min_point();

      T_data cell_val = cells[i]->data();

      unsigned int level = cells[i]->get_code().level;
      if (level == resolution_level) {
        // just copy value to output array
        int out_index=static_cast<int>(ncells-1-(node.z()/step_len) + (node.y()/step_len)*ncells + (node.x()/step_len)*ncells*ncells);

        if (out_index >= data_size)
          std::cout << "boxm_save_block_raw, array out of index! " << out_index << " -- " << data_size << std::endl;
        else
          data[out_index] = cell_val;
      }
      else if (level > resolution_level) {
        // cell is bigger than output cells.  copy value to all contained output cells.
        const unsigned int us_factor = 1 << (level - resolution_level);
        const unsigned int node_x_start = static_cast<unsigned int>(node.x()/step_len);
        const unsigned int node_y_start = static_cast<unsigned int>(node.y()/step_len);
        const unsigned int node_z_start = static_cast<unsigned int>(node.z()/step_len);
        for (unsigned int z=node_z_start; z<node_z_start+us_factor; ++z) {
          for (unsigned int y=node_y_start; y<node_y_start+us_factor; ++y) {
            for (unsigned int x=node_x_start; x<node_x_start+us_factor; ++x) {
              int out_index=ncells-1-z + y*ncells + x*ncells*ncells;

              if (out_index >= data_size)
                std::cout << "boxm_save_block_raw, array out of index! " << out_index << " -- " << data_size << std::endl;
              else
                data[out_index] = cell_val;
            }
          }
        }
      }
      else {
        // cell is smaller than output cells. increment output cell value
        unsigned int ds_factor = 1 << (resolution_level - level);
        double update_weight = 1.0 / double(ds_factor*ds_factor*ds_factor);
        const unsigned int node_x = static_cast<unsigned int>(node.x()/step_len);
        const unsigned int node_y = static_cast<unsigned int>(node.y()/step_len);
        const unsigned int node_z = static_cast<unsigned int>(node.z()/step_len);
        int out_index=ncells-1-node_z + node_y*ncells + node_x*ncells*ncells;

        data[out_index] += (T_data)(cell_val*update_weight);
      }
    }

    // write the data into a bin file, generate a file name
    std::stringstream strm;
    strm << filename<< idx.x() << '_' << idx.y() << '_' << idx.z() << ".bin";
    std::string fn(strm.str());
    vsl_b_ofstream os(fn.c_str(),std::ios::binary);
    if (!os.os().good()) {
      std::cerr << "error opening " << filename << " for write!\n";
      return;
    }

    vsl_b_write(os, (unsigned int) ncells);  // x dimension
    vsl_b_write(os, (unsigned int) ncells);  // y dimension
    vsl_b_write(os, (unsigned int) ncells);  // z dimension

    T_data* dp = data;
    for (; dp < data + data_size; ++dp) {
      T_data v = *dp;
      if (v > 19 || v < -1)
        std::cerr << "Error, v: " << v << '\n';
      vsl_b_write(os, (T_data)v);
    }
    std::cerr << " Data Correct ? " << '\n';

    delete[] data;
    os.close();
    iter++;
  }

  scene.unload_active_blocks();

  assert(ncells > 0);

  // combine the blocks
  vgl_vector_3d<unsigned> dim = scene.world_dim();
  unsigned dimx = dim.x()*ncells;
  unsigned dimy = dim.y()*ncells;
  unsigned dimz = dim.z()*ncells;
  // we will read the data a column at a time and this is enough
  int row_data_size = ncells;
  T_data* read_data = new (std::nothrow) T_data[row_data_size];
  if (read_data == nullptr) {
    std::cout << "boxm_save_block_raw: Could not allocate read data!" << std::endl;
    return;
  }

  // open the binary files streams, saved earlier
  unsigned int nx,ny,nz;
  vbl_array_3d<vsl_b_ifstream*> streams(dim.x(), dim.y(), dim.z());
  for (unsigned z=0; z<dim.z(); z++) {
    for (unsigned y=0; y<dim.y(); y++) {
      for (unsigned x=0; x<dim.x(); x++) {
        vsl_b_ifstream*& s = streams(x,y,z);
        std::stringstream strm;
        strm << filename << x << '_' << y << '_' << z << ".bin";
        std::string fn(strm.str());
        s = new vsl_b_ifstream(fn.c_str(),std::ios::binary);
        vsl_b_read(*s, nx);
        vsl_b_read(*s, ny);
        vsl_b_read(*s, nz);
      }
    }
  }

  // start writing the raw header
  unsigned char data_type = boxm_dristhi_traits<T_data>::dristhi_header();
  vxl_uint_32 nx_uint = dimx;
  vxl_uint_32 ny_uint = dimy;
  vxl_uint_32 nz_uint = dimz;
  std::ofstream os(filename.c_str(),std::ios::binary);
  if (!os.good()) {
    std::cerr << "error opening " << filename << " for write!\n";
    return;
  }

  os.write(reinterpret_cast<char*>(&data_type),sizeof(data_type));
  os.write(reinterpret_cast<char*>(&nx_uint),sizeof(nx_uint));
  os.write(reinterpret_cast<char*>(&ny_uint),sizeof(ny_uint));
  os.write(reinterpret_cast<char*>(&nz_uint),sizeof(nz_uint));

  // combine the column from streams to generate one raw file
  for (unsigned x=0; x<dim.x(); x++) {
    unsigned k=0;
    while (k<dim.y()*dim.z()*ncells*ncells*ncells) {
      for (unsigned y=0; y<dim.y(); y++) {
        unsigned j=0;
        while (j < ncells*ncells*dim.z()) {
          for (unsigned z=dim.z();z>0; z--) {
            vsl_b_ifstream* s=streams(x,y,z-1);
            s->is().read((char*)read_data, ncells*sizeof(T_data));
            os.write((char*)read_data,ncells*sizeof(T_data));
            j+=ncells;
            k+=ncells;
          }
        }
      }
    }
  }

  os.close();
  delete[] read_data;

  // remove the intermediate bin files
  for (unsigned z=0; z<dim.z(); z++) {
    for (unsigned y=0; y<dim.y(); y++) {
      for (unsigned x=0; x<dim.x(); x++) {
        vsl_b_ifstream*& s = streams(x,y,z);
        s->close();
        std::stringstream strm;
        strm << filename << x << '_' << y << '_' << z << ".bin";
        std::string fn(strm.str());
        std::cout << "Deleting " << fn << std::endl;
        vpl_unlink(fn.data());
        delete s;
      }
    }
  }
  return;
}


template <class T_loc, class T_data>
void boxm_save_scene_raw_general_to_byte(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                                         std::string filename,
                                         unsigned int resolution_level)
{
  typedef boct_tree<T_loc, T_data > tree_type;

  std::cout << " Using boxm_save_scene_raw_general_to_byte\n";

  bsta_histogram<float> hist;
  boxm_compute_scene_statistics(&scene, hist);

  // create an array for each block, and save in a binary file

  // number of cells for each block should be the same, because they have the
  // same tree max_level definitions
  unsigned int ncells = 0;
  boxm_block_iterator<tree_type> iter(&scene);

  unsigned int finest_level=scene.finest_level();

  // query the finest level of the tree and do not make the resolution
  // smaller than that
  if (resolution_level < finest_level)
    resolution_level=finest_level;

  iter.begin();
  while (!iter.end())
  {
    vgl_point_3d<int> idx = iter.index();
    scene.load_block(idx);
    boxm_block<tree_type>* block = scene.get_block(idx);
    vgl_box_3d<double> block_bb = block->bounding_box();
    tree_type* tree = block->get_tree();

    // get origin of block
    vgl_point_3d<double> min = block_bb.min_point();

    ncells = 1 << (tree->root_level() - (int)resolution_level);

    // assume that bounding box is a cube
    const double step_len = ((block_bb.max_x() - block_bb.min_x())/double(ncells));

    // origin should specify center of first cell
    vgl_point_3d<double> data_og(min.x() + (step_len/2.0), min.y() + (step_len/2.0), min.z() + (step_len/2.0));

    int data_size = ncells*ncells*ncells;
    float *data = nullptr;
    std::cout << "Data Size=" << data_size << std::endl;
    data = new (std::nothrow)float[data_size];

    if (data == nullptr) {
      std::cout << "boxm_save_block_raw: Could not allocate data!" << std::endl;
      return;
    }
    // init to zero
    for (float* dp = data; dp < data + ncells*ncells*ncells; ++dp) {
      *dp = 0.0f;
    }

    double out_cell_norm_volume = (tree->number_levels() - (int)resolution_level + 1);
    out_cell_norm_volume = out_cell_norm_volume*out_cell_norm_volume*out_cell_norm_volume;

    std::vector<boct_tree_cell<T_loc, T_data > *> cells = tree->leaf_cells();
    for (unsigned i=0; i<cells.size(); i++)
    {
      vgl_point_3d<double> node = tree->cell_bounding_box_local(cells[i]).min_point();

      float cell_val = boxm_cell_to_float(cells[i], step_len);

      unsigned int level = cells[i]->get_code().level;
      if (level == resolution_level) {
        // just copy value to output array
        //int out_index = static_cast<int>((node.x()/step_len)*ncells*ncells + (node.y()/step_len)*ncells + (node.z()/step_len));
        int out_index=static_cast<int>(ncells-1-(node.z()/step_len) + (node.y()/step_len)*ncells + (node.x()/step_len)*ncells*ncells);
        //////int out_index=static_cast<int>((node.z()/step_len) + (node.y()/step_len)*ncells + (node.x()/step_len)*ncells*ncells);
        //////int out_index=static_cast<int>(ncells-(node.z()/step_len)-1 + (node.x()/step_len)*ncells + (node.y()/step_len)*ncells*ncells);

        if (out_index >= data_size)
          std::cout << "boxm_save_block_raw, array out of index! " << out_index << " -- " << data_size << std::endl;
        else
          data[out_index] = cell_val;
      }
      else if (level > resolution_level) {
        // cell is bigger than output cells.  copy value to all contained output cells.
        const unsigned int us_factor = 1 << (level - resolution_level);
        const unsigned int node_x_start = static_cast<unsigned int>(node.x()/step_len);
        const unsigned int node_y_start = static_cast<unsigned int>(node.y()/step_len);
        const unsigned int node_z_start = static_cast<unsigned int>(node.z()/step_len);
        for (unsigned int z=node_z_start; z<node_z_start+us_factor; ++z) {
          for (unsigned int y=node_y_start; y<node_y_start+us_factor; ++y) {
            for (unsigned int x=node_x_start; x<node_x_start+us_factor; ++x) {
              //int out_index=x*y_size + y*ncells + z;
              int out_index=ncells-1-z + y*ncells + x*ncells*ncells;
              //////int out_index=z + y*ncells + x*ncells*ncells;
              //////int out_index=ncells-1-z + x*ncells + y*ncells*ncells;

              if (out_index >= data_size)
                std::cout << "boxm_save_block_raw, array out of index! " << out_index << " -- " << data_size << std::endl;
              else
                data[out_index] = cell_val;
            }
          }
        }
      }
      else {
        // cell is smaller than output cells. increment output cell value
        unsigned int ds_factor = 1 << (resolution_level - level);
        double update_weight = 1.0 / double(ds_factor*ds_factor*ds_factor);
        const unsigned int node_x = static_cast<unsigned int>(node.x()/step_len);
        const unsigned int node_y = static_cast<unsigned int>(node.y()/step_len);
        const unsigned int node_z = static_cast<unsigned int>(node.z()/step_len);
        //unsigned int out_index=node_x*y_size + node_y*ncells + node_z;
        int out_index=ncells-1-node_z + node_y*ncells + node_x*ncells*ncells;
        //////int out_index=node_z + node_y*ncells + node_x*ncells*ncells;
        //////int out_index=ncells-1-node_z + node_x*ncells + node_y*ncells*ncells;

        data[out_index] += float(cell_val*update_weight);
      }
    }

    // write the data into a bin file, generate a file name
    std::stringstream strm;
    strm << filename<< idx.x() << '_' << idx.y() << '_' << idx.z() << ".bin";
    std::string fn(strm.str());
    vsl_b_ofstream os(fn.c_str(),std::ios::binary);
    if (!os.os().good()) {
      std::cerr << "error opening " << filename << " for write!\n";
      return;
    }

    vsl_b_write(os, (unsigned int) ncells);  // x dimension
    vsl_b_write(os, (unsigned int) ncells);  // y dimension
    vsl_b_write(os, (unsigned int) ncells);  // z dimension

    T_data minv = hist.min();
    T_data maxv = hist.max();
    T_data rangev = maxv-minv;

    float* dp = data;
    for (; dp < data + data_size; ++dp) {
      unsigned char c = (unsigned char)(std::floor((255.0f * float((*dp)-minv)/float(rangev)) + 0.5f));
      vsl_b_write(os, (char) c);
    }
    delete[] data;
    os.close();
    iter++;
  }
  assert(ncells > 0);

  scene.unload_active_blocks();

  // combine the blocks
  vgl_vector_3d<unsigned> dim = scene.world_dim();
  unsigned dimx = dim.x()*ncells;
  unsigned dimy = dim.y()*ncells;
  unsigned dimz = dim.z()*ncells;
  // we will read the data a column at a time and this is enough
  int row_data_size = ncells;
  char* byte_data = new (std::nothrow) char[row_data_size];
  if (byte_data == nullptr) {
    std::cout << "boxm_save_block_raw: Could not allocate byte data!" << std::endl;
    return;
  }

  // open the binary files streams, saved earlier
  unsigned int nx,ny,nz;
  vbl_array_3d<vsl_b_ifstream*> streams(dim.x(), dim.y(), dim.z());
  for (unsigned z=0; z<dim.z(); z++) {
    for (unsigned y=0; y<dim.y(); y++) {
      for (unsigned x=0; x<dim.x(); x++) {
        vsl_b_ifstream*& s = streams(x,y,z);
        std::stringstream strm;
        strm << filename << x << '_' << y << '_' << z << ".bin";
        std::string fn(strm.str());
        s = new vsl_b_ifstream(fn.c_str(),std::ios::binary);
        vsl_b_read(*s, nx);
        vsl_b_read(*s, ny);
        vsl_b_read(*s, nz);
      }
    }
  }

  // start writing the raw header
  unsigned char data_type = 0; // 0 means unsigned byte
  vxl_uint_32 nx_uint = dimx;
  vxl_uint_32 ny_uint = dimy;
  vxl_uint_32 nz_uint = dimz;
  std::ofstream os(filename.c_str(),std::ios::binary);
  if (!os.good()) {
    std::cerr << "error opening " << filename << " for write!\n";
    return;
  }

  os.write(reinterpret_cast<char*>(&data_type),sizeof(data_type));
  os.write(reinterpret_cast<char*>(&nx_uint),sizeof(nx_uint));
  os.write(reinterpret_cast<char*>(&ny_uint),sizeof(ny_uint));
  os.write(reinterpret_cast<char*>(&nz_uint),sizeof(nz_uint));

  // combine the column from streams to generate one raw file
  for (unsigned x=0; x<dim.x(); x++) {
    unsigned k=0;
    while (k<dim.y()*dim.z()*ncells*ncells*ncells) {
      for (unsigned y=0; y<dim.y(); y++) {
        unsigned j=0;
        while (j < ncells*ncells*dim.z()) {
          for (unsigned z=dim.z();z>0; z--) {
            vsl_b_ifstream* s=streams(x,y,z-1);
            s->is().read(byte_data, ncells);
            os.write((char*)byte_data,ncells);
            j+=ncells;
            k+=ncells;
          }
        }
      }
    }
  }

  os.close();
  delete[] byte_data;

  // remove the intermediate bin files
  for (unsigned z=0; z<dim.z(); z++) {
    for (unsigned y=0; y<dim.y(); y++) {
      for (unsigned x=0; x<dim.x(); x++) {
        vsl_b_ifstream*& s = streams(x,y,z);
        s->close();
        std::stringstream strm;
        strm << filename << x << '_' << y << '_' << z << ".bin";
        std::string fn(strm.str());
        std::cout << "Deleting " << fn << std::endl;
        vpl_unlink(fn.data());
        delete s;
      }
    }
  }
  std::cout << "Volume Histogram\n";
  hist.pretty_print();
  return;
}
#endif
