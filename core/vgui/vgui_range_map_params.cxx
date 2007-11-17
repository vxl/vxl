#ifndef vgui_range_map_params_cxx_
#define vgui_range_map_params_cxx_

#include <vgui/vgui_range_map_params.h>

vcl_vector<vcl_string> minit()
{
  vcl_vector<vcl_string> m(5);
  m[0]="RGBA"; m[1]="RGB"; m[2]= "XRG";
  m[3]="RXB"; m[4]="RGX";
  return m;
}

vcl_vector<vcl_string> vgui_range_map_params::bmap = minit();

#endif // vgui_range_map_params_cxx_
