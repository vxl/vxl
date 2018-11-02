#ifndef boxm_render_image_h_
#define boxm_render_image_h_

#include <vector>
#include <sstream>
#include <iostream>

#include <boct/boct_tree.h>
#include <boxm/boxm_scene.h>
#include <boxm/sample/boxm_sample.h>
#include <boxm/util/boxm_utils.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/basic/boxm_block_vis_graph_iterator.h>
#include <boxm/basic/boxm_cell_vis_graph_iterator.h>
#include <boxm/sample/algo/boxm_mog_grey_processor.h>
#include <vpgl/vpgl_camera_double_sptr.h>

#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_transform.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_timer.h>

class normalize_expected_functor
{
 public:
  normalize_expected_functor() = default;

  void operator()(float  norm, float & val) const
  {
    if (norm>0)
      val/=norm;
  }
};

class image_exp_functor
{
 public:
  float operator()(float x) const { return x<0?std::exp(x):1.0f; }
};


template <class T_loc, class T_data>
void boxm_render_image_splatting(boxm_scene<boct_tree<T_loc,T_data> >& scene,
                                 vpgl_camera_double_sptr cam,
                                 vil_image_view<typename T_data::obs_datatype>& expected,
                                 vil_image_view<float>& /*mask*/,       // TODO: use this param
                                 int bin=-1,
                                 bool /*use_black_background*/ = false) // TODO: use this param
{
  unsigned ni=expected.ni();
  unsigned nj=expected.nj();
  expected.fill(0.0f);

  typedef boct_tree<T_loc, T_data> tree_type;
  typedef boct_tree_cell<T_loc, T_data> cell_type;
  vil_image_view<float> vis(expected.ni(),expected.nj(),1);
  vis.fill(1.0f);
  vil_image_view<float> alpha_integral(expected.ni(),expected.nj(),1); alpha_integral.fill(0.0f);

  vul_timer t;
  // code to iterate over the blocks in order of visibility
  boxm_block_vis_graph_iterator<boct_tree<T_loc, T_data > >
  block_vis_iter(cam, &scene, expected.ni(), expected.nj());

  int cnt=0;
  while (block_vis_iter.next()) {
    std::vector<vgl_point_3d<int> > block_indices = block_vis_iter.frontier_indices();
    for (auto & block_indice : block_indices) { // code for each block

      t.mark();
      scene.load_block(block_indice.x(),block_indice.y(),block_indice.z());
      std::cout<<"The time taken to read a block is "<<t.all()<<std::endl;

      boxm_block<tree_type> * curr_block=scene.get_active_block();
      std::cout<<"Block: "<<curr_block->bounding_box()<<std::endl
              <<"Tree: "<<curr_block->get_tree()->number_levels()
              << " #of leaf cells "<<curr_block->get_tree()->leaf_cells().size()<<std::endl;

      t.mark();
      // project vertices to the image determine which faces of the cell are visible
      boxm_cell_vis_graph_iterator<T_loc, T_data > frontier_it(cam,curr_block->get_tree(),ni,nj);
      std::cout<<"The time taken to build the vis graph is "<<t.all()<<std::endl;

      // for each frontier layer of each block
      tree_type * tree=curr_block->get_tree();
      vil_image_view<float> front_xyz(expected.ni(),expected.nj(),3);
      vil_image_view<float> back_xyz(expected.ni(),expected.nj(),3);
      vil_image_view<float> alphas(expected.ni(),expected.nj(),1);
      vil_image_view<float> vis_end(expected.ni(),expected.nj(),1);
      vil_image_view<float> temp_expected(expected.ni(),expected.nj(),1);
      vil_image_view<float> temp_weights(expected.ni(),expected.nj(),1);
      normalize_expected_functor norm_fn;

      double  xverts[8];
      double  yverts[8];

      while (frontier_it.next())
      {
        std::vector<cell_type *> vis_cells=frontier_it.frontier();
        typename std::vector<cell_type *>::iterator cell_it=vis_cells.begin();
        front_xyz.fill(0.0f);
        back_xyz.fill(0.0f);
        alphas.fill(0.0f);
        vis_end.fill(0.0f);
        temp_expected.fill(0.0f);
        temp_weights.fill(0.0f);
        std::cout<<'.';
        for (;cell_it!=vis_cells.end();cell_it++)
        {
          // for each cell
          T_data sample=(*cell_it)->data();
          //if (sample.alpha>0.001)
          {
            // get vertices of cell in the form of a bounding box (cells are always axis-aligned))
            vgl_box_3d<double> cell_bb = tree->cell_bounding_box(*cell_it);
            std::vector<vgl_point_3d<double> > corners=boxm_utils::corners_of_box_3d(cell_bb);
            boxm_utils::project_corners(corners,cam,xverts,yverts);
            boct_face_idx  vis_face_ids=boxm_utils::visible_faces(cell_bb,cam,xverts,yverts);
            boxm_utils::project_cube_xyz(corners,vis_face_ids,front_xyz,back_xyz,xverts,yverts);
            // get expected color of cell
            typename T_data::obs_datatype cell_expected  =T_data::apm_processor::expected_color(sample.appearance(bin));
            // get  alpha
            boxm_utils::project_cube_fill_val( vis_face_ids,alphas,sample.alpha, xverts,yverts);
            // fill expected value image
            //boxm_utils::project_cube_fill_val( vis_face_ids,temp_expected,(float)cell_expected, xverts,yverts);
            boxm_utils::project_cube_fill_val_aa( vis_face_ids,temp_expected,temp_weights,(float)cell_expected, xverts,yverts);
         }
        }
        // compute the length of ray segment at each pixel
        vil_image_view<float> len_seg(expected.ni(),expected.nj(),1);

        len_seg.fill(0.0f);
        std::stringstream s,s1;

        vil_math_image_difference<float,float>(back_xyz,front_xyz,back_xyz);
        vil_math_sum_sqr<float,float>(back_xyz,len_seg);
        vil_math_sqrt<float>(len_seg);
        vil_math_image_product(len_seg,alphas, alphas);
        vil_transform2(temp_weights,temp_expected,norm_fn);
        ++cnt;

        // compute visibility
        vil_math_image_difference(alpha_integral, alphas, alpha_integral);
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
      }
      scene.write_active_block();
    }
  }
  std::cout<<"\nThe time taken is"<< t.all()<<std::endl;
  return;
}

#endif
