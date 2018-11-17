// This is brl/bbas/bil/algo/bil_cedt.h
#ifndef bil_cedt_h
#define bil_cedt_h
//:
// \file
// \brief 2D Contour based Euclidean Distance Transform algorithm
// \author Vishal Jain & Huysen Tek Brown University
// \date June 16, 2005
//

#include <vil/vil_image_view.h>
#include <vbl/vbl_array_2d.h>

//:  2D Contour Euclidean Distance transform
class bil_cedt_heap;
class bil_cedt_contour;
class bil_cedt
{
 public:
    bil_cedt();
    ~bil_cedt();

    bil_cedt(const vil_image_view<unsigned char>& im);

    bool compute_cedt();

    double closest_point_x(int i, int j) const {return dx_(i,j);}
    double closest_point_y(int i, int j) const {return dy_(i,j);}

    vil_image_view<float> cedtimg() const {return dist_;}

 private:
    vil_image_view<unsigned char> img_;
    vbl_array_2d<double> dx_;
    vbl_array_2d<double> dy_;
    vil_image_view<float> dist_;

    int ni_;
    int nj_;

    bool  find_dist_trans(vbl_array_2d<double> &level,bil_cedt_heap *heap);
    void propagate_dist(bil_cedt_contour *pf, bil_cedt_heap *heap, vbl_array_2d<double> &surface,
                        vbl_array_2d<unsigned char> &tag_array, vbl_array_2d<unsigned char> &dir_array,
                        int y, int x, int dir);
    void add_to_contour(bil_cedt_contour *pf, bil_cedt_heap *heap, vbl_array_2d<double> &surface,
                        vbl_array_2d<unsigned char> &tag_array, vbl_array_2d<unsigned char> &dir_array,
                        double dist_x, double dist_y, double dist,int y, int x, int dir, int position);
    void initial_diagonal_propagate(bil_cedt_contour *pf, bil_cedt_heap *heap, vbl_array_2d<double> &surface,
                                    vbl_array_2d<unsigned char> &tag_array, vbl_array_2d<unsigned char> &dir_array,
                                    int y, int x, int dir);
    int initial_direction(int x, int y);
};

class bil_cedt_heap
{
 public:
    bil_cedt_heap(int nj,int ni);
    ~bil_cedt_heap();

    double* data;
    int * index;
    int * rank;
    int * loc;
    int * locx;
    int * locy;

    int size, end;
    int N;
    int ni_;
    int nj_;

    void print_heap();
    void remove_max();
    void downheap(int k);
    void insert( int pos, int location, double item);
    void upheap( int k);
};

class bil_cedt_contour
{
 public:
    bil_cedt_contour(int nj,int ni);
    ~bil_cedt_contour();

    int * x;
    int * y;
    int * dir;

    int ptr;
};

#endif // bil_cedt_h
