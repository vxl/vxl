#ifdef __GNUC__
#pragma implemantation
#endif

//:
// \file
// \brief Description!
// \author tim
// hand crafted into vxl by gvw
// 
//  Function Name:  mbl_read_double
//  Synopsis: double mbl_read_double(char* q_str, double default_d)
//  Inputs:   q_str: A question 
//      default_d: Default answer
//        min_d: Min allowed value (optional)
//        max_d: Max allowed reply (optional)
//  Outputs:  -
//  Returns:    The answer or a default
//  Description:  Asks question and waits for an answer.
//      If the answer is a double, returns it.
//      If the answer is an empty vcl_string (return)
//      then returns default.
//      Otherwise waits for another input.
//  References: -
//  Example: 
//    new_scale = mbl_read_double("Scale?",1.00);
//    double new_scale = mbl_read_double("Scale?",1.00,min_scale,max_scale);

//#include <cstdio>
//

#include <mbl/mbl_read_double.h>

const int MAX_LEN = 40;

double RD_ReadDouble1(char *q_str, double default_d,
					double min_d, double max_d)
// If min_d != 0 or max_d != 0 then
// Prints range but doesn't check that reply is in range
{
     double r;
     int got_double = 0;   /* Set to 1 when find result */

     char reply[MAX_LEN];

     do 
	 {
		if (min_d==0 && max_d==0)
		{
			vcl_cout<<q_str;
			vcl_cout<<" ("<<default_d<<") :";
		}
		else
		{
			vcl_cout<<q_str;
			vcl_cout<<" ["<<min_d<<".."<<max_d<<"] ("<<default_d<<") :";
		}
		vcl_cout.flush();

		if (fgets(reply,MAX_LEN,stdin)!=NULL)
		{
	    	if (reply[0]=='\n') 
			{
	        	r = default_d;
				got_double = 1;
			}
	    	else if (sscanf(reply,"%lf",&r)>0) got_double = 1;
	    }
	 }
     while (got_double==0);
     return r;
}

double mbl_read_double(char *q_str, double default_d)
{
	return RD_ReadDouble1(q_str,default_d,0,0);
}

double mbl_read_double(	char *q_str, 
					double default_d,
					double min_d,
					double max_d)
{
	int acceptable = 0;
	double R;
	do
	{
		R = RD_ReadDouble1(q_str,default_d,min_d,max_d);
		if (R<min_d)
			vcl_cout<<"Must be at least "<<min_d<<"\n";
		else
		if (R>max_d)
			vcl_cout<<"Must be no more than "<<max_d<<"\n";
		else acceptable = 1;
	}
	while (!acceptable);
	return R;
}
	

