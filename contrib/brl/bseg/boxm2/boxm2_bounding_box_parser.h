#ifndef boxm2_bounding_box_parser_h_
#define boxm2_bounding_box_parser_h_

#include <string>
#include <iostream>
#include <map>
#include <expatpplib.h>

#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/boxm2_block_metadata.h>
#include <vgl/vgl_point_3d.h>
#include <vcl_compiler.h>

//block level metadata
#define DATASET_TAG "dataSet"
#define ENTITY_TAG  "entity"
#define VOLUME_TAG  "volume"
#define VERT_TAG    "vert"

class boxm2_bounding_box_parser : public expatpp
{
 public:
  boxm2_bounding_box_parser();
  ~boxm2_bounding_box_parser(void) {}

   // ACCESSORS for parser info
   std::string dataset() const { return dataset_; }
   int entity_id() const { return entity_id_; }
   int volume_id() const { return volume_id_; }
   std::vector< vgl_point_3d<double> >  verts(int entity_id, int volume_id)  {return verts_[entity_id][volume_id];}


   int version(){return version_;}
  std::map<int, std::map<int, std::vector< vgl_point_3d<double> > > > verts_;
  std::map<int, std::map<int,double> > heights_;
 private:
  virtual void startElement(const XML_Char* name, const XML_Char** atts);
  virtual void endElement(const XML_Char* /*name*/) {}
  virtual void charData(const XML_Char* /*s*/, int /*len*/) {}

  void init_params();

  std::string dataset_;
  int entity_id_;
  int volume_id_;
  double height_;

  int version_;
};

#endif
