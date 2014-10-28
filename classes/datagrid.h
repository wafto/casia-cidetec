#ifndef _DATAGRID_H
#define _DATAGRID_H

#include "associativememory.h"

class DataGrid {
public:
	DataGrid();
	DataGrid(const DataGrid&);
	DataGrid(unsigned const int, unsigned const int);
	DataGrid(unsigned const int, unsigned const int, const int);
	~DataGrid();

	unsigned const int width() const;
	unsigned const int height() const;

	bool in(const int, const int) const;
	const int at(const int, const int) const;
	void set(const int, const int, const int);

	DataGrid copy(const int, const int, unsigned const int, unsigned const int);
	DataGrid& operator=(const DataGrid&);

	MemoryMatrix toVector() const;

protected:
	bool createMap(unsigned const int, unsigned const int);
	void deleteMap();

private:
	unsigned int _width;
	unsigned int _height;
	int** _data;
};

#endif