// This is mul/clsfy/clsfy_adaboost_trainer.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Functions to train classifiers using AdaBoost algorithm
// \author dac
// \date   Fri Mar  1 23:49:39 2002
//  Functions to train classifiers using AdaBoost algorithm
//  AdaBoost combines a set of (usually simple, weak) classifiers into
//  a more powerful single classifier.  Essentially it selects the
//  classifiers one at a time, choosing the best at each step.
//  The classifiers are trained to distinguish the examples mis-classified
//  by the currently selected classifiers.
// \verbatim
// Modifications
// \endverbatim

#include "clsfy_adaboost_trainer.h"

#include <vcl_iostream.h>
#include <vsl/vsl_indent.h>
#include <vcl_cmath.h>
#include <vcl_cassert.h>

//=======================================================================

clsfy_adaboost_trainer::clsfy_adaboost_trainer()
{
}

//=======================================================================

clsfy_adaboost_trainer::~clsfy_adaboost_trainer()
{
}


//: Extracts the j-th element of each vector in data and puts into v
void clsfy_adaboost_trainer::clsfy_get_elements(
                              vnl_vector<double>& v,
                              mbl_data_wrapper<vnl_vector<double> >& data,
                              int j)
{
  unsigned long n = data.size();
  v.set_size(n);
  data.reset();
  for (unsigned long i=0;i<n;++i)
  {
    v[i] = data.current()[j];
    data.next();
  }
}


//: Correctly classified examples have weights scaled by beta
void clsfy_adaboost_trainer::clsfy_update_weights_weak(
                              vnl_vector<double> &wts,
                              const vnl_vector<double>& data,
                              clsfy_classifier_1d& classifier,
                              int class_number,
                              double beta)
{
  assert(class_number >= 0);
  unsigned int n = wts.size();
  for (unsigned int i=0;i<n;++i)
    if (classifier.classify(data[i])==(unsigned)class_number) wts[i]*=beta;
}


//: Build classifier composed of 1d classifiers working on individual vector elements
//  Builds an n-component classifier, each component of which is a 1D classifier
//  working on a single element of the input vector.
//  here egs0 are -ve examples
//  and egs1 are +ve examples
void clsfy_adaboost_trainer::build_strong_classifier(
                              clsfy_simple_adaboost& strong_classifier,
                              int max_n_clfrs,
                              clsfy_builder_1d& builder,
                              mbl_data_wrapper<vnl_vector<double> >& egs0,
                              mbl_data_wrapper<vnl_vector<double> >& egs1)
{
  // remove all alphas and classifiers from strong classifier
  strong_classifier.clear();


  clsfy_classifier_1d* c1d = builder.new_classifier();
  clsfy_classifier_1d* best_c1d= builder.new_classifier();

  unsigned long n0 = egs0.size();
  unsigned long n1 = egs1.size();
  int n=max_n_clfrs;

  // Dimensionality of data
  unsigned int d = egs0.current().size();
  strong_classifier.set_n_dims(d);

  // Initialise the weights on each sample
  vnl_vector<double> wts0(n0,0.5/n0);
  vnl_vector<double> wts1(n1,0.5/n1);

  vnl_vector<double> egs0_1d, egs1_1d;

  for (int i=0;i<n;++i)
  {
    vcl_cout<<"adaboost training round = "<<i<<'\n';

    //vcl_cout<<"wts0= "<<wts0<<"\nwts1= "<<wts1<<'\n';

    int best_j=-1;
    double min_error= 100000;
    for (unsigned int j=0;j<d;++j)
    {
      //vcl_cout<<"building classifier "<<j<<" of "<<d<<'\n';
      clsfy_get_elements(egs0_1d,egs0,j);
      clsfy_get_elements(egs1_1d,egs1,j);

      double error = builder.build(*c1d,egs0_1d,wts0,egs1_1d,wts1);
      //vcl_cout<<"error= "<<error<<'\n';
      if (j==0 || error<min_error)
      {
        min_error = error;
        delete best_c1d;
        best_c1d= c1d->clone();
        best_j = j;
      }
    }

    vcl_cout<<"best_j= "<<best_j<<'\n'
            <<"min_error= "<<min_error<<'\n';

    if (min_error<1e-10)  // Hooray!
    {
      vcl_cout<<"min_error<1e-10 !!!\n";
      double alpha  = vcl_log(2.0*(n0+n1));   //is this appropriate???
      strong_classifier.add_classifier( best_c1d, alpha, best_j);

      // delete classifiers on heap, because clones taken by strong_classifier
      delete c1d;
      delete best_c1d;
      return;
    }


    if (0.5-min_error<1e-10) // Oh dear, no further improvement possible
    {
      vcl_cout<<"min_error => 0.5 !!!\n";

      // delete classifiers on heap, because clones taken by strong_classifier
      delete c1d;
      delete best_c1d;
      return;
    }

    double beta = min_error/(1.0-min_error);
    double alpha  = -1.0*vcl_log(beta);
    strong_classifier.add_classifier( best_c1d, alpha, best_j);

    if (i<(n-1))
    {
      // apply the best weak classifier
      clsfy_get_elements(egs0_1d,egs0,best_j);
      clsfy_get_elements(egs1_1d,egs1,best_j);

      clsfy_update_weights_weak(wts0,egs0_1d,*best_c1d,0,beta);
      clsfy_update_weights_weak(wts1,egs1_1d,*best_c1d,1,beta);

      // normalise the weights
      double w_sum = wts0.mean()*n0 + wts1.mean()*n1;
      wts0/=w_sum;
      wts1/=w_sum;
    }
  }

  delete c1d;
  delete best_c1d;
}

