#ifndef bstm_data_similarity_traits_h
#define bstm_data_similarity_traits_h
//
#include <iostream>
#include <limits>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>
#include <bstm/bstm_data_traits.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>

#define SIMILARITY_T 0.0001
#define APP_COMP_P_THRESHOLD 0.5
#define EPSILON 0.00001f
#define square_of(x) (x) * (x)

template <bstm_data_type TYPE, boxm2_data_type BOXM2_TYPE>
class bstm_similarity_traits;

template <> class bstm_similarity_traits<BSTM_MOG3_GREY, BOXM2_MOG3_GREY> {
public:
  static bool is_similar(bstm_data_traits<BSTM_MOG3_GREY>::datatype app,
                         boxm2_data_traits<BOXM2_MOG3_GREY>::datatype boxm2_app,
                         float p,
                         float boxm2_p,
                         double /* p_threshold */,
                         double /* app_threshold */) {
    double isabel_measure =
        (bstm_data_traits<BSTM_MOG3_GREY>::processor::expected_color(app) + 1) *
        p;
    double isabel_measure_boxm2 =
        (boxm2_processor_type<BOXM2_MOG3_GREY>::type::expected_color(
             boxm2_app) +
         1) *
        boxm2_p;

    // boxm2_p > 0 to make sure empty voxels always lead to time division
    // otherwise it leads to motion artefacts.
    return std::fabs(isabel_measure - isabel_measure_boxm2) < SIMILARITY_T &&
           (boxm2_p > 0 || p == 0);
  }
};

template <> class bstm_similarity_traits<BSTM_MOG6_VIEW, BOXM2_MOG6_VIEW> {
public:
  static bool
  is_similar(bstm_data_traits<BSTM_MOG6_VIEW>::datatype /*app*/,
             boxm2_data_traits<BOXM2_MOG6_VIEW>::datatype /*boxm2_app*/,
             float /*p*/,
             float /*boxm2_p*/,
             double /*p_threshold*/,
             double /*app_threshold*/) {
    return true;
  }
};

template <>
class bstm_similarity_traits<BSTM_MOG6_VIEW_COMPACT, BOXM2_MOG6_VIEW_COMPACT> {
public:
#if 0
  static bool old_is_similar(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datatype app, boxm2_data_traits<BOXM2_MOG6_VIEW_COMPACT>::datatype boxm2_app, float p, float boxm2_p, double p_threshold, double app_threshold )
  {
    std::cout << "KL surf: " << kl_div_surf( p, boxm2_p ) << " KL app: " << kl_div_app(app, boxm2_app ) << std::endl;
    //it was 0.1 before...
    if (std::fabs(p - boxm2_p) > 0.15 || (boxm2_p == 0 && p != 0)  ) //if p change is over SIMILARITY_T or the new p is 0 but the current p is not.
      return false;
    else
    {
      if (p < 0.1)
        return true;

      //all 8 components must be similar
      float mean_change = 0;
      for (int i = 0; i < 8; i++)
        mean_change  += std::fabs( ((float)app[2*i])/255.0 - ((float)boxm2_app[2*i])/255.0) ;
      mean_change /= 8;

      return mean_change < 0.1;
    }
  }
#endif
  static bool
  is_similar(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datatype app,
             boxm2_data_traits<BOXM2_MOG6_VIEW_COMPACT>::datatype boxm2_app,
             float p,
             float boxm2_p,
             double p_threshold,
             double app_threshold) {
    return (p_threshold < 0.0f || kl_div_surf(p, boxm2_p) < p_threshold) &&
           (app_threshold < 0.0f || p < APP_COMP_P_THRESHOLD ||
            (kl_div_app(app, boxm2_app) < app_threshold));
  }

  // D(boxm2_P || P), measure of information lost when P is used to approximate
  // boxm2_P
  static float kl_div_surf(float p, float boxm2_p) {
    // add epsilons if they are 0
    if (p < EPSILON)
      p = EPSILON;
    if (boxm2_p < EPSILON)
      boxm2_p = EPSILON;

    // subtract epsilons if they are 1
    if (p > 1 - EPSILON)
      p = 1 - EPSILON;
    if (boxm2_p > 1 - EPSILON)
      boxm2_p = 1 - EPSILON;

    return (boxm2_p * std::log(boxm2_p / p) +
            (1 - boxm2_p) * std::log((1 - boxm2_p) / (1 - p))) /
           vnl_math::ln2;
  }

  static float
  kl_div_app(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datatype app,
             boxm2_data_traits<BOXM2_MOG6_VIEW_COMPACT>::datatype boxm2_app) {
    float kl_div_app = 0.0f;
    for (int i = 0; i < 8; i++) {
      float mean = ((float)app[2 * i]) / 255.0;
      float std = ((float)app[2 * i + 1]) / 255.0;
      float boxm2_mean = ((float)boxm2_app[2 * i]) / 255.0;
      float boxm2_std = ((float)boxm2_app[2 * i + 1]) / 255.0;
      if (boxm2_std < EPSILON)
        boxm2_std = EPSILON;
      if (std < EPSILON)
        std = EPSILON;
      kl_div_app += -0.5f + (std::log(std / boxm2_std)) / vnl_math::ln2 +
                    (square_of(mean - boxm2_mean) + square_of(boxm2_std)) /
                        (2 * std * std);
    }
    return kl_div_app / 8;
  }
};

