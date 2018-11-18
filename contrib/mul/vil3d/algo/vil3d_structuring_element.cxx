// This is mul/vil3d/algo/vil3d_structuring_element.cxx
#include <iostream>
#include "vil3d_structuring_element.h"
//:
// \file
// \brief Structuring element for morphology represented as a list of non-zero pixels
// \author Tim Cootes

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

  //: Define elements { (p_i[a],p_j[a],p_k[a]) }
vil3d_structuring_element::vil3d_structuring_element(const std::vector<int>& p_i,
                                                     const std::vector<int>& p_j,
                                                     const std::vector<int>& p_k)
{
  set(p_i,p_j,p_k);
}

  //: Define elements { (p_i[a],p_j[a],p_k[a]) }
void vil3d_structuring_element::set(const std::vector<int>& p_i,
                                    const std::vector<int>& p_j,
                                    const std::vector<int>& p_k)
{
  assert(p_i.size()==p_j.size());
  assert(p_i.size()==p_k.size());
  assert(p_i.size()>0);
  p_i_ = p_i;
  p_j_ = p_j;
  p_k_ = p_k;

  max_i_=min_i_ = p_i[0];
  max_j_=min_j_ = p_j[0];
  max_k_=min_k_ = p_k[0];
  for (unsigned int a=1;a<p_i.size();++a)
  {
    if (p_i[a]<min_i_) min_i_=p_i[a];
    else if (p_i[a]>max_i_) max_i_=p_i[a];

    if (p_j[a]<min_j_) min_j_=p_j[a];
    else if (p_j[a]>max_j_) max_j_=p_j[a];

    if (p_k[a]<min_k_) min_k_=p_k[a];
    else if (p_k[a]>max_k_) max_k_=p_k[a];
  }
}




//: Set to 6 axis-aligned neighbours plus self
void vil3d_structuring_element::set_to_7()
{
  std::vector<int> px,py,pz;
  for (int i=-1;i<=1;i+=2)
  {
    px.push_back(i);  py.push_back(0);  pz.push_back(0);
    px.push_back(0);  py.push_back(i);  pz.push_back(0);
    px.push_back(0);  py.push_back(0);  pz.push_back(i);
  }
  px.push_back(0);  py.push_back(0);  pz.push_back(0);
  set(px,py,pz);
}

  //: Set to 26 touching neighbours plus self
void vil3d_structuring_element::set_to_27()
{
  std::vector<int> px,py,pz;
  for (int k=-1;k<=1;++k)
    for (int j=-1;j<=1;++j)
      for (int i=-1;i<=1;++i)
      {
        px.push_back(i);  py.push_back(j);  pz.push_back(k);
      }
  set(px,py,pz);
}


//: Set to sphere of radius r
//  Select pixels in disk s.t. x*x+y*y+z*z<=r^r
void vil3d_structuring_element::set_to_sphere(double r)
{
  std::vector<int> px,py,pz;
  double r2 = r*r;
  int r0 = int(r+1);
  for (int k=-r0;k<=r0;++k)
    for (int j=-r0;j<=r0;++j)
      for (int i=-r0;i<=r0;++i)
        if (i*i+j*j+k*k<r2)
        { px.push_back(i); py.push_back(j); pz.push_back(k); }
  set(px,py,pz);
}

//: Set to circle of radius r
//  Select pixels in circle s.t. y*y+z*z<=r^r
void vil3d_structuring_element::set_to_circle_i(double r)
{
  std::vector<int> px,py,pz;
  double r2 = r*r;
  int r0 = int(r+1);
  constexpr int i = 0;
  for (int k=-r0;k<=r0;++k)
    for (int j=-r0;j<=r0;++j)
      if (i*i+j*j+k*k<r2)
      { px.push_back(i); py.push_back(j); pz.push_back(k); }
  set(px,py,pz);
}

//: Set to circle of radius r
//  Select pixels in circle s.t. x*x+z*z<=r^r
void vil3d_structuring_element::set_to_circle_j(double r)
{
  std::vector<int> px,py,pz;
  double r2 = r*r;
  int r0 = int(r+1);
  constexpr int j = 0;
  for (int k=-r0;k<=r0;++k)
    for (int i=-r0;i<=r0;++i)
      if (i*i+j*j+k*k<r2)
        { px.push_back(i); py.push_back(j); pz.push_back(k); }
  set(px,py,pz);
}

