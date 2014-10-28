#include "classes/datagrid.h"
#include "classes/circledetector.h"
#include "classes/casiairissynreader.h"
#include "classes/associativememory.h"
#include <iostream>
#include <fstream>
using namespace std;

int main() {

  int conteo = 0;
  int aciertos = 0;
  int fallas = 0;
  int clases = 1000;
  //unsigned int *accumulador = new unsigned int[clases];

  //for (int i = 0; i < clases; i++) accumulador[i] = 0;



  CasiaIrisSynReader reader("/Users/wafto/Desktop/casia");
  FundamentalSet set = reader.fetchFundamentalSet(0, clases - 1, -1);
  AssociativeMemory mem(set, clases);
  bool status;

  
  for (unsigned int i = 0; i < clases; i++) {
    for (unsigned int j = 0; j <= 9; j++) {

      MemoryMatrix entrada = reader.fetchMemoryMatrix(i, j, true);
      
      int clase = -1;
      unsigned int minima = MAX_HAMMING;

      for (unsigned int i2 = 0; i2 < clases; i2++) {
        for (unsigned int j2 = 0; j2 <= 9; j2++) {

          if (i == i2 && j == j2) continue;

          MemoryMatrix candidata = reader.fetchMemoryMatrix(i2, j2, true);

          unsigned int distancia = mem.hammingDistance(entrada, candidata);
          if (distancia <= minima) {
            clase = i2;
            minima = distancia;
          }

        }
      }

      /*
      for (int k = 0; k < clases; k++) {
        if (accumulador[k] <= minima) {
          clase = k;
          minima = accumulador[k];
        }
      }
      */


      cout << "clase: " << i << ", muestra: " << j << ", encontrado: " << clase << ", coincide: " << (i == clase ? "SI" : "NO") << endl;
      
      conteo ++;
      
      if (i == clase)
        aciertos ++;
      else
        fallas ++;

    }
  }

  cout << "total: " << conteo << endl;
  cout << "aciertos: " << aciertos << endl;
  cout << "fallas: " << fallas << endl;
  

  return 0;
}
