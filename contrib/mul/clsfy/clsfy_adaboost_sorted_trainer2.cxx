// This is mul/clsfy/clsfy_adaboost_sorted_trainer2.cxx
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


#include <clsfy/clsfy_adaboost_sorted_trainer2.h>
#include <vcl_iostream.h>
#include <vsl/vsl_indent.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_cassert.h>
#include <mbl/mbl_file_data_collector.h>

//=======================================================================

clsfy_adaboost_sorted_trainer2::clsfy_adaboost_sorted_trainer2()
{
}

//=======================================================================

clsfy_adaboost_sorted_trainer2::~clsfy_adaboost_sorted_trainer2()
{
}


//: Build classifier composed of 1d classifiers working on individual vector elements
//  Builds an n-component classifier, each component of which is a 1D classifier
//  working on a single element of the input vector.
//  here egs0 are -ve examples
//  and egs1 are +ve examples
void clsfy_adaboost_sorted_trainer2::build_strong_classifier(
                            clsfy_simple_adaboost& strong_classifier,
                            int max_n_clfrs,
                            clsfy_binary_threshold_1d_sorted_builder& builder,
                            mbl_data_wrapper<vnl_vector<double> >& egs0,
                            mbl_data_wrapper<vnl_vector<double> >& egs1,
                            int bs)
{

  // first rearrange data and sort!
  assert(bs>0);
  assert(bs!=1);

  assert (max_n_clfrs >= 0);
  // first arrange the data in the form
  // vcl_vector< < vcl_vector< vtl_triple<double,int,int> > > > data
  // + vnl_vector wts
  // then sort all data once, then build the classifier

  // number of examples
  unsigned n0 = egs0.size();
  unsigned n1 = egs1.size();
  unsigned n=n0+n1;
  //vcl_cout<<"n= "<<n<<vcl_endl;

 
  // Dimensionality of data
  egs0.reset();
  int d = egs0.current().size();

  //need file data wrapper instead of old vector 
  //data stored on disk NOT ram
  //vcl_vector< vcl_vector<vbl_triple<double,int,int> > > data(d);
  vcl_string temp_path= "temp.dat";
  mbl_file_data_collector< 
      vcl_vector< vbl_triple<double,int,int> > 
                         > 
              collector( temp_path );

  // perhaps change this so load and sort several vectors at once!!
  // far too slow at present
  // say load in and sort 100 at once?????
  
  //int bs= 100; //batch size
  vcl_vector< vcl_vector< vbl_triple<double,int,int> > >vec(bs);
  vbl_triple<double,int,int> t;

  vcl_cout<<"d= "<<d<<vcl_endl;
  int b=0;
  while ( b<(d-1) )
  {

    int r= vcl_min ( bs, (d-b) );
    assert(r>0);

    vcl_cout<<"sorting weak classifiers = "<<b<<" to "
                          <<(b+r)-1<<" of "<<d<<vcl_endl;

   
    
    
    // have to resize all vectors
    for (unsigned int i=0; i< bs; ++i)
      vec[i].resize(0);

    // add data for class 0
    egs0.reset();
    for (unsigned int j=0;j<n0;++j)
    {

      for (unsigned int i=0; i< r; ++i)
      {
  
        t.first=egs0.current()[b+i];
        t.second=0;
        t.third = j;
        vec[i].push_back(t);
      }
      egs0.next();
    }

    // add data for class 1
    egs1.reset();
    for (unsigned int j=0;j<n1;++j)
    {

      for (unsigned int i=0; i< r; ++i)
      {
  
        t.first=egs1.current()[b+i];
        t.second=1;
        t.third = j+n0;
        vec[i].push_back(t);
      }
      egs1.next();
    }

    
    for (unsigned int i=0; i< r; ++i)
    {

      // sort data for each individual classifier
      //vbl_triple<double,int,int>  * const data_ptr=&(vec[i][0]);
   
      //vcl_cout<<"i= "<<i<<vcl_endl;
      //vcl_cout<<"vec[i].size()= "<<vec[i].size()<<vcl_endl;
      //vcl_cout<<"n= "<<n<<vcl_endl;
      
      assert (vec[i].size() == n);
      //assert (data_ptr != 0);
      assert (n != 0);
    
      //vcl_sort(data_ptr,data_ptr+n);
      vcl_sort(vec[i].begin(), vec[i].end() );

      // store sorted vector of responses for individual weak classifier
      collector.record(vec[i]);

    }

    b+=bs;

  }


  mbl_data_wrapper< vcl_vector< vbl_triple<double,int,int> > >& 
              wrapper=collector.data_wrapper();


  // now apply adaboost to the sorted data
  apply_adaboost(strong_classifier, max_n_clfrs, builder, wrapper, n0, n1);

}


