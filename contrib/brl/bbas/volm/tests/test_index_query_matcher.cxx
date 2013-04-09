#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <vsph/vsph_unit_sphere.h>
#include <vsph/vsph_sph_point_3d.h>
#include <depth_map/depth_map_scene.h>
#include <depth_map/depth_map_region.h>
#include <volm/volm_spherical_query_region.h>
#include <volm/volm_spherical_region_index.h>
#include <volm/volm_spherical_region_query.h>
#include <volm/volm_spherical_index_query_matcher.h>
#include <volm/volm_spherical_container.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <volm/volm_io.h>
#include <volm/volm_vrml_io.h>
#include <volm/volm_camera_space.h>

static void test_index_query_matcher()
{
    vcl_string unit_sph_file = "e:/data/Finder/unit_sphere_1_75_105.vsl";
    vsl_b_ifstream is(unit_sph_file.c_str());
    vsph_unit_sphere_sptr usph_ptr;
    vsl_b_read(is, usph_ptr);

    // inputs for query
    vcl_string depth_scene_path = "Z:/projects/FINDER/test1/p1a_test1_40/p1a_test1_40.vsl";
    vcl_string params_file = "E:/data/Finder/cam_inc_params.txt";
    vcl_string gt_camera_file ="E:/data/Finder/Camera.kml";

    // index file
    vcl_string sph_index_file = "Z:\\projects\\FINDER\\index\\geoindex_zone_17_inc_2_nh_100_pa_1\\geo_index_tile_3_node_-79.968750_32.625000_-79.937500_32.656250_index_label_orientation_hyp_32.6507_-79.949_3.07477.txt";
    double rad = 1.0;
    vgl_point_3d<float> cent(0.0,0.0,0.0);
    vgl_point_3d<double> cent_ray(0.0,0.0,0.0);
    vgl_vector_3d<double> axis_x(1.0, 0.0, 0.0);
    vgl_vector_3d<double> axis_y(0.0, 1.0, 0.0);
    vgl_vector_3d<double> axis_z(0.0, 0.0, 1.0);
    vcl_ofstream os("e:/data/Finder/matcher.wrl");
    if (!os.is_open())
        return;
    bvrml_write::write_vrml_header(os);
    vgl_sphere_3d<float> sp((float)0.0, (float)0.0, (float)0.0, (float)1.0);
    bvrml_write::write_vrml_sphere(os, sp, 0.5f, 0.5f, 0.5f, 0.0f);
    bvrml_write::write_vrml_line(os, cent_ray, axis_x, (float)rad*2, 1.0f, 0.0f, 0.0f);
    bvrml_write::write_vrml_line(os, cent_ray, axis_y, (float)rad*2, 0.0f, 1.0f, 0.0f);
    bvrml_write::write_vrml_line(os, cent_ray, axis_z, (float)rad*2, 0.0f, 1.0f, 1.0f);
    vgl_sphere_3d<float> sp2((float)cent.x(), (float)cent.y()+2, (float)cent.z(), (float)rad/10);
    bvrml_write::write_vrml_sphere(os, sp2, 0.0f, 0.0f, 1.0f, 0.0f);

    // reading depth map
    depth_map_scene_sptr dms = new depth_map_scene;
    vsl_b_ifstream dis(depth_scene_path.c_str());
    if (!dis)
        return;
    dms->b_read(dis);
    dis.close();

    volm_io_expt_params params;
    params.read_params(params_file);
    // check camera input file
    double heading, heading_dev, tilt, tilt_dev, roll, roll_dev;
    double tfov, top_fov_dev, altitude, lat, lon;
    if (!volm_io::read_camera(gt_camera_file, dms->ni(), dms->nj(), heading, heading_dev,
                              tilt, tilt_dev, roll, roll_dev, tfov, top_fov_dev, altitude, lat, lon)) {
        vcl_cout << "problem parsing camera kml file: " << gt_camera_file<< '\n';
        return ;
    }

    if (vcl_abs(heading-0) < 1e-10) heading = 180.0;
    vcl_cout << "cam params:"
             << "\n head: " << heading << " dev: " << heading_dev
             << "\n tilt: " << tilt << " dev: " << tilt_dev << " inc: " << params.tilt_inc
             << "\n roll: " << roll << " dev: " << roll_dev << " inc: " << params.roll_inc
             << "\n  fov: " << tfov << " dev: " << top_fov_dev << " inc: " << params.fov_inc
             << "\n  alt: " << altitude << vcl_endl;

    // construct camera space
    volm_camera_space_sptr  cam_space= new volm_camera_space(tfov, top_fov_dev, params.fov_inc, altitude, dms->ni(), dms->nj(),
                                                             heading, heading_dev, params.head_inc,
                                                             tilt, tilt_dev, params.tilt_inc,
                                                             roll, roll_dev, params.roll_inc);

    volm_spherical_container_sptr sph = new volm_spherical_container(params.solid_angle,
                                                                     params.vmin,
                                                                     params.dmax);

    volm_spherical_region_query srq(dms, cam_space, sph);
    srq.print(vcl_cout);
    vcl_map<vcl_string,vcl_string> filenames;
    filenames["ORIENTATION"] = sph_index_file; 
    // index
    volm_spherical_region_index region_index(filenames,unit_sph_file);
    volm_spherical_index_query_matcher matcher(region_index,srq,cam_space);
    matcher.match();

    volm_vrml_io::display_spherical_region_layer_by_attrbute(os,srq.query_regions(0),ORIENTATION,1.1,0.01);
    volm_vrml_io::display_spherical_region_layer_by_attrbute(os,region_index.index_regions(),ORIENTATION,0.9,0.01);
    os.close();
}

TESTMAIN(test_index_query_matcher);
