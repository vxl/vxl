// This is brl/bseg/sdet/sdet_denoise_mrf.cxx
#include <iostream>
#include <cstdlib>
#include "sdet_denoise_mrf.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <vil/vil_new.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/algo/vnl_sparse_lu.h>
#include <vul/vul_timer.h>
#include <brip/brip_vil_float_ops.h>
#include <brip/brip_line_generator.h>

//---------------------------------------------------------------
// Constructors
//----------------------------------------------------------------

// constructor from a parameter block (the only way)
sdet_denoise_mrf::sdet_denoise_mrf(sdet_denoise_mrf_params& dmp)
  : sdet_denoise_mrf_params(dmp), output_valid_(false), in_resc_(nullptr),
    var_resc_(nullptr), out_resc_(nullptr)
{
  sigma_sq_inv_ = radius_/1.978; //so that exp(radius^2/sigma_^2) = 0.02
  sigma_sq_inv_ *= sigma_sq_inv_;
  sigma_sq_inv_ = 1.0/sigma_sq_inv_;
}

// Default Destructor
sdet_denoise_mrf::~sdet_denoise_mrf()
= default;

double sdet_denoise_mrf::weight(unsigned i0, unsigned j0,
                                unsigned i1, unsigned j1,
                                vil_image_view<float> const& inv,
                                vil_image_view<float> const& varv)
{
  int ni = inv.ni(), nj = inv.nj();
  //first consider a max radius
  //weight due to inter-pixel distance
  double dsq = (i1-i0)*(i1-i0) + (j1-j0)*(j1-j0);
  double d = std::sqrt(dsq);
  if (d>(vnl_math::sqrt2*radius_)) return 0.0;
  //compute the minimum variance along the path between pix0 and pix1
  bool init = true;
  auto xs = static_cast<float>(i0), ys = static_cast<float>(j0);
  auto xe = static_cast<float>(i1), ye = static_cast<float>(j1);
  float x=xs, y=ys;
  double min_var = vnl_numeric_traits<double>::maxval;
  //generate the path between two pixels
  while (brip_line_generator::generate(init, xs, ys, xe, ye, x, y))
  {
    auto i = static_cast<unsigned>(x), j = static_cast<unsigned>(y);
    double v = varv(i, j);
    if (v<min_var)
      min_var = v;
  }
  //compute the average depth difference a window of size radius_
  // compute the average depths
  double sum0=0.0, sum1=0.0;
  double n0 = 0.0, n1 = 0.0;
  int r = static_cast<int>(radius_);
  for (int kj = -r; kj<=r; ++kj) {
    int j0k = j0+kj, j1k = j1+kj;
    if (j0k<0||j0k>=nj||j1k<0||j1k>=nj) continue;
    for (int ki = -r; ki<=r; ++ki) {
      int i0k = i0+ki, i1k = i1+ki;
      if (i0k<0||i0k>=ni||i1k<0||i1k>=ni) continue;
      sum0 += inv(i0k, j0k);
      sum1 += inv(i1k, j1k);
      n0+=1.0;
      n1+=1.0;
    }
  }
  double v_avg0=0.0, v_avg1=0.0;
  if (n0>0.0)
    v_avg0 = sum0/n0;
  if (n1>0.0)
    v_avg1 = sum1/n1;
  //the difference between depth values averaged over the neighbrd
  double Delta = (v_avg1-v_avg0)*(v_avg1-v_avg0);
  double w = std::exp(-dsq/sigma_sq_inv_ +
                     kappa_*min_var -
                     beta_*Delta);
  return w;
}

