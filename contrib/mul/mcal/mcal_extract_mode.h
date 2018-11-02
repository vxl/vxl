#ifndef mcal_extract_mode_h_
#define mcal_extract_mode_h_
//:
// \file
// \brief Functions to learn modes from subsets of data
// \author Tim Cootes

#include <iostream>
#include <vector>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Computes one mode from used elements of each \p dv
//  Compute a mode which only uses a subset of the elements,
//  all other elements are set to zero.
//  Effectives computes the first eigenvector of the
//  covariance matrix formed from selecting the used elements
//  of each \p dv[i], ie \p dv[i][used[j]].
//  Resulting vector is returned as a full length vector
//  (the same size as \p dv[i]).
//  \param dv is assumed to be zero mean.
//
//  The contribution of this vector is removed from each \p dv,
//  \p dv[i]-=mode*b, where \p b=dv[i].mode
void mcal_extract_mode(std::vector<vnl_vector<double> >& dv,
                       const std::vector<unsigned>& elements_used,
                       vnl_vector<double>& mode,
                       double& var);

//: Computes one mode by applying PCA to \p dv
//  Effectives computes the first eigenvector of the
//  covariance matrix.
//  The contribution of this vector is removed from each \p dv,
//  \p dv[i]-=mode*b, where \p b=dv[i].mode
void mcal_extract_mode(std::vector<vnl_vector<double> >& dv,
                       vnl_vector<double>& mode,
                       double& var);

//: Compute modes and associated variance of supplied data
//  \param elements_used[i] indicates the set of elements to be used for
//  mode i.  Modes beyond \p used.size() will use all elements.
void mcal_extract_modes(std::vector<vnl_vector<double> >& dv,
                        const std::vector<std::vector<unsigned> >& elements_used,
                        unsigned max_modes, double var_prop,
                        vnl_matrix<double>& modes,
                        vnl_vector<double>& mode_var);

#endif //mcal_extract_mode_h_
