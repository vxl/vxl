// This is prip/vmap/vbl_controlled_partition.cxx
#include "vbl_controlled_partition.h"

vbl_controlled_partition::vbl_controlled_partition()
{
  nb_sets_=0 ;
}

vbl_controlled_partition::~vbl_controlled_partition()
{
}

void vbl_controlled_partition::initialise(int size)
{
  elements_.resize(size) ;
  for (int i=0; i<size;i++)
  {
    elements_[i].father=i;
  }
  nb_sets_=size ;
}

vbl_controlled_partition & vbl_controlled_partition::operator=(const vbl_controlled_partition & arg)
{
  elements_.resize(arg.elements_.size()) ;
  for (unsigned int i=0; i<elements_.size(); ++i)
  {
    elements_[i].father=arg.elements_[i].father ;
  }
  nb_sets_=arg.nb_sets_ ;
  return *this ;
}

void vbl_controlled_partition::set_as_representative(int i)
{
  int tmp1 = i ;

  // Compress Path to i
  while (tmp1!=elements_[tmp1].father)
  {
    int tmp2 = elements_[tmp1].father ;
    elements_[tmp1].father=i ;
    tmp1=tmp2 ;
  }
  elements_[tmp1].father=i ;
}

int vbl_controlled_partition::representative(int i) const
{
  int tmp1 = i, tmp2=tmp1 ;
  // find set representant
  while (tmp1!=elements_[tmp1].father)
  {
    tmp1=elements_[tmp1].father ;
  }
  // compress paths
  while (tmp2!=elements_[tmp2].father)
  {
    int tmp3 = elements_[tmp2].father ;
    const_cast<int&>(elements_[tmp2].father)=tmp1 ;
    tmp2=tmp3 ;
  }

  return tmp1 ;
}
