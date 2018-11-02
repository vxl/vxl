// This is brl/bseg/bvxm/grid/bvxm_voxel_grid_basic_ops.h
#ifndef bvxm_voxel_grid_basic_ops_h_
#define bvxm_voxel_grid_basic_ops_h_
//:
// \file
// \brief set of basic grid operations
//
// \author Isabel Restrepo mir@lems.brown.edu
//
// \date  June 18, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <limits>
#include "bvxm_voxel_grid_base.h"
#include "bvxm_voxel_grid.h"
#include "bvxm_voxel_slab_iterator.h"
#include "bvxm_voxel_slab.h"
#include "bvxm_opinion.h"
#include <bvxm/bvxm_util.h>

#include <imesh/imesh_mesh.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/algo/vgl_intersection.h>
#include <vpgl/vpgl_lvcs.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/algo/vil3d_distance_transform.h>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Multiplies 2 grids. The types of input grids must have a * operator
template <class T>
bool bvxm_voxel_grid_multiply(bvxm_voxel_grid_base_sptr grid1_base, bvxm_voxel_grid_base_sptr grid2_base, bvxm_voxel_grid_base_sptr grid_out_base)
{
  // cast
  bvxm_voxel_grid<T> *grid1 = dynamic_cast<bvxm_voxel_grid<T>* >(grid1_base.ptr());
  bvxm_voxel_grid<T> *grid2 = dynamic_cast<bvxm_voxel_grid<T>* >(grid2_base.ptr());
  bvxm_voxel_grid<T> *grid_out = dynamic_cast<bvxm_voxel_grid<T>* >(grid_out_base.ptr());

  // check the casting was successful
  if ( !grid1 || !grid2 || !grid_out)
  {
    std::cerr << "One of the input voxels is of the wrong type\n";
    return false;
  }

  // check sizes are the same
  if ( grid1->grid_size() != grid2->grid_size() ||  grid1->grid_size() != grid_out->grid_size() )
  {
    std::cerr << "Grids are not of the same type\n";
    return false;
  }

  // multiply
  typename bvxm_voxel_grid<T>::iterator grid1_it = grid1->begin();
  typename bvxm_voxel_grid<T>::iterator grid2_it = grid2->begin();
  typename bvxm_voxel_grid<T>::iterator grid_out_it = grid_out->begin();

  for (; grid1_it != grid1->end(); ++grid1_it, ++grid2_it, ++grid_out_it)
  {
    typename bvxm_voxel_slab<T>::iterator slab1_it = (*grid1_it).begin();
    typename bvxm_voxel_slab<T>::iterator slab2_it = (*grid2_it).begin();
    typename bvxm_voxel_slab<T>::iterator slab_out_it = (*grid_out_it).begin();

    for (; slab1_it!=(*grid1_it).end(); ++slab1_it, ++slab2_it, ++slab_out_it)
    {
      (*slab_out_it) =(*slab1_it)*(*slab2_it);
    }
  }
  return true;
}

//: Copies grid1 into grid2
template <class T>
bool bvxm_voxel_grid_copy(bvxm_voxel_grid<T> *grid1,  bvxm_voxel_grid<T> *grid2)
{
    //check the pointers
  if ( !grid1 || !grid2 )
  {
    std::cerr << "One of the input voxels is of the wrong type\n";
    return false;
  }

  //check sizes are the same
  if ( grid1->grid_size() != grid2->grid_size() )
  {
    std::cerr << "Grids are not of the same type\n";
    return false;
  }

  //copy
  typename bvxm_voxel_grid<T>::iterator grid1_it = grid1->begin();
  typename bvxm_voxel_grid<T>::iterator grid2_it = grid2->begin();


  for (; grid1_it != grid1->end(); ++grid1_it, ++grid2_it)
  {
    typename bvxm_voxel_slab<T>::iterator slab1_it = (*grid1_it).begin();
    typename bvxm_voxel_slab<T>::iterator slab2_it = (*grid2_it).begin();

    for (; slab1_it!=(*grid1_it).end(); ++slab1_it, ++slab2_it)
    {
      (*slab2_it)=(*slab1_it);
    }
  }
  return true;
}

//: Multiplies 2 grids. The types of input grids must have a * operator
bool bvxm_voxel_grid_compare(bvxm_voxel_grid<float> * dt_grid,
                             bvxm_voxel_grid<bvxm_opinion> * op_grid,
                             bvxm_voxel_grid<float> * out_grid);

//: Thresholds a grid. This function returns the thresholded grid and a mask such that output grid = mask*input_grid
template <class T>
bool bvxm_voxel_grid_threshold(bvxm_voxel_grid_base_sptr grid_in_base,bvxm_voxel_grid_base_sptr grid_out_base,
                               bvxm_voxel_grid_base_sptr mask_grid_base, T min_thresh)
{
  //cast
  bvxm_voxel_grid<T> *grid_in = dynamic_cast<bvxm_voxel_grid<T> * > (grid_in_base.ptr());
  bvxm_voxel_grid<T> *grid_out = dynamic_cast<bvxm_voxel_grid<T> * > (grid_out_base.ptr());
  bvxm_voxel_grid<bool> *mask_grid = dynamic_cast<bvxm_voxel_grid<bool> * > (mask_grid_base.ptr());

  //check the casting was successful
  if ( !grid_in || !grid_out || !mask_grid)
  {
    std::cerr << "One of the input voxels is of the wrong type\n";
    return false;
  }

  //check sizes are the same
  if ( grid_in->grid_size() != grid_out->grid_size() ||  grid_in->grid_size() != mask_grid->grid_size() )
  {
    std::cerr << "Grids are not of the same type\n";
    return false;
  }


  //initialize grids
  grid_out->initialize_data(T(0));
  mask_grid->initialize_data(false);

  // iterate through the grids
  typename bvxm_voxel_grid<T>::iterator in_slab_it = grid_in->begin();
  typename bvxm_voxel_grid<T>::iterator out_slab_it = grid_out->begin();
  bvxm_voxel_grid<bool>::iterator mask_slab_it = mask_grid->begin();

  std::cout << "Thresholding Grid:" << std::endl;
  for (unsigned z=0; z<(unsigned)(grid_in->grid_size().z()); ++z, ++in_slab_it, ++mask_slab_it, ++out_slab_it)
  {
    std::cout << '.';

    //iterate through slab and threshold. At this point the grids get updated on disk
    typename bvxm_voxel_slab<T>::iterator in_it = (*in_slab_it).begin();
    typename bvxm_voxel_slab<T>::iterator out_it = (*out_slab_it).begin();
    bvxm_voxel_slab<bool>::iterator mask_it = (*mask_slab_it).begin();

    for (; in_it!= (*in_slab_it).end(); ++in_it, ++out_it, ++mask_it)
    {
      if (*in_it > min_thresh) {
        (*mask_it) = true;
        //if point is above threshold leave unchanged, otherwise set to 0
        (*out_it) = (*in_it);
      }
    }
  }
  return true;
}

