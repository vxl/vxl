#include "brip_roi.h"
//:
// \file
#include <vsol/vsol_box_2d.h>
#if 0 // later
#include <vgl/vgl_box_2d.h>
#endif

brip_roi::brip_roi(const unsigned n_image_cols, const unsigned n_image_rows)
{
  n_image_cols_ = n_image_cols;
  n_image_rows_ = n_image_rows;
}

void brip_roi::set_image_bounds(const int n_image_cols,
                                const int n_image_rows)
{
  n_image_cols_ = n_image_cols;
  n_image_rows_ = n_image_rows;
}

//:expand (or contract) each region of the roi by delta, creating a new roi.
brip_roi::brip_roi(brip_roi const& roi, float delta)
  : vbl_ref_count(), n_image_cols_(roi.n_image_cols_),
    n_image_rows_(roi.n_image_rows_), regions_(roi.regions_)
{
  float tdelta = delta;
  if (tdelta < 0) tdelta *= -1.0f; // to guarantee dxmin <= dxmax.
  for (auto & region : regions_)
  {
    double xmin = region->get_min_x();
    double ymin = region->get_min_y();
    double xmax = region->get_max_x();
    double ymax = region->get_max_y();

    double dxmin = (xmin-tdelta), dymin = (ymin-tdelta),
      dxmax = (xmax+tdelta),  dymax = (ymax+tdelta);

    //Check image bounds and crop appropriately
    if (dxmin < 0) dxmin = 0;
    if (dymin < 0) dymin = 0;
    if (dxmax >= n_image_cols_) dxmax = n_image_cols_-1;
    if (dymax >= n_image_rows_) dymax = n_image_rows_-1;

    vsol_box_2d_sptr dbox = new vsol_box_2d();
    dbox->add_point(dxmin, dymin);
    dbox->add_point(dxmax, dymax);
    region=dbox;
  }
}

vsol_box_2d_sptr brip_roi::clip_to_image_bounds(const vsol_box_2d_sptr& box)
{
  if (!box||!n_image_cols_||!n_image_rows_)
    return box;
  int x0 = (int)box->get_min_x();
  int y0 = (int)box->get_min_y();
  int xm = (int)box->get_max_x();
  int ym = (int)box->get_max_y();
  //clip to image bounds
  if (x0 < 0)
    x0 = 0;
  if ((unsigned int)x0 >= n_image_cols_)
    x0 = n_image_cols_-1;
  if (y0 < 0)
    y0 = 0;
  if ((unsigned int)y0 >= n_image_rows_)
    y0 = n_image_rows_-1;
  if (xm < 0)
    xm = 0;
  if ((unsigned int)xm >= n_image_cols_)
    xm = n_image_cols_-1;
  if (ym < 0)
    ym = 0;
  if ((unsigned int)ym >= n_image_rows_)
    ym = n_image_rows_-1;
  vsol_box_2d_sptr cbox = new vsol_box_2d();
  cbox->add_point(x0, y0);
  cbox->add_point(xm, ym);
  return cbox;
}

void brip_roi::add_region(const int x0, const int y0, const int xs, const int ys)
{
  vsol_box_2d_sptr reg =  new vsol_box_2d();
  reg->add_point(x0, y0);
  reg->add_point(x0+xs-1, y0);
  reg->add_point(x0+xs-1, y0+ys-1);
  reg->add_point(x0, y0+ys-1);
  if (!n_image_cols_||!n_image_rows_)
  {
    regions_.push_back(reg);
    return;
  }
  //need to potentially clip the region
  vsol_box_2d_sptr creg =
    this->clip_to_image_bounds(reg);
  regions_.push_back(creg);
}

void brip_roi::add_region(const int xc, const int yc, const int radius)
{
  vsol_box_2d_sptr reg =  new vsol_box_2d();
  reg->add_point(xc-radius, yc-radius);
  reg->add_point(xc+radius, yc-radius);
  reg->add_point(xc+radius, yc+radius);
  reg->add_point(xc-radius, yc+radius);
  if (!n_image_cols_||!n_image_rows_)
  {
    regions_.push_back(reg);
    return;
  }
  //need to potentially clip the region
  vsol_box_2d_sptr creg =
    this->clip_to_image_bounds(reg);
  regions_.push_back(creg);
}

void brip_roi::add_region(vsol_box_2d_sptr const & box)
{
  if (!n_image_cols_||!n_image_rows_)
  {
    regions_.push_back(box);
    return;
  }
  //need to potentially clip the region
  vsol_box_2d_sptr creg =
    this->clip_to_image_bounds(box);
  regions_.push_back(creg);
}

//return true if there are no regions or if they are all empty
bool brip_roi::empty() const
{
  return n_regions() == 0;
#if 0 // later
  for (std::vector<vgl_box_2d<int> >::iterator rit = regions_.begin();
       rit != regions_.end(); rit++)
    if (!(*rit).is_empty())
      return false;
#endif
}

void brip_roi::clip_to_image_bounds()
{
  std::vector<vsol_box_2d_sptr> temp;
  for (auto & region : regions_)
    temp.push_back(this->clip_to_image_bounds(region));
  regions_ = temp;
}

int brip_roi::cmin(const unsigned i) const
{
  return i >= regions_.size() ? 0 : (int)regions_[i]->get_min_x();
}

int brip_roi::cmax(const unsigned i) const
{
  return i >= regions_.size() ? 0 : (int)regions_[i]->get_max_x();
}

int brip_roi::rmin(const unsigned i) const
{
  return i >= regions_.size() ? 0 : (int)regions_[i]->get_min_y();
}

int brip_roi::rmax(const unsigned i) const
{
  return i >= regions_.size() ? 0 : (int)regions_[i]->get_max_y();
}

unsigned brip_roi::csize(const unsigned i) const
{
  int temp = cmax(i)-cmin(i) + 1;
  return temp<0 ? 0 : (unsigned)temp;
}

unsigned brip_roi::rsize(const unsigned i) const
{
  int temp = rmax(i)-rmin(i) + 1;
  return temp<0 ? 0 : (unsigned)temp;
}

unsigned brip_roi::ic(int local_col, unsigned i) const
{
  int temp = cmin(i) + local_col;
  return temp<0 ? 0 : (unsigned)temp;
}

unsigned brip_roi::ir(int local_row, unsigned i) const
{
  int temp = rmin(i) + local_row;
  return temp<0 ? 0 : (unsigned)temp;
}

unsigned brip_roi::lc(unsigned global_col, unsigned i) const
{
  int temp = global_col - cmin(i);
  return temp<0 ? 0 : (unsigned)temp;
}

unsigned brip_roi::lr(unsigned global_row, unsigned i) const
{
 int temp = global_row - rmin(i);
  return temp<0 ? 0 : (unsigned)temp;
}

bool brip_roi::remove_region(unsigned i)
{
  std::cerr << "brip_roi::remove_region(" << i << ") NYI\n";
  return false;
}

std::ostream&  operator<<(std::ostream& s, brip_roi const& r)
{
  s << "brip_roi (regions)\n";
  for (unsigned i = 0; i< r.n_regions(); ++i)
    s << *(r.region(i)) <<'\n';
  return s;
}
