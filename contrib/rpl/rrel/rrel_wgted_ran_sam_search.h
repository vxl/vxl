#ifndef rrel_wgted_ran_sam_search_h_
#define rrel_wgted_ran_sam_search_h_
//:
// \file
// \brief Weighted/Non-uniform Random sampling search 
//        for minimization of a robust objective function
// \author Gehua Yang (yangg2@rpi.edu)
// \date Nov 2004

#include <rrel/rrel_ran_sam_search.h>
#include <vcl_vector.h>

class rrel_wgted_ran_sam_search : public rrel_ran_sam_search
{
 public:
  //: Constructor using a non-deterministic random-sampling seed.
  rrel_wgted_ran_sam_search( ) 
  : rrel_ran_sam_search(), is_sim_wgt_set_( false )
  {   }

  //: Constructor using a given random-sampling seed.
  rrel_wgted_ran_sam_search( int seed )
  : rrel_ran_sam_search( seed ), is_sim_wgt_set_( false )
  {   }

  ~rrel_wgted_ran_sam_search( );

  // ----------------------------------------
  //  Main estimation functions
  // ----------------------------------------

  //: \brief Estimation for an "ordinary" estimation problem.
  virtual bool 
  estimate( const rrel_estimation_problem* problem,
            const rrel_objective* obj_fcn );

  // -----------------------------------------
  // Access to results and computed parameters
  // -----------------------------------------

 protected:
  // ------------------------------------------------------------
  //  Random sampling functions.  Don't call directly.  These are
  //  public for test purposes.
  // ------------------------------------------------------------

  //: Determine the next random sample, filling in the "sample" vector.
  virtual void 
  next_sample( unsigned int taken, unsigned int num_points, vcl_vector<int>& sample,
               unsigned int points_per_sample );

 protected:
  struct prob_interval {
    int      index_;
    double   lower_;
    double   upper_;
    
    prob_interval() : index_(-1), lower_(-1.0), upper_(-1.0) {  }
    
    bool operator< ( const prob_interval& rhs ) {
      return this->upper_ < rhs.upper_; 
    }
  };
  
  //: store the intervals
  vcl_vector<prob_interval> intervals_;
  bool is_sim_wgt_set_;
};

#endif
