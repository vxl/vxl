// This is brl/bseg/sdet/sdet_grid_finder.cxx
#include <vcl_list.h>
#include <vcl_algorithm.h> // vcl_sort()
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_math.h>
#include <vbl/vbl_bounding_box.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_4point.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <bsol/bsol_hough_line_index.h>
#include <bsol/bsol_algs.h>
#include <bsol/bsol_distance_histogram.h>
#include "sdet_grid_finder.h"


static void print_lines(vcl_vector<vsol_line_2d_sptr>& lines)
{
	for(vcl_vector<vsol_line_2d_sptr>::iterator lit = lines.begin(); lit != lines.end();
      lit++)
    {
      vgl_homg_line_2d<double> l = (*lit)->vgl_hline_2d();
      l.normalize();
      vcl_cout << l << "\n";
    }
}
class line_dist{
public:
  bool operator ()(const vsol_line_2d_sptr & l1, 
                   const vsol_line_2d_sptr & l2) 
  {
    vgl_homg_line_2d<double> hl1 = l1->vgl_hline_2d();
    vgl_homg_line_2d<double> hl2 = l2->vgl_hline_2d();
    hl1.normalize(); hl2.normalize();
    return hl1.c() > hl2.c();
  }
};


//---------------------------------------------------------------
// Constructors
//
//----------------------------------------------------------------

//: constructor from a parameter block (the only way)
//
sdet_grid_finder::sdet_grid_finder(sdet_grid_finder_params& gfp)
  : sdet_grid_finder_params(gfp)
{
  groups_valid_ = false;
  vanishing_points_valid_=false;
  projective_homography_valid_ = false;
  affine_homography_valid_ = false;
  homography_valid_ = false;
  verbose_ = false;
  index_ = 0;
}

//:Default Destructor
sdet_grid_finder::~sdet_grid_finder()
{
}

static void group_angle_stats(vcl_vector<vsol_line_2d_sptr> const & group,
                              const double angle_tol,
                              double & avg_angle, double& min_angle, 
                              double& max_angle)
{
	min_angle = 360;
  max_angle = -360;
  int n_lines = 0;
  avg_angle=0;
  
  for(vcl_vector<vsol_line_2d_sptr>::const_iterator lit = group.begin();
      lit != group.end(); lit++)
    {
      double ang = (*lit)->tangent_angle();
      if(ang>180)
        ang -= 180.0;
      if(ang<min_angle)
        min_angle = ang;
      if(ang>max_angle)
        max_angle = ang;
    }
  //See if we are on the 180-0 cut
  bool on_cut = (max_angle-min_angle)>2.0*angle_tol + 5.0;
  for(vcl_vector<vsol_line_2d_sptr>::const_iterator lit = group.begin();
      lit != group.end(); lit++, n_lines++)
    {
      double ang = (*lit)->tangent_angle();
      if(ang>180.0)
        ang-=180.0;
      if(on_cut&&ang>90)
        ang -=180;
      avg_angle += ang;
    }
  if(n_lines)
    {
      avg_angle /= n_lines;
    }
  else
    avg_angle=0;
}
//-------------------------------------------------------------------------
//: Set the edges to be processed
//
bool sdet_grid_finder::set_lines(const float xsize, const float ysize,
                                 vcl_vector<vsol_line_2d_sptr> const& lines)
{
  groups_valid_ = false;
  vanishing_points_valid_=false;
  projective_homography_valid_=false;
  affine_homography_valid_ = false;
  homography_valid_=false;
  lines_=lines;
  xmax_ = xsize;
  ymax_ = ysize;
  index_ = new bsol_hough_line_index(0,0,xsize, ysize);    
  for(vcl_vector<vsol_line_2d_sptr>::const_iterator lit = lines.begin();
      lit != lines.end(); lit++)
    index_->index(*lit);
  vcl_vector<vcl_vector<vsol_line_2d_sptr> > groups;
  if(index_->dominant_line_groups(thresh_, angle_tol_, groups)<2)
    return false;
  group0_ = groups[0];
  group1_ = groups[1];
  groups_valid_ = true;
  return true;
}
vgl_homg_point_2d<double> sdet_grid_finder::
get_vanishing_point(vcl_vector<vsol_line_2d_sptr> const & para_lines)
{
  //  vcl_cout << "VP \n";
  vcl_list<vgl_homg_line_2d<double> > vlines, tvlines, stvlines;
  int nlines =0;
  double tx = 0, ty =0;
  for(vcl_vector<vsol_line_2d_sptr>::const_iterator lit = para_lines.begin();
      lit != para_lines.end(); lit++, nlines++)
    {    
      vgl_homg_line_2d<double> l= (*lit)->vgl_hline_2d();
      l.normalize();
      //      vcl_cout << l << "\n";
      tx -= l.a()*l.c();
      ty -= l.b()*l.c();
      vlines.push_back(l);
    }
  tx /=nlines;
  ty /=nlines;
  for(vcl_list<vgl_homg_line_2d<double> >::iterator lit = vlines.begin();
      lit != vlines.end(); lit++)
    {    
      vgl_homg_line_2d<double>& l = (*lit);
      double c = l.c();
      c -= l.a()*tx + l.b()*ty;
      l.set(l.a(), l.b(), c);
      tvlines.push_back(l);
    }
  double c_sq = 0;
  for(vcl_list<vgl_homg_line_2d<double> >::iterator lit = tvlines.begin();
      lit != tvlines.end(); lit++)
    {    
      vgl_homg_line_2d<double>& l = (*lit);
      c_sq += l.c()*l.c();
    }
  c_sq /=nlines;
  double sigma_c = vcl_sqrt(c_sq);
  for(vcl_list<vgl_homg_line_2d<double> >::iterator lit = tvlines.begin();
      lit != tvlines.end(); lit++)
    {    
      vgl_homg_line_2d<double>& l = (*lit);
      double c = l.c();
      if(sigma_c>1e-8)
        c /= sigma_c;
      l.set(l.a(), l.b(), c);
      stvlines.push_back(l);
    }
  vgl_homg_point_2d<double> pv = vgl_homg_operators_2d<double>::lines_to_point(stvlines);
  //restore normalizing transform
  // scale factor;
  double lambda = pv.w()/sigma_c;
  pv.set((pv.x()+lambda*tx), (pv.y()+lambda*ty), lambda);
  return pv;
}

