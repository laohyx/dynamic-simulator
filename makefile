CC=g++
OPT=-Os -std=c++11
OUT=dynamicGraph

$(OUT).o:
	$(CC) -o $(OUT) main.cpp DynamicGraph.cpp $(OPT)