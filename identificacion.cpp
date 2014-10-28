#include "classes/datagrid.h"
#include "classes/circledetector.h"
#include "classes/casiairissynreader.h"
#include "classes/associativememory.h"
#include <iostream>
#include <fstream>
using namespace std;

int main() {
  
  CasiaIrisSynReader reader("/Users/wafto/Desktop/casia");
 
  /* Memorias Asocoativas */
  MemoryMatrix alphaMaxU, alphaMaxD, alphaMaxC;
  alphaMaxU.load("/Users/wafto/Desktop/casia/alphas/MAX-000-009.mem");
  //alphaMaxD.load("/Users/wafto/Desktop/casia/alphas/MAX-000-099.mem");
  //alphaMaxC.load("/Users/wafto/Desktop/casia/alphas/MAX-000-999.mem");

  //MemoryMatrix alphaMinU, alphaMinD, alphaMinC;
  //alphaMinU.load("/Users/wafto/Desktop/casia/alphas/MIN-000-009.mem");
  //alphaMinD.load("/Users/wafto/Desktop/casia/alphas/MIN-000-099.mem");
  //alphaMinC.load("/Users/wafto/Desktop/casia/alphas/MIN-000-999.mem");


  /* Analisis de la Memoria Asociativa MAX desde 000 hasta 009 */
  unsigned int clases = 10;
  unsigned int analizados = 0;
  unsigned int correctos = 0;
  unsigned int incorrectos = 0;


  FundamentalSet fset = reader.fetchFundamentalSet(0, clases - 1, -1);
  AssociativeMemory mem(fset, clases);
  cout << "clase" << ", " << "muestra" << ", " << "candidata" << ", " << "coincidencia" << ", " << "identificacion" << endl;
  for (unsigned int clase = 0; clase < clases; clase++) {
    for (unsigned int muestra = 0; muestra <= 9; muestra++) {
   
      MemoryMatrix entrada = reader.fetchMemoryMatrix(clase, muestra, true);
      MemoryMatrix beta = mem.beta(alphaMaxU, entrada);
      MemoryMatrix recuperacion = mem.betaMin(beta);

      RecoveredClasses classes = mem.recovery(recuperacion);


      bool coincidencia = (muestra == classes.front()) ? true : false;
      bool edoidentificaion = clase == classes.front() ? true : false;

      analizados += 1;
      if (edoidentificaion)
        correctos += 1;
      else
        incorrectos += 1;

      cout << clase << ", " << muestra << ", " << classes.front() << ", " << (coincidencia? "verdadero" : "falso") << ", "
           << (edoidentificaion? "correcta" : "incorrecta")  << endl;
    }
  }

  cout << "analizados: , " << analizados << endl;
  cout << "correctos: , " << correctos << endl;
  cout << "incorrectos: , " << incorrectos << endl;


  return 0;
}
