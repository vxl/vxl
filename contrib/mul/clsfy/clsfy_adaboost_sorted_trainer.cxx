#ifdef __GNUC__
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


#include <clsfy/clsfy_adaboost_sorted_trainer.h>
#include <vcl_iostream.h>
#include <vsl/vsl_indent.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>

//=======================================================================

clsfy_adaboost_sorted_trainer::clsfy_adaboost_sorted_trainer()
{
}

//=======================================================================

clsfy_adaboost_sorted_trainer::~clsfy_adaboost_sorted_trainer()
{
}


//: Build classifier composed of 1d classifiers working on individual vector elements
//  Builds an n-component classifier, each component of which is a 1D classifier
//  working on a single element of the input vector.
//  here egs0 are -ve examples
//  and egs1 are +ve examples
void clsfy_adaboost_sorted_trainer::build_strong_classifier(
                            clsfy_simple_adaboost& strong_classifier,
                            int max_n_clfrs,
                            clsfy_binary_threshold_1d_sorted_builder& builder,
                            mbl_data_wrapper<vnl_vector<double> >& egs0,
                            mbl_data_wrapper<vnl_vector<double> >& egs1)
{
  // first arrange the data in the form
  // vcl_vector< < vcl_vector< vtl_triple<double,int,int> > > > data
  // + vnl_vector wts
  // then sort all data once, then build the classifier

  // number of examples
  unsigned n0 = egs0.size();
  unsigned n1 = egs1.size();
  int n=n0+n1;
  //vcl_cout<<"n= "<<n<<vcl_endl;

  // initialize weights
  vnl_vector<double> wts(n);
  for (int j=0; j<n0; ++j)
    wts(j)=0.5/n0;
  for (int j=0; j<n1; ++j)
    wts(j+n0)=0.5/n1;

  //vcl_cout<<"wts= "<<wts<<vcl_endl;


  // Dimensionality of data
  egs0.reset();
  int d = egs0.current().size();
  vcl_vector< vcl_vector<vbl_triple<double,int,int> > > data(d);

  vbl_triple<double,int,int> t;
  for (int i=0; i<d; ++i)
  {
    data[i].resize(0);
    egs0.reset();
    // add data for class 0
    for (int j=0;j<n0;++j)
    {
      t.first=egs0.current()[i];
      t.second=0;
      t.third = j;
      data[i].push_back(t);
      egs0.next();
    }

    // add data for class 1
    egs1.reset();
    for (int j=0;j<n1;++j)
    {
      t.first=egs1.current()[i];
      t.second=1;
      t.third = j+n0;
      data[i].push_back(t);
      egs1.next();
    }

    // sort data for each individual classifier
    vbl_triple<double,int,int> *data_ptr=&data[i][0];
    vcl_sort(data_ptr,data_ptr+n);


#if 0
    // print out the sorted data
    for (int k=0; k<data[i].size();++k)
    {
      vcl_cout<<"data["<<i<<"]["<<k<<"].first= "<<data[i][k].first<<vcl_endl;
      vcl_cout<<"data["<<i<<"]["<<k<<"].second= "<<data[i][k].second<<vcl_endl;
      vcl_cout<<"data["<<i<<"]["<<k<<"].third= "<<data[i][k].third<<vcl_endl;
    }
#endif
  }


  // remove all alphas and classifiers from strong classifier
  strong_classifier.clear();
  strong_classifier.set_n_dims(d);
  clsfy_classifier_1d* c1d = builder.new_classifier();
  clsfy_classifier_1d* best_c1d= builder.new_classifier();


  vnl_vector<double> best_params;
  double beta, alpha;

  for (int r=0;r<max_n_clfrs;++r)
  {
    vcl_cout<<"adaboost training round = "<<r<<vcl_endl;

    //vcl_cout<<"wts0= "<<wts0<<vcl_endl;
    //vcl_cout<<"wts1= "<<wts1<<vcl_endl;

    int best_i=-1;
    double min_error= 100000;
    for (int i=0;i<d;++i)
    {
      double error = builder.build(*c1d,&data[i][0],wts);
      if (i==0 || error<min_error)
      {
        min_error = error;
        best_c1d= c1d->clone();
        best_i = i;
      }
    }

    assert(best_i != -1);

    vcl_cout<<"best_i= "<<best_i<<vcl_endl;
    vcl_cout<<"min_error= "<<min_error<<vcl_endl;

    if (min_error<1e-10)  // Hooray!
    {
      vcl_cout<<"min_error<1e-10 !!!"<<vcl_endl;
      alpha  = vcl_log(2.0*n);   //is this appropriate???
      strong_classifier.add_classifier( best_c1d, alpha, best_i);

      // delete classifiers on heap, cos clones taken by strong_classifier
      delete c1d;
      delete best_c1d;
      return;
    }


    if (0.5-min_error<1e-10) // Oh dear, no further improvement possible
    {
      vcl_cout<<"min_error => 0.5 !!!" <<vcl_endl;
      beta=1.0;

      // delete classifiers on heap, cos clones taken by strong_classifier
      delete c1d;
      delete best_c1d;
      return;
    }

    beta = min_error/(1.0-min_error);
    alpha  = -1.0*vcl_log(beta);
    strong_classifier.add_classifier( best_c1d, alpha, best_i);

    if (r<(n-1))
    {
      // update the wts using the best weak classifier
      for (int j=0;j<n;++j)
        if (best_c1d->classify(data[best_i][j].first)==data[best_i][j].second)
          wts[data[best_i][j].third]*=beta;

      double w_sum= wts.mean()*n;
      wts/=w_sum;
    }
  }

  delete c1d;
  delete best_c1d;
}

