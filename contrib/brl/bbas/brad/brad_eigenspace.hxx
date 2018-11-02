// Thisx is brl/bbas/brad/brad_eigenspace.hxx
#ifndef brad_eigenspace_hxx_
#define brad_eigenspace_hxx_
#include <iostream>
#include <cstdlib>
#include "brad_eigenspace.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/vnl_numeric_traits.h>
#include <vil/vil_blocked_image_resource.h>

static float bayes(float eig0, float eig1, float eig2,
                   bsta_joint_histogram_3d<float> const& no_atmos,
                   bsta_joint_histogram_3d<float> const& atmos,
                   float prob_ratio = 1.0f)
{
  float t = 1.0e-13f;
  float p_no_atmos = no_atmos.p(eig0, eig1, eig2);
  float p_atmos = atmos.p(eig0, eig1, eig2);
  float sum = p_no_atmos + p_atmos;
  if (sum <t)
    return 0.5f;
  float res = p_no_atmos/(p_no_atmos + prob_ratio*p_atmos);
  return res;
}

static void
print_resource_stats(std::vector<vil_image_resource_sptr> const& rescs)
{
  std::cout << "processing " << rescs.size() << " resources\n";
  std::vector<vil_image_resource_sptr>::const_iterator rit = rescs.begin();
  for (unsigned i = 0; rit!= rescs.end(); ++rit, ++i) {
    unsigned ni = (*rit)->ni(), nj = (*rit)->nj();
    std::cout << '[' << i << "]:(" << ni << ' ' << nj << ")\n" << std::flush;
  }
}

template <class T>
bool brad_eigenspace<T>::
compute_covariance_matrix(std::vector<vil_image_resource_sptr> const& rescs)
{
  unsigned n = funct_.size();
  if (!n) return false;
  std::cout << "computing covariance matrix\n" << std::flush;
  covar_valid_ = false;
  print_resource_stats(rescs);
  mean_.set_size(n);
  mean_.fill(0.0);
  vnl_matrix<double> var(n, n);
  var.fill(0.0);
  unsigned n_samples = 0;
  std::vector<vil_image_resource_sptr>::const_iterator rit = rescs.begin();
  for (; rit!= rescs.end(); ++rit) {
    unsigned ni = (*rit)->ni(), nj = (*rit)->nj();
    if (ni==0||nj==0||ni<nib_||nj<njb_) return false;
    unsigned nbi = ni/nib_, nbj = nj/njb_;
    unsigned i0 = 0, j0 = 0;
    for (unsigned r = 0; r<nbj; ++r, j0+=njb_) {
      i0 = 0;
      for (unsigned c = 0; c<nbi; ++c, i0+=nib_)
      {
        vil_image_view_base_sptr view_ptr =
          (*rit)->get_view(i0, nib_, j0, njb_);
        vil_image_view<float> fview = vil_convert_cast(float(), view_ptr);
        vnl_vector<double> v = funct_(fview);
        mean_ += v;
        var += outer_product(v, v);
        ++n_samples;
      }
      std::cout << '.' << std::flush;
    }
    std::cout << '\n' << std::flush;
  }
  if (!n_samples) return false;
  double ninv = 1.0/static_cast<double>(n_samples);
  mean_ *= ninv;
  covar_ = ninv*var - outer_product(mean_, mean_);
  covar_valid_ = true;
  return true;
}

template <class T>
bool brad_eigenspace<T>::
compute_covariance_matrix_rand(std::vector<vil_image_resource_sptr> const& rescs, double frac, unsigned nit, unsigned njt)
{
  std::vector<vil_image_resource_sptr>::const_iterator rit = rescs.begin();
  double area = 0.0;
  for (unsigned i = 0; rit!= rescs.end(); ++rit, ++i) {
    unsigned ni = (*rit)->ni(), nj = (*rit)->nj();
    if (ni<nib_||nj<njb_)
      return false;
    area += ni*nj;
  }
  //the total image area to be processed
  double proc_area = area*frac;
  // the number of random tiles to be processed
  double nitd = nit, njtd = njt;
  unsigned ntiles = static_cast<unsigned>(proc_area/(nitd*njtd));
  if (!ntiles) return false;

  //set up the covariance data
  unsigned n = funct_.size();
  if (!n) return false;
  std::cout << "computing covariance matrix - randomly selecting "
           << ntiles << " (" << nit << 'x' << njt << ") tiles\n" << std::flush;
  covar_valid_ = false;
  print_resource_stats(rescs);
  mean_.set_size(n);
  mean_.fill(0.0);
  vnl_matrix<double> var(n, n);
  var.fill(0.0);
  unsigned n_samples = 0;

  // process the tiles
  double nd = static_cast<double>(rescs.size());
  for (unsigned t = 0; t<ntiles; ++t) {
    //randomly select a resource
    unsigned ires =
      static_cast<unsigned>((nd)*(std::rand()/(RAND_MAX+1.0)));
    //compute random access to tile
    double nid = rescs[ires]->ni(), njd = rescs[ires]->nj();
    double rd = (njd-njtd-1.0)*(std::rand()/(RAND_MAX+1.0));
    double cd = (nid-nitd-1.0)*(std::rand()/(RAND_MAX+1.0));
    if (rd<0) rd = 0.0;
    if (cd<0) cd = 0.0;
    if (rd>(njd-njtd-1.0)) rd = (njd-njtd-1.0);
    if (cd>(nid-nitd-1.0)) cd = (nid-nitd-1.0);
    unsigned j0 = static_cast<unsigned>(rd), i0 = static_cast<unsigned>(cd);
    vil_image_view_base_sptr tile_ptr =
      rescs[ires]->get_view(i0, nit, j0, njt);
    vil_image_view<float> ftile = vil_convert_cast(float(), tile_ptr);
    unsigned nbi = nit/nib_, nbj = njt/njb_;
    i0 = 0; j0=0;
    for (unsigned r = 0; r<nbj; ++r, j0+=njb_) {
      i0 = 0;
      for (unsigned c = 0; c<nbi; ++c, i0+=nib_) {
        vil_image_view<float> fview(nib_, njb_);
        for (unsigned j =0; j<njb_; ++j)
          for (unsigned i =0; i<nib_; ++i)
            fview(i, j) = ftile(i0+i, j0+j);
        vnl_vector<double> v = funct_(fview);
        mean_ += v;
        var += outer_product(v, v);
        ++n_samples;
        if (n_samples%100==0)
          std::cout << ires << ' ' << std::flush;
      }
    }
  }
  if (!n_samples) return false;
  double ninv = 1.0/static_cast<double>(n_samples);
  mean_ *= ninv;
  covar_ = ninv*var - outer_product(mean_, mean_);
  covar_valid_ = true;
  std::cout << '\n' << std::flush;
  return true;
}

