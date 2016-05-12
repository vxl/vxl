#include <iostream>
#include <vector>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_list_functions.hxx>
#include <vcl_compiler.h>

template std::vector<vtol_one_chain*>* tagged_union(std::vector<vtol_one_chain*>*);
