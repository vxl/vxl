//:
// \file
// \author Tim Cootes
// \brief Example program using F&H method to locate matches on a pair of images

#include <vcl_cassert.h>
#include <vul/vul_arg.h>
#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_gaussian_pyramid_builder_2d.h>
#include <vimt/vimt_image_pyramid.h>
#include <vil/algo/vil_corners.h>
#include <vil/algo/vil_find_peaks.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_fill.h>
#include <vil/vil_crop.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <mbl/mbl_index_sort.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <fhs/fhs_searcher.h>
#include <vimt/algo/vimt_normalised_correlation_2d.h>
#include <mbl/mbl_minimum_spanning_tree.h>
#include <mbl/mbl_draw_line.h>

void print_usage()
{
  vcl_cout<<"find_matches -i1 image1.jpg -i2 image2.jpg -L 2"<<vcl_endl;
  vcl_cout<<"Loads in image1 and image2."<<vcl_endl;
  vcl_cout<<"Locates a set of interesting features (corners) on level L of image1."<<vcl_endl;
  vcl_cout<<"Constructs a model of their relative positions."<<vcl_endl;
  vcl_cout<<"Uses normalised correllation and this model to locate"<<vcl_endl;
  vcl_cout<<"equivalent points on the same level of the second image."<<vcl_endl;
  vul_arg_display_usage_and_exit();
}

//: Write tree into the image
// Draw disks at each point, and lines between linked points
void draw_tree(vil_image_view<vxl_byte>& image,
               const vcl_vector<vgl_point_2d<double> >& pts,
               const vcl_vector<vcl_pair<int,int> >& pairs)
{
  // Draw tree into image for display purposes
  for (unsigned i=0;i<pairs.size();++i)
    mbl_draw_line(image,
                  pts[pairs[i].first],
                  pts[pairs[i].second],vxl_byte(255));

  // Write position of selected points into the original image
  // for display purposes.
  for (unsigned i=0;i<pts.size();++i)
  {
    vil_fill_disk(image,pts[i].x(),pts[i].y(),4,vxl_byte(255));
  }
}

