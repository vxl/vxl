#include <vcl_vector.txx>
#include <vcl_algorithm.txx>

typedef struct vgui_glut_adaptor *elt_t;

VCL_VECTOR_INSTANTIATE(elt_t);
VCL_FIND_INSTANTIATE(vcl_vector<elt_t>::iterator, elt_t);
