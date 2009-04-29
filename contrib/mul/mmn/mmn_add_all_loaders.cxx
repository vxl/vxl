#include "mmn_add_all_loaders.h"
//:
// \file
// \brief Add all binary loaders and factory objects for mmn library
// \author Tim Cootes

#include <mbl/mbl_cloneables_factory.h>
#include "mmn_dp_solver.h"
#include "mmn_lbp_solver.h"

//: Add all binary loaders and factory objects for mmn library
void mmn_add_all_loaders()
{

  mbl_cloneables_factory<mmn_solver>::add(mmn_dp_solver());
  vsl_add_to_binary_loader(mmn_dp_solver());

  mbl_cloneables_factory<mmn_solver>::add(mmn_lbp_solver());
  vsl_add_to_binary_loader(mmn_lbp_solver());
}