template <class T>
bool brad_eigenspace<T>::
compute_covariance_matrix_blocked(std::vector<vil_image_resource_sptr> const& rescs, unsigned nit, unsigned njt)
{
  unsigned n = funct_.size();
  if (!n) return false;
  std::cout << "computing covariance matrix (blocked cache)\n" << std::flush;
  covar_valid_ = false;
  print_resource_stats(rescs);
  mean_.set_size(n);
  mean_.fill(0.0);
  vnl_matrix<double> var(n, n);
  var.fill(0.0);
  unsigned n_samples = 0;
  std::vector<vil_image_resource_sptr>::const_iterator rit = rescs.begin();
  for (; rit!= rescs.end(); ++rit) {
    unsigned ni = (*rit)->ni(), nj = (*rit)->nj();
    if (ni==0||nj==0||ni<nib_||nj<njb_) return false;
    unsigned nbi = ni/nib_, nbj = nj/njb_;
    vil_blocked_image_resource_sptr bresc =
      vil_new_blocked_image_facade(*rit, nit, njt);
    vil_blocked_image_resource_sptr cbresc =
      vil_new_cached_image_resource(bresc);
    unsigned i0 = 0, j0 = 0;
    for (unsigned r = 0; r<nbj; ++r, j0+=njb_) {
      i0 = 0;
      for (unsigned c = 0; c<nbi; ++c, i0+=nib_)
      {
        vil_image_view_base_sptr view_ptr =
          cbresc->get_view(i0, nib_, j0, njb_);
        vil_image_view<float> fview = vil_convert_cast(float(), view_ptr);
        vnl_vector<double> v = funct_(fview);
        mean_ += v;
        var += outer_product(v, v);
        n_samples++;
      }
      std::cout << '.'<< std::flush;
    }
    std::cout << '\n' << std::flush;
  }
  if (!n_samples) return false;
  double ninv = 1.0/static_cast<double>(n_samples);
  mean_ *= ninv;
  covar_ = ninv*var - outer_product(mean_, mean_);
  covar_valid_ = true;
  return true;
}

template <class T>
bool brad_eigenspace<T>::
compute_eigensystem()
{
  if (!covar_valid_)
    return false;
  eigensystem_valid_ = false;
  std::cout << "computing eigensystem\n" << std::flush;
  vnl_symmetric_eigensystem<double> sym_eig(covar_);
  unsigned n = covar_.rows();
  eigenvectors_ = sym_eig.V;
  eigenvalues_.set_size(n);
  for (unsigned i = 0; i<n; ++i)
    eigenvalues_[i] = sym_eig.get_eigenvalue(i);
  eigensystem_valid_ = true;
  return true;
}

template <class T>
bool brad_eigenspace<T>::
compute_eigenimage(vil_image_resource_sptr const& resc,
                   std::string const& output_path)
{
  if (!eigensystem_valid_)
    return false;
  std::cout << "computing eigenvalue color image\n";
  unsigned n = funct_.size();
  vnl_vector<double> v(n);
  vnl_vector<double> v0 = eigenvectors_.get_column(n-1);
  vnl_vector<double> v1 = eigenvectors_.get_column(n-2);
  vnl_vector<double> v2 = eigenvectors_.get_column(n-3);
  unsigned ni = resc->ni(), nj = resc->nj();
  if (ni==0||nj==0||ni<nib_||nj<njb_) return false;
  unsigned nbi = ni/nib_, nbj = nj/njb_;
  vil_image_resource_sptr out_resc =
    vil_new_image_resource(output_path.c_str(), nbi, nbj, 3,
                           VIL_PIXEL_FORMAT_FLOAT,
                           "tiff");
  unsigned i0 = 0, j0 = 0;
  for (unsigned r = 0; r<nbj; ++r, j0+=njb_) {
    i0 = 0;
    vil_image_view<float> row(nbi, 1, 3);
    for (unsigned c = 0; c<nbi; ++c, i0+=nib_)
    {
      vil_image_view_base_sptr view_ptr =
        resc->get_view(i0, nib_, j0, njb_);
      vil_image_view<float> fview = vil_convert_cast(float(), view_ptr);
      vnl_vector<double> v = funct_(fview);
      float eig0 = static_cast<float>(dot_product(v, v0));
      float eig1 = static_cast<float>(dot_product(v, v1));
      float eig2 = static_cast<float>(dot_product(v, v2));
      row(c, 0, 0) = eig0; row(c, 0, 1) = eig1; row(c, 0, 2) = eig2;
    }
    out_resc->put_view(row, 0, r);
    std::cout << '.'<< std::flush;
  }
  std::cout << '\n' << std::flush;
  return true;
}

