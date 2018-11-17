#include "boxm2_compute_derivative_function.h"
//:
// \file
#include <cassert>
#include <utility>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#if 1
# define PROB
#endif

//: "default" constructor
boxm2_compute_derivative_function::boxm2_compute_derivative_function(
            boxm2_block_metadata data, boxm2_block* blk,
            boxm2_data_base* alphas, boxm2_data_base* normals,  boxm2_data_base* points, float prob_threshold,
            float normal_threshold, std::string kernel_x_name, std::string kernel_y_name, std::string kernel_z_name)
{
  //initialize kernels
  kernel_x_ = load_kernel(std::move(kernel_x_name));
  kernel_y_ = load_kernel(std::move(kernel_y_name));
  kernel_z_ = load_kernel(std::move(kernel_z_name));

  boxm2_block_id id = blk->block_id();

  //3d array of trees
  const boxm2_array_3d<uchar16>& trees = blk->trees();
  auto *   alpha_data = (boxm2_data_traits<BOXM2_ALPHA>::datatype*) alphas->data_buffer();
  auto * normals_data = (boxm2_data_traits<BOXM2_NORMAL>::datatype*) normals->data_buffer();
  auto* points_data = (boxm2_data_traits<BOXM2_POINT>::datatype*)points->data_buffer();

  //iterate through each block, filtering the root level first
  std::cout << "Filtering scene: "<< std::flush
           << "(with threshold)" << prob_threshold << ' ' << normal_threshold << std::endl;
  for (unsigned int x=0; x<trees.get_row1_count(); ++x)
  {
    std::cout<<'['<<x<<'/'<<trees.get_row1_count()<<']'<<std::flush;
    for (unsigned int y=0; y<trees.get_row2_count(); ++y)
    {
      for (unsigned int z=0; z<trees.get_row3_count(); ++z)
      {
        //load current block/tree
        uchar16 tree = trees(x,y,z);
        boct_bit_tree bit_tree( (unsigned char*) tree.data_block(), data.max_level_);

        //FOR ALL LEAVES IN CURRENT TREE
        std::vector<int> leafBits = bit_tree.get_leaf_bits();
        std::vector<int>::iterator iter;
        for (iter = leafBits.begin(); iter != leafBits.end(); ++iter)
        {
          int currBitIndex = (*iter);
          int currIdx = bit_tree.get_data_index(currBitIndex);

          //side length of the cell
          int curr_depth = bit_tree.depth_at(currBitIndex);
          double side_len = 1.0 / (double) (1<<curr_depth);

          float prob = 1.0f - (float)std::exp( -alpha_data[currIdx] * side_len * data.sub_block_dim_.x() );

          //put dummy 0s in both normals and points
          normals_data[currIdx][0] = 0;
          normals_data[currIdx][1] = 0;
          normals_data[currIdx][2] = 0;
          points_data[currIdx][0] = 0;
          points_data[currIdx][1] = 0;
          points_data[currIdx][2] = 0;
          points_data[currIdx][3] = 0;


          if (prob < prob_threshold)
              continue;

          //get cell center, and get six neighbor points
          vgl_point_3d<double> localCenter = bit_tree.cell_center(currBitIndex);
          vgl_point_3d<double> cellCenter(localCenter.x() + x, localCenter.y() + y, localCenter.z() + z);


          std::vector<std::pair<vgl_point_3d<int>, vgl_point_3d<double> > >  neighbor_points = this->neighbor_points(cellCenter, side_len, trees, 5);
          if (neighbor_points.size() != kernel_x_.size())
            continue;

          std::vector<std::pair<vgl_point_3d<int>, float> > neighbors = eval_neighbors(data, bit_tree, neighbor_points, trees, alpha_data,  curr_depth);

          //compute derivatives in x,y,z direction
          float derivative_x = apply_filter(neighbors,kernel_x_);
          float derivative_y = apply_filter(neighbors,kernel_y_);
          float derivative_z = apply_filter(neighbors,kernel_z_);

          float norm = std::sqrt(std::pow(derivative_x,2) + std::pow(derivative_y,2) + std::pow(derivative_z,2));
          if (norm < normal_threshold)
            continue;

          //normalize vector
          derivative_x /= norm;
          derivative_y /= norm;
          derivative_z /= norm;

          //save the normals in data base
          normals_data[currIdx][0] = derivative_x;
          normals_data[currIdx][1] = derivative_y;
          normals_data[currIdx][2] = derivative_z;
          normals_data[currIdx][3] = norm;

          //save the points in data base
#ifdef DEBUG
          std::cout << std::endl
                   << "the data origin is " << data.local_origin_.x() << ' ' << data.local_origin_.y() << ' ' << data.local_origin_.z() << ' ' << std::endl
                   << "the sub dim are "    << data.sub_block_dim_.x()<< ' ' << data.sub_block_dim_.y() << ' ' << data.sub_block_dim_.z() << ' ' << std::endl
                   << "cell center is " << cellCenter.x() << ' ' << cellCenter.y() << ' ' << cellCenter.z() << ' ' << std::endl;
#endif
          points_data[currIdx][0] = float( cellCenter.x() * data.sub_block_dim_.x() + data.local_origin_.x() );
          points_data[currIdx][1] = float( cellCenter.y() * data.sub_block_dim_.y() + data.local_origin_.y() );
          points_data[currIdx][2] = float( cellCenter.z() * data.sub_block_dim_.z() + data.local_origin_.z() );
#ifdef PROB
          points_data[currIdx][3] = prob; //use the unused field in points to store prob
#else
          points_data[currIdx][3] = alpha_data[currIdx]; //use the unused field in points to store prob
#endif
        } //end leaf for
      } //end z for
    } //end y for
  } // end x for
}

