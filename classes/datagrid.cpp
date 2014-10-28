#include "datagrid.h"

DataGrid::DataGrid() {
	_width = _height = 0;
	_data = 0;
}

DataGrid::DataGrid(const DataGrid& other) {
	_width = _height = 0;
	_data = 0;
	if (createMap(other.width(), other.height())) {
		for (unsigned int x = 0; x < width(); x++)
			for (unsigned int y = 0; y < height(); y++)
				_data[x][y] = other.at(x, y);
	}
}

DataGrid::DataGrid(unsigned const int w, unsigned const int h) {
	_width = _height = 0;
	_data = 0;
	createMap(w, h);
}

DataGrid::DataGrid(unsigned const int w, unsigned const int h, const int value) {
	_width = _height = 0;
	_data = 0;
	if (createMap(w, h)) {
		for (unsigned int x = 0; x < width(); x++)
			for (unsigned int y = 0; y < height(); y++)
				_data[x][y] = value;
	}
}

DataGrid::~DataGrid() {
	deleteMap();
}

unsigned const int DataGrid::width() const {
	return _width;
}

unsigned const int DataGrid::height() const {
	return _height;
}

bool DataGrid::in(const int x, const int y) const {
	return x >= 0 && y >= 0 && x < width() && y < height();
}

const int DataGrid::at(const int x, const int y) const {
	return in(x, y) ? _data[x][y] : 0;
}

void DataGrid::set(const int x, const int y, const int value) {
	if (in(x, y))	_data[x][y] = value;
}

bool DataGrid::createMap(unsigned const int w, unsigned const int h) {
	if (w != 0 && h != 0) {
		deleteMap();
		_width = w;
		_height = h;
		_data = new int*[width()];
		if (_data != 0) {
			for (unsigned int x = 0; x < width(); x++)
				_data[x] = new int[height()];
		}
		return true;
	}
	return false;
}

void DataGrid::deleteMap() {
	if (_data != 0) {
		for (unsigned int x = 0; x < width(); x++)
      delete [] _data[x];
    delete [] _data; 
    _width = 0;
    _height = 0;
    _data = 0;
	}
}


DataGrid& DataGrid::operator=(const DataGrid& other) {
	if (this != &other) {
		if (createMap(other.width(), other.height())) {
			for (unsigned int x = 0; x < width(); x++)
				for (unsigned int y = 0; y < height(); y++)
					_data[x][y] = other.at(x, y);
		}	
	}
	return *this;
}

DataGrid DataGrid::copy(const int x, const int y, unsigned const int w, unsigned const int h) {
	DataGrid res(w, h);
	for (int xx = 0; xx < res.width(); xx++)
		for (int yy = 0; yy < res.height(); yy++)
			res.set(xx, yy, at(xx + x, yy + y));
	return res;
}

MemoryMatrix DataGrid::toVector() const {
	MemoryMatrix mm(width() * height());
	unsigned int i = 0;
	for (int x = 0; x < width(); x++)
		for (int y = 0; y < height(); y++)
			mm.set(i++, 0, at(x, y));
	return mm;
}




