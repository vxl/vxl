// This is gel/vifa/vifa_line_cover.h
#ifndef VIFA_LINE_COVER_H
#define VIFA_LINE_COVER_H
//-----------------------------------------------------------------------------
//:
// \file
// \brief An index space for queries on parallel line overlap
//
// The vifa_line_cover class is an index which aids in finding parallel line
// covers.  That is, we have a set of lines, some of which are parallel.  The
// key function is to identify the subset of mutually parallel lines.  It is
// assumed that there is a line space (an array) upon which a set of
// approximately parallel lines are projected.  A measure of projection overlap
// is defined by the accumulated projected points along this line like shapes.
// That is, there are groups of parallel line segments having projection
// overlap.
//
// \author J.L. Mundy (12/22/1998)
//
// \verbatim
//  Modifications:
//   MPP Jun 2003, Ported to VXL from TargetJr
// \endverbatim
//-----------------------------------------------------------------------------

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vifa/vifa_typedefs.h>

class vifa_line_cover : public vbl_ref_count
{
  int           dim_;
  int*          index_;
  double*       max_extent_;
  double*       min_extent_;
  imp_line_sptr line_;

 public:
  // Constructors and Destructors
  vifa_line_cover();
  vifa_line_cover(imp_line_sptr prototype_line,
                  int           index_dimension);
  // copy constructor - compiler-provided one sets ref_count to nonzero which is wrong -PVr
  inline vifa_line_cover(vifa_line_cover const& c)
    : vbl_ref_count(), dim_(c.dim_), index_(c.index_),
      max_extent_(c.max_extent_), min_extent_(c.min_extent_), line_(c.line_) {}
  ~vifa_line_cover();

  // Indexing methods
  void InsertLine(imp_line_sptr l);

  // Data Access methods
  double GetCoverage();
  double GetDenseCoverage();
  double GetCustomCoverage(const double norm);
  void   GetExtent(imp_line_sptr& lmin,
                   imp_line_sptr& lmax);

 protected:
  int    get_index_min();
  int    get_index_max();
  double get_signed_distance(vgl_point_2d<double> const& p);
  imp_line_sptr get_offset_line(int    start,
                                int    end,
                                double dist);
};

typedef vbl_smart_ptr<vifa_line_cover> vifa_line_cover_sptr;

#endif  // VIFA_LINE_COVER_H
