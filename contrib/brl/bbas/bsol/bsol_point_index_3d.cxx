//:
// \file
#include <vcl_cmath.h>
#include <vsol/vsol_point_3d.h>
#include <bsol/bsol_algs.h>
#include <bsol/bsol_point_index_3d.h>
#ifdef DEBUG
#include <vcl_iostream.h>
#endif

static void clear_flag(vsol_point_3d_sptr& p)
{
  p->unset_user_flag(VSOL_FLAG1);
}

static void set_flag(vsol_point_3d_sptr & p)
{
  p->set_user_flag(VSOL_FLAG1);
}

static bool flag(vsol_point_3d_sptr const& p)
{
  return p->get_user_flag(VSOL_FLAG1);
}


//------------------------------------------------------------------------
//: Constructors
//------------------------------------------------------------------------
bsol_point_index_3d::bsol_point_index_3d(int nrows, int ncols, int nslabs,
                                         vsol_box_3d_sptr const& bb)
{
  nrows_ = nrows;
  ncols_ = ncols;
  nslabs_ = nslabs;
  //initialize the array
  point_array_.resize(nrows);
  for (int r = 0; r<nrows; r++)
  {
    point_array_[r].resize(ncols);
    for (int c = 0; c<ncols; c++)
      point_array_[r][c].resize(nslabs);
  }
  b_box_ = bb;
  double w = b_box_->width(), h = b_box_->height(), d = b_box_->depth();
  row_spacing_ = 1;
  col_spacing_ = 1;
  slab_spacing_ = 1;
  if (nrows)
    row_spacing_ = h/nrows;
  if (ncols)
    col_spacing_ = w/ncols;
  if (nslabs)
    slab_spacing_ = d/nslabs;
}

bsol_point_index_3d::
bsol_point_index_3d(int nrows, int ncols, int nslabs,
                    vcl_vector<vsol_point_3d_sptr> const& points)
{
  nrows_ = nrows;
  ncols_ = ncols;
  nslabs_ = nslabs;
  //initialize the array
  point_array_.resize(nrows);
  for (int r = 0; r<nrows; r++)
  {
    point_array_[r].resize(ncols);
    for (int c = 0; c<ncols; c++)
      point_array_[r][c].resize(nslabs);
  }
  vbl_bounding_box<double,3> box = bsol_algs::bounding_box(points);
  b_box_ = new vsol_box_3d(box);
  double w = b_box_->width(), h = b_box_->height(), d = b_box_->depth();
  row_spacing_ = 1;
  col_spacing_ = 1;
  slab_spacing_ = 1;
  if (nrows)
    row_spacing_ = h/nrows;
  if (ncols)
    col_spacing_ = w/ncols;
  if (nslabs)
    slab_spacing_ = d/nslabs;

  this->add_points(points);
}

//: Destructor
bsol_point_index_3d::~bsol_point_index_3d()
{
}

void bsol_point_index_3d::origin(double& x0, double& y0, double& z0)
{
  if (!b_box_) { x0 = y0 = z0 = 0; return; }
  x0 = b_box_->get_min_x();
  y0 = b_box_->get_min_y();
  z0 = b_box_->get_min_z();
}

int
bsol_point_index_3d::n_points(const int row, const int col, const int slab)
{
  if (row<0||row>=nrows_||col<0||col>=ncols_||slab<0||slab>=nslabs_)
    return 0;
  int n = point_array_[row][col][slab].size();
  return n;
}

int
bsol_point_index_3d::n_points(const double x, const double y, const double z)
{
  int row=0, col=0, slab=0;
  if (!trans(x, y, z, row, col, slab))
    return 0;
  if (row<0||row>=nrows_||col<0||col>=ncols_||slab<0||slab>=nslabs_)
    return 0;
  return point_array_[row][col][slab].size();
}

int bsol_point_index_3d::n_points()
{
  int n = 0;
  for (int r = 0; r<nrows_; r++)
    for (int c = 0; c<ncols_; c++)
      for (int s = 0; s<nslabs_; s++)
        n += n_points(r, c, s);
  return n;
}

vcl_vector<vsol_point_3d_sptr> bsol_point_index_3d::points(const int row, const int col,
                                                           const int slab)
{
  vcl_vector<vsol_point_3d_sptr> out;
  if (row<0||row>=nrows_||col<0||col>=ncols_||slab<0||slab>=nslabs_)
    return out;
  return point_array_[row][col][slab];
}

