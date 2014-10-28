#include "associativememory.h"


AssociativeMemory::AssociativeMemory(FundamentalSet& data, unsigned int cl) {
  CF = &data;
  classes = cl;
}

AssociativeMemory::~AssociativeMemory() {
  CF = 0;
  classes = 0;
}

unsigned int AssociativeMemory::hammingDistance(const MemoryMatrix& m, const MemoryMatrix& t) const {
  unsigned int distance = MAX_HAMMING;
  if (m.cols() == 1 && t.cols() == 1 && m.rows() == t.rows()) {
    distance = 0;
    for (unsigned int i = 0; i < m.rows(); i++)
      if (m.get(i, 0) != t.get(i, 0)) distance += 1;
  }
  return distance;
}

RecoveredClasses AssociativeMemory::recovery(const MemoryMatrix& m) const {
  RecoveredClasses results;
  if (classes > 0) {
    vector<int> lut(classes, -1);
    unsigned int distance;
    for (FundamentalSet::iterator it = CF->begin(); it != CF->end(); ++it) {
        OrderedCouple* oc = *it;
        distance = hammingDistance(m, *(oc->y));
        if (oc->classname < classes) {
          if (lut[oc->classname] == -1)
            lut[oc->classname] = distance;
          else
            lut[oc->classname] += distance;
        }
    }
    results.push_front(0);
    for (unsigned int i = 1; i < classes; i++) {
      if (lut[i] != -1) {
        if (lut[results.front()] == -1 || lut[i] < lut[results.front()]) {
          results.clear();
          results.push_front(i);
        } else {
          if (lut[i] == lut[results.front()])
            results.push_back(i);
        }
      }
    }
    if (results.front() == 0 && lut[0] == -1)
      results.clear();
  }
  return results;
}

MemoryMatrix AssociativeMemory::alpha(const MemoryMatrix& m, const MemoryMatrix& t) const {
  MemoryMatrix res;
  if (m.cols() == 1 && t.rows() == 1) {
    res.resize(m.rows(), t.cols());
    for (unsigned int i = 0; i < res.rows(); i++) {
      for (unsigned int j = 0; j < res.cols(); j++) {
        if (m.get(i, 0) == t.get(0, j))
          res.set(i, j, (unsigned char) 1);
        else
          res.set(i, j, (unsigned char) (m.get(i, 0) == 1 ? 2 : 0));
      }
    }
  }
  return res;
}

MemoryMatrix AssociativeMemory::beta(const MemoryMatrix& m, const MemoryMatrix& t) const {
  MemoryMatrix res;
  if (t.cols() == 1 && t.rows() == m.cols()) {
    res.resize(m.rows(), m.cols());
    for (unsigned int i = 0; i < res.rows(); i++) {
      for (unsigned int j = 0; j < res.cols(); j++) {
        switch (m.get(i, j)) {
          case 1:
            if (t.get(j, 0) == 1)
              res.set(i, j, (unsigned char) 1);
            else
              res.set(i, j, (unsigned char) 0);
            break;
          case 2:
            res.set(i, j, (unsigned char) 1);
            break;
          default:
            res.set(i, j, (unsigned char) 0);
        }
      }
    }
  }
  return res;
}

MemoryMatrix AssociativeMemory::betaMin(const MemoryMatrix& b) const {
  MemoryMatrix res(b.rows(), 1, 255);
  for (unsigned int i = 0; i < b.rows(); i++) {
    for (unsigned int j = 0; j < b.cols(); j++) {
      unsigned char actual = res.get(i, 0);
      unsigned char p = b.get(i, j);
      if (p < actual)
        res.set(i, 0, p);
    }
  }
  return res;
}

MemoryMatrix AssociativeMemory::betaMax(const MemoryMatrix& b) const {
  MemoryMatrix res(b.rows(), 1, 0);
  for (unsigned int i = 0; i < b.rows(); i++) {
    for (unsigned int j = 0; j < b.cols(); j++) {
      unsigned char actual = res.get(i, 0);
      unsigned char p = b.get(i, j);
      if (p > actual)
        res.set(i, 0, p);
    }
  }
  return res;
}

