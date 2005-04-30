//:
// \file
#include <vsol/vsol_box_2d.h>
#include <brip/brip_roi.h>

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
// If delta is too large for a given region, the region is left unchanged
brip_roi::brip_roi(brip_roi const& roi, float delta)
  : vbl_ref_count(), n_image_cols_(roi.n_image_cols_),
    n_image_rows_(roi.n_image_rows_), regions_(roi.regions_)
{
  float tdelta = delta;//error in const
  if (tdelta < 0) tdelta *= -1.0f; // to guarantee dxmin <= dxmax.
  for (vcl_vector<vsol_box_2d_sptr>::iterator rit = regions_.begin();
       rit != regions_.end(); rit++)
  {
    int xmin = (int)(*rit)->get_min_x();
    int ymin = (int)(*rit)->get_min_y();
    int xmax = (int)(*rit)->get_max_x();
    int ymax = (int)(*rit)->get_max_y();
    unsigned int dxmin = (unsigned int)(xmin-tdelta), // overflows when negative,
                 dymin = (unsigned int)(ymin-tdelta), // but this is no problem
                 dxmax = (unsigned int)(xmax+tdelta), // since the test below
                 dymax = (unsigned int)(ymax+tdelta); // will skip these cases:
    if (dxmin >= n_image_cols_ || dymin >= n_image_rows_ ||
        dxmax >= n_image_cols_ || dymax >= n_image_rows_)
      continue;
    vsol_box_2d_sptr dbox = new vsol_box_2d();
    dbox->add_point(dxmin, dymin);
    dbox->add_point(dxmax, dymax);
    (*rit)=dbox;
  }
}

vsol_box_2d_sptr brip_roi::clip_to_image_bounds(vsol_box_2d_sptr box)
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
#if 0//later
  for (vcl_vector<vgl_box_2d<int> >::iterator rit = regions_.begin();
       rit != regions_.end(); rit++)
    if (!(*rit).is_empty())
      return false;
#endif
}

void brip_roi::clip_to_image_bounds()
{
  vcl_vector<vsol_box_2d_sptr> temp;
  for (vcl_vector<vsol_box_2d_sptr>::iterator rit = regions_.begin();
       rit != regions_.end(); rit++)
    temp.push_back(this->clip_to_image_bounds(*rit));
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
  vcl_cerr << "brip_roi::remove_region(" << i << ") NYI\n";
  return false;
}

