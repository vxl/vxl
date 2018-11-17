#include <iostream>
#include <queue>
#include <set>
#include "boxm2_points_to_volume_function.h"
//:
// \file
#include <cassert>
#include <bbas/imesh/imesh_vertex.h>
#include <imesh/algo/imesh_intersect.h>
#include <imesh/imesh_operations.h>
#include <bvgl/bvgl_intersection.h>
#include <bvgl/bvgl_triangle_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: constructor - populates list of AABBs
boxm2_points_to_volume::boxm2_points_to_volume(const boxm2_scene_sptr& scene,
                                               const boxm2_cache_sptr& cache,
                                               imesh_mesh& points)
: scene_(scene),
  cache_(cache),
  points_(points)
{
  //store mesh triangles
  std::cout<<"Triangulating points:"<<std::endl;
  std::unique_ptr<imesh_regular_face_array<3> > meshTris = imesh_triangulate(points_.faces());
  std::cout<<"   ... done."<<std::endl;

  //store bvpgl triangles
  const imesh_vertex_array<3>& verts = points_.vertices<3>();
  for (unsigned i=0; i<meshTris->size(); ++i) {
    imesh_regular_face<3>& triPts = (*meshTris)[i];
    vgl_box_3d<double> triBox;
    triBox.add( verts[triPts[0]] );
    triBox.add( verts[triPts[1]] );
    triBox.add( verts[triPts[2]] );
    triBoxes_.push_back(triBox);

    //push triangles
    bvgl_triangle_3d<double> tri(verts[triPts[0]], verts[triPts[1]], verts[triPts[2]]);
    tris_.push_back(tri);
  }
}


//: Main public method
void boxm2_points_to_volume::fillVolume()
{
  //grab blocks from scene
  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene_->blocks();

  //zip through each block
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    std::cout<<"Converting Block: "<<id<<std::endl;
    boxm2_block *        blk   = cache_->get_block(scene_, id);
    boxm2_block_metadata data  = blk_iter->second;
    std::size_t           nTrees= blk->trees().size();

    //array of arrays (one arrya for each tree in block)
    std::vector<std::vector<float> > datas(nTrees);
    std::vector<std::vector<float> > datas2(nTrees);
    this->fillBlockByTri(data, blk,datas);
#ifdef DEBUG
    for (int i=0; i<nTrees; ++i)
      if (datas.size() != datas2.size())
        std::cout<<"Tree "<<i<<" doesn't match..."<<std::endl;
#endif
    //------ Store alphas in cache ----------
    //count data lengths for this blocr
    std::size_t dataLen=0;
    for (auto & data : datas)
      dataLen += data.size();
    if (dataLen < nTrees)
      std::cout<<"NOT ALL TREES HAVE DATA THATS NO GOOD"<<std::endl;

    //initialize block data
    boxm2_data_base* newA = new boxm2_data_base(new char[dataLen * sizeof(float) ], dataLen * sizeof(float), id);
    auto* fullAlphas = reinterpret_cast<float*>(newA->data_buffer());
    std::size_t c=0;
    for (auto & data : datas)
      for (unsigned int j=0; j<data.size(); ++j)
        fullAlphas[c++] = data[j];

    //Replace data in the cache
    boxm2_cache_sptr cache = boxm2_cache::instance();
    cache->replace_data_base(scene_, id, boxm2_data_traits<BOXM2_ALPHA>::prefix(), newA);
  }
}

