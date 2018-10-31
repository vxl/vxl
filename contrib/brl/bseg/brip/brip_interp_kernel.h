// This is brl/bseg/brip/brip_interp_kernel.h
#ifndef brip_interp_kernel_h
#define brip_interp_kernel_h
//:
// \file
// \brief Interpolated gaussian derivative kernels (h_0, h_1 and h_G)
// \author Amir Tamrakar
// \date 9 Sept 2006
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include<brip/brip_gaussian_kernel.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_erf.h>

//****************************************************************************************//
// CONSTANT INTERPOLATION KERNELS FOR GAUSSIAN DERIVATIVES
//****************************************************************************************//

//: h0_G kernel
class brip_h0_G_kernel : public brip_gaussian_kernel
{
 public:
  std::vector<double> G_x, G_y; //to minimize computation

  brip_h0_G_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_h0_G_kernel() override= default;

  //: compute the kernel
  virtual void compute_kernel()
  {
    //kernel half size
    int khs = (int) std::ceil(4*sigma);
    G_x.resize(2*khs+1);
    G_y.resize(2*khs+1);

    double c = std::sqrt(2.0)*sigma;

    for (int x = -khs; x <= khs; x++)
      G_x[x+khs] = (vnl_erf((x+0.5-dx)/c) - vnl_erf((x-0.5-dx)/c))/2.0;
    for (int y = -khs; y <= khs; y++)
      G_y[y+khs] = (vnl_erf((y+0.5-dy)/c) - vnl_erf((y-0.5-dy)/c))/2.0;

    for (unsigned i=0; i<ni_; i++){
      for (unsigned j=0; j<nj_; j++){
        top_left_[i*istep_+j*jstep_] = G_x[i]*G_y[j];
      }
    }
  }
};

//: h0_Gx kernel
class brip_h0_Gx_kernel : public brip_gaussian_kernel
{
 public:
  std::vector<double> dG_x, G_y; //to minimize computation

  brip_h0_Gx_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_h0_Gx_kernel() override= default;

  //: compute the kernel
  virtual void compute_kernel()
  {
    //kernel half size
    int khs = (int) std::ceil(4*sigma);
    dG_x.resize(2*khs+1);
    G_y.resize(2*khs+1);

    double ssq = sigma*sigma;
    double c = std::sqrt(2.0)*sigma;
    double sq2pisig = vnl_math::sqrt2pi*sigma;

    for (int x = -khs; x <= khs; x++)
      dG_x[x+khs] = (std::exp(-(x+0.5-dx)*(x+0.5-dx)/(2*ssq)) - std::exp(-(x-0.5-dx)*(x-0.5-dx)/(2*ssq)))/sq2pisig;
    for (int y = -khs; y <= khs; y++)
      G_y[y+khs] = (vnl_erf((y+0.5-dy)/c) - vnl_erf((y-0.5-dy)/c))/2.0;

    for (unsigned i=0; i<ni_; i++){
      for (unsigned j=0; j<nj_; j++){
        top_left_[i*istep_+j*jstep_] = dG_x[i]*G_y[j];
      }
    }
  }
};

//: h0_Gy kernel
class brip_h0_Gy_kernel : public brip_gaussian_kernel
{
 public:
  std::vector<double> G_x, dG_y; //to minimize computation

  brip_h0_Gy_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_h0_Gy_kernel() override= default;

  //: compute the kernel
  virtual void compute_kernel()
  {
    //kernel half size
    int khs = (int) std::ceil(4*sigma);
    G_x.resize(2*khs+1);
    dG_y.resize(2*khs+1);

    double ssq = sigma*sigma;
    double c = std::sqrt(2.0)*sigma;
    double sq2pisig = vnl_math::sqrt2pi*sigma;

    for (int x = -khs; x <= khs; x++)
      G_x[x+khs] = (vnl_erf((x+0.5-dx)/c) - vnl_erf((x-0.5-dx)/c))/2.0;
    for (int y = -khs; y <= khs; y++)
      dG_y[y+khs] = (std::exp(-(y+0.5-dy)*(y+0.5-dy)/(2*ssq)) - std::exp(-(y-0.5-dy)*(y-0.5-dy)/(2*ssq)))/sq2pisig;

    for (unsigned i=0; i<ni_; i++){
      for (unsigned j=0; j<nj_; j++){
        top_left_[i*istep_+j*jstep_] = G_x[i]*dG_y[j];
      }
    }
  }
};

