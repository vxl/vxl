// This is mul/mbl/mbl_stepwise_regression.cxx
#include "mbl_stepwise_regression.h"
//:
// \file
// \brief Conduct stepwise regression
// \author Martin Roberts

#include <vcl_algorithm.h>
#include <vcl_iterator.h>
#include <vcl_utility.h>
#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for vcl_abort()
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_svd.h>
#include <mbl/mbl_stl.h>


mbl_stepwise_regression::mbl_stepwise_regression(const vnl_matrix<double>& x,
                                                 const vnl_vector<double>& y):
        x_(x),y_(y),
        num_examples_(y.size()),
        num_vars_(x.cols()),
        XtX_(num_vars_+1,num_vars_+1,0.0),XtY_(num_vars_+1,0.0),
        FthreshAdd_(2.07),FthreshRemove_(1.8),
        mode_(mbl_stepwise_regression::eFORWARDS)
{
    assert(x.rows() == num_examples_);
    //Initialise the extended covariances

#if 0 // The calculation is as follows
    do
    {
        // XtX += [x, -1]' * [x, -1]
        const vnl_vector<double> &x=inputs.current();
        double y = outputs[inputs.index()] ? 1.0 : -1.0;
        vnl_vector<double> xp(k+1);
        xp.update(x, 0);
        xp(k) = -1.0;
        XtX += outer_product(xp, xp);
        double y = outputs[inputs.index()] ? 1.0 : -1.0;
        XtY += y * xp;
    } while (inputs.next());

#else// However the following version is faster

    rss_ = 0.0;
    for (unsigned irow=0;irow<num_examples_;++irow)
    {
        // XtX += [x, -1]' * [x, -1]
        const double* px=x_[irow];
        double yval = y_[irow];
        for (unsigned i=0; i<num_vars_; ++i)
        {
            //Just work out in triangular form first
            for (unsigned j=0; j<i; ++j)
            {
                XtX_(i,j) += px[i] * px[j];
            }
            XtX_(i,i) += vnl_math_sqr(px[i]);
            XtX_(i,num_vars_) -= px[i];
            XtY_(i) += yval * px[i];
        }
        XtY_(num_vars_) += yval * -1.0;
        rss_ += yval * yval;
    }
    //Initialise residual sum of squares to total sum of y squares about mean
    double sumY = -XtY_(num_vars_);
    rss_ -= (sumY*sumY)/double(num_examples_);
    //Set other half by symmetry
    for (unsigned i=0; i<num_vars_; ++i)
    {
        for (unsigned j=0; j<i; ++j)
        {
            XtX_(j,i) += XtX_(i,j);
        }
        XtX_(num_vars_,i) = XtX_(i,num_vars_);
    }
    XtX_(num_vars_, num_vars_) = (double) num_examples_;

#endif // !0
}

void mbl_stepwise_regression::operator()()
{
    using mbl_stepwise_regression_helpers::lsfit_this_basis;
    if (mode_ == eFORWARDS)
    {
        basis_.clear(); //start from an empty basis
        basis_complement_.clear();
        //And fill the complement with 1..n
        mbl_stl_increments_n(vcl_inserter(basis_complement_,basis_complement_.end()),
                            num_vars_,0);

        //First fit a starting minimally sized basis best sum of squares first
        unsigned  min_basis = vcl_min(num_vars_/10,num_examples_/5);
        if (min_basis<1) min_basis = 1;
        bool forceAdd=true;
        for (unsigned i=0; i<min_basis;i++)
        {
            add_variable(forceAdd);
        }
        //And now carry on and do forward stepwise (with some removals)
        do_forward_stepwise_regression();
    }
    else //Backwards
    {
        //start from an full basis
        mbl_stl_increments_n(vcl_inserter(basis_,basis_.end()),
                            num_vars_,0);
        basis_complement_.clear();
        do_backward_stepwise_regression();
    }
    //And ensure solution is set up for the final basis
    lsfit_this_basis fitter(x_,y_,XtX_,XtY_);
    fitter.set_basis(basis_);
    fitter();
    weights_ = fitter.weights();
}

void mbl_stepwise_regression::do_forward_stepwise_regression()
{
    bool carryOn=true;
    while (carryOn)
    {
        //Now try another addition step followed by one elimination
        bool addedOne = add_variable();
        bool removedOne = remove_variable();
        carryOn = addedOne || removedOne ;
    }
}

void mbl_stepwise_regression::do_backward_stepwise_regression()
{
    bool carryOn=true;
    while (carryOn)
    {
        //try to remove all the unwanted variables
        carryOn = remove_variable();
    }
    //Having removed everything we can check if anything might now come back in that had been removed earlier
    do_forward_stepwise_regression();
}

