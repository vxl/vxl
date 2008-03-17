//:
// \file
// \brief Base for classes which locate feature points
// \author Tim Cootes

#include <mfpf/mfpf_point_finder.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_cloneables_factory.h>

#include <vcl_cmath.h>
#include <vimt/algo/vimt_find_troughs.h>
#include <vnl/vnl_cost_function.h>
#include <vnl/algo/vnl_amoeba.h>
#include <vnl/algo/vnl_powell.h>

//=======================================================================
//=== Utility functions for optimisation
//=======================================================================

//: Return position of minima of parabola fit to (-1,fa),(0,fb), (1,fc)
inline double parabolic_min(double fa, double fb, double fc)
{
  double df1=fa-fb;
  double df2=fc-fb;
  double d=df1+df2;
  if (vcl_fabs(d)<1e-6) return 0.0;
  return 0.5*(df1-df2)/d;
}

class mfpf_pf_cost : public vnl_cost_function
{
private:
  mfpf_point_finder* pf_;
  const vimt_image_2d_of<float>& image_;
  vgl_point_2d<double> p_;
  vgl_vector_2d<double> u_;
  vgl_vector_2d<double> v_;
  double ds_,dA_;
public:
  virtual ~mfpf_pf_cost() {};
  mfpf_pf_cost(mfpf_point_finder& pf,
               const vimt_image_2d_of<float>& image,
               const vgl_point_2d<double>& p0,
               const vgl_vector_2d<double>& u0,
               double ds, double dA)
    : vnl_cost_function(4),
      pf_(&pf),image_(image),p_(p0),
      u_(u0),v_(-u_.y(),u_.x()),
      ds_(ds),dA_(dA) {}

    //:  Given the parameter vector x, compute the value of f(x).
  virtual double f(const vnl_vector<double>& params);

  void get_pose(const vnl_vector<double>& params,
                vgl_point_2d<double>& p,
                vgl_vector_2d<double>& u) const;

  //: Checks if parameter index is at minima by displacing by +/-dx
  //  If it is, then uses parabolic fit to attempt to improved it.
  //  fit is f(params).  On exit params contains new 
  //  values, and fit contains new fit at that position.
  bool check_and_refine_minima(vnl_vector<double>& params,
                          double &fit,
                          unsigned index, double dx);
};

double mfpf_pf_cost::f(const vnl_vector<double>& params)
{
  vgl_point_2d<double> p;
  vgl_vector_2d<double> u;
  get_pose(params,p,u);
  return pf_->evaluate(image_,p,u);
}

void mfpf_pf_cost::get_pose(const vnl_vector<double>& v,
                vgl_point_2d<double>& p,
                vgl_vector_2d<double>& u) const
{
  p = p_+pf_->step_size()*(v[0]*u_+v[1]*v_);
  double s = vcl_pow(ds_,v[2]);
  double A = dA_*v[3];
  u = s*(u_*vcl_cos(A)+v_*vcl_sin(A));
}

//: Checks if parameter index is at minima by displacing by +/-dx
//  If it is, then uses parabolic fit to attempt to improved it.
//  fit is f(params).  On exit params contains new 
//  values, and fit contains new fit at that position.
bool mfpf_pf_cost::check_and_refine_minima(vnl_vector<double>& params,
                          double &fit,
                          unsigned index, double dx)
{
  double p0 = params[index];
  params[index]=p0-dx;
  double f1=f(params);
  if (f1<fit) { fit=f1; return false; }

  params[index]=p0+dx;
  double f2=f(params);
  if (f2<fit) { fit=f2; return false; }

  double a = parabolic_min(f1,fit,f2);
  params[index]=p0+a*dx;
  double f3=f(params);
  if (f3<fit) { fit=f3; return true; }

  // Optimisation no better, so revert
  params[index]=p0;
  return true;
}

//=======================================================================
// Dflt ctor
//=======================================================================

mfpf_point_finder::mfpf_point_finder()
  : step_size_(1.0),
    search_ni_(5),search_nj_(0),
    nA_(0),dA_(0.0),ns_(0),ds_(1.0)
{
}

//=======================================================================
// Destructor
//=======================================================================

