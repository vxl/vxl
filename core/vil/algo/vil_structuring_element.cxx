// This is core/vil/algo/vil_structuring_element.cxx
#include "vil_structuring_element.h"
//:
// \file
// \brief Structuring element for morphology represented as a list of non-zero pixels
// \author Tim Cootes

#include <vcl_cassert.h>
#include <vcl_iostream.h>

//: Define elements { (p_i[k],p_j[k]) }
vil_structuring_element::vil_structuring_element(const vcl_vector<int>& p_i,
                                                 const vcl_vector<int>& p_j)
{
  set(p_i,p_j);
}

//: Define elements { (p_i[k],p_j[k]) }
void vil_structuring_element::set(const vcl_vector<int>& p_i,const vcl_vector<int>& p_j)
{
  assert(p_i.size()==p_j.size());
  assert(p_i.size()>0);
  p_i_ = p_i;
  p_j_ = p_j;

  max_i_=min_i_ = p_i[0];
  max_j_=min_j_ = p_j[0];
  for (unsigned int k=1;k<p_i.size();++k)
  {
    if (p_i[k]<min_i_) min_i_=p_i[k];
    else if (p_i[k]>max_i_) max_i_=p_i[k];

    if (p_j[k]<min_j_) min_j_=p_j[k];
    else if (p_j[k]>max_j_) max_j_=p_j[k];
  }
}

//: Set to disk of radius r
//  Select pixels in disk s.t. x^x+y^y<=r^r
void vil_structuring_element::set_to_disk(double r)
{
  vcl_vector<int> px,py;
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
vcl_ostream& operator<<(vcl_ostream& os, const vil_structuring_element& element)
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
void vil_compute_offsets(vcl_vector<vcl_ptrdiff_t>& offset, const vil_structuring_element& element,
                         vcl_ptrdiff_t istep, vcl_ptrdiff_t jstep)
{
  unsigned n = element.p_i().size();
  offset.resize(n);
  for (unsigned int k=0;k<n;++k)
    offset[k] = static_cast<vcl_ptrdiff_t>(element.p_i()[k]*istep +  element.p_j()[k]*jstep);
}

