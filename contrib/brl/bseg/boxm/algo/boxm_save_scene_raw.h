#ifndef boxm_save_scene_raw_h_
#define boxm_save_scene_raw_h_
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
//   <none yet>
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

#include <bsta/bsta_histogram.h>
#include <vbl/vbl_array_3d.hxx>
#include <vsl/vsl_binary_io.h>
#include <vpl/vpl.h>

#include <vcl_compiler.h>
#include <vcl_cassert.h>

template <class T_loc, class T_data>
void boxm_save_scene_raw(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                         std::string filename,
                         unsigned int resolution_level)
{
  float minv = 1.0e10, maxv = 0.0;
  bsta_histogram<float> h(0.0f, 1.0f, 20);
  typedef boct_tree<T_loc, T_data > tree_type;

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
  while (!iter.end()) {
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
      std::cerr << "error opening " << fn << " for write!\n";
      return;
    }

    vsl_b_write(os, (unsigned int) ncells);  // x dimension
    vsl_b_write(os, (unsigned int) ncells);  // y dimension
    vsl_b_write(os, (unsigned int) ncells);  // z dimension

    float* dp = data;
    for (; dp < data + data_size; ++dp) {
      //double P = 1.0 - std::exp(-*dp*step_len);
      // always positive so this is an ok way to round
      float fv = *dp;
      h.upcount(fv, 1.0f);
      if (fv<minv) minv = fv;
      if (fv>maxv) maxv = fv;
      unsigned char c = (unsigned char)(std::floor((255.0 * (*dp)) + 0.5));
      vsl_b_write(os, (char) c);
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
  std::cout << "Volume Histogram(" << minv << ' ' << maxv << "):[0->1]20bins\n";
  h.print();
  return;
}

#endif // boxm_save_scene_raw_h_
