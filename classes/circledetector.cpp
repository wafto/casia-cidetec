#include "circledetector.h"

CircleDetector::CircleDetector() {}

CircleDetector::~CircleDetector() {}

DataGrid CircleDetector::threshold(const DataGrid& source, unsigned const char u) {
  DataGrid result(source.width(), source.height());
  int value = (int) u;
  for (unsigned int x = 0; x < source.width(); x++)
    for (unsigned int y = 0; y < source.height(); y++)
      result.set(x, y, source.at(x, y) >= value ? 255 : 0);
  return result;
}

DataGrid CircleDetector::equalize(const DataGrid& source) {
  DataGrid equalized(source.width(), source.height());
  long lut[256] = {0};
  double s_hist_eq[256] = {0.0};
  double sum_of_hist[256] = {0.0};
  long state_hst[256] = {0};
  long n = source.width() * source.height();

  for (unsigned int x = 0; x < source.width(); x++)
    for (unsigned int y = 0; y < source.height(); y++)
      if (source.at(x, y) >= 0 && source.at(x, y) < 256) lut[source.at(x, y)] += 1;

  for (long i = 0; i < 256; i++) s_hist_eq[i] = (double) lut[i] / (double) n;
  sum_of_hist[0] = s_hist_eq[0];
  for (long i = 1; i < 256; i++) sum_of_hist[i] = sum_of_hist[i - 1] + s_hist_eq[i];
  for (unsigned int x = 0; x < source.width(); x++) {
    for (unsigned int y = 0; y < source.height(); y++) {
      int value = source.at(x, y);
      if (value < 0) value = 0;
      if (value > 255) value = 255;
      equalized.set(x, y, round(sum_of_hist[value] * 255.0));
    }
  }
  return equalized;
}

Circle CircleDetector::detectPupil(const DataGrid& source, const DataGrid& edges, unsigned const int rmin, unsigned const int rmax) {
  Circle res;
  int min_r = rmin;
  int max_r = rmax;
  if (min_r == 0) min_r = 15;
  if (max_r == 0) max_r = (edges.width() < edges.height() ? edges.width() : edges.height()) / 2;

  if (min_r > max_r) {
    int aux = max_r;
    max_r = min_r;
    min_r = aux;
  }

  DataGridVector houghs(max_r - min_r);
  CircleList circles;
  Circle* aux;
  for (unsigned int i = min_r; i < max_r; i++) {
    houghs[i - min_r] = DataGrid(edges.width(), edges.height(), 0);
    DataGrid& hough = houghs[i - min_r];

    for (unsigned int x = 0; x < edges.width(); x++)
      for (unsigned int y = 0; y < edges.height(); y++)
        if (edges.at(x, y) != 0) acccircle(hough, x, y, i);

    int max = hough.at(0, 0);
    for (unsigned int x = 0; x < hough.width(); x++)
      for (unsigned int y = 0; y < hough.height(); y++)
        if (hough.at(x, y) > max) max = hough.at(x, y);
    
    int treshold = max;
    for (unsigned int x = 0; x < hough.width(); x++) {
      for (unsigned int y = 0; y < hough.height(); y++) {
        if (hough.at(x, y) >= treshold) {
          aux = new Circle(x, y, i);
          if (aux) {
            aux->calculate(source);
            circles.push_back(aux);
          }
        }
      }
    }
  }
  int bx, by, br;
  if (bmatch(circles, &bx, &by, &br)) {
    res.x = bx;
    res.y = by;
    res.radius = br;
  }
  CircleList::const_iterator iterator;
  for (iterator = circles.begin(); iterator != circles.end(); ++iterator)
    delete (*iterator);
  return res;
}

Circle CircleDetector::detectIris(const DataGrid& edges, const Circle& pupil) {
  Circle res;
  if (pupil.radius > 0) {
    int mind = round(pupil.radius * 1.5);
    int maxd = round(pupil.radius * 2.5);
    int lutsize = maxd - mind;
    int distance;
    int* lut = new int[lutsize];
    if (lut) {
      for (int i = 0; i < lutsize; i++) lut[i] = 0;
      for (int x = 0; x < edges.width(); x++) {
        for(int y = 0; y < edges.height(); y++) {
          if (edges.at(x, y) != 0) {
            distance = round(sqrt((x - pupil.x) * (x - pupil.x) + (y - pupil.y) * (y - pupil.y)));
            if (distance >= mind && distance < maxd) lut[distance - mind] += 1;
          }
        }
      }
      res.x = pupil.x;
      res.y = pupil.y;
      int best = 0;
      for (int i = 1; i < lutsize; i++)
        if (lut[i] > lut[best]) best = i;
      res.radius = best + mind;
      delete [] lut;
    }
  }
  return res;
}

