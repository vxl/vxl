// Instantiation of vcl_map<unsigned int,FMatrix*>
class FMatrix;
#include <vcl_functional.h>
#include <vcl_map.txx>
VCL_MAP_INSTANTIATE(unsigned int, FMatrix*, vcl_less<unsigned int>);
