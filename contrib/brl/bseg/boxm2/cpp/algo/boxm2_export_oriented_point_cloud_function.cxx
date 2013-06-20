#include "boxm2_export_oriented_point_cloud_function.h"
//:
// \file

#include <vcl_cassert.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vcl_limits.h>

#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>


void boxm2_export_oriented_point_cloud_function::exportPointCloudXYZ(const boxm2_scene_sptr& scene, boxm2_block_metadata data, boxm2_block* blk,
                                                                     boxm2_data_base* alpha, boxm2_data_base* vis, boxm2_data_base* vis_sum, boxm2_data_base* exp,boxm2_data_base* nobs,
                                                                     boxm2_data_base* points, boxm2_data_base* normals,
                                                                     boxm2_data_base* ray_dir_sum, vcl_ofstream& file,
                                                                     bool output_aux, float vis_t, float nmag_t, float prob_t, float exp_t, vgl_box_3d<double> bb)
{
  boxm2_data_traits<BOXM2_ALPHA>::datatype *     alpha_data = (boxm2_data_traits<BOXM2_ALPHA>::datatype*) alpha->data_buffer();
  boxm2_data_traits<BOXM2_POINT>::datatype *     points_data = (boxm2_data_traits<BOXM2_POINT>::datatype*) points->data_buffer();
  boxm2_data_traits<BOXM2_NORMAL>::datatype *    normals_data = (boxm2_data_traits<BOXM2_NORMAL>::datatype*) normals->data_buffer();
  boxm2_data_traits<BOXM2_VIS_SCORE>::datatype * vis_data = (boxm2_data_traits<BOXM2_VIS_SCORE>::datatype*) vis->data_buffer();
  boxm2_data_traits<BOXM2_VIS_SCORE>::datatype * vis_sum_data = (boxm2_data_traits<BOXM2_VIS_SCORE>::datatype*) vis_sum->data_buffer();
  boxm2_data_traits<BOXM2_EXPECTATION>::datatype *  exp_data = (boxm2_data_traits<BOXM2_EXPECTATION>::datatype*) exp->data_buffer();
  boxm2_data_traits<BOXM2_NUM_OBS_SINGLE_INT>::datatype *  nobs_data = (boxm2_data_traits<BOXM2_NUM_OBS_SINGLE_INT>::datatype*) nobs->data_buffer();
  boxm2_data_traits<BOXM2_RAY_DIR>::datatype *  ray_dir_sum_data = (boxm2_data_traits<BOXM2_RAY_DIR>::datatype*) ray_dir_sum->data_buffer();

  file << vcl_fixed;
  int pointTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());
  for (unsigned currIdx=0; currIdx < (points->buffer_length()/pointTypeSize) ; currIdx++) {
    //check normal magnitude and vis score and that point data is valid
    if (normals_data[currIdx][3] >= nmag_t && vis_data[currIdx] >= vis_t && points_data[currIdx][3] != -1)
    {
      float prob;
      if (!calculateProbOfPoint(scene, blk, points_data[currIdx], alpha_data[currIdx], prob))
        continue;

      //check prob
      if (prob >= prob_t && exp_data[currIdx] >= exp_t)
      {
        //check bounding box
        if (bb.is_empty() || bb.contains(points_data[currIdx][0] ,points_data[currIdx][1] ,points_data[currIdx][2]) )
        {
          file <<  points_data[currIdx][0] << ' ' << points_data[currIdx][1] << ' ' << points_data[currIdx][2] << ' '
               <<  normals_data[currIdx][0] << ' ' << normals_data[currIdx][1] << ' ' << normals_data[currIdx][2] << ' ';

          if (output_aux) {
            file  <<  prob  << ' ' <<  normals_data[currIdx][3] << ' ' <<  vis_data[currIdx] << ' ' <<  vis_sum_data[currIdx];
            file << ' ' << ray_dir_sum_data[currIdx][0] << " " << ray_dir_sum_data[currIdx][1] << " " << ray_dir_sum_data[currIdx][2] << " ";
            file << ' ' <<  exp_data[currIdx] << ' ' <<  nobs_data[currIdx];
          }
          file << vcl_endl;
        }
      }
    }
  }
}

