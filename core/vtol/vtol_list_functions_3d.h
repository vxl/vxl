// .NAME        vtol_list_functions_3d - Base class of vtol_topology_object_3d (inferior/superior mechanics)
// .LIBRARY     vtol
// .INCLUDE     vtol/vtol_list_functions_3d.h
// .FILE        vtol_list_functions_3d.cxx
//
// .SECTION Description
//  These are a set of functions that can be used to process lists
//
// .SECTION Author
//     Patricia A. Vrobel - ported Peter Tu
//



#ifndef vtol_list_functions_3d_H
#define vtol_list_functions_3d_H

#include <vcl_vector.h>

class vsol_spatial_object_3d;

//: \brief function used to get rid of duplicate entries
void tagged_union(vcl_vector<vsol_spatial_object_3d*>*);


#endif
