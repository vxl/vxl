// Instantiation of vcl_map<unsigned int,LineSegSet>
#include <mvl/LineSegSet.h>
#include <vcl_functional.h>
#include <vcl_map.txx>
VCL_MAP_INSTANTIATE(unsigned int, LineSegSet, vcl_less<unsigned int>);
