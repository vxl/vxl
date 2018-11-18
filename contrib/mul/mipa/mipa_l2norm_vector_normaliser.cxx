//:
// \file
// \author Martin Roberts
// \brief Normalise the sampled vector by its overall L2 norm

#include "mipa_l2norm_vector_normaliser.h"
#include <vnl/vnl_vector.h>
#include <vsl/vsl_binary_loader.h>

//=======================================================================

void mipa_l2norm_vector_normaliser::normalise(vnl_vector<double>& v) const
{
    // The mapping is g-> g/(L2norm(g)+epsilon)

    const double epsilon=1.0E-4;//lower bound to avoid divide by zero in flat regions

    double L2norm=std::sqrt(v.squared_magnitude()+epsilon);
    double L2inv=1.0/L2norm;
    v *= L2inv;
}

//=======================================================================

std::string mipa_l2norm_vector_normaliser::is_a() const
{
    return std::string("mipa_l2norm_vector_normaliser");
}

//: Create a copy on the heap and return base class pointer
mipa_vector_normaliser* mipa_l2norm_vector_normaliser::clone() const
{
    return new mipa_l2norm_vector_normaliser(*this);
}

//: Print class to os
void mipa_l2norm_vector_normaliser::print_summary(std::ostream& os) const
{
    os<<is_a()<<'\n';
}

constexpr static short version_no = 1;

//: Save class to binary file stream
void mipa_l2norm_vector_normaliser::b_write(vsl_b_ostream& bfs) const
{
    vsl_b_write(bfs,version_no);
}


//: Load class from binary file stream
void mipa_l2norm_vector_normaliser::b_read(vsl_b_istream& bfs)
{
    short version;
    vsl_b_read(bfs,version);
}
