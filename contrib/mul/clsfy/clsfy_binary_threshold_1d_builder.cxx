// This is mul/clsfy/clsfy_binary_threshold_1d_builder.cxx

//:
// \file
// \brief give a brief description of the file.
// \author dac
// \date   Tue Mar  5 01:11:31 2002
// Put some file description here
//
// \verbatim
// Modifications
// \endverbatim

#include <clsfy/clsfy_binary_threshold_1d_builder.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vsl/vsl_binary_loader.h>
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
// nb here egs0 are -ve examples
// and egs1 are +ve examples
double clsfy_binary_threshold_1d_builder::build(clsfy_classifier_1d& classifier,
                                  vnl_vector<double>& egs0,
                                  vnl_vector<double>& wts0,
                                  vnl_vector<double>& egs1,
                                  vnl_vector<double>& wts1)  const
{
  assert(classifier.is_a()=="clsfy_binary_threshold_1d");

  vcl_vector<vbl_triple<double,int,double> > data;
  int n0 = egs0.size();
  int n1 = egs1.size();
  vbl_triple<double,int,double> t;
  // add data for class 0
  double tot_wts0= 0.0;
  for (int i=0;i<n0;++i)
  {
    tot_wts0+= wts0[i];
    t.first=egs0[i];
    t.second=0;
    t.third = wts0[i];
    data.push_back(t);
  }

  // add data for class 1
  double tot_wts1= 0.0;
  for (int i=0;i<n1;++i)
  {
    tot_wts1+= wts1[i];
    t.first=egs1[i];
    t.second=1;
    t.third = wts1[i];
    data.push_back(t);
  }
#if 0
  vcl_cout<<"tot_wts0= "<<tot_wts0<<vcl_endl;
  vcl_cout<<"tot_wts1= "<<tot_wts1<<vcl_endl;
#endif
  return build(classifier,&data[0],n0+n1,tot_wts0, tot_wts1);
}

//: Train classifier, returning weighted error
//   Assumes two classes
double clsfy_binary_threshold_1d_builder::build(
                                  clsfy_classifier_1d& classifier,
                                  vbl_triple<double,int,double> *data,
                                  int n,
                                  double tot_wts0,
                                  double tot_wts1
                                  ) const
{
  vcl_sort(data,data+n);

  double e0=0.0, e1=0.0, min_err=2.0;
  double etot0,etot1;
  int index=-1, polarity=0;
  for (int i=0;i<n;++i)
  {
    if (data[i].second==0)
      e0+=data[i].third;
    else
      e1+=data[i].third;

    etot0=(tot_wts0-e0) +e1;
    etot1=(tot_wts1-e1) +e0;

    if ( etot0< min_err)
    {
      // ie class1 is maximally separated from class0 at this point
      // also members of class1 are generally greater than members of class0
      polarity=+1;                    //indicates direction of > sign
      index=i;            //the threshold

      min_err= etot0;
    }

    if ( etot1< min_err)
    {
      // ie class1 is maximally separated from class0 at this point
      // also members of class1 are generally less than members of class0
      polarity=-1;                    //indicates direction of > sign
      index=i;            //the threshold

      min_err= etot1;
    }
  }

  assert ( index!=-1 );

  // determine threshold from data index
  double threshold;
  if ( index==n-1 )
    threshold=data[index].first+0.01;
  else
    threshold=(data[index].first+data[index+1].first)/2;

  // pass parameters to classifier
  vnl_vector<double> params(2);
  params[0]=polarity;
  params[1]=threshold*polarity;
  classifier.set_params(params);
  return min_err;
}

#if 0 // This doesn't work properly so try again!

//: Train classifier, returning weighted error
//   Assumes two classes
double clsfy_binary_threshold_1d_builder::build(
                                  clsfy_classifier_1d& classifier,
                                  vbl_triple<double,int,double> *data,
                                  int n,
                                  double tot_wts0,
                                  double tot_wts1
                                  ) const
{
  vcl_sort(data,data+n);

  double e0=0.0, e1=0.0;
  for (int i=0;i<n;++i)
  {
    //vcl_cout<<data[i].first<<vcl_endl;

    if (data[i].second==0)
      e0+=data[i].third;
    else
      e1+=data[i].third;

    if ( tot_wts0-e0 <= e1 &&  e0>e1 )
    {
      // ie class1 is maximally separated from class0 at this point
      // also members of class1 are generally greater than members of class0
      vnl_vector<double> params(2);
      params(0)=+1;                    //indicates direction of > sign
      params(1)=+1*data[i].first;            //the threshold
      classifier.set_params(params);


      //vcl_cout<<"tot_wts0= "<<tot_wts0<<vcl_endl;
      //vcl_cout<<"e0= "<<e0<<vcl_endl;
      //vcl_cout<<"tot_wts1= "<<tot_wts1<<vcl_endl;
      //vcl_cout<<"e1= "<<e1<<vcl_endl;

      return (tot_wts0-e0) + e1;   //ie the error with this classifier
    }

    if ( tot_wts1-e1 <= e0 && e1>e0 )
    {
      // ie class1 is maximally separated from class0 at this point
      // also members of class1 are generally less than members of class0
      vnl_vector<double> params(2);
      params(0)=-1;                       //indicates direction of > sign
      params(1)=-1*data[i].first;            //the threshold
      classifier.set_params(params);


      //vcl_cout<<"tot_wts0= "<<tot_wts0<<vcl_endl;
      //vcl_cout<<"e0= "<<e0<<vcl_endl;
      //vcl_cout<<"tot_wts1= "<<tot_wts1<<vcl_endl;
      //vcl_cout<<"e1= "<<e1<<vcl_endl;

      return (tot_wts1-e1) + e0;   //ie the error with this classifier
    }
  }

  vcl_cout<<" clsfy_binary_threshold_1d_builder:: problem with finding binary threshold\n";
  vcl_abort();
  return -1;  // something went wrong!
}

#endif

//=======================================================================

vcl_string clsfy_binary_threshold_1d_builder::is_a() const
{
  return vcl_string("clsfy_binary_threshold_1d_builder");
}

//=======================================================================

    // required if data stored on the heap is present in this derived class
#if (0)
clsfy_binary_threshold_1d_builder::clsfy_binary_threshold_1d_builder(const clsfy_binary_threshold_1d_builder& new_b):
  data_ptr_(0)
{
  *this = new_b;
}

//=======================================================================

    // required if data stored on the heap is present in this derived class
clsfy_binary_threshold_1d_builder& clsfy_binary_threshold_1d_builder::operator=(const clsfy_binary_threshold_1d_builder& new_b)
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
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, clsfy_binary_threshold_1d_builder&)\n";
    vcl_cerr << "           Unknown version number "<< version << "\n";
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
#endif
}
