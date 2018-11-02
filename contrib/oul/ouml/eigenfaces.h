// This is oul/ouml/eigenfaces.h
#ifndef OTAGO_eigenfaces__h_INCLUDED
#define OTAGO_eigenfaces__h_INCLUDED
//:
// \file
// \brief A simple eigenfaces class
//
// Copyright (c) 2001 Brendan McCane
// University of Otago, Dunedin, New Zealand
// Reproduction rights limited as described in the COPYRIGHT file.
//----------------------------------------------------------------------

#include <iostream>
#include <vector>
#include <cfloat>// for DBL_MAX
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>
#include <vil1/vil1_memory_image_of.h>
#include "inlines.h"
#include "image_database.h"
#include "io.h"

typedef vil1_memory_image_of<double> Image;

class EigenFace
{
 private:
  unsigned int image_size;
  std::vector<vnl_vector<double> *> training_images;
  std::vector<vnl_vector<double> *> encoded_training_images;
  std::vector<char *> training_labels;
  std::vector<vnl_vector<double> *> eigenvectors;
  std::vector<double> eigenvalues;
  vnl_vector<double> *average_training_image;
  vnl_vector<double> *convert_image_to_vector(Image *im);
  void encode_training_images();
  void cleanup();

 public:
  struct LabelDist
  {
    char *label;
    double dist;
    LabelDist(char *str, double d):label(str), dist(d){}
    bool operator<(const LabelDist &x) const {return dist>x.dist;}
  };

  EigenFace(): image_size(0), average_training_image(nullptr){}
  ~EigenFace();
  bool add_training_image(Image *im, const char * label);
  bool calculate_eigenfaces();
  vnl_vector<double> *get_eigenvector(int i);
  double get_eigenvalue(int i);
  inline char *get_label(unsigned int i){
    if (i<training_labels.size()) return training_labels[i];
    else return nullptr;}
  bool check_eigenvectors();
  void save_as_images(int width, int height);
  vnl_vector<double>* encode(Image *im);
  vnl_vector<double>* encode(vnl_vector<double> *im);
  vnl_vector<double>* decode(vnl_vector<double> *wts);
  inline int num_vectors() const {return training_images.size();}
  char *classify(Image *im, double threshold=DBL_MAX, int k=3, int dim=10);
  void check_training();
  void output_xgobi(char *basefile);
};

// a helper class
class EigenSort
{
 public:
  int eigenindex;
  double weight;
  bool operator<(const EigenSort o1) const
  {return this->weight<o1.weight;}
};


#endif // OTAGO_eigenfaces__h_INCLUDED
