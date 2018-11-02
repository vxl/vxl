// This is core/vgl/vgl_polygon.hxx
#ifndef vgl_polygon_hxx_
#define vgl_polygon_hxx_

#include <iostream>
#include <set>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <string>
#include "vgl_polygon.h"
#include <vnl/vnl_math.h>
//:
// \file

#include "vgl_intersection.h"
#include "vgl_line_2d.h"
#include "vgl_tolerance.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

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
    int n = int(pgon.size());
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
std::ostream& vgl_polygon<T>::print(std::ostream& os) const
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
      else{
        os << " nverts = " << sheets_[s].size() << '\n';
        for (unsigned int p = 0; p < sheets_[s].size(); ++p)
        os << "( " << sheets_[s][p].x() << " , " << sheets_[s][p].y() << " ) ";
        os << '\n';
      }
    }
  }
  return os;
}
template <class T>
std::istream& vgl_polygon<T>::read(std::istream& is){
  std::string s;
  is >> s;
  if(s == "Empty polygon")
    return is;
  is >> s; // skip "with"
  unsigned n_sheets;
  is >> n_sheets;
  if(n_sheets == 0)
    return is;
  is >> s; // Skip  "sheets:"
  unsigned k;
  sheets_.resize(n_sheets);
  for(unsigned sh = 0; sh<n_sheets; ++sh){
    is >> s; // Sheet
    is >> k;
    is >> s; // nverts or empty
         if(s == "(empty)")
      return is;
    is >> s;// =
    unsigned nv;
    is >> nv;
    for(unsigned iv = 0; iv<nv; ++iv){
      T x, y;
      is >> s; //'('
      is >> x ;
      is >> s; //','
      is >> y;
      is >> s; //')'
      vgl_point_2d<T> pt(x, y);
      sheets_[sh].push_back(pt);
    }
  }
  return is;
}