//: h0_Gxx kernel
class brip_h0_Gxx_kernel : public brip_gaussian_kernel
{
 public:
  std::vector<double> d2G_x, G_y; //to minimize computation

  brip_h0_Gxx_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_h0_Gxx_kernel() override= default;

  //: compute the kernel
  virtual void compute_kernel()
  {
    //kernel half size
    int khs = (int) std::ceil(4*sigma);
    d2G_x.resize(2*khs+1);
    G_y.resize(2*khs+1);

    double ssq = sigma*sigma;
    double c = std::sqrt(2.0)*sigma;
    double sq2pisig = vnl_math::sqrt2pi*sigma;

    for (int x = -khs; x <= khs; x++)
      d2G_x[x+khs] = (-(x+0.5-dx)*std::exp(-(x+0.5-dx)*(x+0.5-dx)/(2*ssq)) + (x-0.5-dx)*std::exp(-(x-0.5-dx)*(x-0.5-dx)/(2*ssq)))/(sq2pisig*ssq);
    for (int y = -khs; y <= khs; y++)
      G_y[y+khs] = (vnl_erf((y+0.5-dy)/c) - vnl_erf((y-0.5-dy)/c))/2.0;

    for (unsigned i=0; i<ni_; i++){
      for (unsigned j=0; j<nj_; j++){
        top_left_[i*istep_+j*jstep_] = d2G_x[i]*G_y[j];
      }
    }
  }
};

//: h0_Gxy kernel
class brip_h0_Gxy_kernel : public brip_gaussian_kernel
{
 public:
  std::vector<double> dG_x, dG_y; //to minimize computation

  brip_h0_Gxy_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_h0_Gxy_kernel() override= default;

  //: compute the kernel
  virtual void compute_kernel()
  {
    //kernel half size
    int khs = (int) std::ceil(4*sigma);
    dG_x.resize(2*khs+1);
    dG_y.resize(2*khs+1);

    double ssq = sigma*sigma;
    double sq2pisig = vnl_math::sqrt2pi*sigma;

    for (int x = -khs; x <= khs; x++)
      dG_x[x+khs] = (std::exp(-(x+0.5-dx)*(x+0.5-dx)/(2*ssq)) - std::exp(-(x-0.5-dx)*(x-0.5-dx)/(2*ssq)))/sq2pisig;
    for (int y = -khs; y <= khs; y++)
      dG_y[y+khs] = (std::exp(-(y+0.5-dy)*(y+0.5-dy)/(2*ssq)) - std::exp(-(y-0.5-dy)*(y-0.5-dy)/(2*ssq)))/sq2pisig;

    for (unsigned i=0; i<ni_; i++){
      for (unsigned j=0; j<nj_; j++){
        top_left_[i*istep_+j*jstep_] = dG_x[i]*dG_y[j];
      }
    }
  }
};

//: h0_Gyy kernel
class brip_h0_Gyy_kernel : public brip_gaussian_kernel
{
 public:
  std::vector<double> G_x, d2G_y; //to minimize computation

  brip_h0_Gyy_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_h0_Gyy_kernel() override= default;

