// This is gel/vifa/vifa_line_cover.cxx
#include <vifa/vifa_line_cover.h>
//:
// \file
#include <vcl_algorithm.h>


//-----------------------------------------------------------
//: Constructors

vifa_line_cover::
vifa_line_cover()
{
  line_ = NULL;
  index_ = NULL;
  max_extent_ = NULL;
  min_extent_ = NULL;
  dim_ =0;
}

vifa_line_cover::
vifa_line_cover(imp_line_sptr  prototype_line,
                int            indexdimension)
{
  line_ = prototype_line;
  dim_ = indexdimension;
  index_ = new int[dim_];
  max_extent_ = new double[dim_];
  min_extent_ = new double[dim_];
  for (int i = 0; i < dim_; i++)
  {
    index_[i] = 0;
    max_extent_[i] = -1E6;
    min_extent_[i] = 1E6;
  }
}

//------------------------------------------------------------
//: Destructor
vifa_line_cover::
~vifa_line_cover()
{
  delete [] index_;
  delete [] max_extent_;
  delete [] min_extent_;
}

//------------------------------------------------------------
//: Insert a line in the index. The line is inserted in each index bin it covers.
void vifa_line_cover::
InsertLine(imp_line_sptr  l)
{
  int  rdim1 = dim_ - 1;

  // Find the 2-D projection of l onto the prototype line
  // That is, find the parameter bounds
  vgl_point_2d<double>  sp;
  vgl_point_2d<double>  ep;
  sp = l->point1();
  ep = l->point2();
  double  ts = line_->find_t(sp);
  double  te = line_->find_t(ep);

  // Get properly ordered and limited parameter bounds
  int ist;
  int iend;
  if (ts < te)
  {
    if (ts < 0)
      ts = 0;
    if (te > 1.0)
      te = 1.0;

    ist = (int)(rdim1 * ts);
    iend = (int)(rdim1 * te);
  }
  else
  {
    if (te < 0)
      te = 0;
    if (ts > 1.0)
      ts = 1.0;

    ist = (int)(rdim1 * te);
    iend = (int)(rdim1 * ts);
  }

  // Next accumulate the bins for each line coverage
  for (int i = ist; i <= iend; i++)
  {
    index_[i]++;
    double          t = (double)i / (double)rdim1;
    vgl_point_2d<double>  pos = line_->find_at_t(t);

    vgl_point_2d<double>  p = l->project_2d_pt(pos);
    double          d = this->get_signed_distance(p);

    min_extent_[i] = vcl_min(min_extent_[i], d);
    max_extent_[i] = vcl_max(max_extent_[i], d);
  }
}

//---------------------------------------------------------------
//: Get the average coverage along the line.
//  Normalized by the full bounding box diagonal.  Coverage must be
//   greater than one line in order to indicate mutual coverage
double vifa_line_cover::
GetCoverage()
{
  int  sum = 0;
  for (int i = 0; i < dim_; i++)
  {
    int  nlines = index_[i];
    if (nlines > 1)
      sum += (nlines - 1);
  }

  return (double)sum / dim_;
}

//---------------------------------------------------------------
//: Get the average coverage along the line.
//  Coverage must be greater than one line in order to indicate mutual coverage
double vifa_line_cover::
GetDenseCoverage()
{
  double  cov = this->GetCoverage();
  double  cover_extent = double(this->get_index_max() - this->get_index_min());
  if (cover_extent <= 0)
    return 0.0;
  else
    return (dim_ * cov) / cover_extent;
}


//-------------------------------------------------------------
//: Get coverage relative to the total perimeter of indexed lines
//
double vifa_line_cover::
GetCustomCoverage(const double  norm)
{
  double  total_cover = this->GetCoverage() * dim_;
  if (norm <= 0)
    return 0.0;
  else
    return total_cover / norm;
}
//---------------------------------------------------------------
//: Get extent of image space where parallel lines overlap under projection onto the prototype.
void vifa_line_cover::
GetExtent(imp_line_sptr&  lmin,
          imp_line_sptr&  lmax)
{
  if (!line_)
  {
    lmin = NULL;
    lmax = NULL;
    return;
  }

  int    st = get_index_min();
  int    en = get_index_max();
  double  min_ex = 1E6;
  double  max_ex = -1E6;

  // Scan for the max and min lateral extents
  for (int i = st; i <= en; i++)
  {
    min_ex = vcl_min(min_extent_[i], min_ex);
    max_ex = vcl_max(max_extent_[i], max_ex);
  }

  // Construct min and max bounding lines
  lmin = get_offset_line(st, en, min_ex);
  lmax = get_offset_line(st, en, max_ex);
}

int vifa_line_cover::
get_index_min()
{
  for (int i = 1; (i < dim_); i++)
  {
    if (index_[i] > 1)
      return i;
  }

  return 0;
}

int vifa_line_cover::
get_index_max()
{
  for (int i = dim_ - 1; i > 0; --i)
  {
    if (index_[i] > 1)
      return i;
  }
  return 0;
}

// Signed distance (algebraic dist) of a point from the prototype line
double vifa_line_cover::
get_signed_distance(vgl_point_2d<double> const &  p)
{
  double  a = line_->a(), b = line_->b(), c = line_->c();
  double  d = a * p.x() + b * p.y() + c;
  return d;
}

// Get a bounded ImplicitLine which is offset from the prototype line
// by the signed distance, d.
imp_line_sptr vifa_line_cover::
get_offset_line(int    start,
                int    end,
                double d)
{
  // The normal vector
  vgl_vector_2d<double>  n = line_->normal();

  // The end points on the prototype line
  double  ts = (double)start / double(dim_ - 1);
  vgl_point_2d<double>  ps = line_->find_at_t(ts);
  double  te = (double)end / double(dim_ - 1);
  vgl_point_2d<double>  pe = line_->find_at_t(te);

  n *= d; // Extend the normal
  ps += n; // The offset start point
  pe += n; // The offset end point
  imp_line_sptr  il = new imp_line(ps, pe);
  return il;
}