template <class T>
bool bvxm_load_mesh_into_grid(bvxm_voxel_grid<T>* grid,imesh_mesh& mesh,T val)
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
              bvxm_voxel_slab_iterator<T> slab_it = grid->slab_iterator(grid_size.z()-z);
              bvxm_voxel_slab<T>& slab = *slab_it;
              T& tempval = slab(x,y);
              tempval=val;
              ++slab_it;
            }
          }
        }
      }
    }
  }

  return true;
}

//: function to convert a mesh in global coordinates to a mesh in local coordinates
void covert_global_mesh_to_local(imesh_mesh & inmesh,vpgl_lvcs& lvcs);

bool bvxm_load_mesh_normals_into_grid(bvxm_voxel_grid<vnl_vector_fixed<float,3> >* grid,imesh_mesh& mesh);

//: Digitize a simple polygon in local coordinates i.e. the same coordinate system of the grid.
template <class T>
bool bvxm_load_polygon_into_grid(bvxm_voxel_grid<T>* grid,
                                 std::vector<vgl_point_3d<double> > v_list,
                                 T val)
{
  vgl_box_3d<double> bb;
  for (const auto & i : v_list)
    bb.add(i);

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
          if (bvxm_util::intersection(voxel_box, v_list)) {
            bvxm_voxel_slab_iterator<T> slab_it = grid->slab_iterator(grid_size.z()-z);
            bvxm_voxel_slab<T>& slab = *slab_it;
            T& tempval = slab(x,y);
            tempval=val;
            ++slab_it;
          }
        }
      }
    }
  }

  return true;
}


template <class T>
bool bvxm_grid_dist_transform(bvxm_voxel_grid<float>* grid,
                              bvxm_voxel_grid<vnl_vector_fixed<float,3> >* dir,
                              bvxm_voxel_grid<float>* mag)
{
  // convert grid to a vil3d_image_view
  int k=0;
  vil3d_image_view<float> image(grid->grid_size().x(),grid->grid_size().y(),grid->grid_size().z());

  // create the image
  bvxm_voxel_slab_iterator<float> slab=grid->slab_iterator(k,1);
  while (slab != grid->end()) {
    bvxm_voxel_slab<float>& s = *slab;
    for (unsigned i=0; i<grid->grid_size().x(); ++i) {
      for (unsigned j=0; j<grid->grid_size().y(); ++j) {
          if (s(i,j)>0)
            image(i,j,k)=0;
          else
            image(i,j,k)=std::numeric_limits<float>::max();
      }
    }
    ++k;
    ++slab;
  }

  // create the direction vector
  vil3d_image_view<vil_rgb<float> > directions(grid->grid_size().x(),grid->grid_size().y(),grid->grid_size().z());
  directions.fill(vil_rgb<float> (0,0,0));
  vil3d_distance_transform_with_dir(image, directions,1,1,1);

  // put back the image into the grid
  k=0;
  slab=grid->slab_iterator(k,1);
  while (slab != grid->end()) {
    bvxm_voxel_slab<float>& s = *slab;
    for (unsigned i=0; i<grid->grid_size().x(); ++i) {
      for (unsigned j=0; j<grid->grid_size().y(); ++j) {
        s(i,j)=image(i,j,k);
      }
    }
    ++slab;
    ++k;
  }

  // create a directions grid
  // put back the image into the grid
  k=0;
  bvxm_voxel_slab_iterator<vnl_vector_fixed<float,3> > dir_slab=dir->slab_iterator(k,1);
  bvxm_voxel_slab_iterator<float> mag_slab=mag->slab_iterator(k,1);

  while (dir_slab != dir->end()) {
    bvxm_voxel_slab<vnl_vector_fixed<float,3> >& d = *dir_slab;
    bvxm_voxel_slab<float>& d_mag = *mag_slab;
    for (unsigned i=0; i<dir->grid_size().x(); ++i) {
      for (unsigned j=0; j<dir->grid_size().y(); ++j) {
          d(i,j)=vnl_vector_fixed<float,3>(directions(i,j,k).R(),directions(i,j,k).G(),directions(i,j,k).B());
          d_mag(i,j)=std::sqrt(directions(i,j,k).R()*directions(i,j,k).R()
                             +directions(i,j,k).G()*directions(i,j,k).G()
                             +directions(i,j,k).B()*directions(i,j,k).B());
      }
    }
    ++dir_slab;
    ++mag_slab;
    ++k;
  }

  return true;
}

#endif // bvxm_voxel_grid_basic_ops_h_