void clsfy_adaboost_sorted_trainer2::apply_adaboost(
                            clsfy_simple_adaboost& strong_classifier,
                            int max_n_clfrs,
                            clsfy_binary_threshold_1d_sorted_builder& builder,
                            mbl_data_wrapper< 
                          vcl_vector< vbl_triple<double,int,int> >                
                                            >& wrapper,
                                            int n0, int n1)
{

  // remove all alphas and classifiers from strong classifier
  wrapper.reset();
  int n= n0+n1;
  assert ( wrapper.current().size() == n );
  int d= wrapper.size();
  
/*

  vcl_cout<<"d= "<<d<<vcl_endl;
  wrapper.set_index(3);
  const vcl_vector< vbl_triple<double,int,int> >& vec = wrapper.current();
  for (int i=0; i< vec.size(); ++i)
  {
    vcl_cout<<"vec["<<i<<"].first= "<<vec[i].first<<vcl_endl;
    vcl_cout<<"vec["<<i<<"].second= "<<vec[i].second<<vcl_endl;
    vcl_cout<<"vec["<<i<<"].third= "<<vec[i].third<<vcl_endl;
  }
*/

   // initialize weights
  vnl_vector<double> wts(n);
  for (unsigned int j=0; j<n0; ++j)
    wts(j)=0.5/n0;
  for (unsigned int j=0; j<n1; ++j)
    wts(j+n0)=0.5/n1;

  //vcl_cout<<"wts= "<<wts<<vcl_endl;

  strong_classifier.clear();
  strong_classifier.set_n_dims(d);
  clsfy_classifier_1d* c1d = builder.new_classifier();
  clsfy_classifier_1d* best_c1d= builder.new_classifier();


  double beta, alpha;

  long new_time, old_time;
  double tot_time=0;

  for (unsigned int r=0;r<(unsigned)max_n_clfrs;++r)
  {
    vcl_cout<<"adaboost training round = "<<r<<vcl_endl;
    
    new_time = vcl_clock();
    
    if(r>0)
    {
      double dt = (1.0*(new_time-old_time))/CLOCKS_PER_SEC;
      vcl_cout<<"Time for AdaBoost round: "<<vnl_math_rnd(dt)<<"secs"<<vcl_endl;
      tot_time+=dt;
      vcl_cout<<"Total time for rounds so far: "<<vnl_math_rnd(tot_time)<<"secs"<<vcl_endl;
    }

    old_time = new_time;

    //vcl_cout<<"wts0= "<<wts0<<vcl_endl;
    //vcl_cout<<"wts1= "<<wts1<<vcl_endl;
   

    int best_i=-1;
    double min_error= 100000;
    wrapper.reset();  // make sure pointing to first data vector
    for (int i=0;i<d;++i)
    {
      
      const vcl_vector< vbl_triple<double,int,int> >& vec = wrapper.current();
   
      double error = builder.build(*c1d,&vec[0],wts);
      if (i==0 || error<min_error)
      {
        min_error = error;
        delete best_c1d;
        best_c1d= c1d->clone();
        best_i = i;
      }

      wrapper.next();   // move to next data vector
    }

    assert(best_i != -1);

    vcl_cout<<"best_i= "<<best_i<<vcl_endl;
    vcl_cout<<"min_error= "<<min_error<<vcl_endl;

    if (min_error<1e-10)  // Hooray!
    {
      vcl_cout<<"min_error<1e-10 !!!\n";
      alpha  = vcl_log(2.0*n);   //is this appropriate???
      strong_classifier.add_classifier( best_c1d, alpha, best_i);

      // delete classifiers on heap, cos clones taken by strong_classifier
      delete c1d;
      delete best_c1d;
      return;
    }


    if (0.5-min_error<1e-10) // Oh dear, no further improvement possible
    {
      vcl_cout<<"min_error => 0.5 !!!\n";
      beta=1.0;

      // delete classifiers on heap, cos clones taken by strong_classifier
      delete c1d;
      delete best_c1d;
      return;
    }

    beta = min_error/(1.0-min_error);
    alpha  = -1.0*vcl_log(beta);
    strong_classifier.add_classifier( best_c1d, alpha, best_i);

    
    if (r+1<n)  // ie round number less than number of examples used 
    {

      // extract the best weak classifier results
      wrapper.set_index(best_i);
      const vcl_vector< vbl_triple<double,int,int> >& vec = wrapper.current();

      // update the wts using the best weak classifier
      for (unsigned int j=0;j<n;++j)
        if (
              best_c1d-> classify( vec[j].first )
            ==
              (unsigned) vec[j].second
           )
          wts[vec[j].third]*=beta;

      double w_sum= wts.mean()*n;
      wts/=w_sum;
    }
  }

  delete c1d;
  delete best_c1d;
}

