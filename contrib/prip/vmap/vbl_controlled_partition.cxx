// This is contrib/prip/vmap/vbl_controlled_partition.cxx
#include "vbl_controlled_partition.h"


vbl_controlled_partition::vbl_controlled_partition()
{
    _nb_sets=0 ;
}

vbl_controlled_partition::~vbl_controlled_partition()
{}

void vbl_controlled_partition::initialise(int size)
{
    _elements.resize(size) ;
    for (int i=0; i<size;i++)
    {
        _elements[i].father=i ;
    }
    _nb_sets=size ;
}

vbl_controlled_partition & vbl_controlled_partition::operator=(const vbl_controlled_partition & arg) 
{
	 _elements.resize(arg._elements.size()) ;
    for (int i=0; i<_elements.size();i++)
    {
        _elements[i].father=arg._elements[i].father ;
    }
		_nb_sets=arg._nb_sets ;
}
		
void vbl_controlled_partition::set_as_representative(int i)
{
    int tmp1 = i ;

    // Compress Path to i
    while (tmp1!=_elements[tmp1].father)
    {
        int tmp2 = _elements[tmp1].father ;
        _elements[tmp1].father=i ;
        tmp1=tmp2 ;
    }
    _elements[tmp1].father=i ;
}

int vbl_controlled_partition::representative(int i) const
{
    int tmp1 = i, tmp2=tmp1 ;
    // find set representant
    while (tmp1!=_elements[tmp1].father)
    {
        tmp1=_elements[tmp1].father ;
    }
    // compress paths
    while (tmp2!=_elements[tmp2].father)
    {
        int tmp3 = _elements[tmp2].father ;
        const_cast<int&>(_elements[tmp2].father)=tmp1 ;
        tmp2=tmp3 ;
    }

    return tmp1 ;
}
