/*
 *          uarray2b.c
 *      Authors: Nick Murphy and Reed Kass-Mullet
 *      Assignment: Comp40 Homework 3, Locality
 *      Due: October 12
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
//#include "uarray2.h"
#include "uarray.h"
#include <assert.h>
#include "uarray2b.h"
#include "uarray2.h"
#include <math.h>

#define T UArray2b_T

const int MAX_SIZE = 64000;

extern int calc_Col(int index, T array2b);
extern int calc_Row(int index, T array2b);

struct T {
    int width;
    int height;
    int element_size;
    int block_size;
    UArray_T blocks; // UArray of UArrays (2d Version)
};

/*
 *      Takes in the dimensions for a new UArray2b and returns it
 *      Gets: Dimensions for new UArray2b
 *      Returns: A new UArray2b
 */
extern T UArray2b_new(int width, int height, int size, int blocksize)
{
    assert(height > 0);
    assert(width > 0);
    assert(size > 0);
    assert(blocksize > 0);
    //find number of blocks needed
    int num_block_width = (width + blocksize - 1) / blocksize;
    int num_block_height = (height + blocksize - 1) / blocksize;
    UArray_T blocks = UArray_new(blocksize * blocksize * 
                                num_block_width * num_block_height, size);
    T arrayb = malloc(sizeof(*arrayb));
    assert(arrayb != NULL);
    arrayb -> width = width;
    arrayb -> height = height;
    arrayb -> element_size = size;
    arrayb -> block_size = blocksize;
    arrayb -> blocks = blocks;
    return arrayb;
}


/*
 *      Initializes a new blocked 2D array with 
 *      blocksize as large as it can be, at most 64 KB
 *      Gets: Dimensions for new blocked UArray2b
 *      Returns: New blocked UArray2b 
 */
extern T UArray2b_new_64K_block(int width, int height, int size)
{
    assert(height > 0);
    assert(width > 0);
    assert(size > 0);
    
    int block_size = 0;
    int capacity = MAX_SIZE / size;
    
    /* If size > Max_size: */
    if (capacity == 0) {
        block_size = 1;
    } else {
        /* Calculate the max block size through int math*/
        block_size = (int) floor (sqrt ((double) capacity));
    }

    T array2b = UArray2b_new(width, height, size, block_size);
    return array2b;
}

/*
 *      Frees a UArray2b
 *      Gets: a UArray2b
 *      Returns: Nothing
 */
extern void UArray2b_free(T *array2b)
{
    assert(array2b != NULL);
    assert(*array2b != NULL);
    UArray_free(&((*array2b) -> blocks));
    free(*array2b);
}

/*
 *      Returns the width of a UArray2b
 *      Gets: a UArray2b
 *      Returns: The width of the inputted UArray2b
 */
extern int UArray2b_width(T array2b)
{
    assert(array2b != NULL);
    return array2b -> width;
}

/*
 *      Returns the height of a UArray2b
 *      Gets: a UArray2b
 *      Returns: The height of the inputted UArray2b
 */
extern int UArray2b_height(T array2b)
{
    assert(array2b != NULL);
    return array2b -> height;
}

/*
 *      Returns the size of a UArray2b
 *      Gets: a UArray2b
 *      Returns: The size of the inputted UArray2b
 */
extern int UArray2b_size(T array2b)
{
    assert(array2b != NULL);
    return array2b -> element_size;
}

/*
 *      Returns the block_size of a UArray2b
 *      Gets: a UArray2b
 *      Returns: The block_size of the inputted UArray2b
 */
extern int UArray2b_blocksize(T array2b)
{
    assert(array2b != NULL);
    return array2b -> block_size;
}

/*
 *      Returns the element at the inputted indices of a UArray2b
 *      Gets: a UArray2b
 *      Returns: The desired element
 */
extern void *UArray2b_at(T array2b, int column, int row)
{
    assert(array2b != NULL);
    assert((column >= 0) && (column < array2b -> width));
    assert((row >= 0) && (row < array2b -> height));
    /* Calculate index */
    int w = array2b -> width;
    int bs = array2b -> block_size;
    int bc = column / bs;
    int br = row / bs;

    int bn = (w + bs - 1) / bs;

    int bloc = bc + (bn * br);

    int c_block = column % bs;
    int r_block = row % bs;

    int inside_in = c_block + (r_block * bs);
    int index = inside_in + (bloc * bs * bs);
    /* Return pointer to cell at index */
    return UArray_at(array2b -> blocks, index);
}

/* visits every cell in one block before moving to another block */
extern void UArray2b_map(T array2b,
                         void apply(int col, int row, T array2b,
                                    void *elem, void *cl),
                         void *cl)
{
    assert(array2b != NULL);
    assert(apply != NULL);
    for(int i = 0; i < UArray_length((array2b -> blocks)); ++i) {
        int c = calc_Col(i, array2b);
        int r = calc_Row(i, array2b);
        if (c < array2b -> width  && r < array2b -> height) {
            apply(c, r, array2b, UArray_at(array2b -> blocks, i), cl);
        }
    }
}

/*
 *      Calculates the 2D col of the index within the UArray2b
 *      Gets: an index and a UArray2b
 *      Returns: The column index of the element
 */
extern int calc_Col(int index, T array2b)
{
    int blocksize = array2b->block_size;
    int bloc = index / (blocksize * blocksize);

    int inner_index = index - (bloc * blocksize * blocksize);

    int inner_Col = inner_index % blocksize;
    int block_number = (array2b -> width + blocksize - 1) / blocksize;
    int bloc_Col = bloc % block_number;
    int index_Col = (bloc_Col * blocksize) + inner_Col;
    return index_Col;
}

/*
 *      Calculates the 2D row of the index within the UArray2b
 *      Gets: an index and a UArray2b
 *      Returns: The row index of the element
 */
extern int calc_Row(int index, T array2b)
{
    int blocksize = array2b->block_size;
    int bloc = index / (blocksize * blocksize);

    int inner_index = index - (bloc * blocksize * blocksize);

    int inner_Row = inner_index / blocksize;
    int block_number = (array2b -> width + blocksize - 1) / blocksize;
    int bloc_Row = bloc / block_number;
    int index_Row = (bloc_Row * blocksize) + inner_Row;
    return index_Row;
}

#undef T
