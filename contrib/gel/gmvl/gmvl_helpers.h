#ifndef gmvl_helpers_h_
#define gmvl_helpers_h_
#ifdef __GNUC__
#pragma interface "gmvl_helpers"
#endif
/*
  crossge@crd.ge.com
*/

#include <vcl/vcl_vector.h>
#include <vcl/vcl_string.h>
#include <gmvl/gmvl_node_ref.h>

// standard double, double list
vcl_vector<gmvl_node_ref> gmvl_load_raw_corners( const vcl_string filename);

gmvl_node_ref gmvl_load_image( const vcl_string filename);

#endif
