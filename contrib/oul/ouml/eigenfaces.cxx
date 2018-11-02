// This is oul/ouml/eigenfaces.cxx
#include <queue>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstring>
#include "eigenfaces.h"
//:
// \file

#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vil1/vil1_save.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>


//----------------------------------------------------------------------
//: Destructor
//
// Scans through training_images and eigenvectors and deletes the
// elements thereof.
//
// \status under development
// \author Brendan McCane
//----------------------------------------------------------------------

EigenFace::~EigenFace()
{
  delete average_training_image;

  // delete contents of the training images vector
  std::vector<vnl_vector<double> *>::iterator iter;
  for (iter=training_images.begin();
       iter!=training_images.end(); iter++)
    delete *iter;

  // delete contents of the eigenvectors
  for (iter=eigenvectors.begin(); iter!=eigenvectors.end(); iter++)
    delete *iter;

  // delete contents of the encoded_training_images
  for (iter=encoded_training_images.begin();
       iter!=encoded_training_images.end(); iter++)
    delete *iter;

  // delete contents of the training labels
  std::vector<char *>::iterator citer;
  for (citer=training_labels.begin(); citer!=training_labels.end(); citer++)
    delete *citer;
}

//----------------------------------------------------------------------
//: Add a training image to the list of training images
//
// Converts the input training image into a vector first, then adds
// this vector to the list.
//
// \param im    : the image to add
// \param label : the class label for the image
// \returns bool: success or otherwise of the insertion procedure
//
// \status under development
// \author Brendan McCane
//----------------------------------------------------------------------

bool EigenFace::add_training_image(Image *im, const char * label)
{
  // precondition
  assert(im!=nullptr);
  vnl_vector<double> *image_vector = convert_image_to_vector(im);
  if (!image_vector) return false;
  if (image_size==0)
    image_size = image_vector->size();
  if (image_size!=image_vector->size())
  {
    std::cerr << "Error adding training image\n"
             << "Image of incorrect size\n";
    return false;
  }
  training_images.push_back(image_vector);
  training_labels.push_back(strdup(label));
  // postconditions: elements actually inserted
  assert(training_images.back()==image_vector);
  assert(std::strcmp(training_labels.back(),label)==0);
  return true;
}

//----------------------------------------------------------------------
//: get_eigenvector
//
// Returns the requested eigenvector if it exists, NULL if it doesn't.
//
// \param i: the index of the requested eigenvector
// \returns vnl_vector<double>: the requested eigenvector
//
// \status under development
// \author Brendan McCane
//----------------------------------------------------------------------

vnl_vector<double> *EigenFace::get_eigenvector(int i)
{
  assert(i>=0);
  if ((unsigned int)i>=eigenvectors.size())
  {
    std::cerr << "Requesting eigenvector, " << i << " which doesn't exist\n"
             << "Number of eigenvectors is: " << eigenvectors.size() << std::endl;
    return nullptr;
  }
  return eigenvectors[i];
}

//----------------------------------------------------------------------
//: get_eigenvalue
//
// Returns the requested eigenvalue if it exists, 0 if it doesn't.
//
// \param i: the index of the requested eigenvector
// \returns double: the requested eigenvalue
//
// \status under development
// \author Brendan McCane
//----------------------------------------------------------------------

double EigenFace::get_eigenvalue(int i)
{
  assert(i>=0);
  if ((unsigned int)i>=eigenvalues.size())
  {
    std::cerr << "Requesting eigenvalue, " << i << " which doesn't exist\n"
             << "Number of eigenvalues is: " << eigenvalues.size() << std::endl;
    return 0.0;
  }
  return eigenvalues[i];
}

void EigenFace::cleanup()
{
  // first clean up any old faces
  if (!average_training_image)
    average_training_image = new vnl_vector<double>(training_images[0]->size(), 0.0);
  else average_training_image->fill(0.0);
  // delete contents of the eigenvectors
  // need to sort this out - all should be deleted prior to relearning
#if 0
  vector<vnl_vector<double> *>::iterator iter;
  for (iter=eigenvectors.begin(); iter!=eigenvectors.end(); iter++)
    delete *iter;
  eigenvectors.clear();
  eigenvalues.clear();
  // now remove the encoded training images
  for (iter=encoded_training_images.begin();
       iter!=encoded_training_images.end(); iter++)
  {
    delete *iter;
  }
  encoded_training_images.clear();
#endif
}
//----------------------------------------------------------------------
//: calculate_eigenfaces
//
// Actually perform the eigenfaces calculation. Returns true on
// success, false on failure. This will populate the eigenvectors and
// eigenvalues vectors.
//
// \status under development
// \author Brendan McCane
//----------------------------------------------------------------------

