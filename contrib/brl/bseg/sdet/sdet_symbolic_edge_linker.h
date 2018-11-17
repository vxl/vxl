// This is brl/bseg/sdet/sdet_symbolic_edge_linker_det.h
#ifndef sdet_symbolic_edge_linker_det_h
#define sdet_symbolic_edge_linker_det_h
//:
//\file
//\brief Symbolic edge linker
//
//\verbatim
//  Modifications
//\endverbatim

#include <sdet/sdet_edgemap_sptr.h>
#include <sdet/sdet_curvelet_map.h>
#include <sdet/sdet_edgel_link_graph.h>
#include <sdet/sdet_curve_fragment_graph.h>
#include <sdet/sdet_symbolic_edge_linker_params.h>

//: function to compute generic edges
class sdet_symbolic_edge_linker : public sdet_symbolic_edge_linker_params
{
 public:
  sdet_symbolic_edge_linker()
    : sdet_symbolic_edge_linker_params() {}

  sdet_symbolic_edge_linker(sdet_symbolic_edge_linker_params const& params)
    : sdet_symbolic_edge_linker_params(params) {}

  void apply(const sdet_edgemap_sptr& edgemap);

  sdet_curvelet_map const & curvelet_map() { return CM; }
  sdet_edgel_link_graph const & edgel_link_graph() { return ELG; }
  sdet_curve_fragment_graph const & curve_fragment_graph() { return CFG; }

 private:
  sdet_curvelet_map CM;
  sdet_edgel_link_graph ELG;
  sdet_curve_fragment_graph CFG;
};

#endif // sdet_symbolic_edge_linker_h
