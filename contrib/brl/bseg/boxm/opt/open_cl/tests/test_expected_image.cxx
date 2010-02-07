#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <boxm/opt/open_cl/boxm_ray_trace_manager.h>
#include <boxm/opt/boxm_update_image_functor.h>
#include <boxm/opt/boxm_render_expected_image_functor.h>
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_loc_code.h>
#include <boxm/boxm_block_vis_graph_iterator.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_utils.h>
#include <vgl/vgl_intersection.h>

#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>

bool generate_ray_init(vpgl_perspective_camera<double> *cam_,vgl_box_3d<double> const& block_bb, vgl_box_2d<double> &img_bb,unsigned ni,unsigned nj)
{
    // determine intersection of block bounding box projection and image bounds
    vgl_box_2d<double> img_bounds;
    img_bounds.add(vgl_point_2d<double>(0,0));
    img_bounds.add(vgl_point_2d<double>(0 + ni - 1, 0 + nj - 1));

    vgl_box_2d<double> block_projection;
    double u,v;
    cam_->project(block_bb.min_x(),block_bb.min_y(),block_bb.min_z(),u,v);
    block_projection.add(vgl_point_2d<double>(u,v));
    cam_->project(block_bb.min_x(),block_bb.min_y(),block_bb.max_z(),u,v);
    block_projection.add(vgl_point_2d<double>(u,v));
    cam_->project(block_bb.min_x(),block_bb.max_y(),block_bb.min_z(),u,v);
    block_projection.add(vgl_point_2d<double>(u,v));
    cam_->project(block_bb.min_x(),block_bb.max_y(),block_bb.max_z(),u,v);
    block_projection.add(vgl_point_2d<double>(u,v));
    cam_->project(block_bb.max_x(),block_bb.min_y(),block_bb.min_z(),u,v);
    block_projection.add(vgl_point_2d<double>(u,v));
    cam_->project(block_bb.max_x(),block_bb.min_y(),block_bb.max_z(),u,v);
    block_projection.add(vgl_point_2d<double>(u,v));
    cam_->project(block_bb.max_x(),block_bb.max_y(),block_bb.min_z(),u,v);
    block_projection.add(vgl_point_2d<double>(u,v));
    cam_->project(block_bb.max_x(),block_bb.max_y(),block_bb.max_z(),u,v);
    block_projection.add(vgl_point_2d<double>(u,v));

    img_bb=vgl_intersection(img_bounds,block_projection);

    if (img_bb.is_empty())
        return false;
    else
        return true;
}


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

vil_image_view<float> run_expected_image(boxm_scene<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY > > > * s,vpgl_perspective_camera<double> * pcam,unsigned ni, unsigned nj)
{
    boxm_ray_trace_manager<boxm_sample<BOXM_APM_MOG_GREY> >* ray_mgr = boxm_ray_trace_manager<boxm_sample<BOXM_APM_MOG_GREY> >::instance();
    ray_mgr->set_perspective_camera(pcam);
    ray_mgr->setup_ray_origin();
    ray_mgr->setup_expected_image(ni,nj);
    ray_mgr->setup_camera();
    ray_mgr->setup_img_dims(ni,nj);
    typedef  boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY > > tree_type;
    boxm_block_vis_graph_iterator<tree_type > block_vis_iter(pcam, s, ni,nj);

    while (block_vis_iter.next())
    {
        vcl_vector<vgl_point_3d<int> > block_indices = block_vis_iter.frontier_indices();
        for (unsigned i=0; i<block_indices.size(); i++) // code for each block
        {
            s->load_block(block_indices[i]);
            boxm_block<tree_type> * curr_block=s->get_active_block();
            vcl_cout << "processing block at index (" <<block_indices[i] << ')' << vcl_endl;
            // make sure block projects to inside of image
            vgl_box_3d<double> block_bb = curr_block->bounding_box();

            if (!boxm_utils::is_visible(block_bb,pcam,ni,nj))
                continue;
            vgl_box_2d<double> img_bb;
            // initialize ray_origin() function for this block
            if (!generate_ray_init(pcam,block_bb, img_bb,ni,nj)) {
                continue;
            }

            tree_type * tree=curr_block->get_tree();

            ray_mgr->set_tree(tree);
            ray_mgr->setup_tree();

            ray_mgr->setup_roi_dims(img_bb.min_x(),img_bb.max_x(),img_bb.min_y(),img_bb.max_y());

            ray_mgr->setup_tree_input_buffers();
            ray_mgr->setup_camera_input_buffer();
            ray_mgr->setup_roidims_input_buffer();
            ray_mgr->setup_ray_origin_buffer();
            ray_mgr->setup_expected_img_buffer();
            ray_mgr->setup_tree_global_bbox_buffer();
            ray_mgr->setup_imgdims_buffer();

            ray_mgr->run();

            //save_expected_image("./gpuexpected",250,250,ray_mgr->ray_results());
        }
    }
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

    float dist=0;
    for (unsigned i=0;i<im_nongpu.ni();i++)
    {
        for (unsigned j=0;j<im_nongpu.nj();j++)
        {
            dist+=vcl_fabs(im_nongpu(i,j)-im_gpu(i,j));
        }
    }
    s.clean_scene();

    if (dist<1e2)
        TEST("test_expected_image_driver", true, true);
    else
        TEST("test_expected_image_driver", true, false);
}

TESTMAIN(test_expected_image);
