// \file 
// \brief to match two sets of curves 
// \author vj (vj@lems.brown.edu)
// \date   8/20/2003
// \verbatim
// Modifications
// \endverbatim
// Input : two lists of curves
// Output : returns the best hypothesis for correspondence between the curves.
#ifndef bdgl_curve_matching_h_
#define bdgl_curve_matching_h_

#include<bdgl/bdgl_tracker_curve.h>
#include<bdgl/bdgl_tracker_curve_sptr.h>
#include<bdgl/bdgl_curve_description.h>
#include<vcl_vector.h>
#include<vcl_algorithm.h>
#include<vcl_map.h>

class bdgl_curve_matching_params
{
 public:
  // Parameters

  int  motion_in_pixels;
  int  no_of_top_choices;
 
  bdgl_curve_matching_params(){motion_in_pixels=0;no_of_top_choices=0;}
  bdgl_curve_matching_params(int motion,int top_ranks)
  {
    motion_in_pixels=motion;
	no_of_top_choices=top_ranks;
  }

  ~bdgl_curve_matching_params(){}
};
class bdgl_curve_matching
{
  public:

  bdgl_curve_matching();
  // constructor initializing the estimated motion of object
  bdgl_curve_matching(bdgl_curve_matching_params & mp)
    {
	  motion_in_pixels_=mp.motion_in_pixels;
	  no_of_top_choices_=mp.no_of_top_choices;
	 }
  ~bdgl_curve_matching(){}
  
  // takes two sets of curves and returns the best corresponding(matching) curves
  void match(vcl_vector<bdgl_tracker_curve_sptr >  * new_curves,
			 vcl_vector<bdgl_tracker_curve_sptr >  * old_curves);
  
 // coarser version of curve-matching to reduce the number of 
 // candidates of curve-matching
  double coarse_match_DP(bdgl_curve_description * desc1,
						 bdgl_curve_description * desc2);
 // curve-matching algorithm 
  double match_DP(bdgl_curve_description  * desc1,
				  bdgl_curve_description  * desc2,
				  vcl_map<int,int> &mapping,double &cost,
		          vnl_matrix<double> &R,vnl_matrix<double> &T,
		          vnl_matrix<double> &Tbar,double & scale,
	         	  vcl_vector<int> & tail1,vcl_vector<int> & tail2);

  // compute the euclidean dist of  b using transformation of a
  double compute_euc_dist(bdgl_tracker_curve_sptr a,
						  bdgl_tracker_curve_sptr b);
  
  protected:
	  // compute the best match of tail in previous curve
  void match_prev_tail_curve(bdgl_tracker_curve_sptr parent_curve,
							 bdgl_tracker_curve_sptr & tail_curve,
							 vcl_vector<bdgl_tracker_curve_sptr> * new_curves);
	 // compute the best match of tail in next curve
  void match_next_tail_curve(bdgl_tracker_curve_sptr parent_curve,
						     bdgl_tracker_curve_sptr & tail_curve,
						     vcl_vector<bdgl_tracker_curve_sptr> * old_curves);
	// intialize the cost matrix 
  void initialize_matrix(vcl_vector<bdgl_tracker_curve>  new_curves,
						 vcl_vector<bdgl_tracker_curve > old_curves);
  
	// function to compute matches using statistical info of curves
  double match_stat(bdgl_curve_description * desc1,
				    bdgl_curve_description  * desc2);
	// check if the bounding boxes of the curves intersect or not
  bool bounding_box_intersection(vsol_box_2d_sptr box1,vsol_box_2d_sptr box2);
	// compute the best pairwise match
  void best_matches(vcl_vector<bdgl_tracker_curve_sptr> * new_curves,
					vcl_vector<bdgl_tracker_curve_sptr> * old_curves);
	// merging curves to form a single curve
  void merge_curves(bdgl_tracker_curve_sptr cs1,
					bdgl_tracker_curve_sptr cs2,
					bdgl_tracker_curve_sptr &outcs);
  
	// compute mean 
  double compute_mean(vcl_vector<double> t);
    // compute stalndard deviation 
  double compute_std(vcl_vector<double> t);
	// estimated motion in pixels
  int motion_in_pixels_;
    // after coarse DP Match filter top rank choices
  int no_of_top_choices_;

};



#endif
