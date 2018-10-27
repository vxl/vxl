#include <iostream>
#include <vector>
#include <vcl_compiler.h>
#include <vcl_compiler.h>
#include <rrel/rrel_util.hxx>

// Apply explicit instantiation
typedef std::vector<double>::iterator Iter;
RREL_UTIL_INSTANTIATE_RAN_ITER(double, Iter);
