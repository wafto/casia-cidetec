#include "classes/datagrid.h"
#include "classes/circledetector.h"
#include "classes/casiairissynreader.h"
#include "classes/associativememory.h"
#include <iostream>
#include <fstream>
using namespace std;

int main() {
  
  CasiaIrisSynReader reader("/Users/wafto/Desktop/casia");
  bool status;



  /* Memorias para 10 clases */ 
/*
  FundamentalSet fc = reader.fetchFundamentalSet(0, 9, -1);
  AssociativeMemory mem(fc, 10);
  MemoryMatrix alphaMin = mem.min(&status);
  alphaMin.save("/Users/wafto/Desktop/casia/alphas/MIN-000-009.mem");
  MemoryMatrix alphaMax = mem.max(&status);
  alphaMax.save("/Users/wafto/Desktop/casia/alphas/MAX-000-009.mem");
  reader.cleanFundamentalSetFetched(fc, true);
*/


  /* Memorias para 100 clases */
/*
  FundamentalSet fc = reader.fetchFundamentalSet(0, 99, -1);
  AssociativeMemory mem(fc, 100);
  MemoryMatrix alphaMin = mem.min(&status);
  alphaMin.save("/Users/wafto/Desktop/casia/alphas/MIN-000-099.mem");
  MemoryMatrix alphaMax = mem.max(&status);
  alphaMax.save("/Users/wafto/Desktop/casia/alphas/MAX-000-099.mem");
  reader.cleanFundamentalSetFetched(fc, true);
*/

  /* Memorias para 1000 clases */
 
  FundamentalSet fc = reader.fetchFundamentalSet(0, 999, -1);
  AssociativeMemory mem(fc, 1000);
  MemoryMatrix alphaMin = mem.min(&status);
  alphaMin.save("/Users/wafto/Desktop/casia/alphas/MIN-000-999.mem");
  MemoryMatrix alphaMax = mem.max(&status);
  alphaMax.save("/Users/wafto/Desktop/casia/alphas/MAX-000-999.mem");
  reader.cleanFundamentalSetFetched(fc, true);


  return 0;
}
