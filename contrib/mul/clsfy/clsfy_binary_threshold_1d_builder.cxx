// This is mul/clsfy/clsfy_binary_threshold_1d_builder.cxx
#include "clsfy_binary_threshold_1d_builder.h"
//:
// \file
// \author dac
// \date   Tue Mar  5 01:11:31 2002

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vsl/vsl_binary_loader.h>
#include <vnl/vnl_double_2.h>
#include <clsfy/clsfy_builder_1d.h>
#include <clsfy/clsfy_binary_threshold_1d.h>
#include <vcl_algorithm.h>

//=======================================================================

clsfy_binary_threshold_1d_builder::clsfy_binary_threshold_1d_builder()
{
}

//=======================================================================

clsfy_binary_threshold_1d_builder::~clsfy_binary_threshold_1d_builder()
{
}

//=======================================================================

short clsfy_binary_threshold_1d_builder::version_no() const
{
  return 1;
}


//: Create empty classifier
// Caller is responsible for deletion
clsfy_classifier_1d* clsfy_binary_threshold_1d_builder::new_classifier() const
{
  return new clsfy_binary_threshold_1d();
}


//: Build a binary_threshold classifier
//  Train classifier, returning weighted error
//  Selects parameters of classifier which best separate examples from two classes,
//  weighting examples appropriately when estimating the missclassification rate.
//  Returns weighted sum of error, e.wts, where e_i =0 for correct classifications,
//  e_i=1 for incorrect.
double clsfy_binary_threshold_1d_builder::build(clsfy_classifier_1d& classifier,
                                                const vnl_vector<double>& egs,
                                                const vnl_vector<double>& wts,
                                                const vcl_vector<unsigned> &outputs) const
{
  // this method sorts the data and passes it to the method below
  assert(classifier.is_class("clsfy_mean_square_1d"));

  unsigned int n= egs.size();
  assert ( wts.size() == n );
  assert ( outputs.size() == n );

  // create triples data, so can sort
  vcl_vector<vbl_triple<double,int,int> > data;

  vbl_triple<double,int,int> t;
  // add data to triples
  for (unsigned int i=0;i<n;++i)
  {
    t.first=egs(i);
    t.second= outputs[i];
    t.third = i;
    data.push_back(t);
  }

  vbl_triple<double,int,int> *data_ptr=&data[0];
  vcl_sort(data_ptr,data_ptr+n);
  return build_from_sorted_data(classifier, &data[0], wts);
}


//: Build a binary_threshold classifier
// nb here egs0 are -ve examples
// and egs1 are +ve examples
double clsfy_binary_threshold_1d_builder::build(clsfy_classifier_1d& classifier,
                                                vnl_vector<double>& egs0,
                                                vnl_vector<double>& wts0,
                                                vnl_vector<double>& egs1,
                                                vnl_vector<double>& wts1)  const
{
  // this method sorts the data and passes it to the method below
  assert(classifier.is_class("clsfy_binary_threshold_1d"));

  vcl_vector<vbl_triple<double,int,int> > data;
  unsigned int n0 = egs0.size();
  unsigned int n1 = egs1.size();
  vnl_vector<double> wts(n0+n1);
  vbl_triple<double,int,int> t;
  // add data for class 0
  for (unsigned int i=0;i<n0;++i)
  {
    t.first=egs0[i];
    t.second=0;
    t.third = i;
    wts(i)= wts0[i];
    data.push_back(t);
  }

  // add data for class 1
  for (unsigned int i=0;i<n1;++i)
  {
    t.first=egs1[i];
    t.second=1;
    t.third = i+n0;
    wts(i+n0)= wts1[i];
    data.push_back(t);
  }

  unsigned int n=n0+n1;

  vbl_triple<double,int,int> *data_ptr=&data[0];
  vcl_sort(data_ptr,data_ptr+n);

  return build_from_sorted_data(classifier,&data[0], wts);
}


