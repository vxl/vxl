// This is brl/bseg/bapl/bapl_keypoint_set.cxx
#include <iostream>
#include <set>
#include "bapl_keypoint_set.h"
//:
// \file

#include "bapl_keypoint.h"
#include "bapl_lowe_keypoint.h"
#include "bapl_lowe_keypoint_sptr.h"
#include "bapl_keypoint_sptr.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_point_2d.h>
#include <vpgl/vpgl_fundamental_matrix.h>
#include <bpgl/algo/bpgl_fm_compute_ransac.h>
#include <vgl/algo/vgl_h_matrix_2d_optimize_lmq.h>

//: Binary io, NOT IMPLEMENTED, signatures defined to use bapl_keypoint_set as a brdb_value
void vsl_b_write(vsl_b_ostream & /*os*/, bapl_keypoint_set const & /*ph*/)
{
  std::cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream & /*is*/, bapl_keypoint_set & /*ph*/)
{
  std::cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream& is, bapl_keypoint_set* ph)
{
  delete ph;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    std::vector<bapl_keypoint_sptr> vec;
    ph = new bapl_keypoint_set(vec);
    vsl_b_read(is, *ph);
  }
  else
    ph = nullptr;
}

void vsl_b_write(vsl_b_ostream& os, const bapl_keypoint_set* &ph)
{
  if (ph==nullptr)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*ph);
  }
}


//: Binary io, NOT IMPLEMENTED, signatures defined to use bapl_keypoint_match_set as a brdb_value
void vsl_b_write(vsl_b_ostream & /*os*/, bapl_keypoint_match_set const & /*ph*/)
{
  std::cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream & /*is*/, bapl_keypoint_match_set & /*ph*/)
{
  std::cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream& is, bapl_keypoint_match_set* ph)
{
  delete ph;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    std::vector<bapl_key_match> vec;
    ph = new bapl_keypoint_match_set(0,0,vec);  // dummy instance
    vsl_b_read(is, *ph);
  }
  else
    ph = nullptr;
}

void vsl_b_write(vsl_b_ostream& os, const bapl_keypoint_match_set* &ph)
{
  if (ph==nullptr)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*ph);
  }
}

//: remove spurious matches, i.e remove if a keypoint from J is shared : (i1,j) (i2,j), remove (i2,j) since one of them is definitely spurious
void bapl_keypoint_match_set::prune_spurious_matches(std::vector<bapl_key_match>& matches)
{
  std::set<int> helper;
  int cnt = (int)matches.size();
  for (int ii = 0; ii < cnt; ii++) {
    auto it = helper.find(matches[ii].second->id());
    if (it == helper.end()) {
      helper.insert(matches[ii].second->id());
    }
    else {
      matches.erase(matches.begin() + ii);
      ii--;
      cnt--;
    }
  }
}

//: refine matches by computing F
void bapl_keypoint_match_set::refine_matches(float outlier_threshold, std::vector<bapl_key_match>& refined_matches)
{
  std::vector<vgl_point_2d<double> > lpts, rpts; std::vector<vgl_point_2d<double> > lpts_refined, rpts_refined;
  for (const auto& m : matches_) {
    bapl_lowe_keypoint_sptr kp1;
    kp1.vertical_cast(m.first);
    bapl_lowe_keypoint_sptr kp2;
    kp2.vertical_cast(m.second);
    vgl_point_2d<double> lpt(kp1->location_i(), kp1->location_j());
    vgl_point_2d<double> rpt(kp2->location_i(), kp2->location_j());
    lpts.push_back(lpt); rpts.push_back(rpt);
  }

  vpgl_fundamental_matrix<double> fm;
  bpgl_fm_compute_ransac fmcr;
  fmcr.set_generate_all(true);
  fmcr.set_outlier_threshold(outlier_threshold);
  fmcr.compute( rpts, lpts, fm);
  vnl_matrix_fixed<double,3,3> fm_vnl = fm.get_matrix();
  std::cout << "\nRansac estimated fundamental matrix:\n" << fm_vnl << '\n';
  std::vector<bool> outliers = fmcr.outliers;
  std::vector<double> res = fmcr.residuals;
#ifdef DEBUG
  std::cout << "\noutliers\n";
  for (unsigned i = 0; i<outliers.size(); ++i)
    std::cout << "O[" << i << "]= " << outliers[i]
             << "  e "<< res[i] <<  '\n';
#endif
  std::vector<vgl_homg_point_2d<double> > rpoints, lpoints;

  //: prune the outliers
  for (unsigned i = 0; i<lpts.size(); ++i)
  {
    if (outliers[i])
      continue;
    refined_matches.push_back(matches_[i]);
    lpoints.emplace_back(lpts[i]);
    rpoints.emplace_back(rpts[i]);
    lpts_refined.push_back(lpts[i]);
    rpts_refined.push_back(rpts[i]);
  }

#if 0 // not implemented yet
  //further refine F using Levenberg-Marquardt
  .. refinement code here ... should compute:  vnl_matrix_fixed<double, 3, 3> F_optimized

  //: remove the matches that are outliers to the optimized F
  rrel_fm_problem estimator( rpts_refined, lpts_refined ); // class to be used to compute residuals
  vnl_vector<double> params;
  vpgl_fundamental_matrix<double> fm_optimized(F_optimized);
  estimator.fm_to_params(fm_optimized, params);
  std::vector<double> residuals;
  estimator.compute_residuals(params, residuals);

  std::vector<bool> outliers2;
  for ( unsigned i = 0; i < rpts_refined.size(); i++ ){
    if ( residuals[i] > outlier_threshold )
      outliers2.push_back( true );
    else
      outliers2.push_back( false );
  }

  std::vector<bapl_key_match> matches_pruned2;
  for (unsigned i = 0; i < rpts_refined.size(); i++) {
    if (outliers2[i])
      continue;
    matches_pruned2.push_back(matches_pruned[i]);
  }

  std::cout << "After F optimization found: " << matches_pruned2.size() << " matches, initial number was: " << matches_pruned.size() << "!\n";
#endif
}
