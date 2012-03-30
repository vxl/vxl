#include <boct/boct_bit_tree.h>


#include "boxm2_mog3_grey_processor.h"
#include "boxm2_gauss_rgb_processor.h"
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data.h>

#include "boxm2/boxm2_util.h"

#include "boxm2_export_stack_images_function.h"

void boxm2_export_stack_images_function	::export_greyscale_stack_images(const boxm2_scene_sptr& scene, boxm2_cache_sptr & cache, vil3d_image_view<unsigned char> & img3d)
{

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

	int maxcells = (int)vcl_pow((float)2,(float)blk_mdata.max_level_-1) ;

	int img_x = (max_index.x()-min_index.x()+1)*ntrees_x*maxcells;
	int img_y = (max_index.y()-min_index.y()+1)*ntrees_y*maxcells;
	int img_z = (max_index.z()-min_index.z()+1)*ntrees_z*maxcells;

	img3d.set_size(img_x,img_y,img_z,4);


	double side_len = blk_mdata.sub_block_dim_.x() / ((int)vcl_pow((float)2,(float)blk_mdata.max_level_));

	vcl_map<boxm2_block_id, boxm2_block_metadata>& blocks = scene->blocks();
	vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;

	int index_x = 0;
	int index_y = 0;
	int index_z = 0;

	for (blk_iter= blocks.begin(); blk_iter!=blocks.end(); blk_iter++)
	{
		boxm2_block  * blk = cache->get_block(blk_iter->first);
		boxm2_data_base *  alpha_base  = cache->get_data_base(blk_iter->first,boxm2_data_traits<BOXM2_ALPHA>::prefix());
		boxm2_data<BOXM2_ALPHA> *alpha_data=new boxm2_data<BOXM2_ALPHA>(alpha_base->data_buffer(),alpha_base->buffer_length(),alpha_base->block_id());

		boxm2_data_base *  int_base  = cache->get_data_base(blk_iter->first,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
		boxm2_data<BOXM2_MOG3_GREY> *int_data=new boxm2_data<BOXM2_MOG3_GREY>(int_base->data_buffer(),int_base->buffer_length(),int_base->block_id());

		typedef vnl_vector_fixed<unsigned char, 16> uchar16;
		boxm2_array_3d<uchar16>&  trees = blk->trees();  //trees to refine
		boxm2_array_3d<uchar16>::iterator tree_iter;
		unsigned ni = trees.get_row1_count();
		unsigned nj = trees.get_row2_count();
		unsigned nk = trees.get_row3_count();

		vgl_vector_3d<double> sub_blk_dims = blk_iter->second.sub_block_dim_ ;
		index_x = (blk_iter->first.i() - min_index.x()) * ntrees_x;

		for (unsigned i = 0; i < ni; i++,index_x++)
		{
			index_y = (blk_iter->first.j() - min_index.y()) * ntrees_y;
			for (unsigned j = 0; j < nj; j++,index_y++)
			{
				index_z = (blk_iter->first.k() - min_index.z()) * ntrees_z;
				for (unsigned k = 0; k < nk; k++,index_z++)
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
							vgl_point_3d<double> cc = curr_tree.cell_box(ti).min_point();
							float prob =alpha_data->data()[index];
							prob =  1- vcl_exp (-prob* side_len *sub_blk_dims.x());
							int factor = vcl_pow((float)2,(float)blk_iter->second.max_level_-depth-1);
							unsigned char intensity  = (unsigned char) vcl_floor( boxm2_mog3_grey_processor::expected_color(int_data->data()[index]) *255.0f);

							for(unsigned subi = 0 ; subi < factor; subi++)
								for(unsigned subj = 0 ; subj < factor; subj++)
									for(unsigned subk = 0 ; subk < factor; subk++)
									{
										img3d(index_x*maxcells+(int)vcl_floor(cc.x()*maxcells)+subi,
											index_y*maxcells+(int)vcl_floor(cc.y()*maxcells)+subj,
											index_z*maxcells+(int)vcl_floor(cc.z()*maxcells)+subk,3) = (unsigned char) vcl_floor(prob * 255.0f);
										img3d(index_x*maxcells+(int)vcl_floor(cc.x()*maxcells)+subi,
											index_y*maxcells+(int)vcl_floor(cc.y()*maxcells)+subj,
											index_z*maxcells+(int)vcl_floor(cc.z()*maxcells)+subk,0) =intensity;
										img3d(index_x*maxcells+(int)vcl_floor(cc.x()*maxcells)+subi,
											index_y*maxcells+(int)vcl_floor(cc.y()*maxcells)+subj,
											index_z*maxcells+(int)vcl_floor(cc.z()*maxcells)+subk,1) =intensity;
										img3d(index_x*maxcells+(int)vcl_floor(cc.x()*maxcells)+subi,
											index_y*maxcells+(int)vcl_floor(cc.y()*maxcells)+subj,
											index_z*maxcells+(int)vcl_floor(cc.z()*maxcells)+subk,2) =intensity;
									}
						}
					}
				}
			}
		}
	}
		vcl_cout<<"Scene varies from "<<min_index<<" to "<<max_index<<vcl_endl;
	vcl_cout<<"Volume Dimensions are "<<img_x<<" "<<img_y<<" "<<img_z<<vcl_endl;
}



