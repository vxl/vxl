#ifndef bdgl_curve_tracking_h_
#define bdgl_curve_tracking_h_

#include <vtol/vtol_edge_2d_sptr.h>
#include <vcl_vector.h>

#include <bdgl/bdgl_tracker_curve_sptr.h>
#include <bdgl/bdgl_curve_matching.h>
#include <bdgl/bdgl_curve_clustering.h>


#include<vcl_iostream.h>
class bdgl_curve_tracking_params
{
 public:
  // Parameters

  bool clustering_;
  bool transitive_closure;
  int window_size;
  int min_length_of_curves;
  bool ps_moving_curves;
  bool ps_match_curves;

  bdgl_curve_matching_params mp ;
  bdgl_curve_clustering_params cp ;

  bdgl_curve_tracking_params(){}

  bdgl_curve_tracking_params(bdgl_curve_matching_params &mpa,
                             bdgl_curve_clustering_params &cpa,
                             bool clustering,int min_len)
  { mp=mpa;cp=cpa,clustering_=clustering;min_length_of_curves=min_len;}

  ~bdgl_curve_tracking_params(){}
};


class bdgl_curve_tracking
{
 public:
  // Input : for each image, for each
  bdgl_curve_tracking(){}
  bdgl_curve_tracking(bdgl_curve_tracking_params &tp);
  ~bdgl_curve_tracking(){}
  // list of input curves for curve-tracking
  vcl_vector<vcl_vector< vtol_edge_2d_sptr > > input_curves_;

  // Output
  vcl_vector<vcl_vector<bdgl_tracker_curve_sptr > > output_curves_;
  // Output to compute transitive closure
  vcl_vector<vcl_vector<bdgl_tracker_curve_sptr > > output_curves_tc_;

#if 0
  // Params
  bdgl_curve_tracker_params params_;
  bdgl_curve_tracking(bdgl_curve_tracker_params p){ params_ = p; }
#endif // 0

  vcl_vector<vcl_vector<vtol_edge_2d_sptr > > * get_input(){ return & input_curves_; }
  void set_input(vcl_vector< vcl_vector< vtol_edge_2d_sptr > > curve){ input_curves_=curve; }

  vcl_vector< vcl_vector< bdgl_tracker_curve_sptr> > * get_output(){ return & output_curves_; }
  void set_output(vcl_vector< vcl_vector< bdgl_tracker_curve_sptr > > curve){ output_curves_=curve; }

  // returns no of curves in the given frame
  int get_output_size_at(unsigned int frame)
  {
    if (frame>=output_curves_.size())
      return -1;
    else
      return output_curves_[frame].size();
  }

  vcl_vector< bdgl_tracker_curve_sptr> *get_output_curves(unsigned int frame_no);
  bdgl_tracker_curve_sptr get_output_curve(unsigned int frame_no, int set_id);


  void write_clusters(vcl_string filename,int i);
  void get_reliable_curves(unsigned int frame_no, unsigned int window_sz);
  // tracking of  the sequence
  void track();
  // tracking for the given frame
  void track_frame(unsigned int frame);
  void write_results(vcl_string name);
  void write_tracks(bdgl_tracker_curve_sptr curve,vcl_string fname,int min_length_of_track);
  // traversal function to get tracks
  void level_order_traversal(bdgl_tracker_curve_sptr curve,vcl_list<bdgl_tracker_curve_sptr> & tr);
  void obtain_tracks();
  int get_min_len_of_curves(){return tp_.min_length_of_curves;}
  vcl_map<int,int> len_of_tracks_;
  vcl_string outfilename;
 protected:
	 
	 bdgl_curve_tracking_params tp_;
	 vcl_vector<vcl_vector<bdgl_tracker_curve_sptr> > moving_curves_;
	 vcl_vector<vcl_vector<vcl_vector<bdgl_tracker_curve_sptr> > > frame_moving_curves_;

 private:
	// current frame number 
  int frame_;
  bool clustering_;
 
  int min_len_of_curves_;
};

#endif