bool CircleDetector::bmatch(CircleList& circles, int* xc, int* yc, int* r) {
  int size = circles.size();
  if (size < 1) return false;
  CircleList sorted;
  Circle *circle, *front;
  CircleList::const_iterator iterator;
  bool inserted = false;
  double tolerance = 0.85;

  for (iterator = circles.begin(); iterator != circles.end(); ++iterator) {
    circle = *iterator;
    if (!inserted) {
      sorted.push_front(circle);
      inserted = true;
    } else {
      front = sorted.front();
      if (circle->rating >= tolerance) {
        if (circle->radius >= front->radius)
          sorted.push_front(circle);
      } else {
        if (circle->rating > front->rating)
          sorted.push_front(circle);
      }
    }
  }
  front = sorted.front();
  *xc = front->x;
  *yc = front->y;
  *r  = front->radius;
  return true;
}

void CircleDetector::accpixel(DataGrid& img, int x, int y) {
  img.set(x, y, img.at(x, y) + 1);
}

void CircleDetector::acccircle(DataGrid& img, int xp, int yp, int radius) {
  int f = 1 - radius;
  int ddF_x = 1;
  int ddF_y = -2 * radius;
  int x = 0;
  int y = radius;
 
  accpixel(img, xp, yp + radius);
  accpixel(img, xp, yp - radius);
  accpixel(img, xp + radius, yp);
  accpixel(img, xp - radius, yp);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;    
    accpixel(img, xp + x, yp + y);
    accpixel(img, xp - x, yp + y);
    accpixel(img, xp + x, yp - y);
    accpixel(img, xp - x, yp - y);
    accpixel(img, xp + y, yp + x);
    accpixel(img, xp - y, yp + x);
    accpixel(img, xp + y, yp - x);
    accpixel(img, xp - y, yp - x);
  }
}

DataGrid CircleDetector::normalize(const DataGrid& source, const Circle& pupil, const Circle& iris) {
  DataGrid res(NORMALIZE_W, NORMALIZE_H);
  if (pupil.radius < iris.radius && pupil.x == iris.x && pupil.y == iris.y) {    
    double scale = (double) (iris.radius - pupil.radius) / (double) res.height();
    double angle = 360.00 / (double) res.width();

    for (int x = 0; x < res.width(); x++) {
      double theta = angle * (double) x * MATH_PI / 180.00;
      for(int y = 0; y < res.height(); y++) {
        int nradius = round(y * scale + pupil.radius);     
        int sx = iris.x + round(nradius * sin(theta));
        int sy = iris.y + round(nradius * cos(theta));
        res.set(res.width() - 1 - x, res.height() - 1 - y, source.at(sx, sy));
      }
    }
  }
  return res;
}

string CircleDetector::getDataString(const DataGrid& normalized, int faces, int face) {
  string data;
  if (normalized.width() == NORMALIZE_W && normalized.height() == NORMALIZE_H && faces > 0) {
    int fsize = round(NORMALIZE_W / faces);
    data.resize(fsize * NORMALIZE_H, '?');
    if (face > 0 && face <= faces) {
      int k = 0;
      for (int iw = (face - 1) * fsize, fw = iw + fsize; iw < fw; iw++) {
        for (int ih = 0; ih < NORMALIZE_H; ih++) {
          data[k++] = (normalized.at(iw, ih) == 255) ? '1' : '0';
        }
      }
    }
  }
  return data;
}

