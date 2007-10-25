// This is brl/bbas/bwm/bwm_macros.h
#ifndef bwm_macros_h_
#define bwm_macros_h_
//:
// \file
// \brief Macros for use in a main function ow bwm project.
// \author Gamze Tunali, (gtunali@brown.edu)
// \date 10/24/07
//
// See bwm/exe
//
// \verbatim
//  Modifications
// \endverbatim

#define REG_TABLEAU(tab) \
  bwm_tableau_mgr::instance()->register_tableau(new tab());

#define REG_PROCESS(proc) \
  bwm_tableau_mgr::instance()->register_process(new proc());


#endif // bwm_macros_h_
