#include "bmrf_epi_seg.h"
//:
// \file

#include <vcl_cassert.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_numeric_traits.h>
#include <bmrf/bmrf_epi_point.h>

// constructors
bmrf_epi_seg::bmrf_epi_seg()
{
  limits_valid_ = false;
  int_valid_ = false;
  min_index_=0;
  max_index_=0;
  min_alpha_=0;
  max_alpha_=0;
  min_s_=0;
  max_s_=0;
  avg_tan_ang_=0;
  min_tan_ang_=0;
  max_tan_ang_=0;
  avg_left_int_=0;
  avg_right_int_=0;
  left_int_sd_=0;
  right_int_sd_=0;
  min_left_int_=0;
  max_left_int_=0;
  min_right_int_=0;
  max_right_int_=0;
}

bmrf_epi_seg::bmrf_epi_seg(vcl_vector<bmrf_epi_point_sptr> const& points)
{
  for (vcl_vector<bmrf_epi_point_sptr>::const_iterator pit=points.begin();
       pit != points.end(); pit++)
    seg_.push_back(*pit);
  limits_valid_ = false;
  int_valid_ = false;
  min_index_=0;
  max_index_=0;
  min_alpha_=0;
  max_alpha_=0;
  min_s_=0;
  max_s_=0;
  avg_left_int_=0;
  avg_right_int_=0;
  left_int_sd_=0;
  right_int_sd_=0;
  min_left_int_=0;
  max_left_int_=0;
  min_right_int_=0;
  max_right_int_=0;
}


void bmrf_epi_seg::add_point(bmrf_epi_point_sptr ep)
{
  seg_.push_back(ep);
  limits_valid_ = false;
  int_valid_ = false;
}

//============================================================
//: Compute geometric bounds for the segment
//============================================================
void bmrf_epi_seg::compute_limits()
{
  int n = this->n_pts();
  if (!n)
    return;
  double alpha0 = seg_[0]->alpha();
  double alpha_n1 = seg_[n-1]->alpha();
  if (alpha0<alpha_n1)
  {
    min_alpha_ = alpha0;
    max_alpha_ = alpha_n1;
  }
  else
  {
    min_alpha_ = alpha_n1;
    max_alpha_ = alpha0;
  }
  // if we assume linear interpolation then the extrema happen at the knots
  min_s_ = vnl_numeric_traits<double>::maxval;
  max_s_ = 0.0;//epipole is zero on the epipolar line.
  min_tan_ang_ = 360;
  max_tan_ang_ = -360;
  //average angle is found by averaging the equivalent vectors
  double sin_sum = 0, cos_sum = 0;
  double deg_to_rad = vnl_math::pi/180.0;
  for (int i= 0; i<n; i++)
  {
    double s = seg_[i]->s();
    min_s_ = vnl_math_min(min_s_, s);
    max_s_ = vnl_math_max(max_s_, s);
    double ang_deg = seg_[i]->tan_ang();
    double ang_rad = deg_to_rad*ang_deg;
    min_tan_ang_ = vnl_math_min(min_tan_ang_, ang_deg);
    max_tan_ang_ = vnl_math_max(max_tan_ang_, ang_deg);
    sin_sum += vcl_sin(ang_rad);
    cos_sum += vcl_cos(ang_rad);
  }
  avg_tan_ang_ = vcl_atan2(sin_sum, cos_sum)/deg_to_rad;
  limits_valid_=true;
}

// Geometric limits that do lazy evaluation
double bmrf_epi_seg::min_alpha()
{
  if (!limits_valid_)
    this->compute_limits();
  return min_alpha_;
}

double bmrf_epi_seg::max_alpha()
{
  if (!limits_valid_)
    this->compute_limits();
  return max_alpha_;
}

double bmrf_epi_seg::min_s()
{
  if (!limits_valid_)
    this->compute_limits();
  return min_s_;
}

double bmrf_epi_seg::max_s()
{
  if (!limits_valid_)
    this->compute_limits();
  return max_s_;
}

double bmrf_epi_seg::avg_tan_ang()
{
  if (!limits_valid_)
    this->compute_limits();
  return avg_tan_ang_;
}

double bmrf_epi_seg::min_tan_ang()
{
  if (!limits_valid_)
    this->compute_limits();
  return min_tan_ang_;
}

double bmrf_epi_seg::max_tan_ang()
{
  if (!limits_valid_)
    this->compute_limits();
  return max_tan_ang_;
}

//=====================================================================
//: linear interpolation on an interval
//=====================================================================
static double linear_interpolate(double xm, double xp, double ym,
                                 double yp, double x)
{
  double dx = xp-xm;
  double dy = yp-ym;
  if (!dx)
    return ym;
  double xi = (dy/dx)*(x-xm) + ym;
  return xi;
}