//: Set to circle of radius r
//  Select pixels in circle s.t. y*y+z*z<=r^r
void vil3d_structuring_element::set_to_circle_k(double r)
{
  std::vector<int> px,py,pz;
  double r2 = r*r;
  int r0 = int(r+1);
  constexpr int k = 0;
  for (int j=-r0;j<=r0;++j)
    for (int i=-r0;i<=r0;++i)
      if (i*i+j*j+k*k<r2)
      { px.push_back(i); py.push_back(j); pz.push_back(k); }
  set(px,py,pz);
}

//: Set to sphere of radius r, but with non isotropic voxel sizes
//  Voxel size supplied in sx,sy and sz. r then becomes an absolute radius
//  Select pixels in disk s.t. x*x+y*y+z*z<=r^r
void vil3d_structuring_element::set_to_sphere_noniso(double r, double sx, double sy, double sz)
{
  std::vector<int> px,py,pz;
  double r2 = r*r;
  double sx2 = sx*sx;
  double sy2 = sy*sy;
  double sz2 = sz*sz;
  int r0 = int(r+1);
  for (int k=-r0;k<=r0;++k)
    for (int j=-r0;j<=r0;++j)
      for (int i=-r0;i<=r0;++i)
        if (i*i*sx2+j*j*sy2+k*k*sz2<r2)
        { px.push_back(i); py.push_back(j); pz.push_back(k); }
  set(px,py,pz);
}

//: Set to line along i (ilo,0)..(ihi,0)
void vil3d_structuring_element::set_to_line_i(int ilo, int ihi)
{
  p_i_.resize(1+ihi-ilo);
  p_j_.resize(1+ihi-ilo);
  p_k_.resize(1+ihi-ilo);
  for (int i = ilo;i<=ihi;++i)
  {
    p_i_[i-ilo]=i; p_j_[i-ilo]=0; p_k_[i-ilo]=0;
  }

  min_i_ = ilo; max_i_ = ihi;
  min_j_ = 0;   max_j_ = 0;
  min_k_ = 0;   max_k_ = 0;
}

//: Set to line along j (jlo,0)..(jhi,0)
void vil3d_structuring_element::set_to_line_j(int jlo, int jhi)
{
  p_i_.resize(1+jhi-jlo);
  p_j_.resize(1+jhi-jlo);
  p_k_.resize(1+jhi-jlo);
  for (int j = jlo;j<=jhi;++j)
  {
    p_i_[j-jlo]=0; p_j_[j-jlo]=j; p_k_[j-jlo]=0;
  }

  min_i_ = 0;   max_i_ = 0;
  min_j_ = jlo; max_j_ = jhi;
  min_k_ = 0;   max_k_ = 0;
}

//: Set to line along k (klo,0)..(khi,0)
void vil3d_structuring_element::set_to_line_k(int klo, int khi)
{
  p_i_.resize(1+khi-klo);
  p_j_.resize(1+khi-klo);
  p_k_.resize(1+khi-klo);
  for (int k = klo;k<=khi;++k)
  {
    p_i_[k-klo]=0; p_j_[k-klo]=0; p_k_[k-klo]=k;
  }

  min_i_ = 0;   max_i_ = 0;
  min_j_ = 0;   max_j_ = 0;
  min_k_ = klo; max_k_ = khi;
}

//: Write details to stream
std::ostream& operator<<(std::ostream& os, const vil3d_structuring_element& element)
{
  os<<"Bounds ["
    <<element.min_i()<<','<<element.max_i()<<"]["
    <<element.min_j()<<','<<element.max_j()<<"]["
    <<element.min_k()<<','<<element.max_k()<<"] Points: ";
  for (unsigned int a=0;a<element.p_i().size();++a)
  {
    os<<'('<<element.p_i()[a]<<','
      <<element.p_j()[a]<<','<<element.p_k()[a]<<") ";
  }
  return os;
}

//: Generate a list of offsets for use on image with istep,jstep,kstep
void vil3d_compute_offsets(std::vector<std::ptrdiff_t>& offset,
                          const vil3d_structuring_element& element,
                          std::ptrdiff_t istep,
                          std::ptrdiff_t jstep,
                          std::ptrdiff_t kstep)
{
  unsigned n = element.p_i().size();
  offset.resize(n);
  for (unsigned int a=0;a<n;++a)
    offset[a] = element.p_i()[a]*istep + element.p_j()[a]*jstep
              + element.p_k()[a]*kstep;
}
