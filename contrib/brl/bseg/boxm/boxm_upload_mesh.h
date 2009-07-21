#ifndef boxm_upload_mesh_h_
#define boxm_upload_mesh_h_
//:
// \file
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boxm/boxm_sample.h>
#include <boxm/boxm_scene.h>
#include <vgl/vgl_intersection.h>
#include <vcl_iostream.h>

template <class T_loc, class T_data>
void boxm_upload_mesh(boxm_block<boct_tree<T_loc, T_data> > *block,
                      imesh_mesh& mesh, T_data val)
{
  typedef boct_tree<T_loc, T_data> tree_type;
  tree_type* tree = block->get_tree();

  // initialize grid with big values
  imesh_face_array_base& fs = mesh.faces();

  for (unsigned i=0; i < fs.size(); ++i)
  {
    vcl_vector<vgl_point_3d<double> > v_list;
    imesh_vertex_array<3>& vertices = mesh.vertices<3>();
    vgl_box_3d<double> bb;
    for (unsigned j=0; j<fs.num_verts(i); ++j) {
      unsigned int v_id = fs(i,j);
      //double lx, ly, lz;
      //lvcs.global_to_local(vertices(v_id,0), vertices(v_id,1), vertices(v_id,2),
      //                     bgeo_lvcs::wgs84,lx,ly,lz);
      //vgl_point_3d<double> v(lx,ly,lz);
      vgl_point_3d<double> v(vertices(v_id,0), vertices(v_id,1), vertices(v_id,2));
      bb.add(v);
      v_list.push_back(v);
    }
  }

  // check if polygon's bounding box intersects with the block
  vgl_box_3d<double> block_bb = block->bounding_box();
  vgl_box_3d<double> inters = vgl_intersection(block_bb, bb);
  if (inters.is_empty())
    return false;
  
  //tree->initialize();

  // locate the polygon bounding box in tree
  boct_tree_cell<T_loc,T_data>* region=tree->locate_region(bb);
  if (region) {
    // test all the children for intersection
    vcl_vector<boct_tree_cell<T_loc,T_data>*> children;
    region->leaf_children(children);
    for (unsigned i=0; i<children.size(); i++) {
      boct_tree_cell<T_loc,T_data>* cell=children[i];
      vgl_box_3d<double> cell_bb=tree->cell_bounding_box(cell);
      if (vgl_intersection(cell_bb, v_list).size() > 0) {
        cell->set_data(val);
      }
    }
  }
}

#endif // boxm_upload_mesh_h_
