//:
// \file
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_numeric_traits.h>
#include <vsol/vsol_point_2d.h>
#include <bsol/bsol_algs.h>
#include <bsol/bsol_point_index_2d.h>

static void clear_flag(vsol_point_2d_sptr& p)
{
  p->unset_user_flag(VSOL_FLAG1);
}
static void set_flag(vsol_point_2d_sptr &  p)
{
  p->set_user_flag(VSOL_FLAG1);
}
static bool flag(vsol_point_2d_sptr const&  p)
{
  return p->get_user_flag(VSOL_FLAG1);
}


//------------------------------------------------------------------------
//: Constructors
//------------------------------------------------------------------------
bsol_point_index_2d::bsol_point_index_2d(int nrows, int ncols, 
                                   vsol_box_2d_sptr const& bb)
{
  nrows_ = nrows;
  ncols_ = ncols;
  //initialize the array 
  point_array_.resize(nrows);
  for(int r = 0; r<nrows; r++)
    point_array_[r].resize(ncols);
  b_box_ = bb;
  double w = b_box_->width(), h = b_box_->height();
  row_spacing_ = 1;
  col_spacing_ = 1;
  if(nrows)
    row_spacing_ = h/nrows;
  if(ncols)
    col_spacing_ = w/ncols;
}
bsol_point_index_2d::
bsol_point_index_2d(int nrows, int ncols, 
                 vcl_vector<vsol_point_2d_sptr> const& points)
{
  nrows_ = nrows;
  ncols_ = ncols;
  //initialize the array 
  point_array_.resize(nrows);
  for(int r = 0; r<nrows; r++)
    point_array_[r].resize(ncols);
  vbl_bounding_box<double,2> box = bsol_algs::bounding_box(points);
  b_box_ = new vsol_box_2d(box);

  double w = b_box_->width(), h = b_box_->height();
  row_spacing_ = 1;
  col_spacing_ = 1;
  if(nrows)
    row_spacing_ = h/nrows;
  if(ncols)
    col_spacing_ = w/ncols;
  this->add_points(points);
}

//: Destructor
bsol_point_index_2d::~bsol_point_index_2d()
{
}

//:offset to origin of bounds and convert to cell integer coordinates
bool bsol_point_index_2d::trans(const double x, const double y,
                             int& row, int& col)
{
	if(!bsol_algs::in(b_box_, x, y))
    return false;
  col = (int)((x-b_box_->get_min_x())/col_spacing_);
  row = (int)((y-b_box_->get_min_y())/row_spacing_);
  return true;
}

//---------------------------------------------------------------------
//: Add a point to the index.  Should check for duplicate points, but not
//  doing that right now. 
//---------------------------------------------------------------------
bool bsol_point_index_2d::add_point(vsol_point_2d_sptr const& p)
{
  double x = p->x(), y = p->y();
  int row=0, col=0;
  if(!trans(x, y, row, col))
    return false;
  if(row<0||row>=nrows_||col<0||col>=ncols_)
    return false;
  point_array_[row][col].push_back(p);
  return true;
}

bool bsol_point_index_2d::add_points(vcl_vector<vsol_point_2d_sptr> const& points)
{
  bool ok = true;
  for(vcl_vector<vsol_point_2d_sptr>::const_iterator pit = points.begin();
      pit != points.end(); pit++)
    if(!this->add_point(*pit))
      ok = false;
  return ok;
}

bool bsol_point_index_2d::find_point(vsol_point_2d_sptr const& p)
{
  int row=0, col=0;
  if(!this->trans(p->x(), p->y(), row, col))
    return false;
  if(row<0||row>=nrows_||col<0||col>=ncols_)
    return false;
  vcl_vector<vsol_point_2d_sptr>& points =  point_array_[row][col];
  for(vcl_vector<vsol_point_2d_sptr>::iterator pit = points.begin();
      pit!=points.end(); pit++)
    if((*pit)==p)
      return true;
  return false;
}

//:find the points within a radius of p 
bool bsol_point_index_2d::in_radius(const double radius,
                                 vsol_point_2d_sptr const& p,
                                 vcl_vector<vsol_point_2d_sptr>& points)
{
  if(!p)
    return false;
  bool found_points = false;
  // find the cell corresponding to p
  double x = p->x(), y = p->y();
  int row = 0, col =0;
  this->trans(x, y, row, col);
  //get points from surrounding cells
  int row_radius = (int)(radius/row_spacing_),
    (int)(col_radius = radius/col_spacing_);
  //include points near cell boundaries
  row_radius++; col_radius++;
  for(int ro = -row_radius; ro<=row_radius; ro++)
    for(int co = -col_radius; co<=col_radius; co++)
      {
        int r = row+ro, c = col+co;
        if(r<0||r>=nrows_||c<0||c>=ncols_)
          continue;
        vcl_vector<vsol_point_2d_sptr>& points_in_cell = point_array_[r][c];
        int n = points_in_cell.size();
        if(!n)
          continue;
        for(int i = 0; i<n; i++)
          if(!flag(points_in_cell[i]))
            {
              points.push_back(points_in_cell[i]);
              found_points = true;
            }
      }
  return found_points;
}
//:find the closest point with the specified radius.  If none return false.
bool bsol_point_index_2d::closest_in_radius(const double radius,
                                         vsol_point_2d_sptr const& p,
                                         vsol_point_2d_sptr& point)
{
  vcl_vector<vsol_point_2d_sptr> points;
  this->in_radius(radius, p, points);
  double d =0;
  point = bsol_algs::closest_point(p, points, d);
  if(!point||d>radius)
    return false;
//   vcl_cout << "p("<< p->x() << " " << p->y()<< "):P(" << point->x() << " " << point->y() << "):" << d << "\n" << vcl_flush;
  return true;
}

