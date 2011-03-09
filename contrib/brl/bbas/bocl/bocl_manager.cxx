#include "bocl_manager.h"
//:
// \file

//: Binary save boxm2_scene scene from stream.
void vsl_b_write(vsl_b_ostream& os, bocl_manager_child const& scene){}
void vsl_b_write(vsl_b_ostream& os, const bocl_manager_child* &p){}
void vsl_b_write(vsl_b_ostream& os, bocl_manager_child_sptr& sptr){}
void vsl_b_write(vsl_b_ostream& os, bocl_manager_child_sptr const& sptr){}

//: Binary load boxm2_scene scene from stream.
void vsl_b_read(vsl_b_istream& is, bocl_manager_child &scene){}
void vsl_b_read(vsl_b_istream& is, bocl_manager_child* p){}
void vsl_b_read(vsl_b_istream& is, bocl_manager_child_sptr& sptr){}
void vsl_b_read(vsl_b_istream& is, bocl_manager_child_sptr const& sptr){}
