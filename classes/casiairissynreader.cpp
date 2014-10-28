#include "casiairissynreader.h"

CasiaIrisSynReader::CasiaIrisSynReader() {
  setDirectoryRoot("");
}

CasiaIrisSynReader::CasiaIrisSynReader(const string& dir) {
  setDirectoryRoot(dir);
}

CasiaIrisSynReader::~CasiaIrisSynReader() {
  setDirectoryRoot("");
}

void CasiaIrisSynReader::setDirectoryRoot(const string& dir) {
  _dirroot = dir;
}

string CasiaIrisSynReader::getDirectoryRoot() const {
  return _dirroot;
}

DataGrid CasiaIrisSynReader::readImage(const unsigned int cl, const unsigned int sample, bool* flag) {
  DataGrid grid;
  *flag = false;
  if (cl < 1000 && sample < 10) {
    string pathfile = createFilePath(cl, sample, "jpg");
    FILE *infile = fopen(pathfile.c_str(), "rb");
    
    if (infile) {
      struct jpeg_decompress_struct cinfo;
      struct jpeg_error_mgr jerr;
      JSAMPROW row_pointer[1];
      unsigned char *raw_image = 0;
      unsigned long location = 0;
        
      cinfo.err = jpeg_std_error(&jerr);
      jpeg_create_decompress(&cinfo);
      jpeg_stdio_src(&cinfo, infile);
      jpeg_read_header(&cinfo, true);

      if (cinfo.jpeg_color_space == JCS_GRAYSCALE && cinfo.num_components == 1) {
        grid = DataGrid(cinfo.image_width, cinfo.image_height);
        jpeg_start_decompress(&cinfo);

        row_pointer[0] = (unsigned char*) malloc(cinfo.output_width * cinfo.num_components);
        raw_image = (unsigned char*) malloc(cinfo.output_width * cinfo.output_height * cinfo.num_components);        

        while( cinfo.output_scanline < cinfo.image_height) {
          jpeg_read_scanlines(&cinfo, row_pointer, 1);
          for(int i = 0; i < cinfo.image_width * cinfo.num_components; i++)
            raw_image[location++] = row_pointer[0][i];
        }

        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        free(row_pointer[0]);
        fclose(infile);
      }
      if (raw_image) {
        if (location > 0) {
          for (unsigned int i = 0; i < cinfo.image_height; i++)
            for (unsigned int j = 0; j < cinfo.image_width; j++)
              grid.set(j, i, raw_image[i * cinfo.image_width + j]);
        }
        free(raw_image);
        *flag = true;
      }      
    }
  }
  return grid;
}

DataGrid CasiaIrisSynReader::evaluate(const DataGrid& grid) {
  CircleDetector detector;

  DataGrid canny = detector.canny(grid, 60, 200);
  DataGrid equalized = detector.equalize(grid);
  DataGrid cc = detector.canny(equalized, 50, 150);

  Circle pupil = detector.detectPupil(grid, canny, 35, 60);
  Circle iris = detector.detectIris(cc, pupil);

  return detector.equalize(detector.normalize(grid, pupil, iris));
}

bool CasiaIrisSynReader::extractFeatures(const unsigned int cl, const unsigned int sample) {
  if (cl < 1000 && sample < 10) {
    string filename = createFilePath(cl, sample, "mem");
    bool status;
    DataGrid grid = readImage(cl, sample, &status);
    CircleDetector detector;

    if (status) {
      DataGrid normalized = evaluate(grid);
      DataGrid binarized = detector.threshold(normalized, 110);

      MemoryMatrix mmIzq = binarized.copy(40, 5, 60, 30).toVector();
      MemoryMatrix mmDer = binarized.copy(260, 5, 60, 30).toVector();

      ofstream outbin(filename.c_str(), ios::binary);
      if (outbin) {
        unsigned int r = mmIzq.rows() + mmDer.rows();
        unsigned int c = 1;
        outbin.write((char*) &r, sizeof(unsigned int));
        outbin.write((char*) &c, sizeof(unsigned int));

        for (unsigned int y = 0; y < mmIzq.rows(); y++)
          for (unsigned int x = 0; x < mmIzq.cols(); x++)
            outbin.put(mmIzq.get(y, x));

        for (unsigned int y = 0; y < mmDer.rows(); y++)
          for (unsigned int x = 0; x < mmDer.cols(); x++)
            outbin.put(mmDer.get(y, x));

        outbin.close();
        return true;
      }
    }

  }
  return false;
}