void sdet_grid_finder::compute_vanishing_points()
{
  if(!groups_valid_)
    return;
  vp0_ = get_vanishing_point(group0_);
  vp1_ = get_vanishing_point(group1_);
  vanishing_points_valid_ = true;
}

//--------------------------------------------------------------------------
//: find the grid in the set of line segments
bool sdet_grid_finder::match_grid()
{
  return true;
}

//-------------------------------------------------------------------------
//: Compute a projective homography that sends the two major 
//  group vanishing points to the x and y directions
bool sdet_grid_finder::compute_projective_homography()
{
  this->compute_vanishing_points();
  if(!vanishing_points_valid_)
    return false;
  if(projective_homography_valid_)
    return true;
  affine_homography_valid_ = false;
  homography_valid_ = false;
  if(verbose_)
    {
      vcl_cout << "VP0 " << vp0_ << "\n";
      vcl_cout << "VP1 " << vp1_ << "\n";
    }

  //Keep the sense of the axes pointing to infinity
  vgl_homg_point_2d<double> x_inf(1,0,0), x_minus_inf(-1,0,0);
  vgl_homg_point_2d<double> y_inf(0,1,0), y_minus_inf(0,-1,0);
  vgl_homg_point_2d<double> origin(0,0,1);
  vgl_homg_point_2d<double> max_corner(xmax_,ymax_,1);

  //compute a point homography that maps the 
  //vanishing points to infinity and leaves the corners of the image
  //invariant
  vcl_vector<vgl_homg_point_2d<double> > image;
  vcl_vector<vgl_homg_point_2d<double> > grid;
  if(vcl_fabs(vp0_.x()/vp0_.w())>vcl_fabs(vp1_.x()/vp1_.w()))
    {
      image.push_back(vp0_);   image.push_back(vp1_); 
    }
  else
    {
      image.push_back(vp1_);   image.push_back(vp0_); 
    }
  image.push_back(origin); image.push_back(max_corner);
  if(image[0].x()/image[0].w()>0)
    grid.push_back(x_inf);
  else
    grid.push_back(x_minus_inf);

  if(image[1].y()/image[1].w()>0)
    grid.push_back(y_inf);
  else
    grid.push_back(y_minus_inf);
  grid.push_back(origin); grid.push_back(max_corner);
  vgl_h_matrix_2d_compute_4point comp_4pt;
  if(!comp_4pt.compute(image, grid, projective_homography_)) 
    return false;
  if(verbose_)
    vcl_cout << "The projective homography \n" << projective_homography_ << "\n";
  projective_homography_valid_ = true;
  return true;
}
//---------------------------------------------------------------
//: Remove any remaining skew by finding an affine transformation
//  the maps horizontally inclined lines (angle ah) to zero degrees
//  and vertically inclined lines (angle av) to 90 degrees.
//  That is, find a transformation of the line normals
// 
//  |q00  q01 ||-sin(av) -sin(ah)|    |-1 0|
//  |q10  q11 || cos(av)  cos(ah)|  = | 0 1|
// 
//  The point transformation,Q, is then the inverse transpose of q
// 
//      | sin(av) -cos(av)|
//  Q = |-sin(ah)  cos(ah)|  = [q]^-t
//
static vnl_matrix_fixed<double, 3,3> skew_transform(const double ah,
                                                   const double av)
{
  vnl_matrix_fixed<double, 3, 3> Q;
  Q.put(0, 0, sin(av)); Q.put(0,1, -cos(av)); Q.put(0,2,0);
  Q.put(1, 0, -sin(ah)); Q.put(1,1, cos(ah)); Q.put(1,2,0);
  Q.put(2, 0, 0); Q.put(2,1, 0); Q.put(2,2,1);
  return Q;
}
//------------------------------------------------------------------------
//:Form a histogram of all pairwise distances of lines from the origin
// h_i is the count, d_i is the average distance in a bin
// The first peak is the distance between grid lines in the horizontal (gh)
// and vertical (gv) directions. The transformation makes each of these
// distances equal to spacing. The result is returned in S. 
static 
bool scale_transform(const double max_distance, double spacing,
                     vcl_vector<vsol_line_2d_sptr> const& gh,
                     vcl_vector<vsol_line_2d_sptr> const& gv,
                     vnl_matrix_fixed<double, 3, 3>& S)
{
  int nbins = 50;
  double delta = max_distance/nbins;
  bsol_distance_histogram Hh(nbins, gh), Hv(nbins, gv);
  if(false)
    {
      vcl_cout << Hh << "\n\n";
      vcl_cout << Hv << "\n";
    }
   double ph = Hh.second_distance_peak();
   double pv = Hv.second_distance_peak();
   if(false)
     {
       vcl_cout << "Horizontal Peak " << ph << "\n";
       vcl_cout << "Vertical Peak " << pv << "\n";
     }
  //failed to find peaks
  if(ph<0||pv<0)
    return false;

  //adjust the spacing to be equal.
   S.put(0, 0, spacing/ph); S.put(0,1, 0); S.put(0,2,0);
   S.put(1, 0, 0); S.put(1,1, spacing/pv); S.put(1,2,0);
   S.put(2, 0, 0); S.put(2,1, 0); S.put(2,2,1);
   return true;
}
//---------------------------------------------------------------
//:  Form an index of lines based on the perpendicular distance
//   from the origin.  The weight vector is the normalized total length
//   of lines in each bin.
static bool distance_index(const double spacing, 
                           vcl_vector<vsol_line_2d_sptr> const& lines,
                           vcl_vector<double>& average_d,
                           vcl_vector<double>& weight,
                           vcl_vector<vcl_vector<vsol_line_2d_sptr> >& index)
{
  //find the max/min range of distances
  //and cache the distances
  vcl_vector<double> distances;
  double min_d = 1.0e+8, max_d = -1.0e+8;
  for(vcl_vector<vsol_line_2d_sptr>::const_iterator lit = lines.begin();
      lit != lines.end(); lit++)
    {
      vgl_homg_line_2d<double> gl = (*lit)->vgl_hline_2d();
      gl.normalize();
      double d = -gl.c();
      distances.push_back(d);
      if(d<min_d)
        min_d = d;
      if(d>max_d)
        max_d = d;
    }
  min_d -= spacing/2;
  max_d += spacing/2;
  int range = (int)((max_d-min_d)/spacing)+1;
  //Not enough grid samples
  if(range<3)
    return false;
  if(false)
    {
      vcl_cout << "range[" << min_d << "->" << max_d << "]  \n";
      for(int k = 0; k<range; k++)
        vcl_cout << "bin_thresh[" << k << "]=" << (k+1)*spacing+min_d << "\n";
    }
  //Set index size
  vcl_vector<double> length_sum(range,0.0);
  weight.resize(range);
  average_d.resize(range,0.0);
  index.resize(range);
  //Index each line and accumulate a distance sum 
  int nl = lines.size();
  for(int i=0; i< nl; i++)
    {
      bool insert = false;
      for(int k = 0; k<range&&!insert; k++)
        if((k+1)*spacing>(distances[i]-min_d))
          {
            index[k].push_back(lines[i]);
            double length = lines[i]->length();
            length_sum[k]+=length;
            average_d[k]+=length*distances[i];
            insert = true;
          }
    }
  //compute average distance in each index bin
  double total_length = 0;
  for(int k = 0; k<range; k++)
    if(length_sum[k]>0)
      {
        total_length+=length_sum[k];
        average_d[k]/=length_sum[k];
      }

  for(int k = 0; k<range; k++)
    weight[k]=length_sum[k]/total_length;

	return true;
}
static vnl_matrix_fixed<double, 3,3> translation_transform(const double tx,
                                                           const double ty)
{
  vnl_matrix_fixed<double, 3, 3> T;
  T.put(0, 0, 1); T.put(0,1, 0); T.put(0,2,tx);
  T.put(1, 0, 0); T.put(1,1, 1); T.put(1,2,ty);
  T.put(2, 0, 0); T.put(2,1, 0); T.put(2,2,1);
  return T;
}