mfpf_point_finder::~mfpf_point_finder()
{
}

//: Size of step between sample points
void mfpf_point_finder::set_step_size(double s)
{
  step_size_=s;
}

//: Define search region size
//  During search, samples at points on grid [-ni,ni]x[-nj,nj],
//  with axes defined by u.
void mfpf_point_finder::set_search_area(unsigned ni, unsigned nj)
{
  search_ni_=ni;
  search_nj_=nj;
}

//: Define angle search parameters
void mfpf_point_finder::set_angle_range(unsigned nA, double dA)
{
  nA_=nA;
  dA_=dA;
}

//: Define scale search parameters
void mfpf_point_finder::set_scale_range(unsigned ns, double ds)
{
  ns_=ns;
  ds_=ds;
}

//: Search given image around p, using u to define scale and angle 
//  Evaluates responses on a grid, finds the best point on the
//  grid, then optimises its position by fitting a parabola.
//
//  On exit, new_p defines position of the best nearby match.
//  Returns a qualtity of fit measure at that
//  point (the smaller the better).
double mfpf_point_finder::search_one_pose_with_opt(
                      const vimt_image_2d_of<float>& image,
                      const vgl_point_2d<double>& p0,
                      const vgl_vector_2d<double>& u,
                      vgl_point_2d<double>& new_p)
{
  vimt_image_2d_of<double> response_im;
  evaluate_region(image,p0,u,response_im);

  vimt_transform_2d im2w = response_im.world2im().inverse();
  const vil_image_view<double>& r_im =  response_im.image();

  // Find position of lowest point in response image
  unsigned x=0,y=0;
  double f0 = r_im(x,y);
  for (unsigned j=0;j<r_im.nj();++j)
    for (unsigned i=0;i<r_im.ni();++i)
    {
      if (r_im(i,j)<f0) { f0=r_im(i,j); x=i; y=j; }
    }

  // Perform local prediction of minima
  double dx=0.0;
  if (x>0 && x+1<r_im.ni())
    dx = parabolic_min(r_im(x-1,y  ),f0,r_im(x+1,y  ));

  double dy=0.0;
  if (y>0 && y+1<r_im.nj())
    dy = parabolic_min(r_im(x  ,y-1),f0,r_im(x  ,y+1));

  // Check the estimate is better
  new_p=im2w(x+dx,y+dy);

  if (dx==0.0 && dy==0.0) return f0;

  double f1=evaluate(image,new_p,u);
  if (f1<f0) return f1;

  // new_p not an improvement
  new_p=p0;
  return f0;
}


//: Search given image around p, using u to define scale and orientation 
//  On exit, new_p and new_u define position, scale and orientation of 
//  the best nearby match.  Returns a qualtity of fit measure at that
//  point (the smaller the better).
double mfpf_point_finder::search(const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        vgl_point_2d<double>& new_p,
                        vgl_vector_2d<double>& new_u)
{
  if (nA_==0 && ns_==0)
  {
    // Only search at one scale/orientation
    new_u = u;
    return search_one_pose(image,p,u,new_p);
  }

  double best_fit=9e99;
  int best_iA,best_is;
  vgl_point_2d<double> p1;

  vgl_vector_2d<double> v(-u.y(),u.x());

  for (int is=-int(ns_);is<=int(ns_);++is)
  {
    double s = vcl_pow(ds_,is);
    for (int iA=-int(nA_);iA<=int(nA_);++iA)
    {
      double A = iA*dA_;
      vgl_vector_2d<double> uA = s*(u*vcl_cos(A)+v*vcl_sin(A));

      double f = search_one_pose(image,p,uA,p1);
      if (f<best_fit)
      {
        best_iA=iA;
        best_is=is;
        best_fit = f;
        new_u = uA;
        new_p = p1;
      }
    }
  }
  return best_fit;
}

