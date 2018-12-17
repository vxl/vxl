#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "../bjson.h"


class IJsonSerializable
{
public:
   virtual ~IJsonSerializable( void ) = default;;
   virtual void Serialize( Json::Value& root ) =0;
   virtual void Deserialize( Json::Value& root) =0;
};

class TestClassA : public IJsonSerializable
{
public:
        TestClassA( void )= default;
   ~TestClassA( void ) override= default;
   void Serialize( Json::Value& root ) override{
       // serialize primitives
       root["testintA"] = m_nTestInt;
       root["testfloatA"] = m_fTestFloat;
       root["teststringA"] = m_TestString;
       root["testboolA"] = m_bTestBool;
       Json::Value ilist;
       for(size_t i = 0; i<m_IntList.size(); ++i){
         Json::Value::ArrayIndex ai = static_cast<Json::Value::ArrayIndex>(i);
         ilist[ai] = m_IntList[i];
       }
       root["testListA"] = ilist;
     }
     void Deserialize( Json::Value& root) override{
   // deserialize primitives
       m_nTestInt = root.get("testintA",0).asInt();
       m_fTestFloat = root.get("testfloatA", 0.0).asDouble();
       m_TestString = root.get("teststringA", "").asString();
       m_bTestBool = root.get("testboolA", false).asBool();
       const Json::Value ilist = root["testListA"];
       for(Json::Value::const_iterator lit = ilist.begin();
           lit != ilist.end(); ++lit)
         m_IntList.push_back((*lit).asInt());
     }
   int           m_nTestInt;
   double        m_fTestFloat;
   std::string   m_TestString;
   bool          m_bTestBool;
  std::vector<int>  m_IntList;
};

/* sample json
  {
"testboolA" : true,
"testfloatA" : 3.14159,
"testintA" : 42,
"teststringA" : "foo"
"testListA" : [ 1, 2 ]
}

*/

class CJsonSerializer
{
public:
  static bool Serialize( IJsonSerializable* pObj, std::string& output ){
    if (pObj == nullptr)
      return false;

    Json::Value serializeRoot;
    pObj->Serialize(serializeRoot);

    Json::StyledWriter writer;
    output = writer.write( serializeRoot );

    return true;
  }
  static bool Deserialize( IJsonSerializable* pObj, std::string& input ){
    if (pObj == nullptr)
      return false;

    Json::Value deserializeRoot;
    Json::Reader reader;

    if ( !reader.parse(input, deserializeRoot) )
      return false;

    pObj->Deserialize(deserializeRoot);

    return true;
  }
private:
   CJsonSerializer( void ) = default;;
};

static void test_bjson()
{
   TestClassA testClass;
   std::string input = "{ \"testintA\" : 42, \"testfloatA\" : 3.14159, \"teststringA\" : \"foo\", \"testboolA\" : true , \"testListA\" : [ 1, 2 ] }\n";
   CJsonSerializer::Deserialize( &testClass, input );
   bool good = testClass.m_nTestInt == 42;
    good = good && testClass.m_fTestFloat == 3.14159;
    good = good && testClass.m_TestString == "foo";
    good = good && testClass.m_bTestBool == true;
   std::cout << "Raw Json Input\n" << input << "\n\n";
   TEST("test parse json string",good, true);
   std::string output;
   CJsonSerializer::Serialize( &testClass, output);
   std::cout << "testClass Serialized Output\n" << output << "\n\n\n";
   std::string actual_output ="{\n   \"testListA\" : [ 1, 2 ],\n   \"testboolA\" : true,\n   \"testfloatA\" : 3.1415899999999999,\n   \"testintA\" : 42,\n   \"teststringA\" : \"foo\"\n}\n";
   good = output==actual_output;
   TEST("test construct json string",good, true);
}


TESTMAIN(test_bjson);
