// This is mul/clsfy/clsfy_binary_threshold_1d_gini_builder.cxx
#include <iostream>
#include <string>
#include <algorithm>
#include "clsfy_binary_threshold_1d_gini_builder.h"
//:
// \file
// \author Martin Roberts

#include <cassert>
#include <vsl/vsl_binary_loader.h>
#include <vnl/vnl_double_2.h>
#include <clsfy/clsfy_builder_1d.h>
#include <clsfy/clsfy_binary_threshold_1d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// Note this is used by clsfy_binary_tree_builder
// Derived from clsfy_binary_threshold_1d_builder but uses a slightly different
// interface to do the gini index optimisation, as tis returns the reduction
// in the gini impurity (not classification error).

//=======================================================================

clsfy_binary_threshold_1d_gini_builder::clsfy_binary_threshold_1d_gini_builder() = default;

//=======================================================================

clsfy_binary_threshold_1d_gini_builder::~clsfy_binary_threshold_1d_gini_builder() = default;

//=======================================================================

short clsfy_binary_threshold_1d_gini_builder::version_no() const
{
    return 1;
}


//: Create empty classifier
// Caller is responsible for deletion
clsfy_classifier_1d* clsfy_binary_threshold_1d_gini_builder::new_classifier() const
{
    return new clsfy_binary_threshold_1d();
}

//: Build a binary_threshold classifier
//  Train classifier
//  Selects parameters of classifier which best separate examples from two classes,
// Uses the gini impurity index
// Note it returns the -reduction in Gini impurity produced by the split
// Not the misclassification rate
// (i.e. but minimise as per error rate)
double clsfy_binary_threshold_1d_gini_builder::build_gini(clsfy_classifier_1d& classifier,
                                                          const vnl_vector<double>&  inputs,
                                                          const std::vector<unsigned> &outputs) const
{
    assert(classifier.is_class("clsfy_binary_threshold_1d"));

    unsigned n = inputs.size();
    assert ( outputs.size() == n );

    // create triples data, so can sort
    std::vector<vbl_triple<double,int,int> > data;
    data.reserve(n);

    //First just create sorted data
    auto classIter=outputs.begin();
    vnl_vector<double  >::const_iterator inputIter=inputs.begin();
    vnl_vector<double  >::const_iterator inputIterEnd=inputs.end();
    vbl_triple<double,int,int> t;
    unsigned i=0;
    while (inputIter != inputIterEnd)
    {
        t.first = *inputIter++;
        t.second=*classIter++;
        t.third = i++;
        data.push_back(t);
    }

    assert(i==inputs.size());

    std::sort(data.begin(),data.end());
    return build_gini_from_sorted_data(static_cast<clsfy_classifier_1d&>(classifier), data);
}


