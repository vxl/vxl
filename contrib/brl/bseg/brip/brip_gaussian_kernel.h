// This is brl/bseg/brip/brip_gaussian_kernel.h
#ifndef brip_gaussian_kernel_h
#define brip_gaussian_kernel_h
//:
// \file
// \brief Gaussian derivative kernels
// \author Amir Tamrakar
// \date 9 Sept 2006
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <vector>
#include <brip/brip_kernel.h>
#include <vnl/vnl_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Gaussian derivative kernel base class
class brip_gaussian_kernel : public brip_kernel
{
 public:
  int khs;                     //kernel half size
  std::vector<double> K_x, K_y; //separated kernels to minimize computation

 protected:
  double sigma; ///< operator sigma

 public:
  //: constructor given sigma and shifts
  brip_gaussian_kernel(double sigma_, double dx_=0.0, double dy_=0.0, double theta_=0.0):
    brip_kernel((unsigned)(2*std::ceil(4*sigma_)+1), (unsigned)(2*std::ceil(4*sigma_)+1), dx_, dy_, theta_),
    khs((int) std::ceil(4*sigma_)), K_x(2*khs+1, 0.0), K_y(2*khs+1, 0.0), sigma(sigma_)
  {
    compute_kernel();
  }
  //: destructor
  ~brip_gaussian_kernel() override= default;

  //: compute the kernel
  virtual void compute_kernel(bool /*separated_kernels_only*/=false){}

  //: recompute kernel with given subpixel shifts
  void recompute_kernel(double dx_=0.0, double dy_=0.0, double theta_=0.0) override
  {
    dx = dx_;
    dy = dy_;
    theta = theta_;
    compute_kernel();
  }
};

//: Gaussian Left half kernel at the given orientation
//  Note: not separable
class brip_G_Lhalf_kernel : public brip_gaussian_kernel
{
 public:
  brip_G_Lhalf_kernel(double sigma_, double dx_=0.0, double dy_=0.0, double theta_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_, theta_){}
  ~brip_G_Lhalf_kernel() override= default;

  //: compute the kernel
  void compute_kernel(bool /*separated_kernels_only*/=false) override
  {
    double ssq = sigma*sigma;
    //double pisig2 = vnl_math::twopi*ssq;
    double cc = vnl_math::sqrt2pi*ssq*sigma;

    //not separable
    for (int x = -khs; x <= khs; x++){
      for (int y = -khs; y <= khs; y++){

        double xx = x* std::cos(theta) + y*std::sin(theta) - dx;
        double yy = x*-std::sin(theta) + y*std::cos(theta) - dy;

        //only one half is a Gaussian (the other half is zeros)
        if (yy>=0)
          top_left_[(x+khs)*istep_+ (y+khs)*jstep_] = 0.0;
        else
          //top_left_[(x+khs)*istep_+ (y+khs)*jstep_] = 2*std::exp(-xx*xx/(2*ssq))*std::exp(-yy*yy/(2*ssq))/pisig2;
          top_left_[(x+khs)*istep_+ (y+khs)*jstep_] = std::exp(-xx*xx/(2*ssq))*yy*-std::exp(-yy*yy/(2*ssq))/cc;
      }
    }
  }
};

//: Gaussian Right half kernel at the given orientation
//  Note: not separable
class brip_G_Rhalf_kernel : public brip_gaussian_kernel
{
 public:
  brip_G_Rhalf_kernel(double sigma_, double dx_=0.0, double dy_=0.0, double theta_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_, theta_){}
  ~brip_G_Rhalf_kernel() override= default;