void boxm2_export_oriented_point_cloud_function::exportPointCloudPLY(const boxm2_scene_sptr& scene, boxm2_block_metadata data, boxm2_block* blk,
                                            boxm2_data_base* alpha, boxm2_data_base* vis,
                                            boxm2_data_base* points, boxm2_data_base* normals,
                                            vcl_ofstream& file,
                                            bool output_aux, float vis_t, float nmag_t, float prob_t, vgl_box_3d<double> bb, unsigned& num_vertices)
{
  boxm2_data_traits<BOXM2_ALPHA>::datatype *   alpha_data = (boxm2_data_traits<BOXM2_ALPHA>::datatype*) alpha->data_buffer();
  boxm2_data_traits<BOXM2_POINT>::datatype *   points_data = (boxm2_data_traits<BOXM2_POINT>::datatype*) points->data_buffer();
  boxm2_data_traits<BOXM2_NORMAL>::datatype *  normals_data = (boxm2_data_traits<BOXM2_NORMAL>::datatype*) normals->data_buffer();
  boxm2_data_traits<BOXM2_VIS_SCORE>::datatype * vis_data = (boxm2_data_traits<BOXM2_VIS_SCORE>::datatype*) vis->data_buffer();

  file << vcl_fixed;
  int pointTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());
  for (unsigned currIdx=0; currIdx < (points->buffer_length()/pointTypeSize) ; currIdx++) {
    //check normal magnitude and vis score and that point data is valid
    if (normals_data[currIdx][3] >= nmag_t && vis_data[currIdx] >= vis_t && points_data[currIdx][3] != -1)
    {
      float prob;
      if (!calculateProbOfPoint(scene, blk, points_data[currIdx], alpha_data[currIdx], prob))
        continue;

      //check prob
      if (prob >= prob_t )
      {
        //check bounding box
        if (bb.is_empty() || bb.contains(points_data[currIdx][0] ,points_data[currIdx][1] ,points_data[currIdx][2]) )
        {
          file <<  points_data[currIdx][0] << ' ' << points_data[currIdx][1] << ' ' << points_data[currIdx][2];
          num_vertices++;

          if (output_aux){
            file << ' ' << normals_data[currIdx][0] << ' ' << normals_data[currIdx][1] << ' ' << normals_data[currIdx][2] << ' ' << prob  << ' ' << vis_data[currIdx];
          }
          file << vcl_endl;
        }
      }
    }
  }
}

void boxm2_export_oriented_point_cloud_function::exportPointCloudPLY(const boxm2_scene_sptr& scene, boxm2_block_metadata data, boxm2_block* blk,
                                                                     boxm2_data_base* mog, boxm2_data_base* alpha,
                                                                     boxm2_data_base* points, boxm2_data_base* covariances, vcl_ofstream& file,
                                                                     float prob_t, vgl_box_3d<double> bb, unsigned& num_vertices, bool color_using_model)
{
  boxm2_data_traits<BOXM2_POINT>::datatype *   points_data = (boxm2_data_traits<BOXM2_POINT>::datatype*) points->data_buffer();
  boxm2_data_traits<BOXM2_COVARIANCE>::datatype *  covs_data = (boxm2_data_traits<BOXM2_COVARIANCE>::datatype*) covariances->data_buffer();
  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype * mog_data = (boxm2_data_traits<BOXM2_MOG3_GREY>::datatype*) mog->data_buffer();

  file << vcl_fixed;
  int pointTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());
  for (unsigned currIdx=0; currIdx < (points->buffer_length()/pointTypeSize) ; currIdx++) {
    //check if the point data is valid
    //if (covs_data[currIdx][0] >= 0.0)
    //{
      float prob = 0.0f;
      vnl_vector_fixed<double, 3> axes;
      //vnl_vector_fixed<double, 3> eval;
      double LE, CE, exp_color;
      if (!calculateProbOfPoint(scene, blk, points_data[currIdx], covs_data[currIdx], mog, alpha, prob, exp_color, axes, LE, CE))
        continue;

      if (prob >= prob_t)
      {
        //check bounding box
        if (bb.is_empty() || bb.contains(points_data[currIdx][0] ,points_data[currIdx][1] ,points_data[currIdx][2]) )
        {
          exp_color = boxm2_data_traits<BOXM2_MOG3_GREY>::processor::expected_color(mog_data[currIdx]);
          file <<  points_data[currIdx][0] << ' ' << points_data[currIdx][1] << ' ' << points_data[currIdx][2] << ' ';
          int col = (int)(exp_color*255);
          col = col > 255 ? 255 : col;
          file << col << ' ' << col << ' ' << col << ' '
               << axes[0] << ' ' << axes[1] << ' ' << axes[2] << ' '
        //     << eval[0] << ' ' << eval[1] << ' ' << eval[2] << ' '
               << LE << ' ' << CE << ' ';
          num_vertices++;
          file  <<  prob << vcl_endl;
        }
      }
    //}
  }
}

