#ifndef boxm_render_image_h_
#define boxm_render_image_h_

#include <vcl_vector.h>

#include <boct/boct_tree.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_sample.h>

#include <vpgl/vpgl_camera.h>

#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_transform.h>
#include <vil/vil_save.h>
#include <vil/vil_view_as.h>



template <class T_loc, boxm_apm_type APM>
void boxm_render_image_splatting(boxm_scene<boct_tree<T_loc, boxm_sample<APM>> > &scene, vpgl_camera_double_sptr cam, 
					   vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> &expected, 
					   vil_image_view<float> &mask, 
					   bool use_black_background = false)
{
	unsigned ni=expected.ni();
	unsigned nj=expected.nj();
	//: code to iterate over the blocks in order of visibility 
	boxm_block_vis_graph_iterator<tree_type> block_vis_iter(cam, &scene, expected.ni(), expected.nj());
	while (block_vis_iter.next()) {
		vcl_vector<vgl_point_3d<int> > block_indices = block_vis_iter.frontier_indices();
		for(unsigned i=0; i<block_indices.size(); i++) {
			//: code for each block
			scene.load_block(block_indices[i]);
			boxm_block<boct_tree<T_loc,boxm_sample<APM> > > * curr_block=scene.get_active_block();
			boxm_cell_vis_graph_iterator<T_loc, boxm_sample<APM> > frontier_it(cam,curr_block->get_tree(),ni,nj);	
			//: for each frontier layer of each block
			while (frontier_it.next()){
				vcl_vector<boct_tree_cell<T_loc, boxm_sample<APM> > *> vis_cells=cell_iterator.frontier();
				for(unsigned i=0;i<vis_cells.size();i++)
				{
					//: for each cell

				}

			}
		}
	}
	return;
}




#endif

