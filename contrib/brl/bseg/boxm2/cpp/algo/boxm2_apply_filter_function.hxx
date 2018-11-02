#ifndef boxm2_apply_filter_function_hxx_
#define boxm2_apply_filter_function_hxx_
//:
// \file
#include <iostream>
#include "boxm2_apply_filter_function.h"

#if 1
# define PROB
#endif


#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: "default" constructor
template<boxm2_data_type RESPONSE_DATA_TYPE>
boxm2_apply_filter_function<RESPONSE_DATA_TYPE>::boxm2_apply_filter_function(
    std::string kernel_base_file_name, unsigned id_kernel)
{
  std::string filename;

  //compute full kernel file name
  std::stringstream ss;
  ss << kernel_base_file_name << '_' << id_kernel << ".txt";
  std::cout << "Loading " << ss.str() << std::endl;

  //load kernel from file
  std::ifstream ifs(ss.str().c_str());

  //check file
  if (!ifs.good()) {
    std::cout << "Problem loading kernels...exiting." << std::endl;
    return;
  }

  //read the min and max kernel extent
  vgl_point_3d<float> min, max;
  ifs >> min;
  ifs >> max;

  //read 3d points and weights
  while (true) {
    vgl_point_3d<float> this_loc;
    float weight;
    ifs >> this_loc;
    ifs >> weight;

    float x = this_loc.x(), y = this_loc.y(), z = this_loc.z();
    //interpolate
    int x_interp = (x >= 0) ? (int)(x + 0.5) : (int)(x - 0.5);
    int y_interp = (y >= 0) ? (int)(y + 0.5) : (int)(y - 0.5);
    int z_interp = (z >= 0) ? (int)(z + 0.5) : (int)(z - 0.5);

    vnl_vector_fixed<int,3> loc_interp(x_interp, y_interp, z_interp);

    if (ifs.eof())
      break;
    //insert to kernel
    kernel_[loc_interp] = weight;
  }
  std::cout << "Loaded kernel " << id_kernel << " successfully...\n"
           << "Kernel extent is from " << min << " to " << max << std::endl;
}


template<boxm2_data_type RESPONSE_DATA_TYPE>
void boxm2_apply_filter_function<RESPONSE_DATA_TYPE>::apply_filter(
    boxm2_block_metadata data, boxm2_block* blk, boxm2_data_base* alphas,
    boxm2_data_base* response, float prob_threshold, unsigned octree_lvl)
{
  boxm2_block_id id = blk->block_id();

  //3d array of trees
  const boxm2_array_3d<uchar16>& trees = blk->trees();
  boxm2_data_traits<BOXM2_ALPHA>::datatype * alpha_data = (boxm2_data_traits<BOXM2_ALPHA>::datatype*) alphas->data_buffer();

  typename boxm2_data_traits<RESPONSE_DATA_TYPE>::datatype * response_data =
          (typename boxm2_data_traits<RESPONSE_DATA_TYPE>::datatype*) response->data_buffer();

  //iterate through each block, filtering the root level first
  for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
    std::cout << '[' << x << '/' << trees.get_row1_count() << ']' << std::flush;
    for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
      for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
        //load current block/tree
        uchar16 tree = trees(x, y, z);
        boct_bit_tree bit_tree((unsigned char*) tree.data_block(), data.max_level_);

        //FOR ALL LEAVES IN CURRENT TREE
        std::vector<int> leafBits = bit_tree.get_leaf_bits();
        std::vector<int>::iterator iter;
        for (iter = leafBits.begin(); iter != leafBits.end(); ++iter) {
          int currBitIndex = (*iter);
          int currIdx = bit_tree.get_data_index(currBitIndex);

          double side_len = 1.0 / (double) (1 << octree_lvl);
          float prob = 1.0f - (float)std::exp(-alpha_data[currIdx] * side_len * data.sub_block_dim_.x());

          response_data[currIdx] = 0.0f;

          if (prob < prob_threshold)
            continue;

          //get cell center, and get neighbor points needed
          vgl_point_3d<double> localCenter = bit_tree.cell_center(currBitIndex);
          vgl_point_3d<double> cellCenter(localCenter.x() + x, localCenter.y()+ y, localCenter.z() + z);

#if 0
          //TODO: move this to a more decent place, a process that is run prior to this for instance.
          //save pt info
          points_data[currIdx][0] = (cellCenter.x() )*data.sub_block_dim_.x() + data.local_origin_.x();
          points_data[currIdx][1] = (cellCenter.y() )*data.sub_block_dim_.y() + data.local_origin_.y();
          points_data[currIdx][2] = (cellCenter.z() )*data.sub_block_dim_.z() + data.local_origin_.z();
          points_data[currIdx][3] = prob; //use the unused field in points to store prob
#endif


          //compute neighborhood of cell center
          std::map<vnl_vector_fixed<int,3> , vgl_point_3d<double> > neighborhood;

          //if at least one kernel cannot be evaluated, don't compute anything and skip voxel
          if (!neighbor_points(cellCenter, side_len, trees, neighborhood))
            continue;

          //apply filter
          response_data[currIdx] = eval_filter(neighborhood, data, bit_tree, trees, alpha_data, octree_lvl);
        } //end leaf for
      } //end z for
    } //end y for
  } // end x for
}


