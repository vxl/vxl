// @author fsm

#include <iostream>
#include <vcl_compiler.h>

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
  for (std::list<osl_edge*>::iterator i=edges.begin(); i!=edges.end(); ++i)
  {
    osl_edge const *e = *i;
    std::cout << std::endl << e->size() << std::endl;
    for (unsigned int j=0; j<e->size(); ++j)
      std::cout << e->GetX(j) << ' ' << e->GetY(j) << std::endl;
  }

  return 0;
}
