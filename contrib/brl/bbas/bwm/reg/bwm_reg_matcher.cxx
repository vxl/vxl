#include "bwm_reg_matcher.h"
//:
// \file
#include <vcl_cmath.h>
#include <vnl/vnl_numeric_traits.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <bsol/bsol_algs.h>
#define REG_DEBUG
static const double max_dist = 20.0;
static const unsigned dist_bins = 20;

bwm_reg_matcher::
bwm_reg_matcher(vcl_vector<vsol_digital_curve_2d_sptr> const& model_edges,
                unsigned search_col_origin, unsigned search_row_origin,
                unsigned search_cols, unsigned search_rows,
                vcl_vector<vsol_digital_curve_2d_sptr> const& search_edges
               ) :  model_edges_(model_edges), angle_threshold_(0),
                    search_col_origin_(search_col_origin),
                    search_row_origin_(search_row_origin),
                    search_cols_(search_cols), search_rows_(search_rows),
                    search_edges_(search_edges),
                    champh_(bwm_reg_edge_champher(search_col_origin,
                                                  search_row_origin,
                                                  search_cols, search_rows,
                                                  search_edges)),
                    hist_(bsta_histogram<double>(0.0,max_dist,dist_bins)),
                    min_hist_(bsta_histogram<double>(0.0,max_dist,dist_bins))
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
  model_col_origin_ = static_cast<unsigned>(dcmin);
  model_row_origin_ = static_cast<unsigned>(drmin);
  model_cols_ = static_cast<unsigned>(dcmax-dcmin);
  model_rows_ = static_cast<unsigned>(drmax-drmin);
}

// find the distance between the model and the search champher
// at the given search column and search row
void bwm_reg_matcher::distance_histogram(int tc, int tr)
{
  //reset histogram
  for (unsigned i = 0; i<dist_bins; ++i)
    hist_.set_count(i, 0.0);
  unsigned n_edges = 0;
#ifdef REG_DEBUG
  vcl_cout << "Search Position(" << tc << ' ' << tr << ")\n";
#endif
  vcl_vector<vsol_digital_curve_2d_sptr>::iterator cit = model_edges_.begin();
  for (; cit != model_edges_.end(); ++cit)
    for (unsigned i = 0; i<(*cit)->size(); ++i)
    {
      vsol_point_2d_sptr p = (*cit)->point(i);
      n_edges++;
      double dx = 0, dy = 0;
      bsol_algs::tangent(*cit, i, dx, dy);
      double c = p->x() + tc, r = p->y() + tr;
      unsigned ic = static_cast<unsigned>(vcl_floor(c)),
               ir = static_cast<unsigned>(vcl_floor(r));
      double dc = champh_.distance(ic, ir);
      bool dir_match = champh_.match_tangent(ic, ir, dx, dy, angle_threshold_);
      if (!dir_match) {
        hist_.upcount(max_dist, 1.0);
        continue;
      }
      if (dc<=max_dist){
        hist_.upcount(dc, 1.0);
        continue;
      }
      hist_.upcount(max_dist, 1.0);
    }
#ifdef REG_DEBUG
  hist_.print();
#endif
}

//the simplest possible matcher, brute force

bool bwm_reg_matcher::match(int& tcol, int& trow, double distance_threshold,
                            double angle_threshold,
                            double min_probability)
{
  angle_threshold_ = angle_threshold;
  tcol = 0; trow = 0;
  if (model_cols_>search_cols_||model_rows_>search_rows_)
    return false;
  int tcol_start = search_col_origin_ - model_col_origin_;
  int tcol_end = tcol_start + search_cols_ - model_cols_;
  int trow_start = search_row_origin_ - model_row_origin_;
  int trow_end = trow_start + search_rows_ - model_rows_;
  tcol_start +=2;  trow_start += 2;
  tcol_end -= 2;   trow_end -=2;

  //  double min_distance = vnl_numeric_traits<double>::maxval;
  double max_prob = 0.0;
  double delta = hist_.delta();
  for (int tr = trow_start; tr<=trow_end; ++tr)
    for (int tc = tcol_start; tc<=tcol_end; ++tc)
    {
      distance_histogram(tc, tr);

      //compute cumulative probability up to distance threshold
      double p = 0;
      bool done = false;
      for (unsigned i = 0; i<=dist_bins&&!done; i++)
        if (i*delta<=distance_threshold)
          p+=hist_.p(i);
        else done = true;
      //find translation with max probability below threshold
      if (p > max_prob)
      {
        max_prob = p;
        tcol = tc;
        trow = tr;
        min_hist_ = hist_;
      }
    }
  vcl_cout << "Search the range (" << tcol_start << ' ' << trow_start << ")("
           << tcol_end << ' ' << trow_end << ")\n"
           << " Max zero prob " << max_prob << " at (" << tcol << ' ' << trow
           << ")\n";

  vcl_cout << " The histogram of distances\n";
  print_hist();

  vcl_cout << vcl_flush;
  if (max_prob>distance_threshold)
    return false;
  return true;
}

bool bwm_reg_matcher::
close_edges(double filter_distance, double angle_threshold,
            unsigned min_curve_length,
            vcl_vector<vsol_digital_curve_2d_sptr>& close_edges)
{
  close_edges.clear();
  vcl_vector<vsol_digital_curve_2d_sptr>::iterator cit = model_edges_.begin();
  for (; cit != model_edges_.end(); ++cit)
  {
    vcl_vector<vsol_point_2d_sptr> filtered_points;
    for (unsigned i = 0; i<(*cit)->size(); ++i)
    {
      vsol_point_2d_sptr p = (*cit)->point(i);
      double c = p->x() , r = p->y();
      double dx = 0, dy = 0;
      bsol_algs::tangent(*cit, i, dx, dy);
      unsigned ic = static_cast<unsigned>(vcl_floor(c)),
               ir = static_cast<unsigned>(vcl_floor(r));
      double dc = champh_.distance(ic, ir);
      bool dir_match = champh_.match_tangent(ic, ir, dx, dy, angle_threshold);
      if (dir_match&&dc<=filter_distance)
        filtered_points.push_back(p);
    }
    unsigned n = filtered_points.size();
    if (n>=min_curve_length)
    {
      //do not allow gaps between samples that are larger than
      //a multiple of the filter distance. A model curve could meander closer
      //and further away from the other mode target curve
      double gap_factor = 5.0;//The multiple of filter distance
      double gap = gap_factor*filter_distance;//the allowed gap
      vsol_point_2d_sptr p0 = filtered_points[0];
      vcl_vector<vsol_point_2d_sptr> temp;
      unsigned len = 1;
      for (unsigned j = 1; j<n; ++j)
      {
        vsol_point_2d_sptr p1 = filtered_points[j];
        double dx = p1->x()-p0->x(), dy = p1->y()-p0->y();
        double ds = vcl_sqrt(dx*dx + dy*dy);
        temp.push_back(p0);
        p0 = p1;
        len++;
        //if a gap occurs, then output the accumulated curve
        //and clear the accumulator
        if (ds>gap && len>=min_curve_length) {
          close_edges.push_back(new vsol_digital_curve_2d(temp));
          len = 1;
          temp.clear();
          temp.push_back(p0);
        }
      }
      //clean up any residual curve that hasn't exceeded the gap
      if (len>=min_curve_length)
        close_edges.push_back(new vsol_digital_curve_2d(temp));
    }
  }
  if (!close_edges.size())
    return false;
  return true;
}
