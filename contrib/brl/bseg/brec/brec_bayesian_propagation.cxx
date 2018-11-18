//:
// \file
// \brief brec recognition utilities
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date October 01, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include "brec_bayesian_propagation.h"
#include <vil/vil_image_view.h>
#include <vil/vil_copy.h>
#include <bsta/bsta_joint_histogram.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bool helper_function(double p_b_current, double p_f_current,
                     double p_b_current1, double p_f_current1,
                     double p_ff, double p_fb, double p_bf, double p_bb, float& out) {
  //: now compute 4 multipliers
  double m_ff_d = (p_bf*p_b_current + p_ff*p_f_current); if (std::abs(m_ff_d) < 0.000001) return false;
  double m_ff = p_ff/m_ff_d;
  double m_bb_d = (p_bb*p_b_current + p_fb*p_f_current); if (std::abs(m_bb_d) < 0.000001) return false;
  double m_bb = p_bb/m_bb_d;
  double m_fb_d = (p_bb*p_b_current + p_fb*p_f_current); if (std::abs(m_fb_d) < 0.000001) return false;
  double m_fb = p_fb/m_fb_d;
  double m_bf_d = (p_bf*p_b_current + p_ff*p_f_current); if (std::abs(m_bf_d) < 0.000001) return false;
  double m_bf = p_bf/m_bf_d;

  //: find the probs for each hypothesis:
  double p_h_bb = p_b_current*p_b_current1; double p_h_bf = p_b_current*p_f_current1;
  double p_h_fb = p_f_current*p_b_current1; double p_h_ff = p_f_current*p_f_current1;

  if (p_h_bb >= p_h_bf && p_h_bb >= p_h_ff && p_h_bb >= p_h_fb)
    out = float(m_bb*p_b_current);
  else if (p_h_bf >= p_h_bb && p_h_bf >= p_h_ff && p_h_bf >= p_h_fb)
    out = float(m_bf*p_b_current);
  else if (p_h_fb >= p_h_bb && p_h_fb >= p_h_ff && p_h_fb >= p_h_bf)
    out = 1.0f-float(m_fb*(1-p_b_current));
  else // p_h_ff is greater than all
    out = 1.0f-float(m_ff*(1-p_b_current));

  return true;
}


bool
brec_bayesian_propagation::horizontal_pass(brec_fg_pair_density& fgp, brec_fg_bg_pair_density& fgbgp, brec_bg_pair_density& bgp)
{
  //: for now make only one horizontal pass
  for (unsigned i = 0; i < bg_map_.ni()-1; i++) {
    for (unsigned j = 0; j < bg_map_.nj(); j++) {
      double yi0 = img_(i,j);
      double yi1 = img_(i+1, j);

      double p_b_current = bg_map_(i,j);
      double p_f_current = 1.0-bg_map_(i,j);

      double p_b_current1 = bg_map_(i+1,j);
      double p_f_current1 = 1.0-bg_map_(i+1,j);

      double p_ff = fgp(yi0, yi1);
      double p_fb = fgbgp(yi0, yi1);
      double p_bf = p_fb;  // bg fg model is equivalent to fg bg
      bgp.set_image_coords(i, j);
      double p_bb = bgp(yi0, yi1);

      if (!helper_function(p_b_current, p_f_current, p_b_current1, p_f_current1, p_ff, p_fb, p_bf, p_bb, bg_map_(i,j)))
        continue;

      bg_map_(i,j) > 1.0 ? 1.0 : bg_map_(i,j);
      bg_map_(i,j) < 0.0 ? 0.0 : bg_map_(i,j);
    }
  }
  return true;
}

//: ii is either -1 or 0 or 1
//  jj is either -1 or 0 or 1 so can run in all 8 directions
bool
brec_bayesian_propagation::directional_pass(brec_fg_pair_density& fgp, brec_fg_bg_pair_density& fgbgp, brec_bg_pair_density& bgp, int ii, int jj)
{
  //: for now make only one horizontal pass
  for (unsigned i = 0; i < bg_map_.ni(); i++) {
    for (unsigned j = 0; j < bg_map_.nj(); j++) {
      int in = i+ii;
      int jn = j+jj;
      if (in < 0 || jn < 0) continue;
      if (in >= int(bg_map_.ni()) || jn >= int(bg_map_.nj())) continue;

      double yi0 = img_(i,j);
      double yi1 = img_(in,jn);

      double p_b_current = bg_map_(i,j);
      double p_f_current = 1.0-bg_map_(i,j);

      double p_b_current1 = bg_map_(in,jn);
      double p_f_current1 = 1.0-bg_map_(in,jn);

      double p_ff = fgp(yi0, yi1);
      double p_fb = fgbgp(yi0, yi1);
      double p_bf = p_fb;  // bg fg model is equivalent to fg bg

      bgp.set_image_coords(i, j, (unsigned)in, (unsigned)jn);
      double p_bb = bgp(yi0, yi1);

      if (!helper_function(p_b_current, p_f_current, p_b_current1, p_f_current1, p_ff, p_fb, p_bf, p_bb, bg_map_(i,j)))
        continue;

      bg_map_(i,j) > 1.0 ? 1.0 : bg_map_(i,j);
      bg_map_(i,j) < 0.0 ? 0.0 : bg_map_(i,j);
    }
  }
  return true;
}

