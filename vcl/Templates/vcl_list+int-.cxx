#include <vcl/vcl_iostream.h>
#include <vcl/vcl_iterator.txx>

#include <vcl/vcl_list.txx>
#include <vcl/vcl_iterator.txx>

VCL_LIST_INSTANTIATE(int);

VCL_COPY_INSTANTIATE(vcl_list<int>::const_iterator, vcl_ostream_iterator<int>);
VCL_COPY_INSTANTIATE(vcl_list<int>::iterator, vcl_ostream_iterator<int>);
