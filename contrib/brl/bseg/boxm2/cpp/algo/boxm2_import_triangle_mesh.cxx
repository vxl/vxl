#include "boxm2_import_triangle_mesh.h"
#include <vcl_map.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/basic/boxm2_array_3d.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boct/boct_bit_tree.h>
#include <boxm2/io/boxm2_cache.h>
#include <bvgl/bvgl_intersection.h>
#include <imesh/imesh_mesh.h>
#include <imesh/imesh_face.h>
#include <imesh/imesh_vertex.h>


bool boxm2_import_triangle_mesh(boxm2_scene_sptr scene, boxm2_cache_sptr cache, imesh_mesh const& mesh)
{
  const float big_alpha = 1.0e4;
  const imesh_face_array_base& mesh_faces = mesh.faces();
  const imesh_vertex_array_base& mesh_verts = mesh.vertices();

  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  
  for (vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator iter = blocks.begin(); iter!= blocks.end(); iter ++)
  {
    boxm2_block_id id = iter->first;
    boxm2_block_metadata mdata = iter->second;
    boxm2_block * blk = cache->get_block(scene,id);

    // assume cells are cubes, i.e. subblock_dim_.x() == subblock_dim_.y() == subblock_dim_.z()
    double subblock_len = mdata.sub_block_dim_.x();
    vgl_point_3d<double> block_origin = mdata.local_origin_;

    boxm2_data_base *  alpha_base  = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    alpha_base->enable_write();
    boxm2_data<BOXM2_ALPHA> *alpha_data=new boxm2_data<BOXM2_ALPHA>(alpha_base->data_buffer(),alpha_base->buffer_length(),alpha_base->block_id());

    boxm2_array_3d<vnl_vector_fixed<unsigned char, 16> > &trees = blk->trees();
    // for each subblock
    for (unsigned iz=0; iz<trees.get_row3_count(); ++iz) {
      vcl_cout << "#################   iz = " << iz << vcl_endl;
      for (unsigned iy=0; iy<trees.get_row2_count(); ++iy) {
        for (unsigned ix=0; ix<trees.get_row1_count(); ++ix) {

          vgl_point_3d<double> subblock_origin = block_origin + vgl_vector_3d<double>(subblock_len*ix,
                                                                                      subblock_len*iy,
                                                                                      subblock_len*iz);

          //load current block/tree
          vnl_vector_fixed<unsigned char, 16>  tree = trees(ix, iy, iz);
          boct_bit_tree bit_tree((unsigned char*) tree.data_block(), mdata.max_level_);

          //iterate through leaves of the tree
          vcl_vector<int> leafBits = bit_tree.get_leaf_bits();
          for (vcl_vector<int>::iterator iter = leafBits.begin(); iter != leafBits.end(); ++iter) {
            int currBitIndex = (*iter);
            int data_offset = bit_tree.get_data_index(currBitIndex); //data index


            vgl_box_3d<double> cell_box = bit_tree.cell_box(currBitIndex, subblock_origin, subblock_len);

            // iterate over every triangle in the mesh
            bool found_intersection = false;
            for (unsigned f=0; f<mesh_faces.size(); ++f) {
              vcl_vector<vgl_point_3d<double> > tri_verts(3);
              for (unsigned v=0; v<3; ++v) {
                int vert_idx = mesh_faces(f,v);
                tri_verts[v] = vgl_point_3d<double>(mesh_verts(vert_idx,0),
                                                    mesh_verts(vert_idx,1),
                                                    mesh_verts(vert_idx,2));
              }
              bvgl_triangle_3d<double> triangle(tri_verts[0], tri_verts[1], tri_verts[2]);
              bool vert_in_box = cell_box.contains(tri_verts[0]) || cell_box.contains(tri_verts[1]) || cell_box.contains(tri_verts[2]);
              if (vert_in_box || bvgl_intersection(cell_box, triangle)) {
                found_intersection = true;
                break;
              }
            }
            if (found_intersection) {
              alpha_data->data()[data_offset] = big_alpha ;
            }
            else {
              alpha_data->data()[data_offset] = 0.0f;
              vcl_cout.flush();
            }
          }
        }
      }
    }
  }
  return true;
}
