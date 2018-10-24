#ifndef rrel_muset_obj_h_
#define rrel_muset_obj_h_
//:
//  \file
//  \author Chuck Stewart
//  \date   Summer 2001
//  The MUSET (MUSE trimmed) objective function., which should be used instead of LMS.

#include <rrel/rrel_objective.h>

class rrel_muse_table;

enum rrel_muse_type { RREL_MUSE_TRIMMED, RREL_MUSE_TRIMMED_SQUARE, RREL_MUSE_QUANTILE };

//: The MUSET (MUSE trimmed) objective function, which should be used instead of LMS.
//  MUSE is a robust objective function in the spirit of LMS/LTS
//  (least-median-of-squares / least-trimmed-squares).  In fact, it
//  can be considered a generalization of these objective functions.
//  It can be used in place of them and it really SHOULD because it
//  (a) produces much better results, regardless of the inlier
//  fraction, and (b) (unlike RANSAC) can tolerate large fractions of
//  outliers WITHOUT prior knowledge of scale.  It should be used in
//  combination with a random sampling search and an appropriate problem
//  representation.
//
//  Rather than assuming a fixed minimum fraction of inliers and then
//  building an objective function based on either the order
//  statistics (e.g. the median) or the trimmed statistics, MUSE
//  ADAPTIVELY determines the inlier fraction and computes its
//  objective function from this inlier fraction.   It does this by
//  converting the fitting error (residual) order statistics into
//  scale estimates, normalizing these, and then choosing the
//  smallest.  The version given here is called MUSET, because it is
//  based on trimmed statistics.  This is described in Chapter 4 of
//  Jim Miller's thesis.  A new paper describing MUSE will be written
//  in the autumn of 2001.
//
//  MUSE requires a look-up table to compute its normalizing
//  parameters.  If it does not get one, the constructor builds it.
//  MUSE also requires the minimum and maximum possible inlier
//  fractions in the data.  Usually it is safe to leave the maximum
//  fraction at 0.95.  The minimum inlier fraction determines the
//  random sampling parameters, so it should be set with some care.
//  Setting it too low can cause a lot of unnecessary search.
//  Setting it too high can cause small structures to be missed.
//
//  Finally, MUSE internally contains an additional step called sk
//  refinement, which increases its ability to distinguish small
//  scale structures.  In general it is safe to use this, and the
//  constructor defaults to having it set.

class rrel_muset_obj : public rrel_objective
{
 public:
  //: Constructor.
  //  \a max_n is the size of the look-up table.
  rrel_muset_obj( int max_n, bool use_sk_refine=true);

  //: Constructor with previously computed table.
  //  \a table will be used as the look-up table.
  rrel_muset_obj( rrel_muse_table* table, bool use_sk_refine=true);

  //: Destructor.
  ~rrel_muset_obj() override;

  //: Evaluate the objective function on heteroscedastic residuals.
  //  \sa rrel_objective::fcn.
  double fcn( vect_const_iter res_begin, vect_const_iter res_end,
                      vect_const_iter /* scale is unused */,
                      vnl_vector<double>* = nullptr /* param vector is unused */ ) const override;

  //: Evaluate the objective function on homoscedastic residuals.
  //  \sa rrel_objective::fcn.
  double fcn( vect_const_iter begin, vect_const_iter end,
                      double = 0 /* scale is unused */,
                      vnl_vector<double>* = nullptr /* param vector is unused */ ) const override;

  //: Computes the MUSE estimate and best value of k
  //  \a begin and \a end give the residuals. \a objective is the
  //  value of the objective function, \a sigma_est is an estimate
  //  of the scale (and is the objective function), and \a best_k
  //  is the value of k that produced the scale estimate.
  void internal_fcn( vect_const_iter begin, vect_const_iter end,
                     double& sigma_est, int& best_k ) const;

  //: False.
  //  This MUSE estimator is based on trimmed statistics, and does not
  //  use a scale estimate.
  bool requires_prior_scale() const override
    { return false; }

  //: True, since MUSE can estimate scale.
  //  \sa rrel_objective::can_estimate_scale.
  bool can_estimate_scale() const override { return true; }

  //: Scale estimate.
  double scale( vect_const_iter res_begin, vect_const_iter res_end ) const override;

  //: Set the minimum fraction of the data that are inliers.
  void set_min_inlier_fraction( double min_frac=0.25 )
    {  min_frac_ = min_frac; }

  //: Set the maximum fraction of the data that could be inliers.
  void set_max_inlier_fraction( double max_frac=0.95 )
    {  max_frac_ = max_frac; }

  //: The increment to use in testing different inlier/outlier fractions.
  void set_inlier_fraction_increment( double frac_inc=0.05 )
    {  frac_inc_ = frac_inc; }

  //: The minimum fraction of the data that must be inliers.
  double min_inlier_fraction() const { return min_frac_; }

  //: The maximum fraction of the data that could be inliers.
  double max_inlier_fraction() const { return max_frac_; }

  //: The search step for determining the inlier/outlier fraction.
  double inlier_fraction_increment() const { return frac_inc_; }

  //: Set the type of MUSE objective function
  void set_muse_type( rrel_muse_type t = RREL_MUSE_TRIMMED )
    { muse_type_ = t; }

  //: Access the type of MUSE objective function
  rrel_muse_type muse_type() const { return muse_type_; }

 protected:
  bool use_sk_refine_;
  rrel_muse_type muse_type_;

  bool table_owned_;
  rrel_muse_table* table_;
  double min_frac_;
  double max_frac_;
  double frac_inc_;
};

#endif
