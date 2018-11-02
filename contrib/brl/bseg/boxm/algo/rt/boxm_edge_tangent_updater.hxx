#ifndef boxm_edge_tangent_updater_hxx_
#define boxm_edge_tangent_updater_hxx_

#include <utility>
#include <vector>
#include <iostream>
#include <string>
#include "boxm_edge_tangent_updater.h"

#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_aux_traits.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_aux_scene.h>
#include <boxm/sample/boxm_edge_tangent_sample.h>
#include <boxm/sample/boxm_inf_line_sample.h>
#include <boxm/sample/boxm_plane_obs.h>
#include <boxm/util/boxm_plane_ransac.h>
#include <vpgl/vpgl_camera.h>

#include <vgl/vgl_infinite_line_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_intersection.h>
#include <vgl/algo/vgl_intersection.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bsta/bsta_histogram.h>

template <class T_loc, class APM, class AUX>
boxm_edge_tangent_updater<T_loc,APM,AUX>::boxm_edge_tangent_updater(boxm_scene<boct_tree<T_loc,
                                                                    boxm_inf_line_sample<APM> > > &scene,
                                                                    std::vector<std::string>  image_ids,
                                                                    bool use_ransac,
                                                                    float ransac_ortho_thres,
                                                                    float ransac_volume_ratio,
                                                                    int ransac_concensus_cnt)
: image_ids_(std::move(image_ids)), use_ransac_(use_ransac), ransac_ortho_thres_(ransac_ortho_thres), ransac_volume_ratio_(ransac_volume_ratio), ransac_consensus_cnt_(ransac_concensus_cnt), scene_(scene)
{}


