#ifndef boxm_update_triangle_h_
#define boxm_update_triangle_h_
//:
// \file
#include <iostream>
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boxm/util/boxm_utils.h>
#include <boxm/boxm_block.h>
#include <boxm/basic/boxm_cell_vis_graph_iterator.h>
#include <boxm/basic/boxm_block_vis_graph_iterator.h>
#include <boxm/algo/sp/boxm_render_image.h>
#include <boxm/util/boxm_raytrace_operations.h>
#include <boxm/util/boxm_rational_camera_utils.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_math.h>
#include <vul/vul_timer.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: functor used for normalizing cell_expected image
template <class T_loc, class T_data>
void boxm_update_triangle_pass1(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                                vpgl_camera_double_sptr cam,
                                vil_image_view<typename T_data::obs_mathtype> &img,
                                vil_image_view<float> &norm_img,
                                typename T_data::apm_datatype background_model, int bin)
{
  typedef boct_tree<T_loc, T_data> tree_type;
  typedef boct_tree_cell<T_loc, T_data> cell_type;
  unsigned ni=img.ni();
  unsigned nj=img.nj();
  double xverts[8];
  double yverts[8];
  float vertdists[8];

  vil_image_view<float> pre(ni,nj,1);
  vil_image_view<float> vis(ni,nj,1);
  vil_image_view<float> alpha_integral(ni,nj,1);
  vil_image_view<float> PI_img(ni,nj,1);
  vil_image_view<float> pix_weights_(ni,nj,1);
  vil_image_view<float> alpha_img_(ni,nj,1);

  alpha_integral.fill(0.0f);
  pre.fill(0.0f);
  vis.fill(1.0f);

  vgl_plane_3d<double> projection_plane;
  if (vpgl_rational_camera<double> * rcam = dynamic_cast<vpgl_rational_camera<double> *>(cam.ptr())) {
    vgl_box_3d<double> bbox=scene.get_world_bbox();
    vgl_plane_3d<double> top(0,0,1,-bbox.max_z());
    vgl_plane_3d<double> bottom(0,0,1,-bbox.min_z());
    projection_plane=boxm_rational_camera_utils::boxm_find_parallel_image_plane(rcam, top, bottom,img.ni(),img.nj());
  }

  // code to iterate over the blocks in order of visibility
  boxm_block_vis_graph_iterator<boct_tree<T_loc, T_data > > block_vis_iter(cam, &scene, ni,nj);
  while (block_vis_iter.next())
  {
    std::vector<vgl_point_3d<int> > block_indices = block_vis_iter.frontier_indices();
    for (auto & block_indice : block_indices) // code for each block
    {
      scene.load_block(block_indice.x(),block_indice.y(),block_indice.z());
      boxm_block<tree_type> * curr_block=scene.get_active_block();
      // project vertices to the image determine which faces of the cell are visible
      boxm_cell_vis_graph_iterator<T_loc,T_data > frontier_it(cam,curr_block->get_tree(),ni,nj);

      // for each frontier layer of each block
      tree_type * tree=curr_block->get_tree();
      vil_image_view<float> front_xyz(ni,nj,1);
      vil_image_view<float> back_xyz(ni,nj,1);
      vil_image_view<float> alphas(ni,nj,1);
      vil_image_view<float> vis_end(ni,nj,1);
      vil_image_view<float> temp_expected(ni,nj,1);
      int cnt=0;

      while (frontier_it.next())
      {
        ++cnt;
        std::vector<cell_type *> vis_cells=frontier_it.frontier();
        typename std::vector<cell_type *>::iterator cell_it=vis_cells.begin();
        alpha_img_.fill(0.0f);
        vis_end.fill(0.0f);
        temp_expected.fill(0.0f);
        PI_img.fill(0.0f);
        for (;cell_it!=vis_cells.end();cell_it++)
        {
          // for each cell
          T_data sample=(*cell_it)->data();
          // get vertices of cell in the form of a bounding box (cells are always axis-aligned))
          vgl_box_3d<double> cell_bb = tree->cell_bounding_box(*cell_it);
          std::vector<vgl_point_3d<double> > corners=boxm_utils::corners_of_box_3d(cell_bb);
          if (dynamic_cast<vpgl_perspective_camera<double> *>(cam.ptr()))
          {
            boxm_utils::project_corners(corners,cam,xverts,yverts,vertdists);
          }
          else if (vpgl_rational_camera<double> * rcam = dynamic_cast<vpgl_rational_camera<double> *>(cam.ptr())) {
            boxm_rational_camera_utils::project_corners_rational_camera(corners,rcam,projection_plane,xverts,yverts,vertdists);
          }
          boct_face_idx  vis_face_ids=boxm_utils::visible_faces(cell_bb,cam,xverts,yverts);
          //boxm_utils::project_cube_xyz(corners,vis_face_ids,front_xyz,back_xyz,xverts,yverts,vertdists);
          boxm_alpha_seg_len(xverts, yverts, vertdists, vis_face_ids, sample.alpha, alpha_img_);


          typename T_data::obs_datatype cell_mean_obs;
          if (cube_mean(xverts, yverts, vertdists, vis_face_ids,img,cell_mean_obs)) {
            // get probability density of mean observation
            float cell_PI = T_data::apm_processor::prob_density(sample.appearance(bin), cell_mean_obs);
            if (!((cell_PI >= 0) && (cell_PI < 1e8)) ) {
              std::cout << "\ncell_PI = " << cell_PI << '\n'
                       << "  cell_obs = " << cell_mean_obs << '\n'
                       << "  cell id = " << *cell_it << std::endl;
            }
            // fill obs probability density image
            cube_fill_value(xverts, yverts, vis_face_ids, PI_img, cell_PI);
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
        vil_math_image_product( pix_weights_,PI_img, PI_img);
        // ..and use result to update final expected image
        vil_math_image_sum(PI_img,pre,pre);
        vis.deep_copy(vis_end);
#if 0
        if (cnt == 10) {
          std::cout << "saving debug images" << std::endl;
          std::string output_dir = "D:/vj/scripts/boxm/exp1/";
          vil_save(alpha_img_,(output_dir + "alpha_img.tiff").c_str());
          vil_save(alpha_integral,(output_dir + "alpha_integral.tiff").c_str());
          vil_save(pre,(output_dir + "pre_img.tiff").c_str());
          vil_save(PI_img,(output_dir + "PI_img.tiff").c_str());
          vil_save(vis,(output_dir + "vis.tiff").c_str());
          vil_save(vis_end,(output_dir + "vis_end.tiff").c_str());
          vil_save(img,(output_dir + "obs.tiff").c_str());
        }
#endif
      }
      scene.write_active_block();
    }
  }
  vil_image_view<float> PI_background(norm_img.ni(),norm_img.nj(),1);
  for (unsigned int j=0; j<img.nj(); ++j) {
    for (unsigned int i=0; i<img.ni(); ++i) {
      // this will have to be modified slightly when we deal with multi-plane images -dec
      PI_background(i,j) = T_data::apm_processor::prob_density(background_model, img(i,j));
    }
  }

  vil_math_image_product(PI_background, vis, norm_img);
  vil_math_image_sum(pre,norm_img,norm_img);
  safe_inverse_functor inv_func(1e-8f);
  vil_transform(norm_img, inv_func);
}


template <class T_loc, class T_data>
void boxm_update_triangle_pass2(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                                vpgl_camera_double_sptr cam,
                                vil_image_view<typename T_data::obs_mathtype> &img,
                                vil_image_view<float> &norm_img, int bin)
{
  unsigned ni=img.ni();
  unsigned nj=img.nj();
  typedef boct_tree<T_loc, T_data > tree_type;
  typedef boct_tree_cell<T_loc, T_data > cell_type;
  vil_image_view<float> pre_img(ni,nj,1); pre_img.fill(0.0f);
  vil_image_view<float> vis(ni,nj,1); vis.fill(1.0f);
  vil_image_view<float> alpha_integral(ni,nj,1);
  vil_image_view<float> PI_img(ni,nj,1); PI_img.fill(0.0f);
  vil_image_view<float> pix_weights(ni,nj,1);
  alpha_integral.fill(0.0f);
  pre_img.fill(0.0f);
  vis.fill(1.0f);

  double xverts[8];
  double yverts[8];
  float vertdists[8];
  vgl_plane_3d<double> projection_plane;
  if (vpgl_rational_camera<double> * rcam = dynamic_cast<vpgl_rational_camera<double> *>(cam.ptr())) {
    vgl_box_3d<double> bbox=scene.get_world_bbox();
    vgl_plane_3d<double> top(0,0,1,-bbox.max_z());
    vgl_plane_3d<double> bottom(0,0,1,-bbox.min_z());
    projection_plane=boxm_rational_camera_utils::boxm_find_parallel_image_plane(rcam, top, bottom,img.ni(),img.nj());
  }
  vul_timer t;  t.mark();
  // code to iterate over the blocks in order of visibility
  boxm_block_vis_graph_iterator<boct_tree<T_loc,T_data > > block_vis_iter(cam, &scene, ni,nj);
  while (block_vis_iter.next()) {
    std::vector<vgl_point_3d<int> > block_indices = block_vis_iter.frontier_indices();
    for (auto & block_indice : block_indices) { // code for each block
      scene.load_block(block_indice.x(),block_indice.y(),block_indice.z());
      boxm_block<tree_type> * curr_block=scene.get_active_block();
      boxm_cell_vis_graph_iterator<T_loc, T_data > frontier_it(cam,curr_block->get_tree(),ni,nj);

      // for each frontier layer of each block
      tree_type * tree=curr_block->get_tree();
      vil_image_view<float> alpha_img_(ni,nj,1);
      vil_image_view<float> vis_end(ni,nj,1);
      vil_image_view<float> temp_expected(ni,nj,1);
      vil_image_view<float> update_factor(ni,nj,1);

      while (frontier_it.next())
      {
        std::vector<cell_type *> vis_cells=frontier_it.frontier();
        typename std::vector<cell_type *>::iterator cell_it=vis_cells.begin();
        vis_end.fill(0.0f);
        temp_expected.fill(0.0f);
        PI_img.fill(0.0f);
        alpha_img_.fill(0.0f);

        std::cout<<'.';
        for (;cell_it!=vis_cells.end();cell_it++)
        {
          // for each cell
          T_data sample=(*cell_it)->data();
          // get vertices of cell in the form of a bounding box (cells are always axis-aligned))
          vgl_box_3d<double> cell_bb = tree->cell_bounding_box(*cell_it);
          std::vector<vgl_point_3d<double> > corners=boxm_utils::corners_of_box_3d(cell_bb);
          if (dynamic_cast<vpgl_perspective_camera<double> *>(cam.ptr()))
          {
            boxm_utils::project_corners(corners,cam,xverts,yverts,vertdists);
          }
          else if (vpgl_rational_camera<double> * rcam = dynamic_cast<vpgl_rational_camera<double> *>(cam.ptr())) {
            boxm_rational_camera_utils::project_corners_rational_camera(corners,rcam,projection_plane,xverts,yverts,vertdists);
          }

          boct_face_idx  vis_face_ids=boxm_utils::visible_faces(cell_bb,cam,xverts,yverts);
          boxm_alpha_seg_len(xverts, yverts, vertdists, vis_face_ids, sample.alpha, alpha_img_);
          typename T_data::obs_datatype cell_mean_obs;
          if (cube_mean(xverts, yverts, vertdists, vis_face_ids,img,cell_mean_obs)) {
            // get probability density of mean observation
            float cell_PI = T_data::apm_processor::prob_density(sample.appearance(bin), cell_mean_obs);
#if 0
            if (!((cell_PI >= 0) && (cell_PI < 1e8)) ) {
              std::cout << "\ncell_PI = " << cell_PI << '\n'
                       << "  cell_obs = " << cell_mean_obs << '\n'
                       << "  cell id = " << *cell_it << std::endl;
            }
#endif // 0
            // fill obs probability density image
            cube_fill_value(xverts, yverts, vis_face_ids, PI_img, cell_PI);
          }
          float cell_mean_vis = 0.0f;
          if (cube_mean(xverts, yverts, vertdists, vis_face_ids,vis,cell_mean_vis)) {
            // update appearance model
            if (cell_mean_vis > 1e-6) {
              T_data::apm_processor::update(sample.appearance(bin), cell_mean_obs, cell_mean_vis);
            }
          }
          (*cell_it)->set_data(sample);
        }
        abs_functor abs_fun;
        vil_transform(alpha_img_,alpha_img_,abs_fun);

        // compute visibility
        vil_math_image_difference(alpha_integral, alpha_img_, alpha_integral);

        // compute new vis image
        image_exp_functor exp_fun;
        vil_transform(alpha_integral,vis_end,exp_fun);
        // compute weights for each pixel
        vil_math_image_difference(vis,vis_end,pix_weights);
        // multiply PI by vis..
        vil_math_image_product(PI_img, vis, update_factor);
        // ..add preX to get numerator term for update..
        vil_math_image_sum(pre_img, update_factor, update_factor);
        //.. and normalize
        vil_math_image_product(norm_img, update_factor, update_factor);


        float max_cell_P=0.99f;
        float min_cell_P=0.001f;
        for (cell_it=vis_cells.begin();cell_it!=vis_cells.end();cell_it++)
        {
          // for each cell
          T_data sample=(*cell_it)->data();
          // get vertices of cell in the form of a bounding box (cells are always axis-aligned))
          vgl_box_3d<double> cell_bb = tree->cell_bounding_box(*cell_it);
          std::vector<vgl_point_3d<double> > corners=boxm_utils::corners_of_box_3d(cell_bb);
          if (dynamic_cast<vpgl_perspective_camera<double> *>(cam.ptr()))
          {
            boxm_utils::project_corners(corners,cam,xverts,yverts,vertdists);
          }
          else if (vpgl_rational_camera<double> * rcam = dynamic_cast<vpgl_rational_camera<double> *>(cam.ptr())) {
            boxm_rational_camera_utils::project_corners_rational_camera(corners,rcam,projection_plane,xverts,yverts,vertdists);
          }
          boct_face_idx  vis_face_ids=boxm_utils::visible_faces(cell_bb,cam,xverts,yverts);

          float mean_update_factor = 0.0f;
          if (cube_mean(xverts,yverts,vertdists,vis_face_ids, update_factor, mean_update_factor))
          {
            // update alpha value
            sample.alpha *= mean_update_factor;
            // do bounds check on new alpha value
            float cell_len = float(cell_bb.max_x() - cell_bb.min_x());
            float max_alpha = -std::log(1.0f - max_cell_P)/cell_len;
            float min_alpha = -std::log(1.0f - min_cell_P)/cell_len;
            if (sample.alpha > max_alpha)
            sample.alpha = max_alpha;

            if (sample.alpha < min_alpha)
            sample.alpha = min_alpha;

            if (!((sample.alpha >= min_alpha) && (sample.alpha <= max_alpha))) {
              std::cerr << "\nerror: cell.alpha = " << sample.alpha << '\n'
                       << "mean_update_factor = " << mean_update_factor << '\n';
            }
            (*cell_it)->set_data(sample);
          }
        }
        // multiply cell_PI by cell weights
        vil_math_image_product(pix_weights, PI_img, PI_img);
        // update pre_img
        vil_math_image_sum(PI_img, pre_img, pre_img);
        vis.deep_copy(vis_end);
      }
      scene.write_active_block();
    }
  }
}

template <class T_loc, class T_data>
void boxm_update_triangle(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                          vil_image_view<typename T_data::obs_datatype> &img,
                          vpgl_camera_double_sptr cam, int bin=-1,
                          bool black_background = false)
{
  typename T_data::apm_datatype background_apm;

  if (black_background) {
    std::cout << "using black background model" << std::endl;
    for (unsigned int i=0; i<4; ++i) {
      T_data::apm_processor::update(background_apm, 0.0f, 1.0f);
      T_data::apm_processor::prob_density(background_apm,0.0f);
    }
  }
  vil_image_view<float> norm_img(img.ni(), img.nj(), 1);
  boxm_update_triangle_pass1<T_loc,T_data>(scene, cam,img,norm_img,background_apm,bin);
  std::cout << "update: pass1 completed" << std::endl;
  boxm_update_triangle_pass2<T_loc,T_data>(scene, cam,img,norm_img,bin);
  std::cout << "update: pass2 completed" << std::endl;

  return;
}

#endif
