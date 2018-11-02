#ifndef boxm_depth_image_triangle_h_
#define boxm_depth_image_triangle_h_
//:
// \file
#include <vector>
#include <iostream>
#include "boxm_render_image.h"
#include <boct/boct_tree.h>
#include <boxm/boxm_scene.h>
#include <boxm/sample/boxm_sample.h>
#include <boxm/util/boxm_utils.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/basic/boxm_block_vis_graph_iterator.h>
#include <boxm/basic/boxm_cell_vis_graph_iterator.h>
#include <boxm/sample/algo/boxm_mog_grey_processor.h>
#include <boxm/util/boxm_raytrace_operations.h>
#include <boxm/util/boxm_rational_camera_utils.h>
#include <vpgl/vpgl_camera.h>

#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_transform.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_timer.h>


template <class T_loc, class T_data>
void boxm_depth_image_splatting_triangle(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                                         vpgl_camera_double_sptr cam,
                                         vil_image_view<float> &expected,
                                         vil_image_view<float> &mask)
{
  unsigned ni=expected.ni();
  unsigned nj=expected.nj();
  expected.fill(0.0f);
  vgl_plane_3d<double> projection_plane;
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

  while (block_vis_iter.next())
  {
    std::vector<vgl_point_3d<int> > block_indices = block_vis_iter.frontier_indices();
    for (auto & block_indice : block_indices) // code for each block
    {
      t.mark();
      scene.load_block(block_indice.x(),block_indice.y(),block_indice.z());
      std::cout<<"The time taken to read a block is "<<t.all()<<std::endl;
      boxm_block<tree_type> * curr_block=scene.get_active_block();
      t.mark();
      // project vertices to the image determine which faces of the cell are visible
      boxm_cell_vis_graph_iterator<T_loc, T_data > frontier_it(cam,curr_block->get_tree(),ni,nj);
      std::cout<<"The time taken to build the vis graph is "<<t.all()<<std::endl;

      // for each frontier layer of each block
      tree_type * tree=curr_block->get_tree();
      vil_image_view<float> vis_end(expected.ni(),expected.nj(),1);
      vil_image_view<float> temp_depth(expected.ni(),expected.nj(),1);
      vil_image_view<float> temp_weights(expected.ni(),expected.nj(),1);
      vil_image_view<float> pix_weights_(expected.ni(),expected.nj(),1);
      normalize_expected_functor norm_fn;

      double xverts[8];
      double yverts[8];
      float vertdists[8];
      while (frontier_it.next())
      {
        std::vector<cell_type *> vis_cells=frontier_it.frontier();
        typename std::vector<cell_type *>::iterator cell_it=vis_cells.begin();
        vis_end.fill(0.0f);
        temp_depth.fill(0.0f);
        temp_weights.fill(0.0f);
        alpha_img_.fill(0.0f);

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
            if (dynamic_cast<vpgl_perspective_camera<double> *>(cam.ptr()))
            boxm_utils::project_corners(corners,cam,xverts,yverts,vertdists);
            boct_face_idx  vis_face_ids=boxm_utils::visible_faces(cell_bb,cam,xverts,yverts);
            // get  alpha
            boxm_alpha_seg_len(xverts, yverts, vertdists, vis_face_ids, sample.alpha, alpha_img_);
            boxm_depth_fill(xverts, yverts, vertdists, vis_face_ids,temp_depth);
          }
        }
        abs_functor abs_fun;
        vil_transform(alpha_img_,alpha_img_,abs_fun);

        // compute visibility
        vil_math_image_difference(alpha_integral, alpha_img_, alpha_integral);
        // compute new vis image
        image_exp_functor exp_fun;
        vil_transform(alpha_integral,vis_end,exp_fun);
        // compute weights for each pixel
        vil_math_image_difference(vis,vis_end,pix_weights_);

        // scale cell expected image by weighting function..
        vil_math_image_product( temp_depth,pix_weights_, temp_depth);
        // ..and use result to update final expected image
        vil_math_image_sum(temp_depth,expected,expected);

        vis.deep_copy(vis_end);
      }
    }
  }
  std::cout<<"\nThe time taken is"<< t.all()<<std::endl;
  return;
}

#endif
