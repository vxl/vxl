
#include <vtol/vtol_topology_object.h>


// This is the destructor for topology_object.
vtol_topology_object::~vtol_topology_object()
{
  delete inf_sup_cache;
}

vtol_topology_object::vtol_topology_object(int num_inferiors, int num_superiors) :
  vtol_topology_hierarchy_node(num_inferiors, num_superiors)
{
  inf_sup_cache = 0;
}

void vtol_topology_object::ProtectedDestroy()
{
  vtol_one_chain* onechain = this->CastToOneChain();
  vtol_two_chain* twochain = this->CastToTwoChain();
  if(onechain && onechain->IsSubChain())
    {
      return;
    }

  if(twochain && twochain->IsSubChain())
    {
      return;
    }


  if (( (this->is_referenced())) // if referenced
      || (this->numsup() > 0))  // or has superiors
    {
      return;
    }
  vtol_topology_object::Destroy(this);
}

void vtol_topology_object::Destroy(vtol_topology_object* topobj)
{
  // Recursively traverse the inferiors of objects to
  // delete the entire hierarchy of a topological entity
  // if 1) the object is not protected from outside
  //    2) the object is the highest node...ie has no superiors.
  //    3) the object is not on a SpatialGroup...ie a SpatialGroup is the superior.

  vtol_one_chain* onechain = topobj->CastToOneChain();
  vtol_two_chain* twochain = topobj->CastToTwoChain();
  if(onechain && onechain->IsSubChain())
    return;
  if(twochain && twochain->IsSubChain())
    return;

  if (( !(topobj->is_referenced())) // not referenced
      && (topobj->numsup() == 0))  // no superiors
    {
      for (topobj->GetInferiors()->reset(); topobj->GetInferiors()->next();)
        {
          vtol_topology_object *infptr;
          infptr = (topobj->GetInferiors())->value();
          topobj->vtol_topology_hierarchy_node::unlink_inferior(topobj, infptr);
          vtol_topology_object::Destroy(infptr);
        }

      if(onechain && onechain->ContainsSubChains())
        {
          vcl_vector<vtol_one_chain*>* inflist = onechain->InferiorOneChains();
          for(inflist->reset(); inflist->next();)
            {
              vtol_one_chain* subchain = inflist->value();
              onechain->RemoveInferiorOneChain(subchain);
            }
          for(inflist->reset(); inflist->next();)
            vtol_topology_object::Destroy(inflist->value());
          delete inflist;
        }
      if(twochain)
        {
          vcl_vector<vtol_two_chain*>* inflist = twochain->InferiorTwoChains();
          if(inflist->length())
            {
              for(inflist->reset(); inflist->next();)
                {
                  vtol_two_chain* subchain = inflist->value();
                  twochain->RemoveInferiorTwoChain(subchain);
                  vtol_topology_object::Destroy(subchain);
                }
            }

          delete inflist;
        }

      // Finally delete the object itself.
      delete topobj;
    }
}



vcl_vector<vtol_vertex*>* vtol_topology_object::Vertices()
{
  cerr << "Barfing Merrily, vtol_topology_object::Vertices()" << endl;
  return new vcl_vector<vtol_vertex*>;
}

void vtol_topology_object::Vertices(vcl_vector<vtol_vertex*>& verts)
{
  if(!inf_sup_cache)
    inf_sup_cache = new topology_cache(this);
  inf_sup_cache->Vertices(verts); 
}

vcl_vector<vtol_zero_chain*>* vtol_topology_object::ZeroChains()
{
  cerr << "Barfing Merrily, vtol_topology_object::Vertices()" << endl;
  return new vcl_vector<vtol_zero_chain*>;
}

void vtol_topology_object::ZeroChains( vcl_vector<vtol_zero_chain*>& zerochains)
{
  if(!inf_sup_cache)
    inf_sup_cache = new topology_cache(this);
  inf_sup_cache->ZeroChains(zerochains);
}

vcl_vector<vtol_edge*>* vtol_topology_object::Edges()
{
  cerr << "Barfing Merrily, vtol_topology_object::Vertices()" << endl;
  return new vcl_vector<vtol_edge*>;
}

void vtol_topology_object::Edges(vcl_vector<vtol_edge*>& edges)
{
  if(!inf_sup_cache)
    inf_sup_cache = new topology_cache(this);
  inf_sup_cache->Edges(edges);
}

void vtol_topology_object::Describe(ostream& strm=cout,int blanking=0)
{
  DescribeInferiors(strm, blanking);
  DescribeSuperiors(strm, blanking);
}

void vtol_topology_object::Print (ostream& strm =cout)
{
  strm << "<vtol_topology_object " << (void *)this << ">" << endl;
}






