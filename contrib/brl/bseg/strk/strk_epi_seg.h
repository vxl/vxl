//---*-c++-*--
#ifndef strk_epi_seg_h_
#define strk_epi_seg_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a connected sequence of epi points to maintain edgel connectivity
//    
//  An epi segment is a one-to-one and onto map between the original
//  digital curve parametrization and the parameter space of epipolar lines.
//  Because of the one-to-one and onto map it is possible to identify 
//  potentially matching points on the curve as having the same epipolar
//  parameter. The segment is parametrized by alpha which defines the
//  space of epipolar lines. The class supports interpolation to continous
//  values of alpha.  The properites of the segment that are needed for
//  matching:
//              tangent angle
//              left intensity
//              right intensity
//  
// \author
//  J.L. Mundy - December 27, 2003
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vbl/vbl_ref_count.h>
#include <strk/strk_epi_point_sptr.h>
#include <strk/strk_epi_seg_sptr.h>

class strk_epi_seg :  public vbl_ref_count
{
  
 public:

  strk_epi_seg();

  strk_epi_seg(vcl_vector<strk_epi_point_sptr> const & points);

  virtual ~strk_epi_seg() {}

  //: accessors  
  void add_int_sample(const double alpha, 
                      const double left_int, const double right_int);


  void set_min_index(int index){min_index_ = index;}
  void set_max_index(int index){max_index_ = index;}
  int min_index(){return min_index_;}
  int max_index(){return max_index_;}


  void add_point(strk_epi_point_sptr ep);


  int n_pts() const {return seg_.size();}
  strk_epi_point_sptr p(int i) const {return seg_[i];}
  

  //: extremal geometry values
  double min_alpha();
  double max_alpha();
  double min_s();
  double max_s();
  double avg_tan_ang();
  double min_tan_ang();
  double max_tan_ang();

  //: extremal intensity values
  double avg_left_int();
  double avg_right_int();
  double left_int_sd();
  double right_int_sd();
  double min_left_int();
  double max_left_int();
  double min_right_int();
  double max_right_int();

  //: interpolated values
  double s(double alpha);
  double tan_ang(double alpha);
  double left_int(double alpha);
  double right_int(double alpha);

  //: utility methods
  void clear(){seg_.clear();}

  //: static matching methods
  static double tan_ang_match(const double a,
                              strk_epi_seg_sptr const& sa, 
                              strk_epi_seg_sptr const& sb,
                              const double ang_sd = 5.0); 
  static double left_int_match(const double a, 
                                   strk_epi_seg_sptr const& sa, 
                                   strk_epi_seg_sptr const& sb); 
  static double right_int_match(const double a,
                                    strk_epi_seg_sptr const& sa, 
                                    strk_epi_seg_sptr const& sb); 
  static double velocity_coef(const double a, 
                              strk_epi_seg_sptr const& sa, 
                              strk_epi_seg_sptr const& sb);
  static double match(const double a, 
                      strk_epi_seg_sptr const& sa, 
                      strk_epi_seg_sptr const& sb);
  protected:
  //: local methods
  void compute_limits();
  void compute_int_values();

  bool limits_valid_;//up to date coordinate geometry
  bool int_valid_;//up to date intensity values
  int min_index_;
  int max_index_;

  double min_alpha_;
  double max_alpha_;
  double min_s_;
  double max_s_;
  double avg_tan_ang_;
  double min_tan_ang_;
  double max_tan_ang_;
  //:points are in original digital curve order but also monotonic in alpha
  vcl_vector<strk_epi_point_sptr> seg_;
  vcl_vector<double> int_alpha_;//the alphas for intensity samples
  vcl_vector<double> left_int_; // left intensity average
  vcl_vector<double> right_int_;// right intensity average
  double avg_left_int_;
  double avg_right_int_;
  double left_int_sd_;
  double right_int_sd_;
  double min_left_int_;
  double max_left_int_;
  double min_right_int_;
  double max_right_int_;
};

vcl_ostream&  operator<<(vcl_ostream& s, strk_epi_seg const& ec);  

#endif // strk_epi_seg_h_