//: Search given image around p, using u to define scale and orientation 
//  On exit, new_p and new_u define position, scale and orientation of 
//  the best nearby match.  Returns a qualtity of fit measure at that
//  point (the smaller the better).
double mfpf_point_finder::search_with_opt(
                        const vimt_image_2d_of<float>& image,
                        const vgl_point_2d<double>& p,
                        const vgl_vector_2d<double>& u,
                        vgl_point_2d<double>& new_p,
                        vgl_vector_2d<double>& new_u)
{
  if (nA_==0 && ns_==0)
  {
    // Only search at one scale/orientation
    new_u = u;
    return search_one_pose_with_opt(image,p,u,new_p);
  }

  double best_fit=9e99;
  int best_iA,best_is;
  vgl_point_2d<double> p1;

  vgl_vector_2d<double> v(-u.y(),u.x());

  for (int is=-int(ns_);is<=int(ns_);++is)
  {
    double s = vcl_pow(ds_,is);
    for (int iA=-int(nA_);iA<=int(nA_);++iA)
    {
      double A = iA*dA_;
      vgl_vector_2d<double> uA = s*(u*vcl_cos(A)+v*vcl_sin(A));

      double f = search_one_pose_with_opt(image,p,uA,p1);
      if (f<best_fit)
      {
        best_iA=iA;
        best_is=is;
        best_fit = f;
        new_u = uA;
        new_p = p1;
      }
    }
  }

  mfpf_pf_cost pf_cost(*this,image,new_p,new_u,1+0.7*(ds_-1),0.7*dA_);
  vnl_vector<double> params(4,0.0);
  // Attempt to optimise scale
  pf_cost.check_and_refine_minima(params,best_fit,2,1.0);
  // Attempt to optimise orientation
  pf_cost.check_and_refine_minima(params,best_fit,3,1.0);

  // Compute actual pose for these optimal parameters
  pf_cost.get_pose(params,new_p,new_u);

  return best_fit;
}


//: Search for local optima around given point/scale/angle
//  Search in a grid around p (defined by search_ni and search_nj).
//  Find local minima on this grid and return them in pts.
void mfpf_point_finder::grid_search_one_pose(
                           const vimt_image_2d_of<float>& image,
                           const vgl_point_2d<double>& p,
                           const vgl_vector_2d<double>& u,
                           vcl_vector<mfpf_pose>& pts,
                          vcl_vector<double>& fit)
{
  vimt_image_2d_of<double> response_im;
  evaluate_region(image,p,u,response_im);

  vcl_vector<vgl_point_2d<unsigned> > t_pts;
  vcl_vector<double> t_value;
  vimt_find_image_troughs_3x3(t_pts,t_value,response_im.image());
  vimt_transform_2d im2w = response_im.world2im().inverse();

  for (unsigned i=0;i<t_pts.size();++i)
  {
    unsigned x =t_pts[i].x(), y=t_pts[i].y();
    fit.push_back(t_value[i]);
    pts.push_back(mfpf_pose(im2w(x,y),u));
  }
}

//: Search for local optima around given point/scale/angle
//  Search in a grid around p (defined by search_ni and search_nj).
//  Find local minima on this grid.
//  Perform single sub-grid optimisation by fitting a parabola
//  in x and y and testing resulting point.
//  Append each to pts.  
//  Note: pts is not resized, so empty beforehand if necessary.
void mfpf_point_finder::multi_search_one_pose(
                          const vimt_image_2d_of<float>& image,
                          const vgl_point_2d<double>& p0,
                          const vgl_vector_2d<double>& u,
                          vcl_vector<mfpf_pose>& pts,
                          vcl_vector<double>& fit)
{
  vimt_image_2d_of<double> response_im;
  evaluate_region(image,p0,u,response_im);

  const vil_image_view<double>& r_im =  response_im.image();

  vcl_vector<vgl_point_2d<unsigned> > t_pts;
  vimt_find_image_troughs_3x3(t_pts,r_im);
  vimt_transform_2d im2w = response_im.world2im().inverse();

  for (unsigned i=0;i<t_pts.size();++i)
  {
    unsigned x =t_pts[i].x(), y=t_pts[i].y();
    double f0 = r_im(x,y);  // Value at minima on grid
    // Perform local prediction of minima
    double dx=parabolic_min(r_im(x-1,y  ),f0,r_im(x+1,y  ));
    double dy=parabolic_min(r_im(x  ,y-1),f0,r_im(x  ,y+1));

    // Check the estimate is better
    vgl_point_2d<double> p1=im2w(x+dx,y+dy);
    vgl_vector_2d<double> u1=u;
    double f1=evaluate(image,p1,u);
    if (f1>=f0)
    {
      // Refined minima is no better so revert to grid position
      f1=f0;
      p1=im2w(x,y);
    }

    // Check if locally optimal in scale and orientation
    // Only add in if it is.
    mfpf_pf_cost pf_cost(*this,image,p1,u1,ds_,dA_);
    vnl_vector<double> params(4,0.0);
    // Attempt to optimise scale
    if (vcl_fabs(ds_-1.0)>1e-6)
    {
      // Do parabolic fit if local minima
      pf_cost.check_and_refine_minima(params,f1,2,0.7);

      // Note that above fn returns false if not a local minima
      // However, safest not to discard this until understand
      // shape of fit surface better.
    }

    // Attempt to optimise orientation
    if (dA_!=0.0)
    {
      // Do parabolic fit if local minima
      pf_cost.check_and_refine_minima(params,f1,3,0.7);
    }

    // Compute actual pose for these optimal parameters
    pf_cost.get_pose(params,p1,u1);

    fit.push_back(f1);
    pts.push_back(mfpf_pose(p1,u1));

  }
}


