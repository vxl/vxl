#include <string>
#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h>
//For backwards compatibility
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_image_sptr.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_gauss_sf1.h>

#include <bbgm/bbgm_update.h>
#include <bbgm/bbgm_loader.h>
#include <bbgm/pro/bbgm_processes.h>
#include <bbgm/pro/bbgm_register.h>
#include <bsta/bsta_gaussian_indep.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_random.h>
#include <brdb/brdb_value.h>
#include <brdb/brdb_value_sptr.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_process_sptr.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_macros.h>

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

void test_io_function_2(void)
{
  std::cout << "Starting test_io2\n";
  constexpr float window_size = 50.0;
  constexpr unsigned int max_components = 3;
  const float init_var = 0.01f;
  const unsigned int ni = 640, nj = 480;

  float init_mean = 0.0f;
  //float init_covar(init_var);

  vil_image_view<float> img(ni,nj,1);
  init_random_image(img);

  typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
  typedef bsta_mixture_fixed<gauss_type,3> mix_gauss_type;
  typedef bsta_num_obs<mix_gauss_type> obs_mix_gauss_type;

  bsta_gauss_sf1 init_gauss( init_mean, init_var );
  bsta_mg_window_updater<mix_gauss_type> updater( init_gauss,
                                                  max_components,
                                                  window_size);

  bbgm_image_of<obs_mix_gauss_type>* mptr =
    new bbgm_image_of<obs_mix_gauss_type>(ni,nj,obs_mix_gauss_type());

  update(*mptr,img,updater);

  bbgm_image_sptr mp = mptr;
  std::string source = mp->is_a();
  std::cout << "Starting save/read bbgm_image_sptr\n"
           << "Saving an image_of with type " << source << '\n';
  bprb_process_sptr save_p= bprb_batch_process_manager::instance()->get_process_by_name("bbgmSaveImageOfProcess");
  brdb_value_sptr mv = new brdb_value_t<bbgm_image_sptr>(mp);
  brdb_value_sptr pv = new brdb_value_t<std::string>(std::string("./background.md"));
  bool good = save_p->set_input(0, pv);
  good = good && save_p->set_input(1, mv);
  good = good &&  save_p->execute();
  bprb_process_sptr load_p = bprb_batch_process_manager::instance()->get_process_by_name("bbgmLoadImageOfProcess");
  good = good&& load_p->set_input(0, pv);
  good = good &&  load_p->execute();
  brdb_value_sptr iv = load_p->output(0);
  if (!iv) good = false;
  std::string test;
  if (good){
    auto* vp =
    static_cast<brdb_value_t<bbgm_image_sptr>*>(iv.ptr());
    test = vp->value()->is_a();
  }
  std::cout << "Retrieved image_of with type " << test << '\n';
  good = good && source==test;
  TEST("test save and load image_of", good, true);
  vpl_unlink("./background.md");
}

static void test_io()
{
  REGISTER_DATATYPE_LONG_FORM(std::string, vcl_string );
  REGISTER_DATATYPE( bbgm_image_sptr );
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bbgm_save_image_of_process, "bbgmSaveImageOfProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bbgm_load_image_of_process, "bbgmLoadImageOfProcess");
  std::cout << "Starting test_io\n";
  bbgm_loader::register_loaders();
  constexpr float window_size = 50.0;
  constexpr unsigned int max_components = 3;
  const float init_var = 0.01f;
  const unsigned int ni = 640, nj = 480;

  vnl_vector_fixed<float,3> init_mean(0.0f);
  vnl_vector_fixed<float,3> init_covar(init_var);

  vil_image_view<float> img(ni,nj,3);
  init_random_image(img);

  typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
  typedef bsta_mixture_fixed<gauss_type,3> mix_gauss_type;
  typedef bsta_num_obs<mix_gauss_type> obs_mix_gauss_type;

  typedef bsta_num_obs<bsta_gauss_sf1> sph_gauss_type;

  bsta_gauss_if3 init_gauss( init_mean, init_covar );
  bsta_mg_window_updater<mix_gauss_type> updater( init_gauss,
                                                  max_components,
                                                  window_size);

  bbgm_image_of<obs_mix_gauss_type>* mptr =
    new bbgm_image_of<obs_mix_gauss_type>(ni,nj,obs_mix_gauss_type());

  update(*mptr,img,updater);

  bbgm_image_sptr mp = mptr;
  std::string source = mp->is_a();
  std::cout << "Starting save/read bbgm_image_sptr\n"
           << "Saving an image_of with type " << source << '\n';
 bprb_process_sptr save_p = bprb_batch_process_manager::instance()->get_process_by_name("bbgmSaveImageOfProcess");
 bprb_process_sptr load_p = bprb_batch_process_manager::instance()->get_process_by_name("bbgmLoadImageOfProcess");
  brdb_value_sptr mv = new brdb_value_t<bbgm_image_sptr>(mp);
  brdb_value_sptr pv = new brdb_value_t<std::string>(std::string("./background.md"));
  bool good = save_p->set_input(0, pv);
  good = good && save_p->set_input(1, mv);
  good = good && save_p->execute();
  good = good && load_p->set_input(0, pv);
  good = good && load_p->execute();
  brdb_value_sptr iv = load_p->output(0);
  if (!iv) good = false;
  std::string test;
  if (good){
    auto* vp =
    static_cast<brdb_value_t<bbgm_image_sptr>*>(iv.ptr());
    test = vp->value()->is_a();
  }
  std::cout << "Retrieved image_of with type " << test << '\n';
  good = good && source==test;
  TEST("test save and load image_of", good, true);
  vpl_unlink("./background.md");
  test_io_function_2();
}

TESTMAIN(test_io);
