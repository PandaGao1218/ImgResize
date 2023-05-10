#pragma once


int getImageAreaScore(unsigned char* imgBuf, int width, int height);

void MY_IsFinger(const unsigned char* image, int width, int height, int* gScore, int* aScore, int* wScore);

int My_IsFinger(const unsigned char* image, int width, int height);

void LAPI_IsFinger(unsigned char* image, int width, int height, int* gScore, int* aScore, int* wScore);
