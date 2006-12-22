// This is mul/clsfy/clsfy_binary_hyperplane_gmrho_builder.cxx
#include "clsfy_binary_hyperplane_gmrho_builder.h"
//:
// \file
// \brief Implement a two-class output linear classifier builder using a Geman-McClure robust error function
// \author Martin Roberts
// \date 4 Nov 2006

//=======================================================================

#include <vcl_string.h>
#include <vcl_iterator.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_numeric.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector_ref.h>
#include <vnl/algo/vnl_lbfgs.h>


//: Some helper stuff, like the error function to be minimised
namespace clsfy_binary_hyperplane_gmrho_builder_helpers
{
    //: The cost function, sum Geman-McClure error functions over all training examples
    class gmrho_sum : public vnl_cost_function
    {
        //: Reference to data matrix, one row per training example
        const vnl_matrix<double>& x_;
        //: Reference to required outputs
        const vnl_vector<double>& y_;
        //: Scale factor used in Geman-McClure error function
        double sigma_;
        //: sigma squared
        double var_;
        //: Number of training examples (x_.rows())
        unsigned num_examples_;
        //: Number of dimensions (x_.cols())
        unsigned num_vars_;
        //: var_/(1+var_)^2 - ensures continuity of derivative at hyperplane boundary
        double alpha_;
        //: 1/(1+var_)^2 - with alpha, ensures continuity of function at hyperplane boundary
        double beta_;
      public:
        //: construct passing in reference to data matrix
        gmrho_sum(const vnl_matrix<double>& x,
                  const vnl_vector<double>& y,double sigma=1);

        //: reset the scaling factor
        void set_sigma(double sigma);

        //:  The main function.  Given the vector of weights parameters vector , compute the value of f(x).
        virtual double f(vnl_vector<double> const& w);

        //:  Calculate the gradient of f at parameter vector x.
        virtual void gradf(vnl_vector<double> const& x, vnl_vector<double>& gradient);
    };

    //: functor to accumulate gradient contributions for given training example
    class gm_grad_accum
    {
        const double* px_;
        const double wt_;
      public:
        gm_grad_accum(const double* px,double wt) : px_(px),wt_(wt) {}
        void operator()(double& grad)
        {
            grad += (*px_++) * wt_;
        }
    };

    //: Given the class category variable, return the associated regression value (e.g. 1 for class 1, -1 for class 0)
    class category_value
    {
        const double y0;
        const double y1;
      public:
        category_value(unsigned num_category1,unsigned num_total):
            y0(-1.0*double(num_total-num_category1)/double(num_total)),
            y1(double(num_category1)/double(num_total)) {}

        double operator()(const unsigned& classNum)
        {
            //return classNum ? y1 : y0;
            return classNum ? 1.0 : -1.0;
        }
    };
};

//-----------------------------------------------------------------------------------------------
//------------------------ The builder member functions ------------------------------------------
//------------------------------------------------------------------------------------------------
//: Build a linear classifier, with the given data.
// Return the mean error over the training set.
// n_classes must be 1.
double clsfy_binary_hyperplane_gmrho_builder::build(clsfy_classifier_base& classifier,
                                                    mbl_data_wrapper<vnl_vector<double> >& inputs,
                                                    unsigned n_classes,
                                                    const vcl_vector<unsigned>& outputs) const
{
    assert (n_classes == 1);
    return clsfy_binary_hyperplane_gmrho_builder::build(classifier, inputs, outputs);
}

