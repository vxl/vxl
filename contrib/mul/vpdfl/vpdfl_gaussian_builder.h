#ifndef vpdfl_gaussian_builder_h
#define vpdfl_gaussian_builder_h
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \author Tim Cootes
// \date 16-Oct-98
// \brief Builders for Multi-variate gaussian PDF with arbitrary axes.
// \verbatim
//    IMS   Converted to VXL 18 April 2000
// \endverbatim

// Conditional inclusion control


//=======================================================================
// inclusions
#include <vpdfl/vpdfl_builder_base.h>



//=======================================================================

class vpdfl_gaussian;

#include <vnl/vnl_fwd.h>

//: Class to build vpdfl_gaussian objects
class vpdfl_gaussian_builder : public vpdfl_builder_base {
private:
	double min_var_;
	
		
	vpdfl_gaussian& gaussian(vpdfl_pdf_base& model) const;
	
protected:

		//: Add w*v*v.transpose() to S (resizing if necessary)
		// By repeatedly calling this function with a new vector v and weight w
		// You can calculate the Sum of Squares matrix.
	void updateCovar(vnl_matrix<double>& S, const vnl_vector<double>& v, double w) const;

public:

		//: Dflt ctor
	vpdfl_gaussian_builder();

		//: Destructor
	virtual ~vpdfl_gaussian_builder();
	
		//: Create empty model
	virtual vpdfl_pdf_base* new_model() const;
	
		//: Define lower threshold on variance for built models
	virtual void set_min_var(double min_var);

		//: Get lower threshold on variance for built models
	virtual double min_var();
	
		//: Build default model with given mean
	virtual void build(vpdfl_pdf_base& model,
						const vnl_vector<double>& mean);
		
		//: Build model from data
	virtual void build(vpdfl_pdf_base& model,
						mbl_data_wrapper<vnl_vector<double> >& data);
						
		//: Build model from weighted data
	virtual void weighted_build(vpdfl_pdf_base& model,
						mbl_data_wrapper<vnl_vector<double> >& data,
						const vcl_vector<double>& wts);
						
		//: Computes mean and covariance of given data
	void meanCovar(vnl_vector<double>& mean, vnl_matrix<double>& covar,
	               mbl_data_wrapper<vnl_vector<double> >& data);
						
		//: Build model from mean and covariance
	void buildFromCovar(vpdfl_gaussian& model,
						const vnl_vector<double>& mean,
						const vnl_matrix<double>& covar);
						

	/*========= methods which do not change state (const) ==========*/

		//: Version number for I/O 
	short version_no() const;

		//: Name of the class
	virtual vcl_string is_a() const;

		//: Create a copy on the heap and return base class pointer
	virtual	vpdfl_builder_base*	clone()	const;

		//: Print class to os
	virtual void print_summary(vcl_ostream& os) const;
		
		//: Save class to binary file stream
		//!in: bfs: Target binary file stream
	virtual void b_write(vsl_b_ostream& bfs) const;

	/*========== methods which change state (non-const) ============*/

		//: Load class from binary file stream
		//!out: bfs: Target binary file stream
	virtual void b_read(vsl_b_istream& bfs);

private:

		//: To record name of class, returned by is_a() method
	static vcl_string class_name_;

};
#endif //vpdfl_gaussian_builder_h


