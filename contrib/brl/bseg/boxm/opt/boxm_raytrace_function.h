#ifndef boxm_raytrace_function_h_
#define boxm_raytrace_function_h_
//:
// \file

#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_utils.h>
#include <boxm/boxm_raytrace_operations.h>
#include <boxm/boxm_block_vis_graph_iterator.h>
#include <boxm/opt/boxm_aux_scene.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/algo/vpgl_backproject.h>
#include <vpgl/algo/vpgl_ray.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_line_3d_2_points.h>

#include <vcl_cassert.h>
#include <vcl_iostream.h>

template<class F, class T_loc, class T_data, class T_aux>
class boxm_raytrace_function
{
 public:
    typedef boct_tree<T_loc,T_data> tree_type;
    typedef boct_tree<T_loc,T_aux> aux_tree_type;
    typedef boct_tree_cell<T_loc,T_data> cell_type;
    //: constructor
    boxm_raytrace_function(boxm_scene<tree_type> &scene,
                           vpgl_camera_double_sptr cam,
                           unsigned int ni, unsigned int nj,
                           bool reverse_traversal = false,
                           unsigned int i0 = 0, unsigned int j0 = 0)
    : scene_(scene), aux_scene_(boxm_aux_scene<T_loc,T_data,T_aux>()), reverse_traversal_(reverse_traversal), cam_(cam), img_i0_(i0), img_j0_(j0), img_ni_(ni), img_nj_(nj), debug_lvl_(2) {}
    boxm_raytrace_function(boxm_scene<tree_type> &scene,
                           boxm_aux_scene<T_loc, T_data,  T_aux> &aux_scene,
                           vpgl_camera_double_sptr cam,
                           unsigned int ni, unsigned int nj,
                           bool reverse_traversal = false,
                           unsigned int i0 = 0, unsigned int j0 = 0)
    : scene_(scene), aux_scene_(aux_scene), reverse_traversal_(reverse_traversal), cam_(cam), img_i0_(i0), img_j0_(j0), img_ni_(ni), img_nj_(nj), debug_lvl_(2) {}

