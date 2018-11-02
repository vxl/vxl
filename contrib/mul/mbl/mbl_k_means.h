// This is mul/mbl/mbl_k_means.h
#ifndef mbl_k_means_h
#define mbl_k_means_h
//:
// \file
// \author Ian Scott
// \date 18-May-2001
// \brief K Means clustering functions

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>
#include <mbl/mbl_data_wrapper.h>


//: Find k cluster centres
// Uses batch k-means clustering.
// If you provide parameter partition, it will return the
// cluster index for each data sample. The number of iterations
// performed is returned.
//
// \par Initial Cluster Centres
// If centres contain the correct number of centres, they will
// be used as the initial centres, If not, and if partition is
// given, and it is the correct size, then this will be used
// to find the initial centres.
//
// \par Degenerate Cases
// If at any point the one of the centres has no data points allocated to it
// the number of centres will be reduced below k. This is most likely to
// happen if you start the function with one or more centre identical, or
// if some of the centres start off outside the convex hull of the data set.
// In particular if you let the function initialise the centres, it will
// occur if any of the first k data samples are identical.
unsigned mbl_k_means(mbl_data_wrapper<vnl_vector<double> > &data, unsigned k,
                     std::vector<vnl_vector<double> >* cluster_centres,
                     std::vector<unsigned> * partition =nullptr );


//: Find k cluster centres with weighted data
// Uses batch k-means clustering.
// If you provide parameter partition, it will return the
// cluster index for each data sample. The number of iterations
// performed is returned.
//
// \par Initial Cluster Centres
// If centres contain the correct number of centres, they will
// be used as the initial centres, If not, and if partition is
// given, and it is the correct size, then this will be used
// to find the initial centres.
//
// \par Degenerate Cases
// If at any point the one of the centres has no data points allocated to it
// the number of centres will be reduced below k. This is most likely to
// happen if you start the function with one or more centre identical, or
// if some of the centres start off outside the convex hull of the data set.
// In particular if you let the function initialise the centres, it will
// occur if any of the first k data samples are identical.
unsigned mbl_k_means_weighted(mbl_data_wrapper<vnl_vector<double> > &data, unsigned k,
                              const std::vector<double>& wts,
                              std::vector<vnl_vector<double> >* cluster_centres,
                              std::vector<unsigned> * partition =nullptr);

#endif // mbl_k_means_h
