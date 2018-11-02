#include <iostream>
#include "sdet_atmospheric_image_classifier.h"
//
#include <vnl/vnl_numeric_traits.h>
#include <vul/vul_timer.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vil/vil_load.h>
// test if a given texture category is an atmospheric effect
bool sdet_atmospheric_image_classifier::atmos_cat(std::string const& cat)
{
  for (auto & atmos_categorie : atmos_categories_)
    if (cat == atmos_categorie)
      return true;
  return false;
}
//
// compute the subjective logic opinion,
// omega =  {b_no_atmos, b_atmos, uncertainty, a}
// a = 0.5. For the moment, uncertainty is assigned the
// probability of desert texture since that category is
// easily confused with haze, a true atmospheric effect
// admittedly this approach is not on solid grounds but
// seems to work well in a large number of situations
//
void sdet_atmospheric_image_classifier::
category_quality_color_mix(std::map<std::string, float>& probs,
                           vnl_vector_fixed<float, 3> const& uncert,
                           vnl_vector_fixed<float, 3>& color_mix)
{
  //start with max prob color
  auto hit = category_histograms_.begin();
  // accumulate p_atmos, p_no_atmos, and p_haze
  float atmos_sum = 0.0f;
  float no_atmos_sum = 0.0f;
  float prob_sum = 0.0f;
  float up = 0.0f;
  for (; hit != category_histograms_.end(); ++hit) {
    const std::string& cat = (*hit).first;
    float p = probs[cat];
    prob_sum += p;
    if (atmos_cat(cat))
      atmos_sum += p;
    else if (cat=="des") // cludge for the moment-JLM
      up = p;
    else
      no_atmos_sum += p;
  }
  // define uncertainty as p_haze
  float u = up/prob_sum;
  // normalize sums to probabilities
  float p_bad  = atmos_sum/(prob_sum-up);
  float p_good = no_atmos_sum/(prob_sum-up);
  // potentially reduce uncertainty so that beliefs are not negative
  float p_min = p_bad;
  if (p_good<p_min) p_min = p_good;
  if ((p_min-0.5f*u) < 0.0f) u = 2.0f*p_min;
  // form the beliefs
  float b_bad = p_bad - 0.5f*u;
  float b_good = p_good - 0.5f*u;
  vnl_vector_fixed<float, 3> good, bad;
  bad[0]  = 1.0f;   bad[1] = 0.0f;  bad[2] = 0.0f;
  good[0] = 0.0f;  good[1] = 1.0f; good[2] = 0.0f;
  if (up<prob_sum)
    color_mix = b_good*good + b_bad*bad +  u*uncert;
  else
    color_mix = u*uncert;
}

#if 0 //=====debug====
static bool required_block(int bidxu, int bidxv, int i,
                           int j, int block_size, int margin)
{
  int idxu = (i-margin)/block_size, idxv = (j-margin)/block_size;
  return bidxu == idxu && bidxv == idxv;
}
#endif // 0

vil_image_view<float> sdet_atmospheric_image_classifier::
classify_image_blocks_qual(std::string const& img_path)
{
  vil_image_resource_sptr resc = vil_load_image_resource(img_path.c_str());
  vil_image_view<float> img = scale_image(resc); // map to [0, 1]
  std::cout << "Classifying quality on image " << img_path << '\n' << std::flush;
  return classify_image_blocks_qual(img);
}