    bool run_single_ray(F& step_functor,float i, float j)
    {
        vil_image_view<bool> continue_trace(img_ni_, img_nj_);
        continue_trace.fill(true);
        // code to iterate over the blocks in order of visibility
        boxm_block_vis_graph_iterator<tree_type > block_vis_iter(cam_, &scene_, img_ni_,img_nj_);
        while (block_vis_iter.next())
        {
            vcl_vector<vgl_point_3d<int> > block_indices = block_vis_iter.frontier_indices();
            for (unsigned i=0; i<block_indices.size(); i++) // code for each block
            {
                scene_.load_block(block_indices[i]);
                boxm_block<tree_type> * curr_block=scene_.get_active_block();
                boxm_block<aux_tree_type> * curr_aux_block=NULL;
                if (step_functor.is_aux_)
                    curr_aux_block=aux_scene_.get_block(block_indices[i]);

                if (debug_lvl_ > 0)
                    vcl_cout << "processing block at index (" <<block_indices[i] << ')' << vcl_endl;
                // make sure block projects to inside of image
                vgl_box_3d<double> block_bb = curr_block->bounding_box();

                if (!boxm_utils::is_visible(block_bb,cam_,img_ni_,img_nj_))
                    continue;


                vgl_box_2d<double> img_bb;

                // initialize ray_origin() function for this block
                if (!generate_ray_init(block_bb, img_bb)) {
                    continue;
                }
                tree_type * tree=curr_block->get_tree();
                aux_tree_type * aux_tree=NULL;
                if (step_functor.is_aux_)
                {
                    aux_tree=curr_aux_block->get_tree();
                }

                 // get direction of vector and enter point - this depends on which camera type we have
                        vgl_point_3d<double> ray_origin;
                        vgl_vector_3d<double> direction;
                        vgl_point_3d<double> enter_pt(0.0,0.0,0.0);
                        vcl_vector<vgl_point_3d<double> > plane_intersections(6);
                        // add 0.5 to get center of pixel
                        generate_ray(i , j, block_bb, ray_origin, direction);

                        // compute intersection of each plane with ray
                        double lambda[6];
                        lambda[0] = (block_bb.min_x() - ray_origin.x())/direction.x();
                        lambda[1] = (block_bb.max_x() - ray_origin.x())/direction.x();
                        lambda[2] = (block_bb.min_y() - ray_origin.y())/direction.y();
                        lambda[3] = (block_bb.max_y() - ray_origin.y())/direction.y();
                        lambda[4] = (block_bb.min_z() - ray_origin.z())/direction.z();
                        lambda[5] = (block_bb.max_z() - ray_origin.z())/direction.z();

                        if (block_bb.contains(ray_origin))
                            enter_pt=ray_origin;
                        else
                        {
                            for (unsigned int face=0; face<6; ++face) {
                                plane_intersections[face] = ray_origin + (direction * lambda[face]);
                            }

                            // determine which point is the entrance point based on direction
                            const double epsilon = 1e-6; // use in place of zero to avoid badly conditioned lambdas
                            if ( (plane_intersections[5].x() >= block_bb.min_x()) && (plane_intersections[5].x() <= block_bb.max_x()) &&
                                (plane_intersections[5].y() >= block_bb.min_y()) && (plane_intersections[5].y() <= block_bb.max_y()) &&
                                (direction.z() < -epsilon) )
                            {
                                // ray intersects the zmax plane
                                // check zmax first since it is probably the most common
                                enter_pt = plane_intersections[5];
                            }
                            else if ( (plane_intersections[0].y() >= block_bb.min_y()) && (plane_intersections[0].y() <= block_bb.max_y()) &&
                                (plane_intersections[0].z() >= block_bb.min_z()) && (plane_intersections[0].z() <= block_bb.max_z()) &&
                                (direction.x() > epsilon) )
                            {
                                // ray intersects the xmin plane
                                enter_pt = plane_intersections[0];
                            }
                            else if ( (plane_intersections[1].y() >= block_bb.min_y()) && (plane_intersections[1].y() <= block_bb.max_y()) &&
                                (plane_intersections[1].z() >= block_bb.min_z()) && (plane_intersections[1].z() <= block_bb.max_z()) &&
                                (direction.x() < -epsilon) )
                            {
                                // ray intersects the xmax plane
                                enter_pt = plane_intersections[1];
                            }
                            else if ( (plane_intersections[2].x() >= block_bb.min_x()) && (plane_intersections[2].x() <= block_bb.max_x()) &&
                                (plane_intersections[2].z() >= block_bb.min_z()) && (plane_intersections[2].z() <= block_bb.max_z()) &&
                                (direction.y() > epsilon) )
                            {
                                // ray intersects the ymin plane
                                enter_pt = plane_intersections[2];
                            }
                            else if ( (plane_intersections[3].x() >= block_bb.min_x()) && (plane_intersections[3].x() <= block_bb.max_x()) &&
                                (plane_intersections[3].z() >= block_bb.min_z()) && (plane_intersections[3].z() <= block_bb.max_z()) &&
                                (direction.y() < -epsilon) )
                            {
                                // ray intersects the ymax plane
                                enter_pt = plane_intersections[3];
                            }
                            else if ( (plane_intersections[4].x() >= block_bb.min_x()) && (plane_intersections[4].x() <= block_bb.max_x()) &&
                                (plane_intersections[4].y() >= block_bb.min_y()) && (plane_intersections[4].y() <= block_bb.max_y()) &&
                                (direction.z() > epsilon) )
                            {
                                // ray intersects the zmin plane
                                enter_pt = plane_intersections[4];
                            }
                            else {
                                // no entry point into this block found
                                continue;
                            }
                        }
                        boct_tree_cell<T_loc,T_data > * curr_cell=tree->locate_point_global(enter_pt);
                        while (1)
                        {
                            boct_loc_code<T_loc> cell_code(curr_cell->get_code());
                            
                            boct_tree_cell<T_loc,T_aux > * curr_aux_cell=NULL;
                            if (step_functor.is_aux_)
                                curr_aux_cell=aux_tree->get_cell(cell_code);
                            vgl_box_3d<double> cell_bb = tree->cell_bounding_box(curr_cell);
                            // find exit point of cell
                            vgl_point_3d<double> exit_pt;
                            double lambda=0;
                            boct_face_idx face_id=NONE;
                            bool found_exit =boxm_utils::cube_exit_point(cell_bb,enter_pt,direction, exit_pt,lambda,face_id);
                            if (!found_exit) {
                                vcl_cerr << "error: could not find cell exit point\n"
                                         << "   enter_pt = [" << enter_pt.x() << ", " << enter_pt.y() << ", " << enter_pt.z() << "]\n"
                                         << "   direction = [" << direction.x() << ", " << direction.y() << ", " << direction.z() << "]\n"
                                         << "   cell_bb = [" << cell_bb.min_x() <<", " << cell_bb.max_x() << "]  [" << cell_bb.min_y()
                                         << ", " << cell_bb.max_y() << "]  [" << cell_bb.min_z() << ", " << cell_bb.max_z() <<"]\n";
                                assert(found_exit);
                                break;
                            }
                            T_data cell_val=curr_cell->data();
                            T_aux aux_val;
                            if (step_functor.is_aux_)
                                aux_val=curr_aux_cell->data();
                            continue_trace(i-img_i0_, j-img_j0_) = step_functor.step_cell(enter_pt, exit_pt, cell_val,aux_val);
                            curr_cell->set_data(cell_val);
                            if (step_functor.is_aux_)
                                curr_aux_cell->set_data(aux_val);
                            //: normalize the point to [0-1]to obtain the index.
                            vgl_point_3d<double> exit_pt_norm((exit_pt.x()-block_bb.min_x())/block_bb.width(),
                                                              (exit_pt.y()-block_bb.min_y())/block_bb.height(),
                                                              (exit_pt.z()-block_bb.min_z())/block_bb.depth());
                            //: obtian the code for the exit point
                            boct_loc_code<T_loc> exit_loc_code(exit_pt_norm,tree->root_level());
                            cell_type *neighborcell=NULL;
                            if (curr_cell->find_neighbor(face_id,neighborcell,tree->root_level()))
                                curr_cell=neighborcell->traverse_force(exit_loc_code);
                            else
                            {
                                vcl_cout<<"NO NEIGBORS FOUND!"<<vcl_endl;
                                break;
                            }
                            enter_pt=exit_pt;
                        }
                if (!step_functor.scene_read_only_)
                    scene_.write_active_block();
                if (step_functor.is_aux_)
                    aux_scene_.write_active_block();
            }
        }
        return true;
    
    }