template <class T>
bool brad_eigenspace<T>::
compute_eigenimage_pixel(vil_image_view<float> const& input,
                         vil_image_view<float>& eignimage)
{
  if (!eigensystem_valid_)
    return false;
  unsigned n = funct_.size();
  vnl_vector<double> v(n);
  vnl_vector<double> v0 = eigenvectors_.get_column(n-1);
  vnl_vector<double> v1 = eigenvectors_.get_column(n-2);
  vnl_vector<double> v2 = eigenvectors_.get_column(n-3);
  float l0 = static_cast<float>(eigenvalues_[n-1]);
  float l1 = static_cast<float>(eigenvalues_[n-2]);
  float l2 = static_cast<float>(eigenvalues_[n-3]);
  unsigned ni = input.ni(), nj = input.nj();
  if (ni==0||nj==0||ni<nib_||nj<njb_) return false;
  eignimage.set_size(ni, nj, 3);
  vil_image_resource_sptr resc = vil_new_image_resource_of_view(input);
  unsigned left_half = nib_/2, top_half = njb_/2;
  unsigned right_half = nib_-left_half;
  unsigned bottom_half = njb_-top_half;
  for (unsigned r = top_half; r<nj-bottom_half; ++r) {
    for (unsigned c = left_half; c<ni-right_half; ++c)
    {
      vil_image_view_base_sptr view_ptr =
        resc->get_view(c-left_half, nib_, r-top_half, njb_);
      vil_image_view<float> fview = vil_convert_cast(float(), view_ptr);
      vnl_vector<double> v = funct_(fview);
      float eig0 = static_cast<float>(dot_product(v, v0));
      float eig1 = static_cast<float>(dot_product(v, v1));
      float eig2 = static_cast<float>(dot_product(v, v2));
      eignimage(c, r, 1)=eig0/l0;  eignimage(c, r, 0)=eig1/l1;
      eignimage(c, r, 2)=eig2/l2;
    }
    if (r%10==0)
      std::cout << '.'<< std::flush;
  }
  std::cout << '\n' << std::flush;
  return true;
}

template <class T>
bool brad_eigenspace<T>::
classify_image(vil_image_resource_sptr const& resc,
               bsta_joint_histogram_3d<float> const& no_atmos,
               bsta_joint_histogram_3d<float> const& atmos,
               unsigned nit, unsigned njt,
               std::string const& output_path)
{
  if (!eigensystem_valid_)
    return false;
  unsigned n = funct_.size();
  vnl_vector<double> v(n);
  vnl_vector<double> v0 = eigenvectors_.get_column(n-1);
  vnl_vector<double> v1 = eigenvectors_.get_column(n-2);
  vnl_vector<double> v2 = eigenvectors_.get_column(n-3);
  unsigned ni = resc->ni(), nj = resc->nj();
  if (ni==0||nj==0||ni<nib_||nj<njb_) return false;
  unsigned nbi = ni/nib_, nbj = nj/njb_;
  vil_image_resource_sptr out_resc =
    vil_new_image_resource(output_path.c_str(), nbi, nbj,1,
                           VIL_PIXEL_FORMAT_FLOAT,
                           "tiff");
  vil_blocked_image_resource_sptr bresc =
    vil_new_blocked_image_facade(resc, nit, njt);
  vil_blocked_image_resource_sptr cbresc =
    vil_new_cached_image_resource(bresc);
  unsigned i0 = 0, j0 = 0;
  for (unsigned r = 0; r<nbj; ++r, j0+=njb_) {
    i0 = 0;
    vil_image_view<float> row(nbi, 1, 1);
    for (unsigned c = 0; c<nbi; ++c, i0+=nib_)
    {
      vil_image_view_base_sptr view_ptr =
        cbresc->get_view(i0, nib_, j0, njb_);
      vil_image_view<float> fview = vil_convert_cast(float(), view_ptr);
      vnl_vector<double> v = funct_(fview);
      float eig0 = static_cast<float>(dot_product(v, v0));
      float eig1 = static_cast<float>(dot_product(v, v1));
      float eig2 = static_cast<float>(dot_product(v, v2));
      float q = bayes(eig0, eig1, eig2, no_atmos, atmos);
      row(c, 0)=q;
    }
    out_resc->put_view(row, 0, r);
    std::cout << '.'<< std::flush;
  }
  std::cout << '\n' << std::flush;
  return true;
}
template <class T>
bool brad_eigenspace<T>::
classify_image(vil_image_resource_sptr const& resc,
               bsta_joint_histogram_3d<float> const& no_atmos,
               bsta_joint_histogram_3d<float> const& atmos,
               unsigned nit, unsigned njt,
               vil_image_resource_sptr& out_resc,
               vil_image_resource_sptr& out_resc_orig_size)
{
  if(!eigensystem_valid_)
    return false;
  unsigned n = funct_.size();
  vnl_vector<double> v(n);
  vnl_vector<double> v0 = eigenvectors_.get_column(n-1);
  vnl_vector<double> v1 = eigenvectors_.get_column(n-2);
  vnl_vector<double> v2 = eigenvectors_.get_column(n-3);
  unsigned ni = resc->ni(), nj = resc->nj();
  if(ni==0||nj==0||ni<nib_||nj<njb_) return false;
  unsigned nbi = ni/nib_, nbj = nj/njb_;

  vil_image_view<float> out_r(nbi, nbj, 1);
  out_resc = vil_new_image_resource_of_view(out_r);

  vil_image_view<float> out_orig(ni, nj, 1);
  out_resc_orig_size = vil_new_image_resource_of_view(out_orig);

  vil_blocked_image_resource_sptr bresc =
    vil_new_blocked_image_facade(resc, nit, njt);
  vil_blocked_image_resource_sptr cbresc =
    vil_new_cached_image_resource(bresc);

  unsigned i0 = 0, j0 = 0;
  vil_image_view<float> temp(nib_, njb_);
  for(unsigned r = 0; r<nbj; ++r, j0+=njb_){
    i0 = 0;
    vil_image_view<float> row(nbi, 1, 1);
    for(unsigned c = 0; c<nbi; ++c, i0+=nib_)
      {
        vil_image_view_base_sptr view_ptr =
          cbresc->get_view(i0, nib_, j0, njb_);
        vil_image_view<float> fview = vil_convert_cast(float(), view_ptr);
        vnl_vector<double> v = funct_(fview);
        float eig0 = static_cast<float>(dot_product(v, v0));
        float eig1 = static_cast<float>(dot_product(v, v1));
        float eig2 = static_cast<float>(dot_product(v, v2));
        float q = bayes(eig0, eig1, eig2, no_atmos, atmos);
        row(c, 0)=q;
        temp.fill(q);
        out_resc_orig_size->put_view(temp, i0, j0);
      }
    out_resc->put_view(row, 0, r);
    std::cout << '.'<< std::flush;
  }
  std::cout << '\n' << std::flush;
  return true;
}

