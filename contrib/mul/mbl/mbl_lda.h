// This is mul/mbl/mbl_lda.h
#ifndef mbl_lda_h_
#define mbl_lda_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief  Class to perform linear discriminant analysis
// \author Tim Cootes
//         Converted to VXL by Gavin Wheeler

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iosfwd.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/io/vnl_io_matrix.h>

//=======================================================================
//: Class to perform linear discriminant analysis
class mbl_lda
{
 private:

  vcl_vector<vnl_vector<double> > mean_;
  vcl_vector<vnl_vector<double> > d_mean_;
  vnl_vector<double> mean_class_mean_;
  vcl_vector<int> n_samples_;
  vnl_matrix<double> withinS_;
  vnl_matrix<double> betweenS_;
  vnl_matrix<double> basis_;
  vnl_vector<double> evals_;
  vnl_vector<double> d_m_mean_;

  void updateCovar(vnl_matrix<double>& S, const vnl_vector<double>& v);

  //: Perform LDA on data
  // Classes must be labeled from 0..n-1
  // \param label  Array [0..n-1] of integer indices. label[i] gives class of v[i]
  // \param n      Size of label and of v
  // \param v  Set of vectors [0..n-1]
  // \param wS  Within class covariance to use if compute_wS false
  // \param compute_wS  This boolean parameter determines whether to use wS
  void build(const vnl_vector<double>* v, const int* label, int n,
             const vnl_matrix<double>& wS, bool compute_wS);

 public:

  //: Dflt ctor
  mbl_lda();

  //: Destructor
  virtual ~mbl_lda();

   //: Comparison
  bool operator==
    (const mbl_lda& that) const;

  //: Classify a new data point
  // projects into discriminant space and picks closest mean class vector
  int classify( const vnl_vector<double>& x );

  //: Perform LDA on data
  // \param n  Number of examples
  // \param label  integer indices
  // \param v  Set of vectors [0..n-1]
  //
  // - label[i] gives class of v[i]
  // - If label[i]<0 the class is assumed to be unknown
  //   and example i is ignored
  // - Classes must be labeled from 0..n-1
  void build(const vnl_vector<double>* v, const int* label, int n);

  //: Perform LDA on data
  // \param label  Array [0..n-1] of integers indices
  // \param v  Set of vectors [0..n-1]
  //
  // - label[i] gives class of v[i]
  // - If label[i]<0 the class is assumed to be unknown
  //   and example i is ignored
  // - Classes must be labeled from 0..n-1
  void build(const vnl_vector<double>* v, const vcl_vector<int>& label);

  //: Perform LDA on data
  // \param label  Array [0..n-1] of integers indices
  // \param v  Set of vectors [0..n-1]
  // \param wS  Within class covariance to use
  //
  // - label[i] gives class of v[i]
  // - If label[i]<0 the class is assumed to be unknown
  //   and example i is ignored
  // - Classes must be labeled from 0..n-1
  void build(const vnl_vector<double>* v, const vcl_vector<int>& label,
             const vnl_matrix<double>& wS);

  //: Perform LDA on data
  // \param label  Array [0..n-1] of integers indices
  // \param v  Set of vectors [0..n-1]
  //
  // - label[i] gives class of v[i]
  // - If label[i]<0 the class is assumed to be unknown
  //   and example i is ignored
  // - Classes must be labeled from 0..n-1
  void build(const vcl_vector<vnl_vector<double> >& v, const vcl_vector<int>& label);

  //: Perform LDA on data
  // \param label  Array [0..n-1] of integers indices
  // \param v  Set of vectors [0..n-1]
  // \param wS  Within class covariance to use
  //
  // - label[i] gives class of v[i]
  // - Classes must be labeled from 0..n-1
  // - If label[i]<0 the class is assumed to be unknown and example i is ignored
  void build(const vcl_vector<vnl_vector<double> >& v, const vcl_vector<int>& label,
             const vnl_matrix<double>& wS);

  //: Perform LDA on data
  // - Columns of M form example vectors
  // - i'th column belongs to class label(i)
  // - Note: label([1..n]) not label([0..n-1])
  // - If label[i]<0 the class is assumed to be unknown
  //   and example i is ignored
  // - Note also that this is inefficient - it converts the
  //   matrix to an array and calls build(v,label)
  void build(const vnl_matrix<double>& M, const vcl_vector<int>& label);

  //: Perform LDA on data
  // - Columns of M form example vectors
  // - i'th column belongs to class label(i)
  // - Note: label([1..n]) not label([0..n-1])
  // - Note also that this is inefficient - it converts the
  //   matrix to an array and calls build(v,label)
  // - If label[i]<0 the class is assumed to be unknown
  //   and example i is ignored
  // \param M     The columns of this matrix for the example vectors
  // \param label The vector of class labels corresponding to these examples
  // \param wS    Within class covariance to use
  void build(const vnl_matrix<double>& M, const vcl_vector<int>& label,
             const vnl_matrix<double>& wS);

  //: Number of classes
  int n_classes() const { return mean_.size(); }

  //: Number of examples of each class
  int n_samples(int i) const { return n_samples_[i]; }

  //: Mean vector for i'th class in original space
  const vnl_vector<double>& class_mean(int i) const { return mean_[i]; }

  //: Mean vector for i'th class in discriminant space
  const vnl_vector<double>& d_class_mean(int i) const { return d_mean_[i]; }

  //: Mean of means for each class
  const vnl_vector<double>& mean_class_mean() const { return mean_class_mean_; }

  //: Within class covariance matrix
  const vnl_matrix<double>& within_covar() const { return withinS_; }

  //: Between class covariance matrix
  const vnl_matrix<double>& between_covar() const { return betweenS_; }

  //: Basis for discriminant space
  const vnl_matrix<double>& basis() const { return basis_; }

  //: Eigenvalues associated with each basis vector
  const vnl_vector<double>& basis_e_vals() const { return evals_; }

  //: Project x into discriminant space
  void x_to_d(vnl_vector<double>& d, const vnl_vector<double>& x) const;

  //: Project d from discriminant space into original space
  void d_to_x(vnl_vector<double>& x, const vnl_vector<double>& d) const;

  //: find out how many id in the label vector
  int nDistinctIDs(const int* id, const int n);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: True if this is (or is derived from) class named s
  virtual bool is_class(vcl_string const& s) const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};


//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mbl_lda& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mbl_lda& b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const mbl_lda& b);

#endif // mbl_lda_h_