  //: compute the kernel
  virtual void compute_kernel()
  {
    //kernel half size
    int khs = (int) std::ceil(4*sigma);
    G_x.resize(2*khs+1);
    d2G_y.resize(2*khs+1);

    double ssq = sigma*sigma;
    double c = std::sqrt(2.0)*sigma;
    double sq2pisig = vnl_math::sqrt2pi*sigma;

    for (int x = -khs; x <= khs; x++)
      G_x[x+khs] = (vnl_erf((x+0.5-dx)/c) - vnl_erf((x-0.5-dx)/c))/2.0;
    for (int y = -khs; y <= khs; y++)
      d2G_y[y+khs] = (-(y+0.5-dy)*std::exp(-(y+0.5-dy)*(y+0.5-dy)/(2*ssq)) + (y-0.5-dy)*std::exp(-(y-0.5-dy)*(y-0.5-dy)/(2*ssq)))/(sq2pisig*ssq);

    for (unsigned i=0; i<ni_; i++){
      for (unsigned j=0; j<nj_; j++){
        top_left_[i*istep_+j*jstep_] = G_x[i]*d2G_y[j];
      }
    }
  }
};

//: h0_Gxxx kernel
class brip_h0_Gxxx_kernel : public brip_gaussian_kernel
{
 public:
  std::vector<double> d3G_x, G_y; //to minimize computation

  brip_h0_Gxxx_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_h0_Gxxx_kernel() override= default;

  //: compute the kernel
  virtual void compute_kernel()
  {
    //kernel half size
    int khs = (int) std::ceil(4*sigma);
    d3G_x.resize(2*khs+1);
    G_y.resize(2*khs+1);

    double ssq = sigma*sigma;
    double c = std::sqrt(2.0)*sigma;
    double sq2pisig = vnl_math::sqrt2pi*sigma;

    for (int x = -khs; x <= khs; x++)
      d3G_x[x+khs] = (((x+0.5-dx)*(x+0.5-dx)-ssq)*std::exp(-(x+0.5-dx)*(x+0.5-dx)/(2*ssq)) - ((x-0.5-dx)*(x-0.5-dx)-ssq)*std::exp(-(x-0.5-dx)*(x-0.5-dx)/(2*ssq)))/(sq2pisig*ssq*ssq);
    for (int y = -khs; y <= khs; y++)
      G_y[y+khs] = (vnl_erf((y+0.5-dy)/c) - vnl_erf((y-0.5-dy)/c))/2.0;

    for (unsigned i=0; i<ni_; i++){
      for (unsigned j=0; j<nj_; j++){
        top_left_[i*istep_+j*jstep_] = d3G_x[i]*G_y[j];
      }
    }
  }
};

//: h0_Gxxy kernel
class brip_h0_Gxxy_kernel : public brip_gaussian_kernel
{
 public:
  std::vector<double> d2G_x, dG_y; //to minimize computation

  brip_h0_Gxxy_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_h0_Gxxy_kernel() override= default;

  //: compute the kernel
  void compute_kernel(bool ) override
  {
    //kernel half size
    int khs = (int) std::ceil(4*sigma);
    d2G_x.resize(2*khs+1);
    dG_y.resize(2*khs+1);

    double ssq = sigma*sigma;
    double sq2pisig = vnl_math::sqrt2pi*sigma;

    for (int x = -khs; x <= khs; x++)
      d2G_x[x+khs] = (-(x+0.5-dx)*std::exp(-(x+0.5-dx)*(x+0.5-dx)/(2*ssq)) + (x-0.5-dx)*std::exp(-(x-0.5-dx)*(x-0.5-dx)/(2*ssq)))/(sq2pisig*ssq);
    for (int y = -khs; y <= khs; y++)
      dG_y[y+khs] = (std::exp(-(y+0.5-dy)*(y+0.5-dy)/(2*ssq)) - std::exp(-(y-0.5-dy)*(y-0.5-dy)/(2*ssq)))/sq2pisig;

    for (unsigned i=0; i<ni_; i++){
      for (unsigned j=0; j<nj_; j++){
        top_left_[i*istep_+j*jstep_] = d2G_x[i]*dG_y[j];
      }
    }
  }
};

//: h0_Gxyy kernel
class brip_h0_Gxyy_kernel : public brip_gaussian_kernel
{
 public:
  std::vector<double> dG_x, d2G_y; //to minimize computation

  brip_h0_Gxyy_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_h0_Gxyy_kernel() override= default;

