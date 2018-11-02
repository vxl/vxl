// @author fsm

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <osl/osl_topology.h>
#include <osl/osl_load_topology.h>

//--------------------------------------------------------------------------------

int main()
{
  std::cerr << "reading topology\n";
  std::list<osl_edge*> edges;
  std::list<osl_Vertex*> vertices;
  osl_load_topology(std::cin, edges, vertices);

  std::cerr << "writing edges\n";
  std::cout << edges.size() << " edges\n";
  for (auto e : edges)
  {
    std::cout << std::endl << e->size() << std::endl;
    for (unsigned int j=0; j<e->size(); ++j)
      std::cout << e->GetX(j) << ' ' << e->GetY(j) << std::endl;
  }

  return 0;
}
