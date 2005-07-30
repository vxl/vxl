// This is mul/mbl/mbl_lda.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief  Class to perform linear discriminant analysis
// \author Tim Cootes
//         Converted to VXL by Gavin Wheeler

#include "mbl_lda.h"

#include <vcl_algorithm.h>  // for vcl_find
#include <vcl_cassert.h>
#include <vcl_cstddef.h> // for size_t
#include <vcl_cstring.h> // for memcpy()
#include <vsl/vsl_indent.h>
#include <vsl/vsl_vector_io.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/algo/vnl_generalized_eigensystem.h>
#include <vnl/io/vnl_io_vector.h>
#include <mbl/mbl_matxvec.h>

//=======================================================================

mbl_lda::mbl_lda()
{
}

//=======================================================================

mbl_lda::~mbl_lda()
{
}


//: Classify a new data point
// projects into discriminant space and picks closest mean class vector
int mbl_lda::classify( const vnl_vector<double>& x )
{
  vnl_vector<double> d;
  x_to_d(d, x);
  int nc=n_classes();
  double min_d=1000000;
  int min_i=-1;
  for (int i=0;i<nc; ++i)
  {
    double dist=(d-d_class_mean(i)).squared_magnitude();
    if (dist<min_d ) { min_d= dist; min_i=i; }
  }
  return min_i;
}


//: Comparison
bool mbl_lda::operator==(const mbl_lda& that) const
{
  return mean_ == that.mean_ &&
         d_mean_ == that.d_mean_ &&
         mean_class_mean_ == that.mean_class_mean_ &&
         n_samples_ == that.n_samples_ &&
         withinS_ == that.withinS_ &&
         betweenS_ == that.betweenS_ &&
         basis_ == that.basis_ &&
         evals_ == that.evals_ &&
         d_m_mean_ == that.d_m_mean_;
}

//=======================================================================

void mbl_lda::updateCovar(vnl_matrix<double>& S, const vnl_vector<double>& V)
{
  unsigned int n = V.size();
  if (S.rows()!=n)
  {
    S.set_size(n,n);
    S.fill(0);
  }

  double** s = S.data_array();
  const double* v = V.data_block();
  for (unsigned int i=0;i<n;++i)
  {
    double *row = s[i];
    double vi = v[i];
    for (unsigned int j=0;j<n;++j)
      row[j] += vi*v[j];
  }
}

////////////////////////////////////////////////
// find out how many id in the label vector
int mbl_lda::nDistinctIDs(const int* id, const int n)
{
  vcl_vector<int> dids;
  for (int i=0;i<n;++i)
  {
    if (vcl_find(dids.begin(), dids.end(), id[i])==dids.end())  // if (Index(dids,id[i])<0)
      dids.push_back(id[i]);
  }

  return dids.size();
}

