#include <iostream>
#include <vector>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_list_functions.hxx>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template std::vector<vtol_edge*>* tagged_union(std::vector<vtol_edge*>*);