//:iterates over each triangle that intersects with a block, indexing into trees
void boxm2_points_to_volume::fillBlockByTri(boxm2_block_metadata& data, boxm2_block* blk, std::vector<std::vector<float> >& alphas)
{
  //initialize trees root data to empty
  boxm2_array_3d<uchar16>  trees = blk->trees_copy();  //trees to refine
  int Nx = trees.get_row1_count();
  int Ny = trees.get_row2_count();
  int Nz = trees.get_row3_count();

  //create a vector for each tree in the block (of size one)
  float emptyRoot = alphaProb(.001f, float(data.sub_block_dim_.x()));
  alphas.resize(trees.size());
  for (unsigned int i=0; i<trees.size(); ++i)
    alphas[i].push_back(emptyRoot);

  //grab local tris
  std::vector<vgl_box_3d<double> > localTriBoxes;
  std::vector<bvgl_triangle_3d<double> > localTris;
  this->tris_in_box(data.bbox(), tris_, triBoxes_, localTris, localTriBoxes);
  std::cout<<"  local tris: "<<localTris.size()<<std::endl;
  std::size_t numInts = 0;

  //go through each triangle and expand tree (just root for now)
  vgl_point_3d<double> blkO = data.local_origin_;
  for (unsigned int i=0; i<localTris.size(); ++i) {
    bvgl_triangle_3d<double>& tri = localTris[i];
    vgl_box_3d<double>& triBox = localTriBoxes[i];

    //get index bounds
    double sbLen = data.sub_block_dim_.x();
    int minX = clamp( (int) ( (triBox.min_x() - blkO.x()) / sbLen ), 0, Nx);
    int minY = clamp( (int) ( (triBox.min_y() - blkO.y()) / sbLen ), 0, Ny);
    int minZ = clamp( (int) ( (triBox.min_z() - blkO.z()) / sbLen ), 0, Nz);
    int maxX = clamp( (int) ( (triBox.max_x() - blkO.x()) / sbLen )+1, 0, Nx);
    int maxY = clamp( (int) ( (triBox.max_y() - blkO.y()) / sbLen )+1, 0, Ny);
    int maxZ = clamp( (int) ( (triBox.max_z() - blkO.z()) / sbLen )+1, 0, Nz);

    //grab local tree
    for (int x=minX; x<maxX; ++x)
      for (int y=minY; y<maxY; ++y)
        for (int z=minZ; z<maxZ; ++z) {
          uchar16& tree = trees(x,y,z);
          boct_bit_tree curr_tree( (unsigned char*) tree.data_block(), 4);
          vgl_box_3d<double> datBox = data.bbox(x,y,z);
          if (bvgl_intersection(datBox, tri)) {
            numInts++;
            //make sure tree is refined fully
            int offset = z + Nz*(y + x*Ny);
            refine_tree(curr_tree, datBox, tri, alphas[offset]);
            std::memcpy((unsigned char*) tree.data_block(), curr_tree.get_bits(),16);
            trees(x,y,z) = tree;
          }
        }

  }  //end block triangle loop
  std::cout<<"  num block/tri intersections: "<<numInts<<std::endl;

  //make sure each tree has the right index
  std::size_t c = 0, currDatPtr = 0, numOne=0;
  for (int x=0; x<Nx; ++x)
    for (int y=0; y<Ny; ++y)
      for (int z=0; z<Nz; ++z) {
        uchar16& tree = trees(x,y,z);
        boct_bit_tree curr_tree( (unsigned char*) tree.data_block(), 4);
        curr_tree.set_data_ptr(currDatPtr);
        std::memcpy((unsigned char*) tree.data_block(), curr_tree.get_bits(),16);

        //sanity checks
        if (alphas[c].size()==1) numOne++;
        if (curr_tree.num_cells() != (int)alphas[c].size())
          std::cout<<"Tree and alphas dont match in final copy!!! bad!!!!"<<std::endl;

        currDatPtr += alphas[c].size();
        ++c;
      }
  blk->set_trees(trees);
  std::cout<<" Num size one: "<<numOne<<" of "<<Nx*Ny*Nz<<std::endl;
}

