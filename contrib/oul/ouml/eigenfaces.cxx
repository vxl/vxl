// This is oul/ouml/eigenfaces.cxx
#include "eigenfaces.h"
//:
// \file

#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vil1/vil1_save.h>
#include <vcl_queue.h>
#include <vcl_fstream.h>
#include <vcl_cstdio.h> // for sprintf()
#include <vcl_cstring.h>
#include <vcl_cassert.h>


//----------------------------------------------------------------------
//: Destructor
//
// Scans through training_images and eigenvectors and deletes the
// elements thereof.
//
//.status under development
//.author Brendan McCane
//----------------------------------------------------------------------

EigenFace::~EigenFace()
{
  delete average_training_image;

  // delete contents of the training images vector
  vcl_vector<vnl_vector<double> *>::iterator iter;
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
  vcl_vector<char *>::iterator citer;
  for (citer=training_labels.begin(); citer!=training_labels.end(); citer++)
    delete *citer;
}

//----------------------------------------------------------------------
//: Add a training image to the list of training images
//
// Converts the input training image into a vector first, then adds
// this vector to the list.
//
// .param Image *im: the image to add
// .param char *label: the class label for the image
// .ret bool: success or otherwise of the insertion procedure
//
//.status under development
//.author Brendan McCane
//----------------------------------------------------------------------

bool EigenFace::add_training_image(Image *im, const char * label)
{
  // precondition
  assert(im!=NULL);
  vnl_vector<double> *image_vector = convert_image_to_vector(im);
  if (!image_vector) return false;
  if (image_size==0)
    image_size = image_vector->size();
  if (image_size!=image_vector->size())
  {
    vcl_cerr << "Error adding training image\n"
             << "Image of incorrect size\n";
    return false;
  }
  training_images.push_back(image_vector);
  training_labels.push_back(strdup(label));
  // postconditions: elements actually inserted
  assert(training_images.back()==image_vector);
  assert(vcl_strcmp(training_labels.back(),label)==0);
  return true;
}

//----------------------------------------------------------------------
//: get_eigenvector
//
// Returns the requested eigenvector if it exists, NULL if it doesn't.
//
// .param int i: the index of the requested eigenvector
// .ret vnl_vector<double>: the requested eigenvector
//
//.status under development
//.author Brendan McCane
//----------------------------------------------------------------------

vnl_vector<double> *EigenFace::get_eigenvector(int i)
{
  assert(i>=0);
  if ((unsigned int)i>=eigenvectors.size())
  {
    vcl_cerr << "Requesting eigenvector, " << i << " which doesn't exist\n"
             << "Number of eigenvectors is: " << eigenvectors.size() << vcl_endl;
    return NULL;
  }
  return eigenvectors[i];
}

//----------------------------------------------------------------------
//: get_eigenvalue
//
// Returns the requested eigenvalue if it exists, 0 if it doesn't.
//
// .param int i: the index of the requested eigenvector
// .ret double: the requested eigenvalue
//
//.status under development
//.author Brendan McCane
//----------------------------------------------------------------------

