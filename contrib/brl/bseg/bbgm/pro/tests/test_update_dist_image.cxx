#include <testlib/testlib_test.h>
#include <bbgm/bbgm_image_of.h>
#include <bbgm/bbgm_image_sptr.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/algo/bsta_adaptive_updater.h>

#include <bbgm/bbgm_update.h>
#include <bsta/bsta_gaussian_indep.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_random.h>
#include <brdb/brdb_value.h>
#include <brdb/brdb_value_sptr.h>
#include <bpro/bprb/bprb_process_sptr.h>
#include <bbgm/pro/bbgm_processes.h>
#include <bbgm/pro/bbgm_register.h>

void init_random_image(vil_image_view<float>& img)
{
  vnl_random rand;
  for (unsigned int p=0; p<img.nplanes(); ++p)
    for (unsigned int j=0; j<img.nj(); ++j)
      for (unsigned int i=0; i<img.ni(); ++i)
        img(i,j,p) = static_cast<float>(rand.drand32());
}

static void test_update_dist_image()
{
  REGISTER_DATATYPE( vil_image_view_base_sptr );
  REGISTER_DATATYPE( bbgm_image_sptr );
  REG_PROCESS_FUNC_CONS_INIT(bprb_func_process, bprb_batch_process_manager, bbgm_update_dist_image_process, "bbgmUpdateDistImageProcess");

  const unsigned int ni = 640, nj = 480;
  auto* ip = new vil_image_view<float>(ni,nj,3);
  init_random_image(*ip);

  brdb_value_sptr image_view_value =
    new brdb_value_t<vil_image_view_base_sptr>(ip);

  bprb_process_sptr p1 = bprb_batch_process_manager::instance()->get_process_by_name("bbgmUpdateDistImageProcess");
  bool good = p1->set_input(1, image_view_value);
  good = good && p1->set_input(2 , new brdb_value_t<int>(3));//number of mixture components
  good = good && p1->set_input(3 , new brdb_value_t<int>(300));//window size
  good = good && p1->set_input(4 , new brdb_value_t<float>(0.1f));//initial variance
  good = good && p1->set_input(5 , new brdb_value_t<float>(3.0f));//g_thresh
  good = good && p1->set_input(6 , new brdb_value_t<float>(0.02f));//min standard deviation
  good = good && p1->init();
  good = good && p1->execute();
  TEST("update distribution image process", good, true);
}

TESTMAIN(test_update_dist_image);
