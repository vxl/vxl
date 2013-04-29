//:
// \file
// \brief executable to match a given volumetric query and a camera estimate to an indexed reference volume using different matcher
// \author Yi Dong
// \date Jan 19, 2013

#include <volm/volm_io.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <volm/volm_tile.h>
#include <vul/vul_arg.h>
#include <volm/volm_geo_index.h>
#include <volm/volm_geo_index_sptr.h>
#include <volm/volm_query.h>
#include <volm/volm_query_sptr.h>

#include <volm/volm_camera_space.h>
#include <volm/volm_loc_hyp.h>
#include <boxm2/volm/boxm2_volm_wr3db_index.h>
#include <boxm2/volm/boxm2_volm_wr3db_index_sptr.h>
#include <boxm2/volm/boxm2_volm_matcher_p0.h>
#include <bbas/bocl/bocl_manager.h>
#include <bbas/bocl/bocl_device.h>
#include <bkml/bkml_parser.h>
#include <vil/vil_save.h>
#include <bbas/volm/volm_io.h>
#include <bbas/volm/volm_vrml_io.h>


int main(int argc, char** argv)
{
    // input
    vul_arg<vcl_string> cam_kml("-camkml",  "camera KML", "e:/data/Finder/Camera20.kml");                                // query -- camera space binary
    vul_arg<vcl_string> cam_params("-camparams", "camera params", "Z://projects//FINDER//test1//p1a_test1_20///cam_inc_params.txt");                                // query -- camera space binary
    vul_arg<vcl_string> dms_bin("-dms", "depth_map_scene binary", "Z://projects//FINDER//test1//p1a_test1_20//p1a_test1_20.vsl");                             // query -- depth map scene
    vul_arg<vcl_string> sph_bin("-sph", "spherical shell binary", "e:/data/Finder/unit_sphere_2_75_105.vsl");                             // query -- spherical shell container binary
    vul_arg<vcl_string> geo_index_folder(" -geo", " tree structure", "Z:/projects/FINDER/index/geoindex_zone_17_inc_2_nh_100/"); // index -- folder to read the geo_index and hypos for each leaf
    vul_arg<vcl_string> geo_data_folder("-data", "folder to read the index data", "Z:/projects/FINDER/index/old_indices_remove/geoindex_zone_17_inc_2_nh_100_pa_0.5/"); // index -- folder to read the geo_index and hypos for each leaf
    vul_arg<unsigned>   tile_id("-tile", "ID of the tile that current matcher consdier", 3);       // matcher -- tile id
    vul_arg<unsigned>   zone_id("-zone", "ID of the utm zone of current tile",17);                 // matcher -- zone id
    vul_arg<float>      threshold("-thres", "threshold for choosing valid cameras (0~1)", 0.4f);   // matcher -- threshold for choosing cameras
    vul_arg<unsigned>   max_cam_per_loc("-max_cam", "maximum number of cameras to be saved", 200); // matcher -- output related
    vul_arg<vcl_string> out_folder("-out", "output folder where score binary is stored", "e:/data/Finder/20/");      // matcher -- output folder
    vul_arg<bool>       logger("-logger", "designate one of the exes as logger", false);           // matcher -- log file generation
    vul_arg_parse(argc, argv);

    vcl_stringstream log;
    bool do_log = false;
    if (logger())
        do_log = true;
    // check the input parameters
    if ( cam_kml().compare("") == 0 ||
        dms_bin().compare("") == 0 ||
        sph_bin().compare("") == 0 ||
        geo_index_folder().compare("") == 0 ||
        out_folder().compare("") == 0 )
    {
        log << " ERROR: input file/folders can not be empty\n";
        vcl_cerr << log.str();
        vul_arg_display_usage_and_exit();
        return volm_io::EXE_ARGUMENT_ERROR;
    }

    // load geo_index
    vcl_stringstream file_name_pre;
    file_name_pre << geo_index_folder() << "geo_index_tile_" << tile_id();
    vcl_cout << " geo_index_hyps_file = " << file_name_pre.str() + ".txt" << vcl_endl;
    if (!vul_file::exists(file_name_pre.str() + ".txt")) {
        log << " ERROR: gen_index_folder is wrong (missing last slash/ ?), no geo_index_files found in " << geo_index_folder() << vcl_endl;
        if (do_log) { volm_io::write_log(out_folder(), log.str()); }
        vcl_cerr << log.str() << vcl_endl;
        volm_io::write_status(out_folder(), volm_io::GEO_INDEX_FILE_MISSING);
        return volm_io::EXE_ARGUMENT_ERROR;
    }
    float min_size;
    volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
    volm_geo_index::read_hyps(root, file_name_pre.str());
    vcl_vector<volm_geo_index_node_sptr> leaves;
    volm_geo_index::get_leaves_with_hyps(root, leaves);

    // read in the parameter, create depth_interval
    boxm2_volm_wr3db_index_params params;
    vcl_stringstream params_file;
    params_file << "Z:/projects/FINDER/index/geoindex_zone_17_inc_2_nh_100_combined_pa_2/" << "geo_index_tile_" << tile_id() << "_index.params";
    

    if (!params.read_params_file(params_file.str())) {
        log << " ERROR: cannot read params file from " << params_file.str() << '\n';
        if (do_log)  volm_io::write_log(out_folder(), log.str());
        volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
        vcl_cerr << log.str() << vcl_endl;
        return volm_io::EXE_ARGUMENT_ERROR;
    }
    vcl_string index_file = leaves[0]->get_index_name(file_name_pre.str());
    vsl_b_ifstream is(sph_bin());
    if(!is)
    {
        log << " ERROR: cannot read sph file file from " << sph_bin() << '\n';
    }
    vsph_unit_sphere_sptr usph_ptr;
    vsl_b_read(is, usph_ptr);
    is.close();
    // check depth_map_scene binary
    if (!vul_file::exists(dms_bin())) {
        vcl_cerr << " ERROR: depth map scene binary can not be found ---> " << dms_bin() << vcl_endl;
        volm_io::write_status(out_folder(), volm_io::DEPTH_SCENE_FILE_IO_ERROR);
        return volm_io::EXE_ARGUMENT_ERROR;
    }
    depth_map_scene_sptr dms = new depth_map_scene;
    vsl_b_ifstream dis(dms_bin().c_str());
    if (!dis)
        return volm_io::EXE_ARGUMENT_ERROR;
    dms->b_read(dis);
    dis.close();

    volm_io_expt_params camera_params; 
    camera_params.read_params(cam_params());
    // check camera input file
    double heading, heading_dev, tilt, tilt_dev, roll, roll_dev;
    double tfov, top_fov_dev, altitude, lat, lon;
    if (!volm_io::read_camera(cam_kml(), dms->ni(), dms->nj(), heading, heading_dev, 
        tilt, tilt_dev, roll, roll_dev, tfov, top_fov_dev, altitude, lat, lon)) {
            vcl_cout << "problem parsing camera kml file: " << cam_kml()<< '\n';
            return volm_io::EXE_ARGUMENT_ERROR;
    }
    volm_camera_space_sptr  cam_space
        = new volm_camera_space(tfov, top_fov_dev, camera_params.fov_inc, altitude, dms->ni(), dms->nj(),
                                 heading, heading_dev, camera_params.head_inc,
                                 tilt, tilt_dev, camera_params.tilt_inc,
                                 roll, roll_dev, camera_params.roll_inc); 
    volm_spherical_container_sptr sph 
        = new volm_spherical_container(params.solid_angle,params.vmin,params.dmax);
    //: Create query here.

    volm_spherical_region_query srq(dms, cam_space, sph);
    srq.print(vcl_cout);


    vul_timer t;
    t.mark();
    vul_timer a;
    long ttime = 0;
    vcl_vector<volm_score_sptr> score_all;
    unsigned leaf_id = 0;
    boxm2_volm_wr3db_index_sptr ind= new boxm2_volm_wr3db_index(usph_ptr->size(),1);
    boxm2_volm_wr3db_index_sptr ind_orient= new boxm2_volm_wr3db_index(usph_ptr->size(),1);
    boxm2_volm_wr3db_index_sptr ind_land= new boxm2_volm_wr3db_index(usph_ptr->size(),1);
    boxm2_volm_matcher_p0 matcher(cam_space,srq,0.4);

    unsigned test_hyp_id = 0;
    volm_geo_index_node_sptr  test_leaf =  volm_geo_index::get_closest(root,32.650674, -79.948996, test_hyp_id);

    while(leaf_id<leaves.size())
    {
        vcl_cout<<"Leaf " <<leaf_id<<vcl_endl;
        volm_geo_index_node_sptr hyp_leaf = leaves[leaf_id];// volm_geo_index::get_closest(root,32.6507, -79.949, test_hyp_id);
        vcl_string fileprefix = vul_file::strip_directory(file_name_pre.str());
        vcl_string configfile = geo_data_folder()+"/segment/"+fileprefix+"_"+hyp_leaf->get_string()+"_segment.bin";
        vcl_string datafile = geo_data_folder()+"/segment/"+fileprefix+"_"+hyp_leaf->get_string()+"_segment_data.bin";
        vcl_cout<<"Config File "<<configfile<<vcl_endl;
        vcl_cout<<"Data File "<<datafile<<vcl_endl;
        vcl_ifstream iconfig(configfile.c_str(),vcl_ios::binary);
        vcl_ifstream idata(datafile.c_str(),vcl_ios::binary);
        for(unsigned i = 0 ; i < hyp_leaf->hyps_->size(); i++)
        {
            int num_regions[5];
            iconfig.read(reinterpret_cast<char*> (&num_regions[0]),sizeof(int)*5);          
            vcl_cout<<"Vals "<<num_regions[0]<<" "<<num_regions[1]<<" "<<num_regions[2]<<" "<<num_regions[3]<<vcl_endl;
            int numelements = num_regions[0]*6;
            float * boxes = new float[numelements];
            idata.read(reinterpret_cast<char*> (&boxes[0]),sizeof(float)*numelements);
            
            {
                volm_spherical_region_index region_index(boxes,num_regions[1],num_regions[2],num_regions[3],num_regions[4]);
                volm_score_sptr score = new volm_score(leaf_id,i);
                matcher.match(region_index,score);
                score_all.push_back(score);
            }
        } 
        leaf_id++;
    }
    vcl_stringstream out_fname_bin;
    out_fname_bin << out_folder() << "/ps_0_scores_tile_" << tile_id() << ".bin";
    vcl_cout<<"Size of scores "<<score_all.size()<<vcl_endl;
    volm_score::write_scores(score_all, out_fname_bin.str());  
    vcl_cout<<"Total time taken is "<<t.all()<<" seg time "<<ttime<<" # of intersections "<<matcher.count_<<vcl_endl;
    // finish everything successfully
    volm_io::write_status(out_folder(), volm_io::MATCHER_EXE_FINISHED);
    return volm_io::SUCCESS;
}