//:offset to origin of bounds and convert to cell integer coordinates
bool bsol_point_index_3d::trans(const double x, const double y,
                                const double z,
                                int& row, int& col, int& slab)
{
  if (!bsol_algs::in(b_box_, x, y, z))
    return false;
  col = (int)((x-b_box_->get_min_x())/col_spacing_);
  row = (int)((y-b_box_->get_min_y())/row_spacing_);
  slab = (int)((z-b_box_->get_min_z())/slab_spacing_);
  return true;
}

//---------------------------------------------------------------------
//: Add a point to the index.
//  Should check for duplicate points, but not doing that right now.
//---------------------------------------------------------------------
bool bsol_point_index_3d::add_point(vsol_point_3d_sptr const& p)
{
  double x = p->x(), y = p->y(), z = p->z();
  if (vcl_fabs(y-659.234)<1.0)
    y = y;
  int row=0, col=0, slab=0;
  if (!trans(x, y, z, row, col, slab))
    return false;
  if (row<0||row>=nrows_||col<0||col>=ncols_||slab<0||slab>=nslabs_)
    return false;
  point_array_[row][col][slab].push_back(p);
  return true;
}

bool bsol_point_index_3d::add_points(vcl_vector<vsol_point_3d_sptr> const& points)
{
  bool ok = true;
  for (vcl_vector<vsol_point_3d_sptr>::const_iterator pit = points.begin();
       pit != points.end(); pit++)
    if (!this->add_point(*pit))
      ok = false;
  return ok;
}

bool bsol_point_index_3d::find_point(vsol_point_3d_sptr const& p)
{
  int row=0, col=0, slab = 0;
  if (!this->trans(p->x(), p->y(), p->z(),row, col, slab))
    return false;
  if (row<0||row>=nrows_||col<0||col>=ncols_||slab<0||slab>=nslabs_)
    return false;
  vcl_vector<vsol_point_3d_sptr>& points = point_array_[row][col][slab];
  for (vcl_vector<vsol_point_3d_sptr>::iterator pit = points.begin();
       pit!=points.end(); pit++)
    if ((*pit)==p)
      return true;
  return false;
}

//:find the points within a box
bool bsol_point_index_3d::in_box(vsol_box_3d_sptr const& box,
                                 vcl_vector<vsol_point_3d_sptr>& points)
{
  if (!box)
    return false;
  double xmin = box->get_min_x(),
    ymin = box->get_min_y(), zmin = box->get_min_z();
  double xmax = box->get_max_x(),
    ymax = box->get_max_y(), zmax = box->get_max_z();
  int row_min = 0, col_min = 0, slab_min = 0;
  int row_max = 0, col_max = 0, slab_max = 0;
  trans(xmin, ymin, zmin, row_min, col_min, slab_min);
  trans(xmax, ymax, zmax, row_max, col_max, slab_max);
  if (row_min<0)
    row_min = 0;
  if (col_min<0)
    col_min = 0;
  if (slab_min<0)
    slab_min = 0;
  if (row_max>nrows_)
    row_max = nrows_-1;
  if (col_max>=ncols_)
    col_max = ncols_-1;
  if (slab_max>=nslabs_)
    slab_max = nslabs_-1;
  for (int r = row_min; r<row_max; r++)
    for (int c = col_min; c<col_max; c++)
      for (int s = slab_min; s<slab_max; s++)
      {
        vcl_vector<vsol_point_3d_sptr>& pts = point_array_[r][c][s];
        for (vcl_vector<vsol_point_3d_sptr>::iterator pit = pts.begin();
             pit != pts.end(); pit++)
          points.push_back(*pit);
      }
  return true;
}

//:find the points within a radius of p
bool bsol_point_index_3d::in_radius(const double radius,
                                    vsol_point_3d_sptr const& p,
                                    vcl_vector<vsol_point_3d_sptr>& points)
{
  if (!p)
    return false;
  bool found_points = false;
  // find the cell corresponding to p
  double x = p->x(), y = p->y(), z = p->z();
  int row = 0, col =0, slab = 0;
  this->trans(x, y, z, row, col, slab);
  //get points from surrounding cells
  int row_radius = int(radius/row_spacing_),
      col_radius = int(radius/col_spacing_),
      slab_radius = int(radius/slab_spacing_);
  //include points near cell boundaries
  row_radius++; col_radius++;
  for (int ro = -row_radius; ro<=row_radius; ro++)
    for (int co = -col_radius; co<=col_radius; co++)
      for (int so = -slab_radius; so<=slab_radius; so++)
      {
        int r = row+ro, c = col+co, s = slab + so;
        if (r<0||r>=nrows_||c<0||c>=ncols_||s<0||s>=nslabs_)
          continue;
        vcl_vector<vsol_point_3d_sptr>& points_in_cell = point_array_[r][c][s];
        int n = points_in_cell.size();
        if (!n)
          continue;
        for (int i = 0; i<n; i++)
          if (!flag(points_in_cell[i]))
          {
            points.push_back(points_in_cell[i]);
            found_points = true;
          }
      }
  return found_points;
}

