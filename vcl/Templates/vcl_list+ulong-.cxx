#include <vcl/vcl_list.txx>
#include <vcl/vcl_algorithm.txx>

VCL_LIST_INSTANTIATE(unsigned long);
VCL_FIND_INSTANTIATE_ITER(vcl_list<unsigned long>::iterator, unsigned long);
#ifdef VCL_REMOVE_INSTANTIATE
VCL_REMOVE_INSTANTIATE(vcl_list<unsigned long>::iterator, unsigned long);
#endif
