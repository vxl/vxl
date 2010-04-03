#ifndef boxm_edge_tangent_updater_txx_
#define boxm_edge_tangent_updater_txx_

#include "boxm_edge_tangent_updater.h"

#include <boxm/boxm_apm_traits.h>
#include <boxm/opt/boxm_aux_traits.h>
#include <boxm/boxm_scene.h>
#include <boxm/opt/boxm_aux_scene.h>
#include <boxm/boxm_edge_tangent_sample.h>
#include <boxm/boxm_inf_line_sample.h>
#include <boxm/boxm_plane_obs.h>
#include <boxm/algo/boxm_plane_ransac.h>

#include <vgl/vgl_infinite_line_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_intersection.h>
#include <vgl/algo/vgl_intersection.h>

#include <vcl_vector.h>
#include <vcl_string.h>

template <class T_loc, class APM, class AUX>
boxm_edge_tangent_updater<T_loc,APM,AUX>::boxm_edge_tangent_updater(boxm_scene<boct_tree<T_loc,
                                                                    boxm_inf_line_sample<APM> > > &scene,
                                                                    vcl_vector<vcl_string> const& image_ids)
: image_ids_(image_ids), scene_(scene)
{}


template <class T_loc, class APM, class AUX>
bool boxm_edge_tangent_updater<T_loc,APM,AUX>::add_cells()
{
  // get auxiliary scenes associated with each input image


  typedef boct_tree<T_loc, boxm_inf_line_sample<APM> > tree_type;
  typedef boct_tree<T_loc, boxm_edge_tangent_sample<AUX> > aux_tree_type;

  vcl_vector<boxm_aux_scene<T_loc,  boxm_inf_line_sample<APM>, boxm_edge_tangent_sample<AUX> > > aux_scenes;
  for (unsigned int i=0; i<image_ids_.size(); ++i) {
    boxm_aux_scene<T_loc, boxm_inf_line_sample<APM>, boxm_edge_tangent_sample<AUX> > 
    aux_scene(&scene_,image_ids_[i],boxm_aux_scene<T_loc, boxm_inf_line_sample<APM>, 
    boxm_edge_tangent_sample<AUX> >::LOAD, BOXM_EDGE_TANGENT_LINE);
    aux_scenes.push_back(aux_scene);
  }

  vcl_vector<boxm_edge_tangent_sample<APM> > aux_samples;

  // for each block
  boxm_block_iterator<tree_type> iter(&scene_);
  iter.begin();
  while (!iter.end())
  {
    scene_.load_block(iter.index());
    boxm_block<tree_type>* block = *iter;
    boct_tree<T_loc, boxm_inf_line_sample<APM> >* tree = block->get_tree();
    vcl_vector<boct_tree_cell<T_loc,boxm_inf_line_sample<APM> >*> cells = tree->leaf_cells();
    int nums=0;
    // get a vector of incremental readers for each aux scene.
    vcl_vector<boct_tree_cell_reader<T_loc, boxm_edge_tangent_sample<AUX> > *> aux_readers(aux_scenes.size());
    vcl_vector<unsigned> aux_samples_num(aux_scenes.size());
    for (unsigned int i=0; i<aux_scenes.size(); ++i) {
      aux_readers[i] = aux_scenes[i].get_block_incremental(iter.index());
      aux_samples_num[i] = 0;
    }
    // iterate over cells
    for (unsigned i=0; i<cells.size(); ++i)
    {
      aux_samples.clear();
      boct_tree_cell<T_loc,boxm_inf_line_sample<APM> >* cell = cells[i];

      for (unsigned j=0; j<aux_readers.size(); j++) {
        boct_tree_cell<T_loc, boxm_edge_tangent_sample<AUX> > temp_cell;

        if (!aux_readers[j]->next(temp_cell)) {
          vcl_cerr << "error: incremental reader returned false.\n";
          return false;
        }

        if (!temp_cell.code_.isequal(&(cell->code_))) {
          vcl_cerr << "error: temp_cell idx does not match cell idx.\n";
          return false;
        }
        if(temp_cell.data().num_obs()>0)
        {
            aux_samples.push_back(temp_cell.data());
            //aux_samples_num[j]++;
        }
      }
      unsigned num=0;


      if (aux_samples.size() > 1) {

        vcl_vector<vgl_plane_3d<AUX> > planes;
        vcl_vector<AUX> weights;

        for (unsigned int i=0; i<aux_samples.size(); ++i) {
          boxm_edge_tangent_sample<APM> s = aux_samples[i];
          for (unsigned int j=0; j<s.num_obs(); j++) {
            boxm_plane_obs<AUX> obs = s.obs(j);
            weights.push_back(obs.seg_len_);
            vgl_plane_3d<AUX> plane(obs.plane_);
            planes.push_back(plane);
          }
        }
        nums+=planes.size();
        if (planes.size() > 1) {
          float residual=0;
          ////////////////
          vcl_list<vgl_plane_3d<AUX> > fit_planes;
          vcl_vector<AUX> fit_weights;
          vcl_vector<unsigned> indices;
          boxm_plane_ransac<AUX>(planes, indices, planes.size()); 
          
          for (unsigned i=0; i<indices.size(); i++) {
            unsigned idx = indices[i];
            fit_planes.push_back(planes[idx]);
            fit_weights.push_back(weights[idx]);
          }
          ////////////////
          vgl_infinite_line_3d<AUX> line = vgl_intersection(fit_planes, fit_weights,residual);
          boxm_inf_line_sample<AUX> data(line,aux_samples.size());
          data.residual_=residual;
          vgl_box_3d<double> bb = tree->cell_bounding_box(cell);
          // convert to line type (from float to double)
          vgl_vector_2d<double> x0(line.x0().x(), line.x0().y());
          vgl_vector_3d<double> dir(line.direction().x(), line.direction().y(), line.direction().z());
          vgl_infinite_line_3d<double> dline(x0,dir);
          vgl_point_3d<double> p0,p1;

          if (vgl_intersection<double>(bb, dline, p0, p1)) 
          {
            data.line_clipped_=vgl_line_3d_2_points<float>(vgl_point_3d<float>(p0.x(),p0.y(),p0.z()),vgl_point_3d<float>(p1.x(),p1.y(),p0.z()));
          }
          else
          {
             data.num_obs_=0;
          }
          cell->set_data(data);
        }
        else
          cell->set_data(vgl_infinite_line_3d<AUX>(vgl_vector_2d<AUX>(0,0),vgl_vector_3d<AUX>(10,10,10)));
      }
      else
        cell->set_data(vgl_infinite_line_3d<AUX>(vgl_vector_2d<AUX>(0,0),vgl_vector_3d<AUX>(10,10,10)));
    }
    nums/=cells.size();


    scene_.write_active_block();
    for (unsigned int i=0; i<aux_readers.size(); ++i) {
      aux_readers[i]->close();
    }
    iter++;
 }
#ifdef DEBUG
  vcl_cout << "done with all cells" << vcl_endl;
#endif
  // clear the aux scenes so that its starts with the refined scene next time
  for (unsigned i=0; i<aux_scenes.size(); i++) {
    aux_scenes[i].clean_scene();
  }

  return true;
}


#define BOXM_EDGE_TANGENT_UPDATER_INSTANTIATE(T1,T2,T3) \
template class boxm_edge_tangent_updater<T1,T2,T3 >

#endif // boxm_edge_tangent_updater_txx_