    bool run(F& step_functor)
    {
        vil_image_view<bool> continue_trace(img_ni_, img_nj_);
        continue_trace.fill(true);
        // code to iterate over the blocks in order of visibility
        boxm_block_vis_graph_iterator<tree_type > block_vis_iter(cam_, &scene_, img_ni_,img_nj_);
        while (block_vis_iter.next())
        {
            vcl_vector<vgl_point_3d<int> > block_indices = block_vis_iter.frontier_indices();
            for (unsigned i=0; i<block_indices.size(); i++) // code for each block
            {
                scene_.load_block(block_indices[i]);
                boxm_block<tree_type> * curr_block=scene_.get_active_block();

                if (debug_lvl_ > 0)
                    vcl_cout << "processing block at index (" <<block_indices[i] << ')' << vcl_endl;
                // make sure block projects to inside of image
                vgl_box_3d<double> block_bb = curr_block->bounding_box();

                if (!boxm_utils::is_visible(block_bb,cam_,img_ni_,img_nj_))
                    continue;
                vgl_box_2d<double> img_bb;
                // initialize ray_origin() function for this block
                if (!generate_ray_init(block_bb, img_bb)) {
                    continue;
                }

                tree_type * tree=curr_block->get_tree();

                boxm_block<aux_tree_type> * curr_aux_block=NULL;
                aux_tree_type * aux_tree=NULL;
                if (step_functor.is_aux_)
                {
                   curr_aux_block=aux_scene_.get_block(block_indices[i]);
                   aux_tree=curr_aux_block->get_tree();
                }
                // for each image pixel
                for (unsigned int i = int(img_bb.min_x()+0.99); i <= img_bb.max_x(); ++i)
                {
                    if (debug_lvl_ > 1) {
                        if (!(i % 10))
                            vcl_cout << '.';
                    }
                    for (unsigned int j = int(img_bb.min_y()+0.99); j <= img_bb.max_y(); ++j)
                    {
                        if (!continue_trace(i - img_i0_ , j - img_j0_)) {
                            continue;
                        }
                        // get direction of vector and enter point - this depends on which camera type we have
                        vgl_point_3d<double> ray_origin;
                        vgl_vector_3d<double>direction;
                        vgl_point_3d<double> enter_pt(0.0,0.0,0.0);
                        vgl_point_3d<double> exit_pt;
                        // add 0.5 to get center of pixel
                        generate_ray(i + 0.5f, j + 0.5f, block_bb, ray_origin, direction);
                        if(!entry_point(block_bb,ray_origin,direction,enter_pt))
                            continue;
                        // normalize the entry point to [0,1]
                        vgl_point_3d<double> enter_pt_norm((enter_pt.x()-block_bb.min_x())/block_bb.width(),
                                                           (enter_pt.y()-block_bb.min_y())/block_bb.height(),
                                                           (enter_pt.z()-block_bb.min_z())/block_bb.depth());
                        boct_loc_code<T_loc> curr_cell_code(enter_pt_norm,tree->root_level());
                        boct_tree_cell<T_loc,T_data > * curr_cell=tree->root()->traverse_force(curr_cell_code);

                        while (continue_trace(i-img_i0_, j-img_j0_))
                        {
                            boct_tree_cell<T_loc,T_aux > * curr_aux_cell=NULL;
                            if (step_functor.is_aux_)
                            {
                                boct_loc_code<T_loc> cell_code(curr_cell->get_code());
                                curr_aux_cell=aux_tree->get_cell(cell_code);
                            }
                            //: get the canonical bounding box
                            vgl_box_3d<double> cell_bb = tree->cell_bounding_box(curr_cell);
                            // find exit point of cell

                            double lambda=0;
                            boct_face_idx face_id=NONE;
                            bool found_exit =boxm_utils::cube_exit_point(cell_bb,enter_pt,direction, exit_pt,lambda,face_id);
                            if (!found_exit) {
                                vcl_cerr << "error: could not find cell exit point\n"
                                         << "   enter_pt = [" << enter_pt.x() << ", " << enter_pt.y() << ", " << enter_pt.z() << "]\n"
                                         << "   direction = [" << direction.x() << ", " << direction.y() << ", " << direction.z() << "]\n"
                                         << "   cell_bb = [" << cell_bb.min_x() <<", " << cell_bb.max_x() << "]  [" << cell_bb.min_y()
                                         << ", " << cell_bb.max_y() << "]  [" << cell_bb.min_z() << ", " << cell_bb.max_z() <<"]\n";
                                assert(found_exit);
                                break;
                            }
                            T_data cell_val=curr_cell->data();
                            T_aux aux_val;

                            if (step_functor.is_aux_)
                                aux_val=curr_aux_cell->data();


                            continue_trace(i-img_i0_, j-img_j0_) = step_functor.step_cell(i,j, enter_pt, exit_pt, cell_val,aux_val);
                            curr_cell->set_data(cell_val);
                            if (step_functor.is_aux_)
                                curr_aux_cell->set_data(aux_val);
                             vgl_point_3d<double> exit_pt_norm((exit_pt.x()-block_bb.min_x())/block_bb.width(),
                                                               (exit_pt.y()-block_bb.min_y())/block_bb.height(),
                                                              (exit_pt.z()-block_bb.min_z())/block_bb.depth());
 
                            //: obtian the code for the exit point
                            boct_loc_code<T_loc> exit_loc_code(exit_pt_norm,tree->root_level(), tree->max_val());
                            cell_type *neighborcell=NULL;
                            if (curr_cell->find_neighbor(face_id,neighborcell,tree->root_level()))
                                curr_cell=neighborcell->traverse_force(exit_loc_code);
                            else
                                break;
                            enter_pt=exit_pt;
                        }
                    }
                }
                if (!step_functor.scene_read_only_)
                    scene_.write_active_block();
                if (step_functor.is_aux_)
                    aux_scene_.write_active_block();
            }
        }
        return true;
    }

