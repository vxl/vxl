// This is mul/mbl/tests/test_stepwise_regression.cxx
#include <cstddef>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_random.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_ref.h>

#include <mbl/mbl_stepwise_regression.h>
#include <mbl/mbl_mod_gram_schmidt.h>
#include <testlib/testlib_test.h>

void test_stepwise_regression()
{
    std::cout << "**************************\n"
             << " Testing mbl_stepwise_regression\n"
             << "**************************\n";

    vnl_random mz_random;
    mz_random.reseed(123456);


    // Generate underlying basis vectors
    unsigned ndims=10;
    unsigned nbases=ndims-1; //Some degeneracy
    unsigned num_examples = 1000;
    vnl_matrix<double> data(num_examples,ndims);
    vnl_vector<double > ydata(num_examples);

    std::vector<vnl_vector<double > > bases(nbases);
    vnl_vector<double >  xmean(ndims);
    vnl_vector<double> sigmas(nbases);
    vnl_vector<double> vars(nbases);
    double sigy = 1.0;
    double vary=sigy*sigy;

    for (unsigned k=0;k<nbases;++k)
    {
        bases[k].set_size(ndims);
        vars[k] = 10.0*vary + 5.0*double(nbases-k);
        sigmas[k] = std::sqrt(vars[k]);
    }
    std::cout<<"generating bases with "<<nbases<< " bases"<<std::endl;
    for (unsigned int j=0;j<ndims;++j)
    {
        for (unsigned k=0;k<nbases;++k)
        {
            bases[k][j] = mz_random.normal64();
        }
        xmean[j] = 100.0*std::fabs(mz_random.normal64());
    }
    for (unsigned k=0;k<nbases;++k)
    {
        bases[k].normalize();
    }
    vnl_matrix<double> xbasis(ndims,nbases);
    vnl_matrix<double> orthog_basis(ndims,nbases);

    for (unsigned k=0;k<nbases;++k)
    {
        xbasis.set_column(k,bases[k]);
    }
    mbl_mod_gram_schmidt(xbasis,orthog_basis);

    std::cout<<"generating data for "<<num_examples<< " examples with "<<ndims<< " dimensions"<<std::endl;

    unsigned num_signif = 5;
    std::vector<unsigned> signifIndices;
    signifIndices.push_back(0);
    signifIndices.push_back(2);
    signifIndices.push_back(4);
    signifIndices.push_back(6);
    signifIndices.push_back(8);
    std::vector<double > coeffs;
    coeffs.push_back(1.0);
    coeffs.push_back(0.8);
    coeffs.push_back(0.6);
    coeffs.push_back(0.5);
    coeffs.push_back(0.4);
    constexpr double mu = 100.0;
    for (unsigned int i=0;i<num_examples;++i)
    {
        double* pxdata = data[i];
        vnl_vector_ref<double> xdata(ndims,pxdata);
        std::copy(xmean.begin(),xmean.end(),pxdata);
        //Generate the x value from the principal components
        for (unsigned k=0;k<nbases;++k)
        {
            xdata += mz_random.normal64()*sigmas[k]*orthog_basis.get_column(k);
        }

        // Add a bit of noise
        for (unsigned int j=0;j<ndims;++j)
        {
            xdata[j]+=0.1*mz_random.normal64();
        }
        //Set base y to mean plus a bit of noise
        ydata[i] = mu+sigy*mz_random.normal64();
        //And add in terms for the significant sub-variables
        for (unsigned k=0;k<num_signif;++k)
        {
            unsigned kprime = signifIndices[k];
            ydata[i] += coeffs[k]*xdata[kprime];
        }
    }
    //Now do stepwise regression to recover the significant components
    mbl_stepwise_regression swRegressor(data,ydata);
    swRegressor();
    vnl_vector<double > weights=swRegressor.weights();
    std::set<unsigned> basis=swRegressor.basis();

    TEST("Weights and basis set have a consistent size",basis.size()+1 == weights.size(),true);
    TEST("basis set contains all it should",basis.size() >= signifIndices.size(),true);
    TEST("basis set contains at most one spurious variable",basis.size() <= signifIndices.size()+1,true);

    auto failedIter=basis.end();
    auto seeker=basis.begin();
    auto signifIter=signifIndices.begin();
    auto signifIterEnd=signifIndices.end();
    while (signifIter !=  signifIterEnd)
    {
        seeker=basis.find(*signifIter);
        std::cout<<"Checking for variable "<< *signifIter<<" in basis"<<std::endl;
        TEST("Significant variable located",seeker==failedIter,false);
        ++signifIter;
    }

    //Now check that the returned weights are the correlation coefficients
    vnl_vector<double > delta(basis.size(),0.0);
    auto basisIter=basis.begin();
    for (unsigned k=0;k<delta.size();++k,++basisIter)
    {
        //Note the ordering can be different if there is a spurious variable somewhere in the basis
        //So find the position of the basis variable in the coefficient vector
        auto seekIter=std::find(signifIndices.begin(),signifIndices.end(),*basisIter);
        if (seekIter != signifIndices.end())
        {
            //If the basis variable is significant
            std::ptrdiff_t kprime=std::distance(signifIndices.begin(),seekIter);
            delta[k]=weights[k] - coeffs[kprime];
        }
        else
            TEST_NEAR("insignificant variables have close to zero coefficient",weights[k],0.0,0.02);
    }
    TEST_NEAR("Recovered coefficients correctly",delta.inf_norm(),0.0,0.05);
}

TESTMAIN(test_stepwise_regression);