bool
brec_bayesian_propagation::horizontal_pass_using_prob_density_as_bgp(brec_fg_pair_density& fgp,
                                                                     brec_fg_bg_pair_density& fgbgp)
{
  //: for now make only one horizontal pass
  for (unsigned i = 0; i < bg_map_.ni()-1; i++) {
    for (unsigned j = 0; j < bg_map_.nj(); j++) {
      double yi0 = img_(i,j);
      double yi1 = img_(i+1, j);

      double p_b_current = bg_map_(i,j);
      double p_f_current = 1.0-bg_map_(i,j);

      double p_b_current1 = bg_map_(i+1,j);
      double p_f_current1 = 1.0-bg_map_(i+1,j);

      double p_ff = fgp(yi0, yi1);
      double p_fb = fgbgp(yi0, yi1);
      double p_bf = p_fb;  // bg fg model is equivalent to fg bg

      double p_bb = prob_density_(i,j)*prob_density_(i+1,j);
      //double p_bb = bg_map_(i,j)*bg_map_(i+1,j);

      if (!helper_function(p_b_current, p_f_current, p_b_current1, p_f_current1, p_ff, p_fb, p_bf, p_bb, bg_map_(i,j)))
        continue;

      bg_map_(i,j) > 1.0 ? 1.0 : bg_map_(i,j);
      bg_map_(i,j) < 0.0 ? 0.0 : bg_map_(i,j);

#if 0
      if (i == 572 && j == 335) {
        std::cout << "--------- 572, 335 -----------\n"
                 << "p_b_current: " << p_b_current << " p_f_current: " << p_f_current << std::endl
                 << "p_b_current1: " << p_b_current1 << " p_f_current1: " << p_f_current1 << std::endl
                 << " p_ff: " << p_ff << " p_fb: " << p_fb
                 << " p_bf: " << p_bf << " p_bb: " << p_bb << std::endl
                 << " m_ff: " << m_ff << " m_bb: " << m_bb << " m_fb: " << m_fb << " m_bf: " << m_bf << std::endl
                 << " p_h_bb: " << p_h_bb << " p_h_bf: " << p_h_bf << " p_h_fb: " << p_h_fb << " p_h_ff: " << p_h_ff << std::endl
                 << "the highest prob hypothesis was: ";
        if (p_h_bb >= p_h_bf && p_h_bb >= p_h_ff && p_h_bb >= p_h_fb)
          std::cout << " p_h_bb\n";
        else if (p_h_bf >= p_h_bb && p_h_bf >= p_h_ff && p_h_bf >= p_h_fb)
          std::cout << " p_h_bf\n";
        else if (p_h_fb >= p_h_bb && p_h_fb >= p_h_ff && p_h_fb >= p_h_bf)
          std::cout << " p_h_fb\n";
        else // m_ff is greater than all
          std::cout << " p_h_ff\n";
        std::cout << " new p_b: " << bg_map_(i,j) << " hence p_f: " << 1-bg_map_(i,j) << std::endl
                 << "----------------------------\n";
      }
      if (i == 606 && j == 343) {
        std::cout << "--------- 606, 343 -----------\n"
                 << "p_b_current: " << p_b_current << " p_f_current: " << p_f_current << std::endl
                 << "p_b_current1: " << p_b_current1 << " p_f_current1: " << p_f_current1 << std::endl
                 << " p_ff: " << p_ff << " p_fb: " << p_fb
                 << " p_bf: " << p_bf << " p_bb: " << p_bb << std::endl
                 << " m_ff: " << m_ff << " m_bb: " << m_bb << " m_fb: " << m_fb << " m_bf: " << m_bf << std::endl
                 << " p_h_bb: " << p_h_bb << " p_h_bf: " << p_h_bf << " p_h_fb: " << p_h_fb << " p_h_ff: " << p_h_ff << std::endl
                 << "the highest prob hypothesis was: ";
        if (p_h_bb >= p_h_bf && p_h_bb >= p_h_ff && p_h_bb >= p_h_fb)
          std::cout << " p_h_bb\n";
        else if (p_h_bf >= p_h_bb && p_h_bf >= p_h_ff && p_h_bf >= p_h_fb)
          std::cout << " p_h_bf\n";
        else if (p_h_fb >= p_h_bb && p_h_fb >= p_h_ff && p_h_fb >= p_h_bf)
          std::cout << " p_h_fb\n";
        else // m_ff is greater than all
          std::cout << " p_h_ff\n";
        std::cout << " new p_b: " << bg_map_(i,j) << " hence p_f: " << 1-bg_map_(i,j) << std::endl
                 << "----------------------------\n";
      }
      if (i == 596 && j == 360) {
        std::cout << "--------- 596, 360 -----------\n"
                 << "p_b_current: " << p_b_current << " p_f_current: " << p_f_current << std::endl
                 << "p_b_current1: " << p_b_current1 << " p_f_current1: " << p_f_current1 << std::endl
                 << " p_ff: " << p_ff << " p_fb: " << p_fb
                 << " p_bf: " << p_bf << " p_bb: " << p_bb << std::endl
                 << " m_ff: " << m_ff << " m_bb: " << m_bb << " m_fb: " << m_fb << " m_bf: " << m_bf << std::endl
                 << " p_h_bb: " << p_h_bb << " p_h_bf: " << p_h_bf << " p_h_fb: " << p_h_fb << " p_h_ff: " << p_h_ff << std::endl
                 << "the highest prob hypothesis was: ";
        if (p_h_bb >= p_h_bf && p_h_bb >= p_h_ff && p_h_bb >= p_h_fb)
          std::cout << " p_h_bb\n";
        else if (p_h_bf >= p_h_bb && p_h_bf >= p_h_ff && p_h_bf >= p_h_fb)
          std::cout << " p_h_bf\n";
        else if (p_h_fb >= p_h_bb && p_h_fb >= p_h_ff && p_h_fb >= p_h_bf)
          std::cout << " p_h_fb\n";
        else // m_ff is greater than all
          std::cout << " p_h_ff\n";
        std::cout << " new p_b: " << bg_map_(i,j) << " hence p_f: " << 1-bg_map_(i,j) << std::endl
                 << "----------------------------\n";
      }
#endif
    }
  }
  return true;
}


