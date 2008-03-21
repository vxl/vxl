#include <brip/brip_quadtree_utils.h>

template <class T>
void brip_quadtree_utils<T>::
fill_image_region(brip_quadtree_node_base_sptr node,
                  vil_image_view<T>& img)
{
  if(!node)
    return;
  if(!node->data_valid())
    return;
  unsigned iul, jul, ilr, jlr;
  node->region(iul, jul, ilr, jlr);
  brip_quadtree_node<T>* fn = 
    dynamic_cast<brip_quadtree_node<T>*>(node.ptr());
  float data = fn->data();
  for(unsigned j = jul; j<=jlr; ++j)
    for(unsigned i = iul; i<=ilr; ++i)
      img(i,j)=data;
}

template <class T>
void brip_quadtree_utils<T>::
fill_image_from_node(brip_quadtree_node_base_sptr node,
                     vil_image_view<T>& img)
{
  if(!node)
    return;
  //fill according to this node's data
  //  fill_region(node, img);
  brip_quadtree_utils<T>::fill_image_region(node, img);
  // base case: check if no children
  if(!node->n_children())
    return;

  // recursion step
  for(unsigned i = 0; i<2; ++i)
    for(unsigned j = 0; j<2; ++j)
      fill_image_from_node(node->child(i,j), img);
}

template <class T>
void brip_quadtree_utils<T>::
extract_nodes_from_image(vil_image_view<T> const & img,
                         vil_image_view<bool> const & mask,
                         vil_image_view<T> const& parent_img,
                         vil_image_view<bool> const& parent_mask,
                         unsigned scale,
                         vbl_array_2d<brip_quadtree_node_base_sptr>& nodes)
{
  // the image has two types of pixel values: valid and invalid
  // the invalid values are indicated by the mask plane = false
  // invalid pixels do not generate quadtree nodes
  // the size of nodes is 1/2 of the image dimensions
  unsigned ni = img.ni(), nj = img.nj();
  nodes.resize(nj/2, ni/2);
  nodes.fill(0);
  bool parent = parent_img.ni()>0;
  for(unsigned j = 0; j<nj; j+=2)
    for(unsigned i = 0; i<ni; i+=2){
      bool require_node = false;
      for(unsigned k =0; k<=1&&!require_node; ++k)
        for(unsigned m =0; m<=1&&!require_node; ++m)
          if(mask(i+m, j+k))
            require_node = true;
      if(require_node){
        // upper left corner of 1/2 size quadtree node
        unsigned iul = i*scale, jul = j*scale;
        // lower left corner of 1/2 size quadtree node
        unsigned ilr = iul + 2*scale -1;
        unsigned jlr = jul + 2*scale -1;
        // upper left corner of 1/2 size quadtree node
        brip_quadtree_node<T>* nn; 
        if(parent && parent_mask(i/2, j/2))
          nn = new brip_quadtree_node<T>(iul, jul, ilr, jlr,
                                         parent_img(i/2, j/2));
        else
          nn = new brip_quadtree_node<T>(iul, jul, ilr, jlr);
        nodes[j/2][i/2]= nn;
        // fill out the children, there is at least one
        for(unsigned k =0; k<2; ++k)
          for(unsigned m =0; m<2; ++m){
            unsigned r = j+k, c = i+m;
            if(mask(c, r))
              {
                // upper left corner of child node
                unsigned iulc = c*scale, julc = r*scale;
                //lower right corner of child node
                unsigned ilrc = iulc + scale -1;
                unsigned jlrc = julc + scale -1;
                brip_quadtree_node<T>* cn = 
                  new brip_quadtree_node<T>(iulc, julc,
                                            ilrc, jlrc,
                                            img(c,r));
                nn->set_child(k,m, cn);
                cn->set_parent(nn);
              }
          }
      }
    }
}
//: attach children from prev to the parents in the nodes array
template <class T>
void brip_quadtree_utils<T>::
connect_children(vbl_array_2d<brip_quadtree_node_base_sptr>& nodes,
                 unsigned scale,
                 vbl_array_2d<brip_quadtree_node_base_sptr> const& prev)
{
  unsigned nrow = nodes.rows(), ncol = nodes.cols();
  for(unsigned r = 0; r<nrow; ++r)
    for(unsigned c = 0; c<ncol; ++c)
      for(unsigned k =0; k<2; ++k)
        for(unsigned m =0; m<2; ++m){
          unsigned rp = 2*r+k, cp = 2*c+m;
          if(prev[rp][cp]){
            if(!nodes[r][c]){
              // upper left corner of new node
              unsigned iul = c*scale, jul = r*scale;
              //lower right corner of new node
              unsigned ilr = iul + scale -1;
              unsigned jlr = jul + scale -1;
              nodes[r][c]=new brip_quadtree_node<T>(iul,jul, ilr, jlr);
            }else{
              nodes[r][c]->set_child(k,m,prev[rp][cp]);
              prev[rp][cp]->set_parent(nodes[r][c]);
            }
          }
        }
}
template <class T>
void brip_quadtree_utils<T>::
quadtrees_from_pyramid(vcl_vector<vil_image_view<T> > levels,
                       vcl_vector<vil_image_view<bool> > masks,
                       vbl_array_2d<brip_quadtree_node_base_sptr>& roots)
{
  //start at the base image of the pyramid, i.e. levels[0].
  //at the end a vbl array with quad-tree nodes will be available.
  unsigned scale = 1;
  unsigned n_levels = levels.size();
    
  vil_image_view<T>& parent_img = vil_image_view<T>();
  vil_image_view<bool>& parent_mask = vil_image_view<bool>();
  if(n_levels>1){
    parent_img = levels[1];
    parent_mask = masks[1];
  }
  unsigned ni0 = levels[0].ni(), nj0 = levels[0].nj();
  vbl_array_2d<brip_quadtree_node_base_sptr> prev;
  brip_quadtree_utils<T>::extract_nodes_from_image(levels[0],
                                                   masks[0],
                                                   parent_img,
                                                   parent_mask,
                                                   scale,
                                                   prev);
  scale = 2;
  for(unsigned lev = 1; lev<n_levels; ++lev)
    {
      unsigned ni = levels[lev].ni(), nj = levels[lev].nj();
      if(lev<n_levels-1){
        parent_img = levels[lev+1];
        parent_mask = masks[lev+1];
      }else{
        parent_img = vil_image_view<T>();
        parent_mask = vil_image_view<bool>();
      }
      brip_quadtree_utils<T>::extract_nodes_from_image(levels[lev],
                                                       masks[lev],
                                                       parent_img,
                                                       parent_mask,
                                                       scale,
                                                       roots);
      brip_quadtree_utils<T>::connect_children(roots,scale, prev);

      prev = roots;
      scale = scale*2;
    }
}

