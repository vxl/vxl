#pragma once
#include <boxm2/vecf/boxm2_vecf_orbit_params.h>
class boxm2_vecf_orbit_exporter
{
public:
  boxm2_vecf_orbit_exporter() {}
  void set_params(vcl_string const& param_path, bool is_right);

  void set_params(boxm2_vecf_orbit_params & lprm, boxm2_vecf_orbit_params& rprm);

  void export_orbit(bool is_right, vcl_vector<vgl_point_3d<double> >& crease_pts,
                    vcl_vector<vgl_point_3d<double> >& sup_pts, vcl_vector<vgl_point_3d<double> >& inf_pts,
                    vcl_string export_fname_base = "");

private:
  boxm2_vecf_orbit_params left_params_, right_params_;
};
