#include <cmath>
#include <iostream>
#include <algorithm>
#include <testlib/testlib_test.h>
#include <volm/conf/volm_conf_score.h>
#include <volm/conf/volm_conf_buffer.h>
#include <volm/conf/volm_conf_object.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define EPISLON 1E-5

static void test_volm_conf_score()
{
  std::vector<volm_conf_object> landmarks;
  for (unsigned i = 0; i < 5; i++) {
    landmarks.emplace_back(i*0.1f, i*0.2f, i*0.3f, i);
  }
  volm_conf_score_sptr score_sptr = new volm_conf_score(0.12, -vnl_math::pi, landmarks);
  score_sptr->print(std::cout);
  std::cout << score_sptr->landmarks().size() << " landmarks in score:" << std::endl;
  for (auto & i : score_sptr->landmarks())
    i.print(std::cout);
  std::cout << "direction relative to north: " << score_sptr->theta_to_north() << std::endl;
  TEST_NEAR("Testing score value", score_sptr->score(), 0.12, EPISLON);
  TEST_NEAR("Testing angular value", score_sptr->theta_in_deg(), vnl_math::pi / vnl_math::pi_over_180, EPISLON);
  TEST_NEAR("Testing angular value to north", score_sptr->theta_to_north(), vnl_math::pi_over_2 / vnl_math::pi_over_180, EPISLON);

  // Test binary IO
  vsl_b_ofstream os("./volm_conf_score.bin");
  vsl_b_write(os, score_sptr);
  os.close();
  vsl_b_ifstream is("./volm_conf_score.bin");
  volm_conf_score_sptr score_in = new volm_conf_score();
  vsl_b_read(is, score_in);
  TEST_NEAR("Testing binary IO", score_sptr->score(), score_in->score(), EPISLON);
  TEST_NEAR("Testing binary IO", score_sptr->theta(), score_in->theta(), EPISLON);
  bool landmark_same = true;
  for (unsigned i = 0; i < score_in->landmarks().size(); i++)
    landmark_same = landmark_same && score_sptr->landmarks()[i].is_same(score_in->landmarks()[i]);
  TEST("Testing binary IO", landmark_same, true);

  // Test score buffer
  std::cout << " ---------------- Testing score buffer ----------------- " << std::endl;
  float buffer_capacity = 200000.0f*2.0f* sizeof(volm_conf_score) /(1024.0f*1024.0f*1024.0f);  // buffer size that can store 200000 objects
  volm_conf_buffer<volm_conf_score> index(buffer_capacity);
  std::cout << "Unit size of index of volm_conf_object: " << index.unit_size() << std::endl;
  std::cout << "Maximum number of volm_conf_object can be stored in index buffer: " << index.buffer_size() << std::endl;

  // randomly create a vector of volm_conf_score
  vnl_random rnd(19801231);
  std::vector<volm_conf_score> scores;
  unsigned data_size = rnd.lrand32(0,50);
  for (unsigned k = 0; k < data_size; k++)
  {
    std::vector<volm_conf_object> landmarks;
    unsigned size = rnd.lrand32(0,10);
    for (unsigned i = 0; i < size; i++)
      landmarks.emplace_back(rnd.drand32(0.0f, vnl_math::twopi), rnd.drand32(0.0f, 200.0f), rnd.drand32(0.0f, 32.f), rnd.lrand32(0,255));
    volm_conf_score value(rnd.drand32(0.0f, 1.0f), rnd.drand32(0, 2*vnl_math::twopi), landmarks);
    scores.push_back(value);
  }

  // add scores into buffer
  std::string filename = "./volm_conf_buffered_score.bin";
  TEST("Testing unit size of buffered score", index.unit_size(), sizeof(volm_conf_score));
  TEST("Testing initial mode of buffer", index.index_mode(), "NOT_INITIALIZED");
  TEST("Testing buffer size", index.buffer_size(), 200000);
  TEST("Testing initialing buffer for WRITE operation", index.initialize_write(filename), true);
  TEST("Testing initial global id", index.global_current_id(), 0);
  TEST("Testing initial current id", index.current_id(), 0);
  TEST("Testing initial length vector", index.length_vec().empty(), true);
  for (unsigned k = 0; k < data_size; k++)
    TEST("Testing add score to buffer", index.add_to_index(scores[k]), true);
  TEST("Testing finalize buffer", index.finalize(), true);

  // read scores from buffer
  std::vector<volm_conf_score> scores_in;
  float buffer_capacity_in = 1.0f;
  volm_conf_buffer<volm_conf_score> index_in(buffer_capacity_in);
  TEST("Testing initialing buffer for READ operation", index_in.initialize_read(filename), true);
  for (unsigned k = 0; k < data_size; k++)
  {
    volm_conf_score value;
    TEST("Testing get score form buffer", index_in.get_next(value), true);
    scores_in.push_back(value);
  }

  bool is_same = true;
  for (unsigned k = 0; k < data_size; k++) {
    if (k < 10) {
      std::cout << "    input value " << k << ": ";  scores[k].print(std::cout);
      std::cout << "   loaded value " << k << ": ";  scores_in[k].print(std::cout);
      std::cout << "   input landmarks " << k << ": " << scores[k].landmarks().size() << ", loaded landmarks " << k  << ": " << scores_in[k].landmarks().size() << std::endl;
    }
    is_same = is_same && ( std::fabs(scores[k].score()-scores_in[k].score())<EPISLON )
                      && ( std::fabs(scores[k].theta()-scores_in[k].theta())<EPISLON );
    unsigned num_landmarks = scores[k].landmarks().size();
    for (unsigned i = 0; i < num_landmarks; i++)
      is_same = is_same && scores[k].landmarks()[i].is_same(scores_in[k].landmarks()[i]);
  }

  TEST("Testing buffer binary IO", is_same, true);
  return;
}

TESTMAIN( test_volm_conf_score );
