// This is bvpl_gauss3D_steerable_filters.h
#ifndef bvpl_gauss3D_steerable_filters_h
#define bvpl_gauss3D_steerable_filters_h

//:
// \file
// \brief 3-d Streerable filters implemented theough separable basis
// \author Isabel Restrepo mir@lems.brown.edu
// \date  12-Aug-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vnl/vnl_vector_fixed.h>
#include <boxm/boxm_scene.h>
#include <bvpl/functors/bvpl_algebraic_functor.h>
#include <bvpl/kernels/bvpl_kernel.h>


class bvpl_gauss3D_steerable_filters{

public:
  bvpl_gauss3D_steerable_filters();

  //: The dimension of the filter response e.i. number of steerable filters
  enum { DIM_ = 10 };

  //: Compute basis response at all leaf cells
  bool basis_response_at_leaves(boxm_scene<boct_tree<short, vnl_vector_fixed<float, bvpl_gauss3D_steerable_filters::DIM_> > > *scene,
                                boxm_scene<boct_tree<short, bool> > *valid_scene, double cell_length);

  //: Compute basis response at all levels - assumes the intermediate cells of the tree have ben filled with meaningful info, otherwise the result could be garbage
  bool multiscale_basis_response(boxm_scene<boct_tree<short, vnl_vector_fixed<float, bvpl_gauss3D_steerable_filters::DIM_> > > *scene,
                                 boxm_scene<boct_tree<short, bool> > *valid_scene, unsigned resolution_level);

  //: Assemble basis for: -2 -1 0 1 2
  void assemble_basis_size_5();

  std::vector<std::vector<std::string> > basis() {return basis_;}
  std::vector<std::string> basis_names() {return basis_names_;}
  std::map<std::string, vnl_vector_fixed<float,5> > separable_taps(){return separable_taps_;}

  bool rotation_invariant_interpolation(boxm_scene<boct_tree<short, vnl_vector_fixed<float, bvpl_gauss3D_steerable_filters::DIM_> > > *scene,
                                        boxm_scene<boct_tree<short, bool> > *valid_scene);

protected:

  //: lists of 1-d filter tap names needed for each basis
  std::vector<std::vector<std::string> > basis_;

  //: list of basis identifying strings
  std::vector<std::string> basis_names_;

  //: map containing the 1-d filter taps (actual values) and their identifying string
  std::map<std::string, vnl_vector_fixed<float,5> > separable_taps_;

  std::vector<vgl_vector_3d<int> > axis_;
};
#endif
