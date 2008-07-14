#include "mipa_add_all_loaders.h"
//:
// \file
#include <mbl/mbl_cloneables_factory.h>
#include <mipa/mipa_vector_normaliser.h>
#include <mipa/mipa_l2norm_vector_normaliser.h>
#include <mipa/mipa_identity_normaliser.h>
#include <mipa/mipa_block_normaliser.h>
#include <mipa/mipa_ms_block_normaliser.h>

//: Add all binary loaders and factory objects for mipa library
void mipa_add_all_loaders()
{


    mbl_cloneables_factory<mipa_vector_normaliser>::add(mipa_identity_normaliser());
    mbl_cloneables_factory<mipa_vector_normaliser>::add(mipa_l2norm_vector_normaliser());
    mbl_cloneables_factory<mipa_vector_normaliser>::add(mipa_block_normaliser());
    mbl_cloneables_factory<mipa_vector_normaliser>::add(mipa_ms_block_normaliser());
    vsl_add_to_binary_loader(mipa_identity_normaliser());
    vsl_add_to_binary_loader(mipa_l2norm_vector_normaliser());
    vsl_add_to_binary_loader(mipa_block_normaliser());
    vsl_add_to_binary_loader(mipa_ms_block_normaliser());

}