bool
brec_bayesian_propagation::vertical_pass_using_prob_density_as_bgp(brec_fg_pair_density& fgp, brec_fg_bg_pair_density& fgbgp)
{
  //: for now make only one horizontal pass
  for (unsigned i = 0; i < bg_map_.ni(); i++) {
    for (unsigned j = 0; j < bg_map_.nj()-1; j++) {
      double yi0 = img_(i,j);
      double yi1 = img_(i,j+1);

      double p_b_current = bg_map_(i,j);
      double p_f_current = 1.0-bg_map_(i,j);

      double p_b_current1 = bg_map_(i,j+1);
      double p_f_current1 = 1.0-bg_map_(i,j+1);

      double p_ff = fgp(yi0, yi1);
      double p_fb = fgbgp(yi0, yi1);
      double p_bf = p_fb;  // bg fg model is equivalent to fg bg

      double p_bb = prob_density_(i,j)*prob_density_(i,j+1);
      //double p_bb = bg_map_(i,j)*bg_map_(i,j+1);
      if (!helper_function(p_b_current, p_f_current, p_b_current1, p_f_current1, p_ff, p_fb, p_bf, p_bb, bg_map_(i,j)))
        continue;

      bg_map_(i,j) > 1.0 ? 1.0 : bg_map_(i,j);
      bg_map_(i,j) < 0.0 ? 0.0 : bg_map_(i,j);
    }
  }
  return true;
}

//: ii is either -1 or 0 or 1
//  jj is either -1 or 0 or 1 so can run in all 8 directions
bool
brec_bayesian_propagation::directional_pass_using_prob_density_as_bgp(brec_fg_pair_density& fgp, brec_fg_bg_pair_density& fgbgp, int ii, int jj)
{
  //: for now make only one horizontal pass
  for (unsigned i = 0; i < bg_map_.ni(); i++) {
    for (unsigned j = 0; j < bg_map_.nj(); j++) {
      int in = i+ii;
      int jn = j+jj;
      if (in < 0 || jn < 0) continue;
      if (in >= int(bg_map_.ni()) || jn >= int(bg_map_.nj())) continue;

      double yi0 = img_(i,j);
      double yi1 = img_(in,jn);

      double p_b_current = bg_map_(i,j);
      double p_f_current = 1.0-bg_map_(i,j);

      double p_b_current1 = bg_map_(in,jn);
      double p_f_current1 = 1.0-bg_map_(in,jn);

      double p_ff = fgp(yi0, yi1);
      double p_fb = fgbgp(yi0, yi1);
      double p_bf = p_fb;  // bg fg model is equivalent to fg bg

      double p_bb = prob_density_(i,j)*prob_density_(in,jn);
      if (!helper_function(p_b_current, p_f_current, p_b_current1, p_f_current1, p_ff, p_fb, p_bf, p_bb, bg_map_(i,j)))
        continue;

      bg_map_(i,j) > 1.0 ? 1.0 : bg_map_(i,j);
      bg_map_(i,j) < 0.0 ? 0.0 : bg_map_(i,j);
    }
  }
  return true;
}

