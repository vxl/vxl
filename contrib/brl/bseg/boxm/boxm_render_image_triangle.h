#ifndef boxm_render_image_triangle_h_
#define boxm_render_image_triangle_h_

#include <vcl_vector.h>

#include <boct/boct_tree.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_sample.h>
#include <boxm/boxm_utils.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_block_vis_graph_iterator.h>
#include <boxm/boxm_cell_vis_graph_iterator.h>
#include <boxm/boxm_mog_grey_processor.h>
#include <vpgl/vpgl_camera.h>

#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_transform.h>
#include <vil/vil_save.h>
#include <vil/vil_view_as.h>
#include <vcl_iostream.h>
#include <vil/vil_convert.h>
#include <vul/vul_timer.h>

#include <boxm/boxm_render_image.h>
#include <boxm/boxm_raytrace_operations.h>
#include <boxm/boxm_rational_camera_utils.h>
class abs_functor
{
 public:
	 abs_functor(){}

  float operator()(float x) const { return vcl_fabs(x); }

};

template <class T_loc, class T_data>
void boxm_render_image_splatting_triangle(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                                 vpgl_camera_double_sptr cam,
                                 vil_image_view<typename T_data::obs_datatype> &expected,
                                 vil_image_view<float> &mask,int bin=-1,
                                 bool use_black_background = false)
{
  unsigned ni=expected.ni();
  unsigned nj=expected.nj();
  expected.fill(0.0f);
  vgl_plane_3d<double> projection_plane;
  if (vpgl_rational_camera<double> * rcam = dynamic_cast<vpgl_rational_camera<double> *>(cam.ptr())) {
	  vgl_box_3d<double> bbox=scene.get_world_bbox();
	  vgl_plane_3d<double> top(0,0,1,-bbox.max_z());
	  vgl_plane_3d<double> bottom(0,0,1,-bbox.min_z());
	  projection_plane=boxm_rational_camera_utils::boxm_find_parallel_image_plane(rcam, top, bottom,expected.ni(),expected.nj());
  }
  typedef boct_tree<T_loc, T_data> tree_type;
  typedef boct_tree_cell<T_loc, T_data> cell_type;
  vil_image_view<float> vis(expected.ni(),expected.nj(),1);
  vis.fill(1.0f);
  vil_image_view<float> alpha_integral(expected.ni(),expected.nj(),1); alpha_integral.fill(0.0f);
  vil_image_view<float> alpha_img_(expected.ni(),expected.nj(),1); 
  vul_timer t;
  // code to iterate over the blocks in order of visibility
  boxm_block_vis_graph_iterator<boct_tree<T_loc, T_data > >
  block_vis_iter(cam, &scene, expected.ni(), expected.nj());

  int cnt=0;
  while (block_vis_iter.next()) {
    vcl_vector<vgl_point_3d<int> > block_indices = block_vis_iter.frontier_indices();
    for (unsigned i=0; i<block_indices.size(); i++) { // code for each block

      t.mark();
      scene.load_block(block_indices[i].x(),block_indices[i].y(),block_indices[i].z());
      vcl_cout<<"The time taken to read a block is "<<t.all()<<vcl_endl;

      boxm_block<tree_type> * curr_block=scene.get_active_block();
      vcl_cout<<"Block: "<<curr_block->bounding_box()<<vcl_endl
              <<"Tree: "<<curr_block->get_tree()->num_levels()
              << " #of leaf cells "<<curr_block->get_tree()->leaf_cells().size()<<vcl_endl;

      t.mark();
      // project vertices to the image determine which faces of the cell are visible
      boxm_cell_vis_graph_iterator<T_loc, T_data > frontier_it(cam,curr_block->get_tree(),ni,nj);
      vcl_cout<<"The time taken to build the vis graph is "<<t.all()<<vcl_endl;

      // for each frontier layer of each block
      tree_type * tree=curr_block->get_tree();
      vil_image_view<float> front_xyz(expected.ni(),expected.nj(),3);
      vil_image_view<float> back_xyz(expected.ni(),expected.nj(),3);
      vil_image_view<float> alphas(expected.ni(),expected.nj(),1);
      vil_image_view<float> vis_end(expected.ni(),expected.nj(),1);
      vil_image_view<float> temp_expected(expected.ni(),expected.nj(),1);
      vil_image_view<float> temp_weights(expected.ni(),expected.nj(),1);
	  vil_image_view<float> pix_weights_(expected.ni(),expected.nj(),1);
      normalize_expected_functor norm_fn;

      double  xverts[8];
      double  yverts[8];
	  float vertdists[8];
      while (frontier_it.next())
      {
        vcl_vector<cell_type *> vis_cells=frontier_it.frontier();
        typename vcl_vector<cell_type *>::iterator cell_it=vis_cells.begin();
        vis_end.fill(0.0f);
        temp_expected.fill(0.0f);
        temp_weights.fill(0.0f);
        alpha_img_.fill(0.0f);

        vcl_cout<<'.';
        for (;cell_it!=vis_cells.end();cell_it++)
        {
          // for each cell
          T_data sample=(*cell_it)->data();
          //if (sample.alpha>0.001)
          {
            // get vertices of cell in the form of a bounding box (cells are always axis-aligned))
            vgl_box_3d<double> cell_bb = tree->cell_bounding_box(*cell_it);
            vcl_vector<vgl_point_3d<double> > corners=boxm_utils::corners_of_box_3d(cell_bb);
			if (vpgl_perspective_camera<double> * pcam = dynamic_cast<vpgl_perspective_camera<double> *>(cam.ptr())) 
			{	
					boxm_utils::project_corners(corners,cam,xverts,yverts,vertdists);
			}
			else if (vpgl_rational_camera<double> * rcam = dynamic_cast<vpgl_rational_camera<double> *>(cam.ptr())) {
				boxm_rational_camera_utils::project_corners_rational_camera(corners,rcam,projection_plane,xverts,yverts,vertdists);
			}
            boct_face_idx  vis_face_ids=boxm_utils::visible_faces(cell_bb,cam,xverts,yverts);
            //boxm_utils::project_cube_xyz(corners,vis_face_ids,front_xyz,back_xyz,xverts,yverts);
            // get expected color of cell
            typename T_data::obs_datatype cell_expected  =T_data::apm_processor::expected_color(sample.appearance(bin));
            // get  alpha
   			boxm_alpha_seg_len(xverts, yverts, vertdists, vis_face_ids, sample.alpha, alpha_img_);       

			cube_fill_value(xverts, yverts, vis_face_ids, temp_expected,  cell_expected);            // fill expected value image
            //boxm_utils::project_cube_fill_val( vis_face_ids,temp_expected,(float)cell_expected, xverts,yverts);
            //boxm_utils::project_cube_fill_val_aa( vis_face_ids,temp_expected,temp_weights,(float)cell_expected, xverts,yverts);
         }
        }
        // compute the length of ray segment at each pixel
        //vil_image_view<float> len_seg(expected.ni(),expected.nj(),1);

        //len_seg.fill(0.0f);
        //vcl_stringstream s,s1;

        //vil_math_image_difference<float,float>(back_xyz,front_xyz,back_xyz);
        //vil_math_sum_sqr<float,float>(back_xyz,len_seg);
        //vil_math_sqrt<float>(len_seg);
        //vil_math_image_product(len_seg,alphas, alphas);
        //vil_transform2(temp_weights,temp_expected,norm_fn);
		abs_functor abs_fun;
		vil_transform(alpha_img_,alpha_img_,abs_fun);

        // compute visibility
        vil_math_image_difference(alpha_integral, alpha_img_, alpha_integral);
    	vil_save(alpha_img_,"d:/vj/scripts/boxm/exp1/temp_alpha_img.tiff");
  //  	vil_save(temp_expected,"d:/vj/scripts/boxm/exp1/pre_temp_expected.tiff");

		vil_save(alpha_integral,"d:/vj/scripts/boxm/exp1/temp_alpha_integral.tiff");
        // compute new vis image
        image_exp_functor exp_fun;
        vil_transform(alpha_integral,vis_end,exp_fun);
        // compute weights for each pixel
        vil_math_image_difference(vis,vis_end,pix_weights_);
		//vil_save(pix_weights_,"d:/vj/scripts/boxm/exp1/temp_pix_weights.tiff");

        // scale cell expected image by weighting function..
        vil_math_image_product( temp_expected,pix_weights_, temp_expected);
        // ..and use result to update final expected image
        vil_math_image_sum(temp_expected,expected,expected);

		//vil_save(temp_expected,"d:/vj/scripts/boxm/exp1/temp.tiff");
        vis.deep_copy(vis_end);
      }
      //scene.write_active_block();
    }
  }
  vcl_cout<<"\nThe time taken is"<< t.all()<<vcl_endl;
  return;
}

#endif
