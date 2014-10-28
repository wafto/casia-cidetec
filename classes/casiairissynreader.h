#ifndef _CASIAIRISSYNREADER_H
#define _CASIAIRISSYNREADER_H

#include <string>
#include <fstream>
#include <iostream>

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "datagrid.h"
#include "circledetector.h"
#include "associativememory.h"
#include "jpeglib.h"

using namespace std;

class CasiaIrisSynReader {
public:
  CasiaIrisSynReader();
  CasiaIrisSynReader(const string&);
  ~CasiaIrisSynReader();

  void setDirectoryRoot(const string&);
  string getDirectoryRoot() const;

  DataGrid readImage(const unsigned int, const unsigned int, bool*);
  DataGrid evaluate(const DataGrid&);
  bool extractFeatures(const unsigned int, const unsigned int);
  string createFilePath(const unsigned int, const unsigned int, const string&) const;

  FundamentalSet fetchFundamentalSet(const unsigned int, const unsigned int, const int);

  FundamentalSet makeFundamentalSetUnique(FundamentalSet&);

  void cleanFundamentalSetFetched(FundamentalSet&, bool);

  MemoryMatrix fetchMemoryMatrix(const unsigned int, const unsigned int, bool); 

private:
  string _dirroot;
};

#endif
