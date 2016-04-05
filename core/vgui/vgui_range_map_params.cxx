#include "vgui_range_map_params.h"

std::vector<std::string> minit()
{
  std::vector<std::string> m(5);
  m[0]="RGBA"; m[1]="RGB"; m[2]= "XRG";
  m[3]="RXB"; m[4]="RGX";
  return m;
}

std::vector<std::string> vgui_range_map_params::bmap = minit();