//=======================================================================
//: Perform LDA on data
// \param label  Array [0..n-1] of integers indices
// \param v  Set of vectors [0..n-1]
//
// label[i] gives class of v[i]
// Classes must be labeled from 0 to m-1
void mbl_lda::build(const vnl_vector<double>* v, const int * label, int n,
                    const vnl_matrix<double>& wS, bool compute_wS)
{
  // Find range of class indices and count #valid
  int lo_i=label[0]; // =n causes failure if lo_i is less than n
  int hi_i=-1;
  int n_valid = 0;
  for (int i=0;i<n;++i)
  {
    if (label[i]>=0)
    {
      if (label[i]<lo_i) lo_i=label[i];
      if (label[i]>hi_i) hi_i=label[i];
      n_valid++;
    }
  }

//  assert(lo_i==0);


  // Compute mean of each class
  int n_classes = nDistinctIDs(label,n);
  vcl_cout<<"There are "<<n_classes<<" classes to build LDA space\n"
          <<"Mix label index is "<<hi_i<<vcl_endl
          <<"Min label index is "<<lo_i<<vcl_endl;

  int n_size=hi_i+1;
  mean_.resize(n_size);
  n_samples_.resize(n_size);
  for (int i=0;i<n_size;++i)
    n_samples_[i]=0;

  for (int i=0;i<n;++i)
  {
    int l = label[i];
    if (l<0) continue;
    if (mean_[l].size()==0)
    {
      mean_[l] = v[i];
      n_samples_[l] = 1;
    }
    else
    {
      mean_[l] += v[i];
      n_samples_[l] += 1;
    }
  }

  int n_used_classes = 0;
  for (int i=0;i<n_size;++i)
  {
    if (n_samples_[i]>0)
    {
      mean_[i]/=n_samples_[i];
      if (i==lo_i) mean_class_mean_ = mean_[i];
      else      mean_class_mean_ += mean_[i];
      n_used_classes++;
    }
  }

  mean_class_mean_/=n_used_classes;

  // Build between class covariance
  // Zero to start:
  betweenS_.set_size(0,0);

  for (int i=0;i<n_size;++i)
  {
    if (n_samples_[i]>0)
      updateCovar(betweenS_,mean_[i] - mean_class_mean_);
  }

  betweenS_/=n_used_classes;

  if (compute_wS)
  {
    withinS_.set_size(0,0);
    // Count number of samples used to build matrix
    int n_used=0;
    for (int i=0;i<n;++i)
    {
      int l=label[i];
      if (l>=0 && n_samples_[l]>1)
      {
        updateCovar(withinS_,v[i]-mean_[l]);
        n_used++;
      }
    }
    withinS_/=n_used;
  }
  else
    withinS_ = wS;


#if 0
  vnl_matrix<double> wS_inv;
  //  NR_Inverse(wS_inv,withinS_);
  vnl_svd<double> wS_svd(withinS_, -1.0e-10); // important!!! as the sigma_min=0.0

  wS_inv = wS_svd.inverse();

  //vnl_matrix<double> B=withinS_*wS_inv;
  //vcl_cout<<B<<vcl_endl;

  //vnl_matrix<double> A = betweenS_ * wS_inv;
  vnl_matrix<double> A = wS_inv* betweenS_;

  // Compute eigenvectors and eigenvalues (descending order)
  vnl_matrix<double> EVecs(A.rows(), A.columns());
  vnl_vector<double> evals(A.columns());
  //  NR_CalcSymEigens(A,EVecs,evals,false);

  // **** A not necessarily symmetric!!!! ****
  vnl_symmetric_eigensystem_compute(A, EVecs, evals);
#endif // 0

  vnl_generalized_eigensystem gen_eigs(betweenS_,withinS_);
  vnl_matrix<double> EVecs= gen_eigs.V;
  vnl_vector<double> evals= gen_eigs.D.diagonal();


  //make the eigenvector matrix (columns) and eigenvalue vector in descending order.
  evals.flip();
  EVecs.fliplr();

  // Record n_classes-1 vector basis
  int m = EVecs.rows();

  int t = n_used_classes-1;
  if (t>m) t=m;
  // Copy first t to basis_
  basis_.set_size(m,t);

  double **E = EVecs.data_array();
  double **b = basis_.data_array();
  vcl_size_t bytes_per_row = t * sizeof(double);
  for (int i=0;i<m;++i)
  {
    vcl_memcpy(b[i],E[i],bytes_per_row);
  }

  evals_.set_size(t);
  for (int i=0;i<t;++i)
    evals_[i] = evals[i];

  // Compute projection of mean into d space
  d_m_mean_.set_size(t);
  mbl_matxvec_prod_vm(mean_class_mean_,basis_,d_m_mean_);

  // Project each mean into d-space
  d_mean_.resize(n_size);
  for (int i=0;i<n_size;++i)
    if (n_samples_[i]>0)
      x_to_d(d_mean_[i],mean_[i]);
}

//=======================================================================
//: Perform LDA on data
void mbl_lda::build(const vnl_vector<double>* v, const int* label, int n)
{
  build(v,label,n,vnl_matrix<double>(),true);
}

//=======================================================================
//: Perform LDA on data
void mbl_lda::build(const vnl_vector<double>* v, const vcl_vector<int>& label)
{
  build(v,&label.front(),label.size(),vnl_matrix<double>(),true);
}

//=======================================================================
//: Perform LDA on data
void mbl_lda::build(const vnl_vector<double>* v, const vcl_vector<int>& label,
                    const vnl_matrix<double>& wS)
{
  build(v,&label.front(),label.size(),wS,false);
}

//=======================================================================
//: Perform LDA on data
void mbl_lda::build(const vcl_vector<vnl_vector<double> >& v, const vcl_vector<int>& label)
{
  assert(v.size()==label.size());
  build(&v.front(),&label.front(),label.size(),vnl_matrix<double>(),true);
}

//=======================================================================
//: Perform LDA on data
void mbl_lda::build(const vcl_vector<vnl_vector<double> >& v, const vcl_vector<int>& label,
                    const vnl_matrix<double>& wS)
{
  assert(v.size()==label.size());
  build(&v.front(),&label.front(),label.size(),wS,false);
}