template <class T>
bool brad_eigenspace<T>::
classify_image_pixel(vil_image_view<float> const& image,
                     bsta_joint_histogram_3d<float> const& no_atmos,
                     bsta_joint_histogram_3d<float> const& atmos,
                     float prob_ratio,
                     vil_image_view<float>& class_image)
{
  if (!eigensystem_valid_)
    return false;
  unsigned n = funct_.size();
  vnl_vector<double> v(n);
  vnl_vector<double> v0 = eigenvectors_.get_column(n-1);
  vnl_vector<double> v1 = eigenvectors_.get_column(n-2);
  vnl_vector<double> v2 = eigenvectors_.get_column(n-3);
  unsigned ni = image.ni(), nj = image.nj();
  if (ni==0||nj==0||ni<nib_||nj<njb_) return false;
  class_image.set_size(ni, nj);
  vil_image_resource_sptr resc = vil_new_image_resource_of_view(image);
  unsigned left_half = nib_/2, top_half = njb_/2;
  unsigned right_half = nib_-left_half;
  unsigned bottom_half = njb_-top_half;
  for (unsigned r = top_half; r<nj-bottom_half; ++r) {
    for (unsigned c = left_half; c<ni-right_half; ++c)
    {
      vil_image_view_base_sptr view_ptr =
        resc->get_view(c-left_half, nib_, r-top_half, njb_);
      vil_image_view<float> fview = vil_convert_cast(float(), view_ptr);
      vnl_vector<double> v = funct_(fview);
      float eig0 = static_cast<float>(dot_product(v, v0));
      float eig1 = static_cast<float>(dot_product(v, v1));
      float eig2 = static_cast<float>(dot_product(v, v2));
      float q = bayes(eig0, eig1, eig2, no_atmos, atmos, prob_ratio);
      class_image(c, r) = q;
    }
    if (r%10==0)
      std::cout << '.'<< std::flush;
  }
  std::cout << '\n' << std::flush;
  return true;
}

template <class T>
bool brad_eigenspace<T>::
init_histogram(vil_image_resource_sptr const& resc, unsigned nbins,
               bsta_joint_histogram_3d<float>& hist)
{
  if (!eigensystem_valid_)
    return false;
  std::cout << "intializing eigenvalue histogram\n";
  unsigned n = funct_.size();
  vnl_vector<double> v(n), minv(n), maxv(n);
  minv.fill(vnl_numeric_traits<float>::maxval);
  maxv.fill(0.0f);
  vnl_vector<double> v0 = eigenvectors_.get_column(n-1);
  vnl_vector<double> v1 = eigenvectors_.get_column(n-2);
  vnl_vector<double> v2 = eigenvectors_.get_column(n-3);
  unsigned ni = resc->ni(), nj = resc->nj();
  if (ni==0||nj==0||ni<nib_||nj<njb_) return false;
  unsigned nbi = ni/nib_, nbj = nj/njb_;
  unsigned i0 = 0, j0 = 0;
  for (unsigned r = 0; r<nbj; ++r, j0+=njb_) {
    i0 = 0;
    for (unsigned c = 0; c<nbi; ++c, i0+=nib_)
    {
      vil_image_view_base_sptr view_ptr =
        resc->get_view(i0, nib_, j0, njb_);
      vil_image_view<float> fview = vil_convert_cast(float(), view_ptr);
      vnl_vector<double> v = funct_(fview);
      float eig0 = static_cast<float>(dot_product(v, v0));
      float eig1 = static_cast<float>(dot_product(v, v1));
      float eig2 = static_cast<float>(dot_product(v, v2));
      if (eig0<minv[0]) minv[0]=eig0; if (eig1<minv[1]) minv[1]=eig1;
      if (eig2<minv[2]) minv[2]=eig2;
      if (eig0>maxv[0]) maxv[0]=eig0; if (eig1>maxv[1]) maxv[1]=eig1;
      if (eig2>maxv[2]) maxv[2]=eig2;
    }
    std::cout << '.'<< std::flush;
  }
  std::cout << '\n' << std::flush;
  vnl_vector<double> delta = (maxv-minv)/static_cast<float>(nbins);
  float min0 = static_cast<float>(minv[0]-delta[0]);
  float max0 = static_cast<float>(maxv[0]+delta[0]);
  float min1 = static_cast<float>(minv[1]-delta[1]);
  float max1 = static_cast<float>(maxv[1]+delta[1]);
  float min2 = static_cast<float>(minv[2]-delta[2]);
  float max2 = static_cast<float>(maxv[2]+delta[2]);
  hist = bsta_joint_histogram_3d<float>(min0, max0, nbins,
                                        min1, max1, nbins,
                                        min2, max2, nbins);
  std::cout << " not blocked " << min0 << ' ' << max0 << '\n' << std::flush;
  return true;
}