  //: compute the kernel
  virtual void compute_kernel()
  {
    //kernel half size
    int khs = (int) std::ceil(4*sigma);
    dG_x.resize(2*khs+1);
    d2G_y.resize(2*khs+1);

    double ssq = sigma*sigma;
    double sq2pisig = vnl_math::sqrt2pi*sigma;

    for (int x = -khs; x <= khs; x++)
      dG_x[x+khs] = (std::exp(-(x+0.5-dx)*(x+0.5-dx)/(2*ssq)) - std::exp(-(x-0.5-dx)*(x-0.5-dx)/(2*ssq)))/sq2pisig;
    for (int y = -khs; y <= khs; y++)
      d2G_y[y+khs] = (-(y+0.5-dy)*std::exp(-(y+0.5-dy)*(y+0.5-dy)/(2*ssq)) + (y-0.5-dy)*std::exp(-(y-0.5-dy)*(y-0.5-dy)/(2*ssq)))/(sq2pisig*ssq);

    for (unsigned i=0; i<ni_; i++){
      for (unsigned j=0; j<nj_; j++){
        top_left_[i*istep_+j*jstep_] = dG_x[i]*d2G_y[j];
      }
    }
  }
};

//: h0_Gyyy kernel
class brip_h0_Gyyy_kernel : public brip_gaussian_kernel
{
 public:
  std::vector<double> G_x, d3G_y; //to minimize computation

  brip_h0_Gyyy_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_h0_Gyyy_kernel() override= default;

  //: compute the kernel
  virtual void compute_kernel()
  {
    //kernel half size
    int khs = (int) std::ceil(4*sigma);
    G_x.resize(2*khs+1);
    d3G_y.resize(2*khs+1);

    double ssq = sigma*sigma;
    double c = std::sqrt(2.0)*sigma;
    double sq2pisig = vnl_math::sqrt2pi*sigma;

    for (int x = -khs; x <= khs; x++)
      G_x[x+khs] = (vnl_erf((x+0.5-dx)/c) - vnl_erf((x-0.5-dx)/c))/2.0;
    for (int y = -khs; y <= khs; y++)
      d3G_y[y+khs] = (((y+0.5-dy)*(y+0.5-dy)-ssq)*std::exp(-(y+0.5-dy)*(y+0.5-dy)/(2*ssq)) - ((y-0.5-dy)*(y-0.5-dy)-ssq)*std::exp(-(y-0.5-dy)*(y-0.5-dy)/(2*ssq)))/(sq2pisig*ssq*ssq);

    for (unsigned i=0; i<ni_; i++){
      for (unsigned j=0; j<nj_; j++){
        top_left_[i*istep_+j*jstep_] = G_x[i]*d3G_y[j];
      }
    }
  }
};

//****************************************************************************************//
// LINEAR INTERPOLATION KERNELS FOR GAUSSIAN DERIVATIVES
//****************************************************************************************//

//: h1_G kernel
class brip_h1_G_kernel : public brip_gaussian_kernel
{
 public:
  std::vector<double> G_x, G_y; //to minimize computation

  brip_h1_G_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_h1_G_kernel() override= default;