vil_image_view<float>
sdet_atmospheric_image_classifier::classify_image_blocks_qual(vil_image_view<float> const& image)
{
  std::cout << "image size(" << image.ni()<< ' ' << image.nj() << ")pixels:["
           << texton_dictionary_.size() << "]categories \n" << std::flush;
  vul_timer t;
  if (!color_map_valid_)
    this->init_color_map();
  if (!texton_index_valid_)
    this->compute_texton_index();
  this->compute_filter_bank(image);
  unsigned dim = filter_responses_.n_levels();
  std::cout << "texton dimension " << dim +2<< '\n';

  int margin = static_cast<int>(this->max_filter_radius());
  std::cout << "filter kernel margin " << margin << '\n';
  int ni = static_cast<int>(image.ni());
  int nj = static_cast<int>(image.nj());
  if ((ni-margin)<=0 || (nj-margin)<=0) {
    std::cout << "Image smaller than filter margin\n";
    return vil_image_view<float>(0, 0);
  }
  //number of pixels in a block
  unsigned block_area = block_size_*block_size_;
  float weight = 1.0f/static_cast<float>(block_area);

  vil_image_view<float> prob(ni, nj, 3);
  // fill image with the uncertainty = 1 color
  vnl_vector_fixed<float, 3> unct;
  unct[0] = 0.0f;  unct[1] = 0.0f; unct[2] = 1.0f;
  for (unsigned j = 0; j<image.nj(); ++j)
    for (unsigned i = 0; i<image.ni(); ++i)
      for (unsigned p = 0; p<3; ++p)
        prob(i,j,p) = unct[p];

  unsigned nh = texton_index_.size();
  int bidxv = 0;
  for (int j = margin; j<(nj-margin); j+=block_size_, ++bidxv) {
    int bidxu = 0;
    for (int i = margin; i<(ni-margin); i+=block_size_, ++bidxu) {
      std::vector<float> h(nh, 0.0f);
      for (unsigned r = 0; r<block_size_; ++r)
        for (unsigned c = 0; c<block_size_; ++c) {
          vnl_vector<double> temp(dim+2);
          for (unsigned f = 0; f<dim; ++f)
            temp[f]=filter_responses_.response(f)(i+c,j+r);
          temp[dim]=laplace_(i+c,j+r); temp[dim+1]=gauss_(i+c,j+r);
          //hist bins are probabilities
          //i.e., sum h[i] = 1.0 so weight should typically be 1/Nupdates
          this->update_hist(temp, weight, h);
        }

      //finished a block - compute category probabilites from the histogram
      std::map<std::string, float> texture_probs =
        this->texture_probabilities(h);
      vnl_vector_fixed<float, 3> color;
      //colorize output according to probabilities of each category
      this->category_quality_color_mix(texture_probs,unct, color);
      for (unsigned r = 0; r<block_size_; ++r)
        for (unsigned c = 0; c<block_size_; ++c)
          for (unsigned b = 0; b<3; ++b)
            prob(i+c,j+r,b) = color[b];
    }
    std::cout << '.' << std::flush;
  }
  std::cout << "\nBlock classification took " << t.real()/1000.0 << " seconds\n" << std::flush;
  return prob;
}

vil_image_view<vxl_byte>
sdet_atmospheric_image_classifier::classify_image_blocks_qual2(vil_image_view<float> const& image, std::map<std::string, unsigned char>& cat_id_map, std::map<std::string, float>& cat_percentage_map)
{
  if (!texton_index_valid_)
    this->compute_texton_index();
  this->compute_filter_bank(image);
  unsigned dim = filter_responses_.n_levels();

  int margin = static_cast<int>(this->max_filter_radius());
  int ni = static_cast<int>(image.ni());
  int nj = static_cast<int>(image.nj());

  vil_image_view<vxl_byte> prob(ni, nj);
  prob.fill(0);

  if ((ni-margin)<=0 || (nj-margin)<=0) {
    std::cout << "Image smaller than filter margin\n";
    return vil_image_view<vxl_byte>(0, 0);
  }
  //number of pixels in a block
  unsigned block_area = block_size_*block_size_;
  float weight = 1.0f/static_cast<float>(block_area);

  for (auto & iter : cat_id_map)
    cat_percentage_map[iter.first] = 0.0f;

  int max_j = margin > block_size_ ? nj-margin : nj-block_size_;
  int max_i = margin > block_size_ ? ni-margin : ni-block_size_;
  unsigned nh = texton_index_.size();
  unsigned pix_count = 0;
  std::cout << "ni: " << ni << ", nj: " << nj << std::endl;
  std::cout << "margin: " << margin << ", max_j: " << max_j << ", max_i: " << max_i << ", block_size: " << block_size_ << std::endl;

  for (int j = margin; j<max_j; j+=block_size_) {
    for (int i = margin; i<max_i; i+=block_size_) {
      std::vector<float> h(nh, 0.0f);
      for (unsigned r = 0; r<block_size_; ++r)
        for (unsigned c = 0; c<block_size_; ++c) {
          vnl_vector<double> temp(dim+2);
          for (unsigned f = 0; f<dim; ++f)
            temp[f]=filter_responses_.response(f)(i+c,j+r);
          temp[dim]=laplace_(i+c,j+r); temp[dim+1]=gauss_(i+c,j+r);
          //hist bins are probabilities
          //i.e., sum h[i] = 1.0 so weight should typically be 1/Nupdates
          this->update_hist(temp, weight, h);
        }

      // new method: just assign the highest prob class
      std::pair<std::string, float> class_prob = this->highest_prob_class(h);
      cat_percentage_map[class_prob.first] += block_area;
      for (unsigned r = 0; r < block_size_; ++r)
        for (unsigned c = 0; c < block_size_; ++c) {
          prob(i+c, j+r) = cat_id_map[class_prob.first];
          pix_count++;
        }
    }
    std::cout << j << "," << std::flush;
  }
  for (auto & iter : cat_percentage_map) {
    iter.second /= pix_count;
    iter.second *= 100.0;
  }

  return prob;
}

