#include "boxm2_reg_mutual_info.h"
//:
// \file
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_intersection.h>
#include <boxm2/boxm2_block_metadata.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_stream_scene_cache.h>

#include <boct/boct_bit_tree.h>
#include <brip/brip_mutual_info.h>

//calc mutual information (2d histogram of two scenes)
double boxm2_reg_mutual_info(boxm2_cache_sptr& cacheA,
                             boxm2_cache_sptr& cacheB,
                             const vgl_vector_3d<int>& trans)
{
  boxm2_scene_sptr sceneA = cacheA->get_scene();
  boxm2_scene_sptr sceneB = cacheB->get_scene();

  //translate scene
  vcl_map<boxm2_block_id, boxm2_block_metadata>& blocksB = sceneB->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk;
  for (blk=blocksB.begin(); blk!=blocksB.end(); ++blk) {
    boxm2_block_metadata& data = blk->second;
    vgl_vector_3d<double> scaled( data.sub_block_dim_.x() * trans.x(),
                                  data.sub_block_dim_.y() * trans.y(),
                                  data.sub_block_dim_.z() * trans.z() );
    data.local_origin_ += scaled;
  }

  //intersection of bounding box is all we care about
  vgl_box_3d<double> boxA = sceneA->bounding_box();
  vgl_box_3d<double> boxB = sceneB->bounding_box();
  vcl_cout<<"Box A: "<<boxA<<vcl_endl
          <<"Box B: "<<boxB<<vcl_endl;
  vgl_box_3d<double> intersection = vgl_intersection(boxA, boxB);
  vcl_cout<<"Intersection area..."<<intersection<<vcl_endl;

  //create sample images
  vil_image_view<double> A, B;
  align_samples(cacheA, cacheB, intersection, A, B);

  //calc brip mutual info
  double MI = brip_mutual_info(A,B, 0.0, 1.0, 20);
  for (blk=blocksB.begin(); blk!=blocksB.end(); ++blk) {
    boxm2_block_metadata& data = blk->second;
    vgl_vector_3d<double> scaled( data.sub_block_dim_.x() * trans.x(),
                                  data.sub_block_dim_.y() * trans.y(),
                                  data.sub_block_dim_.z() * trans.z() );
    data.local_origin_ -= scaled;
  }
  return MI;
}

//: returns a vector
void align_samples(boxm2_cache_sptr& cacheA,
                   boxm2_cache_sptr& cacheB,
                   const vgl_box_3d<double>& roi,
                   vil_image_view<double>& A, vil_image_view<double>& B)
{
  boxm2_scene_sptr sceneA = cacheA->get_scene();
  boxm2_scene_sptr sceneB = cacheB->get_scene();

  //calc side len for scene A
  vcl_map<boxm2_block_id, boxm2_block_metadata>& blocks = sceneA->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter = blocks.begin();
  boxm2_block_metadata& data = blk_iter->second;
  vgl_vector_3d<double> sbDim = data.sub_block_dim_;

  //align counter to scene A
  vgl_point_3d<double> minPoint = roi.min_point() + sbDim / 2.0;
  vgl_point_3d<double> maxPoint = roi.max_point();

#if 0 // Warning: unused!
  //total points
  vgl_vector_3d<double> n = (maxPoint - minPoint);
  unsigned totalPoints = (int) ( n.x()/sbDim.x() + .5) *
                         (int) ( n.y()/sbDim.y() + .5) *
                         (int) ( n.z()/sbDim.z() + .5);
#endif
  vcl_vector<float> probsA, probsB;
  for (double x = minPoint.x(); x<maxPoint.x(); x+=sbDim.x()) {
    for (double y = minPoint.y(); y<maxPoint.y(); y+=sbDim.y()) {
      for (double z = minPoint.z(); z<maxPoint.z(); z+=sbDim.z()) {
        vgl_point_3d<double> pt(x,y,z);
        float probA=0.0f;
        float probB=0.0f;
        float iA=0.0f, iB=0.0f;
        bool good = boxm2_util::query_point(sceneA, cacheA, pt, probA, iA) &&
                    boxm2_util::query_point(sceneB, cacheB, pt, probB, iB);
        if (good) {
          probsA.push_back(probA);
          probsB.push_back(probB);
        }
      }
    }
  }

  A.set_size(1, probsA.size(), 1);
  B.set_size(1, probsB.size(), 1);
  for (unsigned int i=0; i<probsA.size(); ++i)
    A(0,i) = probsA[i];
  for (unsigned int i=0; i<probsB.size(); ++i)
    B(0,i) = probsB[i];
}

