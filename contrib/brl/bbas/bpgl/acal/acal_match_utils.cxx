#include <cmath>

#include "vil/vil_save.h"
#include "vil/vil_image_view.h"
#include <vil/algo/vil_sobel_3x3.h>

#include "acal_match_utils.h"


bool
acal_match_utils::write_match_matrix(
     std::string match_matrix_path,
     std::map<size_t, std::map<size_t, std::vector<acal_match_pair> > >& matches,
     std::map<size_t, std::string>& img_paths)
{
  size_t n_images = img_paths.size();
  // color ramp
  std::map<size_t, std::vector<unsigned char> > c;
  std::vector<unsigned char> c0(3, 25);
  std::vector<unsigned char> c1(3, 0);                      c1[2]=170;
  std::vector<unsigned char> c2(3, 0); c2[0]=128;           c2[2]=128;
  std::vector<unsigned char> c3(3, 0);            c3[1]=75; c3[2]=100;
  std::vector<unsigned char> c4(3, 0);            c4[1]=125;
  std::vector<unsigned char> c5(3, 0); c5[0]=150; c5[1]=150;
  std::vector<unsigned char> c6(3, 0); c6[0]=255; c6[1]=130;
  std::vector<unsigned char> c7(3, 0); c7[0]=255;

  // blue to red ramp on num matches
  c[0]   = c0;
  c[5]   = c1;
  c[10]  = c2;
  c[20]  = c3;
  c[50]  = c4;
  c[100] = c5;
  c[200] = c6;
  c[500] = c7;

  size_t aindx = 0;
  std::map<size_t, size_t> img_indx_to_array_indx;
  for(std::map<size_t, std::string>::iterator iit = img_paths.begin();
      iit != img_paths.end(); ++iit, ++aindx)
    img_indx_to_array_indx[iit->first] = aindx;

  size_t npix = 10;// number of pixels for array cell
  size_t n = npix*n_images;
  vil_image_view<unsigned char> array(n, n, 3);
  array.fill(static_cast<unsigned char>(0));

  std::vector<std::vector<size_t> > nmatches(n_images, std::vector<size_t>(n_images, 0));
  for(std::map<size_t, size_t>::iterator iit = img_indx_to_array_indx.begin();
      iit != img_indx_to_array_indx.end(); ++iit){
    size_t iidx = iit->first;
    size_t i = iit->second;
    if(matches.count(iidx)==0)
      continue;
    std::map<size_t, std::vector<acal_match_pair> >& mtch = matches[iidx];
    for(std::map<size_t, size_t>::iterator jit = img_indx_to_array_indx.begin();
      jit != img_indx_to_array_indx.end(); ++jit){
      size_t jidx = jit->first;
      size_t j = jit->second;
      if(mtch.count(jidx)==0)
        continue;
	  size_t nm = mtch[jidx].size();
	  nmatches[i][j] = nm;
      nmatches[j][i] = nmatches[i][j];
    }
  }

  for(size_t i =0; i<n_images; ++i){
    size_t start_idx1 = npix*i, end_idx1 = start_idx1 + npix -1;
    for(size_t j =0; j<n_images; ++j){
      size_t start_idx2 = npix*j, end_idx2 = start_idx2 + npix -1;
      size_t nm = nmatches[i][j];
      bool found = false;
      std::vector<unsigned char> clr;
      for(std::map<size_t, std::vector<unsigned char> >::iterator cit = c.begin();
          cit != c.end()&&!found; ++cit){
        if(cit->first >= nm){
          clr = cit->second;
          found = true;
        }
      }
      if(!found)
        clr = c7;
      for(size_t ii = start_idx1; ii<=end_idx1; ++ii)
        for(size_t jj = start_idx2; jj<=end_idx2; ++jj)
          for(size_t p = 0; p<3; ++p)
            array(ii, jj, p) = clr[p];

      for(size_t ii = start_idx1; ii<=end_idx1; ++ii)
        for(size_t jj = start_idx2; jj<=end_idx2; ++jj)
          if(ii == start_idx1 || ii == end_idx1 || jj == start_idx2 || jj == end_idx2){
            array(ii, jj, 0) = 0;  array(ii, jj, 1) = 0; array(ii, jj, 2) = 0;
          }
    }
  }
  return vil_save(array, match_matrix_path.c_str());
}


