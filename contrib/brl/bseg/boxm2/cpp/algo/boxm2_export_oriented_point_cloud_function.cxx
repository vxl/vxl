#include <iostream>
#include <limits>
#include <vnl/vnl_math.h>
#include "boxm2_export_oriented_point_cloud_function.h"
//:
// \file

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/algo/vnl_symmetric_eigensystem.h>

#include <rply.h>   //.ply parser

#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>


void boxm2_export_oriented_point_cloud_function::exportPointCloudXYZ(const boxm2_scene_sptr& scene, const boxm2_block_metadata&  /*data*/, boxm2_block* blk,
                                                                     boxm2_data_base* alpha, boxm2_data_base* vis, boxm2_data_base* vis_sum, boxm2_data_base* exp,boxm2_data_base* nobs,
                                                                     boxm2_data_base* points, boxm2_data_base* normals,
                                                                     boxm2_data_base* ray_dir_sum, std::ofstream& file,
                                                                     bool output_aux, float vis_t, float nmag_t, float prob_t, float exp_t, vgl_box_3d<double> bb)
{
  auto *     alpha_data = (boxm2_data_traits<BOXM2_ALPHA>::datatype*) alpha->data_buffer();
  auto *     points_data = (boxm2_data_traits<BOXM2_POINT>::datatype*) points->data_buffer();
  auto *    normals_data = (boxm2_data_traits<BOXM2_NORMAL>::datatype*) normals->data_buffer();
  auto * vis_data = (boxm2_data_traits<BOXM2_VIS_SCORE>::datatype*) vis->data_buffer();
  auto * vis_sum_data = (boxm2_data_traits<BOXM2_VIS_SCORE>::datatype*) vis_sum->data_buffer();
  auto *  exp_data = (boxm2_data_traits<BOXM2_EXPECTATION>::datatype*) exp->data_buffer();
  auto *  nobs_data = (boxm2_data_traits<BOXM2_NUM_OBS_SINGLE_INT>::datatype*) nobs->data_buffer();
  auto *  ray_dir_sum_data = (boxm2_data_traits<BOXM2_RAY_DIR>::datatype*) ray_dir_sum->data_buffer();

  file << std::fixed;
  int pointTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());
  //check for invalid parameters
  if( pointTypeSize == 0 ) //This should never happen, it will result in division by zero later
  {
    std::cerr << "ERROR: Division by zero in " << __FILE__ << __LINE__ << std::endl;
    throw 0;
  }

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
          file << std::endl;
        }
      }
    }
  }
}

void boxm2_export_oriented_point_cloud_function::exportPointCloudPLY(const boxm2_scene_sptr& scene, const boxm2_block_metadata&  /*data*/, boxm2_block* blk,
                                            boxm2_data_base* alpha, boxm2_data_base* vis,
                                            boxm2_data_base* points, boxm2_data_base* normals,
                                            std::ofstream& file,
                                            bool output_aux, float vis_t, float nmag_t, float prob_t, vgl_box_3d<double> bb, unsigned& num_vertices)
{
  auto *   alpha_data = (boxm2_data_traits<BOXM2_ALPHA>::datatype*) alpha->data_buffer();
  auto *   points_data = (boxm2_data_traits<BOXM2_POINT>::datatype*) points->data_buffer();
  auto *  normals_data = (boxm2_data_traits<BOXM2_NORMAL>::datatype*) normals->data_buffer();
  auto * vis_data = (boxm2_data_traits<BOXM2_VIS_SCORE>::datatype*) vis->data_buffer();

  file << std::fixed;
  int pointTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());
  //check for invalid parameters
  if( pointTypeSize == 0 ) //This should never happen, it will result in division by zero later
  {
    std::cerr << "ERROR: Division by 0 in " << __FILE__ << __LINE__ << std::endl;
    throw 0;
  }

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
            file << ' ' << normals_data[currIdx][0] << ' ' << normals_data[currIdx][1] << ' ' << normals_data[currIdx][2] << ' ' << prob  << ' ' << vis_data[currIdx] << ' ' << normals_data[currIdx][3];
          }
          file << std::endl;
        }
      }
    }
  }
}

