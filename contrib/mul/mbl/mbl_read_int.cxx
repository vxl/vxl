#ifdef __GNUC__
#pragma implemantation
#endif

//:
// \file
// \brief Asks question and waits for an answer
// \author tim, updated by me apparently!!!
//  updated to vxl caringly in the way one can only do on a Friday afternoon by gvw
//	Function Name:	mbl_read_int
//	Synopsis:	int mbl_read_int(char* q_str, int default_int)
//	Inputs:		q_str: A question 
//			default_int: Default answer
//			min_int: Min allowed value (optional)
//			max_int: Max allowed value (optional)
//	Outputs:	-
//	Returns:	The answer or a default
//	Description:	Asks question and waits for an answer.
//			If the answer is an integer, returns it.
//			If the answer is an empty vcl_string (return)
//			then returns default.
//			Otherwise waits for another input.
//	References: -
//	Example: 
//  int new_scale = mbl_read_int("Scale?",5);
//  int new_scale = mbl_read_int("Scale?",5,min_scale,max_scale);


//#include <cstdio>
//


#include <mbl/mbl_read_int.h>

const int MAX_LEN = 20;

int RD_ReadInt1(char* q_str, int default_int, 
				int min_int, int max_int)
// If min_int != 0 or max_int != 0 then
// Displays range but doesn't check it.
{
	int r;
	int got_int = 0;   /* Set to 1 when find result */

	char reply[MAX_LEN];

	do 
	{
		if (min_int==0 && max_int==0)
		{
			vcl_cout<<q_str<<" ("<<default_int<<") :";
		}
		else
		{
			vcl_cout<<q_str<<" ["<<min_int<<".."<<max_int;
			vcl_cout<<"] ("<<default_int<<") :";
		}
		vcl_cout.flush();

		if (fgets(reply,MAX_LEN,stdin)!=NULL)
		{
			if (reply[0]=='\n') 
			{
				r = default_int;
				got_int = 1;
			}
	    	else if (sscanf(reply,"%d",&r)>0) got_int = 1;
		}
	}
	while (got_int==0);
	return r;
}


int mbl_read_int(char* q_str, int default_int)
{
	return RD_ReadInt1(q_str,default_int,0,0);
}
    
int mbl_read_int(char* q_str, int default_int, 
				int min_int, int max_int)
{
	int R;
	int acceptable = 0;
	
	do
	{
		R = RD_ReadInt1(q_str,default_int,min_int,max_int);
		if (R<min_int)
			vcl_cout<<"Must be at least "<<min_int<<"\n";
		else
		if (R>max_int)
			vcl_cout<<"Must be no more than "<<max_int<<"\n";
		else
			acceptable = 1;
	}
	while (!acceptable);

	return R;
}
