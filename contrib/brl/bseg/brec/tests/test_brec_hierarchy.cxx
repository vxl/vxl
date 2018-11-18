#include <iostream>
#include <vector>
#include <testlib/testlib_test.h>

#include <brec/brec_part_base.h>
#include <brec/brec_part_base_sptr.h>
#include <brec/brec_part_hierarchy.h>
#include <brec/brec_part_hierarchy_sptr.h>
#include <brec/brec_hierarchy_edge.h>
#include <brec/brec_hierarchy_edge_sptr.h>
#include <brec/brec_part_hierarchy_builder.h>

#include <brip/brip_vil_float_ops.h>

#include <brec/brec_part_gaussian.h>
#include <bsta/algo/bsta_gaussian_updater.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_brec_hierarchy()
{
  // first create a hierarchy
  brec_part_hierarchy_sptr h = new brec_part_hierarchy();

  brec_part_base_sptr p_0_0 = new brec_part_base(0, 0);
  brec_part_gaussian_sptr pi_0_0 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, -45.0f, false, 0);
  brec_part_base_sptr p_0_1 = new brec_part_base(0, 1);
  brec_part_gaussian_sptr pi_0_1 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 3.0f, 1.0f, -45.0f, false, 1);
  brec_part_base_sptr p_0_2 = new brec_part_base(0, 2);
  brec_part_gaussian_sptr pi_0_2 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 1.0f, 3.0f, -45.0f, false, 2);
  brec_part_base_sptr p_0_3 = new brec_part_base(0, 3);
  brec_part_gaussian_sptr pi_0_3 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 2.0f, 2.0f, -45.0f, false, 3);
  h->add_vertex(p_0_0); h->add_vertex(p_0_1); h->add_vertex(p_0_2); h->add_vertex(p_0_3);
  h->add_dummy_primitive_instance(pi_0_0->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_1->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_2->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_3->cast_to_instance());

  brec_part_base_sptr p_1_0 = new brec_part_base(1, 0); p_1_0->prior_prob_ = 1.0;
  brec_part_base_sptr p_1_1 = new brec_part_base(1, 1); p_1_1->prior_prob_ = 1.0;
  brec_part_base_sptr p_1_2 = new brec_part_base(1, 2); p_1_2->prior_prob_ = 1.0;
  brec_part_base_sptr p_1_3 = new brec_part_base(1, 3); p_1_3->prior_prob_ = 1.0;
  h->add_vertex(p_1_0); h->add_vertex(p_1_1); h->add_vertex(p_1_2); h->add_vertex(p_1_3);

  brec_hierarchy_edge_sptr e;

  e = new brec_hierarchy_edge(p_1_0, p_0_0, true);
  h->add_edge_no_check(e); p_1_0->add_outgoing_edge(e); p_0_0->add_incoming_edge(e);
  e = new brec_hierarchy_edge(p_1_0, p_0_1, false);
  h->add_edge_no_check(e); p_1_0->add_outgoing_edge(e); p_0_1->add_incoming_edge(e);
  bsta_gaussian_sphere<double, 1> dm(1.0, 0.5);  bsta_gaussian_sphere<double, 1> am(0.0, 0.1);
  e->set_model(dm, am, 1.0);

  e = new brec_hierarchy_edge(p_1_1, p_0_0, true);
  h->add_edge_no_check(e); p_1_1->add_outgoing_edge(e); p_0_0->add_incoming_edge(e);
  e = new brec_hierarchy_edge(p_1_1, p_0_1, false);
  h->add_edge_no_check(e); p_1_1->add_outgoing_edge(e); p_0_1->add_incoming_edge(e);
  dm.set_mean(3.0); am.set_mean(vnl_math::pi/4.0); e->set_model(dm, am, 1.0);

  e = new brec_hierarchy_edge(p_1_2, p_0_2, true); h->add_edge_no_check(e);
  h->add_edge_no_check(e); p_1_2->add_outgoing_edge(e); p_0_2->add_incoming_edge(e);
  e = new brec_hierarchy_edge(p_1_2, p_0_3, false); h->add_edge_no_check(e);
  h->add_edge_no_check(e); p_1_2->add_outgoing_edge(e); p_0_3->add_incoming_edge(e);
  dm.set_mean(2.0); am.set_mean(vnl_math::pi/8.0); e->set_model(dm, am, 1.0);

  e = new brec_hierarchy_edge(p_1_3, p_0_2, true); h->add_edge_no_check(e);
  h->add_edge_no_check(e); p_1_3->add_outgoing_edge(e); p_0_2->add_incoming_edge(e);
  e = new brec_hierarchy_edge(p_1_3, p_0_3, false); h->add_edge_no_check(e);
  h->add_edge_no_check(e); p_1_3->add_outgoing_edge(e); p_0_3->add_incoming_edge(e);
  dm.set_mean(10.0); am.set_mean(vnl_math::pi/2.0); e->set_model(dm, am, 1.0);

  brec_part_base_sptr p_2_0 = new brec_part_base(2, 0); p_2_0->prior_prob_ = 1.0;
  brec_part_base_sptr p_2_1 = new brec_part_base(2, 1); p_2_1->prior_prob_ = 1.0;
  h->add_vertex(p_2_0); h->add_vertex(p_2_1);

  e = new brec_hierarchy_edge(p_2_0, p_1_0, true);
  h->add_edge_no_check(e); p_2_0->add_outgoing_edge(e); p_1_0->add_incoming_edge(e);
  e = new brec_hierarchy_edge(p_2_0, p_1_2, false);
  h->add_edge_no_check(e); p_2_0->add_outgoing_edge(e); p_1_2->add_incoming_edge(e);
  dm.set_mean(10.0); am.set_mean(vnl_math::pi/2.0); e->set_model(dm, am, 1.0);

  e = new brec_hierarchy_edge(p_2_1, p_1_1, true);
  h->add_edge_no_check(e); p_2_1->add_outgoing_edge(e); p_1_1->add_incoming_edge(e);
  e = new brec_hierarchy_edge(p_2_1, p_1_3, false);
  h->add_edge_no_check(e); p_2_1->add_outgoing_edge(e); p_1_3->add_incoming_edge(e);
  dm.set_mean(10.0); am.set_mean(vnl_math::pi/2.0); e->set_model(dm, am, 1.0);

  h->draw_to_ps(1, "./out.ps", 10.0f);

  // create an instance as if detected to calculate a score for it in the test hierarchy
  brec_part_gaussian_sptr pt_0_0 = new brec_part_gaussian(10.0f, 0.0f, 0.0f, 1.0f, 1.0f, -45.0f, false, 0);
  brec_part_gaussian_sptr pt_0_1 = new brec_part_gaussian(13.0f, 0.0f, 0.0f, 3.0f, 1.0f, -45.0f, false, 1);
  brec_part_gaussian_sptr pt_0_2 = new brec_part_gaussian(20.0f, 0.0f, 0.0f, 1.0f, 3.0f, -45.0f, false, 2);
  brec_part_gaussian_sptr pt_0_3 = new brec_part_gaussian(30.0f, 0.0f, 0.0f, 2.0f, 2.0f, -45.0f, false, 3);

  brec_part_instance_sptr pt_1_0 = new brec_part_instance(1, 10, brec_part_instance_kind::COMPOSED, 10.0f, 0.0f, 0.0f);
  e = new brec_hierarchy_edge(pt_1_0->cast_to_base(), pt_0_0->cast_to_base(), true); pt_1_0->add_outgoing_edge(e); pt_0_0->add_incoming_edge(e);
  e = new brec_hierarchy_edge(pt_1_0->cast_to_base(), pt_0_1->cast_to_base(), false); pt_1_0->add_outgoing_edge(e); pt_0_1->add_incoming_edge(e);

  brec_part_instance_sptr pt_1_1 = new brec_part_instance(1, 15, brec_part_instance_kind::COMPOSED, 20.0f, 0.0f, 0.0f);
  e = new brec_hierarchy_edge(pt_1_1->cast_to_base(), pt_0_2->cast_to_base(), true); pt_1_1->add_outgoing_edge(e); pt_0_2->add_incoming_edge(e);
  e = new brec_hierarchy_edge(pt_1_1->cast_to_base(), pt_0_3->cast_to_base(), false); pt_1_1->add_outgoing_edge(e); pt_0_3->add_incoming_edge(e);

  brec_part_instance_sptr pt_2_0 = new brec_part_instance(2, 30, brec_part_instance_kind::COMPOSED, 10.0f, 0.0f, 0.0f);
  e = new brec_hierarchy_edge(pt_2_0->cast_to_base(), pt_1_0->cast_to_base(), true); pt_2_0->add_outgoing_edge(e); pt_1_0->add_incoming_edge(e);
  e = new brec_hierarchy_edge(pt_2_0->cast_to_base(), pt_1_1->cast_to_base(), false); pt_2_0->add_outgoing_edge(e); pt_1_1->add_incoming_edge(e);

  std::vector<double> scores;