void boxm2_export_stack_images_function	::export_color_stack_images(const boxm2_scene_sptr& scene, boxm2_cache_sptr & cache, vil3d_image_view<unsigned char> & img3d)
{

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

	int maxcells = (int)vcl_pow((float)2,(float)blk_mdata.max_level_-1) ;

	int img_x = (max_index.x()-min_index.x()+1)*ntrees_x*maxcells;
	int img_y = (max_index.y()-min_index.y()+1)*ntrees_y*maxcells;
	int img_z = (max_index.z()-min_index.z()+1)*ntrees_z*maxcells;

	img3d.set_size(img_x,img_y,img_z,4);
	double side_len = blk_mdata.sub_block_dim_.x() / ((int)vcl_pow((float)2,(float)blk_mdata.max_level_));

	vcl_map<boxm2_block_id, boxm2_block_metadata>& blocks = scene->blocks();
	vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;

	int index_x = 0;
	int index_y = 0;
	int index_z = 0;

	for (blk_iter= blocks.begin(); blk_iter!=blocks.end(); blk_iter++)
	{
		boxm2_block  * blk = cache->get_block(blk_iter->first);
		boxm2_data_base *  alpha_base  = cache->get_data_base(blk_iter->first,boxm2_data_traits<BOXM2_ALPHA>::prefix());
		boxm2_data<BOXM2_ALPHA> *alpha_data=new boxm2_data<BOXM2_ALPHA>(alpha_base->data_buffer(),alpha_base->buffer_length(),alpha_base->block_id());
		boxm2_data_base *  int_base  = cache->get_data_base(blk_iter->first,boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix());
		boxm2_data<BOXM2_GAUSS_RGB> *int_data=new boxm2_data<BOXM2_GAUSS_RGB>(int_base->data_buffer(),int_base->buffer_length(),int_base->block_id());

		typedef vnl_vector_fixed<unsigned char, 16> uchar16;
		boxm2_array_3d<uchar16>&  trees = blk->trees();  //trees to refine
		boxm2_array_3d<uchar16>::iterator tree_iter;
		unsigned ni = trees.get_row1_count();
		unsigned nj = trees.get_row2_count();
		unsigned nk = trees.get_row3_count();

		vgl_vector_3d<double> sub_blk_dims = blk_iter->second.sub_block_dim_ ;
		index_x = (blk_iter->first.i() - min_index.x()) * ntrees_x;

		for (unsigned i = 0; i < ni; i++,index_x++)
		{
			index_y = (blk_iter->first.j() - min_index.y()) * ntrees_y;

			for (unsigned j = 0; j < nj; j++,index_y++)
			{
				index_z = (blk_iter->first.k() - min_index.z()) * ntrees_z;
				for (unsigned k = 0; k < nk; k++,index_z++)
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
							vgl_point_3d<double> cc = curr_tree.cell_box(ti).min_point();
							float prob =alpha_data->data()[index];
							prob =  1- vcl_exp (-prob* side_len *sub_blk_dims.x());
							int factor = vcl_pow((float)2,(float)blk_iter->second.max_level_-depth-1);

							unsigned char r,g,b ; 
							vnl_vector_fixed<float,3> color = boxm2_gauss_rgb_processor::expected_color(int_data->data()[index]);
							r = (unsigned char) vcl_floor(color[0]*255.0f) ;
							g = (unsigned char) vcl_floor(color[1]*255.0f) ;
							b = (unsigned char) vcl_floor(color[2]*255.0f) ;

							for(unsigned subi = 0 ; subi < factor; subi++)
							{
								for(unsigned subj = 0 ; subj < factor; subj++)
								{
									for(unsigned subk = 0 ; subk < factor; subk++)
									{
										img3d(index_x*maxcells+(int)vcl_floor(cc.x()*maxcells)+subi,
											index_y*maxcells+(int)vcl_floor(cc.y()*maxcells)+subj,
											index_z*maxcells+(int)vcl_floor(cc.z()*maxcells)+subk,3) = (unsigned char) vcl_floor(prob * 255.0f);
										img3d(index_x*maxcells+(int)vcl_floor(cc.x()*maxcells)+subi,
											index_y*maxcells+(int)vcl_floor(cc.y()*maxcells)+subj,
											index_z*maxcells+(int)vcl_floor(cc.z()*maxcells)+subk,0) =r;
										img3d(index_x*maxcells+(int)vcl_floor(cc.x()*maxcells)+subi,
											index_y*maxcells+(int)vcl_floor(cc.y()*maxcells)+subj,
											index_z*maxcells+(int)vcl_floor(cc.z()*maxcells)+subk,1) =g;
										img3d(index_x*maxcells+(int)vcl_floor(cc.x()*maxcells)+subi,
											index_y*maxcells+(int)vcl_floor(cc.y()*maxcells)+subj,
											index_z*maxcells+(int)vcl_floor(cc.z()*maxcells)+subk,2) =b;
									}
								}
							}
						}
					}
				}
			}
		}

	}
		vcl_cout<<"Scene varies from "<<min_index<<" to "<<max_index<<vcl_endl;
	vcl_cout<<"Volume Dimensions are "<<img_x<<" "<<img_y<<" "<<img_z<<vcl_endl;
}