bool EigenFace::calculate_eigenfaces()
{
  if (training_images.size()<=0)
  {
    std::cerr << "No training images\n";
    return false;
  }
  // construct the A matrix from the training vectors
  vnl_matrix<double> A(image_size, training_images.size());

  std::cout << "cleaning up\n";
  cleanup();
  // now calculate the new vectors
  // first calculate the average training image
  std::cout << "Average training image\n"
           << "ati size = " << average_training_image->size() << std::endl;
  for (unsigned int i=0; i<training_images.size(); i++)
  {
    std::cout << "adding training image " << i << " size = "
             << training_images[i]->size() << std::endl;
    *average_training_image += *training_images[i];
  }
  *average_training_image /= (double)training_images.size();
  std::cout << "Populating A\n";
  for (unsigned int i=0; i<training_images.size(); i++)
  {
    vnl_vector<double> *training = training_images[i];
    for (unsigned int j=0; j<training->size(); j++)
      A(j, i) = (*training)[j]-(*average_training_image)[j];
  }

  std::cout << "AtA\n";
  // now build AtA - then find the eigenvectors of this matrix
  vnl_matrix<double> AtA = A.transpose()*A;
  vnl_symmetric_eigensystem<double> eigen(AtA);

  std::cout << "Eigenvectors\n";
  for (unsigned int i=0; i<training_images.size(); i++)
  {
    auto *new_vec = new vnl_vector<double>(image_size);
    *new_vec = A*eigen.get_eigenvector(i);
    //new_vec->normalize();
    eigenvectors.push_back(new_vec);
    eigenvalues.push_back(eigen.get_eigenvalue(i));
  }

  std::cout << "Eigenvalues are:" ;
  std::vector<double>::iterator val_iter;
  for (val_iter=eigenvalues.begin(); val_iter!=eigenvalues.end(); val_iter++)
    std::cout << ' ' << (*val_iter);
  std::cout << "\nEncoding training images\n";
  encode_training_images();
  // should check they are in fact eigenvectors
  return true;
}

void EigenFace::check_training()
{
  std::cout << "Check training image\n";
  if (average_training_image!=nullptr)
    std::cout << "ati size = " << average_training_image->size() << std::endl;
  else
    std::cout << "ati not set\n";
  for (unsigned int i=0; i<training_images.size(); i++)
  {
    std::cout << "training image " << i << " size = "
             << training_images[i]->size() << std::endl;
  }
}

//----------------------------------------------------------------------
//: convert_image_to_vector
//
// Convert the input image to a 1d vector by stacking the rows of the
// image on top of each other (vertically).
//
// \param im: the image to convert
// \returns vnl_vector<double> *: the resultant vector
//
// \status under development
// \author Brendan McCane
//----------------------------------------------------------------------

vnl_vector<double> *EigenFace::convert_image_to_vector(Image *im)
{
  assert(im!=nullptr);
  int index=0;
  auto *new_vec = new vnl_vector<double>(im->width()*im->height());
  for (int j=0; j<im->height(); j++)
    for (int i=0; i<im->width(); i++)
      (*new_vec)[index++] = (double)(*im)(i,j);
  new_vec->normalize();
  return new_vec;
}

//----------------------------------------------------------------------
//: check_eigenvectors
//
// Check to see if the calculated vectors are in fact eigenvectors.
//
// \returns bool: true if eigenvectors, false otherwise
//
// \status under development
// \author Brendan McCane
//----------------------------------------------------------------------

