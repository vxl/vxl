// This is mul/clsfy/clsfy_mean_square_1d_builder.cxx
#include "clsfy_mean_square_1d_builder.h"
//:
// \file
// \author dac
// \date   Tue Mar  5 01:11:31 2002

#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vcl_cstdlib.h>
#include <vsl/vsl_binary_loader.h>
#include <vnl/vnl_double_2.h>
#include <clsfy/clsfy_builder_1d.h>
#include <clsfy/clsfy_mean_square_1d.h>
#include <vcl_algorithm.h>

//=======================================================================

clsfy_mean_square_1d_builder::clsfy_mean_square_1d_builder()
{
}

//=======================================================================

clsfy_mean_square_1d_builder::~clsfy_mean_square_1d_builder()
{
}

//=======================================================================

short clsfy_mean_square_1d_builder::version_no() const
{
  return 1;
}


//: Create empty classifier
// Caller is responsible for deletion
clsfy_classifier_1d* clsfy_mean_square_1d_builder::new_classifier() const
{
  return new clsfy_mean_square_1d();
}


//: Build a binary_threshold classifier
//  Train classifier, returning weighted error
//  Selects parameters of classifier which best separate examples from two classes,
//  weighting examples appropriately when estimating the missclassification rate.
//  Returns weighted sum of error, e.wts, where e_i =0 for correct classifications,
//  e_i=1 for incorrect.
double clsfy_mean_square_1d_builder::build(clsfy_classifier_1d& classifier,
                                           const vnl_vector<double>& egs,
                                           const vnl_vector<double>& wts,
                                           const vcl_vector<unsigned> &outputs) const
{
  // this method sorts the data and passes it to the method below
  assert(classifier.is_class("clsfy_mean_square_1d"));

  unsigned int n = egs.size();
  assert ( wts.size() == n );
  assert ( outputs.size() == n );

  // calc weighted mean of positive data
  double wm_pos= 0.0;
  double tot_pos_wts=0.0, tot_neg_wts=0.0;
  unsigned int n_pos=0, n_neg=0;
  for (unsigned int i=0; i<n; ++i)
  {
#ifdef DEBUG
    vcl_cout<<"egs["<<i<<"]= "<<egs[i]<<vcl_endl
            <<"wts["<<i<<"]= "<<wts[i]<<vcl_endl
            <<"outputs["<<i<<"]= "<<outputs[i]<<vcl_endl;
#endif
    if ( outputs[i] == 1 )
    {
      //vcl_cout<<"wm_pos= "<<wm_pos<<vcl_endl;
      wm_pos+= wts(i)*egs(i);
      tot_pos_wts+= wts(i);
      ++n_pos;
    }
    else
    {
      tot_neg_wts+= wts(i);
      ++n_neg;
    }
  }

  assert( n_pos+n_neg== n );
  wm_pos/=tot_pos_wts;
#ifdef DEBUG
  vcl_cout<<"wm_pos= "<<wm_pos<<vcl_endl;
#endif
  // create triples data, so can sort
  vcl_vector<vbl_triple<double,int,int> > data;

  vbl_triple<double,int,int> t;
  // add data to triples
  for (unsigned int i=0;i<n;++i)
  {
    double k= wm_pos-egs[i];
    t.first=k*k;
    t.second= outputs[i];
    t.third = i;
    data.push_back(t);
  }

  vbl_triple<double,int,int> *data_ptr=&data[0];
  vcl_sort(data_ptr,data_ptr+n);

  double wt_pos=0;
  double wt_neg=0;
  double min_error= 1000000;
  double min_thresh= -1;
  for (unsigned int i=0;i<n;++i)
  {
    if ( data[i].second == 0 ) wt_neg+= wts[ data[i].third] ;
    else if ( data[i].second == 1 ) wt_pos+= wts[ data[i].third];
    else
    {
      vcl_cout<<"ERROR: clsfy_mean_square_1d_builder::build()\n"
              <<"Unrecognised output value in triple (ie must be 0 or 1)\n"
              <<"data.second="<<data[i].second<<vcl_endl;
      vcl_abort();
    }
    double error= tot_pos_wts-wt_pos+wt_neg;
#ifdef DEBUG
    vcl_cout<<"data[i].first= "<<data[i].first<<vcl_endl
            <<"data[i].second= "<<data[i].second<<vcl_endl
            <<"data[i].third= "<<data[i].third<<vcl_endl

            <<"wt_pos= "<<wt_pos<<vcl_endl
            <<"tot_wts1= "<<tot_wts1<<vcl_endl
            <<"wt_neg= "<<wt_neg<<vcl_endl

            <<"error= "<<error<<vcl_endl;
#endif
    if ( error< min_error )
    {
      min_error= error;
      min_thresh = data[i].first + 0.001 ;
    }
  }

  assert( vcl_fabs (wt_pos - tot_pos_wts) < 1e-9 );
  assert( vcl_fabs (wt_neg - tot_neg_wts) < 1e-9 );
#ifdef DEBUG
  vcl_cout<<"min_error= "<<min_error<<vcl_endl
          <<"min_thresh= "<<min_thresh<<vcl_endl;
#endif
  // pass parameters to classifier
  classifier.set_params(vnl_double_2(wm_pos,min_thresh).as_vector());
  return min_error;
}


