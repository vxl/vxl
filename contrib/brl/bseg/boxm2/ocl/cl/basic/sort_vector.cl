//:
// \file
// \brief Register implementation for sorting an array )

//will sort 'numEl' of list in place
void sort_vector( float* list, int numEl )
{
  bool swapped = true;
  while(swapped)
  {
    swapped = false;
    for(int i=1; i<numEl; ++i) {
      if( list[i-1] > list[i] ) {

        //swap a[i-1] and a[i]
        float buff = list[i];
        list[i] = list[i-1];
        list[i-1] = buff;
        swapped = true;
      }
    }
  } //end while
}


