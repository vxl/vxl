//:
// \file
// \author Tim Cootes
// \date 21-Jul-2000
// \brief Implementation of Multi-variate principal Component gaussian PDF.
// \verbatim
//    IMS   Converted to VXL 23 April 2000
// \endverbatim

#ifdef __GNUC__
#pragma implementation
#endif

#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vsl/vsl_indent.h>
#include <vpdfl/vpdfl_gaussian_sampler.h>
#include <vpdfl/vpdfl_gaussian.h>
#include "vpdfl_pc_gaussian.h"
#include "vpdfl_pc_gaussian_builder.h"
#include <vnl/vnl_math.h>
#include <mbl/mbl_matxvec.h>





//=======================================================================
// Method: log_p
//=======================================================================

//: Calculate the log probability density at position x.
// You could use vpdfl_gaussian::log_p() which would give the same answer,
// but this method, only rotates into the principal components, not the entire rotated space,
// so saving considerable time.
double vpdfl_pc_gaussian::log_p(const vnl_vector<double>& x)
{
	
	int m = n_principal_components();
	int n = n_dims();
	assert(x.size() == n);


	if (m>=n-1) // it is probably not worth the speed up unless we avoid calculating more than one basis vector.
		return vpdfl_gaussian::log_p(x);

	double mahalDIFS, euclidDFFS;
	get_distances(mahalDIFS, euclidDFFS, x);

	return log_k() - log_k_principal() -euclidDFFS/(2 * (eigenvals()(m+1))) - mahalDIFS;
}

//=======================================================================
// method: getDistances()
//=======================================================================
//: Return Mahalanobis and Euclidean distances from centroid to input.
// Strictly it is the normalised Mahalanobis distance (-log_p()) from the input projected into the 
// principal space to the centroid, and the Euclidean distance from the input
// to the input projected into the principal space.
// Also, the two values are the squares of the distances.
void vpdfl_pc_gaussian::get_distances(double &mahalDIFS, double &euclidDFFS, const vnl_vector<double>& x)
{
	
	int m = n_principal_components();
	int n = n_dims();

	assert(x.size() == n);

	dx_ = x;
	dx_ -= mean();
	
	if (b_.size()!=m) b_.resize(m);
	


	// Rotate dx_ into co-ordinate frame of axes of gaussian
	// b_ = dx_' * P
	// This function will only use the first b_.size() columns of eigenvecs();
	mbl_matxvec_prod_vm(dx_, eigenvecs(), b_);

	const double* b_data = b_.data_block();
	const double* v_data = eigenvals().data_block();
	
	double sum=0.0;
	
	int i=m;
	double db, sumBSq=0.0;
	while (i--)
	{
		db = b_data[i];
		sum+=(db*db)/v_data[i];
		sumBSq+=(db*db);
	}

	mahalDIFS = - log_k_principal() + 0.5*sum;
	i=n;
	const double* dx_data = dx_.data_block() ;
	double ddx, sumDxSq=0.0;
	while (i--)
	{
		ddx = dx_data[i];
		sumDxSq+=(ddx*ddx);
	}

	//By pythagoras sum(squares(b_i)) + sum(squares(c_i)) = sum(squares(b_i)),
	// where b_ is d_ projected into the principal component space
	// and c_ is d_ projected into the complementary component space
	// because b_ and C_ are perpendicular.
	euclidDFFS = sumDxSq - sumBSq;
}


//: Precalculate the constant needed when evaluating Mahalanobis Distance
void vpdfl_pc_gaussian::calcPartLogK()
{
	const double *v_data = eigenvals().data_block();
	double log_v_sum = 0.0;
	const int &n = partition_;

	for (int i=0;i<n;i++)	log_v_sum+=vcl_log(v_data[i]);

  log_k_principal_ = -0.5 * (n*vcl_log(2 * vnl_math::pi) + log_v_sum);
}



