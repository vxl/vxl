#include <testlib/testlib_test.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_image_sptr.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/algo/bsta_adaptive_updater.h>

#include <bbgm/bbgm_update.h>
#include <bbgm/bbgm_loader.h>
#include <bsta/bsta_gaussian_indep.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_random.h>
#include <bbgm/pro/bbgm_save_image_of_process.h>
#include <bbgm/pro/bbgm_load_image_of_process.h>
#include <brdb/brdb_value.h>
#include <brdb/brdb_value_sptr.h>

namespace
{
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
};

MAIN( test_io )
{
  vcl_cout << "Starting test_io \n";
  bbgm_loader::register_loaders();
  const float window_size = 50.0;
  const unsigned int max_components = 3;
  const float init_var = 0.01f;
  const unsigned int ni = 640, nj = 480;

  vnl_vector_fixed<float,3> init_mean(0.0f);
  vnl_vector_fixed<float,3> init_covar(init_var);

  vil_image_view<float> img(ni,nj,3);
  init_random_image(img);

  typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
  typedef bsta_mixture_fixed<gauss_type,3> mix_gauss_type;
  typedef bsta_num_obs<mix_gauss_type> obs_mix_gauss_type;

  bsta_gauss_if3 init_gauss( init_mean, init_covar );
  bsta_mg_window_updater<mix_gauss_type> updater( init_gauss,
                                                  max_components,
                                                  window_size);

  bbgm_image_of<obs_mix_gauss_type>* mptr =
    new bbgm_image_of<obs_mix_gauss_type>(ni,nj,obs_mix_gauss_type());

  update(*mptr,img,updater);

  bbgm_image_sptr mp = mptr;
  vcl_string source = mp->is_a();
  vcl_cout << "Starting save/read bbgm_image_sptr\n"
           << "Saving an image_of with type " << source << '\n';
  bprb_process_sptr lm = new bbgm_load_image_of_process();
  bprb_process_sptr sm = new bbgm_save_image_of_process();
  brdb_value_sptr mv = new brdb_value_t<bbgm_image_sptr>(mp);
  brdb_value_sptr pv = new brdb_value_t<vcl_string>(vcl_string("./background.md"));
  sm->set_input(0, pv);
  sm->set_input(1, mv);
  bool good = sm->execute();
  lm->set_input(0, pv);
  good = good&&lm->execute();
  brdb_value_sptr iv = lm->output(0);
  if (!iv) good = false;
  vcl_string test;
  if (good){
    brdb_value_t<bbgm_image_sptr>* vp =
    static_cast<brdb_value_t<bbgm_image_sptr>*>(iv.ptr());
    test = vp->value()->is_a();
  }
  vcl_cout << "Retrieved image_of with type " << test << '\n';
  good = good && source==test;
  TEST("test save and load image_of", good, true);
  vpl_unlink("./background.md");
  SUMMARY();
}

