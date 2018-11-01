// This is oxl/mvl/HomgInterestPointSet.cxx
//:
// \file

#include <iostream>
#include <fstream>
#include <vector>
#include "HomgInterestPointSet.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil1/vil1_memory_image_of.h>

#include <mvl/HomgInterestPoint.h>
#include <mvl/ImageMetric.h>

class HomgInterestPointSetData : public std::vector<HomgInterestPoint>
{
 public:
  HomgInterestPointSetData() = default;
  HomgInterestPointSetData(int n):
    std::vector<HomgInterestPoint>(n, HomgInterestPoint())
  {}
  ~HomgInterestPointSetData() = default;
};

//: Construct an empty corner set.
HomgInterestPointSet::HomgInterestPointSet()
{
  data_ = new HomgInterestPointSetData;

  init_conditioner(nullptr);
}

//: Construct an empty corner set which will use the given conditioner to convert from image to homogeneous coordinates.
HomgInterestPointSet::HomgInterestPointSet(const HomgMetric& c)
{
  data_ = new HomgInterestPointSetData;

  init_conditioner(c);
}

//: Load corners from ASCII disk file
HomgInterestPointSet::HomgInterestPointSet(const char* filename, const HomgMetric& c)
{
  data_ = nullptr;
  read(filename, c);
  init_conditioner(c);
}

//: Construct corner set from container of vgl_homg_point_2d<double>, and set the conditioner.
// The vgl_homg_point_2ds are assumed to already be in conditioned coordinates.
HomgInterestPointSet::HomgInterestPointSet(std::vector<vgl_homg_point_2d<double> > const& points, ImageMetric* conditioner)
{
  unsigned n = points.size();
  if (n > 0)
    data_ = new HomgInterestPointSetData(n);
  else
    data_ = new HomgInterestPointSetData();

  for (unsigned i = 0; i < n; ++i)
    (*data_)[i] = HomgInterestPoint(points[i], conditioner, 0.0f);

  conditioner_ = conditioner;
}

//: Construct corner set from container of HomgPoint2D, and set the conditioner.
// The HomgPoint2Ds are assumed to already be in conditioned coordinates.
HomgInterestPointSet::HomgInterestPointSet(const std::vector<HomgPoint2D>& points, ImageMetric* conditioner)
{
  unsigned n = points.size();
  if (n > 0)
    data_ = new HomgInterestPointSetData(n);
  else
    data_ = new HomgInterestPointSetData();

  for (unsigned i = 0; i < n; ++i)
    (*data_)[i] = HomgInterestPoint(points[i], conditioner, 0.0f);

  conditioner_ = conditioner;
}

// - Untested
HomgInterestPointSet::HomgInterestPointSet(const HomgInterestPointSet& that)
{
  unsigned n = that.data_->size();
  if (n > 0)
    data_ = new HomgInterestPointSetData(n);
  else
    data_ = new HomgInterestPointSetData();

  std::cerr << "HomgInterestPointSet::copy ctor: size " << n << std::endl;

  for (unsigned i = 0; i < n; ++i)
    (*data_)[i] = (*that.data_)[i];
  conditioner_ = that.conditioner_;
}

// - Untested
HomgInterestPointSet& HomgInterestPointSet::operator=(const HomgInterestPointSet& that)
{
  unsigned n = that.data_->size();
  delete data_;
  if (n > 0)
    data_ = new HomgInterestPointSetData(n);
  else
    data_ = new HomgInterestPointSetData();
  for (unsigned i = 0; i < n; ++i) {
    (*data_)[i] = (*that.data_)[i];
  }
  conditioner_ = that.conditioner_;
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
  conditioner_ = c;
}

void HomgInterestPointSet::delete_conditioner()
{
}

//: Clear corner set.
void HomgInterestPointSet::clear()
{
  delete data_;
  data_ = new HomgInterestPointSetData;
  set_conditioner(nullptr);
}

//: Destructor
HomgInterestPointSet::~HomgInterestPointSet()
{
  delete data_;
}

// Operations----------------------------------------------------------------
//
//: Add a corner to the end of the list
bool HomgInterestPointSet::add(vgl_homg_point_2d<double> const& c)
{
  return add(c.x()/c.w(),c.y()/c.w());
}
bool HomgInterestPointSet::add(const HomgPoint2D& c)
{
  double x, y;
  c.get_nonhomogeneous(x,y);
  return add(x,y);
}
bool HomgInterestPointSet::add(const HomgInterestPoint& c)
{
  return add(c.homg_);
}
//
//: Add corner (x, y), using ImageMetric to convert to homogeneous.
bool HomgInterestPointSet::add(double x, double y)
{
  data_->push_back(HomgInterestPoint(x, y, conditioner_));
  return true;
}