template<boxm2_data_type RESPONSE_DATA_TYPE>
bool boxm2_apply_filter_function<RESPONSE_DATA_TYPE>::neighbor_points(
    const vgl_point_3d<double>& cellCenter, double side_len,
    const boxm2_array_3d<uchar16>& trees, std::map < vnl_vector_fixed<int,3> , vgl_point_3d<double> >& neighborhood)
{

  for (std::map< vnl_vector_fixed<int,3> , float>::const_iterator iter = kernel_.begin(); iter != kernel_.end(); iter++) {
    vnl_vector_fixed<int,3> v = (*iter).first;
    int i = v[0], j = v[1], k = v[2];

    //check if calculated neighbor is out of bounds
    if ( cellCenter.x() + i*side_len < trees.get_row1_count() &&
         cellCenter.y() + j*side_len < trees.get_row2_count() &&
         cellCenter.z() + k*side_len < trees.get_row3_count() &&
         cellCenter.x() + i*side_len >= 0 &&
         cellCenter.y() + j*side_len >= 0 &&
         cellCenter.z() + k*side_len >= 0) {
        std::pair< vnl_vector_fixed<int,3> , vgl_point_3d<double> >
          mypair(v,
                 vgl_point_3d<double>(cellCenter.x() + i*side_len, cellCenter.y() + j*side_len,  cellCenter.z() + k*side_len));
        neighborhood.insert(mypair);
    }
    else
      return false; //immediately return
  }
  return true;
}

template<boxm2_data_type RESPONSE_DATA_TYPE>
float boxm2_apply_filter_function<RESPONSE_DATA_TYPE>::eval_alpha(
    boxm2_block_metadata data, const boct_bit_tree& bit_tree, const vgl_point_3d<double> & point,
    const boxm2_array_3d<uchar16>& trees, const boxm2_data_traits<BOXM2_ALPHA>::datatype* alpha_data, int curr_depth)
{
    double side_len = 1.0 / (double) (1 << curr_depth);


    vgl_point_3d<int> blkIdx((int)point.x(), (int)point.y(), (int)point.z());
    uchar16 ntree = trees(blkIdx.x(), blkIdx.y(), blkIdx.z());
    boct_bit_tree neighborTree((unsigned char*) ntree.data_block(), data.max_level_);

    //traverse to local center
    vgl_point_3d<double> locCenter((double) point.x() - blkIdx.x(),
                                   (double) point.y() - blkIdx.y(),
                                   (double) point.z() - blkIdx.z());
    int neighborBitIdx = neighborTree.traverse(locCenter, curr_depth);

    //if the cells are the same size, or the neighbor is larger
    if (neighborTree.is_leaf(neighborBitIdx)) {
      //get data index
      int idx = neighborTree.get_data_index(neighborBitIdx);

#ifdef PROB
      //grab alpha, calculate probability
      boxm2_data_traits<BOXM2_ALPHA>::datatype alpha = alpha_data[idx];

      return 1.0f - (float)std::exp(-alpha * side_len * data.sub_block_dim_.x());
#else
      //grab alpha
      return alpha_data[idx];
#endif
    }
    else //neighbor is smaller, must combine neighborhood
    {
      //get cell, combine neighborhood to one probability
      boxm2_data_traits<BOXM2_ALPHA>::datatype totalAlphaL = 0.0f;

      std::vector<int> subLeafBits = neighborTree.get_leaf_bits(neighborBitIdx);
      std::vector<int>::iterator leafIter;
      for (leafIter = subLeafBits.begin(); leafIter != subLeafBits.end(); ++leafIter) {
        //side length of the cell
        int dataIndex = neighborTree.get_data_index(*leafIter);
        totalAlphaL += (float)(alpha_data[dataIndex]);
      }

#ifdef PROB
      return 1.0f - (float)std::exp( -totalAlphaL * side_len);
#else
      return totalAlphaL;
#endif
    }
}


template<boxm2_data_type RESPONSE_DATA_TYPE>
float boxm2_apply_filter_function<RESPONSE_DATA_TYPE>::eval_filter(std::map<vnl_vector_fixed<int,3> , vgl_point_3d<double> > neighbors,
                                                                   boxm2_block_metadata data, const boct_bit_tree& bit_tree,  const boxm2_array_3d<uchar16>& trees,
                                                                   const boxm2_data_traits<BOXM2_ALPHA>::datatype* alpha_data, int curr_depth)
{

  float sum = 0;
  for (std::map< vnl_vector_fixed<int,3> , float>::const_iterator iter = kernel_.begin(); iter !=kernel_.end(); iter++)
    sum += (*iter).second * eval_alpha(data, bit_tree, neighbors[(*iter).first], trees, alpha_data, curr_depth);     //add data*filter to sum

  return sum;
}


#endif //boxm2_apply_filter_function_hxx_
