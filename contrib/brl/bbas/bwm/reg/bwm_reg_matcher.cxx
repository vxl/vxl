#include "bwm_reg_matcher.h"
//:
// \file
#include <vcl_cmath.h>
#include <vnl/vnl_numeric_traits.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_digital_curve_2d.h>

bwm_reg_matcher::
bwm_reg_matcher(vcl_vector<vsol_digital_curve_2d_sptr> const& model_edges,
                unsigned search_col_origin, unsigned search_row_origin,
                unsigned search_cols, unsigned search_rows,
                vcl_vector<vsol_digital_curve_2d_sptr> const& search_edges
               ) :  model_edges_(model_edges),
                    search_col_origin_(search_col_origin),
                    search_row_origin_(search_row_origin),
                    search_cols_(search_cols), search_rows_(search_rows),
                    search_edges_(search_edges),
                    champh_(bwm_reg_edge_champher(search_col_origin,
                                                  search_row_origin,
                                                  search_cols, search_rows,
                                                  search_edges))
{
  //get the bounds on the model edges
  double dcmin = vnl_numeric_traits<double>::maxval, dcmax = 0;
  double drmin = dcmin, drmax = 0;
  vcl_vector<vsol_digital_curve_2d_sptr>::const_iterator cit =
    model_edges.begin();
  for (; cit != model_edges.end(); ++cit)
    for (unsigned i = 0; i<(*cit)->size(); ++i)
    {
      vsol_point_2d_sptr p = (*cit)->point(i);
      double c = p->x(), r = p->y();
      if (c<dcmin) dcmin = c;
      if (c>dcmax) dcmax = c;
      if (r<drmin) drmin = r;
      if (r>drmax) drmax = r;
    }
  model_cols_ = static_cast<unsigned>(dcmax-dcmin);
  model_rows_ = static_cast<unsigned>(drmax-drmin);
}

// find the distance between the model and the search champher
// at the given search column and search row
double bwm_reg_matcher::total_distance(int tc, int tr)
{
  double d = 0;
  vcl_vector<vsol_digital_curve_2d_sptr>::iterator cit =
    model_edges_.begin();
  for (; cit != model_edges_.end(); ++cit)
    for (unsigned i = 0; i<(*cit)->size(); ++i)
    {
      vsol_point_2d_sptr p = (*cit)->point(i);
      double c = p->x() + tc, r = p->y() + tr;
      unsigned ic = static_cast<unsigned>(vcl_floor(c)),
               ir = static_cast<unsigned>(vcl_floor(r));
      d += champh_.distance(ic, ir);
    }
  return d;
}

//the simplest possible matcher, brute force

bool bwm_reg_matcher::match(int& tcol, int& trow, double distance_threshold)
{
  tcol = 0; trow = 0;
  if (model_cols_>search_cols_||model_rows_>search_rows_)
    return false;

  int r = model_cols_%2;
  int model_col_center = (model_cols_-r)/2;
  r = search_cols_%2;
  int search_col_center = (search_rows_-r)/2;
  int tcol_start = model_col_center-search_col_center + 1;
  int tcol_end = -tcol_start;

  r = model_rows_%2;
  int model_row_center = (model_rows_-r)/2;
  r = search_rows_%2;
  int search_row_center = (search_rows_-r)/2;
  int trow_start = model_row_center-search_row_center + 1;
  int trow_end = -trow_start;

  double min_distance = vnl_numeric_traits<double>::maxval;

  for (int tr = trow_start; tr<=trow_end; ++tr)
    for (int tc = tcol_start; tc<=tcol_end; ++tc)
    {
      double d = total_distance(tc, tr);
      if (d < min_distance)
      {
        min_distance = d;
        tcol = tc;
        trow = tr;
      }
    }
  if (min_distance>distance_threshold)
    return false;
  return true;
}
