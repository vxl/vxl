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

void print_usage()
{
  vcl_cout << "find_matches -i1 image1.jpg -i2 image2.jpg\n"
           << "Loads in image1 and image2.\n"
           << "Locates a set of interesting features on image1.\n"
           << "Constructs a model of their relative positions.\n"
           << "Uses normalised correllation and this model to locate\n"
           << "equivalent points on the second image." << vcl_endl;
  vul_arg_display_usage_and_exit();
}

//: Select the smallest pair s.t. first is in a, second in b
static vcl_pair<int,int> mbl_mst_next_pair(const vnl_matrix<double>& D,
                                           const vcl_vector<unsigned>& a,
                                           const vcl_vector<unsigned>& b)
{
  vcl_pair<int,int> p;
  double min_sim = 9.9e9;
  for (unsigned i=0;i<a.size();++i)
    for (unsigned j=0;j<b.size();++j)
    {
      double s = D(a[i],b[j]);
      if (s<min_sim)
      {
        min_sim=s;
        p.first=a[i];
        p.second=b[j];
      }
    }
  return p;
}

//: Compute the minimum spanning tree given a distance matrix
//  \param pairs[0].first is the root node
//  Tree defined by pairs.
//  \param pairs[i].second is linked to \param pairs[i].first
//  We compute the minimum spanning tree of the graph using Prim's algorithm.
void mbl_minimum_spanning_tree(const vnl_matrix<double>& D,
                               vcl_vector<vcl_pair<int,int> >& pairs)
{
  unsigned n = D.rows();
  vcl_vector<unsigned> a(0),b(n);
  for (unsigned i=0;i<n;++i) b[i]=i;
  // Select element closest to all others on average
  double min_sum=9e9;
  unsigned best_i=0;
  for (unsigned i=0;i<n;++i)
  {
    double sum = D.get_row(i).sum();
    if (sum<min_sum) { min_sum=sum; best_i=i; }
  }
  b.erase(vcl_find(b.begin(),b.end(),best_i));
  a.push_back(best_i);

  for (unsigned i=1;i<n;++i)
  {
    vcl_pair<int,int> p = mbl_mst_next_pair(D,a,b);
    pairs.push_back(p);
    b.erase(vcl_find(b.begin(),b.end(),p.second));
    a.push_back(p.second);
  }
}

//: Compute the minimum spanning tree of given points
//  \param pairs[0].first is the root node
//  Tree defined by pairs.
//  \param pairs[i].second is linked to \param pairs[i].first
//  We compute the minimum spanning tree of the graph using Prim's algorithm.
void mbl_minimum_spanning_tree(const vcl_vector<vgl_point_2d<double> >& pts,
                               vcl_vector<vcl_pair<int,int> >& pairs)
{
  unsigned n=pts.size();
  vnl_matrix<double> D(n,n);
  for (unsigned i=0;i<n;++i) D(i,i)=0.0;
  for (unsigned i=1;i<n;++i)
    for (unsigned j=0;j<i;++j)
    {
      D(i,j) = (pts[i]-pts[j]).length();
      D(j,i) = D(i,j);
    }

  mbl_minimum_spanning_tree(D,pairs);
}

// Draws value along line between p1 and p2
template<class T>
void draw_line(vil_image_view<T>& image,
               vgl_point_2d<double> p1,
               vgl_point_2d<double> p2, T value)
{
  vgl_vector_2d<double> dp = p2-p1;
  unsigned n = unsigned(1.5+vcl_max(vcl_fabs(dp.x()),vcl_fabs(dp.y())));
  dp/=n;
  unsigned ni=image.ni(), nj=image.nj();
  for (unsigned i=0;i<=n;++i,p1+=dp)
  {
    unsigned pi=unsigned(p1.x()+0.5); if (pi>=ni) continue;
    unsigned pj=unsigned(p1.y()+0.5); if (pj>=nj) continue;
    image(pi,pj)=value;
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
  for (unsigned i=0;i<pairs.size();++i)
    draw_line(image1.image(),
              w_pts[pairs[i].first],w_pts[pairs[i].second],vxl_byte(255));

  // Write position of selected points into the original image
  // for display purposes.
  for (unsigned i=0;i<n_c;++i)
  {
    vil_fill_disk(image1.image(),w_pts[i].x(),w_pts[i].y(),4,vxl_byte(255));
  }

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
  for (unsigned i=0;i<pairs.size();++i)
    draw_line(image2.image(),
              pts2[pairs[i].first],pts2[pairs[i].second],vxl_byte(255));

  // Write position of selected points into the original image
  // for display purposes.
  for (unsigned i=0;i<n_c;++i)
  {
    vil_fill_disk(image2.image(),pts2[i].x(),pts2[i].y(),4,vxl_byte(255));
  }

  if (vil_save(image2.image(),output_image2_path().c_str()))
  {
    vcl_cout<<"Saved output image 2 to "<<output_image2_path()<<vcl_endl;
  }

  return 0;
}
