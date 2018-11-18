//:
// \file
// \author Martin Roberts
// \brief Do nowt

#include "mipa_identity_normaliser.h"
#include <vnl/vnl_vector.h>
#include <vsl/vsl_binary_loader.h>

//=======================================================================

void mipa_identity_normaliser::normalise(vnl_vector<double>& ) const
{
}

//=======================================================================

std::string mipa_identity_normaliser::is_a() const
{
    return std::string("mipa_identity_normaliser");
}

//: Create a copy on the heap and return base class pointer
mipa_vector_normaliser* mipa_identity_normaliser::clone() const
{
    return new mipa_identity_normaliser();
}

//: Print class to os
void mipa_identity_normaliser::print_summary(std::ostream& os) const
{
    os<<is_a()<<'\n';
}

constexpr static short version_no = 1;

//: Save class to binary file stream
void mipa_identity_normaliser::b_write(vsl_b_ostream& bfs) const
{
    vsl_b_write(bfs,version_no);
}


//: Load class from binary file stream
void mipa_identity_normaliser::b_read(vsl_b_istream& bfs)
{
    short version;
    vsl_b_read(bfs,version);
}
