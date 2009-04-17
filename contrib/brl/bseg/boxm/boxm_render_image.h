#ifndef boxm_render_image_h_
#define boxm_render_image_h_

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


class image_exp_functor
{
 public:
  float operator()(float x)       const { return x<0?vcl_exp(x):1.0f; }
};


template <class T_loc, boxm_apm_type APM>
void boxm_render_image_splatting(boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene, vpgl_camera_double_sptr cam,
             vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> &expected,
             vil_image_view<float> &mask,
             bool use_black_background = false)
{
  unsigned ni=expected.ni();
  unsigned nj=expected.nj();
  expected.fill(0.0);

  typedef boct_tree<T_loc, boxm_sample<APM> > tree_type;
  vil_image_view<float> vis(expected.ni(),expected.nj(),1);vis.fill(1.0);
  vil_image_view<float> alpha_integral(expected.ni(),expected.nj(),1);alpha_integral.fill(0.0);

	//: code to iterate over the blocks in order of visibility 
	boxm_block_vis_graph_iterator<boct_tree<T_loc, boxm_sample<APM> > > block_vis_iter(cam, &scene, expected.ni(), expected.nj());
	int cnt=0;
	while (block_vis_iter.next()) {
		vcl_vector<vgl_point_3d<int> > block_indices = block_vis_iter.frontier_indices();
		for(unsigned i=0; i<block_indices.size(); i++) {
			//: code for each block
			//if(block_indices[i].x()==1 &&block_indices[i].y()==1&&block_indices[i].z()==0)
			{
			scene.load_block(block_indices[i].x(),block_indices[i].y(),block_indices[i].z());
			boxm_block<tree_type> * curr_block=scene.get_active_block();
			vcl_map<boct_face_idx,vcl_vector< vgl_point_3d<double> > >  faces1;
			boct_face_idx  vis_face_ids1;
			// project vertices to the image determine which faces of the cell are visible
			boxm_cell_vis_graph_iterator<T_loc, boxm_sample<APM> > frontier_it(cam,curr_block->get_tree(),ni,nj);	

			//: for each frontier layer of each block
			boct_tree<T_loc,boxm_sample<APM> > * tree=curr_block->get_tree();
			while (frontier_it.next()){
				vcl_vector<boct_tree_cell<T_loc, boxm_sample<APM> > *> vis_cells=frontier_it.frontier();
				vcl_vector<boct_tree_cell<T_loc, boxm_sample<APM> > *>::iterator cell_it=vis_cells.begin();
				vil_image_view<float> front_xyz(expected.ni(),expected.nj(),3);front_xyz.fill(0.0);
				vil_image_view<float> back_xyz(expected.ni(),expected.nj(),3); back_xyz.fill(0.0);
				vil_image_view<float> alphas(expected.ni(),expected.nj(),1);alphas.fill(0.0);
				vil_image_view<float> vis_end(expected.ni(),expected.nj(),1);vis_end.fill(0.0);
				vil_image_view<float> temp_expected(expected.ni(),expected.nj(),1);temp_expected.fill(0.0);

				for(;cell_it!=vis_cells.end();cell_it++)
				{
					//: for each cell
					boxm_sample<APM> sample=(*cell_it)->data();
					//if(sample.alpha>0.001)
					{
						// get vertices of cell in the form of a bounding box (cells are always axis-aligned))
						vgl_box_3d<double> cell_bb = tree->cell_bounding_box(*cell_it);
						vcl_map<boct_face_idx,vcl_vector< vgl_point_3d<double> > >  faces;
						boct_face_idx  vis_face_ids;
						// project vertices to the image determine which faces of the cell are visible
						boxm_utils::project_cube(cell_bb,cam,faces,vis_face_ids);
						// get XYZ of each cell
						boxm_utils::project_cube_xyz(faces,vis_face_ids,front_xyz,back_xyz,cam);
						// get expected color of cell
						typename boxm_apm_traits<APM>::obs_datatype cell_expected  = 
							boxm_apm_traits<APM>::apm_processor::expected_color(sample.appearance);
						// get  alpha
						boxm_utils::project_cube_fill_val( faces,vis_face_ids,alphas,sample.alpha, cam);
						// fill expected value image
						boxm_utils::project_cube_fill_val( faces,vis_face_ids,temp_expected,(float)cell_expected, cam);
   
					}
				}
				//: compute the length of ray segment at each pixel
				vil_image_view<float> len_seg(expected.ni(),expected.nj(),1);
				len_seg.fill(0.0);
				//vil_save(vil_plane(back_xyz,0),"./back_x.tiff");
				//vil_save(vil_plane(back_xyz,1),"./back_y.tiff");
				//vil_save(vil_plane(back_xyz,2),"./back_z.tiff");
				//
				//vil_save(vil_plane(front_xyz,0),"./front_x.tiff");
				//vil_save(vil_plane(front_xyz,1),"./front_y.tiff");
				//vil_save(vil_plane(front_xyz,2),"./front_z.tiff");
				//vil_save(temp_expected,"./temp_expected.tiff");

				vil_math_image_difference<float,float>(back_xyz,front_xyz,back_xyz);
				vil_math_sum_sqr<float,float>(back_xyz,len_seg);
				vil_math_sqrt<float>(len_seg);
				//vil_save(len_seg,"./length.tiff");
				vil_math_image_product(len_seg,alphas, alphas);
				//vil_save(alphas,"./lengthalpha.tiff");
				//: compute visibility
				vil_math_image_difference(alpha_integral, alphas, alpha_integral);
				//vil_save(alpha_integral,"./alpha_integral.tiff");
				// compute new vis image
				image_exp_functor exp_fun;
				vil_transform(alpha_integral,vis_end,exp_fun);
				// compute weights for each pixel
				vil_math_image_difference(vis,vis_end,vis);
				// scale cell expected image by weighting function..
				vil_math_image_product( temp_expected,vis, temp_expected);
				// ..and use result to update final expected image
				vil_math_image_sum(temp_expected,expected,expected);
				
				vis.deep_copy(vis_end);
				//vil_save(vis,"./vis.tiff");

				vcl_ostringstream s;
				s<<"./tempimage"<<cnt<<".tif";
				//vil_image_view<unsigned char> img;
				//vil_convert_stretch_range(expected,img);
				vil_save(expected,s.str().c_str());
				cnt++;
				
			}
		}
		}
	}
	return;

}


#endif
