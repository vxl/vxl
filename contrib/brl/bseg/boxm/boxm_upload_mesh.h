#ifndef boxm_render_mesh_h_
#define boxm_render_mesh_h_
//:
// \file
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boxm/boxm_block.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_utils.h>

#include <imesh/imesh_mesh.h>
#include <vpgl/bgeo/bgeo_lvcs.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_intersection.h>
#include <vcl_iostream.h>

template <class T_loc, class T_data>
void boxm_upload_mesh_into_block(boxm_block<boct_tree<T_loc, T_data> > *block,
                                 imesh_mesh& mesh, bgeo_lvcs& lvcs, bool use_lvcs, T_data val)
{
  typedef boct_tree<T_loc, T_data> tree_type;
  tree_type* tree = block->get_tree();
  vgl_box_3d<double> block_bb = block->bounding_box();

  // initialize grid with big values
  imesh_face_array_base& fs = mesh.faces();
  
  for (unsigned i=0; i < fs.size(); ++i)
  {
    vcl_vector<vgl_point_3d<double> > v_list;
    imesh_vertex_array<3>& vertices = mesh.vertices<3>();
    vgl_box_3d<double> bb;
    for (unsigned j=0; j<fs.num_verts(i); ++j) {
      unsigned int v_id = fs(i,j);
      vgl_point_3d<double> v;
      if (use_lvcs) {
        double lx, ly, lz;
        lvcs.global_to_local(vertices(v_id,0), vertices(v_id,1), vertices(v_id,2),
                             bgeo_lvcs::wgs84,lx,ly,lz);
        v=vgl_point_3d<double>(lx,ly,lz);
      } else
        v=vgl_point_3d<double>(vertices(v_id,0), vertices(v_id,1), vertices(v_id,2));

      vgl_vector_3d<double> diff = v-block_bb.min_point();   // get the value according to the block origin
      //convert the values into [0,1] range
      v.set(diff.x()/block_bb.width(),diff.y()/ block_bb.height(),diff.y()/ block_bb.depth());
      bb.add(v);
      v_list.push_back(v);
    }
  

    // check if polygon's bounding box intersects with the block
    vgl_box_3d<double> inters = vgl_intersection(block_bb, bb);
    if (inters.is_empty())
      return;

    // locate the polygon bounding box in tree
    boct_tree_cell<T_loc,T_data>* region=tree->locate_region(bb);
    if (region) {
      // test all the children for intersection
      vcl_vector<boct_tree_cell<T_loc,T_data>*> children;
      if (!region->is_leaf())
        region->leaf_children(children);
      else // insert the node itself, if no children
        children.push_back(region);
      for (unsigned i=0; i<children.size(); i++) {
        boct_tree_cell<T_loc,T_data>* cell=children[i];
        vgl_box_3d<double> cell_bb=tree->cell_bounding_box(cell);
        if (boxm_utils::intersection(cell_bb, v_list)) {
          cell->set_data(val);
        }
      }
    }
  }
}

template <class T_loc, class T_data>
void boxm_upload_mesh_into_scene(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                                 imesh_mesh& mesh, bool use_lvcs, T_data val)
{
  typedef boct_tree<T_loc, T_data > tree_type;
  bgeo_lvcs lvcs=scene.lvcs();
  boxm_block_iterator<tree_type> iter(&scene);
  for (; !iter.end(); iter++) {
    scene.load_block(iter.index());
    boxm_block<tree_type>* block = *iter;
    boxm_upload_mesh_into_block(block, mesh, lvcs, use_lvcs, val);
    scene.write_active_block();
  }
}
#endif // boxm_upload_mesh_h_