bool
brec_bayesian_propagation::initialize_bg_map(float high_prior, float top_percentile)
{
  //: initialize the map
  bg_map_.set_size(ni_, nj_);
  brec_bg_pair_density::initialize_bg_map(prob_density_, bg_map_, high_prior, top_percentile);
  return true;
}

//: use background mixture of gaussian image
bool
brec_bayesian_propagation::run(brec_fg_pair_density& fgp, brec_fg_bg_pair_density& fgbgp, brec_bg_pair_density& bgp,
                               int npasses)
{
  for (int i = 0; i < npasses; i++) {
    //if (!horizontal_pass(fgp, fgbgp, bgp))
    //  return false;
    for (int ii = -1; ii < 2; ii++) {
      for (int jj = -1; jj < 2; jj++) {
        if (ii == 0 && jj == 0)
          continue;
        if (!directional_pass(fgp, fgbgp, bgp, ii, jj))
          return false;
      }
    }
  }

  return true;
}

bool helper_function2(double p_b_current, double p_f_current,
                      double  /*p_b_current1*/, double  /*p_f_current1*/, // both unused - FIXME
                      double p_bb, double p_bf, double p_fb, double p_ff,
                      double& out1, double& out2, double& out3, double& out4) {
  //: now compute 4 multipliers
  double m_ff_d = (p_bf*p_b_current + p_ff*p_f_current); if (std::abs(m_ff_d) < 0.000001) return false;
  double m_ff = p_ff/m_ff_d;
  double m_bb_d = (p_bb*p_b_current + p_fb*p_f_current); if (std::abs(m_bb_d) < 0.000001) return false;
  double m_bb = p_bb/m_bb_d;
  double m_fb_d = (p_bb*p_b_current + p_fb*p_f_current); if (std::abs(m_fb_d) < 0.000001) return false;
  double m_fb = p_fb/m_fb_d;
  double m_bf_d = (p_bf*p_b_current + p_ff*p_f_current); if (std::abs(m_bf_d) < 0.000001) return false;
  double m_bf = p_bf/m_bf_d;

  //if (p_h_bb >= p_h_bf && p_h_bb >= p_h_ff && p_h_bb >= p_h_fb)
  out1 = float(m_bb*p_b_current);
  //else if (p_h_bf >= p_h_bb && p_h_bf >= p_h_ff && p_h_bf >= p_h_fb)
  out2 = float(m_bf*p_b_current);
  //else if (p_h_fb >= p_h_bb && p_h_fb >= p_h_ff && p_h_fb >= p_h_bf)
  out3 = 1.0f-float(m_fb*(1-p_b_current));
  //else // p_h_ff is greater than all
  out4 = 1.0f-float(m_ff*(1-p_b_current));

  return true;
}

inline bool
hyp_more(const std::pair<double, double>& p1, const std::pair<double, double>& p2)
{
  return p1.first >= p2.first;
}

