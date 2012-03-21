#ifndef boxm2_export_point_cloud_xyz_function_txx
#define boxm2_export_point_cloud_xyz_function_txx

#include "boxm2_export_point_cloud_xyz_function.h"

//:
// \file
#if 0
query_point(boxm2_scene_sptr& scene,
                             boxm2_cache_sptr& cache,
                             const vgl_point_3d<double>& point,
                             float& prob, float& intensity)
#endif

void boxm2_export_point_cloud_xyz_function::exportPointCloudXYZ(const boxm2_scene_sptr& scene, boxm2_block_metadata data, boxm2_block* blk,
                                                                boxm2_data_base* alpha, boxm2_data_base* vis,
                                                                boxm2_data_base* points, boxm2_data_base* normals,
                                                                vcl_ofstream& file,
                                                                bool output_aux, float vis_t, float nmag_t, float prob_t, vgl_box_3d<double> bb)
{
    boxm2_data_traits<BOXM2_ALPHA>::datatype *   alpha_data = (boxm2_data_traits<BOXM2_ALPHA>::datatype*) alpha->data_buffer();
    boxm2_data_traits<BOXM2_POINT>::datatype *   points_data = (boxm2_data_traits<BOXM2_POINT>::datatype*) points->data_buffer();
    boxm2_data_traits<BOXM2_NORMAL>::datatype *  normals_data = (boxm2_data_traits<BOXM2_NORMAL>::datatype*) normals->data_buffer();
    boxm2_data_traits<BOXM2_VIS_SCORE>::datatype *    vis_data = (boxm2_data_traits<BOXM2_VIS_SCORE>::datatype*) vis->data_buffer();

    file << vcl_fixed;
    int pointTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());
    for (unsigned currIdx=0; currIdx < (points->buffer_length()/pointTypeSize) ; currIdx++) {
      //check normal magnitude and vis score and that point data is valid
      if (normals_data[currIdx][3] >= nmag_t && vis_data[currIdx] >= vis_t && points_data[currIdx][3] != -1)
      {
        float prob;
        if(!calculateProbOfPoint(scene, blk, points_data[currIdx], alpha_data[currIdx], prob))
          continue;

        //check prob
        if(prob >= prob_t)
        {
          //check bounding box
          if(bb.is_empty() || bb.contains(points_data[currIdx][0] ,points_data[currIdx][1] ,points_data[currIdx][2]) )
          {
            file <<  points_data[currIdx][0] << ' ' << points_data[currIdx][1] << ' ' << points_data[currIdx][2] << ' ';
            file <<  normals_data[currIdx][0] << ' ' << normals_data[currIdx][1] << ' ' << normals_data[currIdx][2] << ' ';

            if (output_aux)
               file  <<  prob << ' ' <<  vis_data[currIdx] << ' ' <<  normals_data[currIdx][3];
            file << vcl_endl;
          }
        }
      }

    }
}

void boxm2_export_point_cloud_xyz_function::exportPointCloudPLY(const boxm2_scene_sptr& scene, boxm2_block_metadata data, boxm2_block* blk,
                                                                boxm2_data_base* alpha, boxm2_data_base* vis,
                                                                boxm2_data_base* points, boxm2_data_base* normals,
                                                                vcl_ofstream& file, bool output_aux, float vis_t, float nmag_t, float prob_t,
                                                                vgl_box_3d<double> bb, unsigned& num_vertices)
{
  boxm2_data_traits<BOXM2_ALPHA>::datatype *   alpha_data = (boxm2_data_traits<BOXM2_ALPHA>::datatype*) alpha->data_buffer();
  boxm2_data_traits<BOXM2_POINT>::datatype *   points_data = (boxm2_data_traits<BOXM2_POINT>::datatype*) points->data_buffer();
  boxm2_data_traits<BOXM2_NORMAL>::datatype *  normals_data = (boxm2_data_traits<BOXM2_NORMAL>::datatype*) normals->data_buffer();
  boxm2_data_traits<BOXM2_VIS_SCORE>::datatype *    vis_data = (boxm2_data_traits<BOXM2_VIS_SCORE>::datatype*) vis->data_buffer();

  file << vcl_fixed;
  int pointTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());
  for (unsigned currIdx=0; currIdx < (points->buffer_length()/pointTypeSize) ; currIdx++) {
    //check normal magnitude and vis score and that point data is valid
    if (normals_data[currIdx][3] >= nmag_t && vis_data[currIdx] >= vis_t && points_data[currIdx][3] != -1)
    {
      float prob;
      if(!calculateProbOfPoint(scene, blk, points_data[currIdx], alpha_data[currIdx], prob))
        continue;

      //check prob
      if(prob >= prob_t)
      {
        //check bounding box
        if(bb.is_empty() || bb.contains(points_data[currIdx][0] ,points_data[currIdx][1] ,points_data[currIdx][2]) )
        {
          file <<  points_data[currIdx][0] << ' ' << points_data[currIdx][1] << ' ' << points_data[currIdx][2] << ' ';
          file <<  normals_data[currIdx][0] << ' ' << normals_data[currIdx][1] << ' ' << normals_data[currIdx][2] << ' ';
          num_vertices++;

          if (output_aux)
             file  <<  prob << ' ' <<  vis_data[currIdx] << ' ' <<  normals_data[currIdx][3];
          file << vcl_endl;
        }
      }
    }

  }
}


