
//
// .SECTION Description
//  These are a set of functions that can be used to process lists

//
// .NAME        vtol_list_functions_2d - Base class of vtol_topology_object_2d (inferior/superior mechanics)
// .LIBRARY     vtol
// .INCLUDE     Topology/vtol_list_functions_2d.h
// .FILE        vtol_list_functions_2d.h
// .FILE        vtol_list_functions_2d.cxx
// .SECTION Author
//     Patricia A. Vrobel - ported Peter Tu
//



#ifndef vtol_list_functions_2d_H
#define vtol_list_functions_2d_H

#include <vcl/vcl_vector.h>

class vsol_spatial_object_2d;

//: \brief function used to get rid of duplicate entries
void tagged_union(vcl_vector<vsol_spatial_object_2d*>*);


#endif
