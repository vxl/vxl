// This is gel/vmal/vmal_lines_correlation.h
#ifndef vmal_lines_corrletation_h_
#define vmal_lines_corrletation_h_
//--------------------------------------------------------------------------------
// .NAME        vmal_lines_correlation
// .INCLUDE     vmal/vmal_lines_correlation
// .FILE        vmal_lines_correlation.cxx
// .SECTION Description:
//   Compute the correlation between two lines. They must be of the same length
//   and the first end-point of the first line must match with the first end-point
//   of the second (the same thing for the second end-point). It means that before
//   calling vmal_lines_correlation, you must have:
//   - sort the two lines so their end-points are in the same order
//   - refine the lines so their length is the same
//   I created this class when I was working on lines matching. It means that
//   the first line is transformed by a homography in the second image. Then, the
//   the transformed line is projected on the second line after they had been sorted.
//   Then, w apply the lines_correlation to see if the lines could match. But, the
//   problem is that the homography may be not perfect and that the transformed line
//   is not exactly at the good place. So, to solve that, we make a research along
//   the transformed line to find a better correlation which will correct the error
//   due to the homography.
// .SECTION Author
//   L. Guichard 29-May-2001
// .SECTION Modifications:
//--------------------------------------------------------------------------------

#include <vnl/vnl_double_3.h>
#include <vil1/vil1_memory_image_of.h>
#include <vxl_config.h>

class vmal_lines_correlation
{
 public:
  vmal_lines_correlation();

  vmal_lines_correlation(int, double);

  ~vmal_lines_correlation();

  double find_min_corr(vnl_double_3 &line0p, vnl_double_3 &line0q,
                       vnl_double_3 &line1p, vnl_double_3 &line1q,
                       vil1_memory_image_of<vxl_byte> &image0,
                       vil1_memory_image_of<vxl_byte> &image1,
                       vnl_double_3 &trans);

  double lines_correlation(vnl_double_3 &line0_p, vnl_double_3 &line0_q,
                           vnl_double_3 &line1_p, vnl_double_3 &line1_q,
                           vil1_memory_image_of<vxl_byte> &image0,
                           vil1_memory_image_of<vxl_byte> &image1,
                           vnl_double_3 &trans, int bias);

  bool interpol_pixel(vnl_double_3 &pixel0, vnl_double_3 &pixel1,
                      vil1_memory_image_of<vxl_byte> &image0,
                      vil1_memory_image_of<vxl_byte> &image1,
                      unsigned char &value0, unsigned char &value1);

 private:
  int delta_;
  double radius_;
};

#endif // vmal_lines_corrletation_h_