//: Train classifier, returning weighted error
//   Assumes two classes
//  Note that input "data" must be sorted to use this routine
//Return -improvement in impurity (as normally these builders minimise)
double clsfy_binary_threshold_1d_gini_builder::build_gini_from_sorted_data(
    clsfy_classifier_1d& classifier,
    const std::vector<vbl_triple<double,int,int> >& data) const
{
    // here the triple consists of (value, class number, example index)
    // the example index specifies the weight of each example
    //
    // NB DATA must be sorted for this to work!!!!

    //Validate that the data is not homogeneous
    const double epsilon=1.0E-20;
    if (std::fabs(data.front().first-data.back().first)<epsilon)
    {
        std::cerr<<"WARNING - clsfy_binary_threshold_1d_gini_builder::build_from_sorted_data - homogeneous data - cannot split\n";
        int polarity=1;
        double threshold=data[0].first;
        vnl_double_2 params(polarity, threshold*polarity);
        classifier.set_params(params.as_vector());
        return 1.0;
    }

    unsigned int ntot=data.size();
    auto dntot=double (ntot);
    auto dataIter=data.begin();
    auto dataIterEnd=data.end();
    unsigned n0Tot=0;
    unsigned n1Tot=0;
    while (dataIter != dataIterEnd)
    {
        if (dataIter->second==0)
            ++n0Tot;
        else
            ++n1Tot;
        ++dataIter;
    }

    double parentImp=0.0;
    //Parent level impurity to start with

    double p=double (n0Tot)/dntot;
    parentImp=2.0*p*(1-p);

    dataIter=data.begin();
    double s=dataIter->first-epsilon;
    double deltaImpBest= -1.0; //initialise to split makes it worse
    double  sbest=s;
    //Put none into left bin, all else go right
    unsigned nL0=0;
    unsigned nL1=0;
    unsigned nR0=n0Tot;
    unsigned nR1=n1Tot;
    double parity=1.0;
    while (dataIter != dataIterEnd)
    {
        s=dataIter->first;
        auto dataIterNext=dataIter;

        //Increment till threshold increases (may have some same data values)
        while (dataIterNext != dataIterEnd && (dataIterNext->first-s)<epsilon)
        {
            if (dataIterNext->second==0)
            {
                ++nL0;
                --nR0;
            }
            else
            {
                ++nL1;
                --nR1;
            }
            ++dataIterNext;
        }

        unsigned nLTot=nL0+nL1;
        unsigned nRTot=nR0+nR1;
        double probL=double(nL0)/double(nLTot);
        double probR=double(nR1)/double(nRTot);
        //Two-class Gini index for left and right
        double impL=2.0*probL*(1-probL);
        double impR=2.0*probR*(1-probR);

        //Proportional weights
        double pL=double (nLTot)/dntot;
        double pR=1.0-pL;

        double deltaImp=parentImp-(pL*impL + pR*impR);
        if (deltaImp>deltaImpBest)
        {
            deltaImpBest=deltaImp;
            sbest=s;
            if (nR1>=nL1) //More class 1 are going above thresh
                parity=1;
            else
                parity=-1; //Reverse sign as more class one are going below thresh
        }

        dataIter=dataIterNext;
    }

    double threshold=sbest;

    // pass parameters to classifier
    vnl_double_2 params(parity, threshold*parity);
    classifier.set_params(params.as_vector());
    return -deltaImpBest;
}

//=======================================================================

std::string clsfy_binary_threshold_1d_gini_builder::is_a() const
{
  return std::string("clsfy_binary_threshold_1d_gini_builder");
}

bool clsfy_binary_threshold_1d_gini_builder::is_class(std::string const& s) const
{
  return s == clsfy_binary_threshold_1d_gini_builder::is_a() || clsfy_builder_1d::is_class(s);
}

# if 0
//=======================================================================


// required if data stored on the heap is present in this derived class
clsfy_binary_threshold_1d_gini_builder::clsfy_binary_threshold_1d_gini_builder(
                             const clsfy_binary_threshold_1d_gini_builder& new_b) :
  data_ptr_(0)
{
  *this = new_b;
}

//=======================================================================

// required if data stored on the heap is present in this derived class
clsfy_binary_threshold_1d_gini_builder&
clsfy_binary_threshold_1d_gini_builder::operator=(const clsfy_binary_threshold_1d_gini_builder& new_b)
{
    if (&new_b==this) return *this;

    static_cast<clsfy_binary_threshold_1d_builder&>(*this)=
        static_cast<const clsfy_binary_threshold_1d_builder&> (new_b);

    return *this;
}
#endif
//=======================================================================

// required if data is present in this base class
void clsfy_binary_threshold_1d_gini_builder::print_summary(std::ostream& os) const
{
    os<<"clsfy_binary_threshold_1d_gini_builder"<<std::endl;
}

//=======================================================================


clsfy_builder_1d* clsfy_binary_threshold_1d_gini_builder::clone() const
{
    return new clsfy_binary_threshold_1d_gini_builder(*this);
}
//=======================================================================

// required if data is present in this base class
void clsfy_binary_threshold_1d_gini_builder::b_write(vsl_b_ostream& bfs) const
{
    vsl_b_write(bfs, version_no());
    clsfy_binary_threshold_1d_builder::b_write(bfs);
}

//=======================================================================

  // required if data is present in this base class
void clsfy_binary_threshold_1d_gini_builder::b_read(vsl_b_istream& bfs)
{
    if (!bfs) return;

    short version;
    vsl_b_read(bfs,version);
    switch (version)
    {
        case (1):
            clsfy_binary_threshold_1d_builder::b_read(bfs);
            break;
        default:
            std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, clsfy_binary_threshold_1d_gini_builder&)\n"
                     << "           Unknown version number "<< version << '\n';
            bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
            return;
    }
}
