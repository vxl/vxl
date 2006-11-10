// This is mul/mbl/tests/test_stepwise_regression.cxx
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vcl_iterator.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_ref.h>

#include <mbl/mbl_stepwise_regression.h>
#include <mbl/mbl_mod_gram_schmidt.h>
#include <testlib/testlib_test.h>

void test_stepwise_regression()
{
    vcl_cout << "**************************\n"
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
    

    vcl_vector<vnl_vector<double > > bases(nbases);
    vnl_vector<double >  xmean(ndims);
    vnl_vector<double> sigmas(nbases);
    vnl_vector<double> vars(nbases);
    double sigy = 1.0;
    double vary=sigy*sigy;
    
    for (unsigned k=0;k<nbases;++k)
    {
        bases[k].set_size(ndims);
        vars[k] = 10.0*vary + 5.0*double(nbases-k);
        sigmas[k] = vcl_sqrt(vars[k]);
    }
    vcl_cout<<"generating bases with "<<nbases<< " bases"<<vcl_endl;
    for (int j=0;j<ndims;++j)
    {
        for (unsigned k=0;k<nbases;++k)
        {
            bases[k][j] = mz_random.normal64();
        }
        xmean[j] = 100.0*vcl_fabs(mz_random.normal64());
    }
    for (unsigned k=0;k<nbases;++k)
    {
        bases[k].normalize();
    }
    vnl_matrix<double> xbasis(ndims,nbases);
    vnl_matrix<double> orthog_basis(ndims,nbases);
    
    for(unsigned k=0;k<nbases;++k)
    {
        xbasis.set_column(k,bases[k]);
    }
    mbl_mod_gram_schmidt(xbasis,orthog_basis);
 
    vcl_cout<<"generating data for "<<num_examples<< " examples with "<<ndims<< " dimensions"<<vcl_endl;
    
    unsigned num_signif = 5;
    vcl_vector<unsigned> signifIndices;
    signifIndices.push_back(0);
    signifIndices.push_back(2);
    signifIndices.push_back(4);
    signifIndices.push_back(6);
    signifIndices.push_back(8);
    vcl_vector<double > coeffs;
    coeffs.push_back(1.0);
    coeffs.push_back(0.8);
    coeffs.push_back(0.6);
    coeffs.push_back(0.5);
    coeffs.push_back(0.4);
    const double mu=100.0;
    for (int i=0;i<num_examples;++i)
    {
        double* pxdata = data[i];
        vnl_vector_ref<double> xdata(ndims,pxdata);
        vcl_copy(xmean.begin(),xmean.end(),pxdata);
        //Generate the x value from the principal components
        for (unsigned k=0;k<nbases;++k)
        {
            xdata += mz_random.normal64()*sigmas[k]*orthog_basis.get_column(k);
        }

        // Add a bit of noise
        for (int j=0;j<ndims;++j)
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
    vcl_set<unsigned> basis=swRegressor.basis();

    TEST("Weights and basis set have a consistent size",basis.size()+1 == weights.size(),true);
    TEST("basis set contains all it should",basis.size() >= signifIndices.size(),true);
    TEST("basis set contains at most one spurious variable",basis.size() <= signifIndices.size()+1,true);
    
    vcl_set<unsigned>::iterator failedIter=basis.end();
    vcl_set<unsigned>::iterator seeker=basis.begin();
    vcl_vector<unsigned >::iterator signifIter=signifIndices.begin();
    vcl_vector<unsigned >::iterator signifIterEnd=signifIndices.end();
    while( signifIter !=  signifIterEnd)
    {
        seeker=basis.find(*signifIter);
        vcl_cout<<"Checking for variable "<< *signifIter<<" in basis"<<vcl_endl;
        TEST("Significant variable located",seeker==failedIter,false);
        ++signifIter;
    }
    

    //Now check that the returned weights are the correlation coefficients
    vnl_vector<double > delta(basis.size(),0.0);
    vcl_set<unsigned >::iterator basisIter=basis.begin();
    for(unsigned k=0;k<delta.size();++k,++basisIter)
    {
        //Note the ordering can be different if there is a spurious variable somewhere in the basis
        //So find the position of the basis variable in the coefficient vector
        vcl_vector<unsigned >::iterator seekIter=vcl_find(signifIndices.begin(),signifIndices.end(),*basisIter);
        if(seekIter != signifIndices.end())
        {
            //If the basis variable is significant
            unsigned kprime=vcl_distance(signifIndices.begin(),seekIter);
            delta[k]=weights[k] - coeffs[kprime];
        }
        else
            TEST_NEAR("insignificant variables have close to zero coefficient",weights[k],0.0,0.02);
    }
    TEST_NEAR("Recovered coefficients correctly",delta.inf_norm(),0.0,0.05);
}

TESTMAIN(test_stepwise_regression);