  //: compute the kernel
  virtual void compute_kernel()
  {
    //kernel half size
    int khs = (int) std::ceil(4*sigma);
    G_x.resize(2*khs+1);
    G_y.resize(2*khs+1);

    double ssq = sigma*sigma;
    double c = std::sqrt(2.0)*sigma;
    double sq2pi = vnl_math::sqrt2pi;

    for (int x = -khs; x <= khs; x++)
      G_x[x+khs] = ((x-1-dx)*vnl_erf((x-1-dx)/c) - 2*(x-dx)*vnl_erf((x-dx)/c) +(x+1-dx)*vnl_erf((x+1-dx)/c))/2.0 +
                      sigma*(std::exp(-(x-1-dx)*(x-1-dx)/(2*ssq)) - 2*std::exp(-(x-dx)*(x-dx)/(2*ssq)) + std::exp(-(x+1-dx)*(x+1-dx)/(2*ssq)))/sq2pi;
    for (int y = -khs; y <= khs; y++)
      G_y[y+khs] = ((y-1-dy)*vnl_erf((y-1-dy)/c) - 2*(y-dy)*vnl_erf((y-dy)/c) +(y+1-dy)*vnl_erf((y+1-dy)/c))/2.0 +
                      sigma*(std::exp(-(y-1-dy)*(y-1-dy)/(2*ssq)) - 2*std::exp(-(y-dy)*(y-dy)/(2*ssq)) + std::exp(-(y+1-dy)*(y+1-dy)/(2*ssq)))/sq2pi;

    for (unsigned i=0; i<ni_; i++){
      for (unsigned j=0; j<nj_; j++){
        top_left_[i*istep_+j*jstep_] = G_x[i]*G_y[j];
      }
    }
  }
};

//: h1_Gx kernel
class brip_h1_Gx_kernel : public brip_gaussian_kernel
{
 public:
  std::vector<double> dG_x, G_y; //to minimize computation

  brip_h1_Gx_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_h1_Gx_kernel() override= default;

  //: compute the kernel
  virtual void compute_kernel()
  {
    //kernel half size
    int khs = (int) std::ceil(4*sigma);
    dG_x.resize(2*khs+1);
    G_y.resize(2*khs+1);

    double ssq = sigma*sigma;
    double c = std::sqrt(2.0)*sigma;
    double sq2pi = vnl_math::sqrt2pi;

    for (int x = -khs; x <= khs; x++)
      dG_x[x+khs] = (vnl_erf((x-1-dx)/c) - 2*vnl_erf((x-dx)/c) + vnl_erf((x+1-dx)/c))/2.0;
    for (int y = -khs; y <= khs; y++)
      G_y[y+khs] = ((y-1-dy)*vnl_erf((y-1-dy)/c) - 2*(y-dy)*vnl_erf((y-dy)/c) +(y+1-dy)*vnl_erf((y+1-dy)/c))/2.0 +
                      sigma*(std::exp(-(y-1-dy)*(y-1-dy)/(2*ssq)) - 2*std::exp(-(y-dy)*(y-dy)/(2*ssq)) + std::exp(-(y+1-dy)*(y+1-dy)/(2*ssq)))/sq2pi;

    for (unsigned i=0; i<ni_; i++){
      for (unsigned j=0; j<nj_; j++){
        top_left_[i*istep_+j*jstep_] = dG_x[i]*G_y[j];
      }
    }
  }
};

//: h1_Gy kernel
class brip_h1_Gy_kernel : public brip_gaussian_kernel
{
 public:
  std::vector<double> G_x, dG_y; //to minimize computation

  brip_h1_Gy_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_h1_Gy_kernel() override= default;

  //: compute the kernel
  virtual void compute_kernel()
  {
    //kernel half size
    int khs = (int) std::ceil(4*sigma);
    G_x.resize(2*khs+1);
    dG_y.resize(2*khs+1);

    double ssq = sigma*sigma;
    double c = std::sqrt(2.0)*sigma;
    double sq2pi = vnl_math::sqrt2pi;

    for (int x = -khs; x <= khs; x++)
      G_x[x+khs] = ((x-1-dx)*vnl_erf((x-1-dx)/c) - 2*(x-dx)*vnl_erf((x-dx)/c) +(x+1-dx)*vnl_erf((x+1-dx)/c))/2.0 +
                      sigma*(std::exp(-(x-1-dx)*(x-1-dx)/(2*ssq)) - 2*std::exp(-(x-dx)*(x-dx)/(2*ssq)) + std::exp(-(x+1-dx)*(x+1-dx)/(2*ssq)))/sq2pi;
    for (int y = -khs; y <= khs; y++)
      dG_y[y+khs] = (vnl_erf((y-1-dy)/c) - 2*vnl_erf((y-dy)/c) + vnl_erf((y+1-dy)/c))/2.0;

    for (unsigned i=0; i<ni_; i++){
      for (unsigned j=0; j<nj_; j++){
        top_left_[i*istep_+j*jstep_] = G_x[i]*dG_y[j];
      }
    }
  }
};

