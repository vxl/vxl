// This is mul/mbl/mbl_mod_gram_schmidt.cxx
#include "mbl_mod_gram_schmidt.h"
//:
// \file
// \brief Orthoganalise a basis using modified Gram-Schmidt (and normalise)
// \author Martin Roberts

#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for vcl_abort()
#include <vnl/vnl_math.h>

//=======================================================================
//: Orthoganalise a basis using modified Gram-Schmidt
// Transform basis {vk} to orthonormal basis {ek} with k in range 1..N
// for j = 1 to N
//     ej = vj
//     for k = 1 to j-1
//         ej = ej - <ej,ek>ek   //NB Classical GS has vj in inner product
//     end
//     ej = ej/|ej|
//  end

//: Convert input basis {v} to orthonormal basis {e}
// Each basis vector is a column of v, and likewise the orthonormal bases are returned as columns of e
void mbl_mod_gram_schmidt(const vnl_matrix<double>& v,
                          vnl_matrix<double>& e)
{
    unsigned N=v.cols();

    //Note internally it is easier to deal with the basis as a vector of vnl_vectors
    //As matrices are stored row-wise
    vcl_vector<vnl_vector<double > > vbasis(N);
    vcl_vector<vnl_vector<double > > evecs(N);

    //Copy into more convenient holding storage as vector of vectors
    //And also initialise output basis to input
    for(unsigned jcol=0;jcol<N;++jcol)
    {
        evecs[jcol] = vbasis[jcol] = v.get_column(jcol);
    }
    evecs[0].normalize();

    for (unsigned j=1;j<N;++j)
    {
        //Loop over previously created bases and subtract off partial projections
        //Thus producing orthogonality

        unsigned n2 = j-1;
        for (unsigned k=0;k<=n2;++k)
        {
            evecs[j] -= dot_product(evecs[j],evecs[k]) * evecs[k];
        }
        evecs[j].normalize(); 
    }

    //And copy into column-wise matrix (kth base is the kth column)
    e.set_size(v.rows(),N);
    for(unsigned jcol=0;jcol<N;++jcol)
    {
        e.set_column(jcol,evecs[jcol]);
    }
}