void boxm2_points_to_volume::refine_tree(boct_bit_tree& tree,
                                         vgl_box_3d<double>& treeBox,
                                         bvgl_triangle_3d<double>& tri,
                                         std::vector<float>& alpha)
{
  constexpr int maxCell = 9;
  double blockLen = treeBox.width();
  boct_bit_tree orig(tree);
  unsigned int origSize = tree.num_cells();
  if (alpha.size() != origSize)
    std::cout<<"Alpha size doesnt match orig size. BAD!!!"<<std::endl;

  //keep track of new cells that intersect
  std::set<int> newSurface;

  //traverse tree, put data in BFS order
  std::queue<int> queue; queue.push(0);
  while (!queue.empty()) {
    int currBit = queue.front(); queue.pop();
    vgl_box_3d<double> cellBox = tree.cell_box(currBit, treeBox.min_point(), blockLen);

    //if tri intersects, move onto children
    if (bvgl_intersection(cellBox, tri))
    {
      //keep track of new surfaces
      if (tree.bit_at(currBit)==0)
        newSurface.insert(currBit);

      if (currBit < maxCell) {
        //flip currBit to denote children
        tree.set_bit_at(currBit, true);
        int firstChild = tree.child_index(currBit);
        for (int i=0; i<8; ++i)
          queue.push(firstChild+i);
      }
    }
  }

#ifdef DEBUG //sanity check
 if ( !newSurface.empty() ) {
   std::cout<<"  New surfaces:";
   for (std::set<int>::iterator i=newSurface.begin(); i!=newSurface.end(); ++i)
     std::cout<<' '<<*i;
   std::cout<<std::endl;
 }
#endif
  //now make sure the data is in place
  int refineSize = tree.num_cells();

  //otherwise zip through the tree in BFS order, setting alpha
  int alphIdx = 0;
  float fullRoot = alphaProb(.999f, 1.0f); //divide by cellLen
  float emptyRoot = alphaProb(.001f, 1.0f); //divide by cellLen
  std::vector<float> origAlphas = alpha;
  alpha.resize(refineSize);
  queue.push(0); //should be empty from above
  while (!queue.empty()) {
    int currBit = queue.front(); queue.pop();

    //if currBit is a valid cell in the old tree, move it's data into place
    if (newSurface.find(currBit)!=newSurface.end()) {
      //std::cout<<"  Inserting new surface! for "<<currBit<<std::endl;
      double cellLen = blockLen*tree.cell_len(currBit);
      alpha[alphIdx] = fullRoot/(float)cellLen;
    }
    else if ( orig.valid_cell(currBit) ) {
      alpha[alphIdx] = origAlphas[orig.get_relative_index(currBit)];
    }
    else {
      double cellLen = blockLen*tree.cell_len(currBit);
      alpha[alphIdx] = emptyRoot/(float)cellLen;
    }

    //tack children onto queue
    if ( tree.bit_at(currBit) && currBit < maxCell ){
      int firstChild = tree.child_index(currBit);
      for (int i=0; i<8; ++i)
        queue.push(firstChild+i);
    }
    ++alphIdx;
  }
}

//:Accelerated triangle bounding box intersection
void
boxm2_points_to_volume::tris_in_box(const vgl_box_3d<double>& bbox,
                                    const std::vector<bvgl_triangle_3d<double> >& tris,
                                    const std::vector<vgl_box_3d<double> >& bboxes,
                                    std::vector<bvgl_triangle_3d<double> >& int_tris,
                                    std::vector<vgl_box_3d<double> >& int_boxes)
{
  if (tris.size() != bboxes.size()) {
    std::cout<<" Triangle intersection not one to one w/ bounding boxes:"<<std::endl;
    return;
  }

  //use AABBs for faster collision detection
  auto tri = tris.begin();
  auto box = bboxes.begin();
  for ( ; tri != tris.end(); ++tri, ++box) {
    if (!bbox_intersect(*box, bbox))
      continue;
    if (bvgl_intersection(bbox, *tri)) {
      int_tris.push_back(*tri);
      int_boxes.push_back(*box);
    }
  }
}

//:slow tris in box method
std::vector<bvgl_triangle_3d<double> >
boxm2_points_to_volume::tris_in_box(const imesh_mesh& mesh, vgl_box_3d<double>& box)
{
  std::vector<bvgl_triangle_3d<double> > contained;
  const imesh_vertex_array<3>& verts = mesh.vertices<3>();
  std::unique_ptr<imesh_regular_face_array<3> > tris = imesh_triangulate(mesh.faces());
  imesh_regular_face_array<3>::const_iterator iter;
  for (iter = tris->begin(); iter != tris->end(); ++iter) {
    imesh_regular_face<3> idx =(*iter);
    bvgl_triangle_3d<double> tri(verts[idx[0]], verts[idx[1]], verts[idx[2]]);
    if (bvgl_intersection(box, tri))
      contained.push_back(tri);
  }
  return contained;
}

//: old, slow tris in box method
std::vector<bvgl_triangle_3d<double> >
boxm2_points_to_volume::tris_in_box(std::vector<bvgl_triangle_3d<double> >& tris, vgl_box_3d<double>& box)
{
  std::vector<bvgl_triangle_3d<double> > contained;
  std::vector<bvgl_triangle_3d<double> >::const_iterator iter;
  for (iter = tris.begin(); iter != tris.end(); ++iter) {
    if (bvgl_intersection(box, *iter))
      contained.push_back(*iter);
  }
  return contained;
}
