#ifndef bdgl_curve_tracker_h_
#define bdgl_curve_tracker_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author P.L. Bazin
// \brief tracking of extracted vdgl_edgel_chains on sequences
//
// \verbatim
//  Initial version February 12, 2003
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_utility.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <bdgl/bdgl_curve_matcher.h>
#include <bdgl/bdgl_curve_description.h>
#include <bdgl/bdgl_curve_tracker_primitive.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>

//-----------------------------------------------------------------------------
class bdgl_curve_tracker_params
{
 public:
  // Parameters

  bool debug_;               //!< Carry out debug processing
  bool verbose_;             //!< Print detailed output

  double match_thres_;         // threshold on the allowed matching quality

  bdgl_curve_matcher_params match_params_;

  bdgl_curve_tracker_params(){}

  bdgl_curve_tracker_params(bool verbose, bool debug){verbose_=verbose; debug_=debug; }
  bdgl_curve_tracker_params(double th){ match_thres_ = th; }

  ~bdgl_curve_tracker_params(){}
};


//-----------------------------------------------------------------------------
class bdgl_curve_tracker
{
 public:
  // Input : for each image, for each
  vcl_vector< vcl_vector< vdgl_edgel_chain_sptr > > input_curve_;

  // Output
  vcl_vector< vcl_vector< bdgl_curve_tracker_primitive > > output_curve_;
  // temporary output for debugging
  vcl_vector< bdgl_curve_tracker_primitive > temp_output_curve_;
  // Params
  bdgl_curve_tracker_params params_;

  bdgl_curve_tracker(){}
  bdgl_curve_tracker(bdgl_curve_tracker_params p){ params_ = p; }
  ~bdgl_curve_tracker(){}
  vcl_vector< vcl_vector< vdgl_edgel_chain_sptr > > get_input(){ return input_curve_; }
  void set_input(vcl_vector< vcl_vector< vdgl_edgel_chain_sptr > > curve){ input_curve_=curve; }

  vcl_vector< vcl_vector< bdgl_curve_tracker_primitive > > get_output(){ return output_curve_; }
  void set_output(vcl_vector< vcl_vector< bdgl_curve_tracker_primitive > > curve){ output_curve_=curve; }

  vdgl_edgel_chain_sptr get_output_curve_at( unsigned int frame, unsigned int num )
    { if (output_curve_.size()<=frame || output_curve_[frame].size()<=num) return 0;
      return output_curve_[frame][num].get_curve(); }
  
  int get_output_id_at( unsigned int frame, unsigned int num )
    { if (output_curve_.size()<=frame || output_curve_[frame].size()<=num) return -1;
      return output_curve_[frame][num].get_id(); }
  
  int get_output_size_at( unsigned int frame )
    { if (output_curve_.size()<=frame) return -1; return output_curve_[frame].size(); }

  //double get_tx_at( unsigned int frame, unsigned int num
  void track();
  void track_frame(unsigned int frame);
  vcl_vector<vsol_spatial_object_2d_sptr>  group_spat_objs_;

  vcl_vector<vcl_map<int,double> >  filter_top_ranks(vcl_vector<vcl_map<int,double> > &cost_table);
  void segment_curves(int frame);
  vcl_vector<vcl_vector<int> > aspects;
  vcl_vector< vcl_vector< bdgl_curve_tracker_primitive > > aspect_output_curve_;

  
 private:

  vcl_map<vcl_pair<int,int>,vcl_vector<double> > map_of_T;
  vcl_map<int,int> store_matches;
	  
};


/*template <class CURVE_TYPE>
class bdgl_curve_tracker
{
 public:
  
  
  // Params
  bdgl_curve_tracker_params params_;

  bdgl_curve_tracker(){}
  bdgl_curve_tracker(bdgl_curve_tracker_params p){ params_ = p; }
  ~bdgl_curve_tracker(){}


  vcl_vector< vcl_vector< CURVE_TYPE > > get_input(){ return input_curve_; }
  void set_input(vcl_vector< vcl_vector< CURVE_TYPE > > curve){ input_curve_=curve; }

  vcl_vector< vcl_vector< bdgl_curve_sets > > get_output(){ return output_curve_; }
  void set_output(vcl_vector< vcl_vector< bdgl_curve_sets > > curve){ output_curve_=curve; }

  

 
  void track();
  void track_frame(unsigned int frame);

  
 
  
 private:
	// Input : for each image, for each
  vcl_list< vcl_vector< CURVE_TYPE > > input_curve_;

  // Output (matched hypotheses)
  vcl_list< vcl_vector< bdgl_curve_sets > > output_curve_;
  
	  
};*/

class bdgl_tracking_feature
{
	public:
		bdgl_tracking_feature(vdgl_digital_curve_sptr & edge);
		~bdgl_tracking_feature(){};
		
		vdgl_digital_curve_sptr get_member_edge(){return member_edge_;}
		int add_child(vdgl_digital_curve_sptr c  );
		int add_parent(vdgl_digital_curve_sptr  p);
		vcl_vector<vdgl_digital_curve_sptr> get_childern();
		vcl_vector<vdgl_digital_curve_sptr> get_parents();
		
	protected:
		vdgl_digital_curve_sptr member_edge_;
		vcl_vector<vdgl_digital_curve_sptr> child_;
		vcl_vector<vdgl_digital_curve_sptr> parent_;
	//	bdgl_curve_description desc;
};

#endif
