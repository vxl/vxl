// This is gel/vifa/vifa_group_pgram.h
#ifndef VIFA_GROUP_PGRAM_H
#define VIFA_GROUP_PGRAM_H

//-----------------------------------------------------------------------------
//:
// \file
// \brief Process-type class to find groups of approximately parallel lines.
//
// The vifa_group_pgram class is a grouper which finds relatively parallellogram-
// like shapes.  That is, there are groups of parallel line segments having
// projection overlap
//
// \author J.L. Mundy (11/27/1998)
//
// \verbatim
//  Modifications:
//   MPP Jun 2003, Ported to VXL from TargetJr
// \endverbatim
//-----------------------------------------------------------------------------

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vifa/vifa_bbox.h>
#include <vifa/vifa_group_pgram_params.h>
#include <vifa/vifa_histogram.h>
#include <vifa/vifa_line_cover.h>
#include <vifa/vifa_typedefs.h>


class vifa_group_pgram : public vifa_group_pgram_params
{
  int         th_dim_;
  double      angle_range_;

  //: Array of dominant theta indices
  std::vector<int>  dominant_dirs_;

  vifa_bbox_sptr  bb_;
  imp_line_table  curves_;
  double          tmp1_;

 public:
  // Constructors and Destructors
  vifa_group_pgram(imp_line_list&                 lg,
                   const vifa_group_pgram_params& old_params,
                   double                         angle_range = 180.0
                  );
  ~vifa_group_pgram() override;

  // Index insertion
  void    Index(const imp_line_sptr&    il);
  void    Index(imp_line_list&  lg);
  void    Clear();

  // Data accessors
  vifa_histogram_sptr   GetCoverageHist(void);
  vifa_line_cover_sptr  GetLineCover(int  angle_bin);
  double         LineCoverage(int  angle_bin);
  void           CollectAdjacentLines(int      angle_bin,
                                      imp_line_list&  lg
                                     );
  vifa_bbox_sptr GetBoundingBox(void);
  void           SetTemp1(const double  tmp) { tmp1_ = tmp; }
  double         GetAdjacentPerimeter(int  bin);
  double         norm_parallel_line_length(void);

 protected:
  int            AngleLoc(const imp_line_sptr&  il);
  imp_line_sptr  LineAtAngle(int  angle_bin);
  void           CheckUpdateBoundingBox(void);
  void           ComputeBoundingBox(void);
  void           ComputeDominantDirs(void);
};

// Test consistency of bound
inline void vifa_group_pgram::
CheckUpdateBoundingBox(void)
{
  if (!bb_ )
  {
    bb_ = new vifa_bbox;
    this->ComputeBoundingBox();
    return;
  }

  if (bb_->older(this))
    this->ComputeBoundingBox();
}

inline vifa_bbox_sptr vifa_group_pgram::
GetBoundingBox(void)
{
  this->CheckUpdateBoundingBox();
  return bb_;
}


#endif  // VIFA_GROUP_PGRAM_H