#if 0 // ????? !!! TODO
  bool result = h->get_score(pt_2_0, scores);
  TEST("hierarchy get_score() " , result, true);
#endif // 0
  std::cout << "there are " << scores.size() << " similar parts in hierarchy, with scores:\n";
  for (unsigned i = 0; i < scores.size(); i++) {
    std::cout << '\t' << scores[i] << '\n';
  }

  // now create an instance which should not be found in the hierarchy
  brec_part_gaussian_sptr pt2_0_0 = new brec_part_gaussian(10.0f, 0.0f, 0.0f, 1.0f, 1.0f, -45.0f, false, 0);
  brec_part_gaussian_sptr pt2_0_1 = new brec_part_gaussian(13.0f, 0.0f, 0.0f, 3.0f, 1.0f, -45.0f, false, 1);
  brec_part_gaussian_sptr pt2_0_2 = new brec_part_gaussian(20.0f, 0.0f, 0.0f, 1.0f, 3.0f, -45.0f, false, 2);
  brec_part_gaussian_sptr pt2_0_3 = new brec_part_gaussian(30.0f, 0.0f, 0.0f, 2.0f, 2.0f, -45.0f, false, 5);

  brec_part_instance_sptr pt2_1_0 = new brec_part_instance(1, 10, brec_part_instance_kind::COMPOSED, 10.0f, 0.0f, 0.0f);
  e = new brec_hierarchy_edge(pt2_1_0->cast_to_base(), pt2_0_0->cast_to_base(), true); pt2_1_0->add_outgoing_edge(e); pt2_0_0->add_incoming_edge(e);
  e = new brec_hierarchy_edge(pt2_1_0->cast_to_base(), pt2_0_1->cast_to_base(), false); pt2_1_0->add_outgoing_edge(e); pt2_0_1->add_incoming_edge(e);

  brec_part_instance_sptr pt2_1_1 = new brec_part_instance(1, 15, brec_part_instance_kind::COMPOSED, 20.0f, 0.0f, 0.0f);
  e = new brec_hierarchy_edge(pt2_1_1->cast_to_base(), pt2_0_2->cast_to_base(), true); pt2_1_1->add_outgoing_edge(e); pt2_0_2->add_incoming_edge(e);
  e = new brec_hierarchy_edge(pt2_1_1->cast_to_base(), pt2_0_3->cast_to_base(), false); pt2_1_1->add_outgoing_edge(e); pt2_0_3->add_incoming_edge(e);

  brec_part_instance_sptr pt2_2_0 = new brec_part_instance(2, 30, brec_part_instance_kind::COMPOSED, 10.0f, 0.0f, 0.0f);
  e = new brec_hierarchy_edge(pt2_2_0->cast_to_base(), pt2_1_0->cast_to_base(), true); pt2_2_0->add_outgoing_edge(e); pt2_1_0->add_incoming_edge(e);
  e = new brec_hierarchy_edge(pt2_2_0->cast_to_base(), pt2_1_1->cast_to_base(), false); pt2_2_0->add_outgoing_edge(e); pt2_1_1->add_incoming_edge(e);

  scores.clear();
#if 0 // ????? !!! TODO
  result = h->get_score(pt2_2_0, scores);
  TEST("hierarchy get_score() ", result, !true);
#endif // 0

  std::cout << "there are " << scores.size() << " similar parts in hierarchy, with scores:\n";
  for (unsigned i = 0; i < scores.size(); i++) {
    std::cout << '\t' << scores[i] << '\n';
  }

  scores.clear();

#if 0 // ????? !!! TODO
  result = h->get_score(pt2_1_0, scores);
  TEST("hierarchy get_score() ", result, true);
#endif // 0
  std::cout << "there are " << scores.size() << " similar parts in hierarchy, with scores:\n";
  for (unsigned i = 0; i < scores.size(); i++) {
    std::cout << '\t' << scores[i] << '\n';
  }

  scores.clear();
#if 0 // ????? !!! TODO
  result = h->get_score(pt2_1_1, scores);
  TEST("hierarchy get_score() ", result, !true);
#endif // 0
}

TESTMAIN( test_brec_hierarchy );