//=======================================================================
//: Perform LDA on data
//  Columns of M form example vectors
//  i'th column belongs to class label[i]
//  Note: label([1..n]) not label([0..n-1])
void mbl_lda::build(const vnl_matrix<double>& M, const vcl_vector<int>& label)
{
  unsigned int n_egs = M.columns();
  assert(n_egs==label.size());
  //  assert(label.lo()==1);
  vcl_vector<vnl_vector<double> > v(n_egs);
  for (unsigned int i=0;i<n_egs;++i)
  {
    v[i] = M.get_column(i);
  }
  build(&v.front(),&label.front(),n_egs,vnl_matrix<double>(),true);
}

//=======================================================================
//: Perform LDA on data
//  Columns of M form example vectors
//  i'th column belongs to class label[i]
//  Note: label([1..n]) not label([0..n-1])
void mbl_lda::build(const vnl_matrix<double>& M, const vcl_vector<int>& label,
                    const vnl_matrix<double>& wS)
{
  unsigned int n_egs = M.columns();
  assert(n_egs==label.size());
  //  assert(label.lo()==1);
  vcl_vector<vnl_vector<double> > v(n_egs);
  for (unsigned int i=0;i<n_egs;++i)
  {
    v[i] = M.get_column(i);
  }
  build(&v.front(),&label.front(),n_egs,wS,false);
}


//=======================================================================
//: Project x into discriminant space
void mbl_lda::x_to_d(vnl_vector<double>& d, const vnl_vector<double>& x) const
{
  d.set_size(d_m_mean_.size());
  mbl_matxvec_prod_vm(x,basis_,d);
  d-=d_m_mean_;
}

//=======================================================================
//: Project d from discriminant space into original space
void mbl_lda::d_to_x(vnl_vector<double>& x, const vnl_vector<double>& d) const
{
  mbl_matxvec_prod_mv(basis_,d,x);
  x+=mean_class_mean_;
}

//=======================================================================

short mbl_lda::version_no() const
{
  return 1;
}

//=======================================================================

vcl_string mbl_lda::is_a() const
{
  return vcl_string("mbl_lda");
}

bool mbl_lda::is_class(vcl_string const& s) const
{
  return s==is_a();
}

//=======================================================================

void mbl_lda::print_summary(vcl_ostream& os) const
{
  int n_classes= n_samples_.size();
  os << "n_classes= "<<n_classes<<vcl_endl;
  for (int i=0; i<n_classes; ++i)
  {
    vcl_cout<<"n_samples_["<<i<<"]= "<<n_samples_[i]<<vcl_endl
            <<"mean_["<<i<<"]= "<<mean_[i]<<vcl_endl
            <<"d_mean_["<<i<<"]= "<<d_mean_[i]<<vcl_endl;
  }

  os << "withinS_= "<<withinS_<<vcl_endl
     << "betweenS_= "<<betweenS_<<vcl_endl
     << "basis_= "<<basis_<<vcl_endl
     << "evals_= "<<evals_<<vcl_endl
     << "d_m_mean_= "<<d_m_mean_<<vcl_endl;
}

//=======================================================================

void mbl_lda::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,mean_);
  vsl_b_write(bfs,d_mean_);
  vsl_b_write(bfs,mean_class_mean_);
  vsl_b_write(bfs,n_samples_);
  vsl_b_write(bfs,withinS_);
  vsl_b_write(bfs,betweenS_);
  vsl_b_write(bfs,basis_);
  vsl_b_write(bfs,evals_);
  vsl_b_write(bfs,d_m_mean_);
}

//=======================================================================

void mbl_lda::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,mean_);
      vsl_b_read(bfs,d_mean_);
      vsl_b_read(bfs,mean_class_mean_);
      vsl_b_read(bfs,n_samples_);
      vsl_b_read(bfs,withinS_);
      vsl_b_read(bfs,betweenS_);
      vsl_b_read(bfs,basis_);
      vsl_b_read(bfs,evals_);
      vsl_b_read(bfs,d_m_mean_);
      break;
    default:
      //CHECK FUNCTION SIGNATURE IS CORRECT
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, mbl_lda &)\n"
               << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mbl_lda& b)
{
  b.b_write(bfs);
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mbl_lda& b)
{
  b.b_read(bfs);
}

//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const mbl_lda& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}
