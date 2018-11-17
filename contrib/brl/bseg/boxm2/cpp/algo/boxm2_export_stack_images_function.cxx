#include "boxm2_export_stack_images_function.h"
//
#include <boct/boct_bit_tree.h>

#include "boxm2_mog3_grey_processor.h"
#include "boxm2_gauss_rgb_processor.h"
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data.h>

#include "boxm2/boxm2_util.h"
#include <vil/vil_save.h>
#include <vil/vil_load.h>

void boxm2_export_stack_images_function::export_opacity_stack_images(const boxm2_scene_sptr& scene, boxm2_cache_sptr & cache, const std::string& outdir)
{
  auto& nc_scene = const_cast<boxm2_scene_sptr&>(scene);
  vgl_point_3d<int> min_index;
  vgl_point_3d<int> max_index;

  vgl_point_3d<double> min_local_origin;
  vgl_point_3d<double> max_local_origin;

  scene->min_block_index(min_index, min_local_origin);
  scene->max_block_index(max_index, max_local_origin);

  boxm2_block_id id = *(scene->get_block_ids().begin());
  boxm2_block_metadata blk_mdata = scene->get_block_metadata_const(id);

  int ntrees_x = blk_mdata.sub_block_num_.x();
  int ntrees_y = blk_mdata.sub_block_num_.y();
  int ntrees_z = blk_mdata.sub_block_num_.z();

  int maxcells = 1 << (blk_mdata.max_level_ - 1);

  int img_x = (max_index.x() - min_index.x() + 1)*ntrees_x*maxcells;
  int img_y = (max_index.y() - min_index.y() + 1)*ntrees_y*maxcells;
  int img_z = (max_index.z() - min_index.z() + 1)*ntrees_z*maxcells;

  vil_image_view<unsigned char> img(img_x, img_y);
  for (int k = 0; k < img_z; ++k)
  {
    std::stringstream ss;
    ss << outdir << "/surf_" << std::setw(5) << std::setfill('0') << k << ".png";
    std::cout << "Filename : " << ss.str() << std::endl;
    vil_save(img, ss.str().c_str());
  }
  std::map<boxm2_block_id, boxm2_block_metadata>& blocks = scene->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;

  int index_x = 0;
  int index_y = 0;
  int index_z = 0;

  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    std::cout << "Block id " << blk_iter->first << std::endl;
    boxm2_block  * blk = cache->get_block(nc_scene, blk_iter->first);
    boxm2_data_base *  alpha_base = cache->get_data_base(nc_scene, blk_iter->first, boxm2_data_traits<BOXM2_ALPHA>::prefix());
    boxm2_data<BOXM2_ALPHA> *alpha_data = new boxm2_data<BOXM2_ALPHA>(alpha_base->data_buffer(), alpha_base->buffer_length(), alpha_base->block_id());

    typedef vnl_vector_fixed<unsigned char, 16> uchar16;
    const boxm2_array_3d<uchar16>&  trees = blk->trees();  //trees to refine
    unsigned ni = trees.get_row1_count();
    unsigned nj = trees.get_row2_count();
    unsigned nk = trees.get_row3_count();

    vgl_vector_3d<double> sub_blk_dims = blk_iter->second.sub_block_dim_;
    index_z = (blk_iter->first.k() - min_index.z()) * ntrees_z;
    for (unsigned k = 0; k < nk; ++k, ++index_z)
      for (int sk = 0; sk < maxcells; ++sk)
      {
        std::stringstream ss;
        ss << outdir << "/img_" << std::setw(5) << std::setfill('0') << k*maxcells + sk << ".png";
        //std::cout<<"Filename : "<<ss.str()<<std::endl;
        vil_image_view_base_sptr img_sptr = vil_load(ss.str().c_str());
        if (auto * cimg = dynamic_cast<vil_image_view<unsigned char> *>(img_sptr.ptr()))
        {
          index_x = (blk_iter->first.i() - min_index.x()) * ntrees_x;
          for (unsigned i = 0; i < ni; ++i, ++index_x)
          {
            index_y = (blk_iter->first.j() - min_index.y()) * ntrees_y;
            for (unsigned j = 0; j < nj; ++j, ++index_y)
            {
              uchar16 tree = trees(i, j, k);
              boct_bit_tree curr_tree((unsigned char*)tree.data_block(), 4);
              for (int ti = 0; ti < 585; ++ti)
              {
                //if current bit is 0 and parent bit is 1, you're at a leaf
                int pi = (ti - 1) >> 3;                                 //Bit_index of parent bit
                bool validParent = curr_tree.bit_at(pi) || (ti == 0); // special case for root
                if (validParent)
                {
                  int depth = curr_tree.depth_at(ti);
                  float side_len = 1.0f / (float)(1 << depth);
                  int index = curr_tree.get_data_index(ti);
                  vgl_point_3d<double> cc = curr_tree.cell_box(ti).min_point();
                  float prob = alpha_data->data()[index];
                  prob = 1.0f - (float)std::exp(-prob* side_len *sub_blk_dims.x());
                  int factor = 1 << (blk_iter->second.max_level_ - depth - 1);

                  for (int subi = 0; subi < factor; ++subi)
                    for (int subj = 0; subj < factor; ++subj)
                      for (int subk = 0; subk < factor; ++subk)
                      {
                        int sub_index_z = int(cc.z()*maxcells) + subk;
                        if (sub_index_z == sk)
                        {
                          (*cimg)(index_x*maxcells + int(cc.x()*maxcells) + subi,
                            index_y*maxcells + int(cc.y()*maxcells)+subj)
                            = (unsigned char)(prob * 255.99f);
                        }
                      }
                }
              }
            }
          }

          std::stringstream ss;
          ss << outdir << "/img_" << std::setw(5) << std::setfill('0') <<  k*maxcells + sk << ".png";
          //std::cout<<"Filename : "<<ss.str()<<std::endl;
          vil_save((*cimg), ss.str().c_str());
        }
      }
  }
  std::cout << "Scene varies from " << min_index << " to " << max_index << '\n'
            << "Volume Dimensions are " << img_x << ' ' << img_y << ' ' << img_z << std::endl;
}