//: Build a linear hyperplane classifier with the given data.
// Reduce the influence of well classified points far into their correct region by
// applying a Geman-McClure robust error function, rather than a least squares fit
double clsfy_binary_hyperplane_gmrho_builder::build(clsfy_classifier_base& classifier,
                                                    mbl_data_wrapper<vnl_vector<double> >& inputs,
                                                    const vcl_vector<unsigned>& outputs) const
{
    using clsfy_binary_hyperplane_gmrho_builder_helpers::category_value;

    //First let the base class get us a starting solution
    clsfy_binary_hyperplane_ls_builder::build( classifier,inputs,outputs);
    //Extract the data into a matrix
    num_examples_ = inputs.size();
    if (num_examples_ == 0)
    {
        vcl_cerr<<"WARNING - clsfy_binary_hyperplane_gmrho_builder::build called with no data\n";
        return 0.0;
    }

    //Now copy from the urggghh data wrapper into a sensible data structure (matrix!)
    inputs.reset();
    num_vars_ = inputs.current().size();
    vnl_matrix<double> data(num_examples_,num_vars_,0.0);
    unsigned i=0;
    do
    {
        double* row=data[i++];
        vcl_copy(inputs.current().begin(),inputs.current().end(),row);
    } while (inputs.next());

    //Set up category regression values determined by output class
    vnl_vector<double> y(num_examples_,0.0);
    vcl_transform(outputs.begin(),outputs.end(),
                  y.begin(),
                  category_value(vcl_count(outputs.begin(),outputs.end(),1u),outputs.size()));
    weights_.set_size(num_vars_+1);

    //Initialise the weights using the standard least squares fit of my base class
    clsfy_binary_hyperplane& hyperplane = dynamic_cast<clsfy_binary_hyperplane &>(classifier);

    weights_.update(hyperplane.weights(),0);
    weights_[num_vars_] = hyperplane.bias();

    //Estimate the scaling factor used in the Geman-McClure function
    double sigma_scale_target = sigma_preset_;
    if (auto_estimate_sigma_)
        sigma_scale_target=estimate_sigma(data,y);

    //To avoid local minima perform deterministic annealing starting from a large initial sigma
    //Set initial kappa so that everything is an inlier
    double kappa = 5.0;
    const double alpha_anneal=0.75;
    //Num of iterations to reduce back to 10% on top of required sigma
    int N = 1+int(vcl_log(1.1/kappa)/vcl_log(alpha_anneal));
    if (N<1) N=1;
    double sigma_scale = kappa * sigma_scale_target;

    epsilon_ = 1.0E-4; //slacken off convergence tolerance during annealing
    for (int ianneal=0;ianneal<N;++ianneal)
    {
        //Then do it at this sigma
        determine_weights(data,y,sigma_scale);
        //and then reduce sigma
        sigma_scale *= alpha_anneal;
    }

    epsilon_ = 1.0E-8; //re-impose a more precise convergence criterion
    //Then re-estimate sigma scale and do a final pair of iterations
    //as sigma depends on the mis-classification overlap depth


    for (unsigned iter=0; iter<(auto_estimate_sigma_ ? 2 : 1); ++iter)
    {
        if (auto_estimate_sigma_)
            sigma_scale_target=estimate_sigma(data,y);
        else
            sigma_scale_target = sigma_preset_;
        //Finally do it at exactly the target sigma
        determine_weights(data,y,sigma_scale_target);
    }
    //And finally copy the parameters into the hyperplane
    vnl_vector_ref<double > weights(num_vars_,weights_.data_block());
    hyperplane.set(weights, weights_[num_vars_]);

    return clsfy_test_error(classifier, inputs, outputs);
}

void clsfy_binary_hyperplane_gmrho_builder::determine_weights(const vnl_matrix<double>& data,
                                                              const vnl_vector<double >& y,
                                                              double sigma) const
{
    //Optimise the weights to fit the data to y

    clsfy_binary_hyperplane_gmrho_builder_helpers::gmrho_sum costFn(data,y,sigma);

    //minimise using the quasi-Newton lbfgs method
    vnl_lbfgs cgMinimiser(costFn);

    cgMinimiser.set_f_tolerance(epsilon_);
    cgMinimiser.set_x_tolerance(epsilon_);

    cgMinimiser.minimize(weights_);
}

double clsfy_binary_hyperplane_gmrho_builder::estimate_sigma(const vnl_matrix<double>& data,
                                                             const vnl_vector<double >& y) const
{
    //Sigma is set to root(3) * (1+d), where d is the median distance past zero
    //of the misclassified values
    //The root(3) is because GM function reduces influence after sigma/sqrt(3)

    vcl_vector<double > falsePosScores;
    vcl_vector<double > falseNegScores;

    double b=weights_[num_vars_]; //constant stored as final variable
    for (unsigned i=0; i<num_examples_;++i) //Loop over examples (matrix rows)
    {
        const double* px=data[i];
        double yval = y[i];
        double ypred = vcl_inner_product(px,px+num_vars_,weights_.begin(),0.0) - b ;
        double e = yval - ypred;
        if (yval>0.0)
        {
            if (ypred<0.0) // mis-classified false negative
            {
                falseNegScores.push_back(vcl_fabs(ypred));
            }
        }
        else
        {
            if (ypred>0.0)//mis-classified false negative
            {
                falsePosScores.push_back(vcl_fabs(ypred));
            }
        }
    }
    double sigma=1.0;
    double delta0=0.0;
    if (!falsePosScores.empty())
    {
        vcl_vector<double >::iterator medianIter=falsePosScores.begin() + falsePosScores.size()/2;
        vcl_nth_element(falsePosScores.begin(),medianIter,falsePosScores.end());
        delta0 = (*medianIter);
    }
    double delta1=0.0;
    if (!falseNegScores.empty())
    {
        vcl_vector<double >::iterator medianIter=falseNegScores.begin() + falseNegScores.size()/2;
        vcl_nth_element(falseNegScores.begin(),medianIter,falseNegScores.end());
        delta1 = (*medianIter);
    }
    sigma += vcl_max(delta0,delta1);

    sigma *= vcl_sqrt(3.0);
    return sigma;
}

//=======================================================================

void clsfy_binary_hyperplane_gmrho_builder::b_write(vsl_b_ostream &bfs) const
{
    const int version_no=1;
    vsl_b_write(bfs, version_no);
    clsfy_binary_hyperplane_ls_builder::b_write(bfs);
}

//=======================================================================