//:find the closest point with the specified radius.  If none return false.
bool bsol_point_index_3d::closest_in_radius(const double radius,
                                            vsol_point_3d_sptr const& p,
                                            vsol_point_3d_sptr& point)
{
  vcl_vector<vsol_point_3d_sptr> points;
  this->in_radius(radius, p, points);
  double d =0;
  point = bsol_algs::closest_point(p, points, d);
  if (!point||d>radius)
    return false;
#ifdef DEBUG
  vcl_cout << "p("<< p->x() << ' ' << p->y()<< "):P(" << point->x() << ' ' << point->y() << "):" << d << vcl_endl;
#endif
  return true;
}

void bsol_point_index_3d::clear()
{
  for (int r =0; r<nrows_; r++)
    for (int c = 0; c<ncols_; c++)
      for (int s = 0; c<nslabs_; c++)
        point_array_[r][c][s].clear();
}

bool bsol_point_index_3d::mark_point(vsol_point_3d_sptr& p)
{
  if (!p||!this->find_point(p))
    return false;
  set_flag(p);
  return true;
}

bool bsol_point_index_3d::unmark_point(vsol_point_3d_sptr& p)
{
  if (!p||!this->find_point(p))
    return false;
  clear_flag(p);
  return true;
}

bool bsol_point_index_3d::marked(vsol_point_3d_sptr const& p)
{
  if (!p||!this->find_point(p))
    return false;
  return flag(p);
}

vcl_vector<vsol_point_3d_sptr> bsol_point_index_3d::points()
{
  vcl_vector<vsol_point_3d_sptr> out;
  for (int r = 0; r<nrows_; r++)
    for (int c = 0; c<ncols_; c++)
      for (int s = 0; s<nslabs_; s++)
      {
        vcl_vector<vsol_point_3d_sptr>& points = point_array_[r][c][s];
        for (vcl_vector<vsol_point_3d_sptr>::iterator pit = points.begin();
             pit!= points.end(); pit++)
          out.push_back(*pit);
      }
  return out;
}

void bsol_point_index_3d::clear_marks()
{
  vcl_vector<vsol_point_3d_sptr> pts = this->points();
  for (vcl_vector<vsol_point_3d_sptr>::iterator pit = pts.begin();
       pit!= pts.end(); pit++)
    clear_flag(*pit);
}

vsol_box_3d_sptr bsol_point_index_3d::
index_cell(const int row, const int col, const int slab)
{
  vsol_box_3d_sptr box = new vsol_box_3d;
  if (row<0||row>=nrows_||col<0||col>=ncols_||slab<0||slab>=nslabs_)
    return box;
  //the cell origin
  double x0, y0, z0;
  this->origin(x0, y0, z0);
  double xmin = col_spacing_*col   + x0;
  double ymin = row_spacing_*row   + y0;
  double zmin = slab_spacing_*slab + z0;
  box->add_point(xmin, ymin, zmin);
  box->add_point(xmin+col_spacing_, ymin+row_spacing_, zmin+slab_spacing_);
  return box;
}

vsol_box_3d_sptr bsol_point_index_3d::
index_cell(const double x, const double y, const double z)
{
  vsol_box_3d_sptr null;
  int row=0, col=0, slab=0;
  if (!trans(x, y, z, row, col, slab))
    return null;
  if (row<0||row>=nrows_||col<0||col>=ncols_||slab<0||slab>=nslabs_)
    return null;
  return this->index_cell(row, col, slab);
}

vsol_box_3d_sptr bsol_point_index_3d::point_bounds()
{
  vsol_box_3d_sptr box = new vsol_box_3d();
  for (int r = 0; r<nrows_; r++)
    for (int c = 0; c<ncols_; c++)
      for (int s = 0; s<nslabs_; s++)
      {
        vcl_vector<vsol_point_3d_sptr>& points = point_array_[r][c][s];
        for (vcl_vector<vsol_point_3d_sptr>::iterator pit = points.begin();
             pit!= points.end(); pit++)
          box->add_point((*pit)->x(), (*pit)->y(), (*pit)->z());
      }
  return box;
}
