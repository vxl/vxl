// This is contrib/prip/vmap/vmap_oriented_kernel.txx
#include "vmap_oriented_kernel.h"


template <class vmap_kernel>
void vmap_oriented_kernel<vmap_kernel>::initialise()
{
    int elts = permutation().nb_elements() ;
    clear() ;
    _Base::initialise() ;
		_mark.reserve(elts) ;
    for (int i=0;i<elts; ++i)
    {
        _mark.push_back(false) ;
    }
}

template <class vmap_kernel>
bool vmap_oriented_kernel<vmap_kernel>::add(const dart_iterator & arg)
{
		if (_mark[permutation().index_of(arg)] || !_Base::add(arg))
        return false ;

    dart_iterator tmp=arg ;

    do
    {
        _mark[permutation().index_of(tmp)]=true ;
        permutation().next_on_cycle(tmp) ;
    }
    while (tmp!=arg) ;
		tmp=arg; permutation().opposite(tmp) ;
    _mark[permutation().index_of(tmp)]=true ;

    return true ;
}