void sdet_denoise_mrf::compute_incidence_matrix()
{
  int ni = in_resc_->ni(), nj = in_resc_->nj();
  int npix = ni*nj;
  W_mat_.resize(npix, npix);
  D_mat_.set_size(npix, npix);
  D_inv_sqrt_.set_size(npix, npix);
  vil_image_view<float> inv = brip_vil_float_ops::convert_to_float(in_resc_);
  vil_image_view<float> varv = brip_vil_float_ops::convert_to_float(var_resc_);
  int r = static_cast<int>(radius_);
  for (int j = 0; j<nj; ++j) {
    for (int i = 0; i<ni; ++i) {
      double D = 0.0;
      unsigned indx = i + ni*j;
      // index over the neighborhood
      for ( int kj = -r; kj<=r; ++kj) {
        int joff = j+kj;
        if (joff<0||joff>=nj)
          continue;
        for ( int ki = -r; ki<=r; ++ki) {
          int ioff = i+ki;
          if (ioff<0||ioff>=ni)
            continue;
          int indx_nbr = (ioff) + ni*(joff);
          if (indx_nbr<0||indx_nbr>=npix)
            continue;
          if ((unsigned int)indx_nbr!=indx) {
            double w = weight(i, j, i+ki, j+kj, inv, varv);
            if (w>0.0) {
              W_mat_.put(indx, indx_nbr, w);
              D += w;
            }
          }
          else {
            W_mat_.put(indx, indx_nbr, 1.0);
            D += 1.0;
          }
        }
      }
      D_mat_.put(indx,indx, D) ;
      D_inv_sqrt_.put(indx,indx,1.0/std::sqrt(D));
    }
  }
}

vil_image_resource_sptr sdet_denoise_mrf::Dimgr()
{
  unsigned nr = D_mat_.rows();
  if (!nr||!in_resc_) return nullptr;
  unsigned ni = in_resc_->ni(), nj = in_resc_->nj();
  if (nr!=ni*nj) return nullptr;
  vil_image_view<float> out(ni, nj);
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i) {
      unsigned indx = i + ni*j;
      out(i,j)=static_cast<float>(D_mat_(indx,indx));
    }
  return vil_new_image_resource_of_view(out);
}

void sdet_denoise_mrf::compute_laplacian_matrix()
{
  L_mat_ = D_mat_-W_mat_;
  L_mat_ = D_inv_sqrt_*L_mat_*D_inv_sqrt_;
}

void sdet_denoise_mrf::compute_F()
{
  vil_image_view<float> in_view = in_resc_->get_view();
  unsigned ni = in_view.ni(), nj = in_view.nj();
  // form the LU "b" matrix
  vnl_vector<double> b(W_mat_.rows());
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i) {
      unsigned indx = i + j*ni;
      double dinsq = D_inv_sqrt_(indx, indx);
      b[indx]=dinsq*in_view(i,j);
    }
  //Form the LU input matrix
  // D^-1
  W_mat_ = D_inv_sqrt_*D_inv_sqrt_;
  // (D^-1 + 2*L)
  W_mat_ += 2.0*L_mat_;
  vnl_sparse_lu lu(W_mat_, vnl_sparse_lu::estimate_condition);
  vnl_vector<double> R = lu.solve(b);
  F_.set_size(W_mat_.rows());
  for (unsigned r = 0; r<W_mat_.rows(); ++r)
    F_[r] = D_inv_sqrt_(r,r)*R[r];
}

bool sdet_denoise_mrf::denoise()
{
  if (!in_resc_) return false;
  if (!var_resc_) return false;
  unsigned area = in_resc_->ni()*in_resc_->nj();
  std::cout << " constructing mrf on " << area << " x " << area
           << " incidence matrix\n";
  vul_timer t;
  this->compute_incidence_matrix();
  this->compute_laplacian_matrix();
  std::cout << "formed incidence and auxiliary matrices in "
           << t.real()/1000.0 << " seconds\n";
  t.mark();
  this->compute_F();
  std::cout << "solved LU decomposition and back substitution in "
           << t.real()/1000.0 << " seconds\n";
  int ni = in_resc_->ni(), nj = in_resc_->nj();
  vil_image_view<float> out_view(ni, nj);
  out_view.fill(0.0f);
  for (int j = 0; j<nj; ++j)
    for (int i = 0; i<ni; ++i) {
      unsigned indx = i + ni*j;
      out_view(i,j) = static_cast<float>(F_[indx]);
    }
  out_resc_ = vil_new_image_resource_of_view(out_view);
  output_valid_ = true;
  return true;
}
