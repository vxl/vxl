//this is /contrib/bm/bseg/bvpl/pro/bvpl_bundler_features_2d_compute_process.cxx
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for computing 2d features from 3d bundler points.
//
// \author Brandon A. Mayer
// \date Sept  8, 2011
// \verbatim
//  Modifications
//
// \endverbatim

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>

#include <bvpl/util/bvpl_bundler_features_2d.h>

#include <bwm/video/bwm_video_corr_sptr.h>
#include <bwm/video/bwm_video_corr.h>

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include<vgl/vgl_point_2d.h>
#include<vgl/vgl_point_3d.h>

#include<vnl/vnl_matrix_fixed.h>
#include<vnl/vnl_vector_fixed.h>
#include<vnl/vnl_vector.h>

#include <vidl/vidl_image_list_istream.h>
#include <vidl/vidl_convert.h>

#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

#include <vul/vul_file.h>

class coord_compare_2d
{
 public:
  coord_compare_2d()= default;
  ~coord_compare_2d()= default;

  bool operator() ( vgl_point_2d<double> const& pa, vgl_point_2d<double> const& pb ) const
  {
    if ( pa.x() != pb.x() )
      return pa.x() < pb.x();
    else
      return pa.y() < pb.y();
  }
};

struct kernel
{
  vgl_point_2d<float> min_pt;
  vgl_point_2d<float> max_pt;
  vnl_vector<double> w;
  std::vector<vgl_point_2d<float> > locs;
};

namespace bvpl_bundler_features_2d_compute_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 1;
}//end bvpl_bundler_features_2d_compute_process_globals

bool bvpl_bundler_features_2d_compute_process_cons( bprb_func_process& pro )
{
  using namespace bvpl_bundler_features_2d_compute_globals;

  std::vector<std::string> input_types_(n_inputs_);
  std::vector<std::string> output_types_(n_outputs_);

  unsigned i = 0;
  input_types_[i++] = "vcl_string";//bundler output file
  input_types_[i++] = "vcl_string";//original image glob
  input_types_[i++] = "vcl_string";//bad camera file
  input_types_[i++] = "vcl_string";//kernel directory

  output_types_[0] = "bvpl_bundler_features_2d_sptr";

  if (!pro.set_input_types(input_types_))
  {
    std::cerr << "----ERROR---- bvpl_bundler_features_2d_compute_process_cons\n"
             << "\tCOULD NOT SET INPUT TYPES.\n"
             << __FILE__ << '\n'
             << __LINE__ << '\n';
    return false;
  }

  if (!pro.set_output_types(output_types_))
  {
    std::cerr << "----ERROR---- bvpl_bundler_features_2d_compute_process_cons\n"
             << "\tCOULD NOT SET OUTPUT TYPES.\n"
             << __FILE__ << '\n'
             << __LINE__ << '\n';
    return false;
  }

  return true;
}//end bvpl_bundler_features_2d_compute_process_cons

