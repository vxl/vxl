// This is brl/bbas/bpgl/algo/rectify_params.h
#ifndef rectify_params_h_
#define rectify_params_h_
struct rectify_params{
  rectify_params():
    min_disparity_z_(NAN), n_points_(1000), upsample_scale_(1.0),
    invalid_pixel_val_(0.0f), min_overlap_fraction_(0.25) {}

  double min_disparity_z_;       // horizontal plane where disparity at each pixel is minimum
  size_t n_points_;              // number of points used to create correspondences
  double upsample_scale_;        // scale factor to upsample rectified images
  float invalid_pixel_val_;
  double min_overlap_fraction_;  // minimum fraction of points in overlap with tile in both images
  friend std::ostream& operator<<(std::ostream &os, const rectify_params& p){
    {
      os << "BPGL affine rectification parameters:\n"
         << "  min_disparity_z ......... " << p.min_disparity_z_ << '\n'
         << "  n_points ................ " << p.n_points_ << '\n'
         << "  upsample_scale .......... " << p.upsample_scale_ << '\n'
         << "  invalid_pixel_val ....... " << p.invalid_pixel_val_ << '\n'
         << "  min_overlap_fraction .... " << p.min_overlap_fraction_ << std::endl
        ;
      return os;
    }
  }
};
#endif