//: h1_Gxx kernel
class brip_h1_Gxx_kernel : public brip_gaussian_kernel
{
 public:
  std::vector<double> d2G_x, G_y; //to minimize computation

  brip_h1_Gxx_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_h1_Gxx_kernel() override= default;

  //: compute the kernel
  virtual void compute_kernel()
  {
    //kernel half size
    int khs = (int) std::ceil(4*sigma);
    d2G_x.resize(2*khs+1);
    G_y.resize(2*khs+1);

    double ssq = sigma*sigma;
    double c = std::sqrt(2.0)*sigma;
    double sq2pi = vnl_math::sqrt2pi;

    for (int x = -khs; x <= khs; x++)
      d2G_x[x+khs] = (std::exp(-(x-1-dx)*(x-1-dx)/(2*ssq)) - 2*std::exp(-(x-dx)*(x-dx)/(2*ssq)) + std::exp(-(x+1-dx)*(x+1-dx)/(2*ssq)))/(sq2pi*sigma);
    for (int y = -khs; y <= khs; y++)
      G_y[y+khs] = ((y-1-dy)*vnl_erf((y-1-dy)/c) - 2*(y-dy)*vnl_erf((y-dy)/c) +(y+1-dy)*vnl_erf((y+1-dy)/c))/2.0 +
                      sigma*(std::exp(-(y-1-dy)*(y-1-dy)/(2*ssq)) - 2*std::exp(-(y-dy)*(y-dy)/(2*ssq)) + std::exp(-(y+1-dy)*(y+1-dy)/(2*ssq)))/sq2pi;

    for (unsigned i=0; i<ni_; i++){
      for (unsigned j=0; j<nj_; j++){
        top_left_[i*istep_+j*jstep_] = d2G_x[i]*G_y[j];
      }
    }
  }
};

//: h1_Gxy kernel
class brip_h1_Gxy_kernel : public brip_gaussian_kernel
{
 public:
  std::vector<double> dG_x, dG_y; //to minimize computation

  brip_h1_Gxy_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_h1_Gxy_kernel() override= default;

  //: compute the kernel
  virtual void compute_kernel()
  {
    //kernel half size
    int khs = (int) std::ceil(4*sigma);
    dG_x.resize(2*khs+1);
    dG_y.resize(2*khs+1);

    double c = std::sqrt(2.0)*sigma;

    for (int x = -khs; x <= khs; x++)
      dG_x[x+khs] = (vnl_erf((x-1-dx)/c) - 2*vnl_erf((x-dx)/c) + vnl_erf((x+1-dx)/c))/2.0;
    for (int y = -khs; y <= khs; y++)
      dG_y[y+khs] = (vnl_erf((y-1-dy)/c) - 2*vnl_erf((y-dy)/c) + vnl_erf((y+1-dy)/c))/2.0;

    for (unsigned i=0; i<ni_; i++){
      for (unsigned j=0; j<nj_; j++){
        top_left_[i*istep_+j*jstep_] = dG_x[i]*dG_y[j];
      }
    }
  }
};

//: h1_Gyy kernel
class brip_h1_Gyy_kernel : public brip_gaussian_kernel
{
 public:
  std::vector<double> G_x, d2G_y; //to minimize computation

  brip_h1_Gyy_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_h1_Gyy_kernel() override= default;

