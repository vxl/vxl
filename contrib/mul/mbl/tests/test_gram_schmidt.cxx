// This is mul/mbl/tests/test_gram_schmidt.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_mod_gram_schmidt.h>
#include <vnl/vnl_vector.h>
#include <testlib/testlib_test.h>

void test_gram_schmidt()
{
    std::cout << "**************************\n"
             << " Testing mbl_gram_schmidt\n"
             << "**************************\n";

    //Construct a 3d basis in a 4 d space
    vnl_matrix<double > basis(4,3);
    const double v0[4] = {1.0,0.0,0.0,0.0};
    const double v1[4] = {0.25,0.75,0.2,0.0};
    const double v2[4] = {0.1,0.25,0.75,0.0};
    basis.set_column(0,v0);
    basis.set_column(1,v1);
    basis.set_column(2,v2);
    vnl_matrix<double > norm_basis(4,3);

    mbl_mod_gram_schmidt(basis, norm_basis);
    vnl_vector<double >  e0,e1,e2;
    e0 = norm_basis.get_column(0);
    e1 = norm_basis.get_column(1);
    e2  = norm_basis.get_column(2);

    TEST_NEAR("mbl_gram_schmidt e0 norm", e0.magnitude(), 1.0, 1e-8);
    TEST_NEAR("mbl_gram_schmidt e1 norm", e1.magnitude(), 1.0, 1e-8);
    TEST_NEAR("mbl_gram_schmidt e2 norm", e2.magnitude(), 1.0, 1e-8);
    TEST_NEAR("mbl_gram_schmidt e0 and e1 orthogonal", dot_product(e0,e1), 0.0, 1e-8);
    TEST_NEAR("mbl_gram_schmidt e0 and e2 orthogonal", dot_product(e0,e2), 0.0, 1e-8);
    TEST_NEAR("mbl_gram_schmidt e1 and e2 orthogonal", dot_product(e1,e2), 0.0, 1e-8);


    //** test version returning normalisation weights
    vnl_vector<double> normalisers;
    norm_basis.fill(0);
    mbl_mod_gram_schmidt(basis, norm_basis, normalisers);
    vnl_vector<double> f0, f1, f2;
    f0 = norm_basis.get_column(0);
    f1 = norm_basis.get_column(1);
    f2 = norm_basis.get_column(2);

    TEST_NEAR("mbl_gram_schmidt e0 norm", f0.magnitude(), 1.0, 1e-8);
    TEST_NEAR("mbl_gram_schmidt e1 norm", f1.magnitude(), 1.0, 1e-8);
    TEST_NEAR("mbl_gram_schmidt e2 norm", f2.magnitude(), 1.0, 1e-8);
    TEST_NEAR("mbl_gram_schmidt e0 and e1 orthogonal", dot_product(f0,f1), 0.0, 1e-8);
    TEST_NEAR("mbl_gram_schmidt e0 and e2 orthogonal", dot_product(f0,f2), 0.0, 1e-8);
    TEST_NEAR("mbl_gram_schmidt e1 and e2 orthogonal", dot_product(f1,f2), 0.0, 1e-8);

    TEST_EQUAL("basis variants e0, f0", e0==f0, true);
    TEST_EQUAL("basis variants e1, f1", e1==f1, true);
    TEST_EQUAL("basis variants e2, f2", e2==f2, true);
}

TESTMAIN(test_gram_schmidt);
