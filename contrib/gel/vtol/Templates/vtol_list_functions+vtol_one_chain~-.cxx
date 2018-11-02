#include <iostream>
#include <vector>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_list_functions.hxx>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template std::vector<vtol_one_chain*>* tagged_union(std::vector<vtol_one_chain*>*);