//: Initialise safely
// The partition between principal components space and complementary space is
// defined by the length of the Eigenvalues vector (evals.)
// Calculates the variance, and checks that
// the Eigenvalues are ordered and the Eigenvectors are unit normal
// Turn off assertions to remove error checking.
void vpdfl_pc_gaussian::set(const vnl_vector<double>& mean, 
		 const vnl_matrix<double>& evecs,
		 const vnl_vector<double>& evals,
		 double complementEVal)
{
	partition_ = evals.size();
	// The partition from full covariance to spherical must be between 0 and the total number of dimensions
	assert (partition_ <= evecs.cols());
	// The Eigenvector matrix should be square (and full rank but we don;t test for that)
	assert (evecs.cols() == evecs.rows());

	int n = evecs.cols();

	vnl_vector<double> allEVals(n);

	// Fill in the complementary space Eigenvalues
	for (int i = 0; i < partition_; i++)
	{
		allEVals(i) = evals(i);
	}
	for (int i = partition_; i < n; i++)
	{
		allEVals(i) = complementEVal;
	}
	vpdfl_gaussian::set(mean, evecs, allEVals);

	calcPartLogK();
}

//: Initialise safely as you would a vpdfl_gaussian.
// Calculates the variance, and checks that
// the Eigenvalues are ordered and the Eigenvectors are unit normal
// Turn off assertions to remove error checking.
void vpdfl_pc_gaussian::set(const vnl_vector<double>& mean,  const vnl_matrix<double>& evecs, const vnl_vector<double>& evals)
{
	int n_principal_components = vpdfl_pc_gaussian_builder::fixed_partition();
	int n = mean.size();
	vnl_vector<double> principalEVals(n_principal_components);
	
	// Apply threshold to variance
	for (int i=0;i<n_principal_components;++i)
		principalEVals(i)=evals(i);

	double eVsum = 0.0; // The sum of the complementary space eigenvalues.
	for (int i=n_principal_components; i < n; i++)
		eVsum += evals(i);

		// The Eigenvalue of the complementary space basis vectors
	double complementaryEVals = eVsum / (n - n_principal_components);

	set(mean, evecs, principalEVals, complementaryEVals);
}

			


//: Return instance of this PDF
vpdfl_sampler_base* vpdfl_pc_gaussian::sampler() const
{
	vpdfl_gaussian_sampler *i = new vpdfl_gaussian_sampler;
	i->set_model(*this);
	return i;
}





//=======================================================================
// Method: is_a
//=======================================================================

vcl_string  vpdfl_pc_gaussian::is_a() const 
{ 
	return vcl_string("vpdfl_pc_gaussian");
}

//=======================================================================
// Method: version_no
//=======================================================================

short vpdfl_pc_gaussian::version_no() const 
{ 
	return 1; 
}

//=======================================================================
// Method: clone
//=======================================================================

vpdfl_pdf_base* vpdfl_pc_gaussian::clone() const
{
	return new vpdfl_pc_gaussian(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void vpdfl_pc_gaussian::print_summary(vcl_ostream& os) const
{
	vpdfl_gaussian::print_summary(os);
	os << vcl_endl;
	vsl_inc_indent(os);
	os << vsl_indent() << "Partition at: " << partition_ << " Log(k) for principal space: ";
	os << log_k_principal_ << vcl_endl;
	vsl_dec_indent(os);
}

//=======================================================================
// Method: save
//=======================================================================

void vpdfl_pc_gaussian::b_write(vsl_b_ostream& bfs) const
{
	vsl_b_write(bfs,is_a());
	vsl_b_write(bfs,version_no());
	vpdfl_gaussian::b_write(bfs);
	vsl_b_write(bfs,partition_);
	vsl_b_write(bfs,log_k_principal_);
}

//=======================================================================
// Method: load
//=======================================================================

void vpdfl_pc_gaussian::b_read(vsl_b_istream& bfs)
{
	vcl_string name;
	vsl_b_read(bfs,name);
	if (name != is_a())
	{
		vcl_cerr << "vpdfl_pc_gaussian::load() : ";
		vcl_cerr << "Attempted to load object of type ";
		vcl_cerr << name <<" into object of type " << is_a() << vcl_endl;
		vcl_abort();
	}

	short version;
	vsl_b_read(bfs,version);
	switch (version)
	{
		case (1):
			vpdfl_gaussian::b_read(bfs);
			vsl_b_read(bfs,partition_);
			vsl_b_read(bfs,log_k_principal_);
			break;
		default:
			vcl_cerr << "vpdfl_pc_gaussian::b_read() ";
			vcl_cerr << "Unexpected version number " << version << vcl_endl;
			vcl_abort();
	}
}