  //: compute the kernel
  void compute_kernel(bool /*separated_kernels_only*/=false) override
  {
    double ssq = sigma*sigma;
    //double pisig2 = vnl_math::twopi*ssq;
    double cc = vnl_math::sqrt2pi*ssq*sigma;

    //not separable
    for (int x = -khs; x <= khs; x++){
      for (int y = -khs; y <= khs; y++){

        double xx = x* std::cos(theta) + y*std::sin(theta) - dx;
        double yy = x*-std::sin(theta) + y*std::cos(theta) - dy;

        //only one half is a Gaussian (the other half is zeros)
        if (yy<0)
          top_left_[(x+khs)*istep_+ (y+khs)*jstep_] = 0.0;
        else
          //top_left_[(x+khs)*istep_+ (y+khs)*jstep_] = 2*std::exp(-xx*xx/(2*ssq))*std::exp(-yy*yy/(2*ssq))/pisig2;
          top_left_[(x+khs)*istep_+ (y+khs)*jstep_] = std::exp(-xx*xx/(2*ssq))*yy*std::exp(-yy*yy/(2*ssq))/cc;
      }
    }
  }
};

//: simple Gaussian smoothing kernel
//  K_x = G_x, K_y = G_y
class brip_G_kernel : public brip_gaussian_kernel
{
 public:
  brip_G_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_G_kernel() override= default;

  //: compute the kernel
  void compute_kernel(bool separated_kernels_only=false) override
  {
    double ssq = sigma*sigma;
    double sq2pisig = vnl_math::sqrt2pi*sigma;

    //1-d kernels
    for (int x = -khs; x <= khs; x++)
      K_x[x+khs] = std::exp(-(x-dx)*(x-dx)/(2*ssq))/sq2pisig;
    for (int y = -khs; y <= khs; y++)
      K_y[y+khs] = std::exp(-(y-dy)*(y-dy)/(2*ssq))/sq2pisig;

    if (!separated_kernels_only){
      for (unsigned i=0; i<ni_; i++){
        for (unsigned j=0; j<nj_; j++){
          top_left_[i*istep_+j*jstep_] = K_x[i]*K_y[j];
        }
      }
    }
  }
};

//: Gx kernel
//  K_x = dG_x, K_y = G_y
class brip_Gx_kernel : public brip_gaussian_kernel
{
 public:
  brip_Gx_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_Gx_kernel() override= default;

  //: compute the kernel
  void compute_kernel(bool separated_kernels_only=false) override
  {
    double ssq = sigma*sigma;
    double sq2pisig = vnl_math::sqrt2pi*sigma;

    //1-d kernels
    for (int x = -khs; x <= khs; x++)
      K_x[x+khs] = -(x-dx)*std::exp(-(x-dx)*(x-dx)/(2*ssq))/(sq2pisig*ssq);
    for (int y = -khs; y <= khs; y++)
      K_y[y+khs] = std::exp(-(y-dy)*(y-dy)/(2*ssq))/sq2pisig;

    if (!separated_kernels_only){
      for (unsigned i=0; i<ni_; i++){
        for (unsigned j=0; j<nj_; j++){
          top_left_[i*istep_+j*jstep_] = K_x[i]*K_y[j];
        }
      }
    }
  }
};

//: Gy kernel
//  K_x = G_x, K_y = dG_y
class brip_Gy_kernel : public brip_gaussian_kernel
{
 public:
  brip_Gy_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_Gy_kernel() override= default;

  //: compute the kernel
  void compute_kernel(bool separated_kernels_only=false) override
  {
    double ssq = sigma*sigma;
    double sq2pisig = vnl_math::sqrt2pi*sigma;

    //1-d kernels
    for (int x = -khs; x <= khs; x++)
      K_x[x+khs] = std::exp(-(x-dx)*(x-dx)/(2*ssq))/sq2pisig;
    for (int y = -khs; y <= khs; y++)
      K_y[y+khs] = -(y-dy)*std::exp(-(y-dy)*(y-dy)/(2*ssq))/(sq2pisig*ssq);

    if (!separated_kernels_only){
      for (unsigned i=0; i<ni_; i++){
        for (unsigned j=0; j<nj_; j++){
          top_left_[i*istep_+j*jstep_] = K_x[i]*K_y[j];
        }
      }
    }
  }
};