std::vector<std::pair<vgl_point_3d<int>, vgl_point_3d<double> > >  boxm2_compute_derivative_function::neighbor_points(const vgl_point_3d<double>& cellCenter, double side_len, const boxm2_array_3d<uchar16>& trees, int nhood_size )
{
    std::vector<std::pair<vgl_point_3d<int>, vgl_point_3d<double> > > toReturn;

      for (int i = -(nhood_size-1)/2; i <= (nhood_size-1)/2; i++) {
          for (int j = -(nhood_size-1)/2; j <= (nhood_size-1)/2; j++) {
              for (int k = -(nhood_size-1)/2; k <= (nhood_size-1)/2; k++) {
                  //check if calculated neighbor is out of bounds
                  if ( cellCenter.x() + i*side_len < trees.get_row1_count() && cellCenter.y() + j*side_len < trees.get_row2_count() && cellCenter.z() + k*side_len < trees.get_row3_count()
                       && cellCenter.x() + i*side_len >= 0 && cellCenter.y() + j*side_len >= 0 && cellCenter.z() + k*side_len >= 0) {
                      std::pair<vgl_point_3d<int>, vgl_point_3d<double> > mypair(vgl_point_3d<int>(i, j, k),
                                                                                vgl_point_3d<double>(cellCenter.x() + i*side_len,
                                                                                                     cellCenter.y() + j*side_len,
                                                                                                     cellCenter.z() + k*side_len));
                      toReturn.push_back(mypair);
                  }
                  else
                      return toReturn; //immediately return
              }
          }
      }
      return toReturn;
}