static
bool get_view(vil_image_resource_sptr const& ir,
              unsigned ni, unsigned nj,
              vil_image_view<float>& view,
              bool& done)
{
  static bool first = true;
  done = false;
  static unsigned cbi = 0, cbj = 0;
  static unsigned i0 = 0, j0 = 0;
  static unsigned bi = 0, bj = 0;
  static vil_image_view<float> cblock;
  static vil_blocked_image_resource_sptr bif = nullptr;
  if (first) {
    unsigned si = ni*16, sj = nj*16;
    bif = vil_new_blocked_image_facade(ir, si, sj);
  }
  unsigned rni = ir->ni(), rnj = ir->nj();
  unsigned sbi = bif->size_block_i();
  unsigned sbj = bif->size_block_j();
  unsigned nbi = bif->n_block_i();
  if (rni%sbi!=0) nbi--;//discard partial blocks
  unsigned nbj = bif->n_block_j();
  if (rnj%sbj!=0) nbj--;//discard partial blocks
  view.set_size(ni, nj);
  if (first) {
    vil_image_view_base_sptr cblock_ptr = bif->get_block(bi, bj);
    cblock = vil_convert_cast(float(), cblock_ptr);
    cbi = bi; cbj =bj;
    first = false;
  }
  else if (bi!=cbi||bj!=cbj) {
    vil_image_view_base_sptr cblock_ptr = bif->get_block(bi, bj);
    cblock = vil_convert_cast(float(), cblock_ptr);
    cbi = bi; cbj =bj;
  }
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
      view(i,j) = cblock(i0+i, j0+j);

  if (i0+ni>=sbi) {
    i0 = 0;
    if (j0+nj>=sbj) {
      if ((bi+1)>=nbi) {
        bi = 0;
        if ((bj+1)>=nbj) {
          done = true;
          std::cout << '\n' << std::flush;
          return true;
        }
        ++bj; j0 = 0;
        std::cout << bj << ' '<< std::flush;
      }
      else {
        ++bi; j0=0;
      }
    }
    else
      j0+=nj;
  }
  else
    i0+=ni;
  return true;
}

template <class T>
bool brad_eigenspace<T>::
update_histogram(vil_image_resource_sptr const& resc,
                 bsta_joint_histogram_3d<float>& hist)
{
  if (!eigensystem_valid_)
    return false;
  std::cout << "updating eigenvalue histogram\n";

  unsigned n = funct_.size();
  vnl_vector<double> v(n);
  vnl_vector<double> v0 = eigenvectors_.get_column(n-1);
  vnl_vector<double> v1 = eigenvectors_.get_column(n-2);
  vnl_vector<double> v2 = eigenvectors_.get_column(n-3);
  bool done = false;
  vil_image_view<float> fview;
  while (!done)
  {
    if (!get_view(resc, nib_, njb_, fview, done))
      return false;
    vnl_vector<double> v = funct_(fview);
    float eig0 = static_cast<float>(dot_product(v, v0));
    float eig1 = static_cast<float>(dot_product(v, v1));
    float eig2 = static_cast<float>(dot_product(v, v2));
    hist.upcount(eig0, 0.333f, eig1, 0.333f, eig2, 0.333f);
  }
  std::cout << '\n' << std::flush;
  return true;
}

#if 0 // old implementation
template <class T>
bool brad_eigenspace<T>::
update_histogram(vil_image_resource_sptr const& resc,
                 bsta_joint_histogram_3d<float>& hist)
{
  if (!eigensystem_valid_)
    return false;
  std::cout << "updating eigenvalue histogram\n";

  unsigned n = funct_.size();
  vnl_vector<double> v(n);
  vnl_vector<double> v0 = eigenvectors_.get_column(n-1);
  vnl_vector<double> v1 = eigenvectors_.get_column(n-2);
  vnl_vector<double> v2 = eigenvectors_.get_column(n-3);
  unsigned ni = resc->ni(), nj = resc->nj();
  if (ni==0||nj==0||ni<nib_||nj<njb_) return false;
  unsigned nbi = ni/nib_, nbj = nj/njb_;
  unsigned i0 = 0, j0 = 0;
  for (unsigned r = 0; r<nbj; ++r, j0+=njb_) {
    i0 = 0;
    for (unsigned c = 0; c<nbi; ++c, i0+=nib_)
    {
      vil_image_view_base_sptr view_ptr =
        bif->get_view(i0, nib_, j0, njb_);
      vil_image_view<float> fview = vil_convert_cast(float(), view_ptr);
      vnl_vector<double> v = funct_(fview);
      float eig0 = static_cast<float>(dot_product(v, v0));
      float eig1 = static_cast<float>(dot_product(v, v1));
      float eig2 = static_cast<float>(dot_product(v, v2));
      hist.upcount(eig0, 0.333f, eig1, 0.333f, eig2, 0.333f);
    }
    std::cout << '.'<< std::flush;
  }
  std::cout << '\n' << std::flush;
  return true;
}
#endif

template <class T>
bool brad_eigenspace<T>::
init_histogram(std::vector<vil_image_resource_sptr> const& rescs,
               unsigned nbins, bsta_joint_histogram_3d<float>& hist)
{
  if (!eigensystem_valid_)
    return false;
  std::cout << "intializing eigenvalue histogram\n";
  unsigned n = funct_.size();
  vnl_vector<double> v(n), minv(n), maxv(n);
  minv.fill(vnl_numeric_traits<float>::maxval);
  maxv.fill(0.0f);
  vnl_vector<double> v0 = eigenvectors_.get_column(n-1);
  vnl_vector<double> v1 = eigenvectors_.get_column(n-2);
  vnl_vector<double> v2 = eigenvectors_.get_column(n-3);
  for (const auto & resc : rescs) {
    unsigned ni = resc->ni(), nj = resc->nj();
    if (ni==0||nj==0||ni<nib_||nj<njb_) return false;
    unsigned nbi = ni/nib_, nbj = nj/njb_;
    unsigned i0 = 0, j0 = 0;
    for (unsigned r = 0; r<nbj; ++r, j0+=njb_) {
      i0 = 0;
      for (unsigned c = 0; c<nbi; ++c, i0+=nib_)
      {
        vil_image_view_base_sptr view_ptr =
          resc->get_view(i0, nib_, j0, njb_);
        vil_image_view<float> fview = vil_convert_cast(float(), view_ptr);
        vnl_vector<double> v = funct_(fview);
        float eig0 = static_cast<float>(dot_product(v, v0));
        float eig1 = static_cast<float>(dot_product(v, v1));
        float eig2 = static_cast<float>(dot_product(v, v2));
        if (eig0<minv[0]) minv[0]=eig0; if (eig1<minv[1]) minv[1]=eig1;
        if (eig2<minv[2]) minv[2]=eig2;
        if (eig0>maxv[0]) maxv[0]=eig0; if (eig1>maxv[1]) maxv[1]=eig1;
        if (eig2>maxv[2]) maxv[2]=eig2;
      }
      std::cout << '.'<< std::flush;
    }
    std::cout << '\n' << std::flush;
  }
  vnl_vector<double> delta = (maxv-minv)/static_cast<float>(nbins);
  float min0 = static_cast<float>(minv[0]-delta[0]);
  float max0 = static_cast<float>(maxv[0]+delta[0]);
  float min1 = static_cast<float>(minv[1]-delta[1]);
  float max1 = static_cast<float>(maxv[1]+delta[1]);
  float min2 = static_cast<float>(minv[2]-delta[2]);
  float max2 = static_cast<float>(maxv[2]+delta[2]);
  hist = bsta_joint_histogram_3d<float>(min0, max0, nbins,
                                        min1, max1, nbins,
                                        min2, max2, nbins);
  return true;
}

