#ifndef boxm_render_mesh_h_
#define boxm_render_mesh_h_
//:
// \file
#include <iostream>
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boxm/boxm_block.h>
#include <boxm/boxm_scene.h>
#include <imesh/imesh_mesh.h>
#include <vpgl/vpgl_lvcs.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_intersection.h>
#include <vgl/algo/vgl_intersection.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T_loc, class T_data>
void boxm_upload_mesh_into_block(boxm_block<boct_tree<T_loc, T_data> > *block,
                                 imesh_mesh& mesh, vpgl_lvcs& lvcs, bool use_lvcs, T_data val)
{
  typedef boct_tree<T_loc, T_data> tree_type;
  tree_type* tree = block->get_tree();
  vgl_box_3d<double> block_bb = block->bounding_box();
  imesh_face_array_base& fs = mesh.faces();
  for (unsigned i=0; i < fs.size(); ++i)
  {
    std::list<vgl_point_3d<double> > v_list;
    imesh_vertex_array<3>& vertices = mesh.vertices<3>();
    vgl_box_3d<double> bb_scale, bb_global;
    for (unsigned j=0; j<fs.num_verts(i); ++j) {
      unsigned int v_id = fs(i,j);
      vgl_point_3d<double> v;
      if (use_lvcs) {
        double lx, ly, lz;
        lvcs.global_to_local(vertices(v_id,0), vertices(v_id,1), vertices(v_id,2),
                             vpgl_lvcs::wgs84,lx,ly,lz);
        v=vgl_point_3d<double>(lx,ly,lz);
      }
      else
        v=vgl_point_3d<double>(vertices(v_id,0), vertices(v_id,1), vertices(v_id,2));
      bb_global.add(v);
      vgl_vector_3d<double> diff = v-block_bb.min_point();   // get the value according to the block origin
      v.set(diff.x(), diff.y(), diff.z());
      // the local coordinates inside the box region
      v_list.push_back(v);
      //convert the values into [0,1] range
      v.set(diff.x()/block_bb.width(),diff.y()/ block_bb.height(),diff.z()/ block_bb.depth());
      bb_scale.add(v);
    }
    // check if polygon's bounding box intersects with the block
    vgl_box_3d<double> inters = vgl_intersection(vgl_box_3d<double>(0,0,0,1,1,1), bb_scale);
    if (inters.is_empty())
      return;

    // locate the polygon bounding box in tree
    boct_tree_cell<T_loc,T_data>* region=tree->locate_region(inters);
    if (region) {
      // test all the children for intersection
      std::vector<boct_tree_cell<T_loc,T_data>*> children;
      if (!region->is_leaf())
        region->leaf_children(children);
      else // insert the node itself, if no children
        children.push_back(region);
      for (unsigned i=0; i<children.size(); i++) {
        boct_tree_cell<T_loc,T_data>* cell=children[i];
        vgl_box_3d<double> cell_bb=tree->cell_bounding_box_local(cell);
        if (vgl_intersection<double>(cell_bb, v_list)) {
          cell->set_data(val);
        }
      }
    }
  }
}

//: this is to copy mesh into existing tree and replacing the appearance model of the existing tree.
template <>
void boxm_upload_mesh_into_block(boxm_block<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > *block,
                                 imesh_mesh& mesh, vpgl_lvcs& lvcs,
                                 bool use_lvcs, boxm_sample<BOXM_APM_MOG_GREY> val)
{
  typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
  tree_type* tree = block->get_tree();
  vgl_box_3d<double> block_bb = block->bounding_box();
  imesh_face_array_base& fs = mesh.faces();
  for (unsigned i=0; i < fs.size(); ++i)
  {
    std::list<vgl_point_3d<double> > v_list;
    imesh_vertex_array<3>& vertices = mesh.vertices<3>();
    vgl_box_3d<double> bb_scale, bb_global;
    for (unsigned j=0; j<fs.num_verts(i); ++j) {
      unsigned int v_id = fs(i,j);
      vgl_point_3d<double> v;
      if (use_lvcs) {
        double lx, ly, lz;
        lvcs.global_to_local(vertices(v_id,0), vertices(v_id,1), vertices(v_id,2),
                             vpgl_lvcs::wgs84,lx,ly,lz);
        v=vgl_point_3d<double>(lx,ly,lz);
      }
      else
        v=vgl_point_3d<double>(vertices(v_id,0), vertices(v_id,1), vertices(v_id,2));
      bb_global.add(v);
      vgl_vector_3d<double> diff = v-block_bb.min_point();   // get the value according to the block origin
      v.set(diff.x(), diff.y(), diff.z());
      // the local coordinates inside the box region
      v_list.push_back(v);
      //convert the values into [0,1] range
      v.set(diff.x()/block_bb.width(),diff.y()/ block_bb.height(),diff.z()/ block_bb.depth());
      bb_scale.add(v);
    }
    // check if polygon's bounding box intersects with the block
    vgl_box_3d<double> inters = vgl_intersection(vgl_box_3d<double>(0,0,0,0.999,0.999,0.999), bb_scale);
    if (inters.is_empty())
      return;

    // locate the polygon bounding box in tree
    boct_tree_cell<short,boxm_sample<BOXM_APM_MOG_GREY> >* region=tree->locate_region(inters);
    if (region) {
      // test all the children for intersection
      std::vector<boct_tree_cell<short,boxm_sample<BOXM_APM_MOG_GREY> >*> children;
      if (!region->is_leaf())
        region->leaf_children(children);
      else // insert the node itself, if no children
        children.push_back(region);
      for (auto cell : children) {
        vgl_box_3d<double> cell_bb=tree->cell_bounding_box(cell);
        if (vgl_intersection<double>(cell_bb, v_list)) {
          boxm_sample<BOXM_APM_MOG_GREY> tempdata=cell->data();
          tempdata.set_appearance(val.appearance());
          tempdata.alpha*=val.alpha;
          cell->set_data(tempdata);
        }
      }
    }
  }
}

//: calls all the blocks for drawing meshes
template <class T_loc, class T_data>
void boxm_upload_mesh_into_scene(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                                 imesh_mesh& mesh, bool use_lvcs, T_data val)
{
  typedef boct_tree<T_loc, T_data > tree_type;
  vpgl_lvcs lvcs=scene.lvcs();
  boxm_block_iterator<tree_type> iter(&scene);
  mesh.compute_face_normals(true);
  for (; !iter.end(); iter++) {
    scene.load_block(iter.index());
    boxm_block<tree_type>* block = *iter;
    boxm_upload_mesh_into_block(block, mesh, lvcs, use_lvcs, val);
    scene.write_active_block();
  }
}

#endif // boxm_render_mesh_h_