std::vector<std::pair<vgl_point_3d<int>, float> >  boxm2_compute_derivative_function::eval_neighbors(
        boxm2_block_metadata data, const boct_bit_tree& bit_tree,
        const std::vector<std::pair<vgl_point_3d<int>, vgl_point_3d<double> > > & neighbors,  const boxm2_array_3d<uchar16>& trees,
        const boxm2_data_traits<BOXM2_ALPHA>::datatype* alpha_data, int curr_depth)
{
    std::vector<std::pair<vgl_point_3d<int>, float> > probs;

#ifdef PROB
    double side_len = 1.0 / (double) (1<<curr_depth);
#endif

    for (const auto & neighbor : neighbors)
    {
        //load neighbor block/tree
        vgl_point_3d<double> abCenter = neighbor.second;
        vgl_point_3d<int>    blkIdx((int) abCenter.x(),
                                    (int) abCenter.y(),
                                    (int) abCenter.z() );
        uchar16 ntree = trees(blkIdx.x(), blkIdx.y(), blkIdx.z());
        boct_bit_tree neighborTree( (unsigned char*) ntree.data_block(), data.max_level_);

        //traverse to local center
        vgl_point_3d<double> locCenter((double) abCenter.x() - blkIdx.x(),
                                       (double) abCenter.y() - blkIdx.y(),
                                       (double) abCenter.z() - blkIdx.z());
        int neighborBitIdx = neighborTree.traverse(locCenter, curr_depth);

        //if the cells are the same size, or the neighbor is larger
        if ( neighborTree.is_leaf(neighborBitIdx) ) {
            //get data index
            int idx = neighborTree.get_data_index(neighborBitIdx);

#ifdef PROB
            //grab alpha, calculate probability
            boxm2_data_traits<BOXM2_ALPHA>::datatype alpha = alpha_data[idx];
            float prob = 1.0f - (float)std::exp(-alpha * side_len * data.sub_block_dim_.x());

            std::pair<vgl_point_3d<int>, float> mypair(neighbor.first, prob);
            probs.push_back(mypair);
#else
            //grab alpha
            boxm2_data_traits<BOXM2_ALPHA>::datatype alpha = alpha_data[idx];
            std::pair<vgl_point_3d<int>, float> mypair(neighbors[i].first, alpha);
            probs.push_back(mypair);
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
#ifdef PROB
                int ndepth = bit_tree.depth_at( *leafIter );
                double nlen = 1.0 / (double) (1<<ndepth);
                totalAlphaL += (float)(alpha_data[dataIndex] * nlen * data.sub_block_dim_.x());
#else
                totalAlphaL += (float)(alpha_data[dataIndex]);
#endif
            }

#ifdef PROB
            float prob = 1.0f - std::exp( -totalAlphaL );
#else
            float prob = totalAlphaL;
#endif
            std::pair<vgl_point_3d<int>, float> mypair(neighbor.first, prob);
            probs.push_back(mypair);
        }
    }

    return probs;
}


std::vector<std::pair<vgl_point_3d<int>, float> > boxm2_compute_derivative_function::load_kernel(const std::string& filename)
{
    std::vector<std::pair<vgl_point_3d<int>, float> > filter;

    std::ifstream ifs(filename.c_str());

    vgl_point_3d<int> min_point, max_point;

    //set the dimension of the 3-d bounding box containing the kernels
    if (!ifs.eof()) {
        ifs >> min_point;
        ifs >> max_point;
    }
#ifdef DEBUG
    std::cout << " Max point: " << max_point << " Min point: " << min_point << std::endl;
#endif
    while (true)
    {
        vgl_point_3d<float> this_loc;
        float weight;
        ifs >> this_loc;
        ifs >> weight;
        if (ifs.eof())
            break;
        filter.emplace_back( vgl_point_3d<int>(int(this_loc.x()),int(this_loc.y()),int(this_loc.z())), weight);
#ifdef DEBUG
        std::cout << this_loc << "  weight: " << weight << std::endl;
#endif
    }
    return filter;
}


float boxm2_compute_derivative_function::apply_filter(std::vector<std::pair<vgl_point_3d<int>, float> > neighbors, std::vector<std::pair<vgl_point_3d<int>, float> > filter)
{
    float sum = 0;
    for (auto & i : filter) {
        vgl_point_3d<int> loc = i.first;
        std::vector<std::pair<vgl_point_3d<int>, float> >::const_iterator it = neighbors.begin();
        bool found = false;
        while (!found) {
            if ( (*it).first ==loc )
                found = true;
            ++it;
        }
        assert(found);
        //add data*filter to sum
        sum += i.second * (*it).second;
    }
    return sum;
}
