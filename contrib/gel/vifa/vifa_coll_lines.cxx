// This is gel/vifa/vifa_coll_lines.cxx
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <vsol/vsol_point_2d.h>
#include <vtol/vtol_face.h>
#include <vifa/vifa_coll_lines.h>


// Static initialization
int  vifa_coll_lines::serial_num_ = 0;

vifa_coll_lines::vifa_coll_lines(vtol_edge_2d_sptr  e,
                                 double             cutoff_angle_deg,
                                 double             endpt_distance,
                                 bool               discard_flag)
{
  const vgl_point_2d<double>&  p1 = e->curve()->p0()->get_p();
  const vgl_point_2d<double>&  p2 = e->curve()->p1()->get_p();

  hypothesized_line_ = new imp_line(p1, p2);
  contributors_.push_back(e);
  projected_length_cutoff_ = vcl_cos(cutoff_angle_deg * vnl_math::pi / 180.0);
  endpt_distance_ = endpt_distance;
  id_ = vifa_coll_lines::serial_num_++;
  discard_flag_ = discard_flag;
}

vifa_coll_lines::~vifa_coll_lines(void)
{
  hypothesized_line_ = 0;
}

bool vifa_coll_lines::get_discard_flag(void) const
{
  return discard_flag_;
}

int vifa_coll_lines::get_id(void) const
{
  return id_;
}

double vifa_coll_lines::get_measure(const vtol_edge_2d&  e) const
{
  return get_measure(e, *hypothesized_line_);
}

double vifa_coll_lines::get_projected_length(const vtol_edge_2d&  e) const
{
  double  v1;
  double  v2;

  return vifa_coll_lines::get_projected_length(e,
                                               *hypothesized_line_,
                                               v1,
                                               v2);
}

edge_2d_list& vifa_coll_lines::get_contributors(void)
{
  return contributors_;
}

face_list* vifa_coll_lines::get_contributor_faces(void)
{
  face_list*  ret = new face_list;

  for (edge_2d_iterator e = contributors_.begin();
      e != contributors_.end(); ++e)
  {
    face_list faces; (*e)->faces(faces);

    for (face_iterator f_it = faces.begin(); f_it != faces.end(); ++f_it)
    {
      vtol_face_sptr  nbr_face = *f_it;
      bool      add_me = true;

      // Make sure the contributor face is 2-D
      if (nbr_face->cast_to_face_2d())
      {
        for (face_iterator f = ret->begin(); f != ret->end(); ++f)
        {
          if (**f == *nbr_face)
          {
            add_me = false;
            break;
          }
        }

        if (add_me)
        {
          ret->push_back(nbr_face);
        }
      }
    }
  }

  return ret;
}

void vifa_coll_lines::lms_fit(const vcl_vector<double>&  x,
                              const vcl_vector<double>&  y,
                              double&                    A,
                              double&                    B,
                              double&                    C)
{
  double  sum_x_sq = 0.0;
  double  sum_x = 0.0;
  double  sum_xy = 0.0;
  double  sum_y = 0.0;
  double  n = 0.0;

  vcl_vector<double>::const_iterator  xi = x.begin();
  vcl_vector<double>::const_iterator  yi = y.begin();
  for (; xi != x.end(); ++xi, ++yi)
  {
    sum_x_sq += (*xi * (*xi));
    sum_x += *xi;
    sum_y += *yi;
    sum_xy += *xi * (*yi);
    n++;
  }

  double  f1 = n - ((sum_x * sum_x) / sum_x_sq);
  double  f2 = sum_y - (sum_x * sum_xy / sum_x_sq);
  double  b = f2 / f1;
  double  m = (sum_xy - (sum_x * b)) / sum_x_sq;

  A = m;
  C = b;
  B = -1;
}

void vifa_coll_lines::add_and_update(vtol_edge_2d_sptr  e)
{
  contributors_.push_back(e);
  this->fit_line();
}

double vifa_coll_lines::spanning_length(void)
{
  vgl_point_2d<double> p1;
  vgl_point_2d<double> p2;

  return spanning_length(p1, p2);
}

