//---*-c++-*--

#include <vcl_cassert.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_numeric_traits.h>
#include <strk/strk_epi_point.h>
#include <strk/strk_epi_seg.h>

//:constructors
strk_epi_seg::strk_epi_seg()
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

strk_epi_seg::strk_epi_seg(vcl_vector<strk_epi_point_sptr> const& points)
{
  for(vcl_vector<strk_epi_point_sptr>::const_iterator pit=points.begin();
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


void strk_epi_seg::add_point(strk_epi_point_sptr ep)
{
  seg_.push_back(ep);
  limits_valid_ = false;
}

//============================================================
//: Compute geometric bounds for the segment
//============================================================
void strk_epi_seg::compute_limits()
{
  int n = this->n_pts();
  if(!n)
    return;
  double alpha0 = seg_[0]->alpha();
  double alpha_n1 = seg_[n-1]->alpha();
  if(alpha0<alpha_n1)
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
  for(int i= 0; i<n; i++)
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
double strk_epi_seg::min_alpha()
{

  if(!limits_valid_)
    this->compute_limits();
  return min_alpha_;

}


double strk_epi_seg::max_alpha()
{

  if(!limits_valid_)
    this->compute_limits();
  return max_alpha_;
}


double strk_epi_seg::min_s()
{

  if(!limits_valid_)
    this->compute_limits();
  return min_s_;

}


double strk_epi_seg::max_s()
{
  if(!limits_valid_)
    this->compute_limits();
  return max_s_;
}

double strk_epi_seg::avg_tan_ang()
{
  if(!limits_valid_)
    this->compute_limits();
  return avg_tan_ang_;
}

double strk_epi_seg::min_tan_ang()
{
  if(!limits_valid_)
    this->compute_limits();
  return min_tan_ang_;
}
double strk_epi_seg::max_tan_ang()
{
  if(!limits_valid_)
    this->compute_limits();
  return max_tan_ang_;
}
//=====================================================================
//linear interpolation on an interval
//=====================================================================
static double linear_interpolate(double xm, double xp, double ym,
                                 double yp, double x)
{
  double dx = xp-xm;
  double dy = yp-ym;
  if(!dx)
    return ym;
  double xi = (dy/dx)*(x-xm) + ym;
  return xi;
}

// Linearly interpolated segment properties
//: Distance from the epipole
double strk_epi_seg::s(double alpha)
{
  assert(alpha >= this->min_alpha() && alpha <= this->max_alpha());
  double last_a;
  int n = this->n_pts();
  for(int i = 0; i<n; i++)
    {
      double a = seg_[i]->alpha();
      if(a<alpha)
        {
          last_a = a;
          continue;
        }
      if(a==alpha)
        return seg_[i]->s();

      double s = linear_interpolate(last_a, a, seg_[i-1]->s(),
                                    seg_[i]->s(), alpha);
      return s;
    }
 return 0;
}

//: Tangent angle in image coordinates
double strk_epi_seg::tan_ang(double alpha)
{

  assert(alpha >= this->min_alpha() && alpha <= this->max_alpha());
  double last_a;
  int n = this->n_pts();
  for(int i = 0; i<n; i++)
    {
      double a = seg_[i]->alpha();
      if(a<alpha)
        {
          last_a = a;
          continue;
        }
      if(a==alpha)
        return seg_[i]->tan_ang();

      double ta = linear_interpolate(last_a, a,
                                     seg_[i-1]->tan_ang(),
                                     seg_[i]->tan_ang(), alpha);
      return ta;
    }
  return 0;
}


//Add an intensity sample 
void strk_epi_seg::add_int_sample(const double alpha, 
                                   const double left_int,
                                   const double right_int)
{

   int_alpha_.push_back(alpha);
   left_int_.push_back(left_int);
   right_int_.push_back(right_int);
  int_valid_ = false;
}


//Linearly interpolate left intensity
double strk_epi_seg::left_int(double alpha)
{

  assert(alpha >= this->min_alpha() && alpha <= this->max_alpha());
  double last_a;
  int n = int_alpha_.size();
  for(int i = 0; i<n; i++)
    {
      double a = int_alpha_[i];
      if(a<alpha)
        {
          last_a = a;
          continue;
        }
      if(a==alpha)
        return left_int_[i];
      double li = linear_interpolate(last_a, a,
                                    left_int_[i-1],
                                    left_int_[i], alpha);
      return li;
    }

  return -1;
}

//Linearly interpolate right intensity
double strk_epi_seg::right_int(double alpha)
{

  assert(alpha >= this->min_alpha() && alpha <= this->max_alpha());
  double last_a;
  int n = int_alpha_.size();
  for(int i = 0; i<n; i++)
    {
      double a = int_alpha_[i];
      if(a<alpha)
        {
          last_a = a;
          continue;
        }
      if(a==alpha)
        return right_int_[i];
      double ri = linear_interpolate(last_a, a,
                                    right_int_[i-1],
                                    right_int_[i], alpha);
      return ri;
    }
  return -1;
}


//Compute extremal intensity values and intensity statistics
void strk_epi_seg::compute_int_values()
{
  int n = int_alpha_.size();
  if(!n)
    return;
  min_left_int_ = vnl_numeric_traits<double>::maxval;
  max_left_int_ = 0.0;
  min_right_int_ = vnl_numeric_traits<double>::maxval;
  max_right_int_ = 0.0;
  double sum_li = 0, sum_li2=0;
  double sum_ri = 0, sum_ri2=0;
  for(int i= 0; i<n; i++)
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
double strk_epi_seg::avg_left_int()
{
  if(!int_valid_)
    compute_int_values();
  return avg_left_int_;
}


double strk_epi_seg::avg_right_int()
{
  if(!int_valid_)
    compute_int_values();
  return avg_right_int_;
}


double strk_epi_seg::left_int_sd()
{
  if(!int_valid_)
    compute_int_values();
  return left_int_sd_;
}


double strk_epi_seg::right_int_sd()
{
  if(!int_valid_)
    compute_int_values();
  return right_int_sd_;
}


double strk_epi_seg::min_left_int()
{

  if(!int_valid_)
    compute_int_values();
  return min_left_int_;
}



double strk_epi_seg::max_left_int()
{

  if(!int_valid_)
    compute_int_values();
  return max_left_int_;
}



double strk_epi_seg::min_right_int()
{

  if(!int_valid_)
    compute_int_values();
  return min_right_int_;
}



double strk_epi_seg::max_right_int()
{

  if(!int_valid_)
    compute_int_values();
  return max_right_int_;
}
//=======================================================================
//: compute the normalized difference in tangent angle for two segments
//=======================================================================
double strk_epi_seg::tan_ang_match(const double a,
                                   strk_epi_seg_sptr const& sa, 
                                   strk_epi_seg_sptr const& sb, 
                                   const double ang_sd) 
{
  if(!sa||!sb||!ang_sd)
    return 1e8;
  double ang_a = sa->tan_ang(a), ang_b = sb->tan_ang(a);
  double da = vcl_fabs(ang_a-ang_b);
  //see if the angles were near the cut
  double da_360 = vcl_fabs(da-360);
  double d = da;
  if(da_360<da)
    d = da_360;
  return d/ang_sd;
}
//=======================================================================
//: compute the normalized difference in left intensity
//=======================================================================
double strk_epi_seg::left_int_match(const double a, 
                                        strk_epi_seg_sptr const& sa, 
                                        strk_epi_seg_sptr const& sb) 
{
  if(!sa||!sb)
    return 1e8;
  double lia = sa->left_int(a), lib = sb->left_int(a);
  if(lia<0||lib<0)
    return -1;
  double lia_sd = sa->left_int_sd(), lib_sd = sb->left_int_sd();
  double sd = vnl_math_min(lia_sd, lib_sd);
  //  vcl_cout << "left_int:(" << lia << " " << lib << " /" << sd << ")\n";
  if(!sd)
    return 1e8;
  return vcl_fabs(lia-lib)/sd;
}
//=======================================================================
//: compute the normalized difference in right intensity
//=======================================================================
double strk_epi_seg::right_int_match(const double a,
                                     strk_epi_seg_sptr const& sa, 
                                     strk_epi_seg_sptr const& sb) 
{
  if(!sa||!sb)
    return 1e8;
  double ria = sa->right_int(a), rib = sb->right_int(a);
  if(ria<0||rib<0)
    return -1;
  double ria_sd = sa->right_int_sd(), rib_sd = sb->right_int_sd();
  double sd = vnl_math_min(ria_sd, rib_sd);
  //  vcl_cout << "right_int:(" << ria << " " << rib << " /" << sd << ")\n";
  if(!sd)
    return 1e8;
  return vcl_fabs(ria-rib)/sd;
}
//=======================================================================
//: compute the velocity parameter, gamma for two matched segments
//=======================================================================
double strk_epi_seg::velocity_coef(const double a, 
                                   strk_epi_seg_sptr const& sa, 
                                   strk_epi_seg_sptr const& sb)
{
  if(!sa||!sb)
    return 1e8;
  double s_a = sa->s(a), s_b = sb->s(a);
  double sum = 0.5*(s_a + s_b);//is average s the best?
  if(!sum)
    return 0;
  return vcl_fabs(s_a-s_b)/sum;
}

//=======================================================================
//: compute the complete match value for two segments at a given alpha
//=======================================================================
double strk_epi_seg::match(const double a, 
                           strk_epi_seg_sptr const& sa, 
                           strk_epi_seg_sptr const& sb)
{
  vcl_cout << sa << " " << sb <<"\n";
  double tam = tan_ang_match(a, sa, sb);
  double lim = left_int_match(a, sa, sb);
  double rim = right_int_match(a, sa, sb);
  if(lim<0||rim<0)
    return -1;//erroneous match
  double v = velocity_coef(a, sa, sb);
  double tot = tam+lim+rim;
  vcl_cout << "M|" << tot << "|(" << a << " " << sa->s(a) << " " << sb->s(a) <<"):[" 
           << tam << " " << lim << " " << rim << "]->" << v 
           << "\n" << vcl_flush;
  return tot;
}

vcl_ostream&  operator<<(vcl_ostream& s, strk_epi_seg const& epi_seg)
{
  int n = epi_seg.n_pts();
  strk_epi_seg& es = (strk_epi_seg)epi_seg;//cast away const
  s << "Epi Segment[" << n <<"]\n";
  s << "alpha:[" << es.min_alpha() << "" << es.max_alpha() << "]\n";
  s << "s:[" << es.min_s() << " " << es.max_s() << "]\n";
  s << "tan_ang:[" << es.min_tan_ang() << " " << es.avg_tan_ang() 
    << " " << es.max_tan_ang() << "]\n";
  s << "left_int:[" << es.min_left_int() << " " << es.avg_left_int() 
    << " " << es.max_left_int() << "]("<< es.left_int_sd()<< ")\n";
  s << "right_int:[" << es.min_right_int() << " " << es.avg_right_int() 
    << " " << es.max_right_int() << "]("<< es.right_int_sd()<< ")\n";
  return s;
}



