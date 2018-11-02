//this is /brl/bbas/bsta/algo/bsta_mvnrand.h
#ifndef BSTA_MVNRAND_H_
#define BSTA_MVNRAND_H_

#include"bsta/bsta_gaussian_full.h"
#include"bsta/bsta_gaussian_indep.h"
#include"bsta/bsta_gaussian_sphere.h"

#include <cmath>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include"vnl/algo/vnl_cholesky.h"
#include"vnl/vnl_matrix_fixed.h"
#include"vnl/vnl_random.h"
#include"vnl/vnl_vector_fixed.h"

//:
// \file
// \brief Gaussian sampling templated by bsta_gaussian types
// \author Brandon A. Mayer (brandon_mayer@brown.edu)
// \date May 30, 2013


// generic interface
template<template<class, unsigned> class G, class T, unsigned n>
class bsta_mvnrand
{
private:
  bsta_mvnrand(){}
  ~bsta_mvnrand(){}
};


//partial specialization for full covariance
template<class T, unsigned n>
class bsta_mvnrand<bsta_gaussian_full, T, n>
{
public:
  typedef typename bsta_gaussian_full<T,n>::vector_type vector_type;
  typedef typename bsta_gaussian_full<T,n>::covar_type covar_type;

  bsta_mvnrand(){}

  ~bsta_mvnrand(){}

  bsta_mvnrand(const bsta_gaussian_full<T,n>& gauss):
    mean_(gauss.mean()){this->factor_covar(gauss.covar());}


  bsta_mvnrand(const vector_type& mean, const covar_type& covar):
    mean_(mean){this->factor_covar(covar);}

  //get copy of mean
  vector_type mean() const {return this->mean_;}

  //get copy of L
  vnl_matrix<double> covar_lower_triangle() const {return this->L_;}

  //set mean
  void set_mean(const vector_type& mean)
    {this->mean_ = mean;}

  //reset covar (L)
  void set_covar(const covar_type& covar)
    {this->factor_covar(covar);}

  //will return samples in 2D matrix of size nsamples x n
  void operator()(const unsigned nsamples, vnl_matrix<T>& samples)
  {
    samples.set_size(nsamples, n);
    for(unsigned r = 0; r < nsamples; ++r){
      vnl_vector<double> isamples(n,0.0);
      for(unsigned c = 0; c < n; ++c)
        isamples[c] = vrand_.normal();
      vnl_vector<double> row = this->L_ * isamples;
      for(unsigned c = 0; c < n; ++c)
        samples[r][c] = this->mean_[c] + static_cast<T>(row[c]);
    }
  }

private:
  void factor_covar(const covar_type& covar)
  {
    //vnl cholesky object only takes matricies of type double.
    //Is there a better way?
    vnl_matrix<double> dcovar(n,n,0.0);
    for(unsigned r = 0; r < n; ++r)
      for(unsigned c = 0; c < n; ++c)
        dcovar[r][c] = static_cast<double>(covar[r][c]);

    vnl_cholesky chol(dcovar);
    this->L_ = chol.lower_triangle();
  }

  vector_type mean_;
  vnl_matrix<double> L_;
  vnl_random vrand_;
};

//specialization for independent gaussian
template<class T, unsigned n>
class bsta_mvnrand<bsta_gaussian_indep, T, n>
{
public:
  typedef typename bsta_gaussian_indep<T,n>::covar_type covar_type;
  typedef typename bsta_gaussian_indep<T,n>::vector_type vector_type;

  bsta_mvnrand(){}

  bsta_mvnrand(const bsta_gaussian_indep<T,n>& gauss):mean_(gauss.mean()){
    covar_type covar = gauss.covar();
    for(unsigned i = 0; i < n; ++i){
      diag_std_[i] = std::sqrt(covar[i]);}
  }

  bsta_mvnrand(const vector_type& mean, const covar_type& covar):mean_(mean){
    for(unsigned i = 0; i < n; ++i){
      diag_std_[i] = std::sqrt(covar[i]);}
  }

  ~bsta_mvnrand(){}

  vector_type mean() const {return this->mean_;}

  covar_type std() const {return this->diag_std_;}

  void set_mean(const vector_type& mean){this->mean_ = mean;}

  void set_std(const covar_type& diag_std_){this->diag_std_ = diag_std_;}

  void operator()(const unsigned nsamples, vnl_matrix<T>& samples){
    samples.set_size(nsamples, n);
    for(unsigned r = 0; r < nsamples; ++r)
      for(unsigned c = 0; c < n; ++c)
        samples[r][c] = this->mean_[c] + vrand_.normal()*this->diag_std_[c];
  }

private:
  vector_type mean_;
  covar_type diag_std_;
  vnl_random vrand_;
};

//specialization for spherical gaussian
template<class T, unsigned n>
class bsta_mvnrand<bsta_gaussian_sphere, T, n>
{
public:
  typedef typename bsta_gaussian_sphere<T,n>::vector_type vector_type;
  typedef typename bsta_gaussian_sphere<T,n>::covar_type covar_type;

  bsta_mvnrand(){}

  bsta_mvnrand(const bsta_gaussian_sphere<T, n>& gauss):
    mean_(gauss.mean()), std_(std::sqrt(gauss.var())){}

  bsta_mvnrand(const vector_type& mean, const covar_type& std):
    mean_(mean), std_(std){}

  ~bsta_mvnrand(){}

  vector_type mean() const {return this->mean_;}

  covar_type std() const {return this->std_;}

  void set_mean(const vector_type& mean){this->mean_ = mean;}

  void set_std(const covar_type& std){this->std_ = std;}

  void operator()(const unsigned nsamples, vnl_matrix<T>& samples){
    samples.set_size(nsamples, n);
    for(unsigned r = 0; r < nsamples; ++r)
      for(unsigned c = 0; c < n; ++c)
        samples[r][c] = this->mean_[c] + vrand_.normal()*this->std_;
  }

private:
  vector_type mean_;
  covar_type std_;
  vnl_random vrand_;
};

#endif