MemoryMatrix AssociativeMemory::max(bool* flag) const {
  MemoryMatrix res, aux;
  *flag = false;
  if (CF->size() > 0) {

    bool passed = false;

    for (FundamentalSet::iterator it = CF->begin(); it != CF->end(); ++it) {
      OrderedCouple* oc = *it; 
      if (!passed) {
        res = alpha(*oc->y, (*oc->x).transpose());
        passed = true;
        *flag = true;
        if (res.cols() == 0 || res.rows() == 0) {
          *flag = false;
          break;
        }
      } else {
        aux = alpha(*oc->y, (*oc->x).transpose());
        if (res.rows() != aux.rows() || res.cols() != aux.cols()) {
          *flag = false;
          break;
        }
        for (unsigned int i = 0; i < aux.rows(); i++) {
          for (unsigned int j = 0; j < aux.cols(); j++) {
            if (aux.get(i, j) > res.get(i, j)) res.set(i, j, aux.get(i, j));            
          }
        }
        *flag = true;
      }
    }
  }
  return res;
}

MemoryMatrix AssociativeMemory::min(bool* flag) const {
  MemoryMatrix res, aux;
  *flag = false;
  if (CF->size() > 0) {

    bool passed = false;

    for (FundamentalSet::iterator it = CF->begin(); it != CF->end(); ++it) {
      OrderedCouple* oc = *it; 
      if (!passed) {
        res = alpha(*oc->y, (*oc->x).transpose());
        passed = true;
        *flag = true;
        if (res.cols() == 0 || res.rows() == 0) {
          *flag = false;
          break;
        }
      } else {
        aux = alpha(*oc->y, (*oc->x).transpose());
        if (res.rows() != aux.rows() || res.cols() != aux.cols()) {
          *flag = false;
          break;
        }
        for (unsigned int i = 0; i < aux.rows(); i++) {
          for (unsigned int j = 0; j < aux.cols(); j++) {
            if (aux.get(i, j) < res.get(i, j)) res.set(i, j, aux.get(i, j));            
          }
        }
        *flag = true;
      }
    }
  }
  return res;
}

MemoryMatrix::MemoryMatrix() {
  _rows = 0;
  _cols = 0;
  _data = 0;
}

MemoryMatrix::MemoryMatrix(unsigned int r, unsigned int c) {
  _rows = 0;
  _cols = 0;
  _data = 0;
  resize(r, c);
}

MemoryMatrix::MemoryMatrix(unsigned int r, unsigned int c, unsigned char value) {
  _rows = 0;
  _cols = 0;
  _data = 0;
  resize(r, c);
  for (unsigned int i = 0; i < rows(); i++)
    for (unsigned int j = 0; j < cols(); j++)
      _data[i][j] = value;
}

MemoryMatrix::MemoryMatrix(unsigned int r) {
  _rows = 0;
  _cols = 0;
  _data = 0;
  resize(r, 1);
}

MemoryMatrix::MemoryMatrix(const MemoryMatrix& mem) {
  _rows = 0;
  _cols = 0;
  _data = 0;
  if (resize(mem.rows(), mem.cols())) {
    for (unsigned int i = 0; i < mem.rows(); i++)
      for (unsigned int j = 0; j < mem.cols(); j++)
        _data[i][j] = mem.get(i, j);
  }
}

MemoryMatrix::~MemoryMatrix() {
  clear();
}

unsigned int MemoryMatrix::rows() const {
  return _rows;
}

unsigned int MemoryMatrix::cols() const {
  return _cols;
}

bool MemoryMatrix::in(unsigned int r, unsigned int c) const {
  return r < _rows && c < _cols;
}

void MemoryMatrix::set(unsigned int r, unsigned int c, unsigned char value) {
  if (in(r, c))
    _data[r][c] = value;
}

unsigned char MemoryMatrix::get(unsigned int r, unsigned int c) const {
  char value = 0;
  if (in(r, c))
    value = _data[r][c];
  return value;
}