bool boxm2_export_point_cloud_xyz_function::calculateProbOfPoint(const boxm2_scene_sptr& scene, boxm2_block * blk,
                                                    const vnl_vector_fixed<float, 4>& point, const float& alpha, float& prob)
{
  vgl_point_3d<double> local;
  boxm2_block_id id;
  vgl_point_3d<double> vgl_point(point[0],point[1],point[2]);
  //if the scene doesn't contain point,
  if (!scene->contains(vgl_point, id, local)) {
    vcl_cout << "Point " << vgl_point << " not present in scene! Skipping..." << vcl_endl;
    return false;
  }
  //if the block passed isn't the block that contains the point, there is something wrong...
  //this happens when the point data is empty (0,0,0,0) for instance, or simply wrong.
  if(blk->block_id() != id)
    return false;

  int index_x=(int)vcl_floor(local.x());
  int index_y=(int)vcl_floor(local.y());
  int index_z=(int)vcl_floor(local.z());

  boxm2_block_metadata mdata = scene->get_block_metadata_const(id);
  vnl_vector_fixed<unsigned char,16> treebits=blk->trees()(index_x,index_y,index_z);
  boct_bit_tree tree(treebits.data_block(),mdata.max_level_);
  int bit_index=tree.traverse(local);
  int depth=tree.depth_at(bit_index);
  float side_len=static_cast<float>(mdata.sub_block_dim_.x()/((float)(1<<depth)));

  prob=1.0f-vcl_exp(-alpha*side_len);
  return true;
}


void boxm2_export_point_cloud_xyz_function::writePLYHeader(vcl_ofstream& file, unsigned num_vertices,vcl_stringstream& ss, bool output_aux)
{
   file << "ply\nformat ascii 1.0\nelement vertex " << num_vertices
        << "\nproperty float x\nproperty float y\nproperty float z\nproperty float nx\nproperty float ny\nproperty float nz\n";
   if(output_aux) {
     file << "property float prob\nproperty float vis_score\nproperty float nmag\n";
   }

   file << "end_header\n";
   file  << ss.str();
}


//helper class to read in bb from file
class ply_bb_reader
{
public:
  vgl_box_3d<double> bbox;
  double p[3];
  vcl_vector<int > vertex_indices;
};

//: Call-back function for a "vertex" element
int bof_plyio_vertex_cb_(p_ply_argument argument)
{
  long index;
  void* temp;
  ply_get_argument_user_data(argument, &temp, &index);

  ply_bb_reader* parsed_ply =  (ply_bb_reader*) temp;

  switch (index)
  {
    case 0: // "x" coordinate
      parsed_ply->p[0] = ply_get_argument_value(argument);
      break;
    case 1: // "y" coordinate
      parsed_ply->p[1] = ply_get_argument_value(argument);
      break;
    case 2: // "z" coordinate
      parsed_ply->p[2] = ply_get_argument_value(argument);
    { // INSERT VERTEX INTO THE MESH
      parsed_ply->bbox.add(vgl_point_3d<double>(parsed_ply->p));
      break;
    }
    default:
      assert(!"This should not happen: index out of range");
  };
  return 1;
}




void boxm2_export_point_cloud_xyz_function::readBBFromPLY(const vcl_string& filename, vgl_box_3d<double>& box) {

  ply_bb_reader parsed_ply;
  parsed_ply.bbox = box;

  p_ply ply = ply_open(filename.c_str(), NULL, 0, NULL);
  if (!ply) {
    vcl_cout << "File " << filename << " doesn't exist.";
  }
  if (!ply_read_header(ply))
    vcl_cout << "File " << filename << " doesn't have header.";

  // vertex
  ply_set_read_cb(ply, "vertex", "x", bof_plyio_vertex_cb_, (void*) (&parsed_ply), 0);
  ply_set_read_cb(ply, "vertex", "y", bof_plyio_vertex_cb_, (void*) (&parsed_ply), 1);
  ply_set_read_cb(ply, "vertex", "z", bof_plyio_vertex_cb_, (void*) (&parsed_ply), 2);

  // Read DATA
  ply_read(ply);

  // CLOSE file
  ply_close(ply);

  box=parsed_ply.bbox;
}



#endif