 protected:

    bool generate_ray_init(vgl_box_3d<double> const& block_bb, vgl_box_2d<double> &img_bb)
    {
        // determine intersection of block bounding box projection and image bounds
        vgl_box_2d<double> img_bounds;
        img_bounds.add(vgl_point_2d<double>(img_i0_,img_j0_));
        img_bounds.add(vgl_point_2d<double>(img_i0_ + img_ni_ - 1, img_j0_ + img_nj_ - 1));

        vgl_box_2d<double> block_projection;
        double u,v;
        cam_->project(block_bb.min_x(),block_bb.min_y(),block_bb.min_z(),u,v);
        block_projection.add(vgl_point_2d<double>(u,v));
        cam_->project(block_bb.min_x(),block_bb.min_y(),block_bb.max_z(),u,v);
        block_projection.add(vgl_point_2d<double>(u,v));
        cam_->project(block_bb.min_x(),block_bb.max_y(),block_bb.min_z(),u,v);
        block_projection.add(vgl_point_2d<double>(u,v));
        cam_->project(block_bb.min_x(),block_bb.max_y(),block_bb.max_z(),u,v);
        block_projection.add(vgl_point_2d<double>(u,v));
        cam_->project(block_bb.max_x(),block_bb.min_y(),block_bb.min_z(),u,v);
        block_projection.add(vgl_point_2d<double>(u,v));
        cam_->project(block_bb.max_x(),block_bb.min_y(),block_bb.max_z(),u,v);
        block_projection.add(vgl_point_2d<double>(u,v));
        cam_->project(block_bb.max_x(),block_bb.max_y(),block_bb.min_z(),u,v);
        block_projection.add(vgl_point_2d<double>(u,v));
        cam_->project(block_bb.max_x(),block_bb.max_y(),block_bb.max_z(),u,v);
        block_projection.add(vgl_point_2d<double>(u,v));

        img_bb=vgl_intersection(img_bounds,block_projection);

        if (img_bb.is_empty())
            return false;
        else
            return true;
    }

