#ifdef __GNUC__
#pragma implementation
#endif

#include "HomgInterestPointSet.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_vector.h>

#include <vil/vil_memory_image_of.h>

#include <mvl/HomgInterestPoint.h>
#include <mvl/ImageMetric.h>

class HomgInterestPointSetData : public vcl_vector<HomgInterestPoint> {
public:
  HomgInterestPointSetData() {}
  HomgInterestPointSetData(int n):
    vcl_vector<HomgInterestPoint>(n, HomgInterestPoint())
  {}
  ~HomgInterestPointSetData() { }
};

//: Construct an empty corner set.
HomgInterestPointSet::HomgInterestPointSet()
{
  _data = new HomgInterestPointSetData;

  init_conditioner(0);
}

//: Construct an empty corner set which will use the given conditioner to
// convert from image to homogeneous coordinates.
HomgInterestPointSet::HomgInterestPointSet(const HomgMetric& c)
{
  _data = new HomgInterestPointSetData;

  init_conditioner(c);
}

//: Load corners from ASCII disk file
HomgInterestPointSet::HomgInterestPointSet(const char* filename, const HomgMetric& c)
{
  _data = 0;
  read(filename, c);
  init_conditioner(c);
}

//: Construct corner set from container of HomgPoint2D, and set the conditioner.
// The HomgPoint2Ds are assumed to already be in conditioned coordinates.
HomgInterestPointSet::HomgInterestPointSet(const vcl_vector<HomgPoint2D>& points, ImageMetric* conditioner)
{
  unsigned n = points.size();
  if (n > 0)
    _data = new HomgInterestPointSetData(n);
  else
    _data = new HomgInterestPointSetData();

  for(unsigned i = 0; i < n; ++i)
    (*_data)[i] = HomgInterestPoint(points[i], conditioner, 0.0f);

  _conditioner = conditioner;
}

// - Untested
HomgInterestPointSet::HomgInterestPointSet(const HomgInterestPointSet& that)
{
  unsigned n = that._data->size();
  if (n > 0)
    _data = new HomgInterestPointSetData(n);
  else
    _data = new HomgInterestPointSetData();

  vcl_cerr << "HomgInterestPointSet::copy ctor: size " << n << vcl_endl;

  for (unsigned i = 0; i < n; ++i)
    (*_data)[i] = (*that._data)[i];
  _conditioner = that._conditioner;
}

// - Untested
HomgInterestPointSet& HomgInterestPointSet::operator=(const HomgInterestPointSet& that)
{
  unsigned n = that._data->size();
  delete _data;
  if (n > 0)
    _data = new HomgInterestPointSetData(n);
  else
    _data = new HomgInterestPointSetData();
  for(unsigned i = 0; i < n; ++i) {
    (*_data)[i] = (*that._data)[i];
  }
  _conditioner = that._conditioner;
  return *this;
}

//: Set conditioner
void HomgInterestPointSet::set_conditioner(const HomgMetric& c)
{
  delete_conditioner();
  init_conditioner(c);
}

void HomgInterestPointSet::init_conditioner(const HomgMetric& c)
{
  _conditioner = c;
}

void HomgInterestPointSet::delete_conditioner()
{
}

//: Clear corner set.
void HomgInterestPointSet::clear()
{
  delete _data;
  _data = new HomgInterestPointSetData;
  set_conditioner(0);
}

//: Destructor
HomgInterestPointSet::~HomgInterestPointSet()
{
  delete _data;
}

// Operations----------------------------------------------------------------
//
//: Add a corner to the end of the list
bool HomgInterestPointSet::add(const HomgPoint2D& c)
{
  double x, y;
  c.get_nonhomogeneous(x,y);
  return add(x,y);
}
bool HomgInterestPointSet::add(const HomgInterestPoint& c)
{
  return add(c._homg);
}
//
//: Add corner (x, y), using ImageMetric to convert to homogeneous.
bool HomgInterestPointSet::add(double x, double y)
{
  _data->push_back(HomgInterestPoint(x, y, _conditioner));
  return true;
}