//=======================================================================

short clsfy_adaboost_sorted_trainer::version_no() const
{
    return 1;
}

//=======================================================================

vcl_string clsfy_adaboost_sorted_trainer::is_a() const
{
  return vcl_string("clsfy_adaboost_sorted_trainer");
}

//=======================================================================

    // required if data stored on the heap is present in this class
#if 0
clsfy_adaboost_sorted_trainer::clsfy_adaboost_sorted_trainer(const clsfy_adaboost_sorted_trainer& new_b):
  data_ptr_(0)
{
    *this = new_b;
}

//=======================================================================

    // required if data stored on the heap is present in this class
clsfy_adaboost_sorted_trainer& clsfy_adaboost_sorted_trainer::operator=(const clsfy_adaboost_sorted_trainer& new_b)
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
#endif

//=======================================================================

    // required if data is present in this class
void clsfy_adaboost_sorted_trainer::print_summary(vcl_ostream& os) const
{
    // os << data_; // example of data output
    vcl_cerr << "clsfy_adaboost_sorted_trainer::print_summary() NYI" << vcl_endl;
}

//=======================================================================

  // required if data is present in this class
void clsfy_adaboost_sorted_trainer::b_write(vsl_b_ostream& bfs) const
{
  //vsl_b_write(bfs, version_no());
  //vsl_b_write(bfs, data_);
  vcl_cerr << "clsfy_adaboost_sorted_trainer::b_write() NYI" << vcl_endl;
}

//=======================================================================

  // required if data is present in this class
void clsfy_adaboost_sorted_trainer::b_read(vsl_b_istream& bfs)
{
  vcl_cerr << "clsfy_adaboost_sorted_trainer::b_read() NYI" << vcl_endl;
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
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, clsfy_adaboost_sorted_trainer&) \n";
    vcl_cerr << "           Unknown version number "<< version << "\n";
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
#endif
}

//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const clsfy_adaboost_sorted_trainer& b)
{
  b.b_write(bfs);
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, clsfy_adaboost_sorted_trainer& b)
{
  b.b_read(bfs);
}

//=======================================================================

void vsl_print_summary(vcl_ostream& os,const clsfy_adaboost_sorted_trainer& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
}

//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const clsfy_adaboost_sorted_trainer& b)
{
  vsl_print_summary(os,b);
  return os;
}