template <>
class bstm_similarity_traits<BSTM_GAUSS_RGB_VIEW_COMPACT,
                             BOXM2_GAUSS_RGB_VIEW_COMPACT> {
public:
  static bool is_similar(
      bstm_data_traits<BSTM_GAUSS_RGB_VIEW_COMPACT>::datatype app,
      boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW_COMPACT>::datatype boxm2_app,
      float p,
      float boxm2_p,
      double p_threshold,
      double app_threshold) {
    return (p_threshold < 0.0f || kl_div_surf(p, boxm2_p) < p_threshold) &&
           (app_threshold < 0.0f || p < APP_COMP_P_THRESHOLD ||
            (app_rms(app, boxm2_app) < app_threshold));
  }

  // D(boxm2_P || P), measure of information lost when P is used to approximate
  // boxm2_P
  static float kl_div_surf(float p, float boxm2_p) {
    // add epsilons if they are 0
    if (p < EPSILON)
      p = EPSILON;
    if (boxm2_p < EPSILON)
      boxm2_p = EPSILON;

    // subtract epsilons if they are 1
    if (p > 1 - EPSILON)
      p = 1 - EPSILON;
    if (boxm2_p > 1 - EPSILON)
      boxm2_p = 1 - EPSILON;

    return (boxm2_p * std::log(boxm2_p / p) +
            (1 - boxm2_p) * std::log((1 - boxm2_p) / (1 - p))) /
           vnl_math::ln2;
  }

  static float
  app_rms(bstm_data_traits<BSTM_GAUSS_RGB_VIEW_COMPACT>::datatype app,
          boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW_COMPACT>::datatype boxm2_app) {
    float rms = 0.0f;
    for (int i = 0; i < 8; i++) {
      vnl_vector_fixed<unsigned char, 4> rgb =
          (vnl_vector_fixed<unsigned char, 4>)app[i];
      vnl_vector_fixed<unsigned char, 4> boxm2_rgb =
          (vnl_vector_fixed<unsigned char, 4>)boxm2_app[i];

      rms += std::sqrt((square_of(rgb[0] - boxm2_rgb[0]) +
                        square_of(rgb[1] - boxm2_rgb[1]) +
                        square_of(rgb[2] - boxm2_rgb[2])) /
                       3.0);
    }

    return rms / 8;
  }
};

template <> class bstm_similarity_traits<BSTM_GAUSS_RGB, BOXM2_GAUSS_RGB> {
public:
  static bool is_similar(bstm_data_traits<BSTM_GAUSS_RGB>::datatype app,
                         boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype boxm2_app,
                         float p,
                         float boxm2_p,
                         double p_threshold,
                         double app_threshold) {
    return (p_threshold < 0.0f || kl_div_surf(p, boxm2_p) < p_threshold) &&
           (app_threshold < 0.0f || p < APP_COMP_P_THRESHOLD ||
            (app_rms(app, boxm2_app) < app_threshold));
  }

  // D(boxm2_P || P), measure of information lost when P is used to approximate
  // boxm2_P
  static float kl_div_surf(float p, float boxm2_p) {
    // add epsilons if they are 0
    if (p < EPSILON)
      p = EPSILON;
    if (boxm2_p < EPSILON)
      boxm2_p = EPSILON;

    // subtract epsilons if they are 1
    if (p > 1 - EPSILON)
      p = 1 - EPSILON;
    if (boxm2_p > 1 - EPSILON)
      boxm2_p = 1 - EPSILON;

    return (boxm2_p * std::log(boxm2_p / p) +
            (1 - boxm2_p) * std::log((1 - boxm2_p) / (1 - p))) /
           vnl_math::ln2;
  }

  static float app_rms(bstm_data_traits<BSTM_GAUSS_RGB>::datatype app,
                       boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype boxm2_app) {
    vnl_vector_fixed<unsigned char, 8> rgb = app;
    vnl_vector_fixed<unsigned char, 8> boxm2_rgb = boxm2_app;

    float rms = std::sqrt((square_of(rgb[0] - boxm2_rgb[0]) +
                           square_of(rgb[1] - boxm2_rgb[1]) +
                           square_of(rgb[2] - boxm2_rgb[2])) /
                          3.0);

    return rms;
  }
};

template <bstm_data_type BSTM_TYPE>
bool is_similar(typename bstm_data_traits<BSTM_TYPE>::datatype app1,
                typename bstm_data_traits<BSTM_TYPE>::datatype app2,
                float p1,
                float p2,
                double p_threshold,
                double app_threshold) {
  return bstm_similarity_traits<BSTM_TYPE,
                                bstm_to_boxm2_type<BSTM_TYPE>::boxm2_type>::
      is_similar(app1, app2, p1, p2, p_threshold, app_threshold);
}

#undef square_of

#endif