// Linearly interpolated segment properties

//: Distance from the epipole
double bmrf_epi_seg::s(double alpha)
{
  assert(alpha >= this->min_alpha() && alpha <= this->max_alpha());
  double last_a = this->min_alpha();
  int n = this->n_pts();
  for (int i = 0; i<n; i++)
  {
    double a = seg_[i]->alpha();
    if (a<alpha)
    {
      last_a = a;
      continue;
    }
    else if (a==alpha)
      return seg_[i]->s();

    double s = linear_interpolate(last_a, a, seg_[i-1]->s(),
                                  seg_[i]->s(), alpha);
    return s;
  }
  return 0;
}

//: Tangent angle in image coordinates
double bmrf_epi_seg::tan_ang(double alpha)
{
  assert(alpha >= this->min_alpha() && alpha <= this->max_alpha());
  double last_a = this->min_alpha();
  int n = this->n_pts();
  for (int i = 0; i<n; i++)
  {
    double a = seg_[i]->alpha();
    if (a<alpha)
    {
      last_a = a;
      continue;
    }
    else if (a==alpha)
      return seg_[i]->tan_ang();

    double ta = linear_interpolate(last_a, a,
                                   seg_[i-1]->tan_ang(),
                                   seg_[i]->tan_ang(), alpha);
    return ta;
  }
  return 0;
}


//Add an intensity sample
void bmrf_epi_seg::add_int_sample(const double alpha,
                                  const double left_ds,
                                  const double left_int,
                                  const double right_ds,
                                  const double right_int)
{
  int_alpha_.push_back(alpha);
  left_ds_.push_back(left_ds);
  left_int_.push_back(left_int);
  right_ds_.push_back(right_ds);
  right_int_.push_back(right_int);
  int_valid_ = false;
}

//Linearly interpolate left interval length
double bmrf_epi_seg::left_ds(double alpha)
{
  assert(alpha >= this->min_alpha() && alpha <= this->max_alpha());
  double last_a = this->min_alpha();
  int n = int_alpha_.size();
  for (int i = 0; i<n; i++)
  {
    double a = int_alpha_[i];
    if (a<alpha)
    {
      last_a = a;
      continue;
    }
    else if (a==alpha)
      return left_ds_[i];
    double lds = linear_interpolate(last_a, a,
                                    left_ds_[i-1],
                                    left_ds_[i], alpha);
    return lds;
  }

  return -1;
}


//Linearly interpolate left intensity
double bmrf_epi_seg::left_int(double alpha)
{
  assert(alpha >= this->min_alpha() && alpha <= this->max_alpha());
  double last_a = this->min_alpha();
  int n = int_alpha_.size();
  for (int i = 0; i<n; i++)
  {
    double a = int_alpha_[i];
    if (a<alpha)
    {
      last_a = a;
      continue;
    }
    else if (a==alpha)
      return left_int_[i];
    double li = linear_interpolate(last_a, a,
                                   left_int_[i-1],
                                   left_int_[i], alpha);
    return li;
  }

  return -1;
}

//Linearly interpolate left interval length
double bmrf_epi_seg::right_ds(double alpha)
{
  assert(alpha >= this->min_alpha() && alpha <= this->max_alpha());
  double last_a = this->min_alpha();
  int n = int_alpha_.size();
  for (int i = 0; i<n; i++)
  {
    double a = int_alpha_[i];
    if (a<alpha)
    {
      last_a = a;
      continue;
    }
    else if (a==alpha)
      return right_ds_[i];
    double rds = linear_interpolate(last_a, a,
                                    right_ds_[i-1],
                                    right_ds_[i], alpha);
    return rds;
  }

  return -1;
}

//Linearly interpolate right intensity
double bmrf_epi_seg::right_int(double alpha)
{
  assert(alpha >= this->min_alpha() && alpha <= this->max_alpha());
  double last_a = this->min_alpha();
  int n = int_alpha_.size();
  for (int i = 0; i<n; i++)
  {
    double a = int_alpha_[i];
    if (a<alpha)
    {
      last_a = a;
      continue;
    }
    else if (a==alpha)
      return right_int_[i];
    double ri = linear_interpolate(last_a, a,
                                   right_int_[i-1],
                                   right_int_[i], alpha);
    return ri;
  }
  return -1;
}


