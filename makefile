app:
	g++ test.cpp classes/casiairissynreader.cpp classes/circledetector.cpp classes/datagrid.cpp classes/associativememory.cpp -ljpeg -o ~/Desktop/app

knn:
	g++ knn.cpp classes/casiairissynreader.cpp classes/circledetector.cpp classes/datagrid.cpp classes/associativememory.cpp -ljpeg -o ~/Desktop/app
	
segmentacion:
	g++ segmentacion.cpp classes/casiairissynreader.cpp classes/circledetector.cpp classes/datagrid.cpp classes/associativememory.cpp -ljpeg -o ~/Desktop/app

extraccion:
	g++ extraccion.cpp classes/casiairissynreader.cpp classes/circledetector.cpp classes/datagrid.cpp classes/associativememory.cpp -ljpeg -o ~/Desktop/app

identificacion:
	g++ identificacion.cpp classes/casiairissynreader.cpp classes/circledetector.cpp classes/datagrid.cpp classes/associativememory.cpp -ljpeg -o ~/Desktop/app
	
