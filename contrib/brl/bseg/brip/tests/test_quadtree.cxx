#include <iostream>
#include <iomanip>
#include <testlib/testlib_test.h>
#include <brip/brip_quadtree_node.h>
#include <brip/brip_quadtree_utils.h>
#include <brip/brip_quadtree_node_base_sptr.h>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_array_2d.h>
static void test_quadtree()
{
  brip_quadtree_node_base_sptr qnb
    = new brip_quadtree_node<float>(0,0,10,10, 1.1f);
  unsigned iul, jul, ilr, jlr;
  qnb->region(iul, jul, ilr, jlr);
  qnb->set_child(0, 0, qnb);
  brip_quadtree_node_base_sptr child00 = qnb->child(0,0);
  brip_quadtree_node_base_sptr child01 = qnb->child(0,1);
  auto* qcast =
    dynamic_cast<brip_quadtree_node<float>*>(child00.ptr());
  bool good = qcast && qcast->data()==1.1f;
  TEST("quadtree node", good, true);

  /// create a quadtree for a sparse image
  brip_quadtree_node_base_sptr root
    = new brip_quadtree_node<float>(0,0,7,7);
  brip_quadtree_node_base_sptr q00
    = new brip_quadtree_node<float>(0,0,3,3, 10.0f);
  brip_quadtree_node_base_sptr q01
    = new brip_quadtree_node<float>(4,0,7,3, 20.0f);
  brip_quadtree_node_base_sptr q10
    = new brip_quadtree_node<float>(0,4,3,7, 5.0f);
  brip_quadtree_node_base_sptr q11
    = new brip_quadtree_node<float>(4,4,7,7, 15.0f);
  root->set_child(0,0,q00);
  root->set_child(0,1,q01);
  root->set_child(1,0,q10);
  root->set_child(1,1,q11);
  brip_quadtree_node_base_sptr q11_10
    = new brip_quadtree_node<float>(4,2,5,3, 25.0f);
  q01->set_child(1,0,q11_10);
  brip_quadtree_node_base_sptr q11_10_11
    = new brip_quadtree_node<float>(5,3,5,3, 30.0f);
  q11_10->set_child(1,1, q11_10_11);

  vil_image_view<float> img(8,8);
  img.fill(-1.0f); //invalid value
  brip_quadtree_utils<float>::fill_image_from_node(root, img);
  for (unsigned j = 0; j<8; ++j)
  {
    for (unsigned i = 0; i<8; ++i)
      std::cout << std::fixed << img(i,j) << ' ';
    std::cout << '\n' << std::endl;
  }
  TEST("quadtree fill", img(5,3), 30.0f);
  vil_image_view<float> null_img;
  vil_image_view<float> null_mask;
  //see if the quadtree can be reverse generated
  vil_image_view<float> level2(2,2);
  vil_image_view<bool> mask2(2,2);
  mask2.fill(true);
  level2(0,0)=10.0f;   level2(0,1)=20.0f;
  level2(1,0)=5.0f;    level2(1,1)=15.0f;
  vbl_array_2d<brip_quadtree_node_base_sptr> nodes2;
  unsigned scale = 4;
  brip_quadtree_utils<float>::extract_nodes_from_image(level2,
                                                       mask2,
                                                       null_img,
                                                       null_mask,
                                                       scale,
                                                       nodes2);
  std::cout << "level 2\n";
  brip_quadtree_utils<float>::print_node(nodes2[0][0]);

  vil_image_view<float> level1(4,4);
  level1.fill(-1.0f);
  vil_image_view<bool> mask1(4,4);
  mask1.fill(false);
  level1(2,1) = 25.0f;
  mask1(2,1) = true;
  scale = 2;
  vbl_array_2d<brip_quadtree_node_base_sptr> nodes1;
  brip_quadtree_utils<float>::extract_nodes_from_image(level1,
                                                       mask1,
                                                       null_img,
                                                       null_mask,
                                                       scale,
                                                       nodes1);
  std::cout << "\n\nlevel 1\n";
  for (unsigned r = 0; r<2; ++r)
    for (unsigned c = 0; c<2; ++c){
      std::cout << "node_adr[" << r << "][" << c << "] = "<< nodes1[r][c] << '\n';
      if (nodes1[r][c])
        brip_quadtree_utils<float>::print_node(nodes1[r][c]);
    }

  vil_image_view<float> level0(8,8);
  level0.fill(-1.0f);
  vil_image_view<bool> mask0(8,8);
  mask0.fill(false);
  level0(5,3)=30.0f;
  mask0(5,3) = true;
  scale = 1;
  vbl_array_2d<brip_quadtree_node_base_sptr> nodes0;
  brip_quadtree_utils<float>::extract_nodes_from_image(level0,
                                                       mask0,
                                                       null_img,
                                                       null_mask,
                                                       scale,
                                                       nodes0);

  std::cout << "\n\nlevel 0\n";
  for (unsigned r = 0; r<4; ++r)
    for (unsigned c = 0; c<4; ++c){
      std::cout << "node_adr[" << r << "][" << c << "] = "<< nodes0[r][c] << '\n';
      if (nodes0[r][c])
        brip_quadtree_utils<float>::print_node(nodes0[r][c]);
    }

  std::vector<vil_image_view<float> > levels;
  levels.push_back(level0); levels.push_back(level1);
  levels.push_back(level2);

  std::vector<vil_image_view<bool> > masks;
  masks.push_back(mask0);   masks.push_back(mask1);
  masks.push_back(mask2);

  vbl_array_2d<brip_quadtree_node_base_sptr> root2;

  brip_quadtree_utils<float>::quadtrees_from_pyramid(levels, masks,
                                                     root2);
  unsigned nrows = root2.rows(), ncols = root2.cols();
  for (unsigned r = 0; r<nrows; ++r)
    for (unsigned c = 0; c<ncols; ++c)
      if (root2[r][c])
        brip_quadtree_utils<float>::print_node(root2[r][c]);

  vil_image_view<float> imgt(8,8);
  imgt.fill(-1.0f); //invalid value
  brip_quadtree_utils<float>::fill_image_from_node(root2[0][0], imgt);
  std::cout <<"Reconstructed image from tree\n";
  for (unsigned j = 0; j<8; ++j)
  {
    for (unsigned i = 0; i<8; ++i)
      std::cout << std::fixed << imgt(i,j) << ' ';
    std::cout << '\n' << std::endl;
  }
  TEST("reconstruction from tree from image", imgt(5,3), 30.0f);
}

TESTMAIN(test_quadtree);
