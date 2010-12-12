#ifndef vtol_dtd_h_
#define vtol_dtd_h_

char vtol_dtd0[] = "\
\n\
<!DOCTYPE vxl [\n\
\n\
<!-- vxl\n\
  -->\n\
\n\
<!ELEMENT vxl ANY>\n\
\n\
<!-- ** Generic structures ** -->\n\
\n\
<!ELEMENT vxl_flip EMPTY>\n\
\n\
<!-- ** Geometry structures ** -->\n\
\n\
<!-- vxl_point_[23]d\n\
        <vxl_point_2d x=\"1\" y=\"3\" w=\"4\"/>\n\
\n\
        x,y,z and w values (w defaults to 1)\n\
        pointer (_sptr) exists\n\
  -->\n\
\n\
<!ELEMENT vxl_point_2d EMPTY>\n\
  <!ATTLIST vxl_point_2d id ID   #IMPLIED\n\
                         x CDATA #REQUIRED\n\
                         y CDATA #REQUIRED\n\
                         w CDATA \"1\" >\n\
<!ELEMENT vxl_point_3d EMPTY>\n\
  <!ATTLIST vxl_point_3d id ID   #IMPLIED\n\
                         x CDATA #REQUIRED\n\
                         y CDATA #REQUIRED\n\
                         z CDATA #REQUIRED\n\
                         w CDATA \"1\" >\n\
\n\
<!ELEMENT vxl_point_2d_sptr EMPTY>\n\
  <!ATTLIST vxl_point_2d_sptr id IDREF #REQUIRED>\n\
<!ELEMENT vxl_point_3d_sptr EMPTY>\n\
  <!ATTLIST vxl_point_3d_sptr id IDREF #REQUIRED>\n\
\n"; char vtol_dtd1[] = "\
<!-- ** Topology structures ** -->\n\
\n\
<!-- vxl_vertex_[23]d\n\
        <vxl_vertex_2d id=\"3\"><vxl_point_2d x=\"1\" y=\"3\"/></vxl_vertex>\n\
\n\
     vxl_vertex_[23]d_sptr\n\
        <vxl_vertex_2d_sptr id=\"3\">\n\
\n\
        Vertex contains a vxl_point or a pointer to a vxl_point.\n\
  -->\n\
\n\
<!ELEMENT vxl_vertex_2d ( vxl_point_2d | vxl_point_2d_sptr )>\n\
  <!ATTLIST vxl_vertex_2d id ID #IMPLIED>\n\
<!ELEMENT vxl_vertex_3d ( vxl_point_3d | vxl_point_2d_sptr )>\n\
  <!ATTLIST vxl_vertex_3d id ID #IMPLIED>\n\
\n\
<!ELEMENT vxl_vertex_2d_sptr EMPTY>\n\
  <!ATTLIST vxl_vertex_2d_sptr id IDREF #REQUIRED>\n\
<!ELEMENT vxl_vertex_3d_sptr EMPTY>\n\
  <!ATTLIST vxl_vertex_3d_sptr id IDREF #REQUIRED>\n\
\n\
<!-- vxl_zero_chain_2d\n\
         <vxl_zero_chain_2d id=\"3\">\n\
              <vxl_vertex_2d ...>\n\
              <vxl_vertex_2d ...>\n\
         </vxl_zero_chain_2d>\n\
  -->\n\
\n\
<!ELEMENT vxl_zero_chain_2d ( ( ( vxl_vertex_2d, vxl_flip? ) |\n\
                               ( vxl_vertex_2d_sptr, vxl_flip? ) )+ )>\n\
  <!ATTLIST vxl_zero_chain_2d id ID #IMPLIED>\n\
\n\
<!ELEMENT vxl_zero_chain_2d_sptr EMPTY>\n\
  <!ATTLIST vxl_zero_chain_2d_sptr id IDREF #REQUIRED>\n\
\n\
<!-- vxl_edge_2d\n\
         <vxl_edge_2d id=\"3\">\n\
              <vxl_vertex_2d ...>\n\
              <vxl_vertex_2d ...>\n\
         </vxl_edge_2d>\n\
  -->\n\
\n"; char vtol_dtd2[] = "\
<!ELEMENT vxl_edge_2d ( vxl_zero_chain_2d | vxl_zero_chain_2d_sptr)>\n\
  <!ATTLIST vxl_edge_2d id ID #IMPLIED>\n\
\n\
<!ELEMENT vxl_edge_2d_sptr EMPTY>\n\
  <!ATTLIST vxl_edge_2d_sptr id IDREF #REQUIRED>\n\
\n\
<!-- vxl_one_chain_[23d]\n\
          <vxl_one_chain_[23d] id=\"6\"\n\
                <vxl_edge_2d ...>\n\
                <vxl_edge_2d ...>\n\
                <vxl_edge_2d ...>\n\
          </vxl_one_chain_2d>\n\
\n\
          One_Chain is a list of edges or edge pointers.\n\
          Closure is enforced by the list.\n\
          Pointer (_sptr) exists.\n\
  -->\n\
\n\
<!ELEMENT vxl_one_chain_2d ( ( ( vxl_edge_2d, vxl_flip? ) |\n\
                              ( vxl_edge_2d_sptr, vxl_flip? ) |\n\
                              vxl_one_chain_2d )+ )>\n\
  <!ATTLIST vxl_one_chain_2d id ID #IMPLIED>\n\
\n\
<!ELEMENT vxl_one_chain_2d_sptr EMPTY>\n\
  <!ATTLIST vxl_one_chain_2d_sptr id IDREF #REQUIRED>\n\
\n\
<!-- vxl_triangle_2d\n\
        <vxl_triangle_2d>\n\
           <vxl_point_2d><vxl_point_2d><vxl_point_2d>\n\
        </vxl_triangle_2d>\n\
\n\
        Three (no more, no less) vxl_vertices (The Lightweight Solution).\n\
        At the same level as a one-chain.\n\
        Pointer (_sptr) exists.\n\
  -->\n\
\n\
<!ELEMENT vxl_triangle_2d ( ( vxl_vertex_2d | vxl_vertex_2d_sptr ),\n\
                            ( vxl_vertex_2d | vxl_vertex_2d_sptr ),\n\
                            ( vxl_vertex_2d | vxl_vertex_2d_sptr ))>\n\
  <!ATTLIST vxl_triangle_2d id ID #IMPLIED>\n\
\n\
<!ELEMENT vxl_triangle_2d_sptr EMPTY>\n\
  <!ATTLIST vxl_triangle_2d_sptr id IDREF #REQUIRED>\n\
\n"; char vtol_dtd3[] = "\
<!-- vxl_face_[23d]\n\
           <vxl_face_2d id=\"7\">\n\
                 <vxl_one_chain_2d ...>\n\
                 <vxl_one_chain_2d ...>\n\
           </vxl_face_2d>\n\
\n\
           Face is a list of one chains or pointers to one chains.\n\
           Pointer (_sptr) exists.\n\
  -->\n\
\n\
<!ELEMENT vxl_face_2d ( ( vxl_one_chain_2d | vxl_one_chain_2d_sptr |\n\
                          vxl_triangle_2d | vxl_triangle_2d_sptr )+ )>\n\
  <!ATTLIST vxl_face_2d id ID #IMPLIED>\n\
\n\
<!ELEMENT vxl_face_2d_sptr EMPTY>\n\
  <!ATTLIST vxl_face_2d_sptr id IDREF #REQUIRED>\n\
\n\
<!-- vxl_two_chain_[23d]\n\
           <vxl_two_chain_2d id=\"12\">\n\
                     <vxl_face_2d ...>\n\
                     <vxl_face_2d ...>\n\
           </vxl_two_chain_2d>\n\
\n\
           Two chain is a list of faces or pointers to faces.\n\
           Pointer (_sptr) exists.\n\
  -->\n\
\n\
<!ELEMENT vxl_two_chain_2d ( ( vxl_face_2d | vxl_face_2d_sptr )+ )>\n\
  <!ATTLIST vxl_two_chain_2d id ID #IMPLIED>\n\
\n\
<!ELEMENT vxl_two_chain_2d_sptr EMPTY>\n\
  <!ATTLIST vxl_two_chain_2d_sptr id IDREF #REQUIRED>\n\
\n\
]>\n\
\n\
\n\
\n\
\n\
\n\
";

#endif // vtol_dtd_h_
