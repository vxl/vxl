#ifndef my_top_level_h_
#define my_top_level_h_

#include "vmap/tests/my_map.h"
#include "vpyr/tests/my_pyramid.h"
#include "vpyr/vpyr_top_tmap.h"

typedef vpyr_top_tmap<my_tpyramid, my_map_vertex, my_map_edge, my_map_face, my_map_dart> my_top_level ;

void initialise_ids(my_top_level & p) ;

#endif
