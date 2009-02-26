
//:
// \file
// \brief Parse in a set of arc definitions using names of nodes
// \author Martin Roberts

#include "mmn_parse_arcs.h"
#include <mbl/mbl_exception.h>
#include <mbl/mbl_parse_block.h>
#include <vcl_sstream.h>
#include <vcl_map.h>
#include <vcl_iostream.h>
#include <vcl_iterator.h>
#include <vcl_algorithm.h>


void  mmn_parse_arcs(vcl_istream& is,
                     const vcl_vector<vcl_string>& node_names,
                     vcl_vector<mmn_arc>& arcs)
{
    //First convert node_names to numbers
    vcl_map<vcl_string,unsigned> nodeMap;
    vcl_vector<vcl_string>::const_iterator nodeIter=node_names.begin();
    vcl_vector<vcl_string>::const_iterator nodeIterEnd=node_names.end();
    unsigned id=0;
    while(nodeIter != nodeIterEnd)
    {
        nodeMap[*nodeIter++]=id++;
    }

    vcl_string s = mbl_parse_block(is);
    vcl_istringstream ss(s);
    char c;
    ss>>c;  // Remove opening brace
    if (c!='{')
    {
        throw mbl_exception_parse_error("Expected '{' in mmn_parse_arcs");
    }
  
    arcs.clear();
    vcl_string arcLabel("arc:");
    //Loop over each arc in the stream
    while (!ss.eof())
    {
        //Locate the next "arc:" keyword
        if(vcl_find(vcl_istream_iterator<vcl_string>(ss),
                    vcl_istream_iterator<vcl_string>(),arcLabel) == 
           vcl_istream_iterator<vcl_string>())
        {
            //No more arc: labels to be processed
            break;
        }
        if(ss.bad())
        {
            vcl_string error_msg = "Stream error parsing arcs block\n";
            throw mbl_exception_parse_error(error_msg);
        }

        //we have read to just past the next "arc:" label
        vcl_string strNodePair=mbl_parse_block(ss);
        vcl_istringstream ssArc(strNodePair); //stream for just this arc

        //Now reduce this line/block into 4 tokens for { name1 name2 }
        //Note this assumes white space separators
        vcl_vector<vcl_string> tokens;
        vcl_copy(vcl_istream_iterator<vcl_string>(ssArc),
                 vcl_istream_iterator<vcl_string>(),
                 vcl_back_inserter(tokens));
        if(ssArc.bad())
        {
            vcl_string error_msg = "Stream error processing line "+strNodePair;
            throw mbl_exception_parse_error(error_msg);
        }
        if(tokens.size() != 4)
        {
            vcl_string error_msg = "Expected format is 4 tokens: { name1 name 2 }\n";
            error_msg+= " Got "+strNodePair;
            throw mbl_exception_parse_error(error_msg);
        }
        if(tokens[0] != "{" || tokens[3] != "}")
        {
            vcl_string error_msg = "Expected format is 4 tokens : { name1 name2 }\n";
            error_msg+= " Missing a (separated?) curly bracket in  "+strNodePair;
            throw mbl_exception_parse_error(error_msg);
        }

        //Seems to validate
        vcl_string node1=tokens[1];
        vcl_string node2=tokens[2];

        //Find the numeric IDs of these nodes
        vcl_map<vcl_string,unsigned>::const_iterator nodeIter1=nodeMap.find(node1);
        if(nodeIter1 == nodeMap.end())
        {
            vcl_string error_msg = "Node name "+node1+" is not in the list of nodes for arc\n";
            error_msg+= strNodePair;
            throw mbl_exception_parse_error(error_msg);
        }

        vcl_map<vcl_string,unsigned>::const_iterator nodeIter2=nodeMap.find(node2);
        if(nodeIter2 == nodeMap.end())
        {
            vcl_string error_msg = "Node name "+node2+" is not in the list of nodes for arc\n";
            error_msg+= strNodePair;
            throw mbl_exception_parse_error(error_msg);
        }
        
        //Finally add the arc
        arcs.push_back(mmn_arc(nodeIter1->second,nodeIter2->second));

    }
}
