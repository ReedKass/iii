/*
 *     filename: bit2.c
 *     by Reed Kass-Mullet and Jerry Wang, 10/01/20
 *     HW2 - iii
 *
 *     Contains the functions and structs for Bit2
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <bit2.h>
#include <assert.h>
#include <mem.h>
#include <bit.h>

#define T Bit2_T

/*  Bit2_T Struct
 *  Contains: int height and width of 2d array
 *            void function pointer to call in map functions
 *            UArray_T filled with Bit_T's
 */
struct T {
    int width;
    int height;
    void (*func) (int i, int j, T a, void *p1, void *p2);
    UArray_T columns;  
};

/* Function: Bit2_new
 * Parameters: int width of desired 2d array, int height of desired 2d array
 *             - Will throw error if height or width is negative
 * Returns: a new Bit2_T object initialized with given width and height
 * Does: Mallocs, initializes, then returns a pointer to Bit2_T object 
 *       with given width and height
 */
T Bit2_new(int width, int height) 
{
    assert(height > 0);
    assert(width > 0);

    T array = malloc(sizeof(*array));
    assert(array != NULL);

    UArray_T columns = UArray_new(width, sizeof(T));
    assert(columns != NULL);

    for (int i = 0; i < width; i++) {
        Bit_T row = Bit_new(height);
        assert(row != NULL);
        *(Bit_T *) UArray_at(columns, i) = row;
    }

    array->width = width;
    array->height = height;
    array->columns = columns;

    return array;
}

/* Function: Bit2_width
 * Parameters: Pointer to Bit2_T that you want the width of
 *             - Will throw error if Bit2_T pointer is NULL
 * Returns: a int representing the width of given Bit2_T
 * Does: Returns the width of the Bit2_T that was passed in
 */
int Bit2_width(T array) 
{
    assert(array != NULL);
    return array->width;
}

/* Function: Bit2_height
 * Parameters: Pointer to Bit2_T that you want the height of
 *             - Will throw error if Bit2_T pointer is NULL
 * Returns: a int representing the height of given Bit2_T
 * Does: Returns the height of the Bit2_T that was passed in
 */
int Bit2_height(T array) 
{
    assert(array != NULL);
    return array->height;
}

/* Function: Bit2_get
 * Parameters: Pointer to Bit2_T with data you want to access, int of col and
 *             row indices of which you want to access in the 2d array
 *             - Will throw error if Bit2_T pointer is NULL
 *             - Will throw error if col or row are negative or not in bounds
 * Returns: a int (0 or 1) found at indices [col][row] of the 2d array
 *          in the given Bit2_T
 * Does: Returns value found at index [col][row] of the 2d array in the 
 *       given Bit2_T
 */
int Bit2_get(T array, int col, int row)
{
    assert(array != NULL);
    assert((col >= 0) && (col < array->width));
    assert((row >= 0) && (row < array->height));

    return Bit_get(*(Bit_T *) UArray_at(array->columns, col), row);
}

/* Function: Bit2_put
 * Parameters: Pointer to Bit2_T with data you want to access, int of col and
 *             row indices of which you want to edit in the 2d array,
 *             int value (0 or 1) that you want to put into the 2d array
 *             - Will throw error if Bit2_T pointer is NULL
 *             - Will throw error if col or row are negative
 *             - Will throw error if value is not 0 or 1
 * Returns: a int (0 or 1) found at indices [col][row] of the 2d array
 *          in the given Bit2_T, before it was replaced
 * Does: Puts the given value at given [col][row] index of the given
 *       Bit2_T's 2d array, returns the value that was replaced
 */
int Bit2_put(T array, int col, int row, int value) 
{
    assert(array != NULL);
    assert((col >= 0) && (col < array->width));
    assert((row >= 0) && (row < array->height));
    assert((value == 0) || (value == 1));

    return Bit_put(*(Bit_T *) UArray_at(array->columns, col), row, value);
}

/* Function: Bit2_map_col_major
 * Parameters: Pointer to Bit2_T with data you want to access,
 *             void function pointer that you want to call on every point
 *             in the 2d array,
 *             void pointer of a flex variable 
 *             - will throw error if Bit2_T pointer is NULL
 *             - will throw error if function pointer is NULL
 * Returns: N/A
 * Does: Calls given function on every point in the 2d array of the given
 *       Bit2_T, first iterating through all the points in a column
 *       before moving to the points in the next column
 */
void Bit2_map_col_major(T array, void (*func) (int i, int j, T a, int p1, 
                        void *p2), void *flex) 
{
    assert(array != NULL);
    assert(func != NULL);

    for (int i = 0; i < array->width; i++) {
        for (int j = 0; j < array->height; j++) {
            func(i, j, array, 
                 Bit_get(*(Bit_T *) UArray_at(array->columns, i), j), flex);
        }
    }
}

/* Function: Bit2_map_row_major
 * Parameters: Pointer to Bit2_T with data you want to access,
 *             void function pointer that you want to call on every point
 *             in the 2d array,
 *             void pointer of a flex variable 
 *             - will throw error if Bit2_T pointer is NULL
 *             - will throw error if function pointer is NULL
 * Returns: N/A
 * Does: Calls given function on every point in the 2d array of the given
 *       Bit2_T, first iterating through all the points in a row
 *       before moving to the points in the next row
 */
void Bit2_map_row_major(T array, void (*func) (int i, int j, T a, int p1, 
                        void *p2), void *flex) 
{
    assert(array != NULL);
    assert(func != NULL);

    for (int i = 0; i < array->height; i++) {
        for (int j = 0; j < array->width; j++) {
            func(j, i, array, 
                 Bit_get(*(Bit_T *) UArray_at(array->columns, j), i), flex);
        }
    }
}

/* Function: Bit2_free
 * Parameters: Double Pointer to Bit2_T of which you want to free
 *             all heap memory
 *             - will throw error if double pointer is NULL
 * Returns: N/A
 * Does: Frees all the Bit_T's found in the UArray of the given Bit2_T,
 *       then frees the UArray, and finally the Bit2_T itself
 */
void Bit2_free(T* array) 
{
    assert(array != NULL);
    for (int i = 0; i < (*array)->width; i++) {
        Bit_free((Bit_T *) (UArray_at((*array)->columns, i)));
    }
    UArray_free(&((*array)->columns));
    free(*array);
}