void
acal_match_utils::intersect_match_pairs(
    std::vector<acal_match_pair> const& mpairs_a,
    std::vector<acal_match_pair> const& mpairs_b,
    std::vector<acal_match_pair>& intersected_mpairs_a,
    std::vector<acal_match_pair>& intersected_mpairs_b,
    bool intersect_a1_with_b1)
{
  intersected_mpairs_a.clear();
  intersected_mpairs_b.clear();
  size_t na = mpairs_a.size();
  size_t nb = mpairs_b.size();
  for(size_t i = 0; i<na; ++i){
    const acal_match_pair& ma = mpairs_a[i];
    bool found = false;
    size_t j_found = -1;
    for(size_t j = 0; j<nb&&!found; ++j){
      const acal_match_pair& mb = mpairs_b[j];
      if(intersect_a1_with_b1){
        found = ma.corr1_.id_ == mb.corr1_.id_;
        j_found = j;
      }else{
        found = ma.corr1_.id_ == mb.corr2_.id_;
        j_found = j;
      }
    }
    if(found){
      intersected_mpairs_a.push_back(ma);
      intersected_mpairs_b.push_back(mpairs_b[j_found]);
    }
  }
}


void
acal_match_utils::reduce_match_pairs_a_by_b(
    std::vector<acal_match_pair> const& mpairs_a,
    std::vector<acal_match_pair> const& mpairs_b,
    std::vector<acal_match_pair>& reduced_mpairs_a,
    std::vector<acal_match_pair>& reduced_mpairs_b)
{
  reduced_mpairs_a.clear();
  size_t na = mpairs_a.size();
  size_t nb = mpairs_b.size();
  for(size_t i = 0; i<na; ++i){
    const acal_match_pair& ma = mpairs_a[i];
    bool found = false;
    size_t j_found = -1;
    for(size_t j = 0; j<nb&&!found; ++j){
      const acal_match_pair& mb = mpairs_b[j];
      found = ma.corr2_.id_ == mb.corr1_.id_;
      j_found = j;
    }
    if(found){
      reduced_mpairs_a.push_back(ma);
      reduced_mpairs_b.push_back(mpairs_b[j_found]);
    }
  }
}


void
acal_match_utils::reduce_node_mpairs(
    std::vector<acal_match_pair> const& node_mpairs,
    std::vector<acal_corr> const& reduced_node_corrs,
    std::vector<acal_match_pair> & reduced_node_mpairs)
{
  reduced_node_mpairs.clear();
  size_t na = node_mpairs.size();
  size_t nb = reduced_node_corrs.size();
  for(size_t i = 0; i<na; ++i){
    const acal_match_pair& ma = node_mpairs[i];
    bool found = false;
    size_t j_found = -1;
    for(size_t j = 0; j<nb&&!found; ++j){
      const acal_corr& cr = reduced_node_corrs[j];
      found = ma.corr1_.id_ == cr.id_;
    }
    if(found)
      reduced_node_mpairs.push_back(ma);
  }
}


void
acal_match_utils::reverse(std::vector<acal_match_pair>& mpairs)
{
  size_t n = mpairs.size();
  for(size_t i = 0; i<n; ++i){
    acal_corr corr1 = mpairs[i].corr1_;
    acal_corr corr2 = mpairs[i].corr2_;
    mpairs[i].corr1_ = corr2;
    mpairs[i].corr2_ = corr1;
  }
}


bool
acal_match_utils::norm_xcorr_score(
    vil_image_view<vxl_byte> const& a,
    vil_image_view<vxl_byte> const& b,
    double& score)
{
  int ni = a.ni(), nj = a.nj();
  if(ni != b.ni() || nj != b.nj()){
    std::cout << "incompatable patch sizes" << std::endl;
    return false;
  }
  double mean_a = 0.0, mean_b = 0.0, nm = 0.0, sq_a = 0.0, sq_b = 0.0;
  for(int v = 0; v<nj; ++v)
    for(int u =0; u<ni; ++u){
      double va = static_cast<double>(a(u,v)), vb = static_cast<double>(b(u,v));
      mean_a += va;
      mean_b += vb;
      sq_a += va * va;
      sq_b += vb * vb;
      nm += 1.0;
    }
  mean_a /= nm; mean_b /= nm;
  sq_a /= nm; sq_b/=nm;
  double var_a = sq_a - (mean_a * mean_a);
  double var_b = sq_b - (mean_b * mean_b);
  nm = 0.0;
  score = 0.0;
  for(int v = 0; v<nj; ++v) {
    for(int u =0; u<ni; ++u) {
      double va = static_cast<double>(a(u,v)), vb = static_cast<double>(b(u,v));
      if(va == 0.0 || vb == 0.0)
        continue;
      nm += 1.0;
      double da = (va-mean_a)/sqrt(var_a), db = (vb-mean_b)/sqrt(var_b);
      score += da * db;
    }
  }
  if(nm == 0.0)
    score = 0.0;
  else
    score /= nm;
  return true;
}


