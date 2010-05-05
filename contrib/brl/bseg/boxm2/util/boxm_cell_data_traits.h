// This is brl/bseg/boxm/boxm_cell_data_traits.h
#ifndef boxm_cell_data_traits_h_
#define boxm_cell_data_traits_h_
//:
// \file
// \brief  Set of accessors of cell's data, taking care of different datatypes
// \author Isabel Restrepo mir@lems.brown.edu
// \date   December 9, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <boct/boct_tree_cell.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_f1.h>
#include <boxm2/sample/boxm_scalar_sample.h>

//: Float to float
template <class T_loc>
float boxm_cell_to_float(boct_tree_cell<T_loc, float >* cell,  double /*step_len*/)
{
  //return 1.0 - vcl_exp(-cell->data()*step_len);
  return cell->data();
}

//: Mixture of gaussians to float; return the mean
template <class T_loc>
float boxm_cell_to_float(boct_tree_cell<T_loc, bsta_num_obs<bsta_gauss_f1> >* cell, double /*step_len*/)
{
  return cell->data().mean();
}

//: boxm_sample to float. Return the probability values
template <class T_loc, class T_data>
float boxm_cell_to_float(boct_tree_cell<T_loc, T_data >* cell, double step_len)
{
  return 1.f - float(vcl_exp(-cell->data().basic_val()*step_len));
}


template <class T_loc, class T_data>
T_data boxm_zero_val()
{
  return T_data(0);
}

template <>
bsta_num_obs<bsta_gauss_f1> boxm_zero_val<short, bsta_num_obs<bsta_gauss_f1> >();

template <>
float boxm_zero_val<short, float >();

//: boxm_scalar_sample to float. Return the normalized scalar value
template <class T_loc>
float boxm_cell_to_float(boct_tree_cell<T_loc, boxm_scalar_sample<float>  >* cell, double /*step_len*/)
{
  return cell->data().basic_val();
}
#endif
