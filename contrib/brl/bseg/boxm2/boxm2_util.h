#ifndef boxm2_util_h
#define boxm2_util_h
//:
// \file
#include <iostream>
#include <limits>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vbl/vbl_array_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <bsta/bsta_histogram.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>

//: Utility class with static methods
class boxm2_util
{
  public:

    //: given a buffer and a length, randomly permutes the members of buffer
    static void random_permutation(int* buffer, int size);
    static float clamp(float x, float a, float b);

    static vpgl_camera_double_sptr camera_from_file(const std::string& camfile);
    static std::vector<std::string> images_from_directory(std::string dir);
    static std::vector<std::string> camfiles_from_directory(std::string dir);
    static std::vector<std::string> files_from_dir(const std::string& dir, const std::string& ext);


    static vbl_array_2d<vil_image_view<vxl_byte>* >  render_upper_hemisphere(int num_az,
                                                                             int num_incline,
                                                                             double radius,
                                                                             unsigned ni,
                                                                             unsigned nj,
                                                                             vgl_box_3d<double> bb);

    static vil_image_view_base_sptr prepare_input_image(const vil_image_view_base_sptr& loaded_image, bool force_grey=true);
    static vil_image_view_base_sptr prepare_input_rgb_image(const vil_image_view_base_sptr& loaded_image);
    static vil_image_view_base_sptr prepare_input_image(std::string filename, bool force_grey=true) {
      vil_image_view_base_sptr loaded_image = vil_load(filename.c_str());
      return boxm2_util::prepare_input_image(loaded_image,force_grey);
    }

    static vil_rgba<vxl_byte> mean_pixel(vil_image_view<vil_rgba<vxl_byte> >& img);
    static vpgl_perspective_camera<double>* construct_camera(double elevation,
                                                             double azimuth,
                                                             double radius,
                                                             unsigned ni,
                                                             unsigned nj,
                                                             vgl_box_3d<double> bb,
                                                             bool fit_bb=true);

    static int find_nearest_cam(vgl_vector_3d<double>& normal,
                                std::vector<vpgl_perspective_camera<double>* >& cams);

    static bool copy_file(const std::string& file, const std::string& dest);

    //Will be used to specify rows and cols for JavaScript App
    static bool generate_html(int width, int height, int nrows, int ncols, const std::string& dest);
    static bool generate_jsfunc(vbl_array_2d<std::string> img_files, const std::string& dest);

    //: function to obtain an image histogram
    static bsta_histogram_sptr generate_image_histogram(const vil_image_view_base_sptr& float_img, unsigned int numbins);

    //verifies that a scene has a valid appearance, spits out data type and appearance type size
    static bool verify_appearance(boxm2_scene& scene, const std::vector<std::string>&valid_types, std::string& data_type, int& appTypeSize );

    static bool get_point_index(boxm2_scene_sptr& scene,
                                boxm2_cache_sptr& cache,
                                const vgl_point_3d<double>& point,
                                boxm2_block_id& bid, int& data_index, float& side_len);

    static bool query_point(boxm2_scene_sptr& scene,
                            boxm2_cache_sptr& cache,
                            const vgl_point_3d<double>& point,
                            float& prob, float& intensity);
    // Computes the order of the blocks spiraling from the curr_block, find the depth from curr_block center and order with respect to that
    static std::vector<boxm2_block_id> order_about_a_block(const boxm2_scene_sptr& scene, const boxm2_block_id& curr_block, double distance = std::numeric_limits<double>::max());

    //: get blocks along a ray
    static std::vector<boxm2_block_id> blocks_along_a_ray(const boxm2_scene_sptr& scene, vgl_point_3d<double> p0, vgl_point_3d<double> p1);

    static bool get_raydirs_tfinal(const std::string& depthdir, const std::string& camsfile, vgl_point_3d<double> origin,
                                   std::vector<vil_image_view<float>*> & raydirs,
                                   std::vector<vil_image_view<float>*> & tfinal,int scale);

    static bool write_blocks_to_kml(boxm2_scene_sptr& scene, const std::string& kml_file, std::vector<boxm2_block_id> blks);
};

#endif // boxm2_util_h
