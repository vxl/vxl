#include <iostream>
#include <queue>
#include <boxm2/boxm2_blocks_vis_graph.h>
#include <vgl/vgl_intersection.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

boxm2_block_vis_graph::boxm2_block_vis_graph(std::map<boxm2_block_id,boxm2_block_metadata> & blkmetadata, vpgl_generic_camera<double> const& cam ): nodes_()
{
    // bounding box representation of image point samples
    vgl_box_2d<double> image_bounds(vgl_point_2d<double>(0,0),
                                    vgl_point_2d<double>(cam.cols()-1, cam.rows()-1));

    std::list<boxm2_block_vis_graph_node_sptr> temp_nodes;
    // create a node for every block in the model
    for(auto & iter : blkmetadata)
    {
        vgl_box_3d<double> bbox = iter.second.bbox();

        // project bounding box and check for any overlap with the image bounds
        vgl_box_2d<double> projected_bbox;
        const std::vector<vgl_point_3d<double> > block_corners = bbox.vertices();
        for (const auto & block_corner : block_corners) {
          double u,v;
          cam.project( block_corner.x(), block_corner.y(), block_corner.z(), u, v);
          projected_bbox.add( vgl_point_2d<double>(u,v) );
        }
        vgl_box_2d<double> cropped_projection = vgl_intersection(projected_bbox, image_bounds);
          // no need to consider blocks whose projections have no overlap with the image
        if (!cropped_projection.is_empty() ) {
          boxm2_block_vis_graph_node_sptr node(new boxm2_block_vis_graph_node(iter.first));
          temp_nodes.push_back(node);
        }
    }


    // loop over each vis graph node and create necessary edges
    // each edge A->B represents the constraint that block A must be processed before
    // block B, i.e. block A is "in front of" block B from the perspective of cam
    for(auto node_iter = temp_nodes.begin();
        node_iter != temp_nodes.end();node_iter++)
    {
      boxm2_block_id this_block_id = (*node_iter)->id_;
      boxm2_block_metadata md = blkmetadata[this_block_id];
      vgl_box_3d<double> block_bbox = md.bbox();

      // project each face of the block into the image and get the nominal ray direction for that face.
      // lots of code duplication between the X,Y,and Z cases; maybe somebody more clever can figure out how to eliminate it.
      // -X and +X faces
      for (unsigned xi =0; xi<2; ++xi) {
        vgl_box_3d<double> block_face;
        double x = xi == 0? block_bbox.min_x() : block_bbox.max_x();
        double u,v;
        vgl_box_2d<double> face_projection;
        cam.project(x, block_bbox.min_y(), block_bbox.min_z(), u, v);
        face_projection.add(vgl_point_2d<double>(u,v));
        cam.project(x, block_bbox.min_y(), block_bbox.max_z(), u, v);
        face_projection.add(vgl_point_2d<double>(u,v));
        cam.project(x, block_bbox.max_y(), block_bbox.min_z(), u, v);
        face_projection.add(vgl_point_2d<double>(u,v));
        cam.project(x, block_bbox.max_y(), block_bbox.max_z(), u, v);
        face_projection.add(vgl_point_2d<double>(u,v));

        vgl_box_2d<double> cropped_face_projection = vgl_intersection(face_projection, image_bounds);

        if (!cropped_face_projection.is_empty()) {
          vgl_point_2d<double> face_proj_center = cropped_face_projection.centroid();
          vgl_ray_3d<double> ray = cam.ray(face_proj_center.x(), face_proj_center.y());
          bool this_block_in_front = (xi == 0)? ray.direction().x() < 0.0 : ray.direction().x() > 0.0;
          if(this_block_in_front) {
            // this block is in front of block on other side of face
            boxm2_block_id other_block_id = this_block_id;
            if (xi == 0) {
              --other_block_id.i_;
            }
            else {
              ++other_block_id.i_;
            }
            // check for other block in list of nodes
            auto other =
              this->find(other_block_id, temp_nodes) ;
            if ( other != temp_nodes.end() )
            {
              // found the other block, so add the edge
              (*node_iter)->out_edges.insert((*other));
              ++(*other)->num_incoming_edges_;
            }
          }
        }
      }
      // -Y and +Y faces
      for (unsigned yi =0; yi<2; ++yi) {
        vgl_box_3d<double> block_face;
        double y = yi == 0? block_bbox.min_y() : block_bbox.max_y();
        double u,v;
        vgl_box_2d<double> face_projection;
        cam.project(block_bbox.min_x(), y, block_bbox.min_z(), u, v);
        face_projection.add(vgl_point_2d<double>(u,v));
        cam.project(block_bbox.min_x(), y, block_bbox.max_z(), u, v);
        face_projection.add(vgl_point_2d<double>(u,v));
        cam.project(block_bbox.max_x(), y, block_bbox.min_z(), u, v);
        face_projection.add(vgl_point_2d<double>(u,v));
        cam.project(block_bbox.max_x(), y, block_bbox.max_z(), u, v);
        face_projection.add(vgl_point_2d<double>(u,v));

        vgl_box_2d<double> cropped_face_projection = vgl_intersection(face_projection, image_bounds);

        if (!cropped_face_projection.is_empty()) {
          vgl_point_2d<double> face_proj_center = cropped_face_projection.centroid();
          vgl_ray_3d<double> ray = cam.ray(face_proj_center.x(), face_proj_center.y());
          bool this_block_in_front = (yi == 0)? ray.direction().y() < 0.0 : ray.direction().y() > 0.0;
          if(this_block_in_front) {
            // this block is in front of block on other side of face
            boxm2_block_id other_block_id = this_block_id;
            if (yi == 0) {
              --other_block_id.j_;
            }
            else {
              ++other_block_id.j_;
            }
            // check for other block in list of nodes
            auto other =
              this->find(other_block_id, temp_nodes) ;
            if ( other != temp_nodes.end() )
            {
              // found the other block, so add the edge
              (*node_iter)->out_edges.insert((*other));
              ++(*other)->num_incoming_edges_;
            }
          }
        }
      }
      // -Z and +Z faces
      for (unsigned zi =0; zi<2; ++zi) {
        vgl_box_3d<double> block_face;
        double z = zi == 0? block_bbox.min_z() : block_bbox.max_z();
        double u,v;
        vgl_box_2d<double> face_projection;
        cam.project(block_bbox.min_x(), block_bbox.min_y(), z, u, v);
        face_projection.add(vgl_point_2d<double>(u,v));
        cam.project(block_bbox.min_x(), block_bbox.max_y(), z, u, v);
        face_projection.add(vgl_point_2d<double>(u,v));
        cam.project(block_bbox.max_x(), block_bbox.min_y(), z, u, v);
        face_projection.add(vgl_point_2d<double>(u,v));
        cam.project(block_bbox.max_x(), block_bbox.max_y(), z, u, v);
        face_projection.add(vgl_point_2d<double>(u,v));

        vgl_box_2d<double> cropped_face_projection = vgl_intersection(face_projection, image_bounds);

        if (!cropped_face_projection.is_empty()) {
          vgl_point_2d<double> face_proj_center = cropped_face_projection.centroid();
          vgl_ray_3d<double> ray = cam.ray(face_proj_center.x(), face_proj_center.y());
          bool this_block_in_front = (zi == 0)? ray.direction().z() < 0.0 : ray.direction().z() > 0.0;
          if(this_block_in_front) {
            // this block is in front of block on other side of face
            boxm2_block_id other_block_id = this_block_id;
            if (zi == 0) {
              --other_block_id.k_;
            }
            else {
              ++other_block_id.k_;
            }
            // check for other block in list of nodes
            auto other =
              this->find(other_block_id, temp_nodes) ;
            if ( other != temp_nodes.end() )
            {
              // found the other block, so add the edge
              (*node_iter)->out_edges.insert((*other));
              ++(*other)->num_incoming_edges_;
            }
          }
        }
      }
    }

    // all edges added. now traverse the graph picking off the node with minimum number of incoming edges;
    while(!temp_nodes.empty()) {
      int min_incoming = 7; // maximum possible number of incoming edges + 1
      boxm2_block_vis_graph_node_sptr next_node;

      for(auto & temp_node : temp_nodes)
      {
        int num_incoming = temp_node->num_incoming_edges_;
        if (num_incoming < min_incoming) {
          min_incoming = num_incoming;
          next_node = temp_node;
        }
        if (min_incoming == 0) {
          // dont bother searching for other 0's
          break;
        }
      }

      if (min_incoming > 0) {
        std::cerr << "Warning: unable to find strict visibilty ordering of blocks." << std::endl;
      }

      assert(next_node);

      // add the best node to the list
      ordered_nodes_.push_back(next_node->id_);

      // decrement incoming edge count of all blocks behind this one
      for (auto sink_node_it =
           next_node->out_edges.begin(); sink_node_it != next_node->out_edges.end(); ++sink_node_it) {
        --(*sink_node_it)->num_incoming_edges_;
      }
      // remove the links to avoid cyclical smart pointers
      next_node->out_edges.clear();

      temp_nodes.remove( next_node );
    }
}

std::list<boxm2_block_vis_graph_node_sptr>::iterator
    boxm2_block_vis_graph::find(const boxm2_block_id & id, std::list<boxm2_block_vis_graph_node_sptr> & list_nodes )
{
    auto iter = list_nodes.begin();
    for (; iter!= list_nodes.end(); iter++)
    {
        if(id == (*iter)->id_ )
            return iter;
    }
    return list_nodes.end();
}