bool sdet_grid_finder::compute_affine_homography()
{
  if(!projective_homography_valid_)
    return false;
  if(affine_homography_valid_)
    return true;
  float affine_angle_factor = 3.0;
  vcl_vector<vsol_line_2d_sptr> affine_lines;
  for(vcl_vector<vsol_line_2d_sptr>::const_iterator lit = lines_.begin();
      lit != lines_.end(); lit++)
    {
      vsol_line_2d_sptr pline = this->transform_line(projective_homography_,
                                                     *lit);
      affine_lines.push_back(pline);
    }

  //Get the bounds of the affine lines (lines with vpoints at infinity)
  vbl_bounding_box<double, 2> b = bsol_algs::bounding_box(affine_lines);
  index_ = new bsol_hough_line_index(b);      

  for(vcl_vector<vsol_line_2d_sptr>::iterator lit = affine_lines.begin();
      lit != affine_lines.end(); lit++)
    index_->index(*lit);

  vcl_vector<vcl_vector<vsol_line_2d_sptr> > groups;
  if(index_->dominant_line_groups(thresh_,
                                  affine_angle_factor*angle_tol_,
                                  groups)<2)
    return false;//failed to find enough groups.

  afgroup0_ = groups[0];
  afgroup1_ = groups[1];
  double avg_angle0=0, avg_angle1=0, min_angle=0, max_angle=0;

  group_angle_stats(afgroup0_, angle_tol_,avg_angle0, min_angle, max_angle);
  group_angle_stats(afgroup1_, angle_tol_,avg_angle1, min_angle, max_angle);

  if(verbose_)
    {
      vcl_cout << "Affine angles \n";
      vcl_cout << "G[" << afgroup0_.size() << "] avg_angle = " << avg_angle0
               << " min_angle = " << min_angle << " max_angle = " 
               << max_angle <<  "\n";
      vcl_cout << "G[" << afgroup1_.size() << "] avg_angle = " << avg_angle1
               << " min_angle = " << min_angle << " max_angle = " 
               << max_angle <<  "\n";
    }
  //Get the orientation of roughly vertical and horizontal lines
  //ang0 is the horizontal direction and ang1 is the vertical direction
  double deg_to_rad = vnl_math::pi/180.0;
  double ang0 =0, ang90=0;
  if(vcl_fabs(avg_angle0-90)>vcl_fabs(avg_angle1-90))
    {
      ang0 = avg_angle0*deg_to_rad;
    ang90= avg_angle1*deg_to_rad;
    }
  else
    {
      ang0 = avg_angle1*deg_to_rad;
      ang90 = avg_angle0*deg_to_rad;
    }
  //lines should be along the positive x axis.
  if(ang0>vnl_math::pi_over_2)
    ang0-=vnl_math::pi;
  
  vnl_matrix_fixed<double, 3,3> Q = skew_transform(ang0, ang90);

  //max distance for distance histogram
  //the distance could be larger but unlikely
  double dx = vcl_fabs(b.xmax()-b.xmin()), dy = vcl_fabs(b.ymax()-b.ymin());
  double max_distance = dx;
  if(dx<dy)
    max_distance = dy;
  vnl_matrix_fixed<double, 3, 3> S;
  if(!scale_transform(max_distance,spacing_, afgroup0_, afgroup1_, S))
    return false;//failed to find a first distance peak

  affine_homography_ = vgl_h_matrix_2d<double>(S*Q);

   //Finally we translate until the first row and column of 
   //lines are at (0,0)
  double length_threshold = 10.0;
  vcl_vector<vsol_line_2d_sptr> grid_lines0, grid_lines1;
  for(vcl_vector<vsol_line_2d_sptr>::iterator lit = afgroup0_.begin();
      lit != afgroup0_.end(); lit++)
    {
      if((*lit)->length()<length_threshold)//JLM
        continue;
      grid_lines0.push_back(this->transform_line(affine_homography_,*lit));
    }
  for(vcl_vector<vsol_line_2d_sptr>::iterator lit = afgroup1_.begin();
      lit != afgroup1_.end(); lit++)
    {
      if((*lit)->length()<length_threshold)//JLM
        continue;
      grid_lines1.push_back(this->transform_line(affine_homography_,*lit));
    }

  vcl_vector<double> weight0, weight1, average_d0, average_d1;
  double tx =0, ty = 0;
  if(distance_index(spacing_, grid_lines0, average_d0, weight0, dindex0_))
    if(verbose_)
      {
        int n = dindex0_.size();
        for(int i = 0; i<n; i++)
          vcl_cout << " distance0 " << average_d0[i] << " weight0 = " 
                   << weight0[i] << "  nlines0 "  << dindex0_[i].size() << "\n";    }

  if(distance_index(spacing_, grid_lines1, average_d1, weight1, dindex1_))
    if(verbose_)
      {
        int n = dindex1_.size();
        for(int i = 0; i<n; i++)
          vcl_cout << " distance1 " << average_d0[i] << " weight1 = " 
                   << weight1[i] << "  nlines1 "  << dindex1_[i].size() << "\n";
      }
  vgl_h_matrix_2d<double> T;
  this->compute_homography_linear(T);
  affine_homography_ = T*affine_homography_;
  if(verbose_)
    vcl_cout << "The affine homography \n" << affine_homography_ << "\n";
  affine_homography_valid_ = true;
  return true;
}
//------------------------------------------------------
//: Assumes that a set of lines have been binned in the 1-d distance index
//  Assumes that dindex0_ are vertical lines and dindex1_ are horizontal
bool sdet_grid_finder::compute_homography_linear(vgl_h_matrix_2d<double> & H)
{
  vcl_vector<vgl_homg_line_2d<double> > lines_grid, lines_image;
  int n0 = dindex0_.size(), n1 = dindex1_.size();
  vcl_vector<double> weights;
  double length_sum = 0;
  if(!n0 || !n1)
    return false;
  for(int i0 = 0; i0< n0; i0++)
    {
      vgl_homg_line_2d<double> lv(1.0, 0.0, -i0*spacing_);
      int nv = dindex0_[i0].size();
      if(!nv)
        continue;
      for(int j0 = 0; j0<nv; j0++)
        {
          lines_grid.push_back(lv);
          vsol_line_2d_sptr l0 = dindex0_[i0][j0];
          double length = l0->length();
          length_sum += length;
          weights.push_back(length);
          lines_image.push_back(l0->vgl_hline_2d());
        }
    }
  for(int i1 = 0; i1< n1; i1++)
    {
      vgl_homg_line_2d<double> lh(0.0, 1.0, -i1*spacing_);
      int nh = dindex1_[i1].size();
      if(!nh)
        continue;
      for(int j1 = 0; j1<nh; j1++)
        {
          lines_grid.push_back(lh);
          vsol_line_2d_sptr l1 = dindex1_[i1][j1];
          double length = l1->length();
          length_sum += length;
          weights.push_back(length);
          lines_image.push_back(l1->vgl_hline_2d());
        }
    }
  if(length_sum)
    for(vcl_vector<double>::iterator wit = weights.begin();
        wit != weights.end(); wit++)
      (*wit)/=length_sum;

  vgl_h_matrix_2d_compute_linear hcl;
  H = hcl.compute(lines_image, lines_grid, weights);
  return true;
}
bool sdet_grid_finder::compute_homography()
{  
  if(!this->compute_projective_homography())
    return false;
  if(!this->compute_affine_homography())
    return false;
  homography_ = affine_homography_*projective_homography_;
  if(verbose_)
    {
      vcl_cout << "The composite homography \n" << homography_ << "\n";
      vcl_cout.flush();
    }
  homography_valid_=true;
  return true;
}
//------------------------------------------------------------------------
//: Transform a vsol line using the point transform
//
vsol_line_2d_sptr 
sdet_grid_finder::transform_line(vgl_h_matrix_2d<double> const& h,
                                 vsol_line_2d_sptr const & l)
{
  vsol_point_2d_sptr p0 = l->p0();
  vsol_point_2d_sptr p1 = l->p1();
  vgl_homg_point_2d<double> vp0(p0->x(), p0->y());
  vgl_homg_point_2d<double> vp1(p1->x(), p1->y());
  vgl_point_2d<double> tvp0 = h(vp0);
  vgl_point_2d<double> tvp1 = h(vp1);
  vsol_point_2d_sptr tp0 = new vsol_point_2d(tvp0.x(), tvp0.y());
  vsol_point_2d_sptr tp1 = new vsol_point_2d(tvp1.x(), tvp1.y());
  return new vsol_line_2d(tp0, tp1);
}