bool EigenFace::check_eigenvectors()
{
  if (eigenvectors.size()<=0) return false;

  std::cout << "Eigenvalues are:" ;
  std::vector<double>::iterator val_iter;
  for (val_iter=eigenvalues.begin(); val_iter!=eigenvalues.end(); val_iter++)
    std::cout << ' ' << (*val_iter);
  std::cout << std::endl;
  std::vector<vnl_vector<double> *>::iterator iter1, iter2;
  for (iter1=eigenvectors.begin(); iter1!=eigenvectors.end(); iter1++)
    for (iter2=iter1+1; iter2!=eigenvectors.end(); iter2++)
      if (!epsilon_equals(dot_product(**iter1, **iter2), 0.0))
      {
        std::cout << "vectors aren't eigenvectors\n"
                 << "offending vectors are: "
                 << '\t' << **iter1 << std::endl
                 << '\t' << **iter2 << std::endl
                 << "dot product is: " << dot_product(**iter1, **iter2) << std::endl;
        return false;
      }
  return true;
}

//----------------------------------------------------------------------
//: save_as_images
//
// Save each of the eigenvectors as an image.
//
// \status under development
// \author Brendan McCane
//----------------------------------------------------------------------

void EigenFace::save_as_images(int width, int height)
{
  assert(width > 0 && height > 0);
  if ((unsigned int)(width*height)!=image_size)
  {
    std::cerr << "width*height must be equal to image size\n"
             << "image size is: " << image_size << std::endl;
    return;
  }

  Image im(width, height);
  std::vector<vnl_vector<double> *>::iterator iter;
  char name[100];
  int index=0;
  for (iter=eigenvectors.begin(); iter!=eigenvectors.end(); iter++)
  {
    double min = (**iter).min_value();
    double max = (**iter).max_value();
    for (int i=0; i<width; i++)
      for (int j=0; j<height; j++)
        im[i][j] = (unsigned char)
          (((**iter)[i+j*width]-min)/(max-min)*255);
    std::sprintf(name, "eigenface%03d.pgm", index++);
    vil1_save(im, name);
  }
}


//----------------------------------------------------------------------
//: encode_training_images
//
// Encode all training images for later classification.
//
// \status under development
// \author Brendan McCane
//----------------------------------------------------------------------
void EigenFace::encode_training_images()
{
  std::vector<vnl_vector<double> *>::iterator iter;
  for (iter=training_images.begin(); iter!=training_images.end(); iter++)
  {
    encoded_training_images.push_back(encode(*iter));
  }
}

//----------------------------------------------------------------------
//: encode
//
// Given an input image, encode it as a vector of weights.
//
// \param im: the input image
// \returns vnl_vector<double>*: the vector of weights
//
// \status under development
// \author Brendan McCane
//----------------------------------------------------------------------

vnl_vector<double>* EigenFace::encode(Image *im)
{
  if (eigenvectors.size()<=0)
  {
    return nullptr;
  }
  vnl_vector<double> *im_vec = convert_image_to_vector(im);
  *im_vec -= *average_training_image;
  auto *wts = new vnl_vector<double>(eigenvectors.size());
  for (unsigned int i=0; i<eigenvectors.size(); i++)
    (*wts)[i] = dot_product(*im_vec, *(eigenvectors[i]));
  delete im_vec;
  return wts;
}

vnl_vector<double>* EigenFace::encode(vnl_vector<double> *t_vec)
{
  if (eigenvectors.size()<=0)
  {
    return nullptr;
  }
  auto *im_vec = new vnl_vector<double>(*t_vec);
  *im_vec -= *average_training_image;
  auto *wts = new vnl_vector<double>(eigenvectors.size());
  for (unsigned int i=0; i<eigenvectors.size(); i++)
    (*wts)[i] = dot_product(*im_vec, *(eigenvectors[i]));
  delete im_vec;
  return wts;
}

//----------------------------------------------------------------------
//: decode
//
// Given an input set of weights, rebuild an image.
//
// \param wts : the input set of weights
// \returns vnl_vector<double>*: the reconstructed image in a vector
//
// \status under development
// \author Brendan McCane
//----------------------------------------------------------------------

vnl_vector<double>* EigenFace::decode(vnl_vector<double> *wts)
{
  vnl_vector<double> *new_im = new vnl_vector<double>(image_size, 0.0);
  for (unsigned int i=0; i<eigenvectors.size(); i++)
    *new_im += ((*wts)[i])*(*(eigenvectors[i]));

  return new_im;
}