void clsfy_binary_hyperplane_gmrho_builder::b_read(vsl_b_istream &bfs)
{
    if (!bfs) return;

    short version;
    vsl_b_read(bfs,version);
    switch (version)
    {
        case (1):
            clsfy_binary_hyperplane_ls_builder::b_read(bfs);
            break;
        default:
            vcl_cerr << "I/O ERROR: clsfy_binary_hyperplane_gmrho_builder::b_read(vsl_b_istream&)\n"
                     << "           Unknown version number "<< version << '\n';
            bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    }
}

//=======================================================================

vcl_string clsfy_binary_hyperplane_gmrho_builder::is_a() const
{
    return vcl_string("clsfy_binary_hyperplane_gmrho_builder");
}

//=======================================================================

bool clsfy_binary_hyperplane_gmrho_builder::is_class(vcl_string const& s) const
{
    return s == clsfy_binary_hyperplane_gmrho_builder::is_a() || clsfy_binary_hyperplane_ls_builder::is_class(s);
}

//=======================================================================

short clsfy_binary_hyperplane_gmrho_builder::version_no() const
{
    return 1;
}

//=======================================================================

void clsfy_binary_hyperplane_gmrho_builder::print_summary(vcl_ostream& os) const
{
    os << is_a();
}

//=======================================================================
clsfy_builder_base* clsfy_binary_hyperplane_gmrho_builder::clone() const
{
    return new clsfy_binary_hyperplane_gmrho_builder(*this);
}

//---------------------------------------------------------------------------------------------
//: The error function class
//  This returns a geman-mcclure robust function if the point is correctly classified
// Otherwise the squared error is returned, with coefficient and offset to ensure continuity
// and smoothness at the join
//---------------------------------------------------------------------------------------------
clsfy_binary_hyperplane_gmrho_builder_helpers::gmrho_sum::gmrho_sum(const vnl_matrix<double>& x,
                                                                    const vnl_vector<double>& y,
                                                                    double sigma):
        vnl_cost_function(x.cols()+1),
        x_(x),y_(y),sigma_(1.0),var_(1.0),num_examples_(x.rows()),num_vars_(x.cols())
{
    set_sigma(sigma);
}

void clsfy_binary_hyperplane_gmrho_builder_helpers::gmrho_sum::set_sigma(double sigma)
{
    sigma_ = sigma;
    var_ = sigma*sigma;
    double s=1.0+var_;
    s = s*s;
    alpha_ = var_/s;
    beta_ = 1.0/s;
}


//: Return the error sum function
double clsfy_binary_hyperplane_gmrho_builder_helpers::gmrho_sum::f(vnl_vector<double> const& w)
{
    //Sum the error contributions from each example
    double sum=0.0;
    double b=w[num_vars_]; //constant stored as final variable
    for (unsigned i=0; i<num_examples_;++i) //Loop over examples (matrix rows)
    {
        const double* px=x_[i];
        double pred = vcl_inner_product(px,px+num_vars_,w.begin(),0.0) - b;
        double e =  y_[i] - pred;
        double e2 = e*e;
        if ( ((y_[i] > 0.0) && (e <= 1.0)) ||
             ((y_[i] < 0.0) && (e >= -1.0)) )
        {
            //In the correctly classified region
            //So use Geman-McClure function
            sum += e2/(e2+var_);
        }
        else
        {
            //Misclassified, so keep as quadratic (influence increases with error)
            //NB alpha and beta are chosen for continuity of function and gradient at boundary
            sum += alpha_*e2 + beta_;
        }
    }
    return sum;
}

//: Calculate gradient of the error sum function
void clsfy_binary_hyperplane_gmrho_builder_helpers::gmrho_sum::gradf(vnl_vector<double> const& w,
                                                                     vnl_vector<double>& gradient)
{
    using clsfy_binary_hyperplane_gmrho_builder_helpers::gm_grad_accum;
    double sum=0.0;
    double b=w[num_vars_]; //constant stored as final variable
    gradient.fill(0.0);

    for (unsigned i=0; i<num_examples_;++i) //Loop over examples (matrix rows)
    {
        const double* px=x_[i];
        double pred = vcl_inner_product(px,px+num_vars_,w.begin(),0.0) - b;

        double e =  y_[i] - pred;
        double e2 = e*e;
        double wt=1.0;
        if ( ((y_[i] > 0.0) && (e <= 1.0)) ||
             ((y_[i] < 0.0) && (e >= -1.0)) )
        {
            wt = e2 + var_;
        }
        else
        {
            //Freeze weight decay once in misclassification region
            wt = 1.0 + var_;
        }

        double wtInv = -e/(wt*wt);
        vcl_for_each(gradient.begin(),gradient.begin()+num_vars_,
                     gm_grad_accum(px,wtInv));

        gradient[num_vars_] += (-wtInv); //dg/db, last term is for constant
    }
    //And multiply everything by 2sigma^2
    vcl_transform(gradient.begin(),gradient.end(),gradient.begin(),
                  vcl_bind2nd(vcl_multiplies<double>(),2.0*var_));
}
