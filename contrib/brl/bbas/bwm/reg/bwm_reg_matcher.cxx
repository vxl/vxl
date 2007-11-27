#include "bwm_reg_matcher.h"
//:
// \file
#include <vnl/vnl_numeric_traits.h>
#include <vcl_cmath.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_digital_curve_2d.h>

bwm_reg_matcher::
bwm_reg_matcher(unsigned model_cols, unsigned model_rows,
                vcl_vector<vsol_digital_curve_2d_sptr> const& model_edges,
                unsigned search_cols, unsigned search_rows,
                vcl_vector<vsol_digital_curve_2d_sptr> const& search_edges
               ) : model_cols_(model_cols), model_rows_(model_rows),
                   model_edges_(model_edges), search_cols_(search_cols),
                   search_rows_(search_rows), search_edges_(search_edges),
                   champh_(bwm_reg_edge_champher(search_cols, search_rows,
                                                 search_edges))
{
}

// find the distance between the model and the search champher
// at the given search column and search row
double bwm_reg_matcher::total_distance(unsigned scol, unsigned srow)
{
  double d = 0;
  vcl_vector<vsol_digital_curve_2d_sptr>::iterator cit =
    search_edges_.begin();
  for (; cit != search_edges_.end(); ++cit)
    for (unsigned i = 0; i<(*cit)->size(); ++i)
    {
      vsol_point_2d_sptr p = (*cit)->point(i);
      double c = p->x(), r = p->y();
      if (c<0||r<0) continue;
      if (c>search_cols_-1||r>search_rows_-1) continue;
      unsigned ic = static_cast<unsigned>(vcl_floor(c)),
      ir = static_cast<unsigned>(vcl_floor(r));
      d += champh_.distance(ic, ir);
    }
  return d;
}

//the simplest possible matcher, brute force
bool bwm_reg_matcher::match(unsigned initial_col, unsigned initial_row,
                            unsigned radius,
                            unsigned& search_col, unsigned& search_row
                           )
{
  //Check conditions
  if (2*radius +1 >search_rows_ || 2*radius + 1 > search_cols_)
    return false;

  unsigned r0 = initial_row;
  if (r0<radius)
    r0 = radius;
  if (r0>search_rows_-1-radius)
    r0 = search_rows_-1-radius;

  unsigned c0 = initial_col;
  if (c0<radius)
    c0 = radius;
  if (c0>search_cols_-1-radius)
    c0 = search_cols_-1-radius;
  double min_distance = vnl_numeric_traits<double>::maxval;
  for (unsigned row = r0-radius; row<= r0+radius; ++row)
    for (unsigned col = c0-radius; col<= c0+radius; ++col)
    {
      double d = total_distance(col, row);
      if (d < min_distance)
      {
        min_distance = d;
        search_col = col;
        search_row = row;
      }
    }
  return true;
}
