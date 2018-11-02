#ifndef boxm_compute_volume_visibility_h
#define boxm_compute_volume_visibility_h
#include <iostream>
#include <vgl/vgl_box_3d.h>
#include <boxm/boxm_scene.h>
#include <boxm/util/boxm_utils.h>
#include <vpgl/vpgl_perspective_camera.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T_loc, class T_data>
float boxm_compute_point_visibility(vgl_point_3d<double> point,
                                    boxm_scene<boct_tree<T_loc, T_data > > & scene,
                                    vpgl_camera_double_sptr & camera )
{
  typedef boct_tree<T_loc,T_data > tree_type;
  // locate the cells corresponding to query volume.
  if (camera->type_name().compare("vpgl_perspective_camera")==0) {
    // make a test for vertices for behind-front case
    vpgl_perspective_camera<double>* cam = static_cast<vpgl_perspective_camera<double>*>(camera.ptr());

    std::cout<<"The point is "<<point;
    if (cam->is_behind_camera(vgl_homg_point_3d<double>(point)))
    return 1.0f;
    // first block of intersection

    // case 1 : when the camera lies inside the scene
    vgl_homg_point_3d<double> cam_center_homg = cam->camera_center();
    vgl_point_3d<double> cam_center(cam_center_homg);
    vgl_point_3d<int> curr_block_index;

    vgl_vector_3d<double> dir=(point-cam_center);
    double lambda0=dir.length();
    dir/=lambda0;

    boxm_block<tree_type> * curr_block=scene.get_block(cam_center);
    vgl_point_3d<double> entry_point=cam_center;
    double lambda=0;

    // case 2: camera lies outside the scene
    if (!curr_block)
    {
      vgl_box_3d<double> world_bb=scene.get_world_bbox();
      boct_face_idx face_id;
      if (!boxm_utils::cube_entry_point(world_bb,cam_center,dir,entry_point,lambda,face_id))
      return 1.0f;
      if (!scene.get_block_index(entry_point,curr_block_index))
      return 1.0f;
    }
    else
    {
      scene.get_block_index(cam_center,curr_block_index);
    }

    scene.load_block(curr_block_index);
    curr_block=scene.get_active_block();
    bool continue_flag=true;

    tree_type * tree=curr_block->get_tree();

    vgl_point_3d<double> exit_point;
    boct_face_idx face_id=NONE;
    double alpha_int=0;
    boct_tree_cell<T_loc,T_data > * curr_cell=tree->locate_point_global(entry_point);
    while (continue_flag)
    {
      // do processing in the block
      if (curr_cell)
      {
        if (boxm_utils::cube_exit_point(tree->cell_bounding_box(curr_cell),
        cam_center,dir,exit_point,lambda,face_id))
        {
          if (lambda>lambda0)
          continue_flag=false;
          else
          {
            std::vector<boct_tree_cell<T_loc,T_data > * > neighbors;
            curr_cell->find_neighbors(face_id,neighbors,tree->root_level());

            vgl_vector_3d<double> len(exit_point-entry_point);
            T_data data=curr_cell->data();
            alpha_int-=data.alpha*len.length();
            curr_cell=nullptr;
#if 0
            std::cout<<"Lambda= "<<lambda<<" lambda0= "<<lambda0
                    <<" exit_point="<<exit_point<<"# of neighbors"<< neighbors.size()<<std::endl;
#endif
            double min_dist=1e5;
            int min_i=-1;
            for (unsigned i=0;i<neighbors.size();++i)
            {
              vgl_box_3d<double> bbox=tree->cell_bounding_box(neighbors[i]);
              double dist=0;
              if (exit_point.x()<bbox.min_x())
                dist+=std::fabs(exit_point.x()-bbox.min_x());
              if (exit_point.x()>bbox.max_x())
                dist+=std::fabs(exit_point.x()-bbox.max_x());
              if (exit_point.y()<bbox.min_y())
                dist+=std::fabs(exit_point.y()-bbox.min_y());
              if (exit_point.y()>bbox.max_y())
                dist+=std::fabs(exit_point.y()-bbox.max_y());
              if (exit_point.z()<bbox.min_z())
                dist+=std::fabs(exit_point.z()-bbox.min_z());
              if (exit_point.z()>bbox.max_z())
                dist+=std::fabs(exit_point.z()-bbox.max_z());

              if (dist<min_dist)
              {
                min_dist=dist;
                min_i=i;
              }
            }
            if (min_i>=0)
              curr_cell=neighbors[min_i];
            else if (neighbors.size()>0)
            {
              std::cout<<"ERROR"<<std::endl;return -1.0f;
            }
            entry_point=exit_point;
          }
        }
      }
      // when the block is exhausted obtain a new block
      else
      {
        if (face_id==X_LOW)
          curr_block_index.set(curr_block_index.x()-1,curr_block_index.y(),curr_block_index.z());
        if (face_id==X_HIGH)
          curr_block_index.set(curr_block_index.x()+1,curr_block_index.y(),curr_block_index.z());
        if (face_id==Y_LOW)
          curr_block_index.set(curr_block_index.x(),curr_block_index.y()-1,curr_block_index.z());
        if (face_id==Y_HIGH)
          curr_block_index.set(curr_block_index.x(),curr_block_index.y()+1,curr_block_index.z());
        if (face_id==Z_LOW)
          curr_block_index.set(curr_block_index.x(),curr_block_index.y(),curr_block_index.z()-1);
        if (face_id==Z_HIGH)
          curr_block_index.set(curr_block_index.x(),curr_block_index.y(),curr_block_index.z()+1);

        if (scene.valid_index(curr_block_index))
        {
          scene.load_block(curr_block_index);
          curr_block=scene.get_active_block();
          tree=curr_block->get_tree();
          curr_cell=tree->locate_point_global(entry_point);
        }
        else
        {
          continue_flag=false;
        }
      }
    }
    return (float)std::exp(alpha_int);
  }
  else
  {
  std::cout<<"Not a perspective camera"<<std::endl;
  return -1.0f;
  }
}


template <class T_loc, class T_data>
float boxm_compute_volume_visibility(vgl_box_3d<double> bbox,
                                     boxm_scene<boct_tree<T_loc,T_data> > & scene,
                                     vpgl_camera_double_sptr & cam )
{
  // locate the cells corresponding to query volume.
  return boxm_compute_point_visibility<T_loc,T_data>(bbox.centroid(),scene,cam);
}

#endif
