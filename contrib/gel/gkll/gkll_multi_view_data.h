#ifndef gkll_multi_view_data_
#define gkll_multi_view_data_
#ifdef __GNUC__
#pragma interface
#endif
//--------------------------------------------------------------------------------
//:
// \file
//   Class to hold the matched points through multi views
// \author
//   L. Guichard
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <mvl/NViewMatches.h>
#include <vbl/vbl_ref_count.h>

template <class T>
class gkll_multi_view_data: public vbl_ref_count {

public:
  gkll_multi_view_data();
  gkll_multi_view_data(int nbviews);
  ~gkll_multi_view_data();

  void new_track();
  void close_track();

  void set_params(int nbviews);
  void set(int view_num,int matchnum,T);
  void set(int view_num,T);

  void get(int, vcl_vector<T> &);
  void get(int, int, vcl_vector<T> &,
           vcl_vector<T> &);
  void get(int, int, int, vcl_vector<T> &,
           vcl_vector<T> &,
           vcl_vector<T> &);

  bool get_pred_match(int view_num,T obj,T & res);
  int get_nb_views(){return nbviews_;};

  void remove(int view_num, T match);

  vcl_ostream& print(vcl_ostream& str);

private:
  int nbviews_;
  int nbfeatures_; //number of features wishing to be detected
  int size_vect_ft_;//the number of features in all images
  int matchnum_; //the number of tracks
  bool closed_track_;

  NViewMatches MVM;
  vcl_vector<T> all_pts;
};
#endif
