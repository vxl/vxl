#include <iostream>
#include <map>
#include "boxm2_import_triangle_mesh.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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


bool boxm2_import_triangle_mesh(boxm2_scene_sptr scene, const boxm2_cache_sptr& cache, imesh_mesh const& mesh,
                                bool zero_model, float occupied_prob)
{
  const imesh_face_array_base& mesh_faces = mesh.faces();
  const imesh_vertex_array_base& mesh_verts = mesh.vertices();

  std::cout << "mesh has " << mesh_faces.size() << " faces." << std::endl;
  std::cout << "mesh has " << mesh_verts.size() << " vertices." << std::endl;

  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();

  for (auto & block : blocks)
  {
    boxm2_block_id id = block.first;
    boxm2_block_metadata mdata = block.second;
    boxm2_block * blk = cache->get_block(scene,id);

    // assume cells are cubes, i.e. subblock_dim_.x() == subblock_dim_.y() == subblock_dim_.z()
    double subblock_len = mdata.sub_block_dim_.x();
    vgl_vector_3d<unsigned int> num_subblocks = mdata.sub_block_num_;
    vgl_point_3d<double> block_origin = mdata.local_origin_;

    boxm2_data_base *  alpha_base  = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    alpha_base->enable_write();
    boxm2_data<BOXM2_ALPHA> *alpha_data=new boxm2_data<BOXM2_ALPHA>(alpha_base->data_buffer(),alpha_base->buffer_length(),alpha_base->block_id());

    if (zero_model) {
      for (float & alpha_it : alpha_data->data()) {
        alpha_it = 0.0f;
      }
    }

    const boxm2_array_3d<vnl_vector_fixed<unsigned char, 16> > &trees = blk->trees();

    // iterate over every triangle in the mesh
    for (unsigned f=0; f<mesh_faces.size(); ++f) {
      std::vector<vgl_point_3d<double> > tri_verts(3);
      vgl_box_3d<double> tri_bbox;
      for (unsigned v=0; v<3; ++v) {
        int vert_idx = mesh_faces(f,v);
        vgl_point_3d<double> vert(mesh_verts(vert_idx,0),
                                  mesh_verts(vert_idx,1),
                                  mesh_verts(vert_idx,2));
        tri_bbox.add( vert );
        tri_verts[v] = vert;
      }
      bvgl_triangle_3d<double> triangle(tri_verts[0], tri_verts[1], tri_verts[2]);

      // compute subblock min and max based on triangle bounding box
      int ix_min = std::max(0,int(std::floor((tri_bbox.min_x() - block_origin.x())/subblock_len)));
      int iy_min = std::max(0,int(std::floor((tri_bbox.min_y() - block_origin.y())/subblock_len)));
      int iz_min = std::max(0,int(std::floor((tri_bbox.min_z() - block_origin.z())/subblock_len)));
      int ix_max = std::min(int(num_subblocks.x()-1),int(std::floor((tri_bbox.max_x() - block_origin.x())/subblock_len)));
      int iy_max = std::min(int(num_subblocks.y()-1),int(std::floor((tri_bbox.max_y() - block_origin.y())/subblock_len)));
      int iz_max = std::min(int(num_subblocks.z()-1),int(std::floor((tri_bbox.max_z() - block_origin.z())/subblock_len)));

      // for each subblock
      if((ix_min > ix_max) && (iy_min > iy_max) && (iz_min > iz_max)) {
        std::cout << "^^^^^^^^^^^ ERROR: triangle does not intersect any subblocks" << std::endl;
      }
      for (int iz=iz_min; iz<=iz_max; ++iz) {
        for (int iy=iy_min; iy<=iy_max; ++iy) {
          for (int ix=ix_min; ix<=ix_max; ++ix) {

            vgl_point_3d<double> subblock_origin = block_origin + vgl_vector_3d<double>(subblock_len*ix,
                                                                                        subblock_len*iy,
                                                                                        subblock_len*iz);

            //load current block/tree
            vnl_vector_fixed<unsigned char, 16>  tree = trees(ix, iy, iz);
            boct_bit_tree bit_tree((unsigned char*) tree.data_block(), mdata.max_level_);

            //iterate through leaves of the tree
            std::vector<int> leafBits = bit_tree.get_leaf_bits();
            for (int currBitIndex : leafBits) {
              int data_offset = bit_tree.get_data_index(currBitIndex); //data index

              vgl_box_3d<double> cell_box = bit_tree.cell_box(currBitIndex, subblock_origin, subblock_len);

              bool vert_in_box = cell_box.contains(tri_verts[0]) || cell_box.contains(tri_verts[1]) || cell_box.contains(tri_verts[2]);
              if (vert_in_box || bvgl_intersection(cell_box, triangle)) {
                double side_len = cell_box.width();
                double alpha = -std::log(1.0 - occupied_prob) / side_len;
                alpha_data->data()[data_offset] = alpha;
              }
            }
          }
        }
      }
    }
  }
  return true;
}