bool boxm2_export_oriented_point_cloud_function::calculateProbOfPoint(const boxm2_scene_sptr& scene, boxm2_block * blk,
                                                                      const vnl_vector_fixed<float, 4>& point,
                                                                      const vnl_vector_fixed<float, 9>& cov,
                                                                      boxm2_data_base* mog,
                                                                      boxm2_data_base* alpha,
                                                                      float& prob, double& color, vnl_vector_fixed<double, 3>& axes, double& LE, double& CE)
{
  vgl_point_3d<double> local;
  boxm2_block_id id;
  vgl_point_3d<double> vgl_point(point[0],point[1],point[2]);
  //if the scene doesn't contain point,
  if (!scene->contains(vgl_point, id, local)) {
    //vcl_cout << "Point " << vgl_point << " not present in scene! Skipping..." << vcl_endl;
    return false;
  }
  //if the block passed isn't the block that contains the point, there is something wrong...
  //this happens when the point data is empty (0,0,0,0) for instance, or simply wrong.
  //if (blk->block_id() != id)
  //  return false;

  prob = 1.0;
  // compute the eigenvalues
  vnl_matrix<double> pt_cov(3,3,0.0);
  pt_cov[0][0] = cov[0];
  pt_cov[0][1] = cov[1];
  pt_cov[0][2] = cov[2];

  pt_cov[1][0] = cov[3];
  pt_cov[1][1] = cov[4];
  pt_cov[1][2] = cov[5];

  pt_cov[2][0] = cov[6];
  pt_cov[2][1] = cov[7];
  pt_cov[2][2] = cov[8];

  vnl_matrix<double> V(3,3,0.0); vnl_vector<double> eigs(3);
  if (!vnl_symmetric_eigensystem_compute(pt_cov, V, eigs))
    return false;

  // place from the longest axis to the shortest, largest eigen value is in eigs[2]
  axes[0] =2*vcl_sqrt(eigs[2])*2.5;  // to find 90% confidence ellipsoid, scale the eigenvalues, see pg. 416 on Intro To Modern Photogrammetry, Mikhail, et. al.
  axes[1] =2*vcl_sqrt(eigs[1])*2.5;
  axes[2] =2*vcl_sqrt(eigs[0])*2.5;
  // check if values are valid (AND is the only way to detect invalid value, do not change into ORs
  if (!(axes[0] < vcl_numeric_limits<double>::max() && axes[0] > vcl_numeric_limits<double>::min() &&
        axes[1] < vcl_numeric_limits<double>::max() && axes[1] > vcl_numeric_limits<double>::min() &&
        axes[2] < vcl_numeric_limits<double>::max() && axes[2] > vcl_numeric_limits<double>::min()))
    return false;

  // now find LE (vertical error) using the eigenvector that corresponds to major axis
  vnl_vector<double> major = V.get_column(2);

  // create the vector that corresponds to error ellipsoid
  vnl_vector<double> major_ellipsoid = axes[0]*major;


  LE = vcl_abs(major_ellipsoid.get(2));
  double CEx = vcl_abs(major_ellipsoid.get(0));
  double CEy = vcl_abs(major_ellipsoid.get(1));

  CE = CEx > CEy ? CEx : CEy;

  if (LE > 2.5)
    return false;
  if (CE > 2.5)
    return false;

  return true;
}


bool boxm2_export_oriented_point_cloud_function::calculateProbOfPoint(const boxm2_scene_sptr& scene, boxm2_block * blk,
                                                                      const vnl_vector_fixed<float, 4>& point,
                                                                      const float& alpha, float& prob)
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
  if (blk->block_id() != id)
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

void boxm2_export_oriented_point_cloud_function::writePLYHeader(vcl_ofstream& file, unsigned num_vertices,vcl_stringstream& ss, bool output_aux)
{
   file << "ply\nformat ascii 1.0\nelement vertex " << num_vertices;
   file << "\nproperty float32 x\nproperty float32 y\nproperty float32 z";
   if (output_aux) {
     file << "\nproperty float x\nproperty float y\nproperty float z\nproperty float nx\nproperty float ny\nproperty float nz\n" << "property float prob\nproperty float vis_score\n";
   }
   file << "\nend_header\n"
        << ss.str();
}

void boxm2_export_oriented_point_cloud_function::writePLYHeaderOnlyPoints(vcl_ofstream& file, unsigned num_vertices, vcl_stringstream& ss)
{
  file << "ply\nformat ascii 1.0\nelement vertex " << num_vertices
       << "\nproperty float x\nproperty float y\nproperty float z\nproperty uchar red\nproperty uchar green\nproperty uchar blue\n"
       << "property float axes_a\nproperty float axes_b\nproperty float axes_c\n"
    // << "property float eval_x\nproperty float eval_y\nproperty eval_z\n"
       << "property float LE\nproperty float CE\n"
       << "property float prob\n"
       << "end_header\n"
       << ss.str();
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
int boxm2_plyio_vertex_cb(p_ply_argument argument)
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
      // INSERT VERTEX INTO THE MESH
      parsed_ply->bbox.add(vgl_point_3d<double>(parsed_ply->p));
      break;
    default:
      assert(!"This should not happen: index out of range");
  }
  return 1;
}


void boxm2_export_oriented_point_cloud_function::readBBFromPLY(const vcl_string& filename, vgl_box_3d<double>& box)
{
  ply_bb_reader parsed_ply;
  parsed_ply.bbox = box;

  p_ply ply = ply_open(filename.c_str(), NULL, 0, NULL);
  if (!ply) {
    vcl_cout << "File " << filename << " doesn't exist.";
  }
  if (!ply_read_header(ply))
    vcl_cout << "File " << filename << " doesn't have header.";

  // vertex
  ply_set_read_cb(ply, "vertex", "x", boxm2_plyio_vertex_cb, (void*) (&parsed_ply), 0);
  ply_set_read_cb(ply, "vertex", "y", boxm2_plyio_vertex_cb, (void*) (&parsed_ply), 1);
  ply_set_read_cb(ply, "vertex", "z", boxm2_plyio_vertex_cb, (void*) (&parsed_ply), 2);

  // Read DATA
  ply_read(ply);

  // CLOSE file
  ply_close(ply);

  box=parsed_ply.bbox;
}

