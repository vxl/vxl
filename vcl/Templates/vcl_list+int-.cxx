#include <vcl_iostream.h>
#include <vcl_iterator.h>

#include <vcl_list.txx>
#include <vcl_algorithm.txx>

VCL_LIST_INSTANTIATE(int);

VCL_COPY_INSTANTIATE(vcl_list<int>::const_iterator, vcl_ostream_iterator<int>);
VCL_COPY_INSTANTIATE(vcl_list<int>::iterator, vcl_ostream_iterator<int>);
VCL_FIND_INSTANTIATE(vcl_list<int>::iterator, int);