//=======================================================================

short clsfy_adaboost_trainer::version_no() const
{
  return 1;
}

//=======================================================================

vcl_string clsfy_adaboost_trainer::is_a() const
{
  return vcl_string("clsfy_adaboost_trainer");
}

bool clsfy_adaboost_trainer::is_class(vcl_string const& s) const
{
  return s == clsfy_adaboost_trainer::is_a();
}

//=======================================================================

#if 0

// required if data stored on the heap is present in this class
clsfy_adaboost_trainer::clsfy_adaboost_trainer(const clsfy_adaboost_trainer& new_b):
  data_ptr_(0)
{
  *this = new_b;
}

//=======================================================================

// required if data stored on the heap is present in this class
clsfy_adaboost_trainer& clsfy_adaboost_trainer::operator=(const clsfy_adaboost_trainer& new_b)
{
  if (&new_b==this) return *this;

  // Copy heap member variables.
  delete data_ptr_; data_ptr_=0;

  if (new_b.data_ptr_)
    data_ptr_ = new_b.data_ptr_->clone();

  // Copy normal member variables
  data_ = new_b.data_;

  return *this;
}

#endif // 0

//=======================================================================

// required if data is present in this class
void clsfy_adaboost_trainer::print_summary(vcl_ostream& /*os*/) const
{
    // os << data_; // example of data output
    vcl_cerr << "clsfy_adaboost_trainer::print_summary() NYI\n";
}

//=======================================================================

// required if data is present in this class
void clsfy_adaboost_trainer::b_write(vsl_b_ostream& /*bfs*/) const
{
  //vsl_b_write(bfs, version_no());
  //vsl_b_write(bfs, data_);
  vcl_cerr << "clsfy_adaboost_trainer::b_write() NYI\n";
}

//=======================================================================

// required if data is present in this class
void clsfy_adaboost_trainer::b_read(vsl_b_istream& /*bfs*/)
{
  vcl_cerr << "clsfy_adaboost_trainer::b_read() NYI\n";
#if 0
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  case (1):
    vsl_b_read(bfs,data_);
    break;
  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, clsfy_adaboost_trainer&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
#endif
}

//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const clsfy_adaboost_trainer& b)
{
  b.b_write(bfs);
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, clsfy_adaboost_trainer& b)
{
  b.b_read(bfs);
}

//=======================================================================

void vsl_print_summary(vcl_ostream& os,const clsfy_adaboost_trainer& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
}

//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const clsfy_adaboost_trainer& b)
{
  vsl_print_summary(os,b);
  return os;
}
