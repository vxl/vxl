// This is gel/vifa/vifa_coll_lines.h
#ifndef VIFA_COLL_LINES
#define VIFA_COLL_LINES

//-----------------------------------------------------------------------------
//:
// \file
// \brief Store information about collinear lines
//
// \author Roddy Collins, from DDB in TargetJr
//
// \verbatim
//  Modifications:
//   MPP Mar 2003, Ported to VXL
// \endverbatim
//-----------------------------------------------------------------------------

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_smart_ptr.h>
#include <vgl/vgl_point_2d.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_2d.h>
#include <vifa/vifa_coll_lines_params.h>
#include <vifa/vifa_typedefs.h>


class vifa_coll_lines : public vifa_coll_lines_params
{
 private:
  bool           discard_flag_;
  static int     serial_num_;
  int            id_;
  imp_line_sptr  hypothesized_line_;
  edge_2d_list   contributors_;
  double         projected_length_cutoff_;
  double         endpt_distance_;

 public:
  vifa_coll_lines(const vtol_edge_2d_sptr&  e,
                  double        angle_cutoff_deg = 5.0,
                  double        endpt_distance = 3.0,
                  bool        src_from_discard = false
                 );
  ~vifa_coll_lines(void) override;

  bool   get_discard_flag(void) const;
  int    get_id(void) const;
  double get_measure(const vtol_edge_2d&  e) const;
  double get_projected_length(const vtol_edge_2d&  e) const;

  edge_2d_list&  get_contributors(void);

  // Caller must delete returned face_list when done
  face_list*    get_contributor_faces(void);

  static void  lms_fit(const std::vector<double>&  x,
                       const std::vector<double>&  y,
                       double&            A,
                       double&            B,
                       double&            C
                      );

  void  add_and_update(const vtol_edge_2d_sptr&  e);
  double  spanning_length(void);
  double  spanning_length(vgl_point_2d<double>&  p1,
                          vgl_point_2d<double>&  p2
                         );
  double  support_length(void);
  bool  contains(const vtol_edge&  e);

 private:
  static double  get_projected_length(const vtol_edge_2d&  e,
                                      const imp_line&    hyp_line,
                                      double&        v1_dist,
                                      double&        v2_dist
                                     );
  static double  get_midpt_dist(const vtol_edge_2d&  e,
                                const imp_line&    hyp_line
                               );
  double  get_measure(const vtol_edge_2d&  e,
                      const imp_line&    hyp_line
                     ) const;

  void fit_line(void);
};

typedef vbl_smart_ptr<vifa_coll_lines>  vifa_coll_lines_sptr;

typedef std::vector<vifa_coll_lines_sptr>      coll_list;
typedef std::vector<vifa_coll_lines_sptr>::iterator  coll_iterator;


#endif  // VIFA_COLL_LINES