void CircleDetector::drawLine(DataGrid& img, int p1x, int p1y, int p2x, int p2y, int cpixel) {
  int F, x, y;

  if (p1x > p2x) {
    swap(p1x, p2x);
    swap(p1y, p2y);
  }

  if (p1x == p2x) {
    if (p1y > p2y) swap(p1y, p2y);
    x = p1x;
    y = p1y;  
    while (y <= p2y) {
      img.set(x, y, cpixel);
      y++;
    }
    return;
  } else if (p1y == p2y) {
    x = p1x;
    y = p1y;
    while (x <= p2x) {
      img.set(x, y, cpixel);
      x++;
    }
    return;
  }

  int dy            = p2y - p1y;
  int dx            = p2x - p1x;
  int dy2           = (dy << 1);
  int dx2           = (dx << 1);
  int dy2_minus_dx2 = dy2 - dx2;
  int dy2_plus_dx2  = dy2 + dx2;

  if (dy >= 0) {
    if (dy <= dx) {
      F = dy2 - dx;
      x = p1x;
      y = p1y;
      while (x <= p2x) {
        img.set(x, y, cpixel);
        if (F <= 0) {
          F += dy2;
        } else {
          y++;
          F += dy2_minus_dx2;
        }
        x++;
      }
    } else {
      F = dx2 - dy;
      y = p1y;
      x = p1x;
      while (y <= p2y) {
        img.set(x, y, cpixel);
        if (F <= 0) {
          F += dx2;
        } else {
          x++;
          F -= dy2_minus_dx2;
        }
        y++;
      }
    }
  } else {
    if (dx >= -dy) {
      F = -dy2 - dx;
      x = p1x;
      y = p1y;
      while (x <= p2x) {
        img.set(x, y, cpixel);
        if (F <= 0) {
          F -= dy2;
        } else {
          y--;
          F -= dy2_plus_dx2;
        }
        x++;
      }
    } else {
      F = dx2 + dy;
      y = p1y;
      x = p1x;
      while (y >= p2y) {
        img.set(x, y, cpixel);
        if (F <= 0) {
          F += dx2;
        } else {
          x++;
          F += dy2_plus_dx2;
        }
        y--;
      }
    }
  }
}

void CircleDetector::drawCircle(DataGrid& img, const Circle& c, int cpixel) {
  circle(img, c.x, c.y, c.radius, cpixel);
}

void CircleDetector::circle(DataGrid& img, int xp, int yp, int radius, int cpixel) {
  int f = 1 - radius;
  int ddF_x = 1;
  int ddF_y = -2 * radius;
  int x = 0;
  int y = radius;
 
  img.set(xp, yp + radius, cpixel);
  img.set(xp, yp - radius, cpixel);
  img.set(xp + radius, yp, cpixel);
  img.set(xp - radius, yp, cpixel);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;    
    img.set(xp + x, yp + y, cpixel);
    img.set(xp - x, yp + y, cpixel);
    img.set(xp + x, yp - y, cpixel);
    img.set(xp - x, yp - y, cpixel);
    img.set(xp + y, yp + x, cpixel);
    img.set(xp - y, yp + x, cpixel);
    img.set(xp + y, yp - x, cpixel);
    img.set(xp - y, yp - x, cpixel);
  }
}

DataGrid CircleDetector::sobel(const DataGrid& source) {
  return gradient(smoothing(source));
}

DataGrid CircleDetector::canny(const DataGrid& source, const int fimin, const int fimax) {
  return suppression(sobel(source), fimin, fimax);
}

DataGrid CircleDetector::gradient(const DataGrid& source) {
  DataGrid result(source.width(), source.height());
  _derX = DataGrid(source.width(), source.height());
  _derY = DataGrid(source.width(), source.height());
  double dx, dy;
  int sobelX[9][3] = {
    {-1, -1, -1}, {-1, 0, 0}, {-1, 1, 1},
    { 0, -1, -2}, { 0, 0, 0}, { 0, 1, 2},
    { 1, -1, -1}, { 1, 0, 0}, { 1, 1, 1}
  };
  int sobelY[9][3] = {
    {-1, -1,  1}, {-1, 0,  2}, {-1, 1,  1},
    { 0, -1,  0}, { 0, 0,  0}, { 0, 1,  0},
    { 1, -1, -1}, { 1, 0, -2}, { 1, 1, -1}
  };
  for (unsigned int y = 0; y < source.height(); y++) {
    for (unsigned int x = 0; x < source.width(); x++) {
      dx = dy = 0;
      for (unsigned int m = 0; m < 9; m++) {
        if (source.in(x + sobelX[m][0], y + sobelX[m][1]))
          dx += source.at(x + sobelX[m][0], y + sobelX[m][1]) * sobelX[m][2];
        if (source.in(x + sobelY[m][0], y + sobelY[m][1]))
          dy += source.at(x + sobelY[m][0], y + sobelY[m][1]) * sobelY[m][2];
      }
      _derX.set(x, y, dx);
      _derY.set(x, y, dy);
      result.set(x, y, abs(dx) + abs(dy));
    }
  }
  return result;
}