template <class T>
bool brad_eigenspace<T>::
update_histogram(std::vector<vil_image_resource_sptr> const& rescs,
                 bsta_joint_histogram_3d<float>& hist)
{
  for (const auto & resc : rescs)
    if (!this->update_histogram(resc, hist))
      return false;
  return true;
}

template <class T>
bool brad_eigenspace<T>::
init_histogram_rand(std::vector<vil_image_resource_sptr> const& rescs,
                    unsigned nbins,
                    bsta_joint_histogram_3d<float>& hist,
                    double frac, unsigned nit, unsigned njt)
{
  if (!eigensystem_valid_)
    return false;
  std::vector<vil_image_resource_sptr>::const_iterator rit = rescs.begin();
  double area = 0.0;
  for (unsigned i = 0; rit!= rescs.end(); ++rit, ++i) {
    unsigned ni = (*rit)->ni(), nj = (*rit)->nj();
    if (ni<nib_||nj<njb_)
      return false;
    area += ni*nj;
  }
  //the total image area to be processed
  double proc_area = area*frac;
  // the number of random tiles to be processed
  double nitd = nit, njtd = njt;
  unsigned ntiles = static_cast<unsigned>(proc_area/(nitd*njtd));
  if (!ntiles) return false;
  unsigned n = funct_.size();
  if (!n) return false;
  std::cout << "initializing histogram - randomly selecting "
           << ntiles << " (" << nit << 'x' << njt << ") tiles\n" << std::flush;
  print_resource_stats(rescs);

  vnl_vector<double> minv(n), maxv(n);
  minv.fill(vnl_numeric_traits<float>::maxval);
  maxv.fill(0.0f);
  vnl_vector<double> v0 = eigenvectors_.get_column(n-1);
  vnl_vector<double> v1 = eigenvectors_.get_column(n-2);
  vnl_vector<double> v2 = eigenvectors_.get_column(n-3);
  // process the tiles
  double nd = static_cast<double>(rescs.size());
  for (unsigned t = 0; t<ntiles; ++t) {
    //randomly select a resource
    unsigned ires =
      static_cast<unsigned>((nd)*(std::rand()/(RAND_MAX+1.0)));
    //compute random access to tile
    double nid = rescs[ires]->ni(), njd = rescs[ires]->nj();
    double rd = (njd-njtd-1.0)*(std::rand()/(RAND_MAX+1.0));
    double cd = (nid-nitd-1.0)*(std::rand()/(RAND_MAX+1.0));
    if (rd<0) rd = 0.0;
    if (cd<0) cd = 0.0;
    if (rd>(njd-njtd-1.0)) rd = (njd-njtd-1.0);
    if (cd>(nid-nitd-1.0)) cd = (nid-nitd-1.0);
    unsigned j0 = static_cast<unsigned>(rd), i0 = static_cast<unsigned>(cd);
    vil_image_view_base_sptr tile_ptr =
      rescs[ires]->get_view(i0, nit, j0, njt);
    vil_image_view<float> ftile = vil_convert_cast(float(), tile_ptr);
    unsigned nbi = nit/nib_, nbj = njt/njb_;
    i0 = 0; j0=0;
    for (unsigned r = 0; r<nbj; ++r, j0+=njb_) {
      i0 = 0;
      for (unsigned c = 0; c<nbi; ++c, i0+=nib_) {
        vil_image_view<float> fview(nib_, njb_);
        for (unsigned j =0; j<njb_; ++j)
          for (unsigned i =0; i<nib_; ++i)
            fview(i, j) = ftile(i0+i, j0+j);
        vnl_vector<double> v = funct_(fview);
        float eig0 = static_cast<float>(dot_product(v, v0));
        float eig1 = static_cast<float>(dot_product(v, v1));
        float eig2 = static_cast<float>(dot_product(v, v2));
        if (eig0<minv[0]) minv[0]=eig0; if (eig1<minv[1]) minv[1]=eig1;
        if (eig2<minv[2]) minv[2]=eig2;
        if (eig0>maxv[0]) maxv[0]=eig0; if (eig1>maxv[1]) maxv[1]=eig1;
        if (eig2>maxv[2]) maxv[2]=eig2;
      }
    }
    std::cout << '.' << std::flush;
  }
  vnl_vector<double> delta = (maxv-minv)/static_cast<float>(nbins);
  float min0 = static_cast<float>(minv[0]-delta[0]);
  float max0 = static_cast<float>(maxv[0]+delta[0]);
  float min1 = static_cast<float>(minv[1]-delta[1]);
  float max1 = static_cast<float>(maxv[1]+delta[1]);
  float min2 = static_cast<float>(minv[2]-delta[2]);
  float max2 = static_cast<float>(maxv[2]+delta[2]);
  hist = bsta_joint_histogram_3d<float>(min0, max0, nbins,
                                        min1, max1, nbins,
                                        min2, max2, nbins);
  std::cout << '\n' << std::flush;
  return true;
}

