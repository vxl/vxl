//:
// \file
// \brief Functions to learn modes from subsets of data
// \author Tim Cootes

#include <iostream>
#include <algorithm>
#include "mcal_extract_mode.h"
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_data_array_wrapper.h>
#include <mcal/mcal_pca.h>

//: Computes one mode from used elements of each \p dv
//  Effectives computes the first eigenvector of the
//  covariance matrix formed from selecting the used elements
//  of each \p dv[i], ie \p dv[i][used[j]].
//  Resulting vector is returned as a full length vector
//  (the same size as \p dv[i]).
//
//  The contribution of this vector is removed from each \p dv,
//  \p dv[i]-=mode*b, where \p b=dv[i].mode
void mcal_extract_mode(std::vector<vnl_vector<double> >& dv,
                       const std::vector<unsigned>& used,
                       vnl_vector<double>& mode,
                       double& var)
{
  if (used.size()==0)
  {
    // Use all elements
    mcal_extract_mode(dv,mode,var);
    return;
  }

  unsigned ns=dv.size();
  unsigned nd=dv[0].size();

  // Extract the elements into a new set of vectors
  std::vector<vnl_vector<double> > v(ns);
  for (unsigned i=0;i<ns;++i)
  {
    v[i].set_size(used.size());
    for (unsigned j=0;j<used.size();++j)
      v[i][j]=dv[i][used[j]];
  }

  // Perform PCA
  // We assume data to be zero mean
  vnl_vector<double> zero_mean(v[0].size(),0.0),mode_var0;
  vnl_matrix<double> modes0;
  mcal_pca pca;
  pca.set_mode_choice(1,1,1.0);
  mbl_data_array_wrapper<vnl_vector<double> > data(v);
  pca.build_about_mean(data,zero_mean,modes0,mode_var0);

  // Reconstruct mode from first mode of PCA
  var = mode_var0[0];
  mode.set_size(nd);
  mode.fill(0.0);
  vnl_vector<double> sub_mode = modes0.get_column(0);
  for (unsigned j=0;j<used.size();++j)
    mode[used[j]] = sub_mode[j];

  // Compute parameters and remove effect from dv
  for (unsigned i=0;i<ns;++i)
  {
    double b = dot_product(sub_mode,v[i]);
    dv[i] -= b*mode;
  }
}

//: Computes one mode by applying PCA to \p dv
//  Effectives computes the first eigenvector of the
//  covariance matrix.
//  The contribution of this vector is removed from each \p dv,
//  \p dv[i]-=mode*b, where \p b=dv[i].mode
void mcal_extract_mode(std::vector<vnl_vector<double> >& dv,
                       vnl_vector<double>& mode,
                       double& var)
{
  unsigned ns=dv.size();

  // Perform PCA
  // We assume data to be zero mean
  vnl_vector<double> zero_mean(dv[0].size(),0.0),mode_var0;
  vnl_matrix<double> modes0;
  mcal_pca pca;
  pca.set_mode_choice(1,1,1.0);
  mbl_data_array_wrapper<vnl_vector<double> > data(dv);
  pca.build_about_mean(data,zero_mean,modes0,mode_var0);

  // Reconstruct mode from first mode of PCA
  var = mode_var0[0];
  mode = modes0.get_column(0);

  // Compute parameters and remove effect from dv
  for (unsigned i=0;i<ns;++i)
  {
    double b = dot_product(mode,dv[i]);
    dv[i] -= b*mode;
  }
}


//: Compute modes and associated variance of supplied data
//  \param used[i] indicates the set of elements to be used for
//  mode i.  Modes beyond \p used.size() will use all elements.
void mcal_extract_modes(std::vector<vnl_vector<double> >& dv,
                        const std::vector<std::vector<unsigned> >& used,
                        unsigned max_modes, double var_prop,
                        vnl_matrix<double>& modes,
                        vnl_vector<double>& mode_var)
{
  unsigned ns=dv.size();

  // Compute total variance, and threshold
  double total_var=0;
  for (unsigned i=0;i<ns;++i)
    total_var += dv[i].squared_magnitude();

  total_var/=ns;
  double var_thresh = var_prop*total_var;

  std::vector<vnl_vector<double> > mode_set(used.size());
  std::vector<double> var_set(used.size());

  double var_sum=0.0;

  // Extract all modes associated with known used subsets
  for (unsigned i=0;i<used.size();++i)
  {
    mcal_extract_mode(dv,used[i],mode_set[i],var_set[i]);
    var_sum+=var_set[i];
  }

  // Now find the modes required to explain the remainder of the
  // data.

  if (var_sum<var_thresh && used.size()<max_modes)
  {
    // Perform PCA
    // We assume data to be zero mean
    vnl_vector<double> mean0(dv[0].size()),mode_var0;
    mean0.fill(0.0);  // Zero mean
    vnl_matrix<double> modes0;
    mcal_pca pca;
    pca.set_mode_choice(1,max_modes-used.size(),1.0);
    mbl_data_array_wrapper<vnl_vector<double> > data(dv);
    pca.build_about_mean(data,mean0,modes0,mode_var0);

    // Add modes until we pass threshold on variance or n.modes.
    unsigned m=0;
    while (mode_set.size()<max_modes &&
           var_sum<var_thresh && m<mode_var0.size())
    {
      // Add mode m
      vnl_vector<double> mode=modes0.get_column(m);
      mode_set.push_back(mode);
      var_set.push_back(mode_var0[m]);
      var_sum += mode_var0[m];
      m++;  // Move to next mode
    }
  }

  unsigned nd = dv[0].size();
  unsigned n_modes = mode_set.size();

  // Construct the output data
  modes.set_size(nd,n_modes);
  mode_var.set_size(n_modes);
  for (unsigned i=0;i<n_modes;++i)
  {
    modes.set_column(i,mode_set[i]);
    mode_var[i]=var_set[i];
  }
}