void bsol_point_index_2d::clear()
{
  for(int r =0; r<nrows_; r++)
    for(int c = 0; c<ncols_; c++)
      point_array_[r][c].clear();
}
  
bool bsol_point_index_2d::mark_point(vsol_point_2d_sptr& p)
{
  if(!p||!this->find_point(p))
    return false;
  set_flag(p);
  return true;
}

bool bsol_point_index_2d::unmark_point(vsol_point_2d_sptr& p)
{
  if(!p||!this->find_point(p))
    return false;
  clear_flag(p);
  return true;
}

bool bsol_point_index_2d::marked(vsol_point_2d_sptr const& p)
{
  if(!p||!this->find_point(p))
    return false;
  return flag(p);
}

vcl_vector<vsol_point_2d_sptr> bsol_point_index_2d::points()
{
  vcl_vector<vsol_point_2d_sptr> out;
  for(int r = 0; r<nrows_; r++)
    for(int c = 0; c<ncols_; c++)
      {
        vcl_vector<vsol_point_2d_sptr>& points = point_array_[r][c];
        for(vcl_vector<vsol_point_2d_sptr>::iterator pit = points.begin();
            pit!= points.end(); pit++)
          out.push_back(*pit);
      }
 return out;
}
  
void bsol_point_index_2d::clear_marks()
{
  vcl_vector<vsol_point_2d_sptr> pts = this->points();
  for(vcl_vector<vsol_point_2d_sptr>::iterator pit = pts.begin();
            pit!= pts.end(); pit++)
    clear_flag(*pit);
}
//: origin of the index space
void bsol_point_index_2d::origin(double& x0, double& y0)
{
  x0 = 0;   y0 = 0;
  if(!b_box_)
    return;
  x0 = b_box_->get_min_x();
  y0 = b_box_->get_min_y();
}
//: number of points in a cell at r, c
int bsol_point_index_2d::n_points(const int row, const int col)
{
  if(row<0||row>=nrows_||col<0||col>=ncols_)
    return 0;
  int n = point_array_[row][col].size();
  return n;
}
//: number of points in a cell at x, y
int bsol_point_index_2d::n_points(const double x, const double y)
{
  int row=0, col=0;
  if(!trans(x, y, row, col))
    return 0;
  if(row<0||row>=nrows_||col<0||col>=ncols_)
    return 0;
  return point_array_[row][col].size();
}
//: total number of points in the index
int bsol_point_index_2d::n_points()
{
  int n = 0;
  for(int r = 0; r<nrows_; r++)
    for(int c = 0; c<ncols_; c++)
      n += n_points(r, c);
  return n;
}
//:the points in an index cell
vcl_vector<vsol_point_2d_sptr> bsol_point_index_2d::
points(const int row, const int col)
{
  vcl_vector<vsol_point_2d_sptr> out;
  if(row<0||row>=nrows_||col<0||col>=ncols_)
    return out;
  return point_array_[row][col];
}

//: the box corresponding to an index cell, r, c
vsol_box_2d_sptr bsol_point_index_2d::index_cell(const int row, const int col)
{
  vsol_box_2d_sptr box = new vsol_box_2d();
  if(row<0||row>=nrows_||col<0||col>=ncols_)
    return box;
  //the cell origin
  double x0, y0;
  this->origin(x0, y0);
  double xmin = col_spacing_*col   + x0;
  double ymin = row_spacing_*row   + y0;
  box->add_point(xmin, ymin);
  box->add_point(xmin+col_spacing_, ymin+row_spacing_);
  return box;
}

vsol_box_2d_sptr bsol_point_index_2d::
index_cell(const double x, const double y)
{
  vsol_box_2d_sptr null;
  int row=0, col=0;
  if(!trans(x, y, row, col))
    return null;
  if(row<0||row>=nrows_||col<0||col>=ncols_)
    return null;
  return this->index_cell(row, col);
}

vsol_box_2d_sptr bsol_point_index_2d::point_bounds()
{
  vsol_box_2d_sptr box = new vsol_box_2d();
  for(int r = 0; r<nrows_; r++)
    for(int c = 0; c<ncols_; c++)
      {
        vcl_vector<vsol_point_2d_sptr>& points = point_array_[r][c];
        for(vcl_vector<vsol_point_2d_sptr>::iterator pit = points.begin();
            pit!= points.end(); pit++)
          box->add_point((*pit)->x(), (*pit)->y());
      }
  return box;
}
