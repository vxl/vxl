// This is core/vgl/vgl_polygon.txx
#ifndef vgl_polygon_txx_
#define vgl_polygon_txx_

#include "vgl_polygon.h"

#include <vcl_iostream.h>
#include <vcl_cassert.h>

// Constructors/Destructor---------------------------------------------------

template <class T>
vgl_polygon<T>::vgl_polygon(vgl_point_2d<T> const p[], int n):
  sheets_(1, sheet_t(n))
{
  for (int i = 0; i < n; ++i)
    sheets_[0][i].set(p[i].x(), p[i].y());
}

template <class T>
vgl_polygon<T>::vgl_polygon(T const* x, T const* y, int n)
  : sheets_(1, sheet_t(n))
{
  for (int i = 0; i < n; ++i)
    sheets_[0][i].set(x[i], y[i]);
}

template <class T>
vgl_polygon<T>::vgl_polygon(T const x_y[], int n)
  : sheets_(1, sheet_t(n))
{
  for (int i = 0; i < n; ++i)
    sheets_[0][i].set(x_y[2*i], x_y[2*i+1]);
}

template <class T>
void vgl_polygon<T>::add_contour(point_t const p[], int n)
{
  sheet_t s(n);
  for (int i = 0; i < n; ++i)
    s[i].set(p[i].x(), p[i].y());
  sheets_.push_back(s);
}

template <class T>
void vgl_polygon<T>::add_contour(T const* x, T const* y, int n)
{
  sheet_t s(n);
  for (int i = 0; i < n; ++i)
    s[i].set(x[i], y[i]);
  sheets_.push_back(s);
}

template <class T>
void vgl_polygon<T>::add_contour(T const x_y[], int n)
{
  sheet_t s(n);
  for (int i = 0; i < n; ++i)
    s[i].set(x_y[2*i], x_y[2*i+1]);
  sheets_.push_back(s);
}

// Functions -----------------------------------------------------------------

template <class T>
void vgl_polygon<T>::push_back(T x, T y)
{
  sheets_[sheets_.size()-1].push_back(point_t(x,y));
}

template <class T>
void vgl_polygon<T>::push_back(point_t const& p)
{
  sheets_[sheets_.size()-1].push_back(p);
}

template <class T>
bool vgl_polygon<T>::contains(T x, T y) const
{
  bool c = false;
  for (unsigned int s=0; s < sheets_.size(); ++s)
  {
    sheet_t const& pgon = sheets_[s];
    int n = pgon.size();
    for (int i = 0, j = n-1; i < n; j = i++)
    {
      // by definition, corner points and edge points are inside the polygon:
      if ((pgon[j].x() - x) * (pgon[i].y() - y) == (pgon[i].x() - x) * (pgon[j].y() - y) &&
          (((pgon[i].x()<=x) && (x<=pgon[j].x())) || ((pgon[j].x()<=x) && (x<=pgon[i].x()))) &&
          (((pgon[i].y()<=y) && (y<=pgon[j].y())) || ((pgon[j].y()<=y) && (y<=pgon[i].y()))))
        return true;
      // invert c for each edge crossing:
      if ((((pgon[i].y()<=y) && (y<pgon[j].y())) || ((pgon[j].y()<=y) && (y<pgon[i].y()))) &&
          (x < (pgon[j].x() - pgon[i].x()) * (y - pgon[i].y()) / (pgon[j].y() - pgon[i].y()) + pgon[i].x()))
        c = !c;
    }
  }
  return c;
}

template <class T>
vcl_ostream& vgl_polygon<T>::print(vcl_ostream& os) const
{
  if (sheets_.size() == 0)
    os << "Empty polygon\n";
  else {
    os << "Polygon with " << num_sheets() << " sheets:\n";
    for (unsigned int s = 0; s < sheets_.size(); ++s)
    {
      os << "Sheet " << s << ' ';
      if (sheets_[s].size()==0)
        os << "(empty)";
      else
      for (unsigned int p = 0; p < sheets_[s].size(); ++p)
        os << '(' << sheets_[s][p].x() << ',' << sheets_[s][p].y() << ") ";
      os << '\n';
    }
  }
  return os;
}

template <class T>
vgl_polygon_sheet_as_array<T>::vgl_polygon_sheet_as_array(vgl_polygon<T> const& p)
{
  assert(p.num_sheets()==1);
  n = p[0].size();
  x = new T[n*2];
  y = x + n;
  for (int v = 0; v < n; ++v)
  {
    x[v] = p[0][v].x();
    y[v] = p[0][v].y();
  }
}

template <class T>
vgl_polygon_sheet_as_array<T>::vgl_polygon_sheet_as_array(typename vgl_polygon<T>::sheet_t const& p)
{
  n = p.size();
  x = new T[n*2];
  y = x + n;
  for (int v = 0; v < n; ++v)
  {
    x[v] = p[v].x();
    y[v] = p[v].y();
  }
}

template <class T>
vgl_polygon_sheet_as_array<T>::~vgl_polygon_sheet_as_array()
{
  delete [] x;
  // do not delete [] y; only one alloc in ctor.
}

#undef VGL_POLYGON_INSTANTIATE
#define VGL_POLYGON_INSTANTIATE(T) \
template class vgl_polygon<T >; \
template struct vgl_polygon_sheet_as_array<T >; \
template vcl_ostream& operator<<(vcl_ostream&,vgl_polygon<T >const&)

#endif // vgl_polygon_txx_