double vifa_coll_lines::spanning_length(vgl_point_2d<double>&  p1,
                                        vgl_point_2d<double>&  p2)
{
  double  min_x=0.0, min_y=0.0, min_d;
  double  max_x=0.0, max_y=0.0, max_d= -1.0;

  for (edge_2d_iterator e = contributors_.begin();
       e != contributors_.end(); ++e)
  {
    const vgl_point_2d<double>&  v1 = (*e)->curve()->p0()->get_p();
    const vgl_point_2d<double>&  v2 = (*e)->curve()->p1()->get_p();

    for (int i = 0; i < 2; i++)
    {
      const vgl_point_2d<double>&  v = (i == 0) ? v1: v2;
      double            x;
      double            y;

      hypothesized_line_->project_2d_pt(v.x(), v.y(), x, y);

      //vcl_cout << "    ---> " << v.x() << ", " << v.y() <<
      //  " projects to " << x << ", " << y << vcl_endl;

      double  d = vcl_sqrt((x * x) + (y * y));
      if (d > max_d)
      {
        max_d = d;
        max_x = x;
        max_y = y;
      }
    }
  }

  //vcl_cout << "  -> span: p1 is " << max_x << " , " << max_y << " ( " << max_d << " )\n";

  min_d = max_d;
  for (edge_2d_iterator e = contributors_.begin();
       e != contributors_.end(); ++e)
  {
    const vgl_point_2d<double>&  v1 = (*e)->curve()->p0()->get_p();
    const vgl_point_2d<double>&  v2 = (*e)->curve()->p1()->get_p();

    for (int i = 0; i < 2; i++)
    {
      const vgl_point_2d<double>&  v = (i == 0) ? v1: v2;
      double            x;
      double            y;

      hypothesized_line_->project_2d_pt(v.x(), v.y(), x, y);

      //vcl_cout << "    ---> " << v.x() << ", " << v.y() <<
      //  " projects to " << x << ", " << y << vcl_endl;

      double  dx = x - max_x;
      double  dy = y - max_y;
      double  d = vcl_sqrt((dx * dx) + (dy * dy));

      if (d > min_d)
      {
        min_d = d;
        min_x = x;
        min_y = y;
      }
    }
  }

  //vcl_cout << "  -> span: p2 is " << min_x << " , " << min_y << " ( " << min_d << " )\n";

  double  dx = max_x - min_x;
  double  dy = max_y - min_y;
  p1 = vgl_point_2d<double>(min_x, min_y);
  p2 = vgl_point_2d<double>(max_x, max_y);

  return vcl_sqrt((dx * dx) + (dy * dy));
}

double vifa_coll_lines::support_length(void)
{
  double  len = 0.0;
  for (edge_2d_iterator e = contributors_.begin();
       e != contributors_.end(); ++e)
  {
    len += this->get_projected_length(**e);
  }

  return len;
}

bool vifa_coll_lines::contains(const vtol_edge&  edgeref)
{
  for (edge_2d_iterator e = contributors_.begin();
       e != contributors_.end(); ++e)
  {
    if (**e == edgeref)
    {
      return true;
    }
  }

  return false;
}

// *****************************************************************************
//                                Private API's
// *****************************************************************************

double vifa_coll_lines::get_projected_length(const vtol_edge_2d&  e,
                                             const imp_line&      hyp_line,
                                             double&              v1_dist,
                                             double&              v2_dist)
{
  const vgl_point_2d<double>&  v1 = e.curve()->p0()->get_p();
  const vgl_point_2d<double>&  v2 = e.curve()->p1()->get_p();
  double            x1;
  double            y1;
  double            x2;
  double            y2;

  hyp_line.project_2d_pt(v1.x(), v1.y(), x1, y1);
  hyp_line.project_2d_pt(v2.x(), v2.y(), x2, y2);

  double  dx = x2 - x1;
  double  dy = y2 - y1;
  double  midpt_dist = vcl_sqrt((dx * dx) + (dy * dy));

  dx = x1 - v1.x();
  dy = y1 - v1.y();
  v1_dist = vcl_sqrt((dx * dx) + (dy * dy));

  dx = x2 - v2.x();
  dy = y2 - v2.y();
  v2_dist = vcl_sqrt((dx * dx) + (dy * dy));

  return midpt_dist;
}

double vifa_coll_lines::get_midpt_dist(const vtol_edge_2d&  e,
                                       const imp_line&      hyp_line)
{
  const vgl_point_2d<double>&  v1 = e.curve()->p0()->get_p();
  const vgl_point_2d<double>&  v2 = e.curve()->p1()->get_p();
  double            midx = (v1.x() + v2.x()) / 2;
  double            midy = (v1.y() + v2.y()) / 2;
  double            mx;
  double            my;

  hyp_line.project_2d_pt(midx, midy, mx, my);

  double  mid_dx = mx - midx;
  double  mid_dy = my - midy;
  return vcl_sqrt((mid_dx * mid_dx) + (mid_dy * mid_dy));
}


double vifa_coll_lines::get_measure(const vtol_edge_2d&  e,
                                    const imp_line&      hyp_line)  const
{
  double    v1_dist;
  double    v2_dist;
  double    proj_len = vifa_coll_lines::get_projected_length(e,
                                                             hyp_line,
                                                             v1_dist,
                                                             v2_dist);
  bool    angle_fits = (proj_len / e.curve()->length() >
                        projected_length_cutoff_);
  bool    vertices_are_close = ((v1_dist < endpt_distance_) &&
                                (v2_dist < endpt_distance_));
  double    rv = 100000;

  if (vertices_are_close || angle_fits)
  {
    rv = vifa_coll_lines::get_midpt_dist(e, hyp_line);
  }

  return rv;
}

void vifa_coll_lines::fit_line(void)
{
  vcl_vector<double>  x;
  vcl_vector<double>  y;
  double              A;
  double              B;
  double              C;

  for (edge_2d_iterator e = contributors_.begin();
      e != contributors_.end(); ++e)
  {
    const vgl_point_2d<double>&  v1 = (*e)->curve()->p0()->get_p();
    const vgl_point_2d<double>&  v2 = (*e)->curve()->p1()->get_p();

    x.push_back(v1.x());
    x.push_back(v2.x());
    y.push_back(v1.y());
    y.push_back(v2.y());
  }

  vifa_coll_lines::lms_fit(x, y, A, B, C);

  // Smart pointer will unref previous hypothesized_line_
  hypothesized_line_ = new imp_line(A, B, C);
}