void boxm2_export_stack_images_function  ::export_greyscale_stack_images(const boxm2_scene_sptr& scene, boxm2_cache_sptr & cache, vil3d_image_view<unsigned char> & img3d)
{
  // we need to const_cast to call the new cache functions which take a (non const) scene as 1st arg.
  // perhaps const versions of get_block and friends could be added
  auto& nc_scene = const_cast<boxm2_scene_sptr&>(scene);
  vgl_point_3d<int> min_index;
  vgl_point_3d<int> max_index;

  vgl_point_3d<double> min_local_origin;
  vgl_point_3d<double> max_local_origin;

  scene->min_block_index(min_index,min_local_origin);
  scene->max_block_index(max_index,max_local_origin);

  boxm2_block_id id = *(scene->get_block_ids().begin());
  boxm2_block_metadata blk_mdata = scene->get_block_metadata_const(id);

  int ntrees_x = blk_mdata.sub_block_num_.x() ;
  int ntrees_y = blk_mdata.sub_block_num_.y() ;
  int ntrees_z = blk_mdata.sub_block_num_.z() ;

  int maxcells = 1 << (blk_mdata.max_level_-1) ;

  int img_x = (max_index.x()-min_index.x()+1)*ntrees_x*maxcells;
  int img_y = (max_index.y()-min_index.y()+1)*ntrees_y*maxcells;
  int img_z = (max_index.z()-min_index.z()+1)*ntrees_z*maxcells;

  img3d.set_size(img_x,img_y,img_z,4);
#if 0
  double side_len = blk_mdata.sub_block_dim_.x() / (1 << blk_mdata.max_level_);
#endif
  std::map<boxm2_block_id, boxm2_block_metadata>& blocks = scene->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;

  int index_x = 0;
  int index_y = 0;
  int index_z = 0;

  for (blk_iter= blocks.begin(); blk_iter!=blocks.end(); ++blk_iter)
  {
    boxm2_block  * blk = cache->get_block(nc_scene,blk_iter->first);
    boxm2_data_base *  alpha_base  = cache->get_data_base(nc_scene, blk_iter->first,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    boxm2_data<BOXM2_ALPHA> *alpha_data=new boxm2_data<BOXM2_ALPHA>(alpha_base->data_buffer(),alpha_base->buffer_length(),alpha_base->block_id());

    boxm2_data_base *  int_base  = cache->get_data_base(nc_scene, blk_iter->first,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
    boxm2_data<BOXM2_MOG3_GREY> *int_data=new boxm2_data<BOXM2_MOG3_GREY>(int_base->data_buffer(),int_base->buffer_length(),int_base->block_id());

    typedef vnl_vector_fixed<unsigned char, 16> uchar16;
    const boxm2_array_3d<uchar16>&  trees = blk->trees();  //trees
    unsigned ni = trees.get_row1_count();
    unsigned nj = trees.get_row2_count();
    unsigned nk = trees.get_row3_count();

    vgl_vector_3d<double> sub_blk_dims = blk_iter->second.sub_block_dim_ ;
    index_x = (blk_iter->first.i() - min_index.x()) * ntrees_x;

    for (unsigned i = 0; i < ni; ++i,++index_x)
    {
      index_y = (blk_iter->first.j() - min_index.y()) * ntrees_y;
      for (unsigned j = 0; j < nj; ++j,++index_y)
      {
        index_z = (blk_iter->first.k() - min_index.z()) * ntrees_z;
        for (unsigned k = 0; k < nk; ++k,++index_z)
        {
          uchar16 tree  = trees(i,j,k);
          boct_bit_tree curr_tree( (unsigned char*) tree.data_block(), 4);
          for (int ti=0; ti<585; ++ti)
          {
            //if current bit is 0 and parent bit is 1, you're at a leaf
            int pi = (ti-1)>>3;                                 //Bit_index of parent bit
            bool validParent = curr_tree.bit_at(pi) || (ti==0); // special case for root
            if (validParent )
            {
              int depth = curr_tree.depth_at(ti);
              float side_len = 1.0f/(float) (1<<depth);
              int index = curr_tree.get_data_index(ti);
              vgl_point_3d<double> cc = curr_tree.cell_box(ti).min_point();
              float prob =alpha_data->data()[index];
              prob =  1.0f - (float)std::exp(-prob* side_len *sub_blk_dims.x());
              int factor = 1<<(blk_iter->second.max_level_-depth-1);
              auto intensity = (unsigned char)(boxm2_mog3_grey_processor::expected_color(int_data->data()[index]) * 255);

              for (int subi = 0; subi < factor; ++subi)
                for (int subj = 0; subj < factor; ++subj)
                  for (int subk = 0; subk < factor; ++subk)
                  {
                    img3d(index_x*maxcells+int(cc.x()*maxcells)+subi,
                          index_y*maxcells+int(cc.y()*maxcells)+subj,
                          index_z*maxcells+int(cc.z()*maxcells)+subk,3) = (unsigned char)(prob * 255.99f);
                    img3d(index_x*maxcells+int(cc.x()*maxcells)+subi,
                          index_y*maxcells+int(cc.y()*maxcells)+subj,
                          index_z*maxcells+int(cc.z()*maxcells)+subk,0) =intensity;
                    img3d(index_x*maxcells+int(cc.x()*maxcells)+subi,
                          index_y*maxcells+int(cc.y()*maxcells)+subj,
                          index_z*maxcells+int(cc.z()*maxcells)+subk,1) =intensity;
                    img3d(index_x*maxcells+int(cc.x()*maxcells)+subi,
                          index_y*maxcells+int(cc.y()*maxcells)+subj,
                          index_z*maxcells+int(cc.z()*maxcells)+subk,2) =intensity;
                  }
            }
          }
        }
      }
    }
  }
  std::cout<<"Scene varies from "<<min_index<<" to "<<max_index<<'\n'
          <<"Volume Dimensions are "<<img_x<<' '<<img_y<<' '<<img_z<<std::endl;
}


void boxm2_export_stack_images_function::export_color_stack_images(const boxm2_scene_sptr& scene, boxm2_cache_sptr & cache, vil3d_image_view<unsigned char> & img3d)
{
  // we need to const_cast to call the new cache functions which take a (non const) scene as 1st arg.
  // perhaps const versions of get_block and friends could be added
  auto& nc_scene = const_cast<boxm2_scene_sptr&>(scene);

  vgl_point_3d<int> min_index;
  vgl_point_3d<int> max_index;

  vgl_point_3d<double> min_local_origin;
  vgl_point_3d<double> max_local_origin;

  scene->min_block_index(min_index,min_local_origin);
  scene->max_block_index(max_index,max_local_origin);

  boxm2_block_id id = *(scene->get_block_ids().begin());
  boxm2_block_metadata blk_mdata = scene->get_block_metadata_const(id);

  int ntrees_x = blk_mdata.sub_block_num_.x() ;
  int ntrees_y = blk_mdata.sub_block_num_.y() ;
  int ntrees_z = blk_mdata.sub_block_num_.z() ;

  int maxcells = 1 << (blk_mdata.max_level_-1) ;

  int img_x = (max_index.x()-min_index.x()+1)*ntrees_x*maxcells;
  int img_y = (max_index.y()-min_index.y()+1)*ntrees_y*maxcells;
  int img_z = (max_index.z()-min_index.z()+1)*ntrees_z*maxcells;

  img3d.set_size(img_x,img_y,img_z,4);
#if 0
  double side_len = blk_mdata.sub_block_dim_.x() / (1 << blk_mdata.max_level_);
#endif
  std::map<boxm2_block_id, boxm2_block_metadata>& blocks = scene->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;

  int index_x = 0;
  int index_y = 0;
  int index_z = 0;

  for (blk_iter= blocks.begin(); blk_iter!=blocks.end(); ++blk_iter)
  {
    boxm2_block  * blk = cache->get_block(nc_scene, blk_iter->first);
    boxm2_data_base *  alpha_base  = cache->get_data_base(nc_scene, blk_iter->first,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    boxm2_data<BOXM2_ALPHA> *alpha_data=new boxm2_data<BOXM2_ALPHA>(alpha_base->data_buffer(),alpha_base->buffer_length(),alpha_base->block_id());
    boxm2_data_base *  int_base  = cache->get_data_base(nc_scene, blk_iter->first,boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix());
    boxm2_data<BOXM2_GAUSS_RGB> *int_data=new boxm2_data<BOXM2_GAUSS_RGB>(int_base->data_buffer(),int_base->buffer_length(),int_base->block_id());

    typedef vnl_vector_fixed<unsigned char, 16> uchar16;
    const boxm2_array_3d<uchar16>&  trees = blk->trees();  //trees to refine
    unsigned ni = trees.get_row1_count();
    unsigned nj = trees.get_row2_count();
    unsigned nk = trees.get_row3_count();

    vgl_vector_3d<double> sub_blk_dims = blk_iter->second.sub_block_dim_ ;
    index_x = (blk_iter->first.i() - min_index.x()) * ntrees_x;

    for (unsigned i = 0; i < ni; ++i,++index_x)
    {
      index_y = (blk_iter->first.j() - min_index.y()) * ntrees_y;

      for (unsigned j = 0; j < nj; ++j,++index_y)
      {
        index_z = (blk_iter->first.k() - min_index.z()) * ntrees_z;
        for (unsigned k = 0; k < nk; ++k,++index_z)
        {
          uchar16 tree  = trees(i,j,k);
          boct_bit_tree curr_tree( (unsigned char*) tree.data_block(), 4);
          for (int ti=0; ti<585; ++ti)
          {
            //if current bit is 0 and parent bit is 1, you're at a leaf
            int pi = (ti-1)>>3;                                 //Bit_index of parent bit
            bool validParent = curr_tree.bit_at(pi) || (ti==0); // special case for root
            if (validParent )
            {
              int depth = curr_tree.depth_at(ti);
              float side_len = 1.0f/(float) (1<<depth);
              int index = curr_tree.get_data_index(ti);
              vgl_point_3d<double> cc = curr_tree.cell_box(ti).min_point();
              float prob =alpha_data->data()[index];
              prob =  1.0f - (float)std::exp(-prob* side_len *sub_blk_dims.x());
              int factor = 1<<(blk_iter->second.max_level_-depth-1);

              vnl_vector_fixed<float,3> color = boxm2_gauss_rgb_processor::expected_color(int_data->data()[index]);
              auto r = (unsigned char)(color[0]*255);
              auto g = (unsigned char)(color[1]*255);
              auto b = (unsigned char)(color[2]*255);

              for (int subi = 0; subi < factor; ++subi)
                for (int subj = 0; subj < factor; ++subj)
                  for (int subk = 0; subk < factor; ++subk)
                  {
                    img3d(index_x*maxcells+int(cc.x()*maxcells)+subi,
                          index_y*maxcells+int(cc.y()*maxcells)+subj,
                          index_z*maxcells+int(cc.z()*maxcells)+subk,3) = (unsigned char)(prob * 255.99f);
                    img3d(index_x*maxcells+int(cc.x()*maxcells)+subi,
                          index_y*maxcells+int(cc.y()*maxcells)+subj,
                          index_z*maxcells+int(cc.z()*maxcells)+subk,0) =r;
                    img3d(index_x*maxcells+int(cc.x()*maxcells)+subi,
                          index_y*maxcells+int(cc.y()*maxcells)+subj,
                          index_z*maxcells+int(cc.z()*maxcells)+subk,1) =g;
                    img3d(index_x*maxcells+int(cc.x()*maxcells)+subi,
                          index_y*maxcells+int(cc.y()*maxcells)+subj,
                          index_z*maxcells+int(cc.z()*maxcells)+subk,2) =b;
                  }
            }
          }
        }
      }
    }
  }
  std::cout<<"Scene varies from "<<min_index<<" to "<<max_index<<'\n'
          <<"Volume Dimensions are "<<img_x<<' '<<img_y<<' '<<img_z<<std::endl;
}


void boxm2_export_stack_images_function::export_float_images(const boxm2_scene_sptr& scene, boxm2_cache_sptr & cache, const std::string& ident, vil3d_image_view<float> & img3d)
{
    // we need to const_cast to call the new cache functions which take a (non const) scene as 1st arg.
    // perhaps const versions of get_block and friends could be added
    auto& nc_scene = const_cast<boxm2_scene_sptr&>(scene);
    vgl_point_3d<int> min_index, max_index;
    vgl_point_3d<double> min_local_origin, max_local_origin;

    scene->min_block_index(min_index, min_local_origin);
    scene->max_block_index(max_index, max_local_origin);

    boxm2_block_id id = *(scene->get_block_ids().begin());
    boxm2_block_metadata blk_mdata = scene->get_block_metadata_const(id);

    int ntrees_x = blk_mdata.sub_block_num_.x();
    int ntrees_y = blk_mdata.sub_block_num_.y();
    int ntrees_z = blk_mdata.sub_block_num_.z();

    int maxcells = 1 << (blk_mdata.max_level_ - 1);



    int img_x = (max_index.x() - min_index.x() + 1)*ntrees_x*maxcells;
    int img_y = (max_index.y() - min_index.y() + 1)*ntrees_y*maxcells;
    int img_z = (max_index.z() - min_index.z() + 1)*ntrees_z*maxcells;
    img3d.set_size(img_x, img_y, img_z);

    std::map<boxm2_block_id, boxm2_block_metadata>& blocks = scene->blocks();
    std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;

    int index_x = 0, index_y = 0, index_z = 0;

    for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
    {
        boxm2_block  * blk = cache->get_block(nc_scene, blk_iter->first);
        boxm2_data_type dtype = boxm2_data_info::data_type(ident);
        boxm2_data_base *  float_base = cache->get_data_base(nc_scene, blk_iter->first, ident);
        auto * data = (float*)float_base->data_buffer();

        typedef vnl_vector_fixed<unsigned char, 16> uchar16;
        const boxm2_array_3d<uchar16>&  trees = blk->trees();  //trees
        unsigned ni = trees.get_row1_count();
        unsigned nj = trees.get_row2_count();
        unsigned nk = trees.get_row3_count();

        vgl_vector_3d<double> sub_blk_dims = blk_iter->second.sub_block_dim_;
        index_x = (blk_iter->first.i() - min_index.x()) * ntrees_x;

        for (unsigned i = 0; i < ni; ++i, ++index_x)
        {
            index_y = (blk_iter->first.j() - min_index.y()) * ntrees_y;
            for (unsigned j = 0; j < nj; ++j, ++index_y)
            {
                index_z = (blk_iter->first.k() - min_index.z()) * ntrees_z;
                for (unsigned k = 0; k < nk; ++k, ++index_z)
                {
                    uchar16 tree = trees(i, j, k);
                    boct_bit_tree curr_tree((unsigned char*)tree.data_block(), 4);
                    for (int ti = 0; ti < 585; ++ti)
                    {
                        //if current bit is 0 and parent bit is 1, you're at a leaf
                        int pi = (ti - 1) >> 3;                                 //Bit_index of parent bit
                        bool validParent = curr_tree.bit_at(pi) || (ti == 0); // special case for root
                        if (validParent)
                        {
                            int depth = curr_tree.depth_at(ti);
                            float side_len = 1.0f / (float)(1 << depth);
                            int index = curr_tree.get_data_index(ti);
                            vgl_point_3d<double> cc = curr_tree.cell_box(ti).min_point();
                            int factor = 1 << (blk_iter->second.max_level_ - depth - 1);
                            float intensity = data[index] ;

                            for (int subi = 0; subi < factor; ++subi)
                                for (int subj = 0; subj < factor; ++subj)
                                    for (int subk = 0; subk < factor; ++subk)
                                    {
                                        img3d(index_x*maxcells + int(cc.x()*maxcells) + subi,
                                            index_y*maxcells + int(cc.y()*maxcells) + subj,
                                            index_z*maxcells + int(cc.z()*maxcells) + subk) = intensity;

                                    }
                        }
                    }
                }
            }
        }
    }
    std::cout << "Scene varies from " << min_index << " to " << max_index << '\n'
        << "Volume Dimensions are " << img_x << ' ' << img_y << ' ' << img_z << std::endl;
}
