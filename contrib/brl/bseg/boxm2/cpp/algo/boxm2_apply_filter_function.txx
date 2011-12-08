#ifndef boxm2_apply_filter_function_txx
#define boxm2_apply_filter_function_txx
//:
// \file
#include "boxm2_apply_filter_function.h"

#if 0
# define PROB
#endif

#include <vcl_cassert.h>
#include <vcl_iostream.h>

//: "default" constructor
template<boxm2_data_type RESPONSE_DATA_TYPE>
boxm2_apply_filter_function<RESPONSE_DATA_TYPE>::boxm2_apply_filter_function(
    vcl_string kernel_base_file_name, unsigned num_kernels)
{
  kernels_.clear();
  num_kernels_ = 0;
  vcl_string filename;
  vgl_box_3d<float> box;

  for (unsigned i = 0; i < num_kernels; i++) {
    //compute full kernel file name
    vcl_stringstream ss;
    ss << kernel_base_file_name << '_' << i << ".txt";
    vcl_cout << "Loading " << ss.str() << vcl_endl;

    //load kernel from file
    vcl_vector<vcl_pair<vgl_point_3d<float> , float> > kernel;
    vcl_ifstream ifs(ss.str().c_str());

    //check file
    if (!ifs.good()) {
      vcl_cout << "Problem loading kernels...exiting." << vcl_endl;
      return;
    }

    //read the min and max kernel extent
    vgl_point_3d<float> min, max;
    ifs >> min;
    ifs >> max;
    box.add(min); box.add(max);

    //read 3d points and weights
    while (true) {
      vgl_point_3d<float> this_loc;
      float weight;
      ifs >> this_loc;
      ifs >> weight;
      if (ifs.eof())
        break;
      kernel.push_back(vcl_pair<vgl_point_3d<float> , float> (
          vgl_point_3d<float> (this_loc.x(), this_loc.y(), this_loc.z()), weight));
    }

    //push onto vector
    kernels_.push_back(kernel);
    num_kernels_++;
  }
  vcl_cout << "Loaded all kernels successfully..." << vcl_endl;

  //compute min and max kernel extent
  kernel_extent_min_ = box.min_point();
  kernel_extent_max_ = box.max_point();

  vcl_cout << "Kernel extent is from " << kernel_extent_min_ << " to " << kernel_extent_max_ << vcl_endl;
}