void boxm2_export_oriented_point_cloud_function::exportPointCloudPLY(const boxm2_scene_sptr&  /*scene*/, const boxm2_block_metadata&  /*data*/, boxm2_block*  /*blk*/,
                                                                     boxm2_data_base* mog, boxm2_data_base* alpha,
                                                                     boxm2_data_base* points, boxm2_data_base* covariances, std::ofstream& file,
                                                                     float prob_t, float LE_t, float CE_t, vgl_box_3d<double> bb, unsigned& num_vertices, const std::string& datatype)
{
  auto *   points_data = (boxm2_data_traits<BOXM2_POINT>::datatype*) points->data_buffer();
  auto *  covs_data = (boxm2_data_traits<BOXM2_COVARIANCE>::datatype*) covariances->data_buffer();
  auto     *   alpha_data   = (boxm2_data_traits<BOXM2_ALPHA>::datatype*) alpha->data_buffer();

  file << std::fixed;
  int pointTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());
  //check for invalid parameters
  if( pointTypeSize == 0 ) //This should never happen, it will result in division by zero later
  {
    std::cerr << "ERROR: Division by 0 in " << __FILE__ << __LINE__ << std::endl;
    throw 0;
  }

  for (unsigned currIdx=0; currIdx < (points->buffer_length()/pointTypeSize) ; currIdx++) {
    //check if the point data is valid
    //if (covs_data[currIdx][0] >= 0.0)
    //{

      //check bounding box
      if (bb.is_empty() || bb.contains(points_data[currIdx][0] ,points_data[currIdx][1] ,points_data[currIdx][2]) )
      {

        //float prob = 0.0f;
        float prob = points_data[currIdx][3]; // during extractPointCloud process the prob of this point should be saved in this field
                                              // that process actually also checks whether the prob is below threshold and marks this field as -1.0

        if (prob >= prob_t)
        {

          vnl_vector_fixed<double, 3> axes;
          //vnl_vector_fixed<double, 3> eval;
          double LE, CE, exp_color;
          //if (!calculateProbOfPoint(scene, blk, points_data[currIdx], covs_data[currIdx], alpha_data[currIdx], prob, exp_color, axes, LE, CE))
          //  continue;
          if (!calculateLECEofPoint(covs_data[currIdx], axes, LE, CE))
            continue;

          if (LE >= LE_t || CE >= CE_t)
            continue;

          file <<  points_data[currIdx][0] << ' ' << points_data[currIdx][1] << ' ' << points_data[currIdx][2] << ' ';

          if(datatype == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
          {
            auto * mog_data = (boxm2_data_traits<BOXM2_MOG3_GREY>::datatype*) mog->data_buffer();
            exp_color = boxm2_processor_type<BOXM2_MOG3_GREY>::type::expected_color(mog_data[currIdx]);
            int col = (int)(exp_color*255);
            col = col > 255 ? 255 : col;
            file << col << ' ' << col << ' ' << col << ' ';
          }
          else if ( datatype == boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix() )
          {
            auto *color_data = (boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype*) mog->data_buffer();
            vnl_vector_fixed<float,3> exp_color = boxm2_processor_type<BOXM2_GAUSS_RGB>::type::expected_color(color_data[currIdx]);

            int col0 = (int)(exp_color[0]*255);
            col0 = col0 > 255 ? 255 : col0;
            int col1 = (int)(exp_color[1]*255);
            col1 = col1 > 255 ? 255 : col1;
            int col2 = (int)(exp_color[2]*255);
            col2 = col2 > 255 ? 255 : col2;
            file << col0 << ' ' << col1 << ' ' << col2 << ' ';
          }
          else
          {
            file << 0 << ' ' << 0 << ' ' << 0 << ' ';
          }
          file << axes[0] << ' ' << axes[1] << ' ' << axes[2] << ' '
               << LE << ' ' << CE << ' ' <<  prob << std::endl;
          num_vertices++;
        }
      }
    //}
  }
}
void boxm2_export_oriented_point_cloud_function::exportColorPointCloudPLY(const boxm2_scene_sptr&  /*scene*/, const boxm2_block_metadata&  /*data*/, boxm2_block*  /*blk*/,
                                                                        boxm2_data_base* mog, boxm2_data_base* alpha,const std::string& datatype ,
                                                                        boxm2_data_base* points,std::ofstream& file,float prob_t,vgl_box_3d<double> bb, unsigned& num_vertices)
{
    auto     *   points_data  = (boxm2_data_traits<BOXM2_POINT>::datatype*) points->data_buffer();

    auto     *   alpha_data   = (boxm2_data_traits<BOXM2_ALPHA>::datatype*) alpha->data_buffer();
    file << std::fixed;
    int pointTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());
    //check for invalid parameters
    if( pointTypeSize == 0 ) //This should never happen, it will result in division by zero later
    {
        std::cerr << "ERROR: Division by 0 in " << __FILE__ << __LINE__ << std::endl;
        throw 0;
    }
    for (unsigned currIdx=0; currIdx < (points->buffer_length()/pointTypeSize) ; currIdx++) {
        float prob = points_data[currIdx][3]; // during extractPointCloud process the prob of this point should be saved in this field
                                              // that process also checks whether the prob is below threshold and marks this field as -1.0
        // the probability check should have already been done by ExtractPointCloud processes
        //if (!calculateProbOfPoint(scene, blk, points_data[currIdx], alpha_data[currIdx], prob))
        //    continue;
        if (prob >= prob_t)
        {
            //check bounding box
            if (bb.is_empty() || bb.contains(points_data[currIdx][0] ,points_data[currIdx][1] ,points_data[currIdx][2]) )
            {
                file <<  points_data[currIdx][0] << ' ' << points_data[currIdx][1] << ' ' << points_data[currIdx][2] << ' ';

                if(datatype == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
                {
                    auto *   mog_data     = (boxm2_data_traits<BOXM2_MOG3_GREY>::datatype*) mog->data_buffer();
                    float exp_color  = boxm2_processor_type<BOXM2_MOG3_GREY>::type::expected_color(mog_data[currIdx]);
                    int col = (int)(exp_color*255);
                    col = col > 255 ? 255 : col;
                    file << col << ' ' << col << ' ' << col;
                }
                else if ( datatype == boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix() )
                {
                    auto *   color_data     = (boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype*) mog->data_buffer();
                    vnl_vector_fixed<float,3> exp_color = boxm2_processor_type<BOXM2_GAUSS_RGB>::type::expected_color(color_data[currIdx]);

                    int col0 = (int)(exp_color[0]*255);
                    col0 = col0 > 255 ? 255 : col0;
                    int col1 = (int)(exp_color[1]*255);
                    col1 = col1 > 255 ? 255 : col1;
                    int col2 = (int)(exp_color[2]*255);
                    col2 = col2 > 255 ? 255 : col2;
                    file << col0 << ' ' << col1 << ' ' << col2;

                }
                else{
                    file << 0 << ' ' << 0 << ' ' << 0;
                }

                file << ' ' << prob << ' ' <<"\n";

                num_vertices++;
            }
        }
    }
}
bool boxm2_export_oriented_point_cloud_function::calculateProbOfPoint(const boxm2_scene_sptr& scene, boxm2_block * blk,
                                                                      const vnl_vector_fixed<float, 4>& point,
                                                                      const vnl_vector_fixed<float, 9>& cov,
                                                                      const float& alpha,
                                                                      float& prob, double&  /*color*/, vnl_vector_fixed<double, 3>& axes, double& LE, double& CE)
{
  vgl_point_3d<double> local;
  boxm2_block_id id;

  vgl_point_3d<double> vgl_point(point[0],point[1],point[2]);

  //the following method checks whether the scene contains the point or not
  if (!calculateProbOfPoint(scene, blk, point, alpha, prob))
    return false;

  if (!calculateLECEofPoint(cov, axes, LE, CE))
    return false;

  return true;
}