DataGrid CircleDetector::smoothing(const DataGrid& source) {
  DataGrid result(source.width(), source.height());
  int sum, den;
  int mask[25][3] = {
    {-2, -2, 2}, {-2, -1,  4}, {-2, 0,  5}, {-2, 1,  4}, {-2, 2, 2},
    {-1, -2, 4}, {-1, -1,  9}, {-1, 0, 12}, {-1, 1,  9}, {-1, 2, 4},
    { 0, -2, 5}, { 0, -1, 12}, { 0, 0, 15}, { 0, 1, 12}, { 0, 2, 5},
    { 1, -2, 4}, { 1, -1,  9}, { 1, 0, 12}, { 1, 1,  9}, { 1, 2, 4},
    { 2, -2, 2}, { 2, -1,  4}, { 2, 0,  5}, { 2, 1,  4}, { 2, 2, 2}
  };
  for (unsigned int y = 0; y < source.height(); y++) {
    for (unsigned int x = 0; x < source.width(); x++) {
      sum = 0;
      den = 159;
      for (unsigned int m = 0; m < 25; m++) {
        if (source.in(x + mask[m][0], y + mask[m][1]))
          sum += source.at(x + mask[m][0], y + mask[m][1]) * mask[m][2];
        else
          den -= mask[m][2];
      }
      if (den != 0) result.set(x, y, sum / den);
    }
  }
  return result;
}

DataGrid CircleDetector::suppression(const DataGrid& source, const int fimin, const int fimax) {
  DataGrid result(source.width(), source.height());
  double tng;
  for (unsigned int y = 1; y < source.height() - 1; y++) {
    for (unsigned int x = 1; x < source.width() - 1; x++) {
      tng = (_derX.at(x, y) == 0) ? 90 : atan(_derY.at(x, y) / _derX.at(x, y) * 180 / MATH_PI);
      int gc = source.at(x, y);
      int gn = source.at(x, y - 1);
      int gs = source.at(x, y + 1);
      int ge = source.at(x + 1, y);
      int go = source.at(x - 1, y);
      int gse = source.at(x - 1, y + 1);
      int gno = source.at(x + 1, y - 1);
      int gne = source.at(x + 1, y + 1);
      int gso = source.at(x - 1, y - 1);
      // Horizontal
      if ((-22.5 < tng && tng <= 22.5) || (157.5 < tng && tng <= -157.5)) {
        if (gc < ge || gc < go)
          result.set(x, y, 0);
        else
          result.set(x, y, (gc > fimin && gc < fimax)? 255 : 0);
      }
      // Vertical
      if ((-112.5 < tng && tng <= -67.5) || (67.5 < tng && tng <= 112.5)) {
        if (gc < gs || gc < gn)
          result.set(x, y, 0);
        else
          result.set(x, y, (gc > fimin && gc < fimax)? 255 : 0);
      }
      // +45 Grados
      if ((-67.5 < tng && tng <= -22.5) || (112.5 < tng && tng <= 157.5)) {
        if (gc < gse || gc < gno)
          result.set(x, y, 0);
        else
          result.set(x, y, (gc > fimin && gc < fimax)? 255 : 0);
      }
      // -45 Grados
      if ((-157.5 < tng && tng <= -112.5) || (67.5 < tng && tng <= 22.5)) {
        if (gc < gne || gc < gso)
          result.set(x, y, 0);
        else
          result.set(x, y, (gc > fimin && gc < fimax)? 255 : 0);
      }
    }
  }
  return result;
}

// Circle class

Circle::Circle() {
  x = y = radius = 0;
  light = dark = area = 0;
  rating = 0.0;
}

Circle::Circle(const int xc, const int yc, const int r) {
  x = xc;
  y = yc;
  radius = r;
  light = dark = area = 0;
  rating = 0.0;
}

Circle::Circle(const Circle& other) {
  x = other.x;
  y = other.y;
  radius = other.radius;
  light = other.light;
  dark = other.dark;
  area = other.area;
  rating = other.rating;
}

Circle::~Circle(){}

Circle& Circle::operator=(const Circle& other) {
  if (this != &other) {
    x = other.x;
    y = other.y;
    radius = other.radius;
    light = other.light;
    dark = other.dark;
    area = other.area;
    rating = other.rating;
  }
  return *this;
}

void Circle::calculate(const DataGrid& source) {
  if (radius <= 0) return;
  light = dark = area = 0;
  int r2 = radius * radius;
  for (int yy = -radius; yy <= radius; yy++) {
    for (int xx = -radius; xx <= radius; xx++) {
      if (xx * xx + yy * yy <= r2) {
        if (source.in(x + xx, y + yy)) {
          if (source.at(x + xx, y + yy) < 15)
            dark += 1;
          else
            light += 1;
        }
        area += 1;
      }
    }
  }
  if (area > 0)
    rating = (double) dark / (double) area;
  else
    rating = 0.0;
}

