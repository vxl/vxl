// This is brl/bseg/sdet/sdet_denoise_mrf.h
#ifndef sdet_denoise_mrf_h_
#define sdet_denoise_mrf_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a processor applying a binary mrf denoise algorithm
//
//
//  This algorithm selectively smooths the image based on
//  a variance value at each pixel. The smoothing is carried out
//  by a MRF with binary cliques, where the clique energy weights
//  are controlled in part by the variance image.
//
//
// \author
//  J.L. Mundy - March 22, 2011
//
//
// The mrf energy minimization is carried out with a closed form algorithm
// i.e. non-iterative as compared with belief propagation,
// which requires iterations of message passing. The algorithm is based in
// part on the paper:
// "Continuous MRF-based image denoising with a closed form solution,"
// Ming Liu, Shifeng Chen, and Jianzhuang Liu
//  Proc. IEEE 17th International Conference on Image Processing, 2010
//
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>
#include <sdet/sdet_denoise_mrf_params.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_sparse_matrix.h>
class sdet_denoise_mrf : public sdet_denoise_mrf_params
{
 public:
  // === Constructors/destructor ===

  sdet_denoise_mrf(sdet_denoise_mrf_params& imp);
  ~sdet_denoise_mrf() override;

  // === Process methods ===

  void set_image(vil_image_resource_sptr const& resource) { in_resc_ = resource; }
  void set_variance(vil_image_resource_sptr const& var_resc)
    { var_resc_ = var_resc; }

  //: construct the mrf graph incidence matrix (no output image)
  // W and D are valid after construction
  void compute_incidence_matrix();

  //: the full denoising process
  bool denoise();

  // === Accessors ===

  vil_image_resource_sptr output() { return out_resc_; }
  bool output_valid() const { return output_valid_; }
  vnl_sparse_matrix<double>& incidence_matrix()  { return W_mat_; }
  vnl_sparse_matrix<double>& diag_matrix()  { return D_mat_; }
  vil_image_resource_sptr Dimgr();
  vnl_sparse_matrix<double>& diag_inv_sqrt()  { return D_inv_sqrt_; }
  vnl_sparse_matrix<double>& L() { return L_mat_; }
  vnl_vector<double>& F()  { return F_; }

 protected:
  // === protected methods ===

  //: compute the clique energy weight between two image locations
  double weight(unsigned i0, unsigned j0, unsigned i1, unsigned j1,
                vil_image_view<float> const& depth,
                vil_image_view<float> const& varv);
  //:the "L" matrix
  void compute_laplacian_matrix();
  //:the solution vector
  void compute_F();

  // === members ===

  bool output_valid_;      //process state flag
  double sigma_sq_inv_;
  vil_image_resource_sptr in_resc_;
  vil_image_resource_sptr var_resc_;
  vil_image_resource_sptr out_resc_;
  vnl_sparse_matrix<double> W_mat_;
  vnl_sparse_matrix<double> D_mat_;
  vnl_sparse_matrix<double> D_inv_sqrt_;
  vnl_sparse_matrix<double> L_mat_;
  vnl_sparse_matrix<double> inv_mat_;
  vnl_vector<double> F_;
};

#endif // sdet_denoise_mrf_h_
