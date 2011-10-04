#ifndef __BRAD_SYNOPTIC_FUNCTION_1D_H
#define __BRAD_SYNOPTIC_FUNCTION_1D_H
//-----------------------------------------------------------------------------
//:
// \file
// \brief A function for interpolating intensity with respect to viewpoint
//
//
// \author J. L. Mundy
// \date October 1, 2011
//
//----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vnl/vnl_double_4.h>
class brad_synoptic_function_1d
{
 public:


  brad_synoptic_function_1d(){}
  //:constructor from batch orbit data
  brad_synoptic_function_1d(vcl_vector<double> const& elevation,
                            vcl_vector<double> const& azimuth,                 
                            vcl_vector<double> const& vis,
                            vcl_vector<double> const& intensity):
    elev_(elevation),azimuth_(azimuth),vis_(vis),intensity_(intensity)
    {this->fit_intensity_cubic();}
  void set_elevation(vcl_vector<double> const& elevation){
    elev_ = elevation;}
  void set_azimuth(vcl_vector<double> const& azimuth){
   azimuth_ = azimuth;}
  void set_vis(vcl_vector<double> const& vis){
   vis_ = vis;}
  void set_intensity(vcl_vector<double> const& intensity){
   intensity_ = intensity;}
  void fit_intensity_cubic();
  //:load batch orbit data from a file
  bool load_samples(vcl_string const& path);
  unsigned size(){return elev_.size();}
  double intensity(unsigned index){return intensity_[index];}
  double vis(unsigned index){return vis_[index];}
  double elev(unsigned index){return elev_[index];}
  double azimuth(unsigned index){return azimuth_[index];}
  //:spherical arc length at sample index
  double arc_length(unsigned index);
  //:spherical angle between two points on unit sphere
  static double angle(double elev0, double az0, double elev1, double az1);
  vnl_double_4 cubic_coef_int(){return cubic_coef_int_;}
  double cubic_interp_inten(double arc_length);
  double fit_error(){return fit_error_;}
 private:
  vcl_vector<double> elev_;
  vcl_vector<double> azimuth_;
  vcl_vector<double> vis_;
  vcl_vector<double> intensity_;
  vnl_double_4 cubic_coef_int_;
  double fit_error_;
};

#endif