bool MemoryMatrix::resize(unsigned int r, unsigned int c) {
  clear();
  if (r > 0 && c > 0) {
    _rows = r;
    _cols = c;
    _data = new unsigned char*[_rows];
    for (unsigned int i = 0; i < _rows; i++)
      _data[i] = new unsigned char[_cols];
    return true;
  }
  return false;
}

void MemoryMatrix::clear() {
  if (rows() > 0 && cols() > 0) {
    for (unsigned int i = 0; i < rows(); i++)
      delete [] _data[i];
    delete [] _data;
    _rows = 0;
    _cols = 0;
  }
}

bool MemoryMatrix::save(const char* filename) const {
  ofstream outbin(filename, ios::binary);
  if (outbin) {
    unsigned int r = rows();
    unsigned int c = cols();
    outbin.write((char*) &r, sizeof(unsigned int));
    outbin.write((char*) &c, sizeof(unsigned int));
    for (unsigned int y = 0; y < rows(); y++)
      for (unsigned int x = 0; x < cols(); x++)
        outbin.put(get(y, x));
    outbin.close();
    return true;
  }
  return false;
}

bool MemoryMatrix::load(const char* filename) {
  ifstream memfile(filename, ios::out|ios::binary);
  if (memfile.is_open()) {
    memfile.seekg(0, ios::end);
    size_t size = memfile.tellg();
    if (size >= 8) {

      char* memblock = new char[size];
      memfile.seekg(0, ios::beg);
      memfile.read(memblock, size);

      unsigned char r[4] = {memblock[0], memblock[1], memblock[2], memblock[3]};
      unsigned char c[4] = {memblock[4], memblock[5], memblock[6], memblock[7]};

      unsigned int rows = *(unsigned int*) &r;
      unsigned int cols = *(unsigned int*) &c;

      resize(rows, cols);

      unsigned int k = 8;
      for (unsigned int i = 0; i < rows; i += 1)
        for (unsigned int j = 0; j < cols; j += 1)
          _data[i][j] = memblock[k++];

      delete [] memblock;
      memfile.close();
      return true;
    }
  }
  return false;
}

bool MemoryMatrix::loadAsBin(const char* filename) {
  ifstream memfile(filename, ios::out|ios::binary);
  if (memfile.is_open()) {
    memfile.seekg(0, ios::end);
    size_t size = memfile.tellg();
    if (size >= 8) {

      char* memblock = new char[size];
      memfile.seekg(0, ios::beg);
      memfile.read(memblock, size);

      unsigned char r[4] = {memblock[0], memblock[1], memblock[2], memblock[3]};
      unsigned char c[4] = {memblock[4], memblock[5], memblock[6], memblock[7]};

      unsigned int rows = *(unsigned int*) &r;
      unsigned int cols = *(unsigned int*) &c;

      resize(rows, cols);

      unsigned int k = 8;
      for (unsigned int i = 0; i < rows; i += 1) {
        for (unsigned int j = 0; j < cols; j += 1) {
          _data[i][j] = memblock[k] == 0 ? 0 : 1;
          k++;
        }
      }

      delete [] memblock;
      memfile.close();
      return true;
    }
  }
  return false;
}

MemoryMatrix MemoryMatrix::transpose() const {
  MemoryMatrix t;
  if (rows() > 0 && cols() > 0) {
    t.resize(cols(), rows());
    for (unsigned int i = 0; i < t.rows(); i++)
      for (unsigned int j = 0; j < t.cols(); j++)
        t.set(i, j, _data[j][i]);
  }
  return t;
}

MemoryMatrix& MemoryMatrix::operator = (const MemoryMatrix& other) {
  if (this != &other) {
    if (resize(other.rows(), other.cols())) {
      for (unsigned int i = 0; i < other.rows(); i++)
        for (unsigned int j = 0; j < other.cols(); j++)
          _data[i][j] = other.get(i, j);
    }
  }
  return *this;
}

ostream& operator << (ostream& out, const MemoryMatrix& mem) {
  for (unsigned int i = 0; i < mem.rows(); i++) {
    for (unsigned int j = 0; j < mem.cols(); j++) {
      out << (int) mem.get(i, j);
      if (j != mem.cols() - 1)
        out << '\t';
    }
    out << '\n';
  }
  return out;
}