bool
brec_bayesian_propagation::pass_using_prob_density_as_bgp(brec_fg_pair_density& fgp, brec_fg_bg_pair_density& fgbgp, vil_image_view<float>& temp)
{
  int ni = (int)(bg_map_.ni())-1;
  int nj = (int)(bg_map_.nj())-1;

  for (int i = 1; i < ni; i++) {
    for (int j = 1; j < nj; j++) {
      double yi0 = img_(i,j);
      double p_b_current = bg_map_(i,j);
      double p_f_current = 1.0-bg_map_(i,j);

      //: consider 8 neighborhood
      std::vector<std::pair<double, double> > values(32, std::pair<double, double>(0.0f,0.0f));
      unsigned count = 0;
      double best_hyp = 0;
      double best_out = 0;
      for (int k = -1; k < 2; k++) {
        for (int m = -1; m < 2; m++) {
          if (k == 0 && m == 0)
            continue;
          double yi1 = img_(i+k,j+m);
          double p_b_current1 = bg_map_(i+k,j+m);
          double p_f_current1 = 1.0-bg_map_(i+k,j+m);

          double p_ff1 = fgp(yi0, yi1);
          double p_fb1 = fgbgp(yi0, yi1);
          double p_bf1 = p_fb1;  // bg fg model is equivalent to fg bg
          //double p_bb1 = prob_density_(i,j)*prob_density_(i+k,j+m);
          double p_bb1 = bg_map_(i,j)*bg_map_(i+k,j+m);

          //: find the probs for each hypothesis:
          // p_h_bb1
          values[count*4+0].first = p_b_current*p_b_current1;
          // p_h_bf1
          values[count*4+1].first = p_b_current*p_f_current1;
          // p_h_fb1
          values[count*4+2].first = p_f_current*p_b_current1;
          // p_h_ff1
          values[count*4+3].first = p_f_current*p_f_current1;

          helper_function2(p_b_current, p_f_current, p_b_current1, p_f_current1,
                           p_bb1, p_bf1, p_fb1, p_ff1,
                           values[count*4+0].second, values[count*4+1].second, values[count*4+2].second, values[count*4+3].second);
          for (unsigned kk = 0; kk < 4; kk++) {
            if (values[count*4+kk].first >= best_hyp &&
                values[count*4+kk].first >= values[count*4+0].first &&
                values[count*4+kk].first >= values[count*4+1].first &&
                values[count*4+kk].first >= values[count*4+2].first &&
                values[count*4+kk].first >= values[count*4+3].first) {
                  best_hyp = values[count*4+kk].first;
                  best_out = values[count*4+kk].second;
            }
          }
          count++;
        }
      }
      if (count != 8) {
        std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Count is not 8!!!\n";
        return false;
      }

      //: sort values with respect to hypothesis
      //std::sort(values.begin(), values.end(), hyp_more);
      //bg_map_(i,j) = (float)values[0].second;
      //bg_map_(i,j) > 1.0 ? 1.0 : bg_map_(i,j);
      //bg_map_(i,j) < 0.0 ? 0.0 : bg_map_(i,j);
      //temp(i,j) = (float)values[0].second;
      temp(i,j) = (float)best_out;
      temp(i,j) > 1.0 ? 1.0 : temp(i,j);
      temp(i,j) < 0.0 ? 0.0 : temp(i,j);
    }
  }
  return true;
}

bool
brec_bayesian_propagation::run_using_prob_density_as_bgp(brec_fg_pair_density& fgp,
                                                         brec_fg_bg_pair_density& fgbgp,
                                                         int npasses)
{
  for (int i = 0; i < npasses; i++) {
    vil_image_view<float> temp(bg_map_.ni(), bg_map_.nj(), 1);
    vil_copy_deep(bg_map_, temp);
#if 0
    if (!horizontal_pass_using_prob_density_as_bgp(fgp, fgbgp))
      return false;
    if (!vertical_pass_using_prob_density_as_bgp(fgp, fgbgp))
      return false;
#endif // 0
    if (!pass_using_prob_density_as_bgp(fgp, fgbgp, temp))
      return false;
    vil_copy_deep(temp, bg_map_);

#if 0
    for (int ii = -1; ii < 2; ii++) {
      for (int jj = -1; jj < 2; jj++) {
        if (ii == 0 && jj == 0)
          continue;
        if (!directional_pass_using_prob_density_as_bgp(fgp, fgbgp, ii, jj))
          return false;
      }
    }
#endif // 0
  }

  return true;
}

float brec_area_density::back_density(float val)
{
  return back_model_.prob_density(val);
}

float brec_area_density::fore_density(float val)
{
  return fore_model_.prob_density(val);
}

bool
brec_bayesian_propagation::run_area(vil_image_view<float>& area_map, float lambda1, float k1, float lambda2, float k2)
{
  vil_image_view<float> temp(bg_map_.ni(), bg_map_.nj(), 1);
  vil_copy_deep(bg_map_, temp);

  //: construct density
  brec_area_density d(lambda1, k1, lambda2, k2);

  for (unsigned i = 0; i < bg_map_.ni(); i++) {
    for (unsigned j = 0; j < bg_map_.nj(); j++) {
      float p_b_current = bg_map_(i,j);
      float p_f_current = 1.0f-bg_map_(i,j);
      temp(i,j) = d.back_density(area_map(i,j))*p_b_current;
      temp(i,j) = temp(i,j)/(temp(i,j)+d.fore_density(area_map(i,j))*p_f_current);
    }
  }

  vil_copy_deep(temp, bg_map_);
  return true;
}