//----------------------------------------------------------------------
//: decode
//
// Try to classify the given image using k-nearest neighbours
//
// \param im        the image to classify
// \param k         the number of nearest neighbours to use
// \param dim       the number of dimensions of the eigenvectors to use
// \param threshold distances above this threshold are treated as not recognised
// \return the label of the recognised image or NULL if unsuccessful
//
// \status under development
// \author Brendan McCane
//----------------------------------------------------------------------

char *EigenFace::classify(Image *im, double threshold, int k, int dim)
{
  std::priority_queue<LabelDist> pq;

  if (num_vectors()==0) return nullptr;
  if (eigenvectors.size()==0) return nullptr;

  vnl_vector<double> *all_rep = encode(im);
  vnl_vector<double> rep(all_rep->extract(dim, all_rep->size()-dim));
  vnl_vector<double> diff(rep.size());
  double min_dist = DBL_MAX;
  int best=-1;
  char *ret=nullptr;
#if 0
  std::cout << "rep = " << *rep << std::endl;
#endif
  for (int i=0; i<num_vectors(); i++)
  {
    vnl_vector<double> eigenvect=
      encoded_training_images[i]->extract
      (dim, encoded_training_images[i]->size()-dim);
    // std::cout << "vec " << i << " = " << *eigenvect << std::endl;
    diff = rep - eigenvect;
    double dist=diff.two_norm()/(double)diff.size();
    if ((dist<min_dist)&&(dist!=0.0))
    {
      min_dist=dist;
      best=i;
    }
    LabelDist t(get_label(i), dist);
    pq.push(t);
  }
  delete all_rep;

  std::cout << "min_dist = " << min_dist << " label = " << get_label(best) << std::endl;
  // now need to search through queue and find most likely label
  std::map<char *, int, ImageDatabase::ltstr> nns; // nearest neighbours
  for (int i=0; i<k; i++)
  {
    LabelDist t=pq.top();
    pq.pop();
    if ((t.dist<threshold)&&(t.dist!=0.0))
      nns[t.label]++;
  }
  std::map<char *, int, ImageDatabase::ltstr>::iterator iter;
  int max=0;
  for (iter=nns.begin(); iter!=nns.end(); iter++)
  {
    if ((*iter).second>max)
    {
      max = (*iter).second;
      ret = (*iter).first;
    }
  }
  std::cout << "label = " << ret << " num = " << max << std::endl;
  if (max<k/2+1) ret=nullptr;
#if 0
  char ch;
  std::cin >> ch;
#endif
  return ret;
}

//----------------------------------------------------------------------
//: output_xgobi
//
// Output the data into xgobi format files
//
// \param basefile  the basename of the files
//
// \status under development
// \author Brendan McCane
//----------------------------------------------------------------------

void EigenFace::output_xgobi(char *basefile)
{
  char filenames[200];
  std::sprintf(filenames, "%s.dat", basefile);
  std::ofstream datfile(filenames);
  std::sprintf(filenames, "%s.glyphs", basefile);
  std::ofstream glyphs(filenames);
  std::sprintf(filenames, "%s.row", basefile);
  std::ofstream rowfile(filenames);
  std::map<char*, int, ImageDatabase::ltstr> glyphmap;
  int glyphnum=1;
  for (int i=0; i<num_vectors(); i++)
  {
    datfile << *(encoded_training_images[i]) << std::endl;
    // no glyph associated
    if (glyphmap.find(training_labels[i])==glyphmap.end())
    {
      std::cout << "Adding training_label " << training_labels[i] << std::endl;
      glyphmap[training_labels[i]] = glyphnum;
      glyphnum += 3;
    }
    glyphs << glyphmap[training_labels[i]] << std::endl;
    rowfile << training_labels[i] << std::endl;
  }
  std::map<char *, int, ImageDatabase::ltstr>::iterator iter;
  for (iter=glyphmap.begin(); iter!=glyphmap.end(); iter++)
    std::cout << (*iter).first << std::endl;
  rowfile.close();
  glyphs.close();
  datfile.close();
}
