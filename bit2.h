/*
 *     filename: bit2.h
 *     by Reed Kass-Mullet and Jerry Wang, 10/1/20
 *     HW2 - iii
 *
 *    contains the header for our bit2 implementation
 */ 

#ifndef Bit2_INCLUDED
#define Bit2_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <uarray.h>

#define T Bit2_T
typedef struct T *T;
 
T Bit2_new(int width, int height);
int Bit2_width(T array);
int Bit2_height(T array);
void Bit2_map_col_major(T array, void (*func)(int i, int j, T a, int p1,
                        void *p2), void *flex);
void Bit2_map_row_major(T array, void (*func)(int i, int j, T a, int p1,
                        void *p2), void *flex);
void Bit2_free(T* array);
int Bit2_put(T array, int col, int row, int value);
int Bit2_get(T array, int col, int row);

#undef T
#endif
