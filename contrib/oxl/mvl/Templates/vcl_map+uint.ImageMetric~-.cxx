// Instantiation of vcl_map<uint.ImageMetric*>
#include <vcl/vcl_functional.h>
#include <vcl/vcl_map.txx>
class ImageMetric;

VCL_MAP_INSTANTIATE(unsigned int, ImageMetric*, vcl_less<unsigned int>);