double EigenFace::get_eigenvalue(int i)
{
  assert(i>=0);
  if ((unsigned int)i>=eigenvalues.size())
  {
    vcl_cerr << "Requesting eigenvalue, " << i << " which doesn't exist\n"
             << "Number of eigenvalues is: " << eigenvalues.size() << vcl_endl;
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
//.status under development
//.author Brendan McCane
//----------------------------------------------------------------------

bool EigenFace::calculate_eigenfaces()
{
  if (training_images.size()<=0)
  {
    vcl_cerr << "No training images\n";
    return false;
  }
  // construct the A matrix from the training vectors
  vnl_matrix<double> A(image_size, training_images.size());

  vcl_cout << "cleaning up\n";
  cleanup();
  // now calculate the new vectors
  // first calculate the average training image
  vcl_cout << "Average training image\n"
           << "ati size = " << average_training_image->size() << vcl_endl;
  for (unsigned int i=0; i<training_images.size(); i++)
  {
    vcl_cout << "adding training image " << i << " size = "
             << training_images[i]->size() << vcl_endl;
    *average_training_image += *training_images[i];
  }
  *average_training_image /= (double)training_images.size();
  vcl_cout << "Populating A\n";
  for (unsigned int i=0; i<training_images.size(); i++)
  {
    vnl_vector<double> *training = training_images[i];
    for (unsigned int j=0; j<training->size(); j++)
      A(j, i) = (*training)[j]-(*average_training_image)[j];
  }

  vcl_cout << "AtA\n";
  // now build AtA - then find the eigenvectors of this matrix
  vnl_matrix<double> AtA = A.transpose()*A;
  vnl_symmetric_eigensystem<double> eigen(AtA);

  vcl_cout << "Eigenvectors\n";
  for (unsigned int i=0; i<training_images.size(); i++)
  {
    vnl_vector<double> *new_vec = new vnl_vector<double>(image_size);
    *new_vec = A*eigen.get_eigenvector(i);
    //new_vec->normalize();
    eigenvectors.push_back(new_vec);
    eigenvalues.push_back(eigen.get_eigenvalue(i));
  }

  vcl_cout << "Eigenvalues are:" ;
  vcl_vector<double>::iterator val_iter;
  for (val_iter=eigenvalues.begin(); val_iter!=eigenvalues.end(); val_iter++)
    vcl_cout << ' ' << (*val_iter);
  vcl_cout << "\nEncoding training images\n";
  encode_training_images();
  // should check they are in fact eigenvectors
  return true;
}

void EigenFace::check_training()
{
  vcl_cout << "Check training image\n";
  if (average_training_image!=NULL)
    vcl_cout << "ati size = " << average_training_image->size() << vcl_endl;
  else
    vcl_cout << "ati not set\n";
  for (unsigned int i=0; i<training_images.size(); i++)
  {
    vcl_cout << "training image " << i << " size = "
             << training_images[i]->size() << vcl_endl;
  }
}

//----------------------------------------------------------------------
//: convert_image_to_vector
//
// Convert the input image to a 1d vector by stacking the rows of the
// image on top of each other (vertically).
//
// .param Image *im: the image to convert
// .ret vnl_vector<double> *: the resultant vector
//
//.status under development
//.author Brendan McCane
//----------------------------------------------------------------------

vnl_vector<double> *EigenFace::convert_image_to_vector(Image *im)
{
  assert(im!=NULL);
  int index=0;
  vnl_vector<double> *new_vec = new vnl_vector<double>(im->width()*im->height());
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
// .ret bool: true if eigenvectors, false otherwise
//
//.status under development
//.author Brendan McCane
//----------------------------------------------------------------------

bool EigenFace::check_eigenvectors()
{
  if (eigenvectors.size()<=0) return false;

  vcl_cout << "Eigenvalues are:" ;
  vcl_vector<double>::iterator val_iter;
  for (val_iter=eigenvalues.begin(); val_iter!=eigenvalues.end(); val_iter++)
    vcl_cout << ' ' << (*val_iter);
  vcl_cout << vcl_endl;
  vcl_vector<vnl_vector<double> *>::iterator iter1, iter2;
  for (iter1=eigenvectors.begin(); iter1!=eigenvectors.end(); iter1++)
    for (iter2=iter1+1; iter2!=eigenvectors.end(); iter2++)
      if (!epsilon_equals(dot_product(**iter1, **iter2), 0.0))
      {
        vcl_cout << "vectors aren't eigenvectors\n"
                 << "offending vectors are: "
                 << '\t' << **iter1 << vcl_endl
                 << '\t' << **iter2 << vcl_endl
                 << "dot product is: " << dot_product(**iter1, **iter2) << vcl_endl;
        return false;
      }
  return true;
}

//----------------------------------------------------------------------
//: save_as_images
//
// Save each of the eigenvectors as an image.
//
//.status under development
//.author Brendan McCane
//----------------------------------------------------------------------

void EigenFace::save_as_images(int width, int height)
{
  assert(width > 0 && height > 0);
  if ((unsigned int)(width*height)!=image_size)
  {
    vcl_cerr << "width*height must be equal to image size\n"
             << "image size is: " << image_size << vcl_endl;
    return;
  }

  Image im(width, height);
  vcl_vector<vnl_vector<double> *>::iterator iter;
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
    vcl_sprintf(name, "eigenface%03d.pgm", index++);
    vil1_save(im, name);
  }
}


//----------------------------------------------------------------------
//: encode_training_images
//
// Encode all training images for later classification.
//
//.status under development
//.author Brendan McCane
//----------------------------------------------------------------------
void EigenFace::encode_training_images()
{
  vcl_vector<vnl_vector<double> *>::iterator iter;
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
// .param Image *im: the input image
//
// .ret vnl_vector<double>*: the vector of weights
//
//.status under development
//.author Brendan McCane
//----------------------------------------------------------------------

vnl_vector<double>* EigenFace::encode(Image *im)
{
  if (eigenvectors.size()<=0)
  {
    return NULL;
  }
  vnl_vector<double> *im_vec = convert_image_to_vector(im);
  *im_vec -= *average_training_image;
  vnl_vector<double> *wts = new vnl_vector<double>(eigenvectors.size());
  for (unsigned int i=0; i<eigenvectors.size(); i++)
    (*wts)[i] = dot_product(*im_vec, *(eigenvectors[i]));
  delete im_vec;
  return wts;
}

vnl_vector<double>* EigenFace::encode(vnl_vector<double> *t_vec)
{
  if (eigenvectors.size()<=0)
  {
    return NULL;
  }
  vnl_vector<double> *im_vec = new vnl_vector<double>(*t_vec);
  *im_vec -= *average_training_image;
  vnl_vector<double> *wts = new vnl_vector<double>(eigenvectors.size());
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
// .param vnl_vector<double>*: the input set of weights
//
// .ret vnl_vector<double>*: the reconstructed image in a vector
//
//.status under development
//.author Brendan McCane
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
//
// \param k         the number of nearest neighbours to use
//
// \param dim       the number of dimensions of the eigenvectors to use
//
// \param threshold distances above this threshold are treated as not recognised
//
// \return the label of the recognised image or NULL if unsuccessful
//
//.status under development
//.author Brendan McCane
//----------------------------------------------------------------------

char *EigenFace::classify(Image *im, double threshold, int k, int dim)
{
  vcl_priority_queue<LabelDist> pq;

  if (num_vectors()==0) return NULL;
  if (eigenvectors.size()==0) return NULL;

  vnl_vector<double> *all_rep = encode(im);
  vnl_vector<double> rep(all_rep->extract(dim, all_rep->size()-dim));
  vnl_vector<double> diff(rep.size());
  double min_dist = DBL_MAX;
  int best=-1;
  char *ret=NULL;
#if 0
  vcl_cout << "rep = " << *rep << vcl_endl;
#endif
  for (int i=0; i<num_vectors(); i++)
  {
    vnl_vector<double> eigenvect=
      encoded_training_images[i]->extract
      (dim, encoded_training_images[i]->size()-dim);
    // vcl_cout << "vec " << i << " = " << *eigenvect << vcl_endl;
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

  vcl_cout << "min_dist = " << min_dist << " label = " << get_label(best) << vcl_endl;
  // now need to search through queue and find most likely label
  vcl_map<char *, int, ImageDatabase::ltstr> nns; // nearest neighbours
  for (int i=0; i<k; i++)
  {
    LabelDist t=pq.top();
    pq.pop();
    if ((t.dist<threshold)&&(t.dist!=0.0))
      nns[t.label]++;
  }
  vcl_map<char *, int, ImageDatabase::ltstr>::iterator iter;
  int max=0;
  for (iter=nns.begin(); iter!=nns.end(); iter++)
  {
    if ((*iter).second>max)
    {
      max = (*iter).second;
      ret = (*iter).first;
    }
  }
  vcl_cout << "label = " << ret << " num = " << max << vcl_endl;
  if (max<k/2+1) ret=NULL;
#if 0
  char ch;
  vcl_cin >> ch;
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
//.status under development
//.author Brendan McCane
//----------------------------------------------------------------------

void EigenFace::output_xgobi(char *basefile)
{
  char filenames[200];
  vcl_sprintf(filenames, "%s.dat", basefile);
  vcl_ofstream datfile(filenames);
  vcl_sprintf(filenames, "%s.glyphs", basefile);
  vcl_ofstream glyphs(filenames);
  vcl_sprintf(filenames, "%s.row", basefile);
  vcl_ofstream rowfile(filenames);
  vcl_map<char*, int, ImageDatabase::ltstr> glyphmap;
  int glyphnum=1;
  for (int i=0; i<num_vectors(); i++)
  {
    datfile << *(encoded_training_images[i]) << vcl_endl;
    // no glyph associated
    if (glyphmap.find(training_labels[i])==glyphmap.end())
    {
      vcl_cout << "Adding training_label " << training_labels[i] << vcl_endl;
      glyphmap[training_labels[i]] = glyphnum;
      glyphnum += 3;
    }
    glyphs << glyphmap[training_labels[i]] << vcl_endl;
    rowfile << training_labels[i] << vcl_endl;
  }
  vcl_map<char *, int, ImageDatabase::ltstr>::iterator iter;
  for (iter=glyphmap.begin(); iter!=glyphmap.end(); iter++)
    vcl_cout << (*iter).first << vcl_endl;
  rowfile.close();
  glyphs.close();
  datfile.close();
}