template <class T_loc, class APM, class AUX>
bool boxm_edge_tangent_updater<T_loc,APM,AUX>::add_cells()
{
  // get auxiliary scenes associated with each input image

  typedef boct_tree<T_loc, boxm_inf_line_sample<APM> > tree_type;

  std::vector<boxm_aux_scene<T_loc,  boxm_inf_line_sample<APM>, boxm_edge_tangent_sample<AUX> > > aux_scenes;
  for (unsigned int i=0; i<image_ids_.size(); ++i) {
    boxm_aux_scene<T_loc, boxm_inf_line_sample<APM>, boxm_edge_tangent_sample<AUX> >
    aux_scene(&scene_,image_ids_[i],boxm_aux_scene<T_loc, boxm_inf_line_sample<APM>,
              boxm_edge_tangent_sample<AUX> >::LOAD, BOXM_EDGE_TANGENT_LINE);
    aux_scenes.push_back(aux_scene);
  }

  std::vector<boxm_edge_tangent_sample<APM> > aux_samples;

  // for each block
  boxm_block_iterator<tree_type> iter(&scene_);
  iter.begin();
  while (!iter.end())
  {
    scene_.load_block(iter.index());
    boxm_block<tree_type>* block = *iter;
    boct_tree<T_loc, boxm_inf_line_sample<APM> >* tree = block->get_tree();
    std::vector<boct_tree_cell<T_loc,boxm_inf_line_sample<APM> >*> cells = tree->leaf_cells();
    int nums=0;
    // get a vector of incremental readers for each aux scene.
    std::vector<boct_tree_cell_reader<T_loc, boxm_edge_tangent_sample<AUX> > *> aux_readers(aux_scenes.size());
    std::vector<unsigned> aux_samples_num(aux_scenes.size());
    for (unsigned int i=0; i<aux_scenes.size(); ++i) {
      aux_readers[i] = aux_scenes[i].get_block_incremental(iter.index());
      aux_samples_num[i] = 0;
    }
    //int a;
    //std::cin>>a;
    // iterate over cells
    for (unsigned i=0; i<cells.size(); ++i)
    {
      aux_samples.clear();
      boct_tree_cell<T_loc,boxm_inf_line_sample<APM> >* cell = cells[i];
      // initialize the cell
      boxm_inf_line_sample<AUX> data(vgl_infinite_line_3d<AUX>(vgl_vector_2d<AUX>(0,0),vgl_vector_3d<AUX>(1,1,1)));
      cell->set_data(data);

      for (unsigned j=0; j<aux_readers.size(); j++) {
        boct_tree_cell<T_loc, boxm_edge_tangent_sample<AUX> > temp_cell;

        if (!aux_readers[j]->next(temp_cell)) {
          std::cerr << "error: incremental reader returned false.\n";
          return false;
        }

        if (!temp_cell.code_.isequal(&(cell->code_))) {
          std::cerr << "error: temp_cell idx does not match cell idx.\n";
          return false;
        }
        if (temp_cell.data().num_obs()>0)
        {
            aux_samples.push_back(temp_cell.data());
            //aux_samples_num[j]++;
        }
      }

      if (aux_samples.size() > 1)
      {
        std::vector<vgl_plane_3d<AUX> > planes;
        std::vector<AUX> weights;

        for (unsigned int k=0;k<aux_samples.size(); ++k) {
          boxm_edge_tangent_sample<APM> s = aux_samples[k];
          for (unsigned int j=0; j<s.num_obs(); j++) {
            boxm_plane_obs<AUX> obs = s.obs(j);
            weights.push_back(obs.seg_len_);
            vgl_plane_3d<AUX> plane(obs.plane_);
            planes.push_back(plane);
          }
        }
        nums+=planes.size();
        if (planes.size() > 1) {
          if (use_ransac_) {
            vgl_infinite_line_3d<AUX> line;
            float residual=1e5;
            vgl_box_3d<double> bb = tree->cell_bounding_box(cell);

            // ransac_consensus_cnt_ used to be hard-coded here to 5; made it into a parameter with a default value of 3
            if (boxm_plane_ransac<AUX>(aux_samples, weights, line, residual,bb, ransac_consensus_cnt_, ransac_ortho_thres_, ransac_volume_ratio_)) {
              boxm_inf_line_sample<AUX> data(line,aux_samples.size());
              data.residual_=residual;

              // now set the clipped line
              // convert to type double
              vgl_vector_2d<double> x0(line.x0().x(), line.x0().y());
              vgl_point_3d<double> p0,p1;
              vgl_vector_3d<double> dir(line.direction().x(), line.direction().y(), line.direction().z());
              vgl_infinite_line_3d<double> dline(x0,dir);
              if (!vgl_intersection<double>(bb, dline, p0, p1))
                data.residual_=0;
              else {
                // convert back to type AUX
                vgl_point_3d<AUX> p0_t(AUX(p0.x()), AUX(p0.y()), AUX(p0.z()));
                vgl_point_3d<AUX> p1_t(AUX(p1.x()), AUX(p1.y()), AUX(p1.z()));
                vgl_line_3d_2_points<AUX> line_clipped(p0_t,p1_t);
                data.line_clipped_ = line_clipped;
              }

              cell->set_data(data);
            }
            else
            {
              boxm_inf_line_sample<AUX> sample(vgl_infinite_line_3d<AUX>(vgl_vector_2d<AUX>(-10000,-10000),vgl_vector_3d<AUX>(0,0,1)));
              cell->set_data(sample);
            }
          }
          else
          {
            boxm_inf_line_sample<AUX> sample(vgl_infinite_line_3d<AUX>(vgl_vector_2d<AUX>(-10000,-10000),vgl_vector_3d<AUX>(0,0,1)));
            cell->set_data(sample);
          }
        }
        else
        {
          boxm_inf_line_sample<AUX> sample(vgl_infinite_line_3d<AUX>(vgl_vector_2d<AUX>(-10000,-10000),vgl_vector_3d<AUX>(0,0,1)));
          cell->set_data(sample);
        }
      }
      else
      {
        boxm_inf_line_sample<AUX> sample(vgl_infinite_line_3d<AUX>(vgl_vector_2d<AUX>(-10000,-10000),vgl_vector_3d<AUX>(0,0,1)));
        cell->set_data(sample);
      }
    }
    nums/=cells.size();

    scene_.write_active_block();
    for (unsigned int i=0; i<aux_readers.size(); ++i) {
      aux_readers[i]->close();
    }
    iter++;
  }
#ifdef DEBUG
  std::cout << "done with all cells" << std::endl;
#endif
#if 0
  // clear the aux scenes so that its starts with the refined scene next time
  for (unsigned i=0; i<aux_scenes.size(); i++) {
    aux_scenes[i].clean_scene();
  }
#endif
  return true;
}

template <class T_loc, class APM, class AUX>
boxm_edge_tangent_refine_updates<T_loc,APM,AUX>::boxm_edge_tangent_refine_updates(boxm_scene<boct_tree<T_loc, boxm_inf_line_sample<APM> > > &scene,
                                                                                  int consensus_cnt,
                                                                                  std::vector<vil_image_view<float> >  edge_images,
                                                                                  std::vector<vpgl_camera_double_sptr>  cameras)
