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
#include <bkml/bkml_parser.h>
#include <bbas/volm/volm_io.h>
#include <bbas/volm/volm_vrml_io.h>

int main(int argc, char** argv)
{
    // input
    vul_arg<vcl_string> sph_bin("-sph", "spherical shell binary", "e:/data/Finder/unit_sphere_0.5_75_105.vsl");                             // query -- spherical shell container binary
    vul_arg<vcl_string> geo_index_folder("-geo", " tree structure", "Z:/projects/FINDER/index/geoindex_zone_17_inc_2_nh_100/"); // index -- folder to read the geo_index and hypos for each leaf
    vul_arg<vcl_string> geo_data_folder("-data", "folder to read the index data", "e:/data/Finder/temp/");//Z:/projects/FINDER/index/old_indices_remove/geoindex_zone_17_inc_2_nh_100_pa_0.5/"); // index -- folder to read the geo_index and hypos for each leaf
    vul_arg<unsigned>   tile_id("-tile", "ID of the tile that current matcher consdier", 3);       // matcher -- tile id
    vul_arg<unsigned>   zone_id("-zone", "ID of the utm zone of current tile",17);                 // matcher -- zone id
    vul_arg<float>      threshold("-thres", "threshold for choosing valid cameras (0~1)", 0.4f);   // matcher -- threshold for choosing cameras
    vul_arg<vcl_string> out_folder("-out", "output folder where score binary is stored", "Z://projects//FINDER//index//old_indices_remove/geoindex_zone_17_inc_2_nh_100_pa_0.5/segment/");      // matcher -- output folder
    vul_arg<bool>       logger("-logger", "designate one of the exes as logger", false);           // matcher -- log file generation
    vul_arg_parse(argc, argv);

    vcl_stringstream log;
    bool do_log = false;
    if (logger())
        do_log = true;
    // check the input parameters
    if ( sph_bin().compare("") == 0 ||
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
        log << " ERROR: gen_index_folder is wrong (missing last slash/ ?), no geo_index_files found in " << geo_index_folder() << '\n';
        if (do_log) { volm_io::write_log(out_folder(), log.str()); }
        vcl_cerr << log.str();
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

    vcl_string index_file = leaves[0]->get_index_name(file_name_pre.str());

#if 0
    if (!params.read_params_file(index_file)) {
        log << " ERROR: cannot read params file from " << index_file << '\n';
        if (do_log)  volm_io::write_log(out_folder(), log.str());
        volm_io::write_status(out_folder(), volm_io::EXE_ARGUMENT_ERROR);
        vcl_cerr << log.str();
        return volm_io::EXE_ARGUMENT_ERROR;
    }
#endif // 0
    vsl_b_ifstream is(sph_bin());
    if (!is)
    {
        log << " ERROR: cannot read sph file file from " << sph_bin() << '\n';
    }
    vsph_unit_sphere_sptr usph_ptr;
    vsl_b_read(is, usph_ptr);
    is.close();

    if (!vul_file::is_directory(out_folder()))
        vul_file::make_directory(out_folder());

    unsigned int test_hyp_id;
    vul_timer t;
    t.mark();

    unsigned leaf_id = 0;
    boxm2_volm_wr3db_index_sptr ind= new boxm2_volm_wr3db_index(usph_ptr->size(),1);
    boxm2_volm_wr3db_index_sptr ind_orient= new boxm2_volm_wr3db_index(usph_ptr->size(),1);
    boxm2_volm_wr3db_index_sptr ind_land= new boxm2_volm_wr3db_index(usph_ptr->size(),1);

    vcl_stringstream  data_file_name_pre;
    data_file_name_pre<<geo_data_folder()<<"/geo_index_tile_" << tile_id();
    test_hyp_id = 0;
    volm_geo_index_node_sptr  test_leaf =   volm_geo_index::get_closest(root,32.650674, -79.948996, test_hyp_id);

    //while (leaf_id <  leaves.size())
    {
        vcl_cout<<"Leaf " <<leaf_id<<vcl_endl;
        volm_geo_index_node_sptr hyp_leaf =  test_leaf;//leaves[leaf_id];//volm_geo_index::get_closest(root,32.6607, -79.9264, test_hyp_id); //
        vcl_string index_depth_file = hyp_leaf->get_index_name(data_file_name_pre.str());
        vcl_string index_orient_file = hyp_leaf->get_label_index_name(data_file_name_pre.str(), "orientation");
        vcl_string index_land_file = hyp_leaf->get_label_index_name(data_file_name_pre.str(), "");

        vcl_string outfileprefix = vul_file::strip_directory(data_file_name_pre.str());
        vcl_string outconfig = out_folder()+"/"+outfileprefix+"_"+hyp_leaf->get_string()+"_segment.bin";
        vcl_string outdata = out_folder()+"/"+outfileprefix+"_"+hyp_leaf->get_string()+"_segment_data.bin";
        // start loop over all indices
        vcl_cout<<index_depth_file<<'\n'
                <<index_orient_file<<'\n'
                <<"Out: "<<outconfig<<'\n'
                <<"Out data: "<<outdata<<vcl_endl;
        vcl_ofstream oconfig(outconfig.c_str(),vcl_ios::binary);
        vcl_ofstream odata(outdata.c_str(),vcl_ios::binary);

        if (!oconfig || ! odata)
        {
            vcl_cout<<"Cannot open output file"<<vcl_endl;
            return volm_io::CAM_FILE_IO_ERROR;
        }
        ind->initialize_read(index_depth_file);
        ind_orient->initialize_read(index_orient_file);
        ind_land->initialize_read(index_land_file);

        vcl_vector<unsigned char> index_values;
        vcl_vector<unsigned char> index_orientation_values;
        vcl_vector<unsigned char> index_label_values;

        for (unsigned i = 0 ; i < hyp_leaf->hyps_->size(); i++)
        {
            ind->get_next(index_values);
            ind_orient->get_next(index_orientation_values);
            ind_land->get_next(index_label_values);
            vcl_cout<<"Size is "<<index_values.size()<<' '
                    <<index_orientation_values.size()<<' '
                    <<index_label_values.size()<<vcl_endl;

            vcl_map<vcl_string,vcl_vector<unsigned char> > index_buffers ;
            //index_buffers["ORIENTATION"] = index_orientation_values;
            //index_buffers["DEPTH_INTERVAL"] = index_values;
            index_buffers["NLCD"] = index_label_values;
            if (i == test_hyp_id)
            {
                double rad = 1.0;
                vgl_point_3d<float> cent(0.0,0.0,0.0);
                vgl_point_3d<double> cent_ray(0.0,0.0,0.0);
                vgl_vector_3d<double> axis_x(1.0, 0.0, 0.0);
                vgl_vector_3d<double> axis_y(0.0, 1.0, 0.0);
                vgl_vector_3d<double> axis_z(0.0, 0.0, 1.0);
                vcl_ofstream os("e:/data/Finder/matcher.wrl");
                if (!os.is_open())
                    return -1;
                bvrml_write::write_vrml_header(os);
                vgl_sphere_3d<float> sp((float)0.0, (float)0.0, (float)0.0, (float)1.0);
                vgl_sphere_3d<float> sp2((float)cent.x(), (float)cent.y()+2, (float)cent.z(), (float)rad/10);
#if 0
                bvrml_write::write_vrml_sphere(os, sp, 0.5f, 0.5f, 0.5f, 0.0f);
                bvrml_write::write_vrml_line(os, cent_ray, axis_x, (float)rad*2, 1.0f, 0.0f, 0.0f);
                bvrml_write::write_vrml_line(os, cent_ray, axis_y, (float)rad*2, 0.0f, 1.0f, 0.0f);
                bvrml_write::write_vrml_line(os, cent_ray, axis_z, (float)rad*2, 0.0f, 1.0f, 1.0f);
                bvrml_write::write_vrml_sphere(os, sp2, 0.0f, 0.0f, 1.0f, 0.0f);
#endif
                volm_spherical_region_index region_index(index_buffers,usph_ptr);
                volm_vrml_io::display_spherical_region_layer_by_attrbute(os,region_index.index_regions(),NLCD,1.05,0.01);
                volm_vrml_io::display_spherical_region_layer_by_attrbute(os,region_index.index_regions(),SKY,1.05,0.01);
                volm_vrml_io::display_segment_sphere(os,region_index.seg(),0.9);
                os.close();
                region_index.write_binary(oconfig,odata);
            }
        }

        oconfig.close();
        odata.close();

        ind->finalize();
        ind_orient->finalize();
        ind_land->finalize();
        leaf_id++;
    }
    vcl_cout<<"DONE"<<vcl_endl;
    return volm_io::SUCCESS;
}