  //: compute the kernel
  virtual void compute_kernel()
  {
    //kernel half size
    int khs = (int) std::ceil(4*sigma);
    G_x.resize(2*khs+1);
    d2G_y.resize(2*khs+1);

    double ssq = sigma*sigma;
    double c = std::sqrt(2.0)*sigma;
    double sq2pi = vnl_math::sqrt2pi;

    for (int x = -khs; x <= khs; x++)
      G_x[x+khs] = ((x-1-dx)*vnl_erf((x-1-dx)/c) - 2*(x-dx)*vnl_erf((x-dx)/c) +(x+1-dx)*vnl_erf((x+1-dx)/c))/2.0 +
                      sigma*(std::exp(-(x-1-dx)*(x-1-dx)/(2*ssq)) - 2*std::exp(-(x-dx)*(x-dx)/(2*ssq)) + std::exp(-(x+1-dx)*(x+1-dx)/(2*ssq)))/sq2pi;
    for (int y = -khs; y <= khs; y++)
      d2G_y[y+khs] = (std::exp(-(y-1-dy)*(y-1-dy)/(2*ssq)) - 2*std::exp(-(y-dy)*(y-dy)/(2*ssq)) + std::exp(-(y+1-dy)*(y+1-dy)/(2*ssq)))/(sq2pi*sigma);

    for (unsigned i=0; i<ni_; i++){
      for (unsigned j=0; j<nj_; j++){
        top_left_[i*istep_+j*jstep_] = G_x[i]*d2G_y[j];
      }
    }
  }
};

//: h1_Gxxx kernel
class brip_h1_Gxxx_kernel : public brip_gaussian_kernel
{
 public:
  std::vector<double> d3G_x, G_y; //to minimize computation

  brip_h1_Gxxx_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_h1_Gxxx_kernel() override= default;

  //: compute the kernel
  virtual void compute_kernel()
  {
    //kernel half size
    int khs = (int) std::ceil(4*sigma);
    d3G_x.resize(2*khs+1);
    G_y.resize(2*khs+1);

    double ssq = sigma*sigma;
    double c = std::sqrt(2.0)*sigma;
    double sq2pi = vnl_math::sqrt2pi;

    for (int x = -khs; x <= khs; x++)
      d3G_x[x+khs] = (-(x-1-dx)*std::exp(-(x-1-dx)*(x-1-dx)/(2*ssq)) + 2*(x-dx)*std::exp(-(x-dx)*(x-dx)/(2*ssq)) - (x+1-dx)*std::exp(-(x+1-dx)*(x+1-dx)/(2*ssq)))/(sq2pi*sigma*ssq);
    for (int y = -khs; y <= khs; y++)
      G_y[y+khs] = ((y-1-dy)*vnl_erf((y-1-dy)/c) - 2*(y-dy)*vnl_erf((y-dy)/c) +(y+1-dy)*vnl_erf((y+1-dy)/c))/2.0 +
                      sigma*(std::exp(-(y-1-dy)*(y-1-dy)/(2*ssq)) - 2*std::exp(-(y-dy)*(y-dy)/(2*ssq)) + std::exp(-(y+1-dy)*(y+1-dy)/(2*ssq)))/sq2pi;

    for (unsigned i=0; i<ni_; i++){
      for (unsigned j=0; j<nj_; j++){
        top_left_[i*istep_+j*jstep_] = d3G_x[i]*G_y[j];
      }
    }
  }
};

//: h1_Gxxy kernel
class brip_h1_Gxxy_kernel : public brip_gaussian_kernel
{
 public:
  std::vector<double> d2G_x, dG_y; //to minimize computation

  brip_h1_Gxxy_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_h1_Gxxy_kernel() override= default;

  //: compute the kernel
  virtual void compute_kernel()
  {
    //kernel half size
    int khs = (int) std::ceil(4*sigma);
    d2G_x.resize(2*khs+1);
    dG_y.resize(2*khs+1);

    double ssq = sigma*sigma;
    double c = std::sqrt(2.0)*sigma;
    double sq2pi = vnl_math::sqrt2pi;

    for (int x = -khs; x <= khs; x++)
      d2G_x[x+khs] = (std::exp(-(x-1-dx)*(x-1-dx)/(2*ssq)) - 2*std::exp(-(x-dx)*(x-dx)/(2*ssq)) + std::exp(-(x+1-dx)*(x+1-dx)/(2*ssq)))/(sq2pi*sigma);
    for (int y = -khs; y <= khs; y++)
      dG_y[y+khs] = (vnl_erf((y-1-dy)/c) - 2*vnl_erf((y-dy)/c) + vnl_erf((y+1-dy)/c))/2.0;

    for (unsigned i=0; i<ni_; i++){
      for (unsigned j=0; j<nj_; j++){
        top_left_[i*istep_+j*jstep_] = d2G_x[i]*dG_y[j];
      }
    }
  }
};

