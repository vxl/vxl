// This is brl/bseg/sdet/tests/test_region_classifier.cxx
#include <sdet/sdet_graph_img_seg.h>
#include <sdet/sdet_selective_search.h>
#include <sdet/sdet_region_classifier.h>
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
static void test_region_classifier(int argc, char * argv[])
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
  std::string in_path = "D:/tests/grouping/rajaei-cargo-2x-mtanks_318.tif";
  //std::string in_path = "D:/tests/grouping/rajaei-crates-2x_318.tif";
  std::string out_path = "D:/tests/grouping/test_rajaei_color_mss_318.tif";
  //std::string region_path = "D:/tests/grouping/rajaei_regions_318.txt";
  std::string bright_region_path = "D:/tests/grouping/rajaei_bright_regions_318.txt";
  std::string bbox_path = "D:/tests/grouping/rajaei_regions_318_bboxes.txt";
  std::string trouble_tank_in = "D:/tests/grouping/trouble-tank-318.tif";
#else // tank 249
  std::string in_path = "D:/tests/grouping/three-tanks-249.tif";
  //std::string in_path = "D:/tests/grouping/rajaei-cargo-2x-mtanks_249.tif";
  //std::string in_path = "D:/tests/grouping/rajaei-crates-2x_249.tif";
  //std::string in_path = "D:/tests/grouping/rajaei-cargo-2x-trouble-crates_249.tif";
  std::string out_path = "D:/tests/grouping/test_rajaei_color_mss_249.tif";
  std::string bright_region_path = "D:/tests/grouping/rajaei_bright_regions_249.txt";
  std::string bright_region_cluster_path = "D:/tests/grouping/rajaei_bright_clusters_249.txt";
  std::string three_tanks_bright_region_path = "D:/tests/grouping/three_tanks_bright_regions_249.txt";
  std::string three_tanks_bright_region_cluster_path = "D:/tests/grouping/three_tanks_bright_clusters_249.txt";
