//: \file
//  \brief Structuring element for morphology represented as a list of non-zero pixels
//  \author Tim Cootes

#include <vil2/algo/vil2_structuring_element.h>

//: Define elements { (p_i[k],p_j[k]) }
void vil2_structuring_element::set(const vcl_vector<int>& p_i,const vcl_vector<int>& p_j)
{
  assert(p_i.size()==p_j.size());
  assert(p_i.size()>0);
  p_i_ = p_i;
  p_j_ = p_j;

  max_i_=min_i_ = p_i[0];
  max_j_=min_j_ = p_j[0];
  for (int k=1;k<p_i.size();++k)
  {
    if (p_i[k]<min_i_) min_i_=p_i[k];
	else if (p_i[k]>max_i_) max_i_=p_i[k];

    if (p_j[k]<min_j_) min_j_=p_j[k];
	else if (p_j[k]>max_j_) max_j_=p_j[k];
  }
}

//: Write details to stream
vcl_ostream& operator<<(vcl_ostream& os, const vil2_structuring_element& element)
{
  os<<"Bounds ["<<element.min_i()<<","<<element.max_i()<<"]["
                <<element.min_j()<<","<<element.max_j()<<"] Points: ";
  for (int k=0;k<element.p_i().size();++k)
    os<<"("<<element.p_i()[k]<<","<<element.p_j()[k]<<")";
  return os;
}

//: Generate a list of offsets for use on image with istep,jstep
//  Gives an efficient way of looping through all the pixels in the structuring element
void vil2_compute_offsets(vcl_vector<int>& offset, const vil2_structuring_element& element,
                          int istep, int jstep)
{
  unsigned n = element.p_i().size();
  offset.resize(n);
  for (int k=0;k<n;++k)
    offset[k] = element.p_i()[k]*istep +  element.p_j()[k]*jstep;
}