template <class T>
vgl_polygon_sheet_as_array<T>::vgl_polygon_sheet_as_array(vgl_polygon<T> const& p)
{
  assert(p.num_sheets()==1);
  n = int(p[0].size());
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
  n = int(p.size());
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
                           std::vector<std::pair<unsigned int,unsigned int> >& e1,
                           std::vector<std::pair<unsigned int,unsigned int> >& e2,
                           std::vector<vgl_point_2d<T> >& ip)
{
  const T tol = std::sqrt(vgl_tolerance<T>::position);
  std::vector<unsigned int> offsets(p.num_sheets()+1,0);
  for (unsigned int s = 0; s < p.num_sheets(); ++s) {
    offsets[s+1] = offsets[s]+(unsigned int)(p[s].size());
  }
  e1.clear();
  e2.clear();
  ip.clear();

  typedef std::pair<unsigned int, unsigned int> upair;
  std::set<upair> isect_set;
  for (unsigned int s1 = 0; s1 < p.num_sheets(); ++s1)
  {
    const typename vgl_polygon<T>::sheet_t& sheet1 = p[s1];
    if (sheet1.size() < 2)
      continue;
    for (unsigned int i1=(unsigned int)(sheet1.size()-1), i1n=0; i1n < sheet1.size(); i1=i1n, ++i1n)
    {
      const vgl_point_2d<T>& v1 = sheet1[i1];
      const vgl_point_2d<T>& v2 = sheet1[i1n];
      // coefficients for linear equation for testing intersections
      // for (x,y) if cx*x+cy*y+c changes sign then we have intersection
      T cx = v1.y()-v2.y();
      T cy = v2.x()-v1.x();
      T c = v1.x()*v2.y()-v2.x()*v1.y();
      T norm = 1/std::sqrt(cx*cx+cy*cy);
      cx *= norm;
      cy *= norm;
      c *= norm;

      unsigned int idx1 = offsets[s1]+i1;

      // inner loop - compare against self
      for (unsigned int s2 = 0; s2 < p.num_sheets(); ++s2)
      {
        const typename vgl_polygon<T>::sheet_t& sheet2 = p[s2];
        const unsigned int size2 = (unsigned int)(sheet2.size());
        if (size2 < 2)
          continue;
        unsigned int start = 0, end = 0;
        if (s1 == s2) {
          start = (i1n+1)%size2;
          end = (i1+size2-1)%size2;
        }

        T dist = cx*sheet2[start].x()+cy*sheet2[start].y()+c;
        bool last_sign = dist > 0;
        bool last_zero = std::abs(dist) <= tol;
        bool first = true;
        for (unsigned int i2=start, i2n=(start+1)%size2; i2!=end || first; i2=i2n, i2n=(i2n+1)%size2)
        {
          const vgl_point_2d<T>& v3 = sheet2[i2];
          const vgl_point_2d<T>& v4 = sheet2[i2n];
          dist = cx*v4.x()+cy*v4.y()+c;
          bool sign = dist > 0;
          bool zero = std::abs(dist) <= tol;
          if (sign != last_sign || zero || last_zero)
          {
            unsigned int idx2 = offsets[s2]+i2;
            upair pair_idx(idx1,idx2);
            if (pair_idx.first > pair_idx.second) {
              pair_idx = upair(idx2,idx1);
            }
            std::set<upair>::iterator f = isect_set.find(pair_idx);
            if (f == isect_set.end())
              isect_set.insert(pair_idx);
            // use vgl_intersection to verify some degenerate false positives
            else if (vgl_intersection(v1,v2,v3,v4,tol)) {
              // make intersection point
              vgl_point_2d<T> ipt;
              if (!vgl_intersection(vgl_line_2d<T>(v1,v2),vgl_line_2d<T>(v3,v4),ipt)
                  || parallel(v2-v1,v4-v3,tol)) // vgl_intersection test is not accurate enough
              {
                // use the median point when lines are parallel
                vgl_vector_2d<T> dir = v2-v1;
                normalize(dir);
                T t1 = 0;
                T t2 = dot_product(dir,v2-v1);
                T t3 = dot_product(dir,v3-v1);
                T t4 = dot_product(dir,v4-v1);
                T t = t1+t2+t3+t4;
                t -= std::min(std::min(t1,t2),std::min(t3,t4));
                t -= std::max(std::max(t1,t2),std::max(t3,t4));
                t /= 2;
                ipt = v1 + t*dir;
              }
              e1.emplace_back(s2,i2);
              e2.emplace_back(s1,i1);
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

//turn all sheets into counterclockwise polygons
template <class T>
vgl_polygon<T> vgl_reorient_polygon(vgl_polygon<T> const &p)
{
  vgl_polygon<T> outPolygon;
  //std::vector<std::vector<vgl_point_2d<T> > > outPolySheets;
  std::vector<std::pair<unsigned,unsigned> > e1, e2;
  std::vector<vgl_point_2d<T> > ip;
  vgl_selfintersections(p, e1, e2, ip);
  if(!e1.empty() && !e2.empty() && !ip.empty())
  {
    //Self intersecting/non simple polygons cannot be said to be clockwise or not
    //hence they are not added and we get an empty polygon.
    std::cout<< "WARNING - self intersecting polygon"<<std::endl;
    return p;
  }
  else
  {
    for(size_t i = 0; i < p.num_sheets();i++)
    {
      std::vector<vgl_point_2d<T> > verts = p[i];
      if(vgl_polygon_sheet_is_counter_clockwise(verts))
      {
        outPolygon.push_back(verts);
      }
      else
      {
        if(i == 0)
        {std::reverse(verts.begin(), verts.end());}
        outPolygon.push_back(verts);
      }
    }
  }
  return outPolygon;
}

template <class T>
bool vgl_polygon_sheet_is_counter_clockwise(std::vector<vgl_point_2d<T> > verts)
{
  double sum = 0.0;
  vgl_point_2d<T> v1;
  vgl_point_2d<T> v2;
  double term1, term2;
  for(size_t x = 1; x <verts.size(); x++)
  {
    v1 = verts.at(x-1);
    v2 = verts.at(x);
    term1 = double(v2.x())-double(v1.x());
    term2 = double(v2.y())+double(v1.y());
    sum += term1*term2;
  }
  v1 = verts.at(verts.size()-1);
  v2 = verts.at(0);
  term1 = double(v2.x())-double(v1.x());
  term2 = double(v2.y())+double(v1.y());
  sum += term1*term2;
  return sum < 0.0;

}


#undef VGL_POLYGON_INSTANTIATE
#define VGL_POLYGON_INSTANTIATE(T) \
template class vgl_polygon<T >; \
template struct vgl_polygon_sheet_as_array<T >; \
template std::ostream& operator<<(std::ostream&,vgl_polygon<T > const&); \
template std::istream& operator>>(std::istream&,vgl_polygon<T >&); \
template void vgl_selfintersections(vgl_polygon<T > const& p, \
                                    std::vector<std::pair<unsigned int,unsigned int> >& e1, \
                                    std::vector<std::pair<unsigned int,unsigned int> >& e2, \
                                    std::vector<vgl_point_2d<T > >& ip); \
template vgl_polygon<T> vgl_reorient_polygon(vgl_polygon<T > const &p); \
template bool vgl_polygon_sheet_is_counter_clockwise(std::vector<vgl_point_2d<T > > verts)

#endif // vgl_polygon_hxx_
