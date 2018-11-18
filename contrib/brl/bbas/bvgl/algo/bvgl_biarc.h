// This is bbas/bvgl/algo/bvgl_biarc.h

#ifndef bvgl_algo_biarc_h_
#define bvgl_algo_biarc_h_

//:
// \file
// \brief Biarc - two connected circular arcs with continuous tangent
//
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date 2/03/05
//
// \verbatim
//  The main algorithm was written by Amir Tamrakar (yakkeeper@lems.brown.edu)
//  Modifications:
// \endverbatim


#include <iostream>
#include <ostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>


class bvgl_biarc
{

public:
  // -------------- BASIC ----------------------------
  //: Constructor - default
  bvgl_biarc()= default;;

  //: Constructor - from intrinsic parameters
  bvgl_biarc(vgl_point_2d<double > start, double start_angle, double k1, double len1, double k2, double len2);

  //: Constructor - from 2 point-tangents
  bvgl_biarc( vgl_point_2d< double > start, double start_angle,
    vgl_point_2d< double > end, double end_angle );

  //: Constructor - from 2 point-tangents
  bvgl_biarc( vgl_point_2d< double > start, vgl_vector_2d<double > start_tangent,
    vgl_point_2d< double > end, vgl_vector_2d<double > end_tangent );

  //: Destructor
  ~bvgl_biarc()= default;;

  //: Return the starting point of the biarc
  vgl_point_2d< double > start() const { return this->start_; }
  //: Set the starting point of the biarc - type 1
  void set_start( double x_start, double y_start ){ this->start_.set(x_start, y_start); }
  //: Set the starting point of the biarc - type 2
  void set_start( const vgl_point_2d< double > & start ){ this->start_ = start; }

  //: Return the starting angle of the biarc, in the range [0, 2*pi)
  double start_angle() const { return this->start_angle_; }
  //: Set the start angle, converted to the range [0, 2*pi)
  void set_start_angle( double start_angle );


  //: Return the end point of the biarc
  vgl_point_2d< double > end() const { return this->end_; }

  //: Return the ending angle of the biarc
  double end_angle() const { return this->end_angle_; }

  //: Return curvature of the first arc
  double k1() const { return this->k1_; }
  //: Set the curvature of the first arc
  void set_k1( double k1 ){ this->k1_ = k1; }

  //: Return the lengh of the first arc
  double len1() const { return this->len1_; }
  void set_len1( double len1 ){ this->len1_ = len1; }

  //: Return curvature of the second arc
  double k2() const { return this->k2_; }
  //: Set the curvature of the second arc
  void set_k2( double k2 ){ this->k2_ = k2; }

  //: Return the lengh of the second arc
  double len2() const { return this->len2_; }
  void set_len2( double len2 ){ this->len2_ = len2; }

  //: Return the flag indicator
  // -1: construction failed
  // 0 : degenerate case (line, arc)
  // 1 : true biarc
  int flag() const { return this->flag_; }

  //: Verify if indeed the biarc parameters and its start and end parameters match
  bool is_consistent() const;

  // ---------------- GEOMETRY PROPERTIES -----------------------

  //: Return radius of the first arc
  double r1() const;
  //: Return radius of the second arc
  double r2() const;

  //: return turning direction of first arc, CCW = +1
  int dir1() const { return ((this->k1() < 0) ? -1 : 1); }
  //: return turning direction of second arc, CCW = +1
  int dir2() const { return ((this->k2() < 0) ? -1 : 1); }

  //: Return the point at which two arcs meet
  vgl_point_2d< double > mid_pt() const;
  //: Return the center of the first arc
  vgl_point_2d< double > center1() const ;
  //: Return the center of the second arc
  vgl_point_2d< double > center2() const;

  // --- TO BE CONSISTENT WITH CURVE CLASS -----

  //: Return the length of the total biarc
  double len() const { return this->len1() + this->len2(); }
  //: Return a point on the biarc with s arclength away from starting point
  vgl_point_2d< double > point_at( double s) const;
  //: Return tangent of a point on the biarc with s arclength away from starting point
  vgl_vector_2d< double > tangent_at( double s) const;
  //: Return curvature of a point on the biarc with s arclength away from starting point
  double curvature_at( double s) const;

  // ---------------- SUPPORT FUNCTIONS ---------------------------

  //: set parameters at starting point
  void set_start_params( vgl_point_2d< double > start, double start_angle );
  //: set parameters at ending point
  void set_end_params( vgl_point_2d< double > end, double end_angle );

  //: compute biar parameters using the currently saved start and end parameters
  bool compute_biarc_params();

  //: compute biar parameters, given start and end parameters
  bool compute_biarc_params( vgl_point_2d< double > start, double start_angle,
    vgl_point_2d< double > end, double end_angle );

  //: compute biar parameters, given start and end parameters
  bool compute_biarc_params(const vgl_point_2d< double >& start,
    const vgl_vector_2d<double >& start_tangent,
    const vgl_point_2d< double >& end,
    const vgl_vector_2d<double >& end_tangent );

  //: Compute the angle at which the two arcs meet
  double compute_join_theta (double k1, double k2);
  //: compute arc length of a circle with curvature k, turning from angle t0 to t1
  double compute_arclength    (double t0, double t1, double k);

  // ---------------- MISCELLANEOUS ----------------------
  //: Print parameters of the biarc
  void print(std::ostream &os ) const;

protected:
  // core member variables
  //: starting point of biarc
  vgl_point_2d< double > start_;
  //: angle of tangent at starting point
  double start_angle_;
  //: curvature of the first arc
  double k1_;
  //: length of the first arc
  double len1_;
  //: curvature of the second arc
  double k2_;
  //: length of second arc
  double len2_;

  // external member variables, used to compute core member variables
  //: ending point
  vgl_point_2d< double > end_;
  //: angle of tangent at end point
  double end_angle_;

  // some other useful params
  //: energy of the biarc
  double energy_;
  //: flag to indicate whether this is a true biarc or just degenerate case (circle, line)
  int flag_;


  //: set the end point of the biarc
  void set_end(const vgl_point_2d< double > & end ){ this->end_ = end; }

  //: Set end angle of the biarc - converted to the range [0, 2pi)
  void set_end_angle( double end_angle );

private:
};


#endif // bbas/bvgl/algo/bvgl_biarc.h
