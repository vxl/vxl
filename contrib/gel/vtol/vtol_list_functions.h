#ifndef vtol_list_functions_H
#define vtol_list_functions_H
//:
//  \file
// \brief Base class of vtol_topology_object (inferior/superior mechanics)
//
//  These are a set of functions that can be used to process lists
//
// \author
//     Patricia A. Vrobel - ported Peter Tu

#include <vcl_list.h>
#include <vcl_vector.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vtol/vtol_topology_object_sptr.h>

//: \brief function used to get rid of duplicate entries

void tagged_union(vcl_vector<vsol_spatial_object_2d *> *);

void tagged_union(vcl_list<vtol_topology_object_sptr> *);

#endif // vtol_list_functions_H