template <class T>
bool brad_eigenspace<T>::
init_histogram_blocked(std::vector<vil_image_resource_sptr> const& rescs,
                       unsigned nbins,
                       bsta_joint_histogram_3d<float>& hist,
                       unsigned nit, unsigned njt) {
  if (!eigensystem_valid_)
    return false;
  std::cout << "intializing eigenvalue histogram(blocked)\n";
  unsigned n = funct_.size();
  vnl_vector<double> v(n), minv(n), maxv(n);
  minv.fill(vnl_numeric_traits<float>::maxval);
  maxv.fill(0.0f);
  vnl_vector<double> v0 = eigenvectors_.get_column(n-1);
  vnl_vector<double> v1 = eigenvectors_.get_column(n-2);
  vnl_vector<double> v2 = eigenvectors_.get_column(n-3);
  for (const auto & resc : rescs) {
    unsigned ni = resc->ni(), nj = resc->nj();
    if (ni==0||nj==0||ni<nib_||nj<njb_) return false;
    unsigned nbi = ni/nib_, nbj = nj/njb_;
    vil_blocked_image_resource_sptr bresc =
      vil_new_blocked_image_facade(resc, nit, njt);
    vil_blocked_image_resource_sptr cbresc =
      vil_new_cached_image_resource(bresc);
    unsigned i0 = 0, j0 = 0;
    for (unsigned r = 0; r<nbj; ++r, j0+=njb_) {
      i0 = 0;
      for (unsigned c = 0; c<nbi; ++c, i0+=nib_)
      {
        vil_image_view_base_sptr view_ptr =
          cbresc->get_view(i0, nib_, j0, njb_);
        vil_image_view<float> fview = vil_convert_cast(float(), view_ptr);
        vnl_vector<double> v = funct_(fview);
        float eig0 = static_cast<float>(dot_product(v, v0));
        float eig1 = static_cast<float>(dot_product(v, v1));
        float eig2 = static_cast<float>(dot_product(v, v2));
        if (eig0<minv[0]) minv[0]=eig0; if (eig0>maxv[0]) maxv[0]=eig0;
        if (eig1<minv[1]) minv[1]=eig1; if (eig1>maxv[1]) maxv[1]=eig1;
        if (eig2<minv[2]) minv[2]=eig2; if (eig2>maxv[2]) maxv[2]=eig2;
      }
      std::cout << '.'<< std::flush;
    }
    std::cout << '\n' << std::flush;
  }
  vnl_vector<double> delta = (maxv-minv)/static_cast<float>(nbins);
  float min0 = static_cast<float>(minv[0]-delta[0]);
  float max0 = static_cast<float>(maxv[0]+delta[0]);
  float min1 = static_cast<float>(minv[1]-delta[1]);
  float max1 = static_cast<float>(maxv[1]+delta[1]);
  float min2 = static_cast<float>(minv[2]-delta[2]);
  float max2 = static_cast<float>(maxv[2]+delta[2]);
  hist = bsta_joint_histogram_3d<float>(min0, max0, nbins,
                                        min1, max1, nbins,
                                        min2, max2, nbins);
  return true;
}

template <class T>
bool brad_eigenspace<T>::
update_histogram_rand(std::vector<vil_image_resource_sptr> const& rescs,
                      bsta_joint_histogram_3d<float>& hist,
                      double frac, unsigned nit, unsigned njt)
{
  if (!eigensystem_valid_)
    return false;
  std::vector<vil_image_resource_sptr>::const_iterator rit = rescs.begin();
  double area = 0.0;
  for (unsigned i = 0; rit!= rescs.end(); ++rit, ++i) {
    unsigned ni = (*rit)->ni(), nj = (*rit)->nj();
    if (ni<nib_||nj<njb_)
      return false;
    area += ni*nj;
  }
  //the total image area to be processed
  double proc_area = area*frac;
  // the number of random tiles to be processed
  double nitd = nit, njtd = njt;
  unsigned ntiles = static_cast<unsigned>(proc_area/(nitd*njtd));
  if (!ntiles) return false;

  unsigned n = funct_.size();
  if (!n) return false;
  std::cout << "updating histogram - randomly selecting "
           << ntiles << " (" << nit << 'x' << njt << ") tiles\n" << std::flush;
  print_resource_stats(rescs);

  vnl_vector<double> v0 = eigenvectors_.get_column(n-1);
  vnl_vector<double> v1 = eigenvectors_.get_column(n-2);
  vnl_vector<double> v2 = eigenvectors_.get_column(n-3);
  // process the tiles
  double nd = static_cast<double>(rescs.size());
  for (unsigned t = 0; t<ntiles; ++t) {
    //randomly select a resource
    unsigned ires =
      static_cast<unsigned>((nd)*(std::rand()/(RAND_MAX+1.0)));
    //compute random access to tile
    double nid = rescs[ires]->ni(), njd = rescs[ires]->nj();
    double rd = (njd-njtd-1.0)*(std::rand()/(RAND_MAX+1.0));
    double cd = (nid-nitd-1.0)*(std::rand()/(RAND_MAX+1.0));
    if (rd<0) rd = 0.0; else if (rd>njd-njtd-1.0) rd = njd-njtd-1.0;
    if (cd<0) cd = 0.0; else if (cd>nid-nitd-1.0) cd = nid-nitd-1.0;
    unsigned j0 = static_cast<unsigned>(rd), i0 = static_cast<unsigned>(cd);
    vil_image_view_base_sptr tile_ptr = rescs[ires]->get_view(i0, nit, j0, njt);
    vil_image_view<float> ftile = vil_convert_cast(float(), tile_ptr);
    unsigned nbi = nit/nib_, nbj = njt/njb_;
    i0 = 0; j0=0;
    for (unsigned r=0; r<nbj; ++r, j0+=njb_) {
      i0 = 0;
      for (unsigned c=0; c<nbi; ++c, i0+=nib_) {
        vil_image_view<float> fview(nib_, njb_);
        for (unsigned j=0; j<njb_; ++j)
          for (unsigned i=0; i<nib_; ++i)
            fview(i, j) = ftile(i0+i, j0+j);
        vnl_vector<double> v = funct_(fview);
        float eig0 = static_cast<float>(dot_product(v, v0));
        float eig1 = static_cast<float>(dot_product(v, v1));
        float eig2 = static_cast<float>(dot_product(v, v2));
        hist.upcount(eig0, 0.333f, eig1, 0.333f, eig2, 0.333f);
      }
    }
    std::cout << '.' << std::flush;
  }
  std::cout << '\n' << std::flush;
  return true;
}