//: h1_Gxyy kernel
class brip_h1_Gxyy_kernel : public brip_gaussian_kernel
{
 public:
  std::vector<double> dG_x, d2G_y; //to minimize computation

  brip_h1_Gxyy_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_h1_Gxyy_kernel() override= default;

  //: compute the kernel
  virtual void compute_kernel()
  {
    //kernel half size
    int khs = (int) std::ceil(4*sigma);
    dG_x.resize(2*khs+1);
    d2G_y.resize(2*khs+1);

    double ssq = sigma*sigma;
    double c = std::sqrt(2.0)*sigma;
    double sq2pi = vnl_math::sqrt2pi;

    for (int x = -khs; x <= khs; x++)
      dG_x[x+khs] = (vnl_erf((x-1-dx)/c) - 2*vnl_erf((x-dx)/c) + vnl_erf((x+1-dx)/c))/2.0;
    for (int y = -khs; y <= khs; y++)
      d2G_y[y+khs] = (std::exp(-(y-1-dy)*(y-1-dy)/(2*ssq)) - 2*std::exp(-(y-dy)*(y-dy)/(2*ssq)) + std::exp(-(y+1-dy)*(y+1-dy)/(2*ssq)))/(sq2pi*sigma);

    for (unsigned i=0; i<ni_; i++){
      for (unsigned j=0; j<nj_; j++){
        top_left_[i*istep_+j*jstep_] = dG_x[i]*d2G_y[j];
      }
    }
  }
};

//: h1_Gyyy kernel
class brip_h1_Gyyy_kernel : public brip_gaussian_kernel
{
 public:
  std::vector<double> G_x, d3G_y; //to minimize computation

  brip_h1_Gyyy_kernel(double sigma_, double dx_=0.0, double dy_=0.0): brip_gaussian_kernel(sigma_, dx_, dy_){}
  ~brip_h1_Gyyy_kernel() override= default;

  //: compute the kernel
  virtual void compute_kernel()
  {
    //kernel half size
    int khs = (int) std::ceil(4*sigma);
    G_x.resize(2*khs+1);
    d3G_y.resize(2*khs+1);

    double ssq = sigma*sigma;
    double c = std::sqrt(2.0)*sigma;
    double sq2pi = vnl_math::sqrt2pi;

    for (int x = -khs; x <= khs; x++)
      G_x[x+khs] = ((x-1-dx)*vnl_erf((x-1-dx)/c) - 2*(x-dx)*vnl_erf((x-dx)/c) +(x+1-dx)*vnl_erf((x+1-dx)/c))/2.0 +
                      sigma*(std::exp(-(x-1-dx)*(x-1-dx)/(2*ssq)) - 2*std::exp(-(x-dx)*(x-dx)/(2*ssq)) + std::exp(-(x+1-dx)*(x+1-dx)/(2*ssq)))/sq2pi;
    for (int y = -khs; y <= khs; y++)
      d3G_y[y+khs] = (-(y-1-dy)*std::exp(-(y-1-dy)*(y-1-dy)/(2*ssq)) + 2*(y-dy)*std::exp(-(y-dy)*(y-dy)/(2*ssq)) - (y+1-dy)*std::exp(-(y+1-dy)*(y+1-dy)/(2*ssq)))/(sq2pi*sigma*ssq);

    for (unsigned i=0; i<ni_; i++){
      for (unsigned j=0; j<nj_; j++){
        top_left_[i*istep_+j*jstep_] = G_x[i]*d3G_y[j];
      }
    }
  }
};

#endif // brip_interp_kernel_h
