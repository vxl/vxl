#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <boxm/ocl/boxm_ray_trace_manager.h>
#include <boxm/algo/rt/boxm_update_image_functor.h>
#include <boxm/algo/rt/boxm_render_expected_image_functor.h>
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_loc_code.h>

#include <boxm/boxm_scene.h>
#include <boxm/util/boxm_utils.h>

#include <vcl_where_root_dir.h>

#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>


void save_expected_image(vcl_string const& image_path,
                         unsigned ni, unsigned nj,
                         float* expected_img)
{
    if (!expected_img)
        return;
    vil_image_view<float> out0(ni, nj);
    vil_image_view<float> out1(ni, nj);
    vil_image_view<float> out2(ni, nj);
    vil_image_view<float> out3(ni, nj);

    unsigned expt_ptr = 0;
    for (unsigned i = 0; i<ni; ++i) {
        for (unsigned j = 0; j<nj; ++j)  {
            out0(i,j) = expected_img[expt_ptr];++expt_ptr;
            out1(i,j) = expected_img[expt_ptr];++expt_ptr;
            out2(i,j) = expected_img[expt_ptr];++expt_ptr;
            out3(i,j) = expected_img[expt_ptr];++expt_ptr;
        }
    }
    vil_save(out0, (image_path+"0.tiff").c_str());
    vil_save(out1, (image_path+"1.tiff").c_str());
    vil_save(out2, (image_path+"2.tiff").c_str());
    vil_save(out3, (image_path+"3.tiff").c_str());
}

vil_image_view<float> expected_image(unsigned ni, unsigned nj,
                                     float* expected_img)
{
    vil_image_view<float> out(ni, nj);
    if (!expected_img)
        return out;

    unsigned expt_ptr = 0;
    for (unsigned i = 0; i<ni; ++i) {
        for (unsigned j = 0; j<nj; ++j)  {
            ++expt_ptr;
            ++expt_ptr;
            out(i,j) = expected_img[expt_ptr];++expt_ptr;
            ++expt_ptr;
        }
    }
    return out;
}

vil_image_view<float> run_expected_image(boxm_scene<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY > > > * scene, vpgl_perspective_camera<double> * pcam,unsigned ni, unsigned nj)
{
    // set up the application-specific function to be called at every cell along a ray
    vcl_string expected_img_functor_fname = vcl_string(VCL_SOURCE_ROOT_DIR)
        +"/contrib/brl/bseg/boxm/opt/open_cl/expected_functor.cl";

    vcl_vector<vcl_string> source_fnames;
    source_fnames.push_back(expected_img_functor_fname);

    boxm_ray_trace_manager<boxm_sample<BOXM_APM_MOG_GREY> >* ray_mgr = boxm_ray_trace_manager<boxm_sample<BOXM_APM_MOG_GREY> >::instance();
    ray_mgr->init_raytrace(scene, pcam, ni, nj, source_fnames);

    ray_mgr->run();

    return expected_image(250,250,ray_mgr->ray_results());
}

bool update_world(vcl_string scene_name,vcl_string camname,vcl_string imgname)
{
    boxm_scene<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY > > > s;
    s.load_scene(scene_name);
    vpgl_perspective_camera<double> *pcam=new vpgl_perspective_camera<double> ();
    vcl_ifstream ifs(camname.c_str());
    if (!ifs)
        return false;
    else
        ifs >> (*pcam);

    vil_image_view_base_sptr loaded_image = vil_load(imgname.c_str() );
    vil_image_view<vxl_byte> *img_byte= dynamic_cast<vil_image_view<vxl_byte>*>(loaded_image.ptr());
    vil_image_view<boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_datatype> img(img_byte->ni(), img_byte->nj(), 1);
    vil_convert_stretch_range_limited(*img_byte ,img, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);

    s.clean_scene();
    boxm_update_image_rt<short, boxm_sample<BOXM_APM_MOG_GREY> >(s,pcam,img,true);
    return true;
}

vil_image_view<float> render_image(vcl_string scene_name,vcl_string camname,unsigned ni,unsigned nj)
{
    boxm_scene<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY > > > s;
    s.load_scene(scene_name);
    vpgl_perspective_camera<double> *pcam=new vpgl_perspective_camera<double> ();
    vcl_ifstream ifs(camname.c_str());
    if (!ifs)
        return vil_image_view<float>();
    else
        ifs >> (*pcam);

    vil_image_view<float> expected_img(ni,nj, 1);
    vil_image_view<float> mask(ni,nj, 1);
    boxm_render_image_rt<short, boxm_sample<BOXM_APM_MOG_GREY> >(s,pcam,expected_img,mask,-1,true);

    return expected_img;
}

static void test_expected_image()
{
    vcl_string root_dir = testlib_root_dir();
    boxm_scene<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY > > > s;
    s.load_scene(root_dir+"/contrib/brl/bseg/boxm/opt/open_cl/tests/scene.xml");

    vcl_string camname=root_dir+"/contrib/brl/bseg/boxm/opt/open_cl/tests/cam_0.txt";
    vcl_string imgname=root_dir+"/contrib/brl/bseg/boxm/opt/open_cl/tests/test_img0.tif";

    vpgl_perspective_camera<double> *pcam=new vpgl_perspective_camera<double> ();
    vcl_ifstream ifs(camname.c_str());
    if (!ifs)
        return ;
    else
        ifs >> (*pcam);

    update_world(root_dir+"/contrib/brl/bseg/boxm/opt/open_cl/tests/scene.xml",camname,imgname);
    vil_image_view<float> im_nongpu=render_image(root_dir+"/contrib/brl/bseg/boxm/opt/open_cl/tests/scene.xml",camname,250,250);
    vil_image_view<float> im_gpu=run_expected_image(&s,pcam,250,250);
    s.clean_scene();
    float ssd = vil_math_ssd(im_gpu, im_nongpu, float());
    float rms_error = vcl_sqrt(ssd / im_gpu.size());

    TEST_NEAR("GPU/Non-GPU Expected Image RMS difference", rms_error, 0.0, 1e-2);
}

TESTMAIN(test_expected_image);