template<boxm2_data_type RESPONSE_DATA_TYPE>
void boxm2_apply_filter_function<RESPONSE_DATA_TYPE>::apply_filter(
    boxm2_block_metadata data, boxm2_block* blk, boxm2_data_base* alphas,
    boxm2_data_base* response, boxm2_data_base* points, float prob_threshold,
    boxm2_data_base* normals, vcl_vector<vnl_vector_fixed<double,4> > * normal_dir)
{
  boxm2_block_id id = blk->block_id();

  //3d array of trees
  boxm2_array_3d<uchar16>& trees = blk->trees();
  boxm2_data_traits<BOXM2_ALPHA>::datatype * alpha_data = (boxm2_data_traits<BOXM2_ALPHA>::datatype*) alphas->data_buffer();
  typename boxm2_data_traits<RESPONSE_DATA_TYPE>::datatype * response_data =
          (typename boxm2_data_traits<RESPONSE_DATA_TYPE>::datatype*) response->data_buffer();
  boxm2_data_traits<BOXM2_POINT>::datatype* points_data = (boxm2_data_traits<BOXM2_POINT>::datatype*) points->data_buffer();

  //iterate through each block, filtering the root level first
  for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
    vcl_cout << '[' << x << '/' << trees.get_row1_count() << ']' << vcl_flush;
    for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
      for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
        //load current block/tree
        uchar16 tree = trees(x, y, z);
        boct_bit_tree bit_tree((unsigned char*) tree.data_block(), data.max_level_);

        //FOR ALL LEAVES IN CURRENT TREE
        vcl_vector<int> leafBits = bit_tree.get_leaf_bits();
        vcl_vector<int>::iterator iter;
        for (iter = leafBits.begin(); iter != leafBits.end(); ++iter) {
          int currBitIndex = (*iter);
          int currIdx = bit_tree.get_data_index(currBitIndex);

          //side length of the cell
          int curr_depth = bit_tree.depth_at(currBitIndex);
          double side_len = 1.0 / (double) (1 << curr_depth);

          float prob = 1.0f - vcl_exp(-alpha_data[currIdx] * side_len * data.sub_block_dim_.x());

          //clear the responses as well as the point data
          for (unsigned response_id = 0; response_id < num_kernels_; response_id++)
            response_data[currIdx][response_id] = 0;

          //init portals
          points_data[currIdx][0] = 0;
          points_data[currIdx][1] = 0;
          points_data[currIdx][2] = 0;
          points_data[currIdx][3] = 0;

          //init normals
          if (normals) {
            boxm2_data_traits<BOXM2_NORMAL>::datatype* normals_data = (boxm2_data_traits<BOXM2_NORMAL>::datatype*) normals->data_buffer();
            normals_data[currIdx][0] = 0;
            normals_data[currIdx][1] = 0;
            normals_data[currIdx][2] = 0;
            normals_data[currIdx][3] = 0;
          }

          if (prob < prob_threshold)
            continue;

          //get cell center, and get neighbor points needed
          vgl_point_3d<double> localCenter = bit_tree.cell_center(currBitIndex);
          vgl_point_3d<double> cellCenter(localCenter.x() + x, localCenter.y()+ y, localCenter.z() + z);

          //compute neighborhood of cell center
          vcl_vector<vcl_pair<vgl_point_3d<int> , vgl_point_3d<double> > > neighborhood;
          //if at least one kernel cannot be evaluated, don' compute anything
          if (!neighbor_points(cellCenter, side_len, trees, neighborhood))
            continue;

          //eval neighborhood
          vcl_vector<vcl_pair<vgl_point_3d<int>, float> > neighbors = eval_neighbors(data, bit_tree, neighborhood, trees, alpha_data,  curr_depth);

          //apply each filter
          for (unsigned response_id = 0; response_id < num_kernels_; response_id++)
            response_data[currIdx][response_id] = eval_filter(neighbors, kernels_[response_id]);

          points_data[currIdx][0] = (cellCenter.x() )*data.sub_block_dim_.x() + data.local_origin_.x();
          points_data[currIdx][1] = (cellCenter.y() )*data.sub_block_dim_.y() + data.local_origin_.y();
          points_data[currIdx][2] = (cellCenter.z() )*data.sub_block_dim_.z() + data.local_origin_.z();
#ifdef PROB
          points_data[currIdx][3] = prob; //use the unused field in points to store prob
#else
          points_data[currIdx][3] = alpha_data[currIdx]; //use the unused field in points to store prob
#endif

          //interpolate result to normals if it is present
          if (normals) {
            boxm2_data_traits<BOXM2_NORMAL>::datatype* normals_data = (boxm2_data_traits<BOXM2_NORMAL>::datatype*) normals->data_buffer();

            double normal_x = 0, normal_y = 0, normal_z = 0;
            double sum_weights_x = 0, sum_weights_y = 0, sum_weights_z = 0;
            for (unsigned response_id = 0; response_id < num_kernels_; response_id++)
            {
              normal_x += response_data[currIdx][response_id] * (*normal_dir)[response_id][0];
              normal_y += response_data[currIdx][response_id] * (*normal_dir)[response_id][1];
              normal_z += response_data[currIdx][response_id] * (*normal_dir)[response_id][2];
              sum_weights_x += vcl_fabs((*normal_dir)[response_id][0]);
              sum_weights_y += vcl_fabs((*normal_dir)[response_id][1]);
              sum_weights_z += vcl_fabs((*normal_dir)[response_id][2]);
            }
            normal_x /= sum_weights_x;  normal_y /= sum_weights_y; normal_z /= sum_weights_z;

            float norm = vcl_sqrt(vcl_pow(normal_x,2) + vcl_pow(normal_y,2) + vcl_pow(normal_y,2));
            normal_x /= norm; normal_y /= norm; normal_z /= norm;

            normals_data[currIdx][0] = normal_x;
            normals_data[currIdx][1] = normal_y;
            normals_data[currIdx][2] = normal_z;
            normals_data[currIdx][3] = norm;
          }
        } //end leaf for
      } //end z for
    } //end y for
  } // end x for
}


template<boxm2_data_type RESPONSE_DATA_TYPE>
bool boxm2_apply_filter_function<RESPONSE_DATA_TYPE>::neighbor_points(
    const vgl_point_3d<double>& cellCenter, double side_len,
    const boxm2_array_3d<uchar16>& trees, vcl_vector<vcl_pair<vgl_point_3d<int> , vgl_point_3d<double> > >& neighborhood)
{
   for (int i = kernel_extent_min_.x(); i <= kernel_extent_max_.x(); i++) {
     for (int j = kernel_extent_min_.y(); j <= kernel_extent_max_.y(); j++) {
       for (int k = kernel_extent_min_.z(); k <= kernel_extent_max_.z(); k++) {
         //check if calculated neighbor is out of bounds
         if ( cellCenter.x() + i*side_len < trees.get_row1_count() &&
              cellCenter.y() + j*side_len < trees.get_row2_count() &&
              cellCenter.z() + k*side_len < trees.get_row3_count() &&
              cellCenter.x() + i*side_len >= 0 &&
              cellCenter.y() + j*side_len >= 0 &&
              cellCenter.z() + k*side_len >= 0) {
             vcl_pair<vgl_point_3d<int>, vgl_point_3d<double> > mypair(
                 vgl_point_3d<int>(i, j, k),
                 vgl_point_3d<double>(cellCenter.x() + i*side_len, cellCenter.y() + j*side_len,  cellCenter.z() + k*side_len));
             neighborhood.push_back(mypair);
         }
         else
           return false; //immediately return
       }
     }
   }

  return true;
}

