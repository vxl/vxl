#include <mcal/mcal_add_all_loaders.h>
#include <mbl/mbl_cloneables_factory.h>
#include <mcal/mcal_pca.h>
#include <mcal/mcal_general_ca.h>
#include <mcal/mcal_sparse_basis_cost.h>

//: Add all binary loaders and factory objects for mcal library
void mcal_add_all_loaders()
{
  mbl_cloneables_factory<mcal_component_analyzer>::add(mcal_pca());
  vsl_add_to_binary_loader(mcal_pca());
  mbl_cloneables_factory<mcal_component_analyzer>::add(mcal_general_ca());
  vsl_add_to_binary_loader(mcal_general_ca());

  mbl_cloneables_factory<mcal_single_basis_cost>::add(mcal_sparse_basis_cost());
  vsl_add_to_binary_loader(mcal_sparse_basis_cost());

}
