#ifdef __GNUC__
#pragma implementation
#endif

#include "vgl_polygon.h"
#include <vcl_cassert.h>

vgl_polygon::vgl_polygon()
{
}

vgl_polygon::vgl_polygon(int num_sheets):
  sheets_(num_sheets, sheet_t())
{
}

vgl_polygon::vgl_polygon(vcl_vector<sheet_t> const& sheets):
  sheets_(sheets)
{
}

vgl_polygon::vgl_polygon(float const* x, float const* y, int n):
  sheets_(1, sheet_t(n))
{
  for(int i = 0; i < n; ++i)
    sheets_[0][i].set(x[i], y[i]);
}

vgl_polygon::vgl_polygon(vcl_vector<point_t> const& points):
  sheets_(1)
{
  sheets_[0] = points;
}

vgl_polygon::vgl_polygon(vgl_polygon const& a)
{
  sheets_ = a.sheets_;
}

vgl_polygon::~vgl_polygon()
{
}
  
void vgl_polygon::new_sheet()
{
  sheets_.push_back(sheet_t());
}

//: Add (x,y) to current sheet
void vgl_polygon::push_back(float x, float y)
{
  sheets_[sheets_.size()-1].push_back(point_t(x,y));
}

void vgl_polygon::push_back(point_t const& p)
{
  sheets_[sheets_.size()-1].push_back(p);
}
 
void vgl_polygon::push_back(sheet_t const& s)
{
  sheets_.push_back(s);
}

static
bool point_in_simple_polygon(float x, float y, vgl_polygon::sheet_t const& pgon)
{
  int n = pgon.size();

  bool c = false;

  int i = 0;
  int j = n - 1;
  for (; i < n; j = i++) 
    if ((((pgon[i].y()<=y) && (y<pgon[j].y())) || ((pgon[j].y()<=y) && (y<pgon[i].y()))) &&
	(x < (pgon[j].x() - pgon[i].x()) * (y - pgon[i].y()) / (pgon[j].y() - pgon[i].y()) + pgon[i].x()))
      c = !c;
  return c;
}

bool vgl_polygon::contains(float tx, float ty)
{
  assert(sheets_.size() == 1);
  return point_in_simple_polygon(tx, ty, sheets_[0]);
}

vcl_ostream& vgl_polygon::print(vcl_ostream& os) const {
  for (int s = 0; s < sheets_.size(); ++s) {
    os << "Sheet " << s << " ";
    for (int p = 0; p < sheets_[s].size(); ++p)
      os << "(" << sheets_[s][p].x() << "," << sheets_[s][p].y() << ") ";
    os << vcl_endl;
  }
  return os;
}

vgl_polygon_sheet_as_array::vgl_polygon_sheet_as_array(vgl_polygon::sheet_t const& p)
{
  n = p.size();
  x = new float[n*2];
  y = x + n;

  for(int v = 0; v < n; ++v) {
    x[v] = p[v].x();
    y[v] = p[v].y();
  }
}
  
vgl_polygon_sheet_as_array::~vgl_polygon_sheet_as_array()
{
  delete [] x;
  // do not delete [] y; only one alloc in ctor.
}
