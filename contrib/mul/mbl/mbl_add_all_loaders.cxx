#include "mbl_add_all_loaders.h"
//:
// \file
#include <mbl/mbl_data_collector_list.h>
#include <vnl/vnl_vector.h>

//: Add all loaders for vapm library
void mbl_add_all_loaders()
{
  vsl_add_to_binary_loader(mbl_data_collector_list<vnl_vector<double> >());
}
