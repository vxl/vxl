// This is contrib/brl/bbas/volm/pro/process/volm_ndsm_generation_process.cxx
#include <bprb/bprb_func_process.h>
// :
// \file
//     process to generate a normalized height image for given input land cover image from series of height image tiles
//     The input land cover image will have various land categories along with associated IDs and the specified ID is used
//     to define the ground height.  The output height map shall have same resolution as input height map tiles
//     Note that all input height image tiles are geotiff under WGS84 and the output image shall have same GSD as input height image tiles
//
// \verbatim
//  Modifications
//    none yet
// \endverbatim

#include <vcl_iomanip.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_resource.h>
#include <vgl/vgl_intersection.h>
#include <bvgl/algo/bvgl_2d_geo_index.h>
#include <bvgl/algo/bvgl_2d_geo_index_sptr.h>
#include <vcl_complex.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <volm/volm_io_tools.h>

namespace volm_ndsm_generation_process_globals
{
const unsigned n_inputs_  = 8;
const unsigned n_outputs_ = 3;
// function to obtain ground pixel height values from land cover image and aggregated height images
bool obtain_ground_pixels(vil_image_view<vxl_byte>* land_img, vpgl_geo_camera* land_cam, vil_image_view<float>* out_dsm,
                          vpgl_geo_camera* h_cam, unsigned const& start_ni, unsigned const& start_nj,
                          unsigned const& end_ni,   unsigned const& end_nj, vcl_vector<unsigned>& ground_ids,
                          vcl_vector<float>& ground_heights, vil_image_view<vxl_byte>& ground_mask);

// function to find ground height value from neighbor sub-pixels
float neighbor_height(vcl_map<vcl_pair<unsigned, unsigned>, float> const& window_min_height_map,
                      unsigned const& w_idx_i, unsigned const& w_idx_j, unsigned const& search_range);

// function to find ground height value from neighbor sub-pixels
bool neighbor_ground_height(unsigned const& i, unsigned const& j, vil_image_view<vxl_byte>* land_img,
                            vpgl_geo_camera* land_cam, vil_image_view<float>* h_img, vpgl_geo_camera* out_cam,
                            vcl_vector<unsigned> const& ground_ids, float & grouhd_height);

}

