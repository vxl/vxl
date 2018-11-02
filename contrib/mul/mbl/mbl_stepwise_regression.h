// This is mul/mbl/mbl_stepwise_regression.h
#ifndef mbl_stepwise_regression_h_
#define mbl_stepwise_regression_h_
//:
// \file
// \brief Conduct stepwise regression
// \author Martin Roberts

#include <iostream>
#include <set>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>


//: Perform the stepwise regression algorithm to determine which subset of variables appear to be significant predictors
class mbl_stepwise_regression
{
    //: Forwards or backwards stepwise (add or remove variables)
    enum step_mode {eFORWARDS,eBACKWARDS};
    //: The data matrix of x values (predictor variables)
    // Each training example is a row, each x-variable dimension corresponds to a column
    const vnl_matrix<double>& x_;
    //: Vector of dependent y values
    const vnl_vector<double>& y_;
    //: number of training examples (i.e. number of rows in x_)
    unsigned num_examples_;
    //: dimensionality - (i.e. number of columns in x_)
    unsigned num_vars_;
    //: [x,-1]'[x,-1]
    vnl_matrix<double> XtX_;
    //: [x,-1]'[y,-1]
    vnl_vector<double> XtY_;
    //: Basis (i.e. all significant variables)
    std::set<unsigned> basis_ ;
    //: All non-basis variables
    std::set<unsigned> basis_complement_ ;
    //: The regression coefficients + constant (final term)
    vnl_vector<double> weights_;

    //: The residual sum of squares
    double rss_;
    //: F-ratio significance threshold for adding a variable
    double FthreshAdd_;
    //: F-ratio significance threshold for removing a variable
    // Note must be less than addition threshold or infinite cycling will occur
    double FthreshRemove_;
    //: forwards or backwards mode
    step_mode mode_;


    //: Add a new variable and return if significant
    // Only added if it makes a significant reduction in RSS, unless forceAdd is set
    // Always adds the variable making most difference to RSS
    bool add_variable(bool forceAdd=false);

    //: Remove a variable that makes no significant difference to RSS
    // Will remove the one that causes least change to RSS
    bool remove_variable();

    double f_ratio(double rssExtended,double rssBase,unsigned q)
    {
        double chi2Extra = rssBase - rssExtended ;
        double f1 = chi2Extra/double(q);
        double f2 = rssExtended/double(num_examples_ - (basis_.size()+q) - 1);
        return f1/f2;
    }
    //: Evaluate F-ratio to test if change in sum of squares was significant
    bool test_significance(double rssExtended,double rssBase, double fthresh)
    {
        return ((f_ratio(rssExtended,rssBase,1) >fthresh) ? true : false);
    }
    //: Step forward through basis complement, adding in new significant variables and removing those that cease to be significant
    void do_forward_stepwise_regression();

    //: Step back and remove all insignificant variables, then try and step forward again
    void do_backward_stepwise_regression();

  public:
    //: Constructor, note you must supply the data references
    // These must remain in scope during algorithm execution
    // The data matrix of x values (predictor variables) is arranged as:
    // Each training example is a row, each x-variable dimension corresponds to a column
    mbl_stepwise_regression(const vnl_matrix<double>& x,
                            const vnl_vector<double>& y);

    //: Run the algorithm as determined by mode
    void operator()();

    //: return the basis variables
    // I.e. those determined to be significantly correlated with y in stepwise search
    const std::set<unsigned > basis() const {return basis_;}

    //: Set the mode to forwards or backwards
    // Note backwards can take a long compute time in a space of high dimension
    void set_mode(step_mode mode) {mode_ = mode;}

    //: Return the regression coefficients + constant (final term)
    const vnl_vector<double >& weights() const {return weights_;}
};

//:Helper stuff for stepwise regression
namespace mbl_stepwise_regression_helpers
{
    //: Do the regression fitting for a given basis instance
    class lsfit_this_basis
    {
        //: The data matrix of x values (predictor variables)
        // Each training example is a row, each x-variable dimension corresponds to a column
        const vnl_matrix<double>& x_;
        //: Vector of dependent y values
        const vnl_vector<double>& y_;
        //: x'x
        const vnl_matrix<double>& XtX_;
        //: x'y
        const vnl_vector<double>& XtY_;
        //: the basis (note ordered by variable index)
        std::set<unsigned> basis_ ;
        //: number of training examples (i.e. number of rows in x_)
        unsigned num_examples_;
        //: dimensionality - (i.e. number of columns in x_)
        unsigned num_vars_;
        //: The regression coefficients determine for the significant variables in the basis
        // NB These are in the order of the basis variables, e.g. weights_[1] is for the second variable
        // Also note the size of weights is one more than the basis, the last term being the constant
        vnl_vector<double> weights_;
      public:
        //: constructor, note supply the data references
        lsfit_this_basis(const vnl_matrix<double>& x,
                         const vnl_vector<double>& y,
                         const vnl_matrix<double>& XtX,
                         const vnl_vector<double>& XtY):
            x_(x),y_(y),XtX_(XtX),XtY_(XtY)
            {
                num_examples_ = y.size();
                num_vars_ = x.cols();
            }
        //: Set the basis
        void set_basis(std::set<unsigned>& basis) {basis_ = basis;}
        //: return the basis
        const std::set<unsigned>& basis() const {return basis_;}

        //:Try adding variable k to the basis and then fit the extended basis, returning resid sum of squares
        // Note the basis is not actually updated, only temporarily for the duration of this call
        double add(unsigned k);

        //:Try removing variable k from the basis and then fit the extended basis, returning resid sum of squares
        // Note the basis is not actually updated, only temporarily for the duration of this call
        double remove(unsigned k);

        //: Fit the current basis
        double operator()();

        //: return the regression coefficients and constant (final term)
        const vnl_vector<double >& weights() const {return weights_;}
    };
};

#endif // mbl_stepwise_regression_h_
