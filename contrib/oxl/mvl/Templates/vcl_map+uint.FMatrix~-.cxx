// Instantiation of vcl_map<uint.FMatrix*>
#include <vcl_functional.h>
#include <vcl_map.txx>
#include <mvl/FMatrix.h>

VCL_MAP_INSTANTIATE(unsigned int, FMatrix*, vcl_less<unsigned int>);