int main( int argc, char* argv[] )
{
  vul_arg<vcl_string> image1_path("-i1","Input image 1");
  vul_arg<vcl_string> image2_path("-i2","Input image 2");
  vul_arg<vcl_string> output_image1_path("-o1","Output image 1","output1.png");
  vul_arg<vcl_string> output_image2_path("-o2","Output image 1","output2.png");
  vul_arg<unsigned> level("-L","Image pyramid level to work on",2);
  vul_arg<unsigned> nc("-n","Number of points to select",10);
  vul_arg<unsigned> w("-w","Half width of filters",7);
  vul_arg<double> f("-f","Relative strength of intensity vs shape",0.1);

  vul_arg_parse(argc, argv);

  if (image1_path()=="" || image2_path()=="")
  {
    print_usage();
    return 0;
  }

  // ============================================
  // Attempt to load in images
  // ============================================

  vimt_image_2d_of<vxl_byte> image1,image2;
  image1.image() = vil_load(image1_path().c_str());
  if (image1.image().size()==0)
  {
    vcl_cerr<<"Unable to read in image from "<<image1_path()<<vcl_endl;
    return 1;
  }
  image2.image() = vil_load(image2_path().c_str());
  if (image2.image().size()==0)
  {
    vcl_cerr<<"Unable to read in image from "<<image2_path()<<vcl_endl;
    return 1;
  }

  // ============================================
  // Build image pyramids and select chosen level
  // ============================================
  vimt_gaussian_pyramid_builder_2d<vxl_byte> pyr_builder;
  vimt_image_pyramid image_pyr1,image_pyr2;
  pyr_builder.build(image_pyr1,image1);
  pyr_builder.build(image_pyr2,image2);

  const vimt_image_2d_of<vxl_byte>& image1_L = static_cast<const vimt_image_2d_of<vxl_byte>&>(image_pyr1(level()));
  const vimt_image_2d_of<vxl_byte>& image2_L = static_cast<const vimt_image_2d_of<vxl_byte>&>(image_pyr2(level()));

  // ====================================================
  // Apply corner operator to image1_L and select corners
  // ====================================================

  vimt_image_2d_of<float> corner_im;
  corner_im.set_world2im(image1_L.world2im());
  vil_corners(image1_L.image(),corner_im.image());

  vcl_vector<unsigned> pi,pj;
  float threshold = 4.0f;
  vil_find_peaks_3x3(pi,pj,corner_im.image(),threshold);

  // Evaluate corner strength at each point (pi[i],pj[i])
  unsigned n = pi.size();
  vcl_vector<float> corner_str(n);
  for (unsigned i=0;i<n;++i)
    corner_str[i] = corner_im.image()(pi[i],pj[i]);

  // Sort and generate a list of image points and equivalent world points
  vcl_vector<unsigned> index;
  mbl_index_sort(corner_str,index);

  unsigned n_c = vcl_min(nc(),n);
  vcl_vector<vgl_point_2d<int> > im_pts(n_c);
  vcl_vector<vgl_point_2d<double> > w_pts(n_c);
  vimt_transform_2d im2w = image1_L.world2im().inverse();
  for (unsigned i=0;i<n_c;++i)
  {
    im_pts[i] = vgl_point_2d<int>(pi[index[n-1-i]],pj[index[n-1-i]]);
    w_pts[i]  = im2w(pi[index[n-1-i]],pj[index[n-1-i]]);
  }


  // ========================================================
  // Extract patches around each selected point and normalise
  // ========================================================
  vcl_vector<vil_image_view<float> > patch(n_c);
  vcl_vector<vgl_point_2d<double> > patch_ref(n_c);  // Reference point
  int ni = image1.image().ni();
  int nj = image1.image().nj();
  for (unsigned i=0;i<n_c;++i)
  {
    // Select region around point, allowing for image edges.
    int ilo = vcl_max(0,int(im_pts[i].x()-w()));
    int ihi = vcl_min(ni-1,int(im_pts[i].x()+w()));
    int jlo = vcl_max(0,int(im_pts[i].y()-w()));
    int jhi = vcl_min(nj-1,int(im_pts[i].y()+w()));

    // Compute position of reference point relative to corner
    int kx = im_pts[i].x()-ilo;
    int ky = im_pts[i].y()-jlo;
    patch_ref[i] =vgl_point_2d<double>(kx,ky);
    vil_convert_cast(vil_crop(image1_L.image(),ilo,1+ihi-ilo, jlo,1+jhi-jlo),
                     patch[i]);
    vil_math_normalise(patch[i]);
  }

  // Construct tree structure for points
  vcl_vector<vcl_pair<int,int> > pairs;
  mbl_minimum_spanning_tree(w_pts,pairs);

  assert(pairs.size()==n_c-1);

  // Draw tree into image for display purposes
  draw_tree(image1.image(),w_pts,pairs);

  if (vil_save(image1.image(),output_image1_path().c_str()))
  {
    vcl_cout<<"Saved output image 1 to "<<output_image1_path()<<vcl_endl;
  }

  // =================================================
  // Construct the arc model from the points and pairs
  // =================================================

  vcl_vector<fhs_arc> arcs(n_c-1);
  int root_node = pairs[0].first;
  for (unsigned i=0;i<pairs.size();++i)
  {
    int i1 = pairs[i].first;
    int i2 = pairs[i].second;
    vgl_vector_2d<double> dp = w_pts[i2]-w_pts[i1];
    double sd_x = vcl_max(0.1*ni,0.2*dp.length());
    double sd_y = vcl_max(0.1*nj,0.2*dp.length());
    arcs[i]=fhs_arc(i1,i2,dp.x(),dp.y(),sd_x*sd_x,sd_y*sd_y);
  }

  // =================================================
  // Apply filters to image2 (initially to whole image)
  // =================================================
  vcl_vector<vimt_image_2d_of<float> > feature_response(n_c);
  for (unsigned i=0;i<n_c;++i)
  {
    // Apply to whole image in first instance
    // Ideally would crop a region around expected position
    vimt_normalised_correlation_2d(image2_L,feature_response[i],
                                   patch[i],patch_ref[i],float());

    // Need good values to be small, not large, so apply -ve factor
    vil_math_scale_values(feature_response[i].image(),-f());
  }

  // ======================================================
  // Use fhs_searcher to locate equivalent points on image2
  // ======================================================

  fhs_searcher searcher;
  searcher.set_tree(arcs,root_node);
  searcher.search(feature_response);
  vcl_vector<vgl_point_2d<double> > pts2;
  searcher.best_points(pts2);

  // Draw tree into image for display purposes
  draw_tree(image2.image(),pts2,pairs);

  if (vil_save(image2.image(),output_image2_path().c_str()))
  {
    vcl_cout<<"Saved output image 2 to "<<output_image2_path()<<vcl_endl;
  }

  return 0;
}
