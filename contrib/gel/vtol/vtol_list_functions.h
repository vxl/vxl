#ifndef vtol_list_functions_h_
#define vtol_list_functions_h_
//:
//  \file
// \brief Base class of vtol_topology_object (inferior/superior mechanics)
//
//  These are a set of functions that can be used to process lists
//  of pointers or smart pointers to spatial  objects.
//
// \author
//     Patricia A. Vrobel
// \verbatim
//  Modifications
//     PTU May 2000  ported to vxl
//     Peter Vanroose - July 2000 - made templated
// \endverbatim

#include <vcl_list.h>
#include <vcl_vector.h>

//: \brief function used to get rid of duplicate entries
template <class T>
vcl_vector<T>* tagged_union(vcl_vector<T>*);

//: \brief function used to get rid of duplicate entries
template <class T>
vcl_list<T>* tagged_union(vcl_list<T>*);

#endif // vtol_list_functions_h_