//: Build a mean_square classifier
// nb here egs0 are -ve examples
// and egs1 are +ve examples
double clsfy_mean_square_1d_builder::build(clsfy_classifier_1d& classifier,
                                           vnl_vector<double>& egs0,
                                           vnl_vector<double>& wts0,
                                           vnl_vector<double>& egs1,
                                           vnl_vector<double>& wts1)  const
{
  // this method sorts the data and passes it to the method below
  assert(classifier.is_class("clsfy_mean_square_1d"));

  // find mean of positive data (ie egs1) then calc square distance from mean
  // for each example
  unsigned int n0 = egs0.size();
  unsigned int n1 = egs1.size();
  assert (wts0.size() == n0 );
  assert (wts1.size() == n1 );

  // calc weighted mean of positive data
  double tot_wts0= wts0.mean()*n0;
  double tot_wts1= wts1.mean()*n1;
  double wm_pos=0.0;
  for (unsigned int i=0; i< n1; ++i)
  {
    wm_pos+= wts1(i)*egs1(i);
#ifdef DEBUG
    vcl_cout<<"egs1("<<i<<")= "<<egs1(i)<<vcl_endl
            <<"wts1("<<i<<")= "<<wts1(i)<<vcl_endl;
#endif
  }
  wm_pos/=tot_wts1;

  vcl_cout<<"wm_pos= "<<wm_pos<<vcl_endl;

  vcl_vector<vbl_triple<double,int,int> > data;

  vnl_vector<double> wts(n0+n1);
  vbl_triple<double,int,int> t;
  // add data for class 0
  for (unsigned int i=0;i<n0;++i)
  {
    double k= wm_pos-egs0[i];
    t.first=k*k;
    t.second=0;
    t.third = i;
    wts(i)= wts0[i];
    data.push_back(t);
  }

  // add data for class 1
  for (unsigned int i=0;i<n1;++i)
  {
    double k= wm_pos-egs1[i];
    t.first=k*k;
    t.second=1;
    t.third = i+n0;
    wts(i+n0)= wts1[i];
    data.push_back(t);
  }

  unsigned int n=n0+n1;

  vbl_triple<double,int,int> *data_ptr=&data[0];
  vcl_sort(data_ptr,data_ptr+n);

  double wt_pos=0;
  double wt_neg=0;
  double min_error= 1000000;
  double min_thresh= -1;
  for (unsigned int i=0;i<n;++i)
  {
    if ( data[i].second == 0 ) wt_neg+= wts[ data[i].third] ;
    else if ( data[i].second == 1 ) wt_pos+= wts[ data[i].third];
    else
    {
      vcl_cout<<"ERROR: clsfy_mean_square_1d_builder::build()\n"
              <<"Unrecognised output value in triple (ie must be 0 or 1)\n"
              <<"data.second="<<data[i].second<<vcl_endl;
      vcl_abort();
    }
    double error= tot_wts1-wt_pos+wt_neg;
#ifdef DEBUG
    vcl_cout<<"data[i].first= "<<data[i].first<<vcl_endl
            <<"data[i].second= "<<data[i].second<<vcl_endl
            <<"data[i].third= "<<data[i].third<<vcl_endl

            <<"wt_pos= "<<wt_pos<<vcl_endl
            <<"tot_wts1= "<<tot_wts1<<vcl_endl
            <<"wt_neg= "<<wt_neg<<vcl_endl

            <<"error= "<<error<<vcl_endl;
#endif
    if ( error< min_error )
    {
      min_error= error;
      min_thresh = data[i].first + 0.001 ;
    }
  }

  assert( vcl_fabs (wt_pos - tot_wts1) < 1e-9 );
  assert( vcl_fabs (wt_neg - tot_wts0) < 1e-9 );
  vcl_cout<<"min_error= "<<min_error<<vcl_endl
          <<"min_thresh= "<<min_thresh<<vcl_endl;

  // pass parameters to classifier
  classifier.set_params(vnl_double_2(wm_pos,min_thresh).as_vector());
  return min_error;
}


