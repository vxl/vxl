#ifndef bvnl_parabolic_interpolator_h_
#define bvnl_parabolic_interpolator_h_

//-----------------------------------------------------------------------------
//:
// \file
// \brief Parabolic interpolation of a 1-d point set
//
// \author
//   J.L. Mundy - November 12, 2003
//
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vnl/vnl_matrix.h>


//-----------------------------------------------------------------------------
class bvnl_parabolic_interpolator
{
 public:
  bvnl_parabolic_interpolator(){};
  ~bvnl_parabolic_interpolator(){};
  //:p is the parameter to be interpolated, v is the data value
  void add_data_point(const double p, const double v);
  //: reset data array
  void clear();
  //: current number of data points
  int n_points();
  //: Solve linear regression for the parabola
  bool solve();
  //:the peak/valley of the parabola 
  double extremum(){return p_ext_;}
  //: Print the data
  void print();
private:
  //private methods
  void fill_scatter_matrix();
  //members
  vcl_vector<double> p_; //parameter values
  vcl_vector<double> v_; //data values
  vnl_matrix<double> s_; //scatter matrix
  double p_ext_;//extremum values
};


};

#endif // bvnl_parabolic_interpolator_h_
