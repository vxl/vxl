// This is core/vgl/vgl_polygon.txx
#ifndef vgl_polygon_txx_
#define vgl_polygon_txx_

#include "vgl_polygon.h"
//:
// \file

#include "vgl_intersection.h"
#include "vgl_line_2d.h"
#include "vgl_tolerance.h"

#include <vcl_iostream.h>
#include <vcl_set.h>
#include <vcl_cmath.h>
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
      const vgl_point_2d<T> &p_i = pgon[i];
      const vgl_point_2d<T> &p_j = pgon[j];

      // by definition, corner points and edge points are inside the polygon:
      if ((p_j.x() - x) * (p_i.y() - y) == (p_i.x() - x) * (p_j.y() - y) &&
          (((p_i.x()<=x) && (x<=p_j.x())) || ((p_j.x()<=x) && (x<=p_i.x()))) &&
          (((p_i.y()<=y) && (y<=p_j.y())) || ((p_j.y()<=y) && (y<=p_i.y()))))
        return true;
      // invert c for each edge crossing:
      if ((((p_i.y()<=y) && (y<p_j.y())) || ((p_j.y()<=y) && (y<p_i.y()))) &&
          (x < (p_j.x() - p_i.x()) * (y - p_i.y()) / (p_j.y() - p_i.y()) + p_i.x()))
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

//: Compute all self-intersections between all edges on all sheets.
// \returns three arrays \a e1, \a e2, and \a ip of equal size.
// Corresponding elements from these arrays describe an intersection.
// e1[k].first is the sheet index containing edge (e1[k].second, e1[k].second+1)
// involved in the k-th intersection.  Similarly, e2[k] indexes the other
// edge involved in the k-th intersection.  The corresponding intersection
// point is returned in ip[k].
template <class T>
void vgl_selfintersections(vgl_polygon<T> const& p,
                           vcl_vector<vcl_pair<unsigned,unsigned> >& e1,
                           vcl_vector<vcl_pair<unsigned,unsigned> >& e2,
                           vcl_vector<vgl_point_2d<T> >& ip)
{
  const T tol = vcl_sqrt(vgl_tolerance<T>::position);
  vcl_vector<unsigned> offsets(p.num_sheets()+1,0);
  for (unsigned s = 0; s < p.num_sheets(); ++s) {
    offsets[s+1] = offsets[s]+p[s].size();
  }
  e1.clear();
  e2.clear();
  ip.clear();

  typedef vcl_pair<unsigned, unsigned> upair;
  vcl_set<upair> isect_set;
  for (unsigned s1 = 0; s1 < p.num_sheets(); ++s1)
  {
    const typename vgl_polygon<T>::sheet_t& sheet1 = p[s1];
    if (sheet1.size() < 2)
      continue;
    for (unsigned i1=sheet1.size()-1, i1n=0; i1n < sheet1.size(); i1=i1n, ++i1n)
    {
      const vgl_point_2d<T>& v1 = sheet1[i1];
      const vgl_point_2d<T>& v2 = sheet1[i1n];
      // coefficients for linear equation for testing intersections
      // for (x,y) if cx*x+cy*y+c changes sign then we have intersection
      T cx = v1.y()-v2.y();
      T cy = v2.x()-v1.x();
      T c = v1.x()*v2.y()-v2.x()*v1.y();
      T norm = 1/vcl_sqrt(cx*cx+cy*cy);
      cx *= norm;
      cy *= norm;
      c *= norm;

      unsigned idx1 = offsets[s1]+i1;

      // inner loop - compare against self
      for (unsigned s2 = 0; s2 < p.num_sheets(); ++s2)
      {
        const typename vgl_polygon<T>::sheet_t& sheet2 = p[s2];
        const unsigned size2 = sheet2.size();
        if (size2 < 2)
          continue;
        unsigned start = 0;
        unsigned end = 0;
        if (s1 == s2) {
          start = (i1n+1)%size2;
          end = (i1+size2-1)%size2;
        }

        T dist = cx*sheet2[start].x()+cy*sheet2[start].y()+c;
        bool last_sign = dist > 0;
        bool last_zero = vcl_abs(dist) <= tol;
        bool first = true;
        for (unsigned i2=start, i2n=(start+1)%size2; i2!=end || first; i2=i2n, i2n=(i2n+1)%size2)
        {
          const vgl_point_2d<T>& v3 = sheet2[i2];
          const vgl_point_2d<T>& v4 = sheet2[i2n];
          dist = cx*v4.x()+cy*v4.y()+c;
          bool sign = dist > 0;
          bool zero = vcl_abs(dist) <= tol;
          if (sign != last_sign || zero || last_zero)
          {
            unsigned idx2 = offsets[s2]+i2;
            upair pair_idx(idx1,idx2);
            if (pair_idx.first > pair_idx.second) {
              pair_idx = upair(idx2,idx1);
            }
            vcl_set<upair>::iterator f = isect_set.find(pair_idx);
            if (f == isect_set.end())
              isect_set.insert(pair_idx);
            // use vgl_intersection to verify some degenerate false positives
            else if (vgl_intersection(v1,v2,v3,v4,tol)) {
              // make intersection point
              e1.push_back(upair(s2,i2));
              e2.push_back(upair(s1,i1));
              vgl_point_2d<T> ipt;
              if (!vgl_intersection(vgl_line_2d<T>(v1,v2),vgl_line_2d<T>(v3,v4),ipt))
              {
                vcl_cerr<< "warning: ill-defined intersection, using mid-point\n";
                ipt.set((v1.x()+v2.x()+v3.x()+v4.x())/4, (v1.y()+v2.y()+v3.y()+v4.y())/4);
              }
              ip.push_back(ipt);
            }
          }
          last_sign = sign;
          last_zero = zero;
          first = false;
        }
      }
    }
  }
}


#undef VGL_POLYGON_INSTANTIATE
#define VGL_POLYGON_INSTANTIATE(T) \
template class vgl_polygon<T >; \
template struct vgl_polygon_sheet_as_array<T >; \
template vcl_ostream& operator<<(vcl_ostream&,vgl_polygon<T >const&); \
template void vgl_selfintersections(vgl_polygon<T > const& p, \
                           vcl_vector<vcl_pair<unsigned,unsigned> >& e1, \
                           vcl_vector<vcl_pair<unsigned,unsigned> >& e2, \
                           vcl_vector<vgl_point_2d<T > >& ip)

#endif // vgl_polygon_txx_
