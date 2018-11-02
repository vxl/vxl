#include <iostream>
#include <list>
#include "bvxm_voxel_grid_basic_ops.h"
//:
// \file

#include <imesh/imesh_vertex.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_vector_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: function to convert a mesh in global coordinates to a mesh in local coordinates
void covert_global_mesh_to_local(imesh_mesh & inmesh,vpgl_lvcs& lvcs)
{
  imesh_vertex_array<3>& in_verts=inmesh.vertices<3>() ;
  for (unsigned i=0; i < in_verts.size(); ++i)
  {
    double lx, ly, lz;
    lvcs.global_to_local(in_verts(i,0),in_verts(i,1),in_verts(i,2),vpgl_lvcs::wgs84,lx,ly,lz);
    imesh_vertex<3> v(lx,ly,lz);
    in_verts[i]=v;
  }
}

bool bvxm_load_mesh_normals_into_grid(bvxm_voxel_grid<vnl_vector_fixed<float,3> >* grid,imesh_mesh& mesh)
{
  // initialize grid with big values
  imesh_face_array_base& fs = mesh.faces();

  for (unsigned i=0; i < fs.size(); ++i)
  {
    std::list<vgl_point_3d<double> > v_list;
    imesh_vertex_array<3>& vertices = mesh.vertices<3>();
    vgl_box_3d<double> bb;
    for (unsigned j=0; j<fs.num_verts(i); ++j) {
      unsigned int v_id = fs(i,j);
      vgl_point_3d<double> v(vertices(v_id,0),
                             vertices(v_id,1),
                             vertices(v_id,2));
      bb.add(v);
      v_list.push_back(v);
    }

    vgl_vector_3d<double> normal=fs.normal(i);
    vnl_vector_fixed<float,3> vnl_normal(float(normal.x()),float(normal.y()),float(normal.z()));

    vgl_vector_3d<unsigned int> grid_size = grid->grid_size();
    vgl_box_3d<double> grid_box;
    grid_box.set_min_point(vgl_point_3d<double>(0,0,0));
    grid_box.set_max_point(vgl_point_3d<double>(grid_size.x()-1,grid_size.y()-1,grid_size.z()-1));
    vgl_point_3d<double> min = bb.min_point();
    vgl_point_3d<double> max = bb.max_point();
    for (int z=(int)min.z(); z<=max.z(); ++z) {
      for (int y=(int)min.y(); y<=max.y(); ++y) {
        for (int x=(int)min.x(); x<=max.x(); ++x) {
          //check if the voxel position is valid
          if (grid_box.contains(x,y,z)) {
            vgl_box_3d<double> voxel_box;
            voxel_box.set_min_point(vgl_point_3d<double>(x,y,z));
            voxel_box.set_max_point(vgl_point_3d<double>(x+1,y+1,z+1));
            if (vgl_intersection<double>(voxel_box, v_list)) {
              bvxm_voxel_slab_iterator<vnl_vector_fixed<float,3> > slab_it = grid->slab_iterator(grid_size.z()-z);
              (*slab_it)(x,y)=vnl_normal;
              ++slab_it;
            }
          }
        }
      }
    }
  }

  return true;
}

bool bvxm_voxel_grid_compare(bvxm_voxel_grid<float> * dt_grid,
                             bvxm_voxel_grid<bvxm_opinion> * op_grid,
                             bvxm_voxel_grid<float> * grid_out)
{
  // check the grids exist
  if ( !dt_grid || !op_grid || !grid_out)
  {
    std::cerr << "One of the input voxels is of the wrong type\n";
    return false;
  }

  // check sizes are the same
  if ( dt_grid->grid_size() != op_grid->grid_size() ||  dt_grid->grid_size() != grid_out->grid_size() )
  {
    std::cerr << "Grids are not of the same size\n";
    return false;
  }

  // multiply
  bvxm_voxel_grid<float>::iterator dt_grid_it = dt_grid->begin();
  bvxm_voxel_grid<bvxm_opinion>::iterator op_grid_it = op_grid->begin();
  bvxm_voxel_grid<float>::iterator grid_out_it = grid_out->begin();

  for (; dt_grid_it != dt_grid->end(); ++dt_grid_it, ++op_grid_it, ++grid_out_it)
  {
    bvxm_voxel_slab<float>::iterator slab1_it = (*dt_grid_it).begin();
    bvxm_voxel_slab<bvxm_opinion>::iterator slab2_it = (*op_grid_it).begin();
    bvxm_voxel_slab<float>::iterator slab_out_it = (*grid_out_it).begin();

    for (; slab1_it!=(*dt_grid_it).end(); ++slab1_it ,++slab2_it, ++slab_out_it)
    {
      if ((*slab1_it)<7)
        (*slab_out_it) =(*slab2_it).b(); // * (*slab1_it)
      else
        (*slab_out_it) =0;
    }
  }
  return true;
}
