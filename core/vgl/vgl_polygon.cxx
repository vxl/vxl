// This is vxl/vgl/vgl_polygon.cxx

//--------------------------------------------------------------
//
// Class : vgl_polygon
//
//--------------------------------------------------------------

#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vgl_polygon.h"
#include <vcl_iostream.h>

// Constructors/Destructor---------------------------------------------------

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vgl_polygon::vgl_polygon()
{
}

//---------------------------------------------------------------------------
// Construct with specified number of sheets/regions
//---------------------------------------------------------------------------
vgl_polygon::vgl_polygon(int num_sheets):
  sheets_(num_sheets, sheet_t())
{
}

//---------------------------------------------------------------------------
// Construct with a set of sheets
//---------------------------------------------------------------------------
vgl_polygon::vgl_polygon(vcl_vector<sheet_t> const& sheets):
  sheets_(sheets)
{
}

//---------------------------------------------------------------------------
// Construct with n points in the first sheet
//---------------------------------------------------------------------------
vgl_polygon::vgl_polygon(vcl_pair<float, float> const p[], int n):
  sheets_(1, sheet_t(n))
{
  for (int i = 0; i < n; ++i)
    sheets_[0][i].set(p[i].first, p[i].second);
}

//---------------------------------------------------------------------------
// Construct with n points in the first sheet
//---------------------------------------------------------------------------
vgl_polygon::vgl_polygon(vcl_pair<double, double> const p[], int n):
  sheets_(1, sheet_t(n))
{
  for (int i = 0; i < n; ++i)
    sheets_[0][i].set(float(p[i].first), float(p[i].second));
}

//---------------------------------------------------------------------------
// Construct with n points in the first sheet
//---------------------------------------------------------------------------
vgl_polygon::vgl_polygon(float const* x, float const* y, int n):
  sheets_(1, sheet_t(n))
{
  for (int i = 0; i < n; ++i)
    sheets_[0][i].set(x[i], y[i]);
}

//---------------------------------------------------------------------------
// Construct with points in the first sheet
//---------------------------------------------------------------------------
vgl_polygon::vgl_polygon(sheet_t const& points):
  sheets_(1)
{
  sheets_[0] = points;
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vgl_polygon::vgl_polygon(vgl_polygon const& a)
{
  sheets_ = a.sheets_;
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vgl_polygon::~vgl_polygon()
{
}

// Functions -----------------------------------------------------------------

//---------------------------------------------------------------------------
// Reset the sheets, so its empty
//---------------------------------------------------------------------------
void vgl_polygon::clear()
{
  sheets_.resize(0);
}

//---------------------------------------------------------------------------
// Add a blank new sheet
//---------------------------------------------------------------------------
void vgl_polygon::new_sheet()
{
  sheets_.push_back(sheet_t());
}

//---------------------------------------------------------------------------
// Add (x,y) to current sheet
//---------------------------------------------------------------------------
void vgl_polygon::push_back(float x, float y)
{
  sheets_[sheets_.size()-1].push_back(point_t(x,y));
}

//---------------------------------------------------------------------------
// Add point to current sheet
//---------------------------------------------------------------------------
void vgl_polygon::push_back(point_t const& p)
{
  sheets_[sheets_.size()-1].push_back(p);
}

//---------------------------------------------------------------------------
// Add a new sheet
//---------------------------------------------------------------------------
void vgl_polygon::push_back(sheet_t const& s)
{
  sheets_.push_back(s);
}

//---------------------------------------------------------------------------
// Determine whether (x,y) is in polygon
//---------------------------------------------------------------------------
bool vgl_polygon::contains(float x, float y) const
{
  bool c = false;
  for (unsigned int s=0; s < sheets_.size(); ++s) {
    vgl_polygon::sheet_t const& pgon = sheets_[s];
    int n = pgon.size();

    int i = 0;
    int j = n - 1;
    for (; i < n; j = i++)
      if ((((pgon[i].y()<=y) && (y<pgon[j].y())) || ((pgon[j].y()<=y) && (y<pgon[i].y()))) &&
          (x < (pgon[j].x() - pgon[i].x()) * (y - pgon[i].y()) / (pgon[j].y() - pgon[i].y()) + pgon[i].x()))
        c = !c;
  }
  return c;
}


vcl_ostream& vgl_polygon::print(vcl_ostream& os) const {
  for (unsigned int s = 0; s < sheets_.size(); ++s) {
    os << "Sheet " << s << " ";
    for (unsigned int p = 0; p < sheets_[s].size(); ++p)
      os << "(" << sheets_[s][p].x() << "," << sheets_[s][p].y() << ") ";
    os << vcl_endl;
  }
  return os;
}

vcl_ostream& operator<< (vcl_ostream& os, vgl_polygon const& p) {
  return p.print(os);
}


//---------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------
vgl_polygon_sheet_as_array::vgl_polygon_sheet_as_array(vgl_polygon::sheet_t const& p)
{
  n = p.size();
  x = new float[n*2];
  y = x + n;

  for (int v = 0; v < n; ++v) {
    x[v] = p[v].x();
    y[v] = p[v].y();
  }
}


//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vgl_polygon_sheet_as_array::~vgl_polygon_sheet_as_array()
{
  delete [] x;
  // do not delete [] y; only one alloc in ctor.
}
