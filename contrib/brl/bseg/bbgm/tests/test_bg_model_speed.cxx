#include <vector>
#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bbgm/bbgm_image_of.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/algo/bsta_adaptive_updater.h>

#include <bbgm/bbgm_update.h>
#include <bsta/bsta_gaussian_indep.h>
#include <vil/vil_image_view.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_random.h>

namespace{

void init_random_image(vil_image_view<float>& img)
{
  vnl_random rand;
  for (unsigned int p=0; p<img.nplanes(); ++p)
    for (unsigned int j=0; j<img.nj(); ++j)
      for (unsigned int i=0; i<img.ni(); ++i)
        img(i,j,p) = static_cast<float>(rand.drand32());
}


void add_random_noise(vil_image_view<float>& img, float std)
{
  vnl_random rand;
  for (unsigned int p=0; p<img.nplanes(); ++p)
    for (unsigned int j=0; j<img.nj(); ++j)
      for (unsigned int i=0; i<img.ni(); ++i){
        img(i,j) = img(i,j) + static_cast<float>(rand.normal()*std);
        if (img(i,j,p)>1.0f) img(i,j,p) = 1.0f;
        if (img(i,j,p)<0.0f) img(i,j,p) = 0.0f;
      }
}

}; // namespace

static void test_bg_model_speed()
{
  constexpr float window_size = 50.0;
  constexpr unsigned int max_components = 3;
  const float init_var = 0.01f;
  const unsigned int ni = 640, nj = 480;

  vnl_vector_fixed<float,3> init_mean(0.0f);
  vnl_vector_fixed<float,3> init_covar(init_var);

  vil_image_view<float> img(ni,nj,3);
  init_random_image(img);

  std::vector<vil_image_view<float> > images(1,img);
  for (unsigned t=1; t<10; ++t){
    vil_image_view<float> new_img;
    new_img.deep_copy(img);
    add_random_noise(new_img, 0.5f);
    images.push_back(new_img);
  }

  std::cout << "testing variable size mixture speeds" << std::endl;
  {
    typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
    typedef bsta_mixture<gauss_type> mix_gauss_type;
    typedef bsta_num_obs<mix_gauss_type> obs_mix_gauss_type;

    bsta_gauss_if3 init_gauss( init_mean, init_covar );
    bsta_mg_window_updater<mix_gauss_type> updater( init_gauss,
                                                    max_components,
                                                    window_size);

    bbgm_image_of<obs_mix_gauss_type> model(ni,nj,obs_mix_gauss_type());

    for (const auto & image : images){
      vul_timer time;
      update(model,image,updater);
      double up_time = time.real() / 1000.0;
      std::cout << " updated in " << up_time << " sec" <<std::endl;
    }
  }

  std::cout << "testing fixed size mixture speeds" << std::endl;
  {
    typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
    typedef bsta_mixture_fixed<gauss_type,3> mix_gauss_type;
    typedef bsta_num_obs<mix_gauss_type> obs_mix_gauss_type;

    bsta_gauss_if3 init_gauss( init_mean, init_covar );
    bsta_mg_window_updater<mix_gauss_type> updater(init_gauss,
                                                   max_components,
                                                   window_size);

    bbgm_image_of<obs_mix_gauss_type> model(ni,nj,obs_mix_gauss_type());

    for (const auto & image : images){
      vul_timer time;
      update(model,image,updater);
      double up_time = time.real() / 1000.0;
      std::cout << " updated in " << up_time << " sec" <<std::endl;
    }
  }
}

TESTMAIN(test_bg_model_speed);