vil_image_view<float> sdet_atmospheric_image_classifier::
classify_image_blocks_expected(std::string const& img_path,
                               std::string const& exp_path)
{
  vil_image_resource_sptr resc = vil_load_image_resource(img_path.c_str());
  vil_image_view<float> img = scale_image(resc); // map to [0, 1]
  vil_image_resource_sptr resce = vil_load_image_resource(exp_path.c_str());
  vil_image_view<float> exp = scale_image(resce); // map to [0, 1]
  std::cout << "Classifying quality on image " << img_path << " using expected image " << exp_path << '\n' << std::flush;
  return classify_image_blocks_expected(img, exp);
}

vil_image_view<float> sdet_atmospheric_image_classifier::
classify_image_blocks_expected(vil_image_view<float> const& image,
                               vil_image_view<float> const& exp
                              )
{
  int ni = static_cast<int>(image.ni());
  int nj = static_cast<int>(image.nj());
  int ni_exp = static_cast<int>(exp.ni());
  int nj_exp = static_cast<int>(exp.nj());
  if ((ni != ni_exp) || (nj != nj_exp)) {
    std::cout << "Incoming image and expected image not of same size\n"
             << std::flush;
    return vil_image_view<float>();
  }
  std::cout << "image size(" << ni << ' '
           << nj << ")pixels \n" << std::flush;

  vul_timer t;

  if (!texton_index_valid_)
    this->compute_texton_index();
  this->compute_filter_bank(image);
  unsigned dim = filter_responses_.n_levels();
  std::cout << "texton dimension " << dim +2<< '\n';

  int margin = static_cast<int>(this->max_filter_radius());
  std::cout << "filter kernel margin " << margin << '\n';
  if ((ni-margin)<=0 || (nj-margin)<=0) {
    std::cout << "Image smaller than filter margin\n";
    return vil_image_view<float>(0, 0);
  }
  //cached filter outputs for the input image
  std::vector<vil_image_view<float> > image_resps =
    filter_responses_.responses();
  vil_image_view<float> laplace = laplace_;
  vil_image_view<float> gauss = gauss_;

  //compute new filter outputs for the expected image
  this->compute_filter_bank(exp);

  vil_image_view<float> prob(ni, nj, 3);
  // fill image with the uncertainty = 1 color
  vnl_vector_fixed<float, 3> unct;
  unct[0] = 0.0f;  unct[1] = 0.0f; unct[2] = 1.0f;
  for (unsigned j = 0; j<image.nj(); ++j)
    for (unsigned i = 0; i<image.ni(); ++i)
      for (unsigned p = 0; p<3; ++p)
        prob(i,j,p) = unct[p];

  std::vector<float>& mod_hist = category_histograms_["mod"];
  unsigned nh = mod_hist.size();
  if (nh == 0) {
    std::cout << "No model category to evaluate image\n";
    return prob;
  }
  double thr = dist_["mod"]["mod"];
  //std::vector<vnl_vector<double> >& textons = texton_dictionary_["mod"]; -- unused!
  thr *= 0.25;//temporary hard coded threshold ratio
  int bidxv = 0;
  for (int j = margin; j<(nj-margin); j+=block_size_, ++bidxv) {
    int bidxu = 0;
    for (int i = margin; i<(ni-margin); i+=block_size_, ++bidxu) {
      float pr = 0.0f, total =0.0f;
      for (unsigned r = 0; r<block_size_; ++r)
        for (unsigned c = 0; c<block_size_; ++c) {
          vnl_vector<double> temp_img(dim+2), temp_exp(dim+2);
          for (unsigned f = 0; f<dim; ++f) {
            temp_exp[f]=filter_responses_.response(f)(i+c,j+r);
            temp_img[f]=image_resps[f](i+c,j+r);
          }
          temp_exp[dim]=laplace_(i+c,j+r); temp_exp[dim+1]=gauss_(i+c,j+r);
          temp_img[dim]=laplace(i+c,j+r); temp_img[dim+1]=gauss(i+c,j+r);
          unsigned indx_exp = this->nearest_texton_index(temp_exp);
          double di_exp_img = vnl_vector_ssd(temp_exp, temp_img);
          di_exp_img = std::sqrt(di_exp_img/temp_exp.size());
          total += mod_hist[indx_exp];
          if (di_exp_img<thr)
            pr += mod_hist[indx_exp];
        }
      pr /= total;
      float u = pr/(1.0f - pr);
      if (u>1.0f) u = 1/u;
      float b_good = pr - 0.5f*u;
      if (b_good< 0.0f) b_good = 0.0f;
      float b_bad = 1.0f - u - b_good;
      vnl_vector_fixed<float, 3> color;
      //colorize output according to probabilities of each category
      for (unsigned r = 0; r<block_size_; ++r)
        for (unsigned c = 0; c<block_size_; ++c) {
          prob(i+c,j+r,0) = b_bad;
          prob(i+c,j+r,1) = b_good;
          prob(i+c,j+r,2) = u;
        }
    }
      std::cout << '.' << std::flush;
  }
  std::cout << "\nBlock classification took " << t.real()/1000.0 << " seconds\n" << std::flush;
  return prob;
}
