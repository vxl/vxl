// This is core/vil/algo/vil_structuring_element.cxx
#include <iostream>
#include "vil_structuring_element.h"
//:
// \file
// \brief Structuring element for morphology represented as a list of non-zero pixels
// \author Tim Cootes

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Define elements { (p_i[k],p_j[k]) }
void vil_structuring_element::set(const std::vector<int>& v_p_i,const std::vector<int>& v_p_j)
{
  assert(v_p_i.size()==v_p_j.size());
  assert(v_p_i.size()>0);
  p_i_ = v_p_i;
  p_j_ = v_p_j;

  max_i_=min_i_ = v_p_i[0];
  max_j_=min_j_ = v_p_j[0];
  for (unsigned int k=1;k<v_p_i.size();++k)
  {
    if (v_p_i[k]<min_i_) min_i_=v_p_i[k];
    else if (v_p_i[k]>max_i_) max_i_=v_p_i[k];

    if (v_p_j[k]<min_j_) min_j_=v_p_j[k];
    else if (v_p_j[k]>max_j_) max_j_=v_p_j[k];
  }
}

//: Set to disk of radius r
//  Select pixels in disk s.t. x^x+y^y<r^r
void vil_structuring_element::set_to_disk(double r)
{
  std::vector<int> px,py;
  double r2 = r*r;
  int r0 = int(r+1);
  for (int j=-r0;j<=r0;++j)
    for (int i=-r0;i<=r0;++i)
      if (i*i+j*j<r2) { px.push_back(i); py.push_back(j); }
  set(px,py);
}

//: Set to line along i (ilo,0)..(ihi,0)
void vil_structuring_element::set_to_line_i(int ilo, int ihi)
{
  p_i_.resize(1+ihi-ilo);
  p_j_.resize(1+ihi-ilo);
  for (int i = ilo;i<=ihi;++i)
  {
    p_i_[i-ilo]=i; p_j_[i-ilo]=0;
  }

  min_i_ = ilo; max_i_ = ihi;
  min_j_ = 0;   max_j_ = 0;
}

//: Set to line along j (jlo,0)..(jhi,0)
void vil_structuring_element::set_to_line_j(int jlo, int jhi)
{
  p_i_.resize(1+jhi-jlo);
  p_j_.resize(1+jhi-jlo);
  for (int j = jlo;j<=jhi;++j)
  {
    p_i_[j-jlo]=0; p_j_[j-jlo]=j;
  }

  min_i_ = 0;   max_i_ = 0;
  min_j_ = jlo; max_j_ = jhi;
}

//: Write details to stream
std::ostream& operator<<(std::ostream& os, const vil_structuring_element& element)
{
  os<<"Bounds ["
    <<element.min_i()<<','<<element.max_i()<<"]["
    <<element.min_j()<<','<<element.max_j()<<"] Points: ";
  for (unsigned int k=0;k<element.p_i().size();++k)
    os<<'('<<element.p_i()[k]<<','<<element.p_j()[k]<<") ";
  return os;
}

//: Generate a list of offsets for use on image with istep,jstep
//  Gives an efficient way of looping through all the pixels in the structuring element
void vil_compute_offsets(std::vector<std::ptrdiff_t>& offset, const vil_structuring_element& element,
                         std::ptrdiff_t istep, std::ptrdiff_t jstep)
{
  unsigned n = element.p_i().size();
  offset.resize(n);
  for (unsigned int k=0;k<n;++k)
    offset[k] = static_cast<std::ptrdiff_t>(element.p_i()[k]*istep +  element.p_j()[k]*jstep);
}
