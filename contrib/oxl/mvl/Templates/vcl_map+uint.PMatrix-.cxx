// Instantiation of vcl_map<unsigned int,PMatrix>
#include <mvl/PMatrix.h>
#include <vcl_functional.h>
#include <vcl_map.txx>
VCL_MAP_INSTANTIATE(unsigned int, PMatrix, vcl_less<unsigned int>);
