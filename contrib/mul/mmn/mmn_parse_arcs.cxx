
//:
// \file
// \brief Parse in a set of arc definitions using names of nodes
// \author Martin Roberts

#include <sstream>
#include <map>
#include <iostream>
#include <iterator>
#include <algorithm>
#include "mmn_parse_arcs.h"
#include <mbl/mbl_exception.h>
#include <mbl/mbl_parse_block.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


void  mmn_parse_arcs(std::istream& is,
                     const std::vector<std::string>& node_names,
                     std::vector<mmn_arc>& arcs)
{
    //First convert node_names to numbers
    std::map<std::string,unsigned> nodeMap;
    auto nodeIter=node_names.begin();
    auto nodeIterEnd=node_names.end();
    unsigned id=0;
    while(nodeIter != nodeIterEnd)
    {
        nodeMap[*nodeIter++]=id++;
    }

    std::string s = mbl_parse_block(is);
    std::istringstream ss(s);
    char c;
    ss>>c;  // Remove opening brace
    if (c!='{')
    {
        throw mbl_exception_parse_error("Expected '{' in mmn_parse_arcs");
    }

    arcs.clear();
    std::string arcLabel("arc:");
    //Loop over each arc in the stream
    while (!ss.eof())
    {
        //Locate the next "arc:" keyword
        if(std::find(std::istream_iterator<std::string>(ss),
                    std::istream_iterator<std::string>(),arcLabel) ==
           std::istream_iterator<std::string>())
        {
            //No more arc: labels to be processed
            break;
        }
        if(ss.bad())
        {
            std::string error_msg = "Stream error parsing arcs block\n";
            throw mbl_exception_parse_error(error_msg);
        }

        //we have read to just past the next "arc:" label
        std::string strNodePair=mbl_parse_block(ss);
        std::istringstream ssArc(strNodePair); //stream for just this arc

        //Now reduce this line/block into 4 tokens for { name1 name2 }
        //Note this assumes white space separators
        std::vector<std::string> tokens;
        std::copy(std::istream_iterator<std::string>(ssArc),
                 std::istream_iterator<std::string>(),
                 std::back_inserter(tokens));
        if(ssArc.bad())
        {
            std::string error_msg = "Stream error processing line "+strNodePair;
            throw mbl_exception_parse_error(error_msg);
        }
        if(tokens.size() != 4)
        {
            std::string error_msg = "Expected format is 4 tokens: { name1 name 2 }\n";
            error_msg+= " Got "+strNodePair;
            throw mbl_exception_parse_error(error_msg);
        }
        if(tokens[0] != "{" || tokens[3] != "}")
        {
            std::string error_msg = "Expected format is 4 tokens : { name1 name2 }\n";
            error_msg+= " Missing a (separated?) curly bracket in  "+strNodePair;
            throw mbl_exception_parse_error(error_msg);
        }

        //Seems to validate
        std::string node1=tokens[1];
        std::string node2=tokens[2];

        //Find the numeric IDs of these nodes
        std::map<std::string,unsigned>::const_iterator nodeIter1=nodeMap.find(node1);
        if(nodeIter1 == nodeMap.end())
        {
            std::string error_msg = "Node name "+node1+" is not in the list of nodes for arc\n";
            error_msg+= strNodePair;
            throw mbl_exception_parse_error(error_msg);
        }

        std::map<std::string,unsigned>::const_iterator nodeIter2=nodeMap.find(node2);
        if(nodeIter2 == nodeMap.end())
        {
            std::string error_msg = "Node name "+node2+" is not in the list of nodes for arc\n";
            error_msg+= strNodePair;
            throw mbl_exception_parse_error(error_msg);
        }

        //Finally add the arc
        arcs.emplace_back(nodeIter1->second,nodeIter2->second);

    }
}