template<boxm2_data_type RESPONSE_DATA_TYPE>
vcl_vector<vcl_pair<vgl_point_3d<int> , float> > boxm2_apply_filter_function<RESPONSE_DATA_TYPE>::eval_neighbors(
    boxm2_block_metadata data,
    const boct_bit_tree& bit_tree, const vcl_vector<vcl_pair<vgl_point_3d<int> , vgl_point_3d<double> > > & neighbors,
    const boxm2_array_3d<uchar16>& trees, const boxm2_data_traits<BOXM2_ALPHA>::datatype* alpha_data, int curr_depth)
{
  vcl_vector<vcl_pair<vgl_point_3d<int> , float> > probs;

  double side_len = 1.0 / (double) (1 << curr_depth);

  for (unsigned int i = 0; i < neighbors.size(); ++i) {
    //load neighbor block/tree
    vgl_point_3d<double> abCenter = neighbors[i].second;
    vgl_point_3d<int> blkIdx((int)abCenter.x(), (int)abCenter.y(), (int)abCenter.z());
    uchar16 ntree = trees(blkIdx.x(), blkIdx.y(), blkIdx.z());
    boct_bit_tree neighborTree((unsigned char*) ntree.data_block(), data.max_level_);

    //traverse to local center
    vgl_point_3d<double> locCenter((double) abCenter.x() - blkIdx.x(),
                                   (double) abCenter.y() - blkIdx.y(),
                                   (double) abCenter.z() - blkIdx.z());
    int neighborBitIdx = neighborTree.traverse(locCenter, curr_depth);

    //if the cells are the same size, or the neighbor is larger
    if (neighborTree.is_leaf(neighborBitIdx)) {
      //get data index
      int idx = neighborTree.get_data_index(neighborBitIdx);

#ifdef PROB
      //grab alpha, calculate probability
      boxm2_data_traits<BOXM2_ALPHA>::datatype alpha = alpha_data[idx];
      float prob = 1.0f - (float)vcl_exp(-alpha * side_len * data.sub_block_dim_.x());

      vcl_pair<vgl_point_3d<int>, float> mypair(neighbors[i].first, prob);
      probs.push_back(mypair);
#else
      //grab alpha
      boxm2_data_traits<BOXM2_ALPHA>::datatype alpha = alpha_data[idx];
      vcl_pair<vgl_point_3d<int> , float> mypair(neighbors[i].first, alpha);
      probs.push_back(mypair);
#endif
    }
    else //neighbor is smaller, must combine neighborhood
    {
      //get cell, combine neighborhood to one probability

      boxm2_data_traits<BOXM2_ALPHA>::datatype totalAlphaL = 0.0f;

      vcl_vector<int> subLeafBits = neighborTree.get_leaf_bits(neighborBitIdx);
      vcl_vector<int>::iterator leafIter;
      for (leafIter = subLeafBits.begin(); leafIter != subLeafBits.end(); ++leafIter) {
        //side length of the cell
        int ndepth = bit_tree.depth_at(*leafIter);
        double nlen = 1.0 / (double) (1 << ndepth);
        int dataIndex = neighborTree.get_data_index(*leafIter);
#ifdef PROB
        totalAlphaL += (float)(alpha_data[dataIndex] * nlen * data.sub_block_dim_.x());
#else
        totalAlphaL += (float) (alpha_data[dataIndex]);
#endif
      }

#ifdef PROB
      float prob = 1.0f - vcl_exp( -totalAlphaL );
#else
      float prob = totalAlphaL;
#endif
      vcl_pair<vgl_point_3d<int> , float> mypair(neighbors[i].first, prob);
      probs.push_back(mypair);
    }
  }

  return probs;
}

template<boxm2_data_type RESPONSE_DATA_TYPE>
float boxm2_apply_filter_function<RESPONSE_DATA_TYPE>::eval_filter(
    vcl_vector<vcl_pair<vgl_point_3d<int> , float> > neighbors, vcl_vector<vcl_pair<vgl_point_3d<float> , float> > filter)
{
  float sum = 0;
  for (unsigned i = 0; i < filter.size(); i++) {
    vgl_point_3d<float> loc_float = filter[i].first;
    float x = loc_float.x(), y = loc_float.y(), z = loc_float.z();

    //interpolate
    int x_interp = (x >= 0) ? (int)(x + 0.5) : (int)(x - 0.5);
    int y_interp = (y >= 0) ? (int)(y + 0.5) : (int)(y - 0.5);
    int z_interp = (z >= 0) ? (int)(z + 0.5) : (int)(z - 0.5);

    vgl_point_3d<int> loc_interp(x_interp, y_interp, z_interp);

    vcl_vector<vcl_pair<vgl_point_3d<int> , float> >::const_iterator it = neighbors.begin();
    bool found = false;
    while (!found) {
      if ((*it).first == loc_interp)
        found = true;
      ++it;
    }
    assert(found);
    //add data*filter to sum
    sum += filter[i].second * (*it).second;
  }
  return sum;
}

#endif //boxm2_apply_filter_function_txx