//: Add a corner which has already been preconditioned by this cornerset's imagemetric.
bool HomgInterestPointSet::add_preconditioned(vgl_homg_point_2d<double> const& h)
{
  double x, y;
  conditioner_.homg_to_image(h, x, y);
  return add(x, y);
}
bool HomgInterestPointSet::add_preconditioned(const HomgPoint2D& h)
{
  double x, y;
  conditioner_.homg_to_image(h, &x, &y);
  return add(x, y);
}

//: Return the number of corners in the set.
unsigned HomgInterestPointSet::size() const
{
  return data_->size();
}

// == ACCESSORS ==

//: Return a reference to the i'th corner structure
const HomgInterestPoint& HomgInterestPointSet::get(int i) const
{
  return (*data_)[i];
}

HomgInterestPoint& HomgInterestPointSet::get(int i)
{
  return (*data_)[i];
}

//: Return the i'th corner as a 2D point
vnl_double_2 const& HomgInterestPointSet::get_2d(int i) const
{
  return (*data_)[i].double2_;
}

//: Return the i'th corner as a 2D point
vnl_vector_fixed<int,2> const& HomgInterestPointSet::get_int(int i) const
{
  return (*data_)[i].int2_;
}

//: Return the i'th corner as a vgl_homg_point_2d<double>
vgl_homg_point_2d<double> HomgInterestPointSet::homg_point(int i) const
{
  assert(i >= 0 && i < int(data_->size()));
  HomgPoint2D& p = (*data_)[i].homg_;
  return {p.x(),p.y(),p.w()};
}

//: Return the i'th corner as a HomgPoint2D
const HomgPoint2D& HomgInterestPointSet::get_homg(int i) const
{
  assert(i >= 0 && i < int(data_->size()));
  return (*data_)[i].homg_;
}

//: Return the i'th mean intensity
float HomgInterestPointSet::get_mean_intensity(int i) const
{
  assert(i >= 0 && i < int(data_->size()));
  float v = (*data_)[i].mean_intensity_;
  if (v == 0.0F) {
    std::cerr << "HomgInterestPointSet: WARNING mean_intensity["<<i<<"] = 0\n";
  }
  return v;
}

// Input/Output--------------------------------------------------------------

// == INPUT/OUTPUT ==

//: Load a corner set from a simple ASCII file of x y pairs.
// If ImageMetric is supplied, it is used to convert image coordinates to
// homogeneous form.
bool HomgInterestPointSet::read(const char* filename, const HomgMetric& c)
{
  std::ifstream f(filename);
  if (!f.good()) {
    std::cerr << "HomgInterestPointSet::read() -- Failed to open \"" << filename << "\"\n";
    return false;
  }

  return read(f, c);
}

bool HomgInterestPointSet::read(std::istream& f, const ImageMetric* c)
{
  clear();

  set_conditioner(c);

  while (f.good()) {
    double x, y;
    f >> x >> y;
    if (!f.good())
      break;
    add(x, y);
    f >> std::ws;
  }
  return true;
}

//: Load a corner set from a simple ASCII file of x y pairs, and use vil1_image src to compute mean_intensities.
// If ImageMetric is supplied, it is used to convert image coordinates to homogeneous form.
// NOT YET IMPLEMENTED
bool HomgInterestPointSet::read(const char* filename, vil1_image const& /*src*/, const HomgMetric& c)
{
  if (!read(filename, c))
    return false;

#if 1
  std::cerr << "HomgInterestPointSet::read() not implemented in any sense of the word\n";
#else
  //std::cerr << "HomgInterestPointSet: Computing mean intensities\n";
  vil1_memory_image_of<unsigned char> imbuf(src);
  for (unsigned i=0; i< size(); i++) {
    ImageWindowOps winops(imbuf, get_int(i), 3);
    (*data_)[i].mean_intensity_ = winops.mean_intensity();
    if ((*data_)[i].mean_intensity_ == 0.0F) {
      //std::cerr << " note " << i << " had mi of 0\n";
      (*data_)[i].mean_intensity_ = 1e6;
    }
  }
#endif

  return true;
}

//: Save a corner set as a simple ASCII file of x y pairs.
bool HomgInterestPointSet::write(const char* filename) const
{
  std::ofstream fout(filename);
  if (!fout.good()) {
    std::cerr << "HomgInterestPointSet::write() -- Failed to open \"" << filename << "\"\n";
    return false;
  }
  std::cerr << "HomgInterestPointSet: Saving corners to \"" << filename << "\"\n";
  return write(fout, get_conditioner());
}

bool HomgInterestPointSet::write(std::ostream& f, const ImageMetric*) const
{
  for (unsigned i=0; i < size(); i++) {
    const vnl_double_2& p = get_2d(i);
    f << p[0] << " " << p[1] << "\n";
  }
  return true;
}
