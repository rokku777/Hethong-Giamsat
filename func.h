#ifndef FUNC_H
#define FUNC_H

#include "datatypes.h"

void initBuffer(Buffer *b, int capacity);
void freeBuffer(Buffer *b);
void initStats(Stats *stats);
int loadFile(const char* filename, SensorSet sensors[]);

#endif