//: Gxx kernel
//  K_x = d2G_x, K_y = G_y
class brip_Gxx_kernel : public brip_gaussian_kernel
{
 public:
  brip_Gxx_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_Gxx_kernel() override= default;

  //: compute the kernel
  void compute_kernel(bool separated_kernels_only=false) override
  {
    double ssq = sigma*sigma;
    double sq2pisig = vnl_math::sqrt2pi*sigma;

    //1-d kernels
    for (int x = -khs; x <= khs; x++)
      K_x[x+khs] = ((x-dx)*(x-dx)-ssq)*std::exp(-(x-dx)*(x-dx)/(2*ssq))/(sq2pisig*ssq*ssq);
    for (int y = -khs; y <= khs; y++)
      K_y[y+khs] = std::exp(-(y-dy)*(y-dy)/(2*ssq))/sq2pisig;

    if (!separated_kernels_only){
      for (unsigned i=0; i<ni_; i++){
        for (unsigned j=0; j<nj_; j++){
          top_left_[i*istep_+j*jstep_] = K_x[i]*K_y[j];
        }
      }
    }
  }
};

//: Gxy kernel
//  K_x = dG_x, K_y = dG_y
class brip_Gxy_kernel : public brip_gaussian_kernel
{
 public:
  brip_Gxy_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_Gxy_kernel() override= default;

  //: compute the kernel
  void compute_kernel(bool separated_kernels_only=false) override
  {
    double ssq = sigma*sigma;
    double sq2pisig = vnl_math::sqrt2pi*sigma;

    //1-d kernels
    for (int x = -khs; x <= khs; x++)
      K_x[x+khs] = -(x-dx)*std::exp(-(x-dx)*(x-dx)/(2*ssq))/(sq2pisig*ssq);
    for (int y = -khs; y <= khs; y++)
      K_y[y+khs] = -(y-dy)*std::exp(-(y-dy)*(y-dy)/(2*ssq))/(sq2pisig*ssq);

    if (!separated_kernels_only){
      for (unsigned i=0; i<ni_; i++){
        for (unsigned j=0; j<nj_; j++){
          top_left_[i*istep_+j*jstep_] = K_x[i]*K_y[j];
        }
      }
    }
  }
};

//: Gyy kernel
//  K_x = G_x, K_y = d2G_y
class brip_Gyy_kernel : public brip_gaussian_kernel
{
 public:
  brip_Gyy_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_Gyy_kernel() override= default;

  //: compute the kernel
  void compute_kernel(bool separated_kernels_only=false) override
  {
    double ssq = sigma*sigma;
    double sq2pisig = vnl_math::sqrt2pi*sigma;

    //1-d kernels
    for (int x = -khs; x <= khs; x++)
      K_x[x+khs] = std::exp(-(x-dx)*(x-dx)/(2*ssq))/sq2pisig;
    for (int y = -khs; y <= khs; y++)
      K_y[y+khs] = ((y-dy)*(y-dy)-ssq)*std::exp(-(y-dy)*(y-dy)/(2*ssq))/(sq2pisig*ssq*ssq);

    if (!separated_kernels_only){
      for (unsigned i=0; i<ni_; i++){
        for (unsigned j=0; j<nj_; j++){
          top_left_[i*istep_+j*jstep_] = K_x[i]*K_y[j];
        }
      }
    }
  }
};

//: Gxxx kernel
//  K_x = d3G_x, K_y = G_y
class brip_Gxxx_kernel : public brip_gaussian_kernel
{
 public:
  brip_Gxxx_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_Gxxx_kernel() override= default;

  //: compute the kernel
  void compute_kernel(bool separated_kernels_only=false) override
  {
    double ssq = sigma*sigma;
    double sq2pisig = vnl_math::sqrt2pi*sigma;

    //1-d kernels
    for (int x = -khs; x <= khs; x++)
      K_x[x+khs] = (x-dx)*(3*ssq -(x-dx)*(x-dx))*std::exp(-(x-dx)*(x-dx)/(2*ssq))/(sq2pisig*ssq*ssq*ssq);
    for (int y = -khs; y <= khs; y++)
      K_y[y+khs] = std::exp(-(y-dy)*(y-dy)/(2*ssq))/sq2pisig;

    if (!separated_kernels_only){
      for (unsigned i=0; i<ni_; i++){
        for (unsigned j=0; j<nj_; j++){
          top_left_[i*istep_+j*jstep_] = K_x[i]*K_y[j];
        }
      }
    }
  }
};

