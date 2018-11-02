#ifndef boxm_save_block_raw_h_
#define boxm_save_block_raw_h_

#include <vector>
#include <new>
#include <iostream>

#include <boct/boct_tree.h>
#include <boxm/boxm_scene.h>
#include <boxm/sample/boxm_sample.h>
#include <boxm/basic/boxm_block_vis_graph_iterator.h>
#include <boxm/basic/boxm_cell_vis_graph_iterator.h>
#include <boxm/sample/algo/boxm_mog_grey_processor.h>
#include <boxm/util/boxm_cell_data_traits.h>

#include <bsta/bsta_attributes.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T_loc, class T_data>
void boxm_save_block_raw(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                         vgl_point_3d<int> block_idx,
                         std::string filename,
                         unsigned int resolution_level)
{
  typedef boct_tree<T_loc, T_data > tree_type;

  scene.load_block(block_idx);
  boxm_block<tree_type>* block = scene.get_block(block_idx);
  vgl_box_3d<double> block_bb = block->bounding_box();
  tree_type* tree = block->get_tree();

  // query the finest level of the tree and do not make the resolution smaller than that
  if (tree->finest_level() > (int)resolution_level)
    resolution_level = tree->finest_level();

  // get origin of block
  vgl_point_3d<double> min = block_bb.min_point();

  const unsigned int ncells = 1 << (tree->root_level() - (int)resolution_level);

  // assume that bounding box is a cube
  const double step_len = ((block_bb.max_x() - block_bb.min_x())/double(ncells));

  // origin should specify center of first cell
  vgl_point_3d<double> data_og(min.x() + (step_len/2.0), min.y() + (step_len/2.0), min.z() + (step_len/2.0));

  int data_size = ncells*ncells*ncells;
  int y_size = ncells*ncells;
  float *data = nullptr;
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
      int out_index = static_cast<int>((node.x()/step_len)*ncells*ncells + (node.y()/step_len)*ncells + (node.z()/step_len));
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
            int out_index = z*y_size + y*ncells + x;
            //std::cout << level << "  " << out_index << std::endl;
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
      unsigned int out_index = static_cast<unsigned int>((node_z)*y_size + (node_y)*ncells + (node_x));
      data[out_index] += float(cell_val*update_weight);
    }
  }
  // convert float values to char

  unsigned char *byte_data = nullptr;
  byte_data = new (std::nothrow) unsigned char[data_size];
  if (byte_data == nullptr) {
    std::cout << "boxm_save_block_raw: Could not allocate byte data!" << std::endl;
    return;
  }

  float* dp = data;
  for (unsigned char* bdp = byte_data; dp < data + data_size; ++dp, ++bdp) {
    //double P = 1.0 - std::exp(-*dp*step_len);
    *bdp = (unsigned char)(std::floor((255.0 * (*dp)) + 0.5)); // always positive so this is an ok way to round
  }
  delete[] data;

  // write header
  unsigned char data_type = 0; // 0 means unsigned byte

  vxl_uint_32 nx = ncells;
  vxl_uint_32 ny = ncells;
  vxl_uint_32 nz = ncells;

  std::ofstream os(filename.c_str(),std::ios::binary);
  if (!os.good()) {
    std::cerr << "error opening " << filename << " for write!\n";
    return;
  }

  os.write(reinterpret_cast<char*>(&data_type),sizeof(data_type));
  os.write(reinterpret_cast<char*>(&nx),sizeof(nx));
  os.write(reinterpret_cast<char*>(&ny),sizeof(ny));
  os.write(reinterpret_cast<char*>(&nz),sizeof(nz));

  os.write((char*)byte_data,data_size);
  delete[] byte_data;

  return;
}


#endif
