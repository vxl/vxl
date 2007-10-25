// This is breye1/bvis1/bvis1_macros.h
#ifndef bwm_macros_h_
#define bwm_macros_h_
//:
// \file
// \brief Macros for use in a main function.
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 2/4/04
//
// See examples\bvis1_brown_eyes_main.cxx
//
// \verbatim
//  Modifications
// \endverbatim

#define REG_TABLEAU(tab) \
  bwm_tableau_mgr::instance()->register_tableau(tab);

/*#define REG_STORAGE(store) {\
  store ## _sptr model = store ## _new(); \
  model->register_binary_io(); \
  vidpro1_repository::register_type( model ); \
  }*/

#define REG_PROCESS(proc) \
  bwm_tableau_mgr::register_process( bpro1_process_sptr( new proc() ) );



#endif // bwm_macros_h_
