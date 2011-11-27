#include "boxm2_export_point_cloud_xyz_function.h"
#include <vgl/vgl_sphere_3d.h>

//:
// \file

//#define DEBUG

void boxm2_export_point_cloud_xyz_function::exportPointCloudXYZ(boxm2_block_metadata data, boxm2_block* blk, boxm2_data_base* vis,
															 boxm2_data_base* points, boxm2_data_base* normals, vcl_ofstream& file,
															 bool output_prob,  bool output_vis, float vis_t)
{
    boxm2_data_traits<BOXM2_POINT>::datatype *   points_data = (boxm2_data_traits<BOXM2_POINT>::datatype*) points->data_buffer();
    boxm2_data_traits<BOXM2_NORMAL>::datatype *  normals_data = (boxm2_data_traits<BOXM2_NORMAL>::datatype*) normals->data_buffer();
    boxm2_data_traits<BOXM2_AUX0>::datatype *    vis_data = (boxm2_data_traits<BOXM2_AUX0>::datatype*) vis->data_buffer();

    int pointTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());
    for (unsigned currIdx=0; currIdx < (points->buffer_length()/pointTypeSize) ; currIdx++) {
     if( !(points_data[currIdx][3] == 0)) { //check probability to decide whether to output or not.
    	 if(vis_data[currIdx] >= vis_t) {
			 file <<  points_data[currIdx][0] << " " << points_data[currIdx][1] << " " << points_data[currIdx][2] << " ";
			 file <<  normals_data[currIdx][0] << " " << normals_data[currIdx][1] << " " << normals_data[currIdx][2];
			 if(output_prob)
				 file << " " <<  points_data[currIdx][3];
			 if(output_vis)
				 file << " " <<  vis_data[currIdx];
			 file << vcl_endl;
    	 }
      }
    }
}

void boxm2_export_point_cloud_xyz_function::exportPointCloudPLY(boxm2_block_metadata data, boxm2_block* blk, boxm2_data_base* vis,
															 boxm2_data_base* points, boxm2_data_base* normals, vcl_ofstream& file,
															 bool output_prob,  bool output_vis, float vis_t, unsigned& num_vertices)
{
    boxm2_data_traits<BOXM2_POINT>::datatype *   points_data = (boxm2_data_traits<BOXM2_POINT>::datatype*) points->data_buffer();
    boxm2_data_traits<BOXM2_NORMAL>::datatype *  normals_data = (boxm2_data_traits<BOXM2_NORMAL>::datatype*) normals->data_buffer();
    boxm2_data_traits<BOXM2_AUX0>::datatype *    vis_data = (boxm2_data_traits<BOXM2_AUX0>::datatype*) vis->data_buffer();

    int pointTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());
    for (unsigned currIdx=0; currIdx < (points->buffer_length()/pointTypeSize) ; currIdx++) {
     if( !(points_data[currIdx][3] == 0)) { //check probability to decide whether to output or not.
    	 if(vis_data[currIdx] >= vis_t) {
			 file <<  points_data[currIdx][0] << " " << points_data[currIdx][1] << " " << points_data[currIdx][2] << " ";
			 file <<  normals_data[currIdx][0] << " " << normals_data[currIdx][1] << " " << normals_data[currIdx][2];
			 num_vertices++;
			 if(output_prob)
				 file << " " <<  points_data[currIdx][3];
			 if(output_vis)
				 file << " " <<  vis_data[currIdx];
			 file << vcl_endl;
    	 }
      }
    }
}

void boxm2_export_point_cloud_xyz_function::writePLYHeader(vcl_ofstream& file, unsigned num_vertices,vcl_stringstream& ss)
{
   file << "ply\nformat ascii 1.0\nelement vertex ";
   file << num_vertices << "\n";
   file << "property float x\nproperty float y\nproperty float z\nproperty float nx\n";
   file << "property float ny\nproperty float nz\nproperty float prob\nproperty float vis\nend_header\n";
   file << ss.str();
}