//=======================================================================

short clsfy_adaboost_sorted_trainer2::version_no() const
{
    return 1;
}

//=======================================================================

vcl_string clsfy_adaboost_sorted_trainer2::is_a() const
{
  return vcl_string("clsfy_adaboost_sorted_trainer2");
}

//=======================================================================

    // required if data stored on the heap is present in this class
#if 0
clsfy_adaboost_sorted_trainer2::clsfy_adaboost_sorted_trainer2(const clsfy_adaboost_sorted_trainer2& new_b):
  data_ptr_(0)
{
    *this = new_b;
}

//=======================================================================

    // required if data stored on the heap is present in this class
clsfy_adaboost_sorted_trainer2& clsfy_adaboost_sorted_trainer2::operator=(const clsfy_adaboost_sorted_trainer2& new_b)
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
void clsfy_adaboost_sorted_trainer2::print_summary(vcl_ostream& /*os*/) const
{
    // os << data_; // example of data output
    vcl_cerr << "clsfy_adaboost_sorted_trainer2::print_summary() NYI\n";
}

//=======================================================================

  // required if data is present in this class
void clsfy_adaboost_sorted_trainer2::b_write(vsl_b_ostream& /*bfs*/) const
{
  //vsl_b_write(bfs, version_no());
  //vsl_b_write(bfs, data_);
  vcl_cerr << "clsfy_adaboost_sorted_trainer2::b_write() NYI\n";
}

//=======================================================================

  // required if data is present in this class
void clsfy_adaboost_sorted_trainer2::b_read(vsl_b_istream& /*bfs*/)
{
  vcl_cerr << "clsfy_adaboost_sorted_trainer2::b_read() NYI\n";
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
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, clsfy_adaboost_sorted_trainer2&) \n";
    vcl_cerr << "           Unknown version number "<< version << "\n";
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
#endif
}

//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const clsfy_adaboost_sorted_trainer2& b)
{
  b.b_write(bfs);
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, clsfy_adaboost_sorted_trainer2& b)
{
  b.b_read(bfs);
}

//=======================================================================

void vsl_print_summary(vcl_ostream& os,const clsfy_adaboost_sorted_trainer2& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
}

//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const clsfy_adaboost_sorted_trainer2& b)
{
  vsl_print_summary(os,b);
  return os;
}