template <class T>
bool brad_eigenspace<T>::
update_histogram_blocked(std::vector<vil_image_resource_sptr> const& rescs,
                         bsta_joint_histogram_3d<float>& hist,
                         unsigned nit, unsigned njt) {
  for (const auto & resc : rescs) {
    vil_blocked_image_resource_sptr bresc =
      vil_new_blocked_image_facade(resc, nit, njt);
    vil_blocked_image_resource_sptr cbresc =
      vil_new_cached_image_resource(bresc);
    if (!eigensystem_valid_)
      return false;
    std::cout << "updating eigenvalue histogram(blocked)\n";

    unsigned n = funct_.size();
    vnl_vector<double> v(n);
    vnl_vector<double> v0 = eigenvectors_.get_column(n-1);
    vnl_vector<double> v1 = eigenvectors_.get_column(n-2);
    vnl_vector<double> v2 = eigenvectors_.get_column(n-3);
    unsigned ni = resc->ni(), nj = resc->nj();
    if (ni==0||nj==0||ni<nib_||nj<njb_) return false;
    unsigned nbi = ni/nib_, nbj = nj/njb_;
    unsigned i0 = 0, j0 = 0;
    for (unsigned r = 0; r<nbj; ++r, j0+=njb_) {
      i0 = 0;
      for (unsigned c = 0; c<nbi; ++c, i0+=nib_)
      {
        vil_image_view_base_sptr view_ptr =
          cbresc->get_view(i0, nib_, j0, njb_);
        vil_image_view<float> fview = vil_convert_cast(float(), view_ptr);
        vnl_vector<double> v = funct_(fview);
        float eig0 = static_cast<float>(dot_product(v, v0));
        float eig1 = static_cast<float>(dot_product(v, v1));
        float eig2 = static_cast<float>(dot_product(v, v2));
        hist.upcount(eig0, 0.333f, eig1, 0.333f, eig2, 0.333f);
      }
      std::cout << '.' << std::flush;
    }
    std::cout << '\n' << std::flush;
  }
  return true;
}

template <class T>
void vsl_b_write(vsl_b_ostream &os, const brad_eigenspace<T>& ep)
{
  vsl_b_write(os, ep.nib());
  vsl_b_write(os, ep.njb());
  vsl_b_write(os, ep.mean());
  vsl_b_write(os, ep.covariance());
  vsl_b_write(os, ep.eigenvalues());
  vsl_b_write(os, ep.eigenvectors());
  vsl_b_write(os, ep.functor());
}


//: Binary load brad_eigenspace from stream.
template <class T>
void vsl_b_read(vsl_b_istream &is, brad_eigenspace<T>& ep)
{
  unsigned nib, njb;
  vnl_vector<double> mean, eigenvals;
  vnl_matrix<double> covar, eigenvecs;
  T funct;
  vsl_b_read(is, nib);
  vsl_b_read(is, njb);
  vsl_b_read(is, mean);
  vsl_b_read(is, covar);
  vsl_b_read(is, eigenvals);
  vsl_b_read(is, eigenvecs);
  vsl_b_read(is, funct);
  ep.set_nib(nib); ep.set_njb(njb);
  ep.set_mean_covar(mean, covar);
  ep.set_eigensystem(eigenvals, eigenvecs);
  ep.set_functor(funct);
}

template <class T>
bool brad_eigenspace<T>::print(std::ostream& os) const
{
  os << "image block size(" << nib_ << ' ' << njb_ << ")\n"
     << "feature vector size " << funct_.size() << '\n'
     << "feature vector type [" << funct_.type() << "]\n";
  if (eigensystem_valid_) {
    os << "eigenvalues==>\n";
    unsigned nv = eigenvalues_.size();
    unsigned nrows = nv/5;
    unsigned rem = nv%5;
    for (unsigned r = 0, i = 0; r<nrows; ++r, i+=5)
      os << eigenvalues_[i] << ' '
         << eigenvalues_[i+1] << ' '
         << eigenvalues_[i+2] << ' '
         << eigenvalues_[i+3] << ' '
         << eigenvalues_[i+4] << '\n';
    if (rem) {
      for (unsigned i = 5*nrows; i<5*nrows+rem; ++i)
        os << eigenvalues_[i] << ' ';
      os << '\n';
    }
    os << "Eigenvector of largest eigenvalue\n";
    unsigned n = funct_.size();
    vnl_vector<double> v(n);
    vnl_vector<double> v0 = eigenvectors_.get_column(n-1);
    os << v0 << '\n';
  }
  return true;
}

//: Print summary
template <class T>
void
vsl_print_summary(std::ostream &os, const brad_eigenspace<T>& ep)
{
  ep.print(os);
}

#undef BRAD_EIGENSPACE_INSTANTIATE
#define BRAD_EIGENSPACE_INSTANTIATE(T) \
template class brad_eigenspace<T >; \
template void vsl_print_summary(std::ostream&,brad_eigenspace<T > const&); \
template void vsl_b_read(vsl_b_istream&,brad_eigenspace<T >&); \
template void vsl_b_write(vsl_b_ostream &,brad_eigenspace<T > const&)

#endif // brad_eigenspace_hxx_