//: Add a corner which has already been preconditioned by this cornerset's
// imagemetric.
bool HomgInterestPointSet::add_preconditioned(const HomgPoint2D& h)
{
  double x, y;
  _conditioner.homg_to_image(h, &x, &y);
  return add(x, y);
}

//: Return the number of corners in the set.
unsigned HomgInterestPointSet::size() const
{
  return _data->size();
}

// @{ACCESSORS@}

//: Return a reference to the i'th corner structure
const HomgInterestPoint& HomgInterestPointSet::get(int i) const
{
  return (*_data)[i];
}

HomgInterestPoint& HomgInterestPointSet::get(int i)
{
  return (*_data)[i];
}

//: Return the i'th corner as a 2D point
vnl_double_2 const& HomgInterestPointSet::get_2d(int i) const
{
  return (*_data)[i]._double2;
}

//: Return the i'th corner as a 2D point
vnl_vector_fixed<int,2> const& HomgInterestPointSet::get_int(int i) const
{
  return (*_data)[i]._int2;
}

//: Return the i'th corner as a HomgPoint2D
const HomgPoint2D& HomgInterestPointSet::get_homg(int i) const
{
  assert(i >= 0 && i < int(_data->size()));
  return (*_data)[i]._homg;
}

//: Return the i'th mean intensity
float HomgInterestPointSet::get_mean_intensity(int i) const
{
  assert(i >= 0 && i < int(_data->size()));
  float v = (*_data)[i]._mean_intensity;
  if (v == 0.0F) {
    vcl_cerr << "HomgInterestPointSet: WARNING mean_intensity["<<i<<"] = 0\n";
  }
  return v;
}

// Input/Output--------------------------------------------------------------

// @{ INPUT/OUTPUT @}

//: Load a corner set from a simple ASCII file of x y pairs.
// If ImageMetric is supplied, it is used to convert image coordinates to
// homogeneous form.
bool HomgInterestPointSet::read(const char* filename, const HomgMetric& c)
{
  vcl_ifstream f(filename);
  if (!f.good()) {
    vcl_cerr << "HomgInterestPointSet::read() -- Failed to open \"" << filename << "\"\n";
    return false;
  }

  return read(f, c);
}

bool HomgInterestPointSet::read(vcl_istream& f, const ImageMetric* c)
{
  clear();

  set_conditioner(c);

  while (f.good()) {
    double x, y;
    f >> x >> y;
    if (!f.good())
      break;
    add(x, y);
    f >> vcl_ws;
  }
  return true;
}

//: Load a corner set from a simple ASCII file of x y pairs,
// and use Image to compute mean_intensities.
// If ImageMetric is supplied, it is used to convert image coordinates to
// homogeneous form.
bool HomgInterestPointSet::read(const char* filename, vil_image const& src, const HomgMetric& c)
{
  if (!read(filename, c))
    return false;

#if 1
  vcl_cerr << "HomgInterestPointSet::read() not implemented in any sense of the word\n";
#else
  //vcl_cerr << "HomgInterestPointSet: Computing mean intensities\n";
  vil_memory_image_of<unsigned char> imbuf(src);
  for (unsigned i=0; i< size(); i++) {
    ImageWindowOps winops(imbuf, get_int(i), 3);
    (*_data)[i]._mean_intensity = winops.mean_intensity();
    if ((*_data)[i]._mean_intensity == 0.0F) {
      //vcl_cerr << " note " << i << " had mi of 0\n";
      (*_data)[i]._mean_intensity = 1e6;
    }
  }
#endif

  return true;
}

//: Save a corner set as a simple ASCII file of x y pairs.
bool HomgInterestPointSet::write(const char* filename) const
{
  vcl_ofstream fout(filename);
  if (!fout.good()) {
    vcl_cerr << "HomgInterestPointSet::write() -- Failed to open \"" << filename << "\"\n";
    return false;
  }
  vcl_cerr << "HomgInterestPointSet: Saving corners to \"" << filename << "\"\n";
  return write(fout, get_conditioner());
}

bool HomgInterestPointSet::write(vcl_ostream& f, const ImageMetric*) const
{
  for (unsigned i=0; i < size(); i++) {
    const vnl_double_2& p = get_2d(i);
    f << p[0] << " " << p[1] << "\n";
  }
  return true;
}