//Compute extremal intensity values and intensity statistics
void bmrf_epi_seg::compute_int_values()
{
  int n = int_alpha_.size();
  if (!n)
    return;
  min_left_int_ = vnl_numeric_traits<double>::maxval;
  max_left_int_ = 0.0;
  min_right_int_ = vnl_numeric_traits<double>::maxval;
  max_right_int_ = 0.0;
  double sum_li = 0, sum_li2=0;
  double sum_ri = 0, sum_ri2=0;
  for (int i= 0; i<n; i++)
  {
    //max, min limits
    double left_int = left_int_[i], right_int = right_int_[i];
    min_left_int_ = vnl_math_min(min_left_int_, left_int);
    max_left_int_ = vnl_math_max(max_left_int_, left_int);
    min_right_int_ = vnl_math_min(min_right_int_, right_int);
    max_right_int_ = vnl_math_max(max_right_int_, right_int);
    //statistics
    sum_li += left_int; sum_li2 += left_int*left_int;
    sum_ri += right_int; sum_ri2 += right_int*right_int;
  }
  avg_left_int_ = sum_li/n;
  left_int_sd_ = sum_li2/n - avg_left_int_*avg_left_int_;
  left_int_sd_ = vcl_sqrt(left_int_sd_);
  avg_right_int_ = sum_ri/n;
  right_int_sd_ = sum_ri2/n - avg_right_int_*avg_right_int_;
  right_int_sd_ = vcl_sqrt(right_int_sd_);
  int_valid_ = true;
}


//Intensity attributes with lazy evaluation

double bmrf_epi_seg::avg_left_int()
{
  if (!int_valid_)
    compute_int_values();
  return avg_left_int_;
}

double bmrf_epi_seg::avg_right_int()
{
  if (!int_valid_)
    compute_int_values();
  return avg_right_int_;
}

double bmrf_epi_seg::left_int_sd()
{
  if (!int_valid_)
    compute_int_values();
  return left_int_sd_;
}

double bmrf_epi_seg::right_int_sd()
{
  if (!int_valid_)
    compute_int_values();
  return right_int_sd_;
}

double bmrf_epi_seg::min_left_int()
{
  if (!int_valid_)
    compute_int_values();
  return min_left_int_;
}

double bmrf_epi_seg::max_left_int()
{
  if (!int_valid_)
    compute_int_values();
  return max_left_int_;
}

double bmrf_epi_seg::min_right_int()
{
  if (!int_valid_)
    compute_int_values();
  return min_right_int_;
}

double bmrf_epi_seg::max_right_int()
{
  if (!int_valid_)
    compute_int_values();
  return max_right_int_;
}
#if 0
//=======================================================================
//: compute the normalized difference in tangent angle for two segments
//=======================================================================
double bmrf_epi_seg::tan_ang_match(const double a,
                                   bmrf_epi_seg_sptr const& sa,
                                   bmrf_epi_seg_sptr const& sb,
                                   const double ang_sd)
{
  if (!sa || !sb || !ang_sd)
    return 1e8;
  double ang_a = sa->tan_ang(a), ang_b = sb->tan_ang(a);
  double da = vcl_fabs(ang_a-ang_b);
  //see if the angles were near the cut
  double da_360 = vcl_fabs(da-360);
  double d = da;
  if (da_360<da)
    d = da_360;
  return d/ang_sd;
}

//=======================================================================
//: compute the normalized difference in left intensity
//=======================================================================
double bmrf_epi_seg::left_int_match(const double a,
                                    bmrf_epi_seg_sptr const& sa,
                                    bmrf_epi_seg_sptr const& sb)
{
  if (!sa || !sb)
    return 1e8;
  double lia = sa->left_int(a), lib = sb->left_int(a);
  if (lia<0 || lib<0)
    return -1;
  double lia_sd = sa->left_int_sd(), lib_sd = sb->left_int_sd();
  double sd = vnl_math_min(lia_sd, lib_sd);
#ifdef DEBUG
  vcl_cout << "left_int:(" << lia << ' ' << lib << " /" << sd << ")\n";
#endif
  if (!sd)
    return 1e8;
  return vcl_fabs(lia-lib)/sd;
}

//=======================================================================
//: compute the normalized difference in right intensity
//=======================================================================
double bmrf_epi_seg::right_int_match(const double a,
                                     bmrf_epi_seg_sptr const& sa,
                                     bmrf_epi_seg_sptr const& sb)
{
  if (!sa || !sb)
    return 1e8;
  double ria = sa->right_int(a), rib = sb->right_int(a);
  if (ria<0 || rib<0)
    return -1;
  double ria_sd = sa->right_int_sd(), rib_sd = sb->right_int_sd();
  double sd = vnl_math_min(ria_sd, rib_sd);
#ifdef DEBUG
  vcl_cout << "right_int:(" << ria << ' ' << rib << " /" << sd << ")\n";
#endif
  if (!sd)
    return 1e8;
  return vcl_fabs(ria-rib)/sd;
}

//=======================================================================
//: compute the velocity parameter, gamma for two matched segments
//=======================================================================
double bmrf_epi_seg::velocity_coef(const double a,
                                   bmrf_epi_seg_sptr const& sa,
                                   bmrf_epi_seg_sptr const& sb)
{
  if (!sa || !sb)
    return 1e8;
  double s_a = sa->s(a), s_b = sb->s(a);
  double sum = 0.5*(s_a + s_b);//is average s the best?
  if (!sum)
    return 0;
  return vcl_fabs(s_a-s_b)/sum;
}