//: Gxxy kernel
//  K_x = d2G_x, K_y = dG_y
class brip_Gxxy_kernel : public brip_gaussian_kernel
{
 public:
  brip_Gxxy_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_Gxxy_kernel() override= default;

  //: compute the kernel
  void compute_kernel(bool separated_kernels_only=false) override
  {
    double ssq = sigma*sigma;
    double sq2pisig = vnl_math::sqrt2pi*sigma;

    //1-d kernels
    for (int x = -khs; x <= khs; x++)
      K_x[x+khs] = ((x-dx)*(x-dx)-ssq)*std::exp(-(x-dx)*(x-dx)/(2*ssq))/(sq2pisig*ssq*ssq);
    for (int y = -khs; y <= khs; y++)
      K_y[y+khs] = -(y-dy)*std::exp(-(y-dy)*(y-dy)/(2*ssq))/(sq2pisig*sigma*ssq);

    if (!separated_kernels_only){
      for (unsigned i=0; i<ni_; i++){
        for (unsigned j=0; j<nj_; j++){
          top_left_[i*istep_+j*jstep_] = K_x[i]*K_y[j];
        }
      }
    }
  }
};

//: Gxyy kernel
//  K_x = dG_x, K_y = d2G_y
class brip_Gxyy_kernel : public brip_gaussian_kernel
{
 public:
  brip_Gxyy_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_Gxyy_kernel() override= default;

  //: compute the kernel
  void compute_kernel(bool separated_kernels_only=false) override
  {
    double ssq = sigma*sigma;
    double sq2pisig = vnl_math::sqrt2pi*sigma;

    //1-d kernels
    for (int x = -khs; x <= khs; x++)
      K_x[x+khs] = -(x-dx)*std::exp(-(x-dx)*(x-dx)/(2*ssq))/(sq2pisig*ssq);
    for (int y = -khs; y <= khs; y++)
      K_y[y+khs] = ((y-dy)*(y-dy)-ssq)*std::exp(-(y-dy)*(y-dy)/(2*ssq))/(sq2pisig*ssq*ssq);

    if (!separated_kernels_only){
      for (unsigned i=0; i<ni_; i++){
        for (unsigned j=0; j<nj_; j++){
          top_left_[i*istep_+j*jstep_] = K_x[i]*K_y[j];
        }
      }
    }
  }
};

//: Gyyy kernel
//  K_x = G_x, K_y = d3G_y
class brip_Gyyy_kernel : public brip_gaussian_kernel
{
 public:
  brip_Gyyy_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_Gyyy_kernel() override= default;

  //: compute the kernel
  void compute_kernel(bool separated_kernels_only=false) override
  {
    double ssq = sigma*sigma;
    double sq2pisig = vnl_math::sqrt2pi*sigma;

    //1-d kernels
    for (int x = -khs; x <= khs; x++)
      K_x[x+khs] = std::exp(-(x-dx)*(x-dx)/(2*ssq))/sq2pisig;
    for (int y = -khs; y <= khs; y++)
      K_y[y+khs] = (y-dy)*(3*ssq -(y-dy)*(y-dy))*std::exp(-(y-dy)*(y-dy)/(2*ssq))/(sq2pisig*ssq*ssq*ssq);

    if (!separated_kernels_only){
      for (unsigned i=0; i<ni_; i++){
        for (unsigned j=0; j<nj_; j++){
          top_left_[i*istep_+j*jstep_] = K_x[i]*K_y[j];
        }
      }
    }
  }
};

#endif // brip_gaussian_kernel_h