bool volm_ndsm_generation_process_cons(bprb_func_process& pro)
{
  using namespace volm_ndsm_generation_process_globals;
  // this process takes 6 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vpgl_camera_double_sptr";  // land cover map geo camera
  input_types_[1] = "vcl_string";               // land cover map file path
  input_types_[2] = "vcl_string";               // geo index for height maps
  input_types_[3] = "vcl_string";               // folder of height map tiles
  input_types_[4] = "unsigned";                 // window size
  input_types_[5] = "float";                    // maximum height limit
  input_types_[6] = "vcl_string";               // a text file to define the land categories that can be treated as ground pixels
  input_types_[7] = "vcl_string";               // ASTER DEM image folder, ensure the DEM images are converted to heights above WGS84 reference ellipsoid.
  // this process takes 2 outputs
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // normalized height image (byte image)
  output_types_[1] = "vil_image_view_base_sptr";  // original height image (float image)
  output_types_[2] = "vpgl_camera_double_sptr";   // geo camera of the output image

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

#if 1
bool volm_ndsm_generation_process(bprb_func_process& pro)
{
  using namespace volm_ndsm_generation_process_globals;
  // input check
  if( !pro.verify_inputs() )
    {
    vcl_cerr << pro.name() << ": Wrong Inputs!\n";
    return false;
    }
  // get the input
  unsigned                in_i = 0;
  vpgl_camera_double_sptr land_cam = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  vcl_string              land_img_file = pro.get_input<vcl_string>(in_i++);
  vcl_string              geo_index_txt = pro.get_input<vcl_string>(in_i++);
  vcl_string              h_map_folder = pro.get_input<vcl_string>(in_i++);
  unsigned                window_size = pro.get_input<unsigned>(in_i++);
  float                   h_max_limit = pro.get_input<float>(in_i++);
  vcl_string              ground_txt = pro.get_input<vcl_string>(in_i++);
  vcl_string              dem_folder = pro.get_input<vcl_string>(in_i++);

  if( !vul_file::exists(geo_index_txt) )
    {
    vcl_cerr << pro.name() << ": can not find geo index file " << geo_index_txt << "!\n";
    return false;
    }
  // parse the ground ids
  vcl_ifstream ifs(ground_txt.c_str() );
  if( !ifs )
    {
    vcl_cerr << pro.name() << ": can not open ground txt: " << ground_txt << "!!\n";
    return false;
    }
  vcl_vector<unsigned> ground_ids;
  while( !ifs.eof() )
    {
    unsigned id;
    ifs >> id;
    vcl_vector<unsigned>::iterator vit = vcl_find(ground_ids.begin(), ground_ids.end(), id);
    if( vit == ground_ids.end() )
      {
      ground_ids.push_back(id);
      }
    }

  // obtain the lower left and upper right of land cover image
  vil_image_resource_sptr img_res = vil_load_image_resource(land_img_file.c_str() );
  unsigned                l_ni = img_res->ni();
  unsigned                l_nj = img_res->nj();
  vpgl_geo_camera*        l_geocam = dynamic_cast<vpgl_geo_camera *>(land_cam.ptr() );
  if( !l_geocam )
    {
    vcl_cerr << pro.name() << ": input camera for land cover image is not valid!\n";
    return false;
    }
  double ll_lon, ll_lat;
  l_geocam->img_to_global(0,  l_nj,  ll_lon, ll_lat);
  double ur_lon, ur_lat;
  l_geocam->img_to_global(l_ni,  0, ur_lon, ur_lat);
  vgl_box_2d<double> l_bbox(ll_lon, ur_lon, ll_lat, ur_lat);
  // load the land image view
  vil_image_view_base_sptr  land_img_sptr = img_res->get_view(0, l_ni, 0, l_nj);
  vil_image_view<vxl_byte>* land_img_view = dynamic_cast<vil_image_view<vxl_byte> *>(land_img_sptr.ptr() );
  if( !land_img_view )
    {
    vcl_cerr << pro.name() << ": The land cover image pixle format: " << img_res->pixel_format()
             << " is not supported, available format is Byte!\n";
    return false;
    }

  // generate output image has GSD that is half of the GSD of land cover image
  unsigned           o_ni = l_ni * 2;
  unsigned           o_nj = l_nj * 2;
  double             scale_x =  (ur_lon - ll_lon) / o_ni;
  double             scale_y = -(ur_lat - ll_lat) / o_nj;
  vnl_matrix<double> trans_matrix(4, 4, 0.0);
  trans_matrix[0][0] = scale_x;
  trans_matrix[1][1] = scale_y;
  trans_matrix[0][3] = ll_lon;
  trans_matrix[1][3] = ur_lat;
  vpgl_geo_camera* out_cam = new vpgl_geo_camera(trans_matrix, l_geocam->lvcs() );
  out_cam->set_scale_format(true);
  vcl_cerr << "land map region: " << l_bbox << vcl_endl;
  vcl_cout << "output image size is ni: " << o_ni << ", nj: " << o_nj << vcl_endl;
  vil_image_view<float>*    out_dsm = new vil_image_view<float>(o_ni, o_nj);
  vil_image_view<vxl_byte>* out_ndsm = new vil_image_view<vxl_byte>(o_ni, o_nj);
  out_dsm->fill(-1.0);
  out_ndsm->fill(255);

  // obtain the height map image id that overlap with land cover image region
  double                                  min_size;
  bvgl_2d_geo_index_node_sptr             root = bvgl_2d_geo_index::read_and_construct<float>(geo_index_txt, min_size);
  vcl_vector<bvgl_2d_geo_index_node_sptr> leaves;
  bvgl_2d_geo_index::get_leaves(root, leaves);
  // get the ids of intersected leaves
  vcl_vector<unsigned> leaf_ids;
  for( unsigned i = 0; i < leaves.size(); i++ )
    {
    if( vgl_intersection(l_bbox, leaves[i]->extent_).area() > 0 )
      {
      leaf_ids.push_back(i);
      }
    }

  vcl_cout << "Number of leaves: " << leaves.size() << vcl_endl;
  if( leaf_ids.empty() )
    {
    vcl_cout << "no height map intersects with land cover image region, return an empty NDSM" << vcl_endl;
    pro.set_output_val<vil_image_view_base_sptr>(0, vil_image_view_base_sptr(out_ndsm) );
    pro.set_output_val<vil_image_view_base_sptr>(1, vil_image_view_base_sptr(out_dsm) );
    pro.set_output_val<vpgl_camera_double_sptr>(2, out_cam);
    return true;
    }

  vcl_cout << leaf_ids.size() << " height image tiles intersect with land cover image" << vcl_endl;

  // load all height maps
  vcl_vector<volm_img_info> h_infos;
  for( unsigned leaf_idx = 0; leaf_idx < leaf_ids.size(); leaf_idx++ )
    {
    vcl_stringstream img_file_stream;
    img_file_stream << h_map_folder << "/scene_" << leaf_ids[leaf_idx] << "_h_stereo.tif";
    vcl_string h_img_file = img_file_stream.str();
    if( !vul_file::exists(h_img_file) )
      {
      continue;
      }
    volm_img_info info;
    volm_io_tools::load_geotiff_image(h_img_file, info, false);
    h_infos.push_back(info);
    }
  // aggregate the height map
  vcl_cout << "Start to aggregate height images for land cover image region..." << vcl_endl;
  for( unsigned i = 0; i < o_ni; i++ )
    {
    if( i % 1000 == 0 )
      {
      vcl_cout << i << '.' << vcl_flush;
      }
    for( unsigned j = 0; j < o_nj; j++ )
      {
      double lon, lat;
      out_cam->img_to_global(i, j, lon, lat);
      bool found = false;
      for( vcl_vector<volm_img_info>::iterator vit = h_infos.begin(); (vit != h_infos.end() && !found); ++vit )
        {
        vgl_box_2d<double> bbox = vit->bbox;
        bbox.expand_about_centroid(2E-5);
        if( !bbox.contains(lon, lat) )
          {
          continue;
          }
        double u, v;
        vit->cam->global_to_img(lon, lat, 0.0, u, v);
        unsigned uu = (unsigned)vcl_floor(u + 0.5);
        unsigned vv = (unsigned)vcl_floor(v + 0.5);
        if( uu < vit->ni && vv < vit->nj )
          {
          found = true;
          vil_image_view<float> h_img(vit->img_r);
          (*out_dsm)(i, j) = h_img(uu, vv);
          }
        }
      }
    }

  // generate normalized image by finding nearest ground pixel height
  // for each pixel, look for the minimum ground height
  vcl_cout << "  \nStart to normalize the original DSM" << vcl_endl;
  vcl_cout << "image size: " << o_ni << 'x' << o_nj << vcl_endl;
  vcl_cout << "sub-pixel size: " << window_size << vcl_endl;
  vcl_cout << "  finding group height value by using ground pixels: ";
  for( unsigned i = 0; i < ground_ids.size(); i++ )
    {
    vcl_cout << ground_ids[i] << ", ";
    }
  vcl_cout << '\n';

  vil_image_view<vxl_byte> grd_mask(o_ni, o_nj);
  grd_mask.fill(0);
  unsigned                                     half_size = (unsigned)(vcl_floor(0.5 * window_size + 0.5) );
  vcl_map<vcl_pair<unsigned, unsigned>, float> window_min_height_map;
  window_min_height_map.clear();
  for( unsigned i = 0; i < o_ni; i++ )
    {
    if( i % 1000 == 0 )
      {
      vcl_cout << i << '.' << vcl_flush;
      }
    unsigned start_i = i - half_size;
    unsigned end_i   = i + half_size;
    if( start_i < 0 ) {start_i = 0; }
    if( end_i > o_ni ) {end_i = o_ni; }
    for( unsigned j = 0; j < o_nj; j++ )
      {
      // compute the search range
      unsigned start_j = j - half_size;
      unsigned end_j   = j + half_size;
      if( start_j < 0 ) {start_j = 0; }
      if( end_j > o_nj ) {end_j = o_nj; }
      if( i == 11000 && j == 10000 )
        {
        vcl_cout << "(" << i << "," << j << ") -- start: (" << start_i << "," << start_j << "), end: (" << end_i
                 << "," << end_j << ")" << vcl_endl;
        }
      vcl_vector<float> ground_heights;
      ground_heights.clear();
      if( !obtain_ground_pixels(land_img_view, l_geocam, out_dsm, out_cam, start_i, start_j, end_i, end_j, ground_ids,
                                ground_heights, grd_mask) )
        {
        continue;
        }
      vcl_sort(ground_heights.begin(), ground_heights.end() );
      vcl_pair<unsigned, unsigned> window_key(i, j);
      window_min_height_map.insert(vcl_pair<vcl_pair<unsigned, unsigned>, float>(window_key, ground_heights[0]) );
      }
    }

  if( window_min_height_map.empty() )
    {
    vcl_cout << pro.name() << ": can not find any ground pixels from land cover image!\n";
    return false;
    }

  vcl_cout << "\n  normalize the image" << vcl_endl;
  vil_image_view<float> grd_img(o_ni, o_nj);
  grd_img.fill(-1.0f);
  unsigned search_range = o_ni;
  if( search_range < o_nj )
    {
    search_range = o_nj;
    }
  for( unsigned i = 0; i < o_ni; i++ )
    {
    if( i % 1000 == 0 )
      {
      vcl_cout << i << '.' << vcl_flush;
      }
    for( unsigned j = 0; j < o_nj; j++ )
      {
      if( (*out_dsm)(i, j) < 0 )
        {
        continue;
        }
      if( grd_mask(i, j) == 1 )
        {
        (*out_ndsm)(i, j) = 0;
        continue;
        }
      // find the ground height for current pixel
      float                                                  grd_h = -1.0f;
      vcl_pair<unsigned, unsigned>                           window_key(i, j);
      vcl_map<vcl_pair<unsigned, unsigned>, float>::iterator mit = window_min_height_map.find(window_key);
      if( mit == window_min_height_map.end() )
        {
        grd_h = neighbor_height(window_min_height_map, i, j, search_range);
        }
      else
        {
        grd_h = mit->second;
        }
      grd_img(i, j) = grd_h;
      if( grd_h < 0 )
        {
        continue;
        }
      float scale = 254.0f / (h_max_limit - grd_h);
      float v = scale * ( (*out_dsm)(i, j) - grd_h);
      if( v > 254.0f )
        {
        continue;
        }
      else if( v < 0.0f )
        {
        (*out_ndsm)(i, j) = 0;
        }
      else
        {
        (*out_ndsm)(i, j) = (unsigned char)vcl_floor(v + 0.5f);
        }
      }
    }

  vil_image_view<float> grd_img1(o_ni, o_nj);
  grd_img1.fill(-1.0);
  for( vcl_map<vcl_pair<unsigned, unsigned>, float>::iterator mit = window_min_height_map.begin();
       mit != window_min_height_map.end(); ++mit )
    {
    grd_img1(mit->first.first, mit->first.second) = mit->second;
    }

  vil_save(grd_mask, "d:/work/find/ara_msi_tiling/DSM/AU/56H/s34e151/temp_continus_win_grd/grd_mask.tif");
  vil_save(grd_img, "d:/work/find/ara_msi_tiling/DSM/AU/56H/s34e151/temp_continus_win_grd/grd_img.tif");
  vil_save(grd_img1, "d:/work/find/ara_msi_tiling/DSM/AU/56H/s34e151/temp_continus_win_grd/grd_img1.tif");
  // output
  pro.set_output_val<vil_image_view_base_sptr>(0, vil_image_view_base_sptr(out_ndsm) );
  pro.set_output_val<vil_image_view_base_sptr>(1, vil_image_view_base_sptr(out_dsm) );
  pro.set_output_val<vpgl_camera_double_sptr>(2, out_cam);
  return true;
}

#endif

#if 0
bool volm_ndsm_generation_process(bprb_func_process& pro)
{
  using namespace volm_ndsm_generation_process_globals;
  // input check
  if( !pro.verify_inputs() )
    {
    vcl_cerr << pro.name() << ": Wrong Inputs!\n";
    return false;
    }
  // get the input
  unsigned                in_i = 0;
  vpgl_camera_double_sptr land_cam = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  vcl_string              land_img_file = pro.get_input<vcl_string>(in_i++);
  vcl_string              geo_index_txt = pro.get_input<vcl_string>(in_i++);
  vcl_string              h_map_folder = pro.get_input<vcl_string>(in_i++);
  unsigned                window_size = pro.get_input<unsigned>(in_i++);
  float                   h_max_limit = pro.get_input<float>(in_i++);
  vcl_string              ground_txt = pro.get_input<vcl_string>(in_i++);
  vcl_string              dem_folder = pro.get_input<vcl_string>(in_i++);

  if( !vul_file::exists(geo_index_txt) )
    {
    vcl_cerr << pro.name() << ": can not find geo index file " << geo_index_txt << "!\n";
    return false;
    }

  // parse the ground ids
  vcl_ifstream ifs(ground_txt.c_str() );
  if( !ifs )
    {
    vcl_cerr << pro.name() << ": can not open ground txt: " << ground_txt << "!!\n";
    return false;
    }
  vcl_vector<unsigned> ground_ids;
  while( !ifs.eof() )
    {
    unsigned id;
    ifs >> id;
    ground_ids.push_back(id);
    }

  // obtain the lower left and upper right of land cover image
  vil_image_resource_sptr img_res = vil_load_image_resource(land_img_file.c_str() );
  unsigned                l_ni = img_res->ni();
  unsigned                l_nj = img_res->nj();
  vpgl_geo_camera*        l_geocam = dynamic_cast<vpgl_geo_camera *>(land_cam.ptr() );
  if( !l_geocam )
    {
    vcl_cerr << pro.name() << ": input camera for land cover image is not valid!\n";
    return false;
    }
  double ll_lon, ll_lat;
  l_geocam->img_to_global(0,  l_nj,  ll_lon, ll_lat);
  double ur_lon, ur_lat;
  l_geocam->img_to_global(l_ni,  0, ur_lon, ur_lat);
  vgl_box_2d<double> l_bbox(ll_lon, ur_lon, ll_lat, ur_lat);
  // load the land image view
  vil_image_view_base_sptr  land_img_sptr = img_res->get_view(0, l_ni, 0, l_nj);
  vil_image_view<vxl_byte>* land_img_view = dynamic_cast<vil_image_view<vxl_byte> *>(land_img_sptr.ptr() );
  if( !land_img_view )
    {
    vcl_cerr << pro.name() << ": The land cover image pixle format: " << img_res->pixel_format()
             << " is not supported, available format is Byte!\n";
    return false;
    }

  // initialize an image that has same GSD as height map and same coverage of land cover image
#  if 0 // generate output image has GSD 1 meter along longitude and latitude

  vcl_string        file_glob = h_map_folder + "//*.tif";
  vul_file_iterator fn = file_glob.c_str();
  vcl_string        h_img_file = fn();
  vcl_cout << "get GSD from image: " << h_img_file << vcl_endl;
  vil_image_resource_sptr h_img_res = vil_load_image_resource(h_img_file.c_str() );
  vpgl_geo_camera*        h_cam;
  vpgl_lvcs_sptr          lvcs_dummy = new vpgl_lvcs;
  vpgl_geo_camera::init_geo_camera(h_img_res, lvcs_dummy, h_cam);
  if( !h_cam )
    {
    vcl_cerr << pro.name() << ": can not load geo camera from height image " << h_img_file << "!\n";
    return false;
    }
  // compute the image size
  double             scale_x = h_cam->trans_matrix()[0][0];
  double             scale_y = h_cam->trans_matrix()[1][1];
  vnl_matrix<double> trans_matrix(4, 4, 0.0);
  trans_matrix[0][0] = scale_x;
  trans_matrix[1][1] = scale_y;
  trans_matrix[0][3] = ll_lon;
  trans_matrix[1][3] = ur_lat;
  vpgl_geo_camera* out_cam = new vpgl_geo_camera(trans_matrix, lvcs_dummy);
  out_cam->set_scale_format(true);
  double o_u, o_v;
  out_cam->global_to_img(ur_lon, ll_lat, 0, o_u, o_v);
  unsigned o_ni = vcl_ceil(o_u);
  unsigned o_nj = vcl_ceil(o_v);
  vcl_cerr << "land map region: " << l_bbox << vcl_endl;
  vcl_cout << "output image size is ni: " << o_ni << ", nj: " << o_nj << vcl_endl;
  vil_image_view<float>*    out_dsm = new vil_image_view<float>(o_ni, o_nj);
  vil_image_view<vxl_byte>* out_ndsm = new vil_image_view<vxl_byte>(o_ni, o_nj);
  out_dsm->fill(-1.0);
  out_ndsm->fill(255);
#  endif

#  if 1 // generate output image has GSD that is half of the GSD of land cover image
  unsigned           o_ni = l_ni * 2;
  unsigned           o_nj = l_nj * 2;
  double             scale_x =  (ur_lon - ll_lon) / o_ni;
  double             scale_y = -(ur_lat - ll_lat) / o_nj;
  vnl_matrix<double> trans_matrix(4, 4, 0.0);
  trans_matrix[0][0] = scale_x;
  trans_matrix[1][1] = scale_y;
  trans_matrix[0][3] = ll_lon;
  trans_matrix[1][3] = ur_lat;
  vpgl_geo_camera* out_cam = new vpgl_geo_camera(trans_matrix, l_geocam->lvcs() );
  out_cam->set_scale_format(true);
  vcl_cerr << "land map region: " << l_bbox << vcl_endl;
  vcl_cout << "output image size is ni: " << o_ni << ", nj: " << o_nj << vcl_endl;
  vil_image_view<float>*    out_dsm = new vil_image_view<float>(o_ni, o_nj);
  vil_image_view<vxl_byte>* out_ndsm = new vil_image_view<vxl_byte>(o_ni, o_nj);
  out_dsm->fill(-1.0);
  out_ndsm->fill(255);
#  endif

  // obtain the height map image id that overlap with land cover image region
  double                                  min_size;
  bvgl_2d_geo_index_node_sptr             root = bvgl_2d_geo_index::read_and_construct<float>(geo_index_txt, min_size);
  vcl_vector<bvgl_2d_geo_index_node_sptr> leaves;
  bvgl_2d_geo_index::get_leaves(root, leaves);
  // get the ids of intersected leaves
  vcl_vector<unsigned> leaf_ids;
  for( unsigned i = 0; i < leaves.size(); i++ )
    {
    if( vgl_intersection(l_bbox, leaves[i]->extent_).area() > 0 )
      {
      leaf_ids.push_back(i);
      }
    }

  vcl_cout << "Number of leaves: " << leaves.size() << vcl_endl;
  if( leaf_ids.empty() )
    {
    vcl_cout << "no height map intersects with land cover image region, return an empty NDSM" << vcl_endl;
    pro.set_output_val<vil_image_view_base_sptr>(0, vil_image_view_base_sptr(out_ndsm) );
    pro.set_output_val<vil_image_view_base_sptr>(1, vil_image_view_base_sptr(out_dsm) );
    pro.set_output_val<vpgl_camera_double_sptr>(2, out_cam);
    return true;
    }

  vcl_cout << leaf_ids.size() << " height image tiles intersect with land cover image" << vcl_endl;
#  if 0
  for( unsigned i = 0; i < leaf_ids.size(); i++ )
    {
    vcl_cout << leaf_ids[i] << ' ';
    }
  vcl_cout << vcl_endl;
#  endif

  // load all height maps
  vcl_vector<volm_img_info> h_infos;
  for( unsigned leaf_idx = 0; leaf_idx < leaf_ids.size(); leaf_idx++ )
    {
    vcl_stringstream img_file_stream;
    img_file_stream << h_map_folder << "/scene_" << leaf_ids[leaf_idx] << "_h_stereo.tif";
    vcl_string h_img_file = img_file_stream.str();
    if( !vul_file::exists(h_img_file) )
      {
      continue;
      }
    volm_img_info info;
    volm_io_tools::load_geotiff_image(h_img_file, info, false);
    h_infos.push_back(info);
    }
  // aggregate the height map
  vcl_cout << "Start to aggregate height images for land cover image region..." << vcl_endl;
  for( unsigned i = 0; i < o_ni; i++ )
    {
    if( i % 1000 == 0 )
      {
      vcl_cout << i << '.' << vcl_flush;
      }
    for( unsigned j = 0; j < o_nj; j++ )
      {
      double lon, lat;
      out_cam->img_to_global(i, j, lon, lat);
      bool found = false;
      for( vcl_vector<volm_img_info>::iterator vit = h_infos.begin(); (vit != h_infos.end() && !found); ++vit )
        {
        vgl_box_2d<double> bbox = vit->bbox;
        bbox.expand_about_centroid(2E-5);
        if( !bbox.contains(lon, lat) )
          {
          continue;
          }
        double u, v;
        vit->cam->global_to_img(lon, lat, 0.0, u, v);
        unsigned uu = (unsigned)vcl_floor(u + 0.5);
        unsigned vv = (unsigned)vcl_floor(v + 0.5);
        if( uu < vit->ni && vv < vit->nj )
          {
          found = true;
          vil_image_view<float> h_img(vit->img_r);
          (*out_dsm)(i, j) = h_img(uu, vv);
          }
        }
      }
    }
  // start to normalize the aggregated height map
  // find the ground pixels for each sub-pixels
  unsigned num_w_i = o_ni / window_size + 1;
  unsigned num_w_j = o_nj / window_size + 1;
  vcl_cout << "\nStart to normalize the original DSM..." << vcl_endl;
  vcl_cout << "image size: " << o_ni << 'x' << o_nj << vcl_endl;
  vcl_cout << "sub-pixel size: " << window_size << vcl_endl;
  vcl_cout << "sup-pixel number: " << num_w_i << 'x' << num_w_j << vcl_endl;
  vcl_cout << "  finding group height value by using ground pixels: ";
  for( unsigned i = 0; i < ground_ids.size(); i++ )
    {
    vcl_cout << ground_ids[i] << ", ";
    }
  vcl_cout << '\n';
  // maps to store the ground height value of each sub pixel (key is the sub_pixel id)
  vcl_map<vcl_pair<unsigned, unsigned>, float> window_min_height_map;
  window_min_height_map.clear();
  vil_image_view<vxl_byte> ground_mask(o_ni, o_nj);
  ground_mask.fill(0);
  for( unsigned w_idx_i = 0; w_idx_i < num_w_i; w_idx_i++ )
    {
    vcl_cout << w_idx_i << '.'; vcl_cout.flush();
    unsigned start_ni, end_ni;
    start_ni = w_idx_i * window_size;
    end_ni = (w_idx_i + 1) * window_size;
    for( unsigned w_idx_j = 0; w_idx_j < num_w_j; w_idx_j++ )
      {
      vcl_pair<unsigned, unsigned> window_key(w_idx_i, w_idx_j);
      unsigned                     start_nj, end_nj;
      start_nj = w_idx_j * window_size;
      end_nj = (w_idx_j + 1) * window_size;
      // obtain the ground pixels
      vcl_vector<float> ground_heights;
      ground_heights.clear();
      if( !obtain_ground_pixels(land_img_view, l_geocam, out_dsm, out_cam, start_ni, start_nj, end_ni, end_nj,
                                ground_ids, ground_heights, ground_mask) )
        {
        vcl_cerr << pro.name() << "Finding ground pixel failed for sub-pixel: (" << w_idx_i << ',' << w_idx_j << ")!\n";
        return false;
        }

      if( ground_heights.size() )
        {
        // find the median height value
        vcl_sort(ground_heights.begin(), ground_heights.end() );
        // float h_median = ground_heights[(unsigned)vcl_floor(ground_heights.size()/2.0)];
        // window_min_height_map.insert(vcl_pair<vcl_pair<unsigned, unsigned>, float>(window_key, h_median));
        float h_minimum = ground_heights[0];
        window_min_height_map.insert(vcl_pair<vcl_pair<unsigned, unsigned>, float>(window_key, h_minimum) );
        // vcl_cout << "\nstart: (" << start_ni << ',' << start_nj << "), end: (" << end_ni << ',' << end_nj << ")" << vcl_endl;
        // vcl_cout << "ground height values: ";
        // for (vcl_vector<float>::iterator vit = ground_heights.begin(); vit != ground_heights.end(); ++vit)
        //  vcl_cout << *vit << ", ";
        // vcl_cout << "\n";
        // vcl_cout << "ground median: " << h_minimum << vcl_endl;
        }
      }
    }
  if( window_min_height_map.empty() )
    {
    vcl_cout << pro.name() << ": can not find any ground pixels from land cover image!\n";
    return false;
    }

  // vcl_cout << "Ground height:" << vcl_endl;
  vil_image_view<float> ground_img(o_ni, o_nj);
  ground_img.fill(-1.0);
  for( vcl_map<vcl_pair<unsigned, unsigned>, float>::iterator mit = window_min_height_map.begin();
       mit != window_min_height_map.end(); ++mit )
    {
    unsigned start_i = mit->first.first * window_size;
    unsigned end_i   = (mit->first.first + 1) * window_size;
    unsigned start_j = mit->first.second * window_size;
    unsigned end_j   = (mit->first.second + 1) * window_size;
    for( unsigned i = start_i; i < end_i; i++ )
      {
      for( unsigned j = start_j; j < end_j; j++ )
        {
        if( i >= o_ni || j >= o_nj )
          {
          continue;
          }
        ground_img(i, j) = mit->second;
        }
      }
    }

  vcl_cout << "  \ngenerating normalized DSM...";
  unsigned search_range = num_w_i;
  if( search_range > num_w_j ) {search_range = num_w_j; }
  for( unsigned w_idx_i = 0; w_idx_i < num_w_i; w_idx_i++ )
    {
    unsigned start_ni, end_ni;
    start_ni = w_idx_i * window_size;
    end_ni = (w_idx_i + 1) * window_size;
    for( unsigned w_idx_j = 0; w_idx_j < num_w_j; w_idx_j++ )
      {
      vcl_pair<unsigned, unsigned> window_key(w_idx_i, w_idx_j);
      unsigned                     start_nj, end_nj;
      start_nj = w_idx_j * window_size;
      end_nj = (w_idx_j + 1) * window_size;
      // check whether pixels in current sub-pixel is all invalid
      bool is_sub_valid = false;
      for( unsigned p_i = start_ni; (p_i < end_ni && !is_sub_valid); p_i++ )
        {
        for( unsigned p_j = start_nj; (p_j < end_nj && !is_sub_valid); p_j++ )
          {
          if( p_i >= o_ni || p_j >= o_nj )
            {
            continue;
            }
          if( (*out_dsm)(p_i, p_j) > 0.0 )
            {
            is_sub_valid = true;
            }
          }
        }
      if( !is_sub_valid )
        {
        continue;
        }
      // find the ground height to normalize the DSM
      float                                                  grd_height = -500.0f;
      vcl_map<vcl_pair<unsigned, unsigned>, float>::iterator mit = window_min_height_map.find(window_key);
      if( mit == window_min_height_map.end() )
        {
        // obtain ground height from its nearest sub-pixel
        grd_height = neighbor_height(window_min_height_map, w_idx_i, w_idx_j, search_range);
        }
      else
        {
        grd_height = mit->second;
        }
      if( grd_height < -400.0f )  // can not find ground height, treat this window as invalid
        {
        continue;
        }

      // use ground height value to normalize the height image
      float scale = 254.0f / (h_max_limit - grd_height);
      for( unsigned i = start_ni; i < end_ni; i++ )
        {
        for( unsigned j = start_nj; j < end_nj; j++ )
          {
          if( i >= o_ni || j >= o_nj )
            {
            continue;
            }
          if( (*out_dsm)(i, j) == -1.0 )
            {
            continue;
            }
          if( ground_mask(i, j) == 1 )
            {
            (*out_ndsm)(i, j) = 0;
            continue;
            }
          float v = scale * ( (*out_dsm)(i, j) - grd_height);
          if( v < 0.0f )
            {
            (*out_ndsm)(i, j) = 0;
            }
          else if( v > 254.0f )
            {
            (*out_ndsm)(i, j) = 255;
            }
          else
            {
            (*out_ndsm)(i, j) = (unsigned char)vcl_floor(v + 0.5f);
            }
          // ground_img(i,j) = grd_height;
          }
        }
      }
    }

  //// generate dem image
  // vcl_vector<volm_img_info> dem_infos;
  // volm_io_tools::load_aster_dem_imgs(dem_folder, dem_infos);
  // vil_image_view<float> dem_img(o_ni, o_nj);
  // vil_image_view<float> dem_img_full(dem_infos[0].img_r);
  // dem_img.fill(-1.0f);
  // for (unsigned i = 0; i < o_ni; i++) {
  //  for (unsigned j = 0; j < o_nj; j++) {
  //    double lon, lat;
  //    out_cam->img_to_global(i, j, lon, lat);
  //    double u, v;
  //    dem_infos[0].cam->global_to_img(lon, lat, 0, u, v);
  //    unsigned uu = (unsigned)vcl_floor(u+0.5);
  //    unsigned vv = (unsigned)vcl_floor(v+0.5);
  //    if (uu < dem_infos[0].ni && vv < dem_infos[0].nj) {
  //      dem_img(i,j) = dem_img_full(uu,vv);
  //    }
  //  }
  // }

  // output
  pro.set_output_val<vil_image_view_base_sptr>(0, vil_image_view_base_sptr(out_ndsm) );
  pro.set_output_val<vil_image_view_base_sptr>(1, vil_image_view_base_sptr(out_dsm) );
  pro.set_output_val<vpgl_camera_double_sptr>(2, out_cam);
  return true;
}

#endif

bool volm_ndsm_generation_process_globals::obtain_ground_pixels(vil_image_view<vxl_byte>* land_img,
                                                                vpgl_geo_camera* land_cam,
                                                                vil_image_view<float>* out_dsm,
                                                                vpgl_geo_camera* out_cam,
                                                                unsigned const& start_ni, unsigned const& start_nj,
                                                                unsigned const& end_ni,   unsigned const& end_nj,
                                                                vcl_vector<unsigned>& ground_ids,
                                                                vcl_vector<float>& ground_heights,
                                                                vil_image_view<vxl_byte>& ground_mask)
{
  ground_heights.clear();
  unsigned l_ni = land_img->ni();
  unsigned l_nj = land_img->nj();
  unsigned h_ni = out_dsm->ni();
  unsigned h_nj = out_dsm->nj();
  for( unsigned i = start_ni; i < end_ni; i++ )
    {
    for( unsigned j = start_nj; j < end_nj; j++ )
      {
      if( i >= h_ni || j >= h_nj )
        {
        continue;
        }
      if( (*out_dsm)(i, j) < 0 )
        {
        continue;
        }
      double lon, lat;
      out_cam->img_to_global(i, j, lon, lat);
      double u, v;
      land_cam->global_to_img(lon, lat, 0.0, u, v);
      unsigned uu = (unsigned)vcl_floor(u + 0.5);
      unsigned vv = (unsigned)vcl_floor(v + 0.5);
      if( uu >= l_ni || vv >= l_nj )
        {
        continue;
        }
      unsigned                       id = (*land_img)(uu, vv);
      vcl_vector<unsigned>::iterator vit = vcl_find(ground_ids.begin(), ground_ids.end(), id);
      if( vit != ground_ids.end() )
        {
        ground_heights.push_back( (*out_dsm)(i, j) );
        ground_mask(i, j) = 1;
        }
      }
    }
  if( ground_heights.empty() )
    {
    return false;
    }
  return true;
}

float volm_ndsm_generation_process_globals::neighbor_height(vcl_map<vcl_pair<unsigned, unsigned>,
                                                                    float> const& window_min_height_map,
                                                            unsigned const& w_idx_i, unsigned const& w_idx_j,
                                                            unsigned const& search_range)
{
  bool     found_neigh_height = false;
  unsigned num_nbrs = 8;

  vcl_vector<float> neigh_heights;
  // keep increasing search radius
  for( int radius = 1; (radius < (int)search_range && !found_neigh_height); radius++ )
    {
    neigh_heights.clear();
    int nbrs8_delta[8][2] = { { radius, 0}, { radius, -radius}, { 0, -radius}, {-radius, -radius},
                                  {-radius, 0}, {-radius, radius}, { 0, radius}, { radius, radius} };
    for( unsigned c = 0; c < num_nbrs; c++ )
      {
      int nbr_i = (int)w_idx_i + nbrs8_delta[c][0];
      int nbr_j = (int)w_idx_j + nbrs8_delta[c][1];
      vcl_map<vcl_pair<unsigned, unsigned>,
              float>::const_iterator mit = window_min_height_map.find(vcl_pair<unsigned, unsigned>(nbr_i, nbr_j) );
      if( mit != window_min_height_map.end() )
        {
        neigh_heights.push_back(mit->second);
        }
      }
    if( neigh_heights.size() != 0 )
      {
      found_neigh_height = true;
      }
    }
  if( neigh_heights.empty() )
    {
    return -500.0f;
    }

  float grd_height = 0.0f;
  for( unsigned i = 0; i < neigh_heights.size(); i++ )
    {
    grd_height += neigh_heights[i];
    }
  return grd_height / neigh_heights.size();
}

bool volm_ndsm_generation_process_globals::neighbor_ground_height(unsigned const& li, unsigned const& lj,
                                                                  vil_image_view<vxl_byte>* land_img,
                                                                  vpgl_geo_camera* land_cam,
                                                                  vil_image_view<float>* h_img,
                                                                  vpgl_geo_camera* h_cam,
                                                                  vcl_vector<unsigned> const& ground_ids,
                                                                  float & grouhd_height)
{
  bool     found_neigh_height = false;
  unsigned num_nbrs = 8;

  vcl_vector<float> neigh_heights;
  unsigned          l_ni = land_img->ni();
  unsigned          l_nj = land_img->nj();
  unsigned          h_ni = h_img->ni();
  unsigned          h_nj = h_img->nj();
  unsigned          search_range = l_ni;
  if( search_range < l_nj )
    {
    search_range = l_nj;
    }
  for( int radius = 1; (radius < (int)search_range && !found_neigh_height); radius++ )
    {
    neigh_heights.clear();
    int nbrs8_delta[8][2] = { { radius, 0}, { radius, -radius}, { 0, -radius}, {-radius, -radius},
                                  {-radius, 0}, {-radius, radius}, { 0, radius}, { radius, radius} };
    for( unsigned c = 0; c < num_nbrs; c++ )
      {
      int nbr_i = (int)li + nbrs8_delta[c][0];
      int nbr_j = (int)lj + nbrs8_delta[c][1];
      if( nbr_i < 0 || nbr_j < 0 || nbr_i >= (int)l_ni || nbr_j >= (int)l_nj )
        {
        continue;
        }
      vcl_vector<unsigned>::const_iterator vit =
        vcl_find(ground_ids.begin(), ground_ids.end(), (unsigned)(*land_img)(nbr_i, nbr_j) );
      if( vit != ground_ids.end() )
        {
        // find the height value from height image
        double lon, lat;
        land_cam->img_to_global(nbr_i, nbr_j, lon, lat);
        double u, v;
        h_cam->global_to_img(lon, lat, 0.0, u, v);
        unsigned uu = (unsigned)vcl_floor(u + 0.5);
        unsigned vv = (unsigned)vcl_floor(v + 0.5);
        if( uu < h_ni && vv < h_nj )
          {
          neigh_heights.push_back( (*h_img)(uu, vv) );
          }
        }
      }

    if( neigh_heights.size() )
      {
      found_neigh_height = true;
      }
    }

  if( neigh_heights.empty() )
    {
    return false;
    }

  // return the minimum ground height value

  vcl_sort(neigh_heights.begin(), neigh_heights.end() );
  grouhd_height = neigh_heights[0];

  return true;
}
