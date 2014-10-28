#ifndef _ASSOCIATIVEMEMORY_H
#define _ASSOCIATIVEMEMORY_H

#include <sstream>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
using namespace std;

#define MAX_HAMMING 1000000000

class MemoryMatrix {
public:
  MemoryMatrix();
  MemoryMatrix(unsigned int, unsigned int);
  MemoryMatrix(unsigned int, unsigned int, unsigned char);
  MemoryMatrix(unsigned int);
  MemoryMatrix(const MemoryMatrix&);
  ~MemoryMatrix();

  unsigned int rows() const;
  unsigned int cols() const;

  bool in(unsigned int, unsigned int) const;
  void set(unsigned int, unsigned int, unsigned char);
  unsigned char get(unsigned int, unsigned int) const;

  MemoryMatrix transpose() const;

  MemoryMatrix& operator = (const MemoryMatrix&);
  friend ostream& operator << (ostream&, const MemoryMatrix&);

  bool resize(unsigned int, unsigned int);
  void clear();

  bool save(const char*) const;
  bool load(const char*);
  bool loadAsBin(const char*);

private:
  unsigned char** _data;
  unsigned int _rows;
  unsigned int _cols;
};

typedef struct {
  MemoryMatrix* x;
  MemoryMatrix* y;
  unsigned int classname;
} OrderedCouple;

typedef list<OrderedCouple*> FundamentalSet;

typedef list<unsigned int> RecoveredClasses;

class AssociativeMemory {
public:
  AssociativeMemory(FundamentalSet&, unsigned int);
  ~AssociativeMemory();

  MemoryMatrix alpha(const MemoryMatrix&, const MemoryMatrix&) const;
  MemoryMatrix beta(const MemoryMatrix&, const MemoryMatrix&) const;
  MemoryMatrix betaMin(const MemoryMatrix&) const;
  MemoryMatrix betaMax(const MemoryMatrix&) const;
  MemoryMatrix max(bool*) const;
  MemoryMatrix min(bool*) const;

  unsigned int hammingDistance(const MemoryMatrix&, const MemoryMatrix&) const;
  RecoveredClasses recovery(const MemoryMatrix&) const;

protected:
  FundamentalSet* CF;
  unsigned int classes;
};

#endif