#endif
  vil_image_view<vxl_byte> imgi = vil_load(in_path.c_str());
  //  vil_image_view<vxl_byte> imgi = vil_load(trouble_tank_in.c_str());
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
  proc.weight_thresh_ = 30.0f;
  std::cout << "==========weight thresh = " <<30.0f << std::endl;
  proc.process();
  proc.weight_thresh_ = 30.0f;
  proc.four_or_eight_conn_ = 8;
  std::cout << "==========weight thresh = " <<30.0f << " 8 conn" << std::endl;
  proc.process();
  proc.weight_thresh_ = 20.0f;
  proc.four_or_eight_conn_ = 4;
  proc.use_vd_edges_ = false;
  std::cout << "==========weight thresh = " <<20.0f << " 4 conn  vd_edges" << std::endl;
  proc.process();
  sdet_region_classifier clasf;
  clasf.set_diverse_regions(proc.diverse_regions());
  clasf.set_diverse_hists(proc.diverse_hists());
  clasf.compute_hist_of_nbrs();
  clasf.compute_bright_regions();
  const std::map<unsigned, sdet_region_sptr>& regions=clasf.diverse_regions();
  const std::set<unsigned>& bright_regions = clasf.bright_regions();
  const std::map<unsigned, bsta_histogram<float> >& diverse_hists   =  clasf.diverse_hists();
  const std::map<unsigned, bsta_histogram<float> >& neighbors_hists = clasf.neighbors_hists();
  std::map<unsigned, sdet_region_sptr> filtered_regions;
  //std::ofstream os(bright_region_path.c_str());
  std::ofstream os(three_tanks_bright_region_path.c_str());
  float ar = 1.8f, amin = 200.0f, amax = 700.0f;
  for(std::map<unsigned, sdet_region_sptr>::const_iterator rit = regions.begin();
      rit != regions.end(); ++rit){
    std::set<unsigned>::iterator bit = bright_regions.find(rit->first);
    if(bit == bright_regions.end())
      continue;
    unsigned lab = (*rit).first;
    sdet_region_sptr r = (*rit).second;
    vgl_oriented_box_2d<float> obox = r->obox();
    if(r->obox_valid()){
      float asp = obox.aspect_ratio();
      if(asp>ar)
        continue;
      float a = vgl_area(obox);
      if(a<amin || a>amax)
        continue;
      obox.write(os);
	  filtered_regions[lab]=r;
#if 0
      vgl_point_2d<float> cent = obox.center();
      std::cout << lab << ' ' << cent << std::endl; 

      float Io = r->Io();
      if(false&&Io < 120.0){
        std::cout << "\n====>Histograms[" << lab << "] at ("<< r->Xo() << ' ' << r->Yo() << ")" << std::endl; 
        std::map<unsigned, bsta_histogram<float> >::const_iterator hit = diverse_hists.find(lab);
        std::map<unsigned, bsta_histogram<float> >::const_iterator nit = neighbors_hists.find(lab);
        if( hit!=diverse_hists.end()&& nit!=neighbors_hists.end()){
          const bsta_histogram<float>& hr = hit->second;
          const bsta_histogram<float>& hn = nit->second;
          std::cout << "Region Hist " << std::endl;
          hr.print(std::cout);
          std::cout << "/nNeighbor Hist " << std::endl;
          hn.print(std::cout);
        }
      }
#endif
    }
  }
  os.close();
  //std::ofstream bos(bright_region_cluster_path.c_str());
  std::ofstream bos(three_tanks_bright_region_cluster_path.c_str());
  clasf.find_iou_clusters(filtered_regions);
  const std::map< unsigned, std::map<unsigned, float> >& ioucs = clasf.iou_clusters();
  for(std::map< unsigned, std::map<unsigned, float> >::const_iterator iit = ioucs.begin();
      iit != ioucs.end(); ++iit){
    std::map<unsigned, sdet_region_sptr>::const_iterator rit = regions.find(iit->first);    
    if(rit == regions.end())
      continue;
    vgl_oriented_box_2d<float> obox = (rit->second)->obox();    
    std::cout << iit->first << ' ' << obox.center() << std::endl;
    obox.write(bos);
  }
  bos.close();
  clasf.compute_iou_cluster_similarity();
  float csim = clasf.compute_partition_quality(clasf.cluster_sim());
  std::map< unsigned, std::map<unsigned, region_sim> > sim_after, sim_after1, sim_after2;
  unsigned labi = 6868, labj = 6874, lab_new = 15000;
  bool good = clasf.merge_similarity_map(clasf.cluster_sim(), sim_after,labi, labj, lab_new);
  float csim1 = clasf.compute_partition_quality(sim_after);
  labi = 6869; labj = 15000; lab_new = 15001;
  good = clasf.merge_similarity_map(sim_after,sim_after1, labi, labj, lab_new);
  float csim2 = clasf.compute_partition_quality(sim_after1);
  labi = 6864; labj = 13780; lab_new = 15002;
  good = clasf.merge_similarity_map(sim_after1,sim_after2, labi, labj, lab_new);
  float csim3 = clasf.compute_partition_quality(sim_after2);
#if 0 //show histograms
  for(std::map<unsigned, sdet_region_sptr>::iterator fit0 = filtered_regions.begin();
	  fit0 != filtered_regions.end(); ++fit0)
	  for(std::map<unsigned, sdet_region_sptr>::iterator fit1= filtered_regions.begin();
	  fit1 != filtered_regions.end(); ++fit1){
            unsigned lab0 = fit0->first, lab1 = fit1->first;
            if(lab0 == lab1)
              continue;
            std::map<unsigned, bsta_histogram<float> >::const_iterator hit0 = diverse_hists.find(lab0);
            if(hit0 == diverse_hists.end())
              continue;
            std::map<unsigned, bsta_histogram<float> >::const_iterator hit1 = diverse_hists.find(lab1);
            if(hit1 == diverse_hists.end())
              continue;
            const bsta_histogram<float>& h0 = hit0->second;
            const bsta_histogram<float>& h1 = hit1->second;
            float s = similarity(fit0->second, h0, fit1->second, h1);
            float iou = obox_int_over_union((fit0->second)->obox(), (fit1->second)->obox());
            if(iou == 0.0f && s>1.0f)
            std::cout << lab0 << ' ' << lab1 << ' ' << s << std::endl;
	  }
#endif //show histograms
#endif //test_tanks
#endif // do tests
}

TESTMAIN_ARGS(test_region_classifier);