FundamentalSet CasiaIrisSynReader::fetchFundamentalSet(const unsigned int from, const unsigned int to, const int skip) {
  FundamentalSet result;
  if (from <= to && from < 1000 && to < 1000) {
    OrderedCouple* ocaux;
    for (unsigned int cl = from; cl <= to; cl += 1) {
      for (unsigned int sample = 0; sample <= 9; sample += 1) {
        if (sample == skip)
          continue;
        MemoryMatrix* mmaux = new MemoryMatrix();
        if (mmaux) {
          mmaux->loadAsBin(createFilePath(cl, sample, "mem").c_str());
          if (mmaux->rows() > 0 && mmaux->cols() == 1) {
            ocaux = new OrderedCouple;
            if (ocaux) {
              ocaux->x = mmaux;
              ocaux->y = mmaux;
              ocaux->classname = cl;
              result.push_back(ocaux);
            }
          }
        }
      }     
    }
  }
  return result;
}

FundamentalSet CasiaIrisSynReader::makeFundamentalSetUnique(FundamentalSet& fs) {
  FundamentalSet result;
  FundamentalSet::const_iterator iterator, second;
  OrderedCouple *oc, *aux, *ocaux;


  for (iterator = fs.begin(); iterator != fs.end(); ++iterator) {
    oc = *iterator;

    bool founded = false;
    for (second = result.begin(); second != result.end(); ++second) {
      aux = *second;
      if (aux->classname == oc->classname) {
        founded = true;
        break;
      }
    }

    if (!founded) {
      ocaux = new OrderedCouple;
      if (ocaux) {
        ocaux->x = new MemoryMatrix(*(oc->x));
        ocaux->y = ocaux->x;
        ocaux->classname = oc->classname;
        result.push_back(ocaux);
      }
    } else {
      if (oc->x->rows() == aux->x->rows() && oc->x->cols() == aux->x->cols()) {
        for (unsigned int i = 0; i < aux->x->rows(); i++) {
          for (unsigned int j = 0; j < aux->x->cols(); j++) {
            unsigned char ocval = oc->x->get(i, j);
            unsigned char auxval = aux->x->get(i, j);
            unsigned char resval = (ocval == 0 || auxval == 0) ? 0 : 255;
            aux->x->set(i, j, resval);
          }
        }
      }
    }
  }
  return result;
}

MemoryMatrix CasiaIrisSynReader::fetchMemoryMatrix(const unsigned int cl, const unsigned int sample, bool bin) {
  MemoryMatrix result;
  if (cl < 1000 && sample < 10)
    if (bin)
      result.loadAsBin(createFilePath(cl, sample, "mem").c_str());
    else
      result.load(createFilePath(cl, sample, "mem").c_str());
  return result;
}

void CasiaIrisSynReader::cleanFundamentalSetFetched(FundamentalSet& fs, bool autoassociative) {
  FundamentalSet::const_iterator iterator;
  OrderedCouple* oc;
  for (iterator = fs.begin(); iterator != fs.end(); ++iterator) {
    oc = *iterator;
    delete oc->x;
    if (!autoassociative)
      delete oc->y;
    delete oc;
  }
  fs.clear();
}

string CasiaIrisSynReader::createFilePath(const unsigned int cl, const unsigned int sample, const string& extension) const {
  string strcl = "000";
  string strsample = "00";
  strsample[strsample.size() - 1] = (char)(sample % 10 + 48);
  strcl[strcl.size() - 1] = (char)(cl % 10 + 48);
  strcl[strcl.size() - 2] = (char)(cl / 10 % 10 + 48);
  strcl[strcl.size() - 3] = (char)(cl / 100 % 10 + 48);
  string pathfile = getDirectoryRoot() + "/" + strcl + "/S6" + strcl + "S" + strsample + "." + extension;
  return pathfile;
}


