#include "easy2D_loader.h"

easy2D_loader::easy2D_loader(vgui_easy2D_tableau_sptr const& e2d)
{
  easy_ = e2d;
}

bool easy2D_loader::point(const char *type, float x,float y)
{
  if (type[0] == 'p')
    easy_->add_point(x,y);
  if (type[0] == '+')
  {
    easy_->add_line(x-5,y,x+5,y);
    easy_->add_line(x,y-5,x,y+5);
  }
  return true;
}

bool easy2D_loader::polyline(float const *x,float const *y,int n)
{
  for (int i = 1;i<n;i++)
    easy_->add_line(x[i-1],y[i-1],x[i],y[i]);
  return true;
}

bool easy2D_loader::set_color(float r,float g,float b)
{
  color[0] = r;
  color[1] = g;
  color[2] = b;
  easy_->set_foreground(r,g,b);
  return true;
}

bool easy2D_loader::set_point_radius(float r)
{
  easy_->set_point_radius(r);
  return true;
}

bool easy2D_loader::set_line_width(float w)
{
  easy_->set_line_width(w);
  return true;
}
