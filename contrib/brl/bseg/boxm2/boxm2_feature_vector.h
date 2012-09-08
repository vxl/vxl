#ifndef boxm2_feature_vector_h_
#define boxm2_feature_vector_h_

#include <vcl_iosfwd.h>

class boxm2_feature_vector
{
 public:
  boxm2_feature_vector();
  float get_feature(unsigned int i) const {return features_[i];}
  void set_feature(unsigned int i, float val) {features_[i] = val;}

  static const unsigned SIZE = 7; // 3 for RGB. 3 for surface derivatives and 1 for magnitude
 protected:
  float features_[SIZE];
};

vcl_ostream& operator<<(vcl_ostream &s, boxm2_feature_vector& f);

float prob_correspondence(boxm2_feature_vector& f1, float p1,
                          boxm2_feature_vector& f2, float p2,
                          double feature_sim_variance=0.1);

#endif