bool bvpl_bundler_features_2d_compute_process( bprb_func_process& pro )
{
  using namespace  bvpl_bundler_features_2d_compute_globals;

  if ( pro.n_inputs() != n_inputs_ )
  {
    std::cerr << pro.name()
             << " bvpl_bundler_features_2d_compute_process: NUMBER OF INPUTS SHOULD BE: "
             << n_inputs_ << std::endl;
    return false;
  }

  //get inputs
  unsigned i = 0;
  std::string bundlerfile = pro.get_input<std::string>(i++);
  std::string img_glob = pro.get_input<std::string>(i++);
  std::string bad_cam_file = pro.get_input<std::string>(i++);
  std::string kernel_dir = pro.get_input<std::string>(i++);

  //------ PARSE BAD CAMERAS --------
  std::ifstream bcfile( bad_cam_file.c_str() );

  if ( !bcfile )
  {
    std::cerr << "---ERROR---- bvpl_bundler_features_2d_compute_process\n"
             << "\tERROR OPENING BAD CAMERA FILE.\n"
             << __FILE__ << '\n'
             << __LINE__ << '\n';
    return false;
  }

  std::set<unsigned> bad_cams;

  while ( !bcfile.eof() )
  {
    unsigned c;
    bcfile >> c;
    bad_cams.insert(c);
  }//end bad camera file iteration

  //------ READING KERNEL FILES--------

  if ( !vul_file::is_directory(kernel_dir) )
  {
    std::cerr << "---ERROR---- bvpl_bundler_features_2d_compute_process\n"
             << "\tKERNEL DIRECTORY NOT VALID.\n"
             << __FILE__ << '\n'
             << __LINE__ << '\n';
    return false;
  }

  std::vector<std::string> filenames;
  filenames.emplace_back("I0");
  filenames.emplace_back("Ix");
  filenames.emplace_back("Iy");
  filenames.emplace_back("Ixx");
  filenames.emplace_back("Iyy");
  filenames.emplace_back("Ixy");

  std::map<std::string, kernel > kernel_map;

  std::vector<std::string>::const_iterator
    k_itr, k_end = filenames.end();

  for ( k_itr = filenames.begin();
        k_itr != k_end; ++k_itr )
  {
    std::string fname = kernel_dir + "//" + *k_itr + ".txt";
    std::ifstream kernel_file(fname.c_str());

    if ( !kernel_file.good() )
    {
      std::cerr << "---ERROR---- bvpl_bundler_features_2d_compute_process\n"
               << "\tCOULD NOT OPEN KERNEL: " << *k_itr << '\n'
               << __FILE__ << '\n'
               << __LINE__ << '\n';
      return false;
    }

    vgl_point_2d<float> min_pt, max_pt;

    kernel_file >> min_pt;
    kernel_file >> max_pt;

    auto nx = (unsigned int)(max_pt.x() - min_pt.y() + 1);
    auto ny = (unsigned int)(max_pt.y() - min_pt.y() + 1);

    vnl_vector<double> w(nx*ny);

    kernel k;
    k.min_pt = min_pt;
    k.max_pt = max_pt;

    unsigned idx = 0;
    while (true)
    {
      vgl_point_2d<float> loc;
      kernel_file >> loc;
      double weight;
      kernel_file >> weight;
      if ( kernel_file.eof() )
        break;
      k.locs.push_back(loc);
      w[idx++] = weight;
    }//end kernel weight iteration
    k.w.set_size(w.size());
    k.w = w;
    kernel_map[*k_itr] = k;
    kernel_file.close();
  }//end kernel file iteration

  //------ READING & PROCESSING BUNDLER FILE --------
  std::ifstream bfile(bundlerfile.c_str());

  if (!bfile)
  {
    std::cerr << "---ERROR---- bvpl_bundler_features_2d_compute_process\n"
             << "\tERROR OPENING BUNDLER OUTPUT FILE.\n"
             << __FILE__ << '\n'
             << __LINE__ << '\n';
    return false;
  }

  vidl_image_list_istream video_stream(img_glob);

  if (!video_stream.is_open())
  {
    std::cerr << "---ERROR---- bvpl_bundler_features_2d_compute_process\n"
             << "\tINVALID VIDEO STREAM.\n"
             << __FILE__ << '\n'
             << __LINE__ << '\n';
    return false;
  }

  //get image size
  unsigned ni = video_stream.width();
  unsigned nj = video_stream.height();

  //central point of image
  vgl_point_2d<double> principal_point((double)ni/2,(double)nj/2);

  char buffer[1024];
  bfile.getline(buffer,1024); // read the header line

  if (bfile.eof())
  {
    std::cerr << "---ERROR---- bvpl_bundler_features_2d_compute_process\n"
             << "\tMISSING BUNDLER FILE DATA.\n"
             << __FILE__ << '\n'
             << __LINE__ << '\n';
    return false;
  }

  unsigned num_cams=0, num_pts=0;
  bfile>>num_cams>>num_pts; // reading number of cameras and number of 3-d pts

  for ( unsigned  i = 0; i < num_cams; ++i)
  {
    double f, k1, k2;
    vnl_matrix_fixed<double,3,3> R;
    vnl_vector_fixed<double,3> T;

    //reading camera matrices
    //don't need them but need to move file pointer
    bfile>>f>>k1>>k2;
    bfile>>R>>T;
  }//end num_cams iteration

  std::vector<bwm_video_corr_sptr> corrs;

  //relation view number - 2d point
  std::map<unsigned, std::set<vgl_point_2d<double>,coord_compare_2d > > view_point_map;

  std::cout << "Reading points..." << std::endl;

  //to minimize loading time calculate all points in each image we
  //need to calculate the value of the kernel at then load each
  //image only once, calculating at all required locations
  for ( unsigned  i = 0; i < num_pts; ++i )
  {
//#ifdef _DEBUG
//    std::cout << "Reading point: " << i << " of " << num_pts << std::endl;
//#endif//_DEBUG
    bwm_video_corr_sptr corr = new bwm_video_corr();
    //read the 3d point
    double x,y,z;
    bfile >> x >> y >> z;

    vgl_point_3d<double> bundler_pt(x,y,z);

    corr->set_world_pt(bundler_pt);

    //read the color value but dont use it
    unsigned r,g,b;
    bfile >> r >> g >> b;

    //read the number of views this 3d pt
    //is associated with
    unsigned num_views;
    bfile >> num_views;

    for ( unsigned j = 0; j < num_views; ++j )
    {
      unsigned view_number, key_number;
      double img_x, img_y;

      bfile >> view_number
          >> key_number
          >> img_x
          >> img_y;

      img_x = img_x + principal_point.x();
      img_y = nj - (img_y + principal_point.y());

      if ( !bad_cams.count(view_number) )
      {
        corr->add(view_number,vgl_point_2d<double>(img_x,img_y));
        if ( !view_point_map.count(view_number) )
        {
          std::set<vgl_point_2d<double>,coord_compare_2d > temp;
          temp.insert(vgl_point_2d<double>(img_x,img_y));
          view_point_map[view_number] = temp;
        }
        else
          view_point_map[view_number].insert(vgl_point_2d<double>(img_x,img_y));
      }//end if ! bad_cams
    }//end num_views iteration

    corrs.push_back(corr);
  }//end num_pts iteration

  //----------Compute Values of Kernels in Ea. View--------
  std::map<unsigned, std::set<vgl_point_2d<double>, coord_compare_2d > >::const_iterator
    v_itr = view_point_map.begin(), v_end = view_point_map.end();

  std::map<unsigned, std::map<vgl_point_2d<double>, vnl_vector<double>, coord_compare_2d> > view_pt_2d_feature_map;

  //for ( v_itr = view_point_map.begin(); v_itr != v_end; ++v_itr )
#ifdef _DEBUG
  std::size_t org_size = view_point_map.size();
#endif //_DEBUG
  std::cout << "Computing Kernels on All Images:" << std::endl;
  while ( !view_point_map.empty() )
  {
#ifdef _DEBUG
    std::cout << "Computing Kernels on image: " << org_size - std::distance(v_itr,v_end) << " of " << org_size << std::endl;
#endif //_DEBUG

    //load the image
    if ( !video_stream.seek_frame(v_itr->first) )
    {
      std::cerr << "---ERROR---- bvpl_bundler_features_2d_compute_process\n"
               << "\tCOULD NOT FIND FRAME: " <<v_itr->first << '\n'
               << __FILE__ << '\n'
               << __LINE__ << '\n';
      return false;
    }

    vil_image_view<vxl_byte> curr_img;
    vidl_convert_to_view(*video_stream.current_frame(), curr_img);

    if ( curr_img.nplanes() != 1 ||
         curr_img.pixel_format() == VIL_PIXEL_FORMAT_RGB_BYTE )
    {
      vil_image_view<vxl_byte> grey_img;
      vil_convert_planes_to_grey(curr_img, grey_img);
      curr_img = grey_img;
    }

    //calculate the value of the kernel at each point
    std::set<vgl_point_2d<double>, coord_compare_2d >::const_iterator
      p_itr, p_end = v_itr->second.end();

    std::map<vgl_point_2d<double>, vnl_vector<double>, coord_compare_2d> point_2d_feature_map;
    for ( p_itr = v_itr->second.begin(); p_itr != p_end; ++p_itr )
    {
      vnl_vector<double> feature_vector(filenames.size());

      for ( k_itr = filenames.begin();
            k_itr != k_end; ++k_itr )
      {
        kernel k = kernel_map[*k_itr];

        vnl_vector<double> neighborhood(k.w.size());

        //build this neighborhood for this kernel
        for ( unsigned i = 0; i < k.locs.size(); ++i )
        {
          int x = int(p_itr->x() + k.locs[i].x());
          int y = int(p_itr->y() + k.locs[i].y());

          //check if pixel index is within bounds
          //else pad with zeros
          if ( x >= 0 && y >= 0 &&
               unsigned(x) < ni && unsigned(y) < nj )
            neighborhood[i] = curr_img(x,y);
          else
            neighborhood[i] = double(0.0);
        }//end kernel loc iteration

        double f = dot_product(k.w,neighborhood);

        if (*k_itr == "I0")
          feature_vector[0] = f;
        else if (*k_itr == "Ix")
          feature_vector[1] = f;
        else if (*k_itr == "Iy")
          feature_vector[2] = f;
        else if (*k_itr == "Ixx")
          feature_vector[3] = f;
        else if (*k_itr == "Iyy")
          feature_vector[4] = f;
        else if (*k_itr == "Ixy")
          feature_vector[5] = f;
      }//end kernel filename iteration

      point_2d_feature_map.insert(std::make_pair(*p_itr,feature_vector));
    }//end point iteration

    view_pt_2d_feature_map.insert(std::make_pair(v_itr->first,point_2d_feature_map));

    //free the memory storing points and views
    //std::cout << "\tErasing " << v_itr->first << std::endl;
    view_point_map.erase(v_itr->first);

    if ( !view_point_map.empty() )
    {
      //std::cout << "\t\tThe map is not empty." << std::endl;
      v_itr = view_point_map.begin();
      v_end = view_point_map.end();
    }
    //else
      //std::cout << "\t\tThe map is empty." << std::endl;
  }//end view iteration

  bvpl_bundler_features_2d_sptr bundler_features_sptr =
    new bvpl_bundler_features_2d();
  //iterate through correspondences and associate the correct
  //3d point with the 2d feature
  std::vector<bwm_video_corr_sptr>::const_iterator c_itr, c_end = corrs.end();

    std::cout << "Retrieving features associated with 3d points.\n" << std::endl;
  for ( c_itr = corrs.begin(); c_itr != c_end; ++c_itr)
  {
#ifdef _DEBUG
    std::cout << "Processing: " << corrs.size() - std::distance(c_itr,c_end) << " of " << corrs.size() << std::endl;
#endif //_DEBUG
    std::map<unsigned, vgl_point_2d<double> > matches = (*c_itr)->matches();

    std::map<unsigned, vgl_point_2d<double> >::iterator m_itr, m_end = matches.end();

    std::map<unsigned, vnl_vector<double> > vf_map;
    for ( m_itr = matches.begin(); m_itr != m_end; ++m_itr )
    {
      std::map<vgl_point_2d<double>, vnl_vector<double>, coord_compare_2d>::const_iterator
        pf_itr = view_pt_2d_feature_map[m_itr->first].find(m_itr->second);
      if ( pf_itr != view_pt_2d_feature_map[m_itr->first].end() )
        vf_map.insert(std::make_pair(m_itr->first,view_pt_2d_feature_map[m_itr->first][m_itr->second]));
    }//end match iteration
    if ( !vf_map.empty() )
      bundler_features_sptr->pt_view_feature_map.insert(
        std::make_pair((*c_itr)->world_pt(), vf_map) );
  }

  pro.set_output_val(0, bundler_features_sptr);

  return true;
}//end bvpl_bundler_features_2d_compute_process