//-------------------------------------------------------------------------
//: Get the original set of lines mapped by the line homography
//
bool 
sdet_grid_finder::get_mapped_lines(vcl_vector<vsol_line_2d_sptr> & lines)
{
  if(!homography_valid_)
    return false;
  lines.clear();
  for(vcl_vector<vsol_line_2d_sptr>::iterator lit = lines_.begin();
      lit != lines_.end(); lit++)
    {
      vsol_line_2d_sptr l = this->transform_line(homography_,*lit);
      lines.push_back(l);
    }
  return true;
}
//-------------------------------------------------------------------------
//: Get the grid lines mapped back onto the image
//
bool 
sdet_grid_finder::get_backprojected_grid(vcl_vector<vsol_line_2d_sptr> & lines)
{
  if(!homography_valid_)
    return false;
  lines.clear();
  vgl_h_matrix_2d<double> grid_to_image = homography_.get_inverse();
  //transform the vertical grid lines back to the image
  for(int y = 0; y<n_lines_y_; y++)
    {
      double xv = y*spacing_, maxy = (n_lines_x_-1)*spacing_;
      vgl_homg_point_2d<double> p0(xv,0), p1(xv, maxy);
      vgl_homg_point_2d<double> tp0, tp1;
      tp0 = grid_to_image(p0);  tp1 = grid_to_image(p1);
      vsol_point_2d_sptr sp0 = new vsol_point_2d(tp0);
	  vsol_point_2d_sptr sp1 = new vsol_point_2d(tp1);
	  
      vsol_line_2d_sptr lv= new vsol_line_2d(sp0, sp1);
      lines.push_back(lv);
    }
  //transform the horizontal grid lines back to the image
  for(int x = 0; x<n_lines_x_; x++)
    {
      double yv = x*spacing_, maxx = (n_lines_y_-1)*spacing_;
      vgl_homg_point_2d<double> p0(0,yv), p1(maxx, yv);
      vgl_homg_point_2d<double> tp0, tp1;
      tp0 = grid_to_image(p0);  tp1 = grid_to_image(p1);
      vsol_point_2d_sptr sp0 = new vsol_point_2d(tp0);
      vsol_point_2d_sptr sp1 = new vsol_point_2d(tp1);
      vsol_line_2d_sptr lv = new vsol_line_2d(sp0, sp1);
      lines.push_back(lv);
    }
  return true;
}
//----------------------------------------------------------
//: Clear internal storage
//
void sdet_grid_finder::clear()
{
  lines_.clear();
  vanishing_points_valid_=false;
  projective_homography_valid_ = false;
  homography_valid_ = false;
}