    void generate_ray(float i, float j, vgl_box_3d<double> const& block_bb, vgl_point_3d<double> &ray_origin, vgl_vector_3d<double> &norm_direction)
    {
      if(cam_->type_name() == "vpgl_perspective_camera"){
        vpgl_perspective_camera<double> const* pcam = 
          static_cast<vpgl_perspective_camera<double> const*>(cam_.ptr());
        // backproject image point to a ray
        ray_origin = pcam->camera_center();
        vgl_line_3d_2_points<double> cam_ray = pcam->backproject(vgl_homg_point_2d<double>((double)i,(double)j));
        norm_direction=cam_ray.direction();
        normalize(norm_direction);
        return;
      }
      if(cam_->type_name() == "vpgl_local_rational_camera"){
        vpgl_local_rational_camera<double> const* lrcam = 
          static_cast<vpgl_local_rational_camera<double> const*>(cam_.ptr());
        vpgl_ray::ray(*lrcam,(double)i, (double)j, ray_origin, norm_direction);
        return;
      }else{
        vcl_cerr << "In boxm_raytrace_function: camera type not handled\n";
        assert(false);
      }
    }
    bool entry_point(vgl_box_3d<double> & block_bb, vgl_point_3d<double>  ray_origin, 
        vgl_vector_3d<double> direction, vgl_point_3d<double> & enter_pt){
            double lambda[6];
            vcl_vector<vgl_point_3d<double> > plane_intersections(6);

            lambda[0] = (block_bb.min_x() - ray_origin.x())/direction.x();
            lambda[1] = (block_bb.max_x() - ray_origin.x())/direction.x();
            lambda[2] = (block_bb.min_y() - ray_origin.y())/direction.y();
            lambda[3] = (block_bb.max_y() - ray_origin.y())/direction.y();
            lambda[4] = (block_bb.min_z() - ray_origin.z())/direction.z();
            lambda[5] = (block_bb.max_z() - ray_origin.z())/direction.z();

            if (block_bb.contains(ray_origin))
                enter_pt=ray_origin;
            else
            {
                for (unsigned int face=0; face<6; ++face) {
                    plane_intersections[face] = ray_origin + (direction * lambda[face]);
                }

                // determine which point is the entrance point based on direction
                const double epsilon = 1e-6; // use in place of zero to avoid badly conditioned lambdas
                if ( (plane_intersections[5].x() >= block_bb.min_x()) && (plane_intersections[5].x() <= block_bb.max_x()) &&
                    (plane_intersections[5].y() >= block_bb.min_y()) && (plane_intersections[5].y() <= block_bb.max_y()) &&
                    (direction.z() < -epsilon) )
                {
                    // ray intersects the zmax plane
                    // check zmax first since it is probably the most common
                    enter_pt = plane_intersections[5];
                }
                else if ( (plane_intersections[0].y() >= block_bb.min_y()) && (plane_intersections[0].y() <= block_bb.max_y()) &&
                    (plane_intersections[0].z() >= block_bb.min_z()) && (plane_intersections[0].z() <= block_bb.max_z()) &&
                    (direction.x() > epsilon) )
                {
                    // ray intersects the xmin plane
                    enter_pt = plane_intersections[0];
                }
                else if ( (plane_intersections[1].y() >= block_bb.min_y()) && (plane_intersections[1].y() <= block_bb.max_y()) &&
                    (plane_intersections[1].z() >= block_bb.min_z()) && (plane_intersections[1].z() <= block_bb.max_z()) &&
                    (direction.x() < -epsilon) )
                {
                    // ray intersects the xmax plane
                    enter_pt = plane_intersections[1];
                }
                else if ( (plane_intersections[2].x() >= block_bb.min_x()) && (plane_intersections[2].x() <= block_bb.max_x()) &&
                    (plane_intersections[2].z() >= block_bb.min_z()) && (plane_intersections[2].z() <= block_bb.max_z()) &&
                    (direction.y() > epsilon) )
                {
                    // ray intersects the ymin plane
                    enter_pt = plane_intersections[2];
                }
                else if ( (plane_intersections[3].x() >= block_bb.min_x()) && (plane_intersections[3].x() <= block_bb.max_x()) &&
                    (plane_intersections[3].z() >= block_bb.min_z()) && (plane_intersections[3].z() <= block_bb.max_z()) &&
                    (direction.y() < -epsilon) )
                {
                    // ray intersects the ymax plane
                    enter_pt = plane_intersections[3];
                }
                else if ( (plane_intersections[4].x() >= block_bb.min_x()) && (plane_intersections[4].x() <= block_bb.max_x()) &&
                    (plane_intersections[4].y() >= block_bb.min_y()) && (plane_intersections[4].y() <= block_bb.max_y()) &&
                    (direction.z() > epsilon) )
                {
                    // ray intersects the zmin plane
                    enter_pt = plane_intersections[4];
                }
                else {
                    // no entry point into this block found
                    return false;;
                }
            }
            return true;
    }

