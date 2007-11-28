#include "bwm_reg_matcher.h"
//:
// \file
#include <vnl/vnl_numeric_traits.h>
#include <vcl_cmath.h>
#include <vnl/vnl_numeric_traits.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_digital_curve_2d.h>

bwm_reg_matcher::
bwm_reg_matcher(vcl_vector<vsol_digital_curve_2d_sptr> const& model_edges,
                unsigned search_cols, unsigned search_rows,
                vcl_vector<vsol_digital_curve_2d_sptr> const& search_edges
                ) :  search_cols_(search_cols), search_rows_(search_rows),
                     search_edges_(search_edges),
                     champh_(bwm_reg_edge_champher(search_cols, search_rows,
                                                   search_edges))
{
  //get the bounds on the model edges
  double dcmin = vnl_numeric_traits<double>::maxval, dcmax = 0;
  double drmin = dcmin, drmax = 0;
  vcl_vector<vsol_digital_curve_2d_sptr>::const_iterator cit = 
    model_edges.begin();
  for(; cit != model_edges.end(); ++cit)
    for(unsigned i = 0; i<(*cit)->size(); ++i)
      {
        vsol_point_2d_sptr p = (*cit)->point(i);
        double c = p->x(), r = p->y();
        if(c<dcmin) dcmin = c;
        if(c>dcmax) dcmax = c;
        if(r<drmin) drmin = r;
        if(r>drmax) drmax = r;
      }
  cit = model_edges.begin();
  for(; cit != model_edges.end(); ++cit)
    {
      vsol_digital_curve_2d_sptr dc = new vsol_digital_curve_2d();
      for(unsigned i = 0; i<(*cit)->size(); ++i)
        {
          vsol_point_2d_sptr p = (*cit)->point(i);
          double c = p->x(), r = p->y();
          dc->add_vertex(new vsol_point_2d(c-dcmin, r-drmin));
        }
      model_edges_.push_back(dc);
    }
  model_cols_ = static_cast<unsigned>(dcmax-dcmin);
  model_rows_ = static_cast<unsigned>(drmax-drmin);
}

// find the distance between the model and the search champher
// at the given search column and search row
double bwm_reg_matcher::total_distance(unsigned tc, unsigned tr)
{
  double d = 0;
  vcl_vector<vsol_digital_curve_2d_sptr>::iterator cit = 
    model_edges_.begin();
  for(; cit != model_edges_.end(); ++cit)
    for(unsigned i = 0; i<(*cit)->size(); ++i)
      {
        vsol_point_2d_sptr p = (*cit)->point(i);
        double c = p->x(), r = p->y();
        if(c<0||r<0) continue;
        if(c>search_cols_-1||r>search_rows_-1) continue;
        unsigned ic = static_cast<unsigned>(vcl_floor(c)),
          ir = static_cast<unsigned>(vcl_floor(r));
        d += champh_.distance(ic+tc, ir+tr);
      }
  return d;
}

//the simplest possible matcher, brute force

bool bwm_reg_matcher::match(unsigned& tcol, unsigned& trow)
{
  tcol = 0; trow = 0;
  if(model_cols_>search_cols_||model_rows_>search_rows_)
    return false;
  unsigned n_row = search_rows_-model_rows_;
  unsigned n_col = search_cols_-model_cols_;
  double min_distance = vnl_numeric_traits<double>::maxval;

  for(unsigned tr = 0; tr<=n_row; ++tr)
    for(unsigned tc = 0; tc<=n_col; ++tc)
      {
        double d = total_distance(tc, tr);
        if(d < min_distance)
          {
            min_distance = d;
            tcol = tc;
            trow = tr;
          }
      }
  
  return true;
}