//: Train classifier, returning weighted error
//   Assumes two classes
double clsfy_binary_threshold_1d_builder::build_from_sorted_data(
                                  clsfy_classifier_1d& classifier,
                                  const vbl_triple<double,int,int> *data,
                                  const vnl_vector<double>& wts
                                  ) const
{
  // here the triple consists of (value, class number, example index)
  // the example index specifies the weight of each example
  //
  // NB DATA must be sorted for this to work!!!!


  // calc total weights for class0 and class1 separately
  unsigned int n=wts.size();
  double tot_wts0=0.0, tot_wts1=0.0;
  for (unsigned int i=0;i<n;++i)
    if (data[i].second==0)
      tot_wts0+=wts(data[i].third);
    else
      tot_wts1+=wts(data[i].third);

  double e0=0.0, e1=0.0, min_err=2.0;
  double etot0,etot1;
  unsigned int index=n; int polarity=0;
  for (unsigned int i=0;i<n;++i)
  {
    if (data[i].second==0)
      e0+=wts(data[i].third);
    else
      e1+=wts(data[i].third);

    etot0=(tot_wts0-e0) +e1;
    etot1=(tot_wts1-e1) +e0;

    if ( etot0< min_err)
    {
      // i.e. class1 is maximally separated from class0 at this point
      // also members of class1 are generally greater than members of class0
      polarity=+1;        //indicates direction of > sign
      index=i;            //the threshold

      min_err= etot0;
    }

    if ( etot1< min_err)
    {
      // i.e. class1 is maximally separated from class0 at this point
      // also members of class1 are generally less than members of class0
      polarity=-1;        //indicates direction of > sign
      index=i;            //the threshold

      min_err= etot1;
    }
  }

  assert ( index!=n );

  // determine threshold from data index
  double threshold;
  if ( index+1==n )
    threshold=data[index].first+0.01;
  else
    threshold=(data[index].first+data[index+1].first)/2;

  // pass parameters to classifier
  vnl_double_2 params(polarity, threshold*polarity);
  classifier.set_params(params.as_vector());
  return min_err;
}

//=======================================================================

vcl_string clsfy_binary_threshold_1d_builder::is_a() const
{
  return vcl_string("clsfy_binary_threshold_1d_builder");
}

bool clsfy_binary_threshold_1d_builder::is_class(vcl_string const& s) const
{
  return s == clsfy_binary_threshold_1d_builder::is_a() || clsfy_builder_1d::is_class(s);
}

//=======================================================================

#if 0

// required if data stored on the heap is present in this derived class
clsfy_binary_threshold_1d_builder::clsfy_binary_threshold_1d_builder(
                             const clsfy_binary_threshold_1d_builder& new_b) :
  data_ptr_(0)
{
  *this = new_b;
}

//=======================================================================

// required if data stored on the heap is present in this derived class
clsfy_binary_threshold_1d_builder&
clsfy_binary_threshold_1d_builder::operator=(const clsfy_binary_threshold_1d_builder& new_b)
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

clsfy_builder_1d* clsfy_binary_threshold_1d_builder::clone() const
{
  return new clsfy_binary_threshold_1d_builder(*this);
}

//=======================================================================

// required if data is present in this base class
void clsfy_binary_threshold_1d_builder::print_summary(vcl_ostream& /*os*/) const
{
  // clsfy_builder_1d::print_summary(os); // Uncomment this line if it has one.
  // vsl_print_summary(os, data_); // Example of data output

  vcl_cerr << "clsfy_binary_threshold_1d_builder::print_summary() NYI\n";
}

//=======================================================================

// required if data is present in this base class
void clsfy_binary_threshold_1d_builder::b_write(vsl_b_ostream& /*bfs*/) const
{
  //vsl_b_write(bfs, version_no());
  //clsfy_builder_1d::b_write(bfs);  // Needed if base has any data
  //vsl_b_write(bfs, data_);
  vcl_cerr << "clsfy_binary_threshold_1d_builder::b_write() NYI\n";
}

//=======================================================================

  // required if data is present in this base class
void clsfy_binary_threshold_1d_builder::b_read(vsl_b_istream& /*bfs*/)
{
  vcl_cerr << "clsfy_binary_threshold_1d_builder::b_read() NYI\n";
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
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, clsfy_binary_threshold_1d_builder&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
#endif // 0
}