: edge_images_(std::move(edge_images)), cameras_(std::move(cameras)), consensus_cnt_(consensus_cnt), scene_(scene)
{}

template <class T_loc, class APM, class AUX>
bool boxm_edge_tangent_refine_updates<T_loc,APM,AUX>::refine_cells()
{
  std::cout << "using " << edge_images_.size() << " images to refine edge world!" << std::endl;

  typedef boct_tree<T_loc, boxm_inf_line_sample<APM> > tree_type;

  // for each block
  boxm_block_iterator<tree_type> iter(&scene_);
  iter.begin();
  while (!iter.end())
  {
    scene_.load_block(iter.index());
    boxm_block<tree_type>* block = *iter;
    boct_tree<T_loc, boxm_inf_line_sample<APM> >* tree = block->get_tree();
    std::vector<boct_tree_cell<T_loc,boxm_inf_line_sample<APM> >*> cells = tree->leaf_cells();

    // iterate over cells
    for (unsigned i=0; i<cells.size(); ++i)
    {
      boct_tree_cell<T_loc,boxm_inf_line_sample<APM> >* cell = cells[i];
      boxm_inf_line_sample<AUX> cell_value = cell->data();

      // update residual of cell_value based on the visibility and support of this 3D line in all the images
      if (cell_value.residual_<1.0)
      {
        // go through each image
        int cnt = 0;
        for (unsigned k = 0; k < edge_images_.size(); k++) {
          int ni = edge_images_[k].ni();
          int nj = edge_images_[k].nj();
          // project this 3D line to the image
          double u1,v1,u2,v2;
          vgl_point_3d<float>  p1=cell_value.line_clipped_.point1();
          vgl_point_3d<float>  p2=cell_value.line_clipped_.point2();
          cameras_[k]->project(p1.x(),p1.y(),p1.z(),u1,v1);
          cameras_[k]->project(p2.x(),p2.y(),p2.z(),u2,v2);

          double line_angle = vnl_math::angle_0_to_2pi(std::atan2(v2-v1, -(u2-u1)));
          double line_angle2 = vnl_math::angle_0_to_2pi(line_angle + vnl_math::pi); // line segment is symmetric

          // find mid point of line seg
          int cent_i = int(u1+(u2-u1)/2.0);
          int cent_j = int(v1+(v2-v1)/2.0);

          // check a neighborhood of 10x10 in the image to find the edgel that is nearest to this line segment
          double angle_of_min = 0.0;
          double dist = 10.0;
          for (int ii = cent_i - 10; ii < cent_i + 10; ii++) {
            if (ii < 0 || ii >= ni) continue;
            for (int jj = cent_j - 10; jj < cent_j + 10; jj++) {
              if (jj < 0 || jj >= nj) continue;
              double ex = edge_images_[k](ii,jj,0);
              double ey = edge_images_[k](ii,jj,1);
              double dir = edge_images_[k](ii,jj,2);
              double dist_to_seg = vgl_distance_to_linesegment<double>(u1, v1, u2, v2, ex, ey);
              if (dist_to_seg < dist) {
                dist = dist_to_seg;
                angle_of_min = dir;
              }
            }
          }
          if (dist < 10.0)
            if (std::abs(angle_of_min - line_angle) < vnl_math::pi/16.0 || std::abs(angle_of_min - line_angle2) < vnl_math::pi/16.0)
              cnt++;
        }

        if (cnt >= consensus_cnt_)
          cell_value.residual_ = AUX(1.0)-AUX(cnt)/AUX(edge_images_.size());
        else
          cell_value.residual_ = AUX(1.0);
        cell->set_data(cell_value);
      }
    }
    scene_.write_active_block();
    iter++;
  }

  return true;
}


#define BOXM_EDGE_TANGENT_UPDATER_INSTANTIATE(T1,T2,T3) \
template class boxm_edge_tangent_updater<T1,T2,T3 >

#define BOXM_EDGE_TANGENT_REFINE_UPDATES_INSTANTIATE(T1,T2,T3) \
template class boxm_edge_tangent_refine_updates<T1,T2,T3 >

#endif // boxm_edge_tangent_updater_hxx_
