#ifndef boxm_save_block_raw_h_
#define boxm_save_block_raw_h_

#include <vcl_vector.h>

#include <boct/boct_tree.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_sample.h>
#include <boxm/boxm_utils.h>
#include <boxm/boxm_block_vis_graph_iterator.h>
#include <boxm/boxm_cell_vis_graph_iterator.h>
#include <boxm/boxm_mog_grey_processor.h>
#include <vpgl/vpgl_camera.h>

#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_transform.h>
#include <vil/vil_save.h>
#include <vil/vil_view_as.h>
#include <boxm/boxm_block_vis_graph_iterator.h>
#include <boxm/boxm_cell_vis_graph_iterator.h>
#include <boxm/boxm_mog_grey_processor.h>
#include <vcl_iostream.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>


/*class image_exp_functor
{
public:
	float operator()(float x)       const { return x<0?vcl_exp(x):1.0f; }
};*/


template <class T_loc, boxm_apm_type APM>
void boxm_save_block_raw(boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene,
                         vgl_point_3d<int> block_idx,
                         vcl_string filename, 
                         unsigned int resolution_level)
{
  typedef boct_tree<T_loc, boxm_sample<APM> > tree_type;

  vcl_ofstream os(filename.c_str(),vcl_ios::binary);
  if (!os.good()) {
    vcl_cerr << "error opening " << filename << " for write! " << vcl_endl;
    return;
  }
  
  scene.load_block(block_idx);
	boxm_block<tree_type>* block = scene.get_block(block_idx);
  vgl_box_3d<double> block_bb = block->bounding_box();
  tree_type* tree = block->get_tree();

  // get origin of block
  vgl_point_3d<double> min = block_bb.min_point();

  const unsigned int ncells = 1 << (tree->num_levels() - 1 - resolution_level);
  // assume that bounding box is a cube
  const double step_len = ((block_bb.max_x() - block_bb.min_x())/double(ncells));

  // origin should specify center of first cell
  vgl_point_3d<double> data_og(min.x() + (step_len/2.0), min.y() + (step_len/2.0), min.z() + (step_len/2.0));
  
  float *data = new float[ncells*ncells*ncells];
  // init to zero
  for (float* dp = data; dp < data + ncells*ncells*ncells; ++dp) {
    *dp = 0.0f;
  }

  double out_cell_norm_volume = (tree->num_levels() - resolution_level + 1);
  out_cell_norm_volume = out_cell_norm_volume*out_cell_norm_volume*out_cell_norm_volume;

  vcl_vector<boct_tree_cell<T_loc, boxm_sample<APM> > *> cells = tree->leaf_cells();
  for (unsigned i=0; i<cells.size(); i++) {
    vgl_point_3d<double> node = tree->cell_bounding_box_local(cells[i]).min_point();
    
    float cell_val = cells[i]->data().alpha;
    
    short level = cells[i]->get_code().level;

    if (level == resolution_level) {
      // just copy value to output array
      unsigned int out_index = node.x()*ncells*ncells + node.y()*ncells + node.z();
      data[out_index] = cell_val;
    }
    else if (level > resolution_level) {
      // cell is bigger than output cells.  copy value to all contained output cells.
      const unsigned int us_factor = 1 << (level - resolution_level);
      const unsigned int node_x_start = node.x()/step_len; //*us_factor;
      const unsigned int node_y_start = node.y()/step_len; //*us_factor;
      const unsigned int node_z_start = node.z()/step_len; //*us_factor;
      for (unsigned int z=node_z_start; z<node_z_start+us_factor; ++z) {
        for (unsigned int y=node_y_start; y<node_y_start+us_factor; ++y) {
          for (unsigned int x=node_x_start; x<node_x_start+us_factor; ++x) {
            unsigned int out_index = z*ncells*ncells + y*ncells + x;
            //vcl_cout << x << " " << y << " " << z << " " << out_index << vcl_endl;
            data[out_index] = cell_val;
          }
        }
      }
    }
    else {
      // cell is smaller than output cells. increment output cell value
      unsigned int ds_factor = 1 << (level - resolution_level);
      double update_weight = 1.0 / double(ds_factor*ds_factor*ds_factor);
      unsigned int out_index = (node.z()/ds_factor)*ncells*ncells + (node.y()/ds_factor)*ncells + (node.x()/ds_factor);
      data[out_index] += float(cell_val*update_weight);
    }
  }
  // convert float values to char
  unsigned char *byte_data = new unsigned char[ncells*ncells*ncells];
  float* dp = data;
  for (unsigned char* bdp = byte_data; dp < data + ncells*ncells*ncells; ++dp, ++bdp) {
    double P = 1.0 - vcl_exp(-*dp*step_len);
    *bdp = (unsigned char)(vcl_floor((255.0 * P) + 0.5)); // always positive so this is an ok way to round
  }
  delete[] data;

  // write header
  unsigned char data_type = 0; // 0 means unsigned byte

  vxl_uint_32 nx = ncells;
  vxl_uint_32 ny = ncells;
  vxl_uint_32 nz = ncells;

  os.write(reinterpret_cast<char*>(&data_type),sizeof(data_type));
  os.write(reinterpret_cast<char*>(&nx),sizeof(nx));
  os.write(reinterpret_cast<char*>(&ny),sizeof(ny));
  os.write(reinterpret_cast<char*>(&nz),sizeof(nz));

  os.write((char*)byte_data,ncells*ncells*ncells);
  delete[] byte_data;

  return;
}




#endif

