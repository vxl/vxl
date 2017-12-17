// This is brl/bseg/sdet/tests/test_selective_search.cxx
#include <sdet/sdet_graph_img_seg.h>
#include <sdet/sdet_selective_search.h>
#include <sdet/sdet_region.h>
#include <sdet/sdet_region_sptr.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vgl/vgl_area.h>
#include <testlib/testlib_test.h>
#include <string>
#include <map>
#include <fstream>
#define do_tests 0
#define test_rtree 0
#define test_tanks 1
#define tank_318 0
static void test_selective_search(int argc, char * argv[])
{
#if do_tests
  vil_image_view<vxl_byte> img(4,4);
  img.fill(0); img(1,2) = 255;  img(2,2) = 255;
  vil_image_view<vil_rgb<vxl_byte> > out_img;
  sdet_segment_img<vxl_byte>(img, 1, 8, (vxl_byte)100, 0, 0, out_img);
  TEST("out img pix 0 is not the same as pix 1", out_img(1,1) != out_img(1,2), true);
  TEST("out img pix 0 is not the same as pix 1", out_img(1,1) == out_img(2,1), true);
  TEST("out img pix 0 is not the same as pix 1", out_img(2,2) != out_img(2,1), true);
  TEST("out img pix 0 is not the same as pix 1", out_img(2,2) == out_img(1,2), true);
#if test_rtree
  std::cout << "\n<<<<<<<   test region_box tree >>>>>>>>>>>>>>\n";
  typedef sdet_rtree_region_box_2d<float> C_; // the helper class
  typedef C_::v_type V_; // the contained object type
  typedef C_::b_type B_; // the bounding object type
  vgl_rtree<V_, B_, C_> tr; // the rtree

  std::string small_img_path = "D:/tests/grouping/rajaei-cargo-2x-tank_249.tif";
  std::string color_path = "D:/tests/grouping/small_tank_249_color.tif";
  std::string bb_path = "D:/tests/grouping/small_tank_249_bboxes.txt";
  vil_image_view<vxl_byte> small_img = vil_load(small_img_path.c_str());
  sdet_selective_search sproc;
  sproc.set_byte_image_view(small_img);
  sproc.use_vd_edges_ = false;
  sproc.weight_thresh_ = 50.0f;
  sproc.use_vd_edges_ = false;
  sproc.process();
  vil_image_view<vil_rgb<vxl_byte> >& cimg = sproc.color_region_image();
  vil_save(cimg, color_path.c_str());
  std::ofstream ostr(bb_path.c_str());
  sproc.save_bboxes(ostr);
  ostr.close();
  std::cout << "region boxes " << std::endl;
   const std::map<unsigned, sdet_region_sptr>& sregions=sproc.diverse_regions();
  std::map<unsigned, sdet_region_sptr>::const_iterator rit = sregions.begin();
  for(;rit != sregions.end(); ++rit){
	vgl_box_2d<float>& bb =(*rit).second->bbox();
    if(!bb.is_empty())
      tr.add(rit->second);
    std::cout << rit->first << ' ' << (*rit).second->bbox() << std::endl;
  }
  std::vector<sdet_region_sptr> near_regions;
  rit = sregions.begin(); rit++;
  vgl_box_2d<float> bb0 = (*rit).second->bbox();
   std::cout << "found regions " << std::endl;
  tr.get(bb0, near_regions);
  for(std::vector<sdet_region_sptr>::iterator nit= near_regions.begin();
	  nit != near_regions.end(); ++nit)
	  std::cout << (*nit)->label() << ' ' << (*nit)->bbox() << std::endl;
#elif test_tanks
#if tank_318
  //std::string in_path = "D:/tests/grouping/rajaei-cargo-2x-mtanks_318.tif";
  std::string in_path = "D:/tests/grouping/rajaei-crates-2x_318.tif";
  std::string out_path = "D:/tests/grouping/test_rajaei_color_mss_318.tif";
  std::string region_path = "D:/tests/grouping/rajaei_regions_318.txt";
  std::string bbox_path = "D:/tests/grouping/rajaei_regions_318_bboxes.txt";
  std::string trouble_tank_in = "D:/tests/grouping/trouble-tank-318.tif";
#else // tank 249
  //std::string in_path = "D:/tests/grouping/rajaei-cargo-2x-mtanks_249.tif";
  //std::string in_path = "D:/tests/grouping/rajaei-crates-2x_249.tif";
 // std::string in_path = "D:/tests/grouping/rajaei-cargo-2x-trouble-crates_249.tif";
  //std::string out_path = "D:/tests/grouping/test_rajaei_color_mss_249.tif";
  //std::string region_path = "D:/tests/grouping/rajaei_regions_249.txt";
  std::string in_path = "D:/tests/grouping/single-frame-iss.tif";
  std::string out_path = "D:/tests/grouping/single-frame-iss-color.tif";
  std::string region_path = "D:/tests/grouping/single_frame-iss-regions.txt";
#endif
  vil_image_view<vxl_byte> imgi = vil_load(in_path.c_str());
  //  vil_image_view<vxl_byte> imgi = vil_load(trouble_tank_in.c_str());
  vxl_byte tr = vxl_byte(30);
  for(unsigned j = 0; j<imgi.nj(); ++j)
    for(unsigned i = 0; i<imgi.ni(); ++i)
      if(imgi(i,j)<tr)
        imgi(i,j) = vxl_byte(0);
  vil_image_view<vil_rgb<vxl_byte> > out_img_ss;
  sdet_selective_search proc;
  proc.set_byte_image_view(imgi);
#if 0
  proc.weight_thresh_ = 60.0f;
  std::cout << "==========weight thresh = " << 60.0f << std::endl;
  proc.process();
  proc.weight_thresh_ = 45.0f;
  std::cout << "==========weight thresh = " <<45.0f << std::endl;
  proc.process();
#endif
  proc.weight_thresh_ = 3.0f;
  proc.use_vd_edges_ = true;
  std::cout << "==========weight thresh = " <<3.0f << std::endl;
  proc.process();
  proc.weight_thresh_ = 20.0f;
  proc.four_or_eight_conn_ = 8;
  std::cout << "==========weight thresh = " <<20.0f << " 8 conn" << std::endl;
  proc.process();
  proc.weight_thresh_ =5.0f;
  proc.four_or_eight_conn_ = 4;
  proc.use_vd_edges_ = false;
  std::cout << "==========weight thresh = " <<5.0f << " 4 conn  no vd_edges" << std::endl;
  proc.process();
  proc.create_color_region_view(10, 2000);
  out_img_ss = proc.color_region_image();
  vil_save(out_img_ss, out_path.c_str());
 // proc.remove_diverse_duplicates();
  const std::map<unsigned, sdet_region_sptr>& regions=proc.diverse_regions();
  std::ofstream os(region_path.c_str());
  float r = 1.0f, amin  =2000.0f, amax = 3000.0f;
  for(std::map<unsigned, sdet_region_sptr>::const_iterator rit = regions.begin();
      rit != regions.end(); ++rit){
#if 0
    vgl_box_2d<float> bbox = (*rit).second->bbox();
    if(!(*rit).second->bbox_valid()) continue;
    float ar = vgl_area(bbox);
    if(ar<amin || ar>amax)
      continue;
    std::cout << (*rit).second->label() << ' ' << bbox << std::endl;
    bbox.write(os);
 
#else
    vgl_oriented_box_2d<float> obox = (*rit).second->obox();
    if((*rit).second->obox_valid()){
      float asp = obox.aspect_ratio();
      if(asp<r)
        continue;
      float a = vgl_area(obox);
      if(a<amin || a>amax)
        continue;
      os << obox;
    }

  
#endif
   }
  os.close();
#endif //test_tanks
#endif // do tests
}

TESTMAIN_ARGS(test_selective_search);