bool
acal_match_utils::gradient_mag_xcorr_score(
    vil_image_view<vxl_byte> const& a,
    vil_image_view<vxl_byte> const& b,
    double& score)
{
  vil_image_view<double> gradai, gradaj, gradbi, gradbj;
  vil_sobel_3x3(a, gradai, gradaj);
  vil_sobel_3x3(b, gradbi, gradbj);
  int ni = a.ni(), nj = a.nj();
  if(ni != b.ni() || nj != b.nj()){
    std::cout << "incompatable patch sizes" << std::endl;
    return false;
  }
  double mean_a = 0.0, mean_b = 0.0, nm = 0.0, sq_a = 0.0, sq_b = 0.0;
  for(int v = 0; v<nj; ++v) {
    for(int u =0; u<ni; ++u){
      double gai = gradai(u,v), gaj = gradaj(u,v);
      double gbi = gradbi(u,v), gbj = gradbj(u,v);
      double gma = sqrt(gai*gai + gaj*gaj);
      double gmb = sqrt(gbi*gbi + gbj*gbj);
      mean_a += gma;
      mean_b += gmb;
      sq_a += gma * gma;
      sq_b += gmb * gmb;
      nm += 1.0;
    }
  }
  mean_a /= nm; mean_b /= nm;
  sq_a /= nm; sq_b/=nm;
  double var_a = sq_a - (mean_a * mean_a);
  double var_b = sq_b - (mean_b * mean_b);
  nm = 0.0;
  score = 0.0;
  for(int v = 0; v<nj; ++v) {
    for(int u =0; u<ni; ++u){
      double gai = gradai(u,v), gaj = gradaj(u,v);
      double gbi = gradbi(u,v), gbj = gradbj(u,v);
      double gma = sqrt(gai*gai + gaj*gaj);
      double gmb = sqrt(gbi*gbi + gbj*gbj);
      nm += 1.0;
      double da = (gma-mean_a)/sqrt(var_a), db = (gmb-mean_b)/sqrt(var_b);
      score += da * db;
    }
  }
  if(nm == 0.0)
    score = 0.0;
  else
    score /= nm;
  return true;
}


bool
acal_match_utils::gradient_direction_score(
    vil_image_view<vxl_byte> const& a,
    vil_image_view<vxl_byte> const& b,
    double& score)
{
  vil_image_view<double> gradai, gradaj, gradbi, gradbj;
  vil_sobel_3x3(a, gradai, gradaj);
  vil_sobel_3x3(b, gradbi, gradbj);
  int ni = a.ni(), nj = a.nj();
  if(ni != b.ni() || nj != b.nj()){
    std::cout << "incompatable patch sizes" << std::endl;
    return false;
  }
  double nm = 0.0;
  score = 0.0;
  for(int v = 0; v<nj; ++v) {
    for(int u =0; u<ni; ++u){
      double gai = gradai(u,v), gaj = gradaj(u,v);
      double gbi = gradbi(u,v), gbj = gradbj(u,v);
      vgl_vector_2d<double> dira(gai, gaj);
      double la = dira.length();
      if(la>0.0)
        dira/=la;
      else
        continue;
      vgl_vector_2d<double> dirb(gbi, gbj);
      double lb = dirb.length();
      if(lb>0.0)
        dirb/=lb;
      else
        continue;
      double s = (la + lb)/2.0;
      double dp = dot_product(dira, dirb);
      score += s*dp;
      nm += s;
    }
  }
  score /= nm;
  return true;
}

