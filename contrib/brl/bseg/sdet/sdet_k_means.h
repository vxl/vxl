// This is brl/bseg/sdet/sdet_k_means.h
#ifndef sdet_k_means_h
#define sdet_k_means_h
//:
// \file
// \author Ian Scott
// \date 18-May-2001
// \brief K Means clustering functions
// \verbatim
// Copied to sdet to avoid cross linking, J.L. Mundy December 13, 2011
// \endverbatim
#include <vcl_vector.h>
#include <vnl/vnl_vector.h>

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
unsigned sdet_k_means(vcl_vector<vnl_vector<double> > &data, unsigned& k,
                     vcl_vector<vnl_vector<double> >* cluster_centres,
                     vcl_vector<unsigned> * partition =0 );


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
unsigned sdet_k_means_weighted(vcl_vector<vnl_vector<double> > &data,
                               unsigned& k,
                              const vcl_vector<double>& wts,
                              vcl_vector<vnl_vector<double> >* cluster_centres,
                              vcl_vector<unsigned> * partition =0);
#endif // sdet_k_means_h