    boxm_scene<tree_type> &scene_;
    boxm_aux_scene<T_loc,  T_data,  T_aux> &aux_scene_;

    bool reverse_traversal_;

    const vpgl_camera_double_sptr cam_;
    unsigned int img_i0_;
    unsigned int img_j0_;
    unsigned int img_ni_;
    unsigned int img_nj_;

    unsigned int debug_lvl_;
};

template<class F, class T_loc, class T_data, class T_aux>
class boxm_iterate_cells_function
{
 public:
    typedef boct_tree<T_loc,T_data> tree_type;
    typedef boct_tree<T_loc,T_aux> aux_tree_type;
    typedef boct_tree_cell<T_loc,T_data> cell_type;
    typedef boct_tree_cell<T_loc,T_aux> aux_cell_type;
    //: constructor
    boxm_iterate_cells_function(boxm_scene<tree_type> &scene,
                                boxm_aux_scene<T_loc,  T_data,  T_aux> &aux_scene,
                                vpgl_camera_double_sptr cam,
                                unsigned int ni, unsigned int nj )
    : scene_(scene), aux_scene_(aux_scene), img_ni_(ni), img_nj_(nj), cam_(cam)
    {}


    bool run(F& step_functor)
    {
        // code to iterate over the blocks in order of visibility
        boxm_block_vis_graph_iterator<tree_type > block_vis_iter(cam_, &scene_, img_ni_,img_nj_);
        while (block_vis_iter.next())
        {
            vcl_vector<vgl_point_3d<int> > block_indices = block_vis_iter.frontier_indices();
            for (unsigned i=0; i<block_indices.size(); i++) // code for each block
            {
                scene_.load_block(block_indices[i]);
                boxm_block<tree_type> * curr_block=scene_.get_active_block();
                boxm_block<aux_tree_type> * curr_aux_block=aux_scene_.get_block(block_indices[i]);
                tree_type * tree=curr_block->get_tree();
                aux_tree_type * aux_tree=curr_aux_block->get_tree();

                vcl_vector<cell_type*> leaves=tree->leaf_cells();
                for (unsigned i=0;i<leaves.size();i++)
                {
                    T_data cell_val=leaves[i]->data();
                    boct_loc_code<T_loc> cell_code(leaves[i]->get_code());
                    aux_cell_type * aux_cell=aux_tree->get_cell(cell_code);
                    T_aux aux_val=aux_cell->data();
                    step_functor.step_cell(cell_val,aux_val);
                    leaves[i]->set_data(cell_val);
                    aux_cell->set_data(aux_val);
                }
                scene_.write_active_block();
                aux_scene_.write_active_block();
            }
        }
        return true;
    }

 protected:
    boxm_scene<tree_type> &scene_;
    boxm_aux_scene<T_loc,  T_data,  T_aux> &aux_scene_;
    bool reverse_traversal_;

    unsigned img_ni_;
    unsigned img_nj_;

    const vpgl_camera_double_sptr cam_;
};

#endif // boxm_raytrace_function_h_
