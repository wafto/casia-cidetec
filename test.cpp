#include "classes/datagrid.h"
#include "classes/circledetector.h"
#include "classes/casiairissynreader.h"
#include "classes/associativememory.h"
#include <iostream>
#include <fstream>
using namespace std;

bool saveRaw(const char*, const DataGrid&);

int main() {
  
  CasiaIrisSynReader reader("/Users/wafto/Desktop/casia");
 
  FundamentalSet fc = reader.fetchFundamentalSet(0, 9, 5);

  // Convertir el conjunto fundamental a unica por clase
  FundamentalSet fc2 = reader.makeFundamentalSetUnique(fc);

  cout << fc2.size() << endl;

  AssociativeMemory mem(fc2, 10);
  bool status;


  // Crea la matriz de aprendizaje alpha max exceptuando los samples 5
  MemoryMatrix alphaMin = mem.min(&status);
  alphaMin.save("/Users/wafto/Desktop/casia/alphas/MIN0010-5.mem");


  //MemoryMatrix alphaMax;
  //alphaMax.load("/Users/wafto/Desktop/casia/alphas/MAX0010-5.mem");

  //MemoryMatrix alphaMin;
  //alphaMin.load("/Users/wafto/Desktop/casia/alphas/MIN0010-5.mem");


  for (unsigned int i = 0; i <= 9; i++) {
    for (unsigned int j = 0; j <= 9; j++) {
      MemoryMatrix entrada = reader.fetchMemoryMatrix(i, j, true);
      MemoryMatrix beta = mem.beta(alphaMin, entrada);
      MemoryMatrix recuperacion = mem.betaMax(beta);

      RecoveredClasses classes = mem.recovery(recuperacion);

      cout << "[" << i << " ," << j << "] " << "encontrados(" << classes.size() << ") coincide(" << classes.front() << ")";
      if (i == classes.front()){
        //cout << "+" << endl;
        cout << endl;
      } else {
        cout << "-" << endl;
      }
    }
  }

  reader.cleanFundamentalSetFetched(fc, true);

  return 0;
}

bool saveRaw(const char* filename, const DataGrid& grid) {
  ofstream outbin(filename, ios::binary);
  int value;
  if (outbin) {
    for (unsigned int y = 0; y < grid.height(); y++) {
      for (unsigned int x = 0; x < grid.width(); x++) {
        value = grid.at(x, y);
        outbin.put((char) value);
      }
    }
    outbin.close();
    return true;
  }
  return false;
}