bool boxm2_export_oriented_point_cloud_function::calculateLECEofPoint(const vnl_vector_fixed<float, 9>& cov,
                                                                      vnl_vector_fixed<double, 3>&axes, double& LE, double& CE)
{
  if (vnl_math::isinf(cov[0]) || vnl_math::isnan(cov[0]))  // the covariance matrices with such values cause problems for eigen value calculation, skip these points!
                                                           // if the covariance has invalid values, then over or under flows might have happened during cov computation using float point precision
                                                           // ideally we should switch to double point precision during covariance calculations
    return false;

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
  axes[0] =2*std::sqrt(eigs[2])*2.5;  // to find 90% confidence ellipsoid, scale the eigenvalues, see pg. 416 on Intro To Modern Photogrammetry, Mikhail, et. al.
  axes[1] =2*std::sqrt(eigs[1])*2.5;
  axes[2] =2*std::sqrt(eigs[0])*2.5;
  // check if values are valid (AND is the only way to detect invalid value, do not change into ORs
  if (!(axes[0] < std::numeric_limits<double>::max() && axes[0] > std::numeric_limits<double>::min() &&
        axes[1] < std::numeric_limits<double>::max() && axes[1] > std::numeric_limits<double>::min() &&
        axes[2] < std::numeric_limits<double>::max() && axes[2] > std::numeric_limits<double>::min()))
    return false;

  // now find LE (vertical error) using the eigenvector that corresponds to major axis
  vnl_vector<double> major = V.get_column(2);

  // create the vector that corresponds to error ellipsoid
  vnl_vector<double> major_ellipsoid = axes[0]*major;


  LE = std::abs(major_ellipsoid.get(2));
  double CEx = std::abs(major_ellipsoid.get(0));
  double CEy = std::abs(major_ellipsoid.get(1));

  CE = CEx > CEy ? CEx : CEy;

  return true;
}

