#include <vcl/vcl_map.h>
#include <vcl/vcl_vector.txx>

typedef vcl_map<unsigned int, unsigned int, vcl_less<unsigned int> > T;
VCL_VECTOR_INSTANTIATE(T);
