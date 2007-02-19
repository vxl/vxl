#ifndef mcal_pca_h
#define mcal_pca_h

//:
// \file
// \author Tim Cootes
// \brief Class to perform Principle Component Analysis

#include <mcal/mcal_component_analyzer.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>
#include <mbl/mbl_data_wrapper.h>

//: Class to perform Principle Component Analysis
//  Applies a PCA to compute mean, variance and eigenvectors/values
//  of covariance matrix of supplied data.
class mcal_pca : public mcal_component_analyzer {
private:
    //: Define how many modes to use
  double var_prop_;
  int min_modes_,max_modes_;

  //: Return the number of modes to retain
  unsigned choose_n_modes(const vnl_vector<double>& evals);

  //: Utility function
  void fillDDt(vnl_matrix<double>& DDt, const vnl_matrix<double>& A, 
               int rlo, int rhi, int clo, int chi);

  //: Compute eigenvectors assuming fewer dimensions than samples
  void build_evecs_nd_smaller(mbl_data_wrapper<vnl_vector<double> >& data,
                                const vnl_vector<double>& mean,
                                vnl_matrix<double>& evecs,
                                vnl_vector<double>& evals);

  //: Compute eigenvectors assuming fewer samples than dimensions
  void build_evecs_ns_smaller(mbl_data_wrapper<vnl_vector<double> >& data,
                                const vnl_vector<double>& mean,
                                vnl_matrix<double>& evecs,
                                vnl_vector<double>& evals);

  //: Max number of doubles allowed in memory
  double max_d_in_memory_;

  //: Whether to use chunks if required
  bool use_chunks_;
    
protected:
public:

    //: Dflt ctor
  mcal_pca();

    //: Destructor
  virtual ~mcal_pca();

    //: Define limits on number of parameters to use in model
    // \param var_proportion  Proportion of variance in data to explain
  void set_mode_choice(unsigned min, unsigned max, double var_proportion);
  

  //: Set the choice for the minimum number of model
  void set_min_modes( unsigned min );
  //: Current lower limit on number of parameters
  unsigned min_modes() const;

    //: Define upper limit on number of parameters
  void set_max_modes(unsigned max);
    //: Current upper limit on number of parameters
  unsigned max_modes() const;
  
    //: Define proportion of data variance to explain
  virtual void set_var_prop(double v);
  
    //: Current proportion of data variance to explain
  virtual double var_prop() const;

  //: Max number of doubles allowed in memory
  void set_max_d_in_memory(double max_n);

  //: Max number of doubles allowed in memory
  double max_d_in_memory() const { return max_d_in_memory_; }

  //: Set whether we may build in chunks if required
  void set_use_chunks(bool chunks);

  //: Indicate whether to use chunks if required
  bool use_chunks() const { return use_chunks_; }

  //: Compute modes of the supplied data relative to the supplied mean
  //  Model is x = mean + modes*b,  where b is a vector of weights on each mode.
  //  mode_var[i] gives the variance of the data projected onto that mode.
  virtual void build_about_mean(mbl_data_wrapper<vnl_vector<double> >& data,
                                const vnl_vector<double>& mean,
                                vnl_matrix<double>& modes,
                                vnl_vector<double>& mode_var);

    //: Version number for I/O 
  short version_no() const;

    //: Name of the class
  virtual vcl_string is_a() const;

    //: Create a copy on the heap and return base class pointer
  virtual  mcal_component_analyzer*  clone()  const;

    //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;
    
    //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

    //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);

  //: Read initialisation settings from a stream.
  // Parameters:
  // \verbatim
  // {
  //   min_modes: 0 max_modes: 99 var_prop: 0.99
  //   // Maximum number of doubles to store in memory at once
  //   max_d_in_memory: 1e8
  //   // Indicate how to build from large amounts of data
  //   use_chunks: false
  // }
  // \endverbatim
  // \throw mbl_exception_parse_error if the parse fails.
  virtual void config_from_stream(vcl_istream & is);

};

#endif // mcal_pca_h