//=======================================================================
//: compute the complete match value for two segments at a given alpha
//=======================================================================
double bmrf_epi_seg::match(const double a,
                           bmrf_epi_seg_sptr const& sa,
                           bmrf_epi_seg_sptr const& sb)
{
  vcl_cout << sa << ' ' << sb <<'\n';
  double tam = tan_ang_match(a, sa, sb);
  double lim = left_int_match(a, sa, sb);
  double rim = right_int_match(a, sa, sb);
  if (lim<0 || rim<0)
    return -1;//erroneous match
  double v = velocity_coef(a, sa, sb);
  double tot = tam+lim+rim;
  vcl_cout << "M|" << tot << "|(" << a << ' ' << sa->s(a) << ' ' << sb->s(a)
           << "):[" << tam << ' ' << lim << ' ' << rim << "]->" << v
           << '\n' << vcl_flush;
  return tot;
}
#endif

//: Binary save self to stream.
void bmrf_epi_seg::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());

  int n = seg_.size();
  vsl_b_write(os, n);
  for (int i = 0; i<n; i++)
    seg_[i]->b_write(os);

  int m = int_alpha_.size();
  vsl_b_write(os, m);
  for (int i = 0; i<m; i++)
  {
    vsl_b_write(os, int_alpha_[i]);
    vsl_b_write(os, left_ds_[i]);
    vsl_b_write(os, left_int_[i]);
    vsl_b_write(os, right_ds_[i]);
    vsl_b_write(os, right_int_[i]);
  }
}

//: Binary load self from stream.
void bmrf_epi_seg::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
   case 1:
    {
      int n=0, m=0;
      vsl_b_read(is, n);
      seg_.clear();
      for (int i=0; i<n; i++)
      {
        bmrf_epi_point_sptr ep = new bmrf_epi_point();
        ep->b_read(is);
        this->seg_.push_back(ep);
      }
      vsl_b_read(is, m);
      this->int_alpha_.resize(m);
      this->left_ds_.resize(m);
      this->left_int_.resize(m);
      this->right_ds_.resize(m);
      this->right_int_.resize(m);
      for (int i=0; i<m; i++)
      {
        vsl_b_read(is, this->int_alpha_[i]);
        vsl_b_read(is, this->left_ds_[i]);
        vsl_b_read(is, this->left_int_[i]);
        vsl_b_read(is, this->right_ds_[i]);
        vsl_b_read(is, this->right_int_[i]);
      }
      break;
    }
   default:
    assert(!"unknown version");
  }
}
//: Return IO version number;
short bmrf_epi_seg::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void bmrf_epi_seg::print_summary(vcl_ostream &os) const
{
  os << *this;
}

//: Return a platform independent string identifying the class
vcl_string bmrf_epi_seg::is_a() const
{
  return vcl_string("bmrf_epi_seg");
}

//: Return true if the argument matches the string identifying the class or any parent class
bool bmrf_epi_seg::is_class(const vcl_string& cls) const
{
  return cls==bmrf_epi_seg::is_a();
}

vcl_ostream&  operator<<(vcl_ostream& s, bmrf_epi_seg const& epi_seg)
{
  int n = epi_seg.n_pts();
  bmrf_epi_seg& es = const_cast<bmrf_epi_seg &>(epi_seg);//cast away const
  s << "Epi Segment[" << n <<"]\n"
    << "alpha:[" << es.min_alpha() << ' ' << es.max_alpha() << "]\n"
    << "s:[" << es.min_s() << ' ' << es.max_s() << "]\n"
    << "tan_ang:[" << es.min_tan_ang() << ' ' << es.avg_tan_ang()
    << ' ' << es.max_tan_ang() << "]\n"
    << "left_int:[" << es.min_left_int() << ' ' << es.avg_left_int()
    << ' ' << es.max_left_int() << "]("<< es.left_int_sd()<< ")\n"
    << "right_int:[" << es.min_right_int() << ' ' << es.avg_right_int()
    << ' ' << es.max_right_int() << "]("<< es.right_int_sd()<< ")\n";
  return s;
}

//: Binary save bmrf_epi_seg to stream.
void
vsl_b_write(vsl_b_ostream &os, bmrf_epi_seg_sptr const& eps)
{
  if (!eps){
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    eps->b_write(os);
  }
}


//: Binary load bmrf_epi_seg from stream.
void
vsl_b_read(vsl_b_istream &is, bmrf_epi_seg_sptr& eps)
{
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr){
    eps = new bmrf_epi_seg();
    eps->b_read(is);
  }
  else
    eps = 0;
}

