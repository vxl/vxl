#include <vcl_list.txx>
class osl_edgel_chain;
VCL_LIST_INSTANTIATE(osl_edgel_chain*);

#include <osl/osl_topology.h>
template osl_edgel_chain *fsm_pop(vcl_list<osl_edgel_chain*>*);
