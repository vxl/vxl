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

//: Float to float
template <class T_loc>
float boxm_cell_to_float(boct_tree_cell<T_loc, float >* cell,  double step_len)
{
  //return 1.0 - vcl_exp(-cell->data()*step_len);
  return cell->data();
}

//: Mixture of gaussians to float; return the mean
template <class T_loc>
float boxm_cell_to_float(boct_tree_cell<T_loc, bsta_num_obs<bsta_gauss_f1> >* cell, double step_len)
{
  return cell->data().mean();
}

//: boxm_sample to float. Return the alpha values
template <class T_loc, class T_data>
float boxm_cell_to_float(boct_tree_cell<T_loc, T_data >* cell, double step_len)
{

  return 1.0 - vcl_exp(-cell->data().basic_val()*step_len);

}

#endif
