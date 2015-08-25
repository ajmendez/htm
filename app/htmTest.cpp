
//dcdtmp #include <VarVecDef.h>
#include "SpatialInterface.h"
#include "SpatialDomain.h"
#include "VarStr.h"
#include "fstream.h"
#include <time.h>
#include <iostream.h>

int
main() {
/*******************************************************
  Initialization
 ******************************************************/

 bool verbose = false;		// verbosity flag
 bool count = false;		//countonly flag  
 bool onecol = false;		// single column output flag  
 bool text = false;		// text interface flag	
 
 int depth = 5;
 int savedepth = 2;

 SpatialIndex index(depth,savedepth);
    
    
 float64 avgArea = 0;
 float64 minArea = 100;
 float64 maxArea = 0;
 float64 leafArea;

 std::ofstream out("area.out");
 std::cout << index.leafCount() << std::endl;
 out.precision(10);

 for(size_t p = 0; p < index.leafCount(); p++) {
   leafArea = index.area(index.idByLeafNumber(p));
   out << index.nameByLeafNumber(p) << "   " << leafArea << std::endl;

   avgArea += leafArea;
   if(minArea > leafArea)minArea = leafArea;
   if(maxArea < leafArea)maxArea = leafArea;
 }
 std::cout << "Avg Area = " << avgArea / index.leafCount() << std::endl;
 std::cout << "Min Area = " << minArea << std::endl;
 std::cout << "Max Area = " << maxArea << std::endl;

 return 0;
} 

