// This is brl/bseg/bvpl/bvpl_octree/bvpl_cell_data_traits.h
#ifndef bvpl_cell_data_traits_h
#define bvpl_cell_data_traits_h
//:
// \file
// \brief  Set of accessors of cell's data, taking care of different datatypes
// \author Isabel Restrepo mir@lems.brown.edu
// \date  Dec 16, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvpl/bvpl_octree/sample/bvpl_octree_sample.h>
#include <boct/boct_tree_cell.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_sf1.h>

//: Float to float
template <class T_loc>
float bvpl_cell_to_float(boct_tree_cell<T_loc, bvpl_octree_sample<float> >* cell)
{
  return cell->data().response_;
}

//: Mixture of gaussians to float; return the mean
template <class T_loc>
float bvpl_cell_to_float(boct_tree_cell<T_loc, bvpl_octree_sample<bsta_num_obs<bsta_gauss_sf1> > >* cell)
{
  return cell->data().response_.mean();
}

#endif