bool mbl_stepwise_regression::add_variable(bool forceAdd)
{
    //Loop over all variables not in the basis
    //Fit an extended basis with that variable in it
    //Store the best such basis (best residual sum of squares)
    //If significant improvement (or forceAdd set) then add the variable

    using mbl_stepwise_regression_helpers::lsfit_this_basis;

    if (basis_complement_.empty()) return false;

    lsfit_this_basis fitter(x_,y_,XtX_,XtY_);
    fitter.set_basis(basis_);
    double FratioMax=-1.0;

    int  knew= -1;
    vcl_set<unsigned>::const_iterator candIter=basis_complement_.begin();
    vcl_set<unsigned>::const_iterator candIterEnd=basis_complement_.end();
    double rssNew=rss_;
    while (candIter != candIterEnd)
    {
        unsigned k = *candIter;
        double rssPrime = fitter.add(k);
        double F = f_ratio(rssPrime,rss_,1);
        if (F>FratioMax)
        {
            FratioMax= F;
            knew = k;
            rssNew = rssPrime;
        }
        ++candIter;
    }
    bool really_added = false;
    bool significant = test_significance(rssNew,rss_,FthreshAdd_);
    if (significant || forceAdd)
    {
        if (knew>=0)
        {
            vcl_pair<vcl_set<unsigned>::iterator,bool> inserted = basis_.insert(knew);
            really_added = inserted.second;
            basis_complement_.erase(knew);
            rss_ = rssNew;
        }
    }
    return significant && really_added;
}

bool mbl_stepwise_regression::remove_variable()
{
    //Loop over all variables in the basis
    //Fit a diminished basis with that variable in it
    //Store the least diminishing (to residual sum of squares) variable
    //If the reduction is not significant then eliminate this variable

    using mbl_stepwise_regression_helpers::lsfit_this_basis;
    lsfit_this_basis fitter(x_,y_,XtX_,XtY_);
    fitter.set_basis(basis_);
    double min_Fratio=1.0E30;
    double rssBest=rss_;
    int knew=-1;
    vcl_set<unsigned>::const_iterator candIter=basis_.begin();
    vcl_set<unsigned>::const_iterator candIterEnd=basis_.end();
    while (candIter != candIterEnd)
    {
        unsigned k = *candIter++;
        double rssNew = fitter.remove(k);
        double F = f_ratio(rss_,rssNew,1);
        if (F < min_Fratio)
        {
            min_Fratio = F;
            knew = k;
            rssBest = rssNew;
        }
    }
    bool significant = test_significance(rss_,rssBest,FthreshRemove_);
    if (!significant)
    {
        basis_.erase(knew);
        basis_complement_.insert(knew);
        rss_ = rssBest;
    }
    return !significant;
}

//---------------------------------------------------------------------------
//------------------------------ Helpers ------------------------------------
//---------------------------------------------------------------------------

double mbl_stepwise_regression_helpers::lsfit_this_basis::add(unsigned k)
{
    basis_.insert(k);
    double rss = (*this)();
    basis_.erase(k);
    return rss;
}

double mbl_stepwise_regression_helpers::lsfit_this_basis::remove(unsigned k)
{
    basis_.erase(k);
    double rss = (*this)();
    basis_.insert(k);
    return rss;
}

double mbl_stepwise_regression_helpers::lsfit_this_basis::operator()()
{
    // Find the solution to X w = Y;
    // However it is easier to find X' X w = X' Y;
    // because X is n_train x n_dims whereas X'X is n_dims x n_dims

    //Solve by SVD
    unsigned ndims = basis_.size();
    //Create working copies of mtrices containing just the subset of variables in the basis
    vnl_matrix<double> XtX(1+ndims,1+ndims);

    vcl_set<unsigned>::iterator basisVarIter=basis_.begin();
    vcl_set<unsigned>::iterator basisVarIterEnd=basis_.end();
    unsigned i = 0;
    vnl_vector<double> XtY(ndims+1, 0.0);

    while (basisVarIter != basisVarIterEnd)
    {
        unsigned k1=*basisVarIter++;
        vcl_set<unsigned>::iterator basisVarInnerIter=basis_.begin();
        unsigned j = 0;
        //Set half of off-diagonals
        while (*basisVarInnerIter < k1) //NB set is ordered
        {
            unsigned k2=*basisVarInnerIter++;
            XtX(i,j) = XtX_(k1,k2);
            ++j;
        }
        XtX(i,i) = XtX_(k1,k1); //diagonal
        XtX(i,ndims) = XtX_(k1,num_vars_); //extra bias column
        XtY(i) = XtY_(k1);
        ++i;
    }
    XtY(ndims) = XtY_(num_vars_);
    //Copy the other half by symmetry
    for (unsigned i=0;i<ndims;++i)
    {
        for (unsigned j=0;j<i;++j)
        {
            XtX(j,i) = XtX(i,j);
        }
        XtX(ndims,i) = XtX(i,ndims);
    }

    XtX(ndims,ndims) = double (num_examples_);

    vnl_svd<double> svd(XtX, 1.0e-12); // 1e-12 = zero-tolerance for singular values
    weights_ = svd.solve(XtY);

    double rss=0.0;
    //Now compute the residual sum of squares
    for (unsigned i=0;i<num_examples_;++i)
    {
        const double* pDataRow=x_[i];
        double ypred = 0.0;
        basisVarIter=basis_.begin();
        vnl_vector<double>::iterator weightIter = weights_.begin();
        while (basisVarIter != basisVarIterEnd)
        {
            ypred += pDataRow[*basisVarIter++] * (*weightIter++);
        }
        ypred -= *weightIter; //Final -1 term
        double dy = y_[i] - ypred;
        rss += dy*dy;
    }
    return rss;
}
