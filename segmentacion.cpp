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
  bool estado = false;
  unsigned int clase, muestra;
  CircleDetector detector;

  cout << "Clase: "; cin >> clase;
  cout << "Muestra: "; cin >> muestra;

  if (clase < 0 || clase > 999 || muestra < 0 || muestra > 9) {
    cout << "Clase debe estar entre 0 y 999 y muestra entre 0 a 9.";
    return 0;
  }

  cout << "Paso 1: Lectura de la imagen." << endl;
  DataGrid original = reader.readImage(clase, muestra, &estado);
  saveRaw("/Users/wafto/Desktop/1-original.raw", original);

  cout << "Paso 2: Detección de bordes Canny con umbrales de 60 y 200." << endl;
  DataGrid canny = detector.canny(original, 60, 200);
  saveRaw("/Users/wafto/Desktop/2-canny_60_200.raw", canny);

  cout << "Paso 3: Detección de la pupila. radio min: 35, radio max: 60." << endl;
  Circle pupil = detector.detectPupil(original, canny, 35, 60);
  DataGrid original2(original);
  detector.drawCircle(original2, pupil, 255);
  saveRaw("/Users/wafto/Desktop/3-pupila.raw", original2);

  cout << "Paso 4: Ecualización de la imagen original:" << endl;
  DataGrid equalized = detector.equalize(original);
  saveRaw("/Users/wafto/Desktop/4-ecualizacion.raw", equalized);

  cout << "Paso 5: Detección de bordes Canny con umbrales de 50 y 150 sobre la imagen ecualizada. " << endl;
  DataGrid cc = detector.canny(equalized, 50, 150);
  saveRaw("/Users/wafto/Desktop/5-canny_50_150.raw", cc);

  cout << "Paso 6: Detección del iris. usando los valores (x, y, z) de la pupila." << endl;
  Circle iris = detector.detectIris(cc, pupil);
  DataGrid original3(original);
  detector.drawCircle(original3, iris, 255);
  saveRaw("/Users/wafto/Desktop/6-iris.raw", original3);

  cout << "Paso 7: Normalización de la imagen original con pupila y radio." << endl;
  DataGrid normal = detector.normalize(original, pupil, iris);
  saveRaw("/Users/wafto/Desktop/7-normalizada.raw", normal);

  cout << "Paso 8; Ecualización de la imagen normalizada." << endl;
  DataGrid normaleq = detector.equalize(normal);
  saveRaw("/Users/wafto/Desktop/8-normalizada_ecualizada.raw", normaleq);

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



  