//: Search for local optima around given point/scale/angle
//  For each angle and scale (defined by internal nA,dA,ns,ds)
//  search in a grid around p (defined by search_ni and search_nj).
//  Find local minima on this grid and return them in pts.
//
//  Note that an object in an image may lead to multiple responses,
//  one at each scale and angle near to the optima.  Thus the
//  poses defined in pts should be further refined to eliminate
//  such multiple responses.
void mfpf_point_finder::grid_search(
                          const vimt_image_2d_of<float>& image,
                          const vgl_point_2d<double>& p,
                          const vgl_vector_2d<double>& u,
                          vcl_vector<mfpf_pose>& pts,
                          vcl_vector<double>& fit)
{
  pts.resize(0);
  fit.resize(0);

  if (nA_==0 && ns_==0)
  {
    // Only search at one scale/orientation
    return grid_search_one_pose(image,p,u,pts,fit);
  }

  vgl_vector_2d<double> v(-u.y(),u.x());

  for (int is=-int(ns_);is<=int(ns_);++is)
  {
    double s = vcl_pow(ds_,is);
    for (int iA=-int(nA_);iA<=int(nA_);++iA)
    {
      double A = iA*dA_;
      vgl_vector_2d<double> uA = s*(u*vcl_cos(A)+v*vcl_sin(A));

      grid_search_one_pose(image,p,uA,pts,fit);
    }
  }
}

//: Search for local optima around given point/scale/angle
//  For each angle and scale (defined by internal nA,dA,ns,ds)
//  search in a grid around p (defined by search_ni and search_nj).
//  Find local minima on this grid.
//  Sub-grid estimation using parabolic fitting included.
//  poses[i] defines result i, with corresponding fit fits[i]
//
//  Note that an object in an image may lead to multiple responses,
//  one at each scale and angle near to the optima.  Thus the
//  poses defined in pts should be further refined to eliminate
//  such multiple responses.
void mfpf_point_finder::multi_search(
                          const vimt_image_2d_of<float>& image,
                          const vgl_point_2d<double>& p,
                          const vgl_vector_2d<double>& u,
                          vcl_vector<mfpf_pose>& poses,
                          vcl_vector<double>& fits)
{
  poses.resize(0);
  fits.resize(0);

  if (nA_==0 && ns_==0)
  {
    // Only search at one scale/orientation
    return multi_search_one_pose(image,p,u,poses,fits);
  }

  vgl_vector_2d<double> v(-u.y(),u.x());

  for (int is=-int(ns_);is<=int(ns_);++is)
  {
    double s = vcl_pow(ds_,is);
    for (int iA=-int(nA_);iA<=int(nA_);++iA)
    {
      double A = iA*dA_;
      vgl_vector_2d<double> uA = s*(u*vcl_cos(A)+v*vcl_sin(A));

      multi_search_one_pose(image,p,uA,poses,fits);
    }
  }
}

