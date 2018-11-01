// This is oxl/mvl/HomgInterestPointSet.h
#ifndef HomgInterestPointSet_h_
#define HomgInterestPointSet_h_
//:
// \file
// \brief Set of interest points on an image
//
// HomgInterestPointSet holds a set of corners (or points of interest) computed
// from an image.  The current implementation provides a moderately
// abstract interface, but assumes that it is efficient to associate
// an index with each corner.  I \e know this ought to be elsewhere.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 17 Aug 96
//
// \verbatim
//  Modifications:
//   Peter Vanroose - 27 aug.97 - moved std::vector<HomgInterestPoint> instantiation to Templates
//   Peter Vanroose - 22 oct.02 - added vgl_homg_point_2d interface
// \endverbatim
//-----------------------------------------------------------------------------

#include <vector>
#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/vnl_double_2.h>
#include <vgl/vgl_homg_point_2d.h>

#include <mvl/HomgPoint2D.h>
#include <mvl/HomgMetric.h>

class vil1_image;
class ImageMetric;
class HomgInterestPointSetData;
class HomgInterestPoint;

class HomgInterestPointSet
{
  // Data Members--------------------------------------------------------------
  HomgInterestPointSetData* data_;
  HomgMetric conditioner_;

 public:
  // Constructors/Destructors--------------------------------------------------

  HomgInterestPointSet();
  HomgInterestPointSet(const HomgMetric&);
  HomgInterestPointSet(const char* filename, const HomgMetric& = nullptr);
  HomgInterestPointSet(const std::vector<HomgPoint2D>&, ImageMetric* conditioner);
  HomgInterestPointSet(std::vector<vgl_homg_point_2d<double> > const&, ImageMetric* conditioner);
  HomgInterestPointSet(const HomgInterestPointSet& that);
 ~HomgInterestPointSet();

  HomgInterestPointSet& operator=(const HomgInterestPointSet& that);

  // Operations----------------------------------------------------------------
  // void set(const IUPointGroup&, const HomgMetric& c);

  // Data Access---------------------------------------------------------------
  unsigned size() const;

  //: Return i'th interest pt.
  const HomgPoint2D& operator[](int i) const { return get_homg(i); }

  HomgInterestPoint& get(int i);
  HomgInterestPoint const& get(int i) const;
  vnl_double_2 const& get_2d(int i) const;
  vnl_vector_fixed<int,2> const& get_int(int i) const;
  HomgPoint2D const& get_homg(int i) const;
  vgl_homg_point_2d<double> homg_point(int i) const;
  float              get_mean_intensity(int i) const;

  std::vector<HomgPoint2D> const & get_homg_points() const;
  std::vector<vgl_homg_point_2d<double> > homg_points() const;

  const ImageMetric* get_conditioner() const { return (const ImageMetric*)conditioner_; }
  void set_conditioner(const HomgMetric& c);

  // Data Control--------------------------------------------------------------
  bool add(double x, double y); // image coordinates
  bool add(const HomgPoint2D&);
  bool add(vgl_homg_point_2d<double> const&);
  bool add(const HomgInterestPoint&);
  bool add_preconditioned(const HomgPoint2D&);
  bool add_preconditioned(vgl_homg_point_2d<double> const&);
  void set_image(vil1_image const& image);
  void clear();

  // Input/Output--------------------------------------------------------------
  bool read(const char* filename, const HomgMetric& c = nullptr);
  bool read(const char* filename, vil1_image const& src, const HomgMetric& c = nullptr);
  bool write(const char* filename) const;

  bool read(std::istream& f, const ImageMetric* c);
  bool write(std::ostream& f, const ImageMetric* c) const;

 protected:
  void init_conditioner(const HomgMetric& c = nullptr);
  void delete_conditioner();
};

#endif // HomgInterestPointSet_h_