bool boxm2_export_oriented_point_cloud_function::calculateProbOfPoint(const boxm2_scene_sptr& scene, boxm2_block * blk,
                                                                      const vnl_vector_fixed<float, 4>& point,
                                                                      const float& alpha, float& prob)
{
  vgl_point_3d<double> local;
  boxm2_block_id id;
  vgl_point_3d<double> vgl_point(point[0],point[1],point[2]);
  //boxm2_block_id id = blk->block_id();
  //if (!scene->block_contains(vgl_point, id, local))
  //  return false;

  //if the scene doesn't contain point,
  if (!scene->contains(vgl_point, id, local)) {
    return false;
  }
  //if the block passed isn't the block that contains the point, there is something wrong...
  //this happens when the point data is empty (0,0,0,0) for instance, or simply wrong.
  /*if (blk->block_id() != id)
    return false;
  */
  int index_x=(int)std::floor(local.x());
  int index_y=(int)std::floor(local.y());
  int index_z=(int)std::floor(local.z());

  boxm2_block_metadata mdata = scene->get_block_metadata_const(id);
  vnl_vector_fixed<unsigned char,16> treebits=blk->trees()(index_x,index_y,index_z);
  boct_bit_tree tree(treebits.data_block(),mdata.max_level_);
  int bit_index=tree.traverse(local);
  int depth=tree.depth_at(bit_index);
  auto side_len=static_cast<float>(mdata.sub_block_dim_.x()/((float)(1<<depth)));

  prob=1.0f-std::exp(-alpha*side_len);
  return true;
}

void boxm2_export_oriented_point_cloud_function::writePLYHeader(std::ofstream& file, unsigned num_vertices,std::stringstream& ss, bool output_aux)
{
   file << "ply\nformat ascii 1.0\nelement vertex " << num_vertices;
   file << "\nproperty float32 x\nproperty float32 y\nproperty float32 z";
   if (output_aux) {
     //file << "\nproperty float x\nproperty float y\nproperty float z\nproperty float nx\nproperty float ny\nproperty float nz\n" << "property float prob\nproperty float vis_score\n";
     file << "\nproperty float nx\nproperty float ny\nproperty float nz\n" << "property float prob\nproperty float vis_score\nproperty float nmag";
   }
   file << "\nend_header\n"
        << ss.str();
}

void boxm2_export_oriented_point_cloud_function::writePLYHeaderOnlyPoints(std::ofstream& file, unsigned num_vertices, std::stringstream& ss)
{
  file << "ply\nformat ascii 1.0\nelement vertex " << num_vertices
       << "\nproperty float x\nproperty float y\nproperty float z\nproperty uchar red\nproperty uchar green\nproperty uchar blue\n"
       << "property float axes_a\nproperty float axes_b\nproperty float axes_c\n"
    // << "property float eval_x\nproperty float eval_y\nproperty eval_z\n"
       << "property float le\nproperty float ce\n"
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
  std::vector<int > vertex_indices;
};


//: Call-back function for a "vertex" element
int boxm2_plyio_vertex_cb(p_ply_argument argument)
{
  long index;
  void* temp;
  ply_get_argument_user_data(argument, &temp, &index);

  auto* parsed_ply =  (ply_bb_reader*) temp;

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


void boxm2_export_oriented_point_cloud_function::readBBFromPLY(const std::string& filename, vgl_box_3d<double>& box)
{
  ply_bb_reader parsed_ply;
  parsed_ply.bbox = box;

  p_ply ply = ply_open(filename.c_str(), nullptr, 0, nullptr);
  if (!ply) {
    std::cout << "File " << filename << " doesn't exist.";
  }
  if (!ply_read_header(ply))
    std::cout << "File " << filename << " doesn't have header.";

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
