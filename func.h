/*
Mô tả: File header chứa các nguyên mẫu hàm (function prototypes)
*/

#ifndef FUNC_H
#define FUNC_H

#include "datatypes.h"

//2
void initBuffer(Buffer *b, int capacity);
void freeBuffer(Buffer *b);
void initStats(Stats *stats);
int loadFile(const char* filename, SensorSet sensors[]);

//3 & 6
void writeLog(const char* message);
void printReport(Stats stats);
float generateRandomValue(const char* type);
void runSimulation(SensorSet sensors[], int num_sensors, Stats *stats, Buffer *b, int duration_seconds);

//4
float applyMovingAverage(SensorSet *s, float newVal);
void handleThreshold(SensorSet *s, float filteredVal, Stats *stats);

//5
void pushBuffer(Buffer *b, SensorMessage msg, Stats *stats);
int popBuffer(Buffer *b, SensorMessage *out_msg);
#endif