template <class T>
void brip_quadtree_utils<T>::
print_node( brip_quadtree_node_base_sptr const& node,
            vcl_ostream& os,
            vcl_string indent)
{
  //cast the node to the type
  brip_quadtree_node<T>* nt = 
    dynamic_cast<brip_quadtree_node<T>* >(node.ptr());
  if(!nt)
    return;
  unsigned iul = 0, jul = 0, ilr = 0, jlr = 0;
  nt->region(iul, jul, ilr, jlr);
  // base case, no children
  if(nt->n_children()==0){
    os << indent << "leaf:(" << iul << ' ' << jul << ")("
       << ilr << ' ' << jlr << "):data= ";
    if(nt->data_valid())
      os << nt->data() << '\n';
    else
      os << "###\n";
    return;
  }
  os << indent << "node:(" << iul << ' ' << jul << ")("
     << ilr << ' ' << jlr << "):data= ";
  if(nt->data_valid())
    os << nt->data() << '\n';
  else
    os << "###\n";
  vcl_string ind = indent + "  ";
  for(unsigned i = 0; i<2; ++i)
    for(unsigned j = 0; j<2; ++j)
      if(nt->child(i,j))
        brip_quadtree_utils<T>::print_node(nt->child(i,j),
                                           os,
                                           ind);
}

         
#undef BRIP_QUADTREE_UTILS_INSTANTIATE
#define BRIP_QUADTREE_UTILS_INSTANTIATE(T) \
template class brip_quadtree_utils<T >
