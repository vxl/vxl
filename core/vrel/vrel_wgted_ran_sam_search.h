#ifndef vrel_wgted_ran_sam_search_h_
#define vrel_wgted_ran_sam_search_h_
//:
// \file
// \brief Weighted/Non-uniform Random sampling search for minimization of a robust objective function
// \author Gehua Yang (yangg2@rpi.edu)
// \date Nov 2004

#include <iostream>
#include <vector>
#include <vrel/vrel_ran_sam_search.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vrel_wgted_ran_sam_search : public vrel_ran_sam_search
{
public:
  //: Constructor using a non-deterministic random-sampling seed.
  vrel_wgted_ran_sam_search()
    : vrel_ran_sam_search()
  {}

  //: Constructor using a given random-sampling seed.
  vrel_wgted_ran_sam_search(int seed)
    : vrel_ran_sam_search(seed)
    , is_sim_wgt_set_(false)
  {}

  ~vrel_wgted_ran_sam_search() override;

  // ----------------------------------------
  //  Main estimation functions
  // ----------------------------------------

  //: \brief Estimation for an "ordinary" estimation problem.
  bool
  estimate(const vrel_estimation_problem * problem, const vrel_objective * obj_fcn) override;

  // -----------------------------------------
  // Access to results and computed parameters
  // -----------------------------------------

protected:
  // ------------------------------------------------------------
  //  Random sampling functions.  Don't call directly.  These are
  //  public for test purposes.
  // ------------------------------------------------------------

  //: Determine the next random sample, filling in the "sample" vector.
  void
  next_sample(unsigned int taken,
              unsigned int num_points,
              std::vector<int> & sample,
              unsigned int points_per_sample) override;

protected:
  struct prob_interval
  {
    int index_{ -1 };
    double lower_{ -1.0 };
    double upper_{ -1.0 };

    prob_interval() = default;

    bool
    operator<(const prob_interval & rhs) const
    {
      return this->upper_ < rhs.upper_;
    }
  };

  //: store the intervals
  std::vector<prob_interval> intervals_;
  bool is_sim_wgt_set_{ false };
};

#endif
