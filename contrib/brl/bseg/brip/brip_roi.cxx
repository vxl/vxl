#include <vsol/vsol_box_2d.h>
#include <brip/brip_roi.h>

brip_roi::brip_roi(const int n_image_cols, const int n_image_rows)
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
  if (y0 < 0)
    y0 = 0;
  if (x0 > n_image_cols_-1)
    x0 = n_image_cols_-1;
  if (y0 > n_image_rows_-1)
    y0 = n_image_rows_-1;
  if (xm < 0)
    xm = 0;
  if (xm > n_image_cols_-1)
    xm = n_image_cols_-1;
  if (ym < 0)
    ym = 0;
  if (ym > n_image_rows_-1)
    ym = n_image_rows_-1;
  vsol_box_2d_sptr cbox = new vsol_box_2d();
  cbox->add_point(x0, y0);
  cbox->add_point(xm, y0);
  cbox->add_point(xm, ym);
  cbox->add_point(x0, ym);
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
bool brip_roi::empty()
{
  if (!regions_.size())
    return true;
#if 0//later
  for (vcl_vector<vgl_box_2d<int> >::iterator rit = regions_.begin();
       rit != regions_.end(); rit++)
    if (!(*rit).is_empty())
      return false;
#endif
  return false;
}

void brip_roi::clip_to_image_bounds()
{
  vcl_vector<vsol_box_2d_sptr> temp;
  for (vcl_vector<vsol_box_2d_sptr>::iterator rit = regions_.begin();
       rit != regions_.end(); rit++)
    temp.push_back(this->clip_to_image_bounds(*rit));
  regions_ = temp;
}

int brip_roi::cmin(int i)
{
  if (i<0||i>=int(regions_.size()))
    return 0;
  else
    return (int)regions_[i]->get_min_x();
}

int brip_roi::cmax(int i)
{
  if (i<0||i>=int(regions_.size()))
    return 0;
  else
    return (int)regions_[i]->get_max_x();
}

int brip_roi::rmin(int i)
{
  if (i<0||i>=int(regions_.size()))
    return 0;
  else
    return (int)regions_[i]->get_min_y();
}

int brip_roi::rmax(int i)
{
  if (i<0||i>=int(regions_.size()))
    return 0;
  else
    return (int)regions_[i]->get_max_y();
}

int brip_roi::ic(int col, int i)
{
  return col + cmin(i);
}

int brip_roi::ir(int row, int i)
{
  return row + rmin(i);
}

bool brip_roi::remove_region(int i)
{
  vcl_cerr << "brip_roi::remove_region(" << i << ") NYI\n";
  return false;
}

