// .NAME        vtol_list_functions - Base class of vtol_topology_object (inferior/superior mechanics)
// .LIBRARY     vtol
// .INCLUDE     vtol/vtol_list_functions.h
// .FILE        vtol_list_functions.cxx
//
// .SECTION Description
//  These are a set of functions that can be used to process lists
//
// .SECTION Author
//     Patricia A. Vrobel - ported Peter Tu
//



#ifndef vtol_list_functions_H
#define vtol_list_functions_H

// for now just include the 3d verison

// #include <vtol/vtol_list_functions_3d.h>

#include <vcl_list.h>
#include <vcl_vector.h>
#include <vsol/vsol_spatial_object_3d_sptr.h>
#include <vtol/vtol_topology_object_sptr.h>

//: \brief function used to get rid of duplicate entries

void tagged_union(vcl_vector<vsol_spatial_object_3d *> *);

void tagged_union(vcl_list<vtol_topology_object_sptr> *);


#endif
