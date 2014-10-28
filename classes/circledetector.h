#ifndef _CIRCLEDETECTOR_H
#define _CIRCLEDETECTOR_H

#define MATH_PI 3.141592653589793238462
#define NORMALIZE_W 360
#define NORMALIZE_H 40

#include "datagrid.h"
#include <cmath>
#include <vector>
#include <iostream>
#include <list>
using namespace std;

class Circle;
typedef list<Circle*> CircleList;

class CircleDetector {
public:
  CircleDetector();
  ~CircleDetector();

  Circle detectPupil(const DataGrid&, const DataGrid&, unsigned const int, unsigned const int);
  Circle detectIris(const DataGrid&, const Circle&);

  DataGrid sobel(const DataGrid&);
  DataGrid canny(const DataGrid&, const int, const int);
  DataGrid equalize(const DataGrid&);
  DataGrid threshold(const DataGrid&, unsigned const char);
  void drawCircle(DataGrid&, const Circle&, int);
  void drawLine(DataGrid&, int, int, int, int, int);

  DataGrid normalize(const DataGrid&, const Circle&, const Circle&);
  string getDataString(const DataGrid&, int, int);

protected:
  DataGrid gradient(const DataGrid&);
  DataGrid smoothing(const DataGrid&);
  DataGrid suppression(const DataGrid&, const int, const int);

  void accpixel(DataGrid&, int, int);
  void acccircle(DataGrid&, int, int, int);
  void circle(DataGrid&, int, int, int, int);
  bool bmatch(CircleList&, int*, int*, int*);

private:
  DataGrid _derX;
  DataGrid _derY;
};

class Circle {
public:
  Circle();
  Circle(const int, const int, const int);
  Circle(const Circle&);
  ~Circle();

  Circle& operator=(const Circle&);

  void calculate(const DataGrid&);

public:
  int x;
  int y;
  int radius;
  long light;
  long dark;
  long area;
  double rating;
};

typedef vector<DataGrid> DataGridVector;

#endif