#ifndef vpdfl_pc_gaussian_builder_h
#define vpdfl_pc_gaussian_builder_h
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \author Ian Scott
// \date 21-Jul-2000
// \brief Interface for Multi-variate Principle Component gaussian PDF Builder.
// Modifications
// 23 April 2001 IMS - Ported to VXL


//=======================================================================
// inclusions
#include <vpdfl/vpdfl_gaussian_builder.h>


//=======================================================================

class vpdfl_gaussian;
class vpdfl_pdf_base;
class vpdfl_pc_gaussian;

//: Class to build vpdfl_pc_gaussian objects with a fixed number of principle components
class vpdfl_pc_gaussian_builder : public vpdfl_gaussian_builder
{
public:
  enum partitionMethods { fixed, proportionate };
private:

  vpdfl_pc_gaussian& gaussian(vpdfl_pdf_base& model) const;

    //: The method used to decide how to calculate the number of principle components.
    // defaults to fixed.
  partitionMethods partitionMethod_;

    //: The proportion of variance that should be encoded with the principle components..
    // Isn't used by default..
  double proportionOfVariance_;
protected:

public:

    //: Dflt ctor
  vpdfl_pc_gaussian_builder();

    //: Destructor
  virtual ~vpdfl_pc_gaussian_builder();

    //: Create empty model
  virtual vpdfl_pdf_base* new_model() const;

    //: Build default model with given mean
  virtual void build(vpdfl_pdf_base& model,
                     const vnl_vector<double>& mean) const;

    //: Build model from data
  virtual void build(vpdfl_pdf_base& model,
                     mbl_data_wrapper<vnl_vector<double> >& data) const;

    //: Build model from weighted data
  virtual void weighted_build(vpdfl_pdf_base& model,
                              mbl_data_wrapper<vnl_vector<double> >& data,
                              const vcl_vector<double>& wts) const;

    //: Computes mean and covariance of given data
  void mean_covar(vnl_vector<double>& mean, vnl_matrix<double>& covar,
                  mbl_data_wrapper<vnl_vector<double> >& data) const;


    //: Decide where to partition an Eigenvector space
    // Returns the number of principle components to be used.
    // Pass in the Eigenvlaues (eVals), the number of samples
    // that went to make up this Gaussian (nSamples), and the noise floor
    // for the dataset. The method may use simplified algorithms if
    // you indicate that the number of samples or noise floor is unknown
    // (by setting the latter parameters to 0.)
  virtual unsigned decide_partition(const vnl_vector<double>& eVals,
                                    unsigned nSamples=0, double noise=0.0) const;

    //: Return the number of principle compoents when using fixed partition.
    // This method is static so that it can be uased as a default in vpdfl_pc_gaussian.
  static unsigned fixed_partition() {return 75;}

    //: Use proportion of variance to decide on the number of principle components.
    // Specify the proportion (between 0 and 1).
    // The default setting uses a fixed number of principle components.
  void set_proportion_partition( double proportion);

    //: Find the proportion of variance to decide on the number of principle components.
    // returns a negative value if not using proportion of variance method.
  double proportionPartition() const
  {
    if (partitionMethod_ == proportionate) return proportionOfVariance_;
    else return -1.0;
  }

    //: Use a fixed number of principle components for building.
    // This is the default setting.
  void set_fixed_parition()
  {
    partitionMethod_ = fixed;
  }



    //: Version number for I/O
  short version_no() const;

    //: Name of the class
  virtual vcl_string is_a() const;

    //: Create a copy on the heap and return base class pointer
  virtual vpdfl_builder_base* clone() const;

    //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

    //: Save class to binary file stream
    //!in: bfs: Target binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;


    //: Load class from binary file stream
    //!out: bfs: Target binary file stream
  virtual void b_read(vsl_b_istream& bfs);

private:

    //: To record name of class, returned by is_a() method
  static vcl_string class_name_;
};

#endif
