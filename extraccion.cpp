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
  bool estado;
 
  for (unsigned int clase = 0; clase <= 0; clase++) {
    for (unsigned int muestra = 0; muestra <= 9; muestra++) {
      
      estado = reader.extractFeatures(clase, muestra);

      if (!estado)
        cout << "Error con la extraccion de la clase " << clase << " muestra " << muestra << endl;
      else
        cout << "Extraccion de la imagen de la clase "<< clase << " y muestra " << muestra << " completada" << endl;  

    }
  }

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