//: Train classifier, returning weighted error
//   Assumes two classes
double clsfy_mean_square_1d_builder::build_from_sorted_data(
                                  clsfy_classifier_1d& /*classifier*/,
                                  const vbl_triple<double,int,int>* /*data*/,
                                  const vnl_vector<double>& /*wts*/
                                  ) const
{
  vcl_cout<<"ERROR: clsfy_mean_square_1d_builder::build_from_sorted_data()\n"
          <<"Function not implemented because can't use pre-sorted data\n"
          <<"the weighted mean of the data is needed to calc the ordering!\n";
  vcl_abort();

  return 0.0;
}

//=======================================================================

vcl_string clsfy_mean_square_1d_builder::is_a() const
{
  return vcl_string("clsfy_mean_square_1d_builder");
}

bool clsfy_mean_square_1d_builder::is_class(vcl_string const& s) const
{
  return s == clsfy_mean_square_1d_builder::is_a() || clsfy_builder_1d::is_class(s);
}

//=======================================================================

#if 0 // two functions commented out

// required if data stored on the heap is present in this derived class
clsfy_mean_square_1d_builder::clsfy_mean_square_1d_builder(
                             const clsfy_mean_square_1d_builder& new_b) :
  data_ptr_(0)
{
  *this = new_b;
}

//=======================================================================

// required if data stored on the heap is present in this derived class
clsfy_mean_square_1d_builder&
clsfy_mean_square_1d_builder::operator=(const clsfy_mean_square_1d_builder& new_b)
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

clsfy_builder_1d* clsfy_mean_square_1d_builder::clone() const
{
  return new clsfy_mean_square_1d_builder(*this);
}

//=======================================================================

// required if data is present in this base class
void clsfy_mean_square_1d_builder::print_summary(vcl_ostream& /*os*/) const
{
  // clsfy_builder_1d::print_summary(os); // Uncomment this line if it has one.
  // vsl_print_summary(os, data_); // Example of data output

  vcl_cerr << "clsfy_mean_square_1d_builder::print_summary() NYI\n";
}

//=======================================================================

// required if data is present in this base class
void clsfy_mean_square_1d_builder::b_write(vsl_b_ostream& /*bfs*/) const
{
  //vsl_b_write(bfs, version_no());
  //clsfy_builder_1d::b_write(bfs);  // Needed if base has any data
  //vsl_b_write(bfs, data_);
  vcl_cerr << "clsfy_mean_square_1d_builder::b_write() NYI\n";
}

//=======================================================================

// required if data is present in this base class
void clsfy_mean_square_1d_builder::b_read(vsl_b_istream& /*bfs*/)
{
  vcl_cerr << "clsfy_mean_square_1d_builder::b_read() NYI\n";
#if 0
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  case (1):
    //clsfy_builder_1d::b_read(bfs);  // Needed if base has any data
    vsl_b_read(bfs,data_);
    break;
  default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, clsfy_mean_square_1d_builder&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
#endif
}
