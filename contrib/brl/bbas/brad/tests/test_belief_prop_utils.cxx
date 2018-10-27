#include <fstream>
#include <testlib/testlib_test.h>
#include <brad/brad_belief_prop_utils.h>
#include <vil/vil_load.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

static void test_belief_prop_utils()
{
  //START("test_belief_prop_utils");
  // test belief prop functions
  brad_ray ray;
  ray.init_ray();
  ray.print();
  std:: cout << ' ';

  std::string files[] = {//"03JAN08145014-P1BS-052869788020_01_P006",
                         //"05DEC11150006-P1BS-052869812040_01_P002",
                         //"07JAN06150926-P1BS-052869817030_01_P004",
                         //"07NOV08144023-P1BS-052869842020_01_P002",
                         //"07NOV08144106-P1BS-052869785010_01_P001",
                         //"07NOV21144208-P1BS-052869820030_01_P004",
                         //"07NOV21144210-P1BS-052869820030_01_P005",
                         //"08OCT18145149-P1BS-052869833040_01_P001",
                         "09DEC06145803-P1BS-052869858050_01_P002",
                         "09DEC06145817-P1BS-052869840050_01_P002",
                         "09DEC06145817-P1BS-500060999100_01_P002",
                         "09DEC12143944-P1BS-052869847050_01_P001",
                         "09DEC12143956-P1BS-052869830010_01_P002",
                         "09DEC12143956-P1BS-500060996100_01_P002",
                         "09DEC12144010-P1BS-052869863040_01_P002",
                         "09DEC12144011-P1BS-500060998150_01_P003",
                         "09DEC14150530-P1BS-052869830020_01_P001",
                         "09DEC14150620-P1BS-052869824020_01_P002",
                         "09DEC17145621-P1BS-052869797010_01_P001",
                         "09DEC24145617-P1BS-052315343010_01_P002",
                         "09DEC24145617-P1BS-052869779040_01_P002",
                         "09DEC25150516-P1BS-052869797020_01_P002",
                         "09DEC25150516-P1BS-500060996010_01_P002",
                         "09DEC25150528-P1BS-052869818010_01_P001",
                         "09DEC25150635-P1BS-052869876020_01_P002",
                         "09DEC25150635-P1BS-500060999030_01_P002",
                         "09DEC28145609-P1BS-052869868010_01_P002",
                         "09DEC28145609-P1BS-500060997120_01_P002",
                         "09DEC28145718-P1BS-052869856040_01_P002",
                         "09DEC31144657-P1BS-052869840030_01_P001",
                         "09NOV30151513-P1BS-052869835030_01_P001",
                         "10DEC01145358-P1BS-052869866030_01_P001",
                         "10DEC09150358-P1BS-052869877050_01_P001",
                         "10DEC12145513-P1BS-052869863030_01_P001",
                         "10DEC14151413-P1BS-052869844040_01_P004",
                         "10FEB07150123-P1BS-052869803030_01_P001",
                         "10FEB10145223-P1BS-052869846010_01_P001",
                         "10FEB15150957-P1BS-052869823050_01_P001",
                         "10FEB21145127-P1BS-052869840020_01_P001",
                         "10FEB26150756-P1BS-052869871050_01_P001",
                         "10JAN05150316-P1BS-052869837030_01_P001",
                         "10JAN08145510-P1BS-052869801050_01_P001",
                         "10JAN08145916-P1BS-052869827030_01_P001",
                         "10JAN11144555-P1BS-052869873040_01_P001",
                         "10JAN16150318-P1BS-052869859050_01_P004",
                         "10JAN22144500-P1BS-052869870010_01_P001",
                         "10JAN27150215-P1BS-052869798020_01_P004",
                         "10JAN27150233-P1BS-052869859040_01_P001",
                         "10JAN30145319-P1BS-052869846050_01_P001",
                         "10MAR01145753-P1BS-052869824050_01_P002",
                         "10NOV06150113-P1BS-052869840010_01_P001",
                         "10NOV06150122-P1BS-052869878020_01_P001",
                         "10NOV06150605-P1BS-052869827040_01_P005",
                         "10NOV10151146-P1BS-052869785030_01_P004",
                         "10NOV10151231-P1BS-052869789020_01_P004",
                         "10NOV17150246-P1BS-052869796050_01_P001",
                         "10NOV23150719-P1BS-052869791010_01_P004",
                         "10NOV23150815-P1BS-052869854020_01_P001",
                         "10NOV28150228-P1BS-052869863010_01_P001",
                         "10OCT07145106-P1BS-052869786040_01_P001",
                         "10OCT07150831-P1BS-052869851020_01_P004",
                         "10OCT18145158-P1BS-052869872050_01_P001",
                         "10OCT24150848-P1BS-052869790040_01_P001",
                         "11DEC02151302-P1BS-052869829010_01_P001",
                         "11DEC10151810-P1BS-052869843050_01_P001",
                         "11DEC11145906-P1BS-052869854010_01_P001",
                         "11DEC13150801-P1BS-052869858020_01_P001",
                         "11DEC15150445-P1BS-052869834010_01_P001",
                         "11DEC16145749-P1BS-500060998010_01_P001",
                         "11DEC16145750-P1BS-052869803010_01_P001",
                         "11DEC19151049-P1BS-052869808030_01_P001",
                         "11DEC21151345-P1BS-052869865020_01_P001",
                         "11DEC24150332-P1BS-052869861020_01_P001",
                         "11FEB05145509-P1BS-052869823020_01_P001",
                         "11FEB05145509-P1BS-500060996060_01_P001",
                         "11FEB13150344-P1BS-052869823040_01_P001",
                         "11FEB13150344-P1BS-500060996080_01_P001",
                         "11JAN11150346-P1BS-052869814020_01_P001",
                         "11JAN13150655-P1BS-052869813020_01_P004",
                         "11JAN13150739-P1BS-052869826040_01_P004",
                         "11JAN14145545-P1BS-052869798050_01_P001",
                         "11JAN19151302-P1BS-052869824040_01_P001",
                         "11JAN22150346-P1BS-052869861050_01_P001",
                         "11JAN25145535-P1BS-052869836050_01_P001",
                         "11JAN30151239-P1BS-052869870040_01_P001",
                         "11MAR04151052-P1BS-052869786050_01_P001",
                         "11NOV02151428-P1BS-052869837010_01_P001",
                         "11NOV05150425-P1BS-052869823030_01_P001",
                         "11NOV05150425-P1BS-500060996070_01_P001",
                         "11NOV16150028-P1BS-052869818030_01_P001",
                         "11NOV19151531-P1BS-052869841030_01_P001",
                         "11NOV24145924-P1BS-052869791020_01_P001",
                         "11NOV29152235-P1BS-052869811010_01_P001",
                         "11OCT09145538-P1BS-052869792030_01_P002",
                         "11OCT09145550-P1BS-052869847020_01_P001",
                         "11OCT09145607-P1BS-052869829020_01_P004",
                         "11OCT12150843-P1BS-052869833020_01_P001",
                         "11OCT22151819-P1BS-052869847030_01_P001",
                         "11OCT22151819-P1BS-500060997040_01_P001",
                         "11OCT25150400-P1BS-052869815020_01_P001",
                         "11OCT25150817-P1BS-052869858030_01_P001",
                         "11OCT30152430-P1BS-052869836030_01_P001",
                         "12FEB03145527-P1BS-052869855050_01_P001",
                         "12FEB05152130-P1BS-052869859030_01_P001",
                         "12FEB05152130-P1BS-500060997090_01_P001",
                         "12FEB11150056-P1BS-052869836040_01_P001",
                         "12FEB17145944-P1BS-052869805040_01_P001",
                         "12FEB19150624-P1BS-052869877020_01_P001",
                         "12FEB22145607-P1BS-052869864020_01_P001",
                         "12FEB24152211-P1BS-052869823010_01_P001",
                         "12FEB24152211-P1BS-500060996050_01_P001",
                         "12JAN01150921-P1BS-500060999060_01_P001",
                         "12JAN05151109-P1BS-052869834030_01_P002",
                         "12JAN05151204-P1BS-052869841040_01_P002",
                         "12JAN12150444-P1BS-052869811020_01_P001",
                         "12JAN18150453-P1BS-052869825040_01_P001",
                         "12JAN28151600-P1BS-052869870050_01_P001",
                         "12JAN31145907-P1BS-052869827050_01_P002",
                         "12JAN31145948-P1BS-052869850030_01_P002",
                         "12MAR01150154-P1BS-052869800050_01_P001",
                         "12NOV03145829-P1BS-052894093070_01_P005",
                         "12NOV03145830-P1BS-052894093070_01_P006"};

  std::string meta_dir = "d:/tests/chiletest/metadata_files/";
  std::string imgcam_dir = "d:/tests/chiletest/crop_dir/";
  std::string post = "_bin_2";

  //unsigned n = 121;
  unsigned n = 113;
  std::vector<vbl_smart_ptr<brad_image_metadata> > metadata;
  for(unsigned i =0; i<n; ++i){
    vbl_smart_ptr<brad_image_metadata> meta_ptr = new brad_image_metadata();
    if(!meta_ptr->parse_from_meta_file(meta_dir + files[i]+".IMD"))
      continue;
    metadata.push_back(meta_ptr);
  }
  brad_belief_prop_utils utils;
  utils.set_metadata(metadata);

  unsigned mnv = utils.most_nadir_view();
  utils.force_single_index(mnv);
#if 0
  std::vector<unsigned> ref_views =utils.index(mnv);
  vgl_vector_3d<double> nview = utils.view_dir(mnv);
  for(std::vector<unsigned>::iterator vit = ref_views.begin();
      vit != ref_views.end(); ++vit){
    vgl_vector_3d<double> view = utils.view_dir(*vit);
    double dp = dot_product(nview, view);
    double angle = std::acos(dp)*180.0/3.14159;
    std::cout << "angle[" << *vit << "]: " << angle << '\n';
  }
#endif
  std::vector<vil_image_view<float> > imgs;
  std::vector<vpgl_camera_double_sptr> cams;
  for(unsigned i = 0; i<n; ++i){
    std::string img_file = imgcam_dir + files[i] + post + ".tif";
    std::string cam_file = imgcam_dir + files[i] + post + ".rpb";
    vil_image_view<float> img = vil_load(img_file.c_str());
    vpgl_camera_double_sptr ratcam = read_local_rational_camera<double>(cam_file);
        if(!img || !ratcam){
                std::cout << "null img or cam" << files[i] << '\n';
                continue;
        }
        imgs.push_back(img);
    cams.push_back(ratcam);
  }
  utils.set_images(imgs);
  utils.set_cams(cams);

#if 0
  vgl_point_3d<double>  p0(257.2, 107.9, 19.0);
  vgl_point_3d<double>  p1(334.3,244.5 , 13.6);
  vgl_point_3d<double>  p2(314.4,387.9 , 14.56);
  vgl_point_3d<double>  p2d(316.0, 388.0, 14.56);
  vgl_point_3d<double>  proad(228.7,342.7, 2.9);
  vgl_point_3d<double>  proof(291.9,431.7, 14.56);
  //vgl_point_3d<double>  proof(293,431.7, 14.56);
  float Iray =0.0f;
  vgl_point_3d<double> p = proof;
  utils.pixel_intensity(imgs[mnv], cams[mnv], p, Iray);
  std::cout << "Iray[" << mnv << "]: " << Iray << " RefInts[";
  std::vector<double> ref_intents = utils.ref_intensities(ref_views, p);
  for(std::vector<double>::iterator intit = ref_intents.begin();
      intit != ref_intents.end(); ++intit)
    std::cout << *intit << ' ';
  std::cout << "]\n";
  vnl_vector_fixed<unsigned char, 8> mog3(static_cast<unsigned char>(0));
  vnl_vector_fixed<float, 4> nobs(0.0f);
  utils.update_mog(ref_intents, mog3, nobs, true);
  double pray = utils.p_mog(Iray, mog3);
  std::cout << "P(Iray): " << pray << " mix-> ";
  utils.print_mog(mog3, nobs);
  ///
  for(double dy = 0.0; dy<10.0; dy+=1.0){
    vgl_point_3d<double> pi(p.x(), p.y()+dy, p.z());
    utils.init_zray(Iray, ref_views, pi, -10.0, true);
    utils.zray_pre();
    utils.zray_post();
    utils.update_PinS();
    utils.update_vis();
    utils.print_zray();
    double ed = utils.expected_depth();
    std::cout << "expected depth " << ed << '\n';
  }
#endif
#if 0
  //  double z0 = 9.36;
  double z0 = 15.0;
  vgl_point_2d<double> p0(399.13, 333.68);
  vgl_point_2d<double> p1(117.65, 330.03);
  vgl_point_2d<double> p2(113.3, 583.9);
  vgl_point_2d<double> p3(397.5, 583.98);
  vgl_box_2d<double> bb;
  bb.add(p0);  bb.add(p1);  bb.add(p2);  bb.add(p3);
  utils.compute_depth_map(bb, 1.0, z0);
  std::string depth_path = "d:/tests/chiletest/depth.tif";
  utils.save_depth_map(depth_path);
#endif
#if 0
  utils.project_intensities(p);
  utils.print_intensities();
  utils.test_appearance_update();
#endif
}
TESTMAIN( test_belief_prop_utils );
