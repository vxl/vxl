#ifndef gkll_multi_view_data_
#define gkll_multi_view_data_
#ifdef __GNUC__
#pragma interface
#endif
//--------------------------------------------------------------------------------
// .NAME	gkll_multi_view_point_data
// .INCLUDE	gkll/gkll_multi_view_point_data
// .FILE	gkll_multi_view_point_data.cxx
// .SECTION Description:
//   Class to hold the matched points through mutli views
// .SECTION Author
//   L. Guichard
// .SECTION Modifications:   
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_list.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <mvl/NViewMatches.h>
#include <vcl_iostream.h>
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
	int get_nb_views(){return _nbviews;}; 

	void remove(int view_num, T match);
	
	vcl_ostream& print(vcl_ostream& str);

private:
	int _nbviews;
	int _nbfeatures; //number of features wishing to be detected 
	int _size_vect_ft;//the number of features in all images
	int _matchnum; //the number of tracks
	bool _closed_track;

	NViewMatches MVM;
	vcl_vector<T> all_pts;

};
#endif