#if 0
//: compute mutual information for a given transformation
float register_world(boxm2_cache_sptr& cacheA,
                     boxm2_cache_sptr& cacheB,
                     vgl_vector_3d<double> tx)
{
  boxm2_scene_sptr sceneA = cacheA->get_scene();
  boxm2_scene_sptr sceneB = cacheB->get_scene();
  //calc side len for scene A
  vcl_map<boxm2_block_id, boxm2_block_metadata>& blocks = sceneA->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  vil_image_view<double> A ;
  vil_image_view<double> B ;
  // iterate over i,j,k
  vcl_vector<float> probsA, probsB;

  for (blk_iter= blocks.begin(); blk_iter!=blocks.end(); blk_iter++)
  {
    boxm2_block  * blk = cacheA->get_block(blk_iter->first);
    boxm2_data_base *  alpha_base  = cacheA->get_data_base(blk_iter->first,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    boxm2_data<BOXM2_ALPHA> *alpha_data=new boxm2_data<BOXM2_ALPHA>(alpha_base->data_buffer(),alpha_base->buffer_length(),alpha_base->block_id());
    boxm2_data_base *  int_base  = cacheA->get_data_base(blk_iter->first,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
    boxm2_data<BOXM2_MOG3_GREY> *int_data=new boxm2_data<BOXM2_MOG3_GREY>(int_base->data_buffer(),int_base->buffer_length(),int_base->block_id());

    typedef vnl_vector_fixed<unsigned char, 16> uchar16;
    boxm2_array_3d<uchar16>&  trees = blk->trees();  //trees to refine
    unsigned ni = trees.get_row1_count();
    unsigned nj = trees.get_row2_count();
    unsigned nk = trees.get_row3_count();

    vgl_point_3d<double> global_center = blk_iter->second.local_origin_;
    vgl_vector_3d<double> sub_blk_dims = blk_iter->second.sub_block_dim_ ;
    for (unsigned i = 0; i < ni; i++)
    {
      for (unsigned j = 0; j < nj; j++)
      {
        for (unsigned k = 0; k < nk; k++)
        {
          uchar16 tree  = trees(i,j,k);
          boct_bit_tree curr_tree( (unsigned char*) tree.data_block(), 4);
          for (int ti=0; ti<585; ti++)
          {
            //if current bit is 0 and parent bit is 1, you're at a leaf
            int pi = (ti-1)>>3;                                 //Bit_index of parent bit
            bool validParent = curr_tree.bit_at(pi) || (ti==0); // special case for root
            if (validParent )
            {
              int depth = curr_tree.depth_at(ti);
              float side_len = 1.0f/(float) (1<<depth);
              int index = curr_tree.get_data_index(ti);
              vgl_point_3d<double> cc = curr_tree.cell_center(ti);
              global_center=blk_iter->second.local_origin_ + vgl_vector_3d<double>(i*sub_blk_dims.x(),
                                                                                   j*sub_blk_dims.y(),
                                                                                   k*sub_blk_dims.z());
              vgl_point_3d<double> pt = global_center + vgl_vector_3d<double>(cc.x()*sub_blk_dims.x(),
                                                                              cc.y()*sub_blk_dims.y(),
                                                                              cc.z()*sub_blk_dims.z());
              vgl_point_3d<double> ptxformed = pt+vgl_vector_3d<double>(tx.x()*sub_blk_dims.x(),
                                                                        tx.y()*sub_blk_dims.y(),
                                                                        tx.z()*sub_blk_dims.z());
              float probA=alpha_data->data()[index];
              probA = 1  - vcl_exp (-probA* side_len *sub_blk_dims.x());
              float probB=0.0f;
              float iA=(float)int_data->data()[index][0]/255.0f; // WARNING: unused!
              float iB=0.0f;
              bool good = boxm2_util::query_point(sceneB, cacheB, ptxformed, probB, iB);
              if (good)
              {
                probsA.push_back(probA);
                probsB.push_back(probB);
              }
            }
          }
        }
      }
    }
  }
  A.set_size(1, probsA.size(), 1);
  B.set_size(1, probsB.size(), 1);
  for (int i=0; i<probsA.size(); ++i)
      A(0,i) = probsA[i];
  for (int i=0; i<probsB.size(); ++i)
      B(0,i) = probsB[i];
  double MI = brip_mutual_info(A,B, 0.0, 1.0, 20);
  return MI;
}
#endif // 0

float register_world(boxm2_cache_sptr& cacheA,
                     boxm2_scene_sptr& sceneB,
                     vgl_vector_3d<double> tx)
{
  boxm2_scene_sptr sceneA = cacheA->get_scene();

  vcl_vector<vcl_string> data_types,identifiers;
  data_types.push_back("alpha");
  identifiers.push_back("");

  boxm2_stream_scene_cache blksB( sceneB, data_types,identifiers);
  vgl_box_3d<int> bbox = sceneB->bounding_box_blk_ids();

  //calc side len for scene A
  vcl_map<boxm2_block_id, boxm2_block_metadata>& blocks = sceneA->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  vil_image_view<double> A ;
  vil_image_view<double> B ;
  // iterate over i,j,k
  vcl_vector<float> probsA, probsB;

  for (blk_iter= blocks.begin(); blk_iter!=blocks.end(); blk_iter++)
  {
    boxm2_block  * blk = cacheA->get_block(blk_iter->first);
    boxm2_data_base *  alpha_base  = cacheA->get_data_base(blk_iter->first,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    boxm2_data<BOXM2_ALPHA> *alpha_data=new boxm2_data<BOXM2_ALPHA>(alpha_base->data_buffer(),alpha_base->buffer_length(),alpha_base->block_id());

    typedef vnl_vector_fixed<unsigned char, 16> uchar16;
    boxm2_array_3d<uchar16>&  trees = blk->trees();  //trees to refine
    unsigned ni = trees.get_row1_count();
    unsigned nj = trees.get_row2_count();
    unsigned nk = trees.get_row3_count();

    vgl_point_3d<double> global_center = blk_iter->second.local_origin_;
    vgl_vector_3d<double> sub_blk_dims = blk_iter->second.sub_block_dim_ ;
    for (unsigned i = 0; i < ni; i++)
    {
      for (unsigned j = 0; j < nj; j++)
      {
        for (unsigned k = 0; k < nk; k++)
        {
          uchar16 tree  = trees(i,j,k);
          boct_bit_tree curr_tree( (unsigned char*) tree.data_block(), 4);
          for (int ti=0; ti<585; ti++)
          {
            //if current bit is 0 and parent bit is 1, you're at a leaf
            int pi = (ti-1)>>3;                                 //Bit_index of parent bit
            bool validParent = curr_tree.bit_at(pi) || (ti==0); // special case for root
            if (validParent )
            {
              int depth = curr_tree.depth_at(ti);
              float side_len = 1.0f/(float) (1<<depth);
              int index = curr_tree.get_data_index(ti);
              vgl_point_3d<double> cc = curr_tree.cell_center(ti);
              global_center=blk_iter->second.local_origin_ + vgl_vector_3d<double>(i*sub_blk_dims.x(),
                                                                                   j*sub_blk_dims.y(),
                                                                                   k*sub_blk_dims.z());
              vgl_point_3d<double> pt = global_center + vgl_vector_3d<double>(cc.x()*sub_blk_dims.x(),
                                                                              cc.y()*sub_blk_dims.y(),
                                                                              cc.z()*sub_blk_dims.z());
              vgl_point_3d<double> ptxformed = pt+vgl_vector_3d<double>(tx.x()*sub_blk_dims.x(),
                                                                        tx.y()*sub_blk_dims.y(),
                                                                        tx.z()*sub_blk_dims.z());
              float probA=alpha_data->data()[index];
              probA = 1.0f  - (float)vcl_exp(-probA* side_len *sub_blk_dims.x());

              float probB=0.0f;
              bool good = true;
              vgl_point_3d<double> local;
              boxm2_block_id id;
              if (!sceneB->contains(ptxformed, id, local))
              {
                good= false;
              }
              else
              {
                boxm2_block_metadata mdata = sceneB->get_block_metadata_const(id);
                unsigned int blk_index = (id.i()-bbox.min_x())*(bbox.depth()+1)*(bbox.height()+1)+
                                         (id.j()-bbox.min_y())*(bbox.depth()+1)+
                                         (id.k()-bbox.min_z());
                int index_x=(int)vcl_floor(local.x());
                int index_y=(int)vcl_floor(local.y());
                int index_z=(int)vcl_floor(local.z());

                vgl_vector_3d<unsigned> blocks_dim  = mdata.sub_block_num_;

                unsigned int sub_blk_index = index_x*blocks_dim.y()*blocks_dim.z()+
                                             index_y*blocks_dim.z()+
                                             index_z;
                unsigned long tree_index = blksB.blk_offsets_[blk_index] + sub_blk_index;
                boct_bit_tree tree(blksB.blk_buffer_[tree_index].data_block(),mdata.max_level_);
                int bit_index=tree.traverse(local);
                int depth=tree.depth_at(bit_index);
                int data_offset=tree.get_data_index(bit_index,false);
                unsigned int alpha_index = blksB.offsets_["alpha"][blk_index]/4 + data_offset;
                float alpha = ((float*)(blksB.data_buffers_["alpha"]))[alpha_index];
                float side_len=static_cast<float>(mdata.sub_block_dim_.x()/((float)(1<<depth)));
                probB=1.0f-vcl_exp(-alpha*side_len);
              }
              if (good)
              {
                probsA.push_back(probA);
                probsB.push_back(probB);
              }
            }
          }
        }
      }
    }
  }
  A.set_size(1, probsA.size(), 1);
  B.set_size(1, probsB.size(), 1);
  for (unsigned int i=0; i<probsA.size(); ++i)
    A(0,i) = probsA[i];
  for (unsigned int i=0; i<probsB.size(); ++i)
    B(0,i) = probsB[i];
  return (float)brip_mutual_info(A,B, 0.0, 1.0, 20);
}