//: Perform local optimisation to refine position,scale and angle
//  On input fit is match at p,u.  On exit p,u and fit are updated.
//  Baseclass implementation uses simplex optimisation.
void mfpf_point_finder::refine_match(const vimt_image_2d_of<float>& image,
                          vgl_point_2d<double>& p,
                          vgl_vector_2d<double>& u,
                          double& fit)
{
  // Strictly should set up scale and orientation tolerances
  // based on extent of object
  double ds=1.5;
  double dA=0.5;
  mfpf_pf_cost cost_fn(*this,image,p,u,ds,dA);

  vnl_vector<double> initial_dv(4,0.5), v(4,0.0);

  vnl_amoeba amoeba(cost_fn);

  // One unit in v moves p by 1, scale by pow(ds,1) and A by dA
  amoeba.set_x_tolerance(0.1);  // Default 1e-8
  amoeba.set_f_tolerance(9e9);
    // amoeba continues until both x and f tolerances are satisfied
    // Setting large f tolerance ensures x is satisfied

  amoeba.minimize(v, initial_dv);

/* Powell minimizer seems to require a huge number of iterations
   Stick with Simplex for the moment.
  vnl_powell powell(&cost_fn);
  powell.set_initial_step(0.5);
  powell.set_linmin_xtol(0.1);
  powell.minimize(v);
*/

  fit = cost_fn.f(v);
  cost_fn.get_pose(v,p,u);
}

//: Return true if modelled regions at pose1 and pose2 overlap
bool mfpf_point_finder::overlap(const mfpf_pose& pose1,
                                const mfpf_pose& pose2) const
{
  return false;
}


//: Return true if base class parameters are the same in pf
bool mfpf_point_finder::base_equality(const mfpf_point_finder& pf) const
{
  if (search_ni_!=pf.search_ni_) return false;
  if (search_nj_!=pf.search_nj_) return false;
  if (nA_!=pf.nA_) return false;
  if (ns_!=pf.ns_) return false;
  if (vcl_fabs(dA_-pf.dA_)>1e-6) return false;
  if (vcl_fabs(ds_-pf.ds_)>1e-6) return false;
  if (vcl_fabs(step_size_-pf.step_size_)>1e-6) return false;
  return true;
}


//=======================================================================
// Method: print
//=======================================================================

void mfpf_point_finder::print_summary(vcl_ostream& os) const
{
  os<<" step_size: "<<step_size_;
  os<<" search_ni: "<<search_ni_;
  os<<" search_nj: "<<search_nj_;
  os<<" nA: "<<nA_<<" dA: "<<dA_;
  os<<" ns: "<<ns_<<" ds: "<<ds_<<" ";
}

//=======================================================================
// Method: version_no
//=======================================================================

short mfpf_point_finder::version_no() const
{
  return 1;
}

//=======================================================================
// Binary I/O
//=======================================================================


void mfpf_point_finder::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,step_size_); 
  vsl_b_write(bfs,search_ni_); 
  vsl_b_write(bfs,search_nj_); 
  vsl_b_write(bfs,nA_); 
  vsl_b_write(bfs,dA_); 
  vsl_b_write(bfs,ns_); 
  vsl_b_write(bfs,ds_); 
}

//=======================================================================
// Method: load
//=======================================================================

void mfpf_point_finder::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,step_size_);
      vsl_b_read(bfs,search_ni_);
      vsl_b_read(bfs,search_nj_);
      vsl_b_read(bfs,nA_); 
      vsl_b_read(bfs,dA_); 
      vsl_b_read(bfs,ns_); 
      vsl_b_read(bfs,ds_); 
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&) \n";
      vcl_cerr << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string mfpf_point_finder::is_a() const
{
  return vcl_string("mfpf_point_finder");
}

//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const mfpf_point_finder& b)
{
  vsl_binary_loader<mfpf_point_finder>::instance().add(b);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mfpf_point_finder& b)
{
    b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mfpf_point_finder& b)
{
    b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const mfpf_point_finder& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const mfpf_point_finder* b)
{
    if (b)
    return os << *b;
    else
    return os << "No mfpf_point_finder defined.";
}
