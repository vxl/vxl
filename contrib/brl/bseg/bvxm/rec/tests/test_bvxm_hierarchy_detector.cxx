#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_vector.h>

#include <rec/bvxm_part_base.h>
#include <rec/bvxm_part_base_sptr.h>
#include <rec/bvxm_part_hierarchy.h>
#include <rec/bvxm_part_hierarchy_sptr.h>
#include <rec/bvxm_hierarchy_edge.h>
#include <rec/bvxm_hierarchy_edge_sptr.h>
#include <rec/bvxm_part_hierarchy_builder.h>
#include <rec/bvxm_part_hierarchy_detector.h>

#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>

#include <brip/brip_vil_float_ops.h>

#include <rec/bvxm_part_gaussian.h>
#include <bsta/algo/bsta_gaussian_updater.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_quaternion.h>

static void test_bvxm_hierarchy_detector()
{
  //  test C class for rtree
  vgl_rtree<V_type, B_type, C_type> trc; // the rtree

  // insert  points
  //vgl_point_2d<float> p0(0.0f, 0.1f), p1(1.0f,0.0f),
    //p2(1.0f, 1.0f), p3(0.5f, 0.5f), p4(0.65f, 0.65f);
  bvxm_part_instance_sptr p0i = new bvxm_part_instance(0, 0, 1, 0.0f, 0.0f, 0.1f);
  bvxm_part_instance_sptr p1i = new bvxm_part_instance(0, 0, 1, 1.0f, 0.0f, 0.1f);
  bvxm_part_instance_sptr p2i = new bvxm_part_instance(0, 0, 1, 1.0f, 1.0f, 0.1f);
  bvxm_part_instance_sptr p3i = new bvxm_part_instance(0, 0, 1, 0.5f, 0.5f, 0.1f);
  bvxm_part_instance_sptr p4i = new bvxm_part_instance(0, 0, 1, 0.65f, 0.65f, 0.1f);
  trc.add(p0i);   trc.add(p1i);   trc.add(p2i);   trc.add(p3i); trc.add(p4i);

  // test contains method
  bool p1_in = trc.contains(p1i);
  TEST("test contains", p1_in, true);

  // test region search
  vgl_box_2d<float> bbc;
  vgl_point_2d<float> pb0c(0.25f, 0.25f), pb1c(0.75f, 0.75f);
  bbc.add(pb0c); bbc.add(pb1c);
  vcl_vector<bvxm_part_instance_sptr> foundc;
  trc.get(bbc, foundc);

  TEST("test region search",foundc[0] , p3i);
  //test iterator
  vcl_cout << "Traversing point_box rtree, num nodes = "<< trc.nodes()<< '\n';

  //vcl_string file = "normalized0_cropped.tif";
  //vcl_string file = "normalized0_cropped_rot_30.tif";
  //vcl_string file = "digits_small.TIF";
  //vcl_string file = "normalized1.png";
  vcl_string file = "normalized1_crop2.png";

  vil_image_resource_sptr img = vil_load_image_resource(file.c_str());
  TEST("test load img", !img, false);

  if (!img)
    return;

  unsigned ni = img->ni(); unsigned nj = img->nj();

  vcl_cout << "image ni: " << ni << " nj: " << nj << vcl_endl;

  //bvxm_part_hierarchy_sptr h = bvxm_part_hierarchy_builder::construct_detector_roi1_2();
  //bvxm_part_hierarchy_sptr h = bvxm_part_hierarchy_builder::construct_eight_detector();
  bvxm_part_hierarchy_sptr h = bvxm_part_hierarchy_builder::construct_detector_roi1_0();
  vcl_ofstream os("roi1_0_detector.xml");
  h->write_xml(os);
  os.close();

  bvxm_part_hierarchy_sptr h_dummy = new bvxm_part_hierarchy();
  vcl_ifstream is("roi1_0_detector.xml");
  bool read_xml_fine = h_dummy->read_xml(is);
  TEST("test reading from xml", read_xml_fine, true);
  is.close();
  TEST("test reading from xml", h_dummy->dummy_primitive_instances_.size(), h->dummy_primitive_instances_.size());
  vcl_ofstream os2("roi1_0_detector_test.xml");
  h_dummy->write_xml(os2);
  os2.close();

  bvxm_part_hierarchy_detector hd(h);
  //hd.detect(img, 90.0f);
  hd.detect(img);

  vcl_vector<bvxm_part_instance_sptr> parts_prims = hd.get_parts(0);
  vcl_cout << "\t extracted " << parts_prims.size() << " primitives\n";

  // test vgl_rtree_point_box_2d on parts_prims
  typedef vgl_rtree_point_box_2d<float> C_; // the helper class
  typedef C_::v_type V_; // the contained object type
  typedef C_::b_type B_; // the bounding object type
  vgl_rtree<V_, B_, C_> tr_vxl; // the rtree

  vgl_point_2d<float> p0(parts_prims[0]->x_, parts_prims[0]->y_);
  tr_vxl.add(p0);
  for (unsigned i = 1; i < parts_prims.size(); i++) {
    vgl_point_2d<float> p1(parts_prims[i]->x_, parts_prims[i]->y_);
    tr_vxl.add(p1);
  }

  // test contains method
  bool p0_in = tr_vxl.contains(p0);
  TEST("test contains", p0_in, true);

  vcl_cout << "test use of rtree\n";
  Rtree_type* tr = hd.get_tree(0); // the rtree
  p1_in = tr->contains(parts_prims[0]);
  TEST("test contains", p1_in, true);
  for (unsigned i = 1; i < parts_prims.size(); i++)
    p1_in = p1_in && tr->contains(parts_prims[i]);
  TEST("test contains", p1_in, true);

  // test region search
  vgl_box_2d<float> bb;
  vgl_point_2d<float> pb0(parts_prims[0]->x_, parts_prims[0]->y_), pb1(parts_prims[1]->x_, parts_prims[1]->y_);
  bb.add(pb0); bb.add(pb1);
  vcl_vector<bvxm_part_instance_sptr> found;
  tr->get(bb, found);
  unsigned n = found.size();
  //for (unsigned i = 0; i<n; ++i)
  //  vcl_cout << "point(s) in region " << found[i] << '\n';;
  //TEST("test region search", n, 2);
  if (n>=2) {
    TEST("test region search",found[0] , parts_prims[0]);
    TEST("test region search",found[1] , parts_prims[1]);
  }

  unsigned ii = 0;
  for (unsigned i = 0; i < parts_prims.size(); i++) {
    if (parts_prims[i]->x_ == 393 && parts_prims[i]->y_ == 198) {
      ii = i;
      break;
    }
  }

  vcl_cout << " part found at id: " << ii << vcl_endl;

  p1_in = tr->contains(parts_prims[ii]);
  TEST("test a specific part", p1_in, true);
  vgl_box_2d<float> bb2;
  pb0.set(383, 189); pb1.set(389, 194);
  vgl_point_2d<float> pb2(393, 198);//, pb3(370, 150), pb4(400,200);
  bb2.add(pb0);
  bb2.add(pb1);
  bb2.add(pb2); //bb2.add(pb3);
  //bb2.add(pb3);
  //bb2.add(pb4);
  bool dummy = bb2.contains(pb1);
  bool dummy1 = bb2.contains(pb0);
  bool dummy2 = bb2.contains(pb2);
  //pb0.set(393, 198);
  //bb2.add(pb0);
  found.clear();
  //tr->get(bb2, found);
  //tr->print();

  vcl_vector<vgl_point_2d<float> > found2;
  tr_vxl.get(bb2, found2);
  vcl_cout << "-----found: " << found2.size() << " points-----\n";

  unsigned highest = h->highest_layer_id();
  vcl_vector<bvxm_part_instance_sptr> parts_upper_most = hd.get_parts(highest);
  vcl_cout << "\t extracted " << parts_upper_most.size() << " parts from highest layer: " << highest << vcl_endl;

  vil_image_view<float> output_map_float(ni, nj);
  bvxm_part_hierarchy::generate_output_map(parts_upper_most, output_map_float);
  float min, max;
  vil_math_value_range(output_map_float, min, max);
  vcl_cout << "\toutput map float value range, min: " << min << " max: " << max << vcl_endl;

  vil_image_view<vxl_byte> output_map_byte(ni, nj);
  vil_convert_stretch_range_limited(output_map_float, output_map_byte, 0.0f, 1.0f);
  vil_save(output_map_byte, "./map_output_receptive_field_highest_detector.png");

  vil_image_view<vxl_byte> output_img(ni, nj, 3);
  vil_image_view<vxl_byte> input_img = img->get_view(0, ni, 0, nj);
  bvxm_part_hierarchy::generate_output_img(parts_upper_most, input_img, output_img);
  vil_save(output_img, "./img_output_receptive_field_highest_detector.png");

  //test rotation with a quaternion
  //vnl_vector_fixed<float, 2> v = pp->direction_vector();  // get orientation vector of central part: pi
  vnl_vector_fixed<float,2> v = parts_prims[0]->direction_vector();
  vcl_cout << " parts prims theta: " << parts_prims[0]->cast_to_gaussian()->theta_ << vcl_endl
           << " direction vector 0: " << v[0] << " 1: " << v[1] << vcl_endl;

  // define a rotation about z axis (in the image plane)
  vnl_quaternion<float> q(0.0f, 0.0f, float(vnl_math::pi/2.0));

  vnl_vector_fixed<float,3> v3d(v[0], v[1], 0.0f);
  vnl_vector_fixed<float,3> out = q.rotate(v3d);
  vcl_cout << " direction vector after rotation by 90 degrees 0: " << out[0] << " 1: " << out[1] << " 2: " << out[2] << vcl_endl;
  vnl_vector_fixed<float,3> out_dist = out*5.0f;
  vcl_cout << " direction vector after scaling by 5: " << out_dist[0] << " 1: " << out_dist[1] << vcl_endl;
}

TESTMAIN( test_bvxm_hierarchy_detector );
