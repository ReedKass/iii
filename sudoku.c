/*
 *     sudoku.c
 *     by Jerry Wang and Reed Kass-Mullet, 10/01/2020
 *     HW2 - iii
 *
 *     Contains all the functions to run sudoku, which is a function that
 *     checks whether or not a given board (in pgm format) is valid solution 
 *     or not
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <pnmrdr.h>
#include <uarray2.h>
#include <stdbool.h>
#include <assert.h>

void get_file(FILE **file, int argc, char *argv[]);
UArray2_T initialize_array(FILE *file);
void sudoku(UArray2_T array, bool *correct);
void check_sudoku(int column, int row, UArray2_T a, void *p1, void *p2);
void check_grid(int column, int row, UArray2_T a, void *p1, void *p2);
void end_sudoku(UArray2_T array, bool correct);

/*
 * Main
 */
int main(int argc, char *argv[])
{
    FILE *file;
    get_file(&file, argc, argv);
    UArray2_T array = initialize_array(file);
    fclose(file);

    bool correct = true;
    sudoku(array, &correct);

    end_sudoku(array, correct);
}


void sudoku(UArray2_T array, bool *correct) 
{
    if (UArray2_width(array) != 9 || UArray2_height(array) != 9) {
        *correct = false;
    }
    UArray2_map_col_major(array, check_sudoku, correct);    
    UArray2_map_col_major(array, check_grid, correct); 
}

/* check_sudoku)
 * Note: This function is called via the map function
 * Gets:  two int values which represent the coordinate
 *        of the location currently under inspection
 *        UArray2_T a which is the array being checked
 *        void *p1 is the number value at the current
 *        location
 *        void *p2 is pointer to a boolean value which
 *        is set to false if a condition fails
 * Returns: nothing
 * Does: Checks each element in the UArray_2 to see
 *       if it is a valid part of a sudoku solution.
 *       it assumed all values are between 1-9 as a
 *       seperate function ensures they are.
 */
void check_sudoku(int column, int row, UArray2_T a, void *p1, void *p2)
{
    if (*(bool *) p2 != false) {
        for (int i = 0; i < UArray2_width(a); i++) {
            if ((*(int *) p1 == *(int *) UArray2_at(a, i, row))) {
                if (column != i) {
                    *(bool *) p2 = false;
                }
            }
        }
        for (int i = 0; i < UArray2_height(a); i++) {
            if ((*(int *) p1 == *(int *) UArray2_at(a, column, i))) {
                if (row != i) {
                    *(bool *) p2 = false;
                }
            }
        }
        int box_row = row / 3;
        int box_col = column / 3; 

        for (int i = box_col * 3; i < box_col * 3 + 3; i++) {
            for (int j = box_row * 3; j < box_row * 3 + 3; j++) {
                if ((*(int*) p1 == *(int *) UArray2_at(a, i, j))) {
                    if (!((i == column) && (j == row))) {
                        *(bool *) p2 = false;
                    }
                }
            }
        }
    }
}

/* check_grid
 * Note: This function is called via the map function
 * Gets:  two int values which represent the coordinate
 *        of the location currently under inspection
 *        UArray2_T a which is the array being checked
 *        void *p1 is the number value at the current
 *        location
 *        void *p2 is pointer to a boolean value which
 *        is set to false if a condition fails
 * Returns: nothing
 * Does: Checks each element to ensure it is a valid
 *       sudolu value
 */
void check_grid(int column, int row, UArray2_T a, void *p1, void *p2)
{
    (void) p1;
    if (*(int *) UArray2_at(a, column, row) > 9 || 
        *(int *) UArray2_at(a, column, row) < 1) {
        *(bool *) p2 = false;
    }
}

/* get_file
 * Gets:  a file double pointer go be initialized.
 *        int argc which is the number of 
 *        parameters passed into unblackedges
 *        *argv[] a char * array which should hold
 *        the name of the file to be opened
 * Returns: Nothing
 * Does: checks if the correct number of parameters
 *       were passed in. Ensures the file exists.
 *       Exits if an error is detected. Sets the file
 *       pointer to the correct location.
 */
void get_file(FILE **file, int argc, char *argv[])
{
    assert(argc <= 2);
    if (argc == 1) {
        /* open from stdin */
        *file = stdin;
    } else if (argc == 2) {
        /* open from filename */
        *file = fopen(argv[1], "r");
        assert(file != NULL);
    }
    if (*file == NULL) {
        fprintf(stderr, "File Null\n");
        exit(EXIT_FAILURE);
    }
}

/* Initialize_array
 * Gets:  FILE *file which is a file pointer which
 *        is open and points to the (in theory) pgm
 *        file to be read in.
 * Returns: Nothing
 * Does: Ensures that the file points to a correct
 *       pgm file. Initializes a UArray2_T to represent
 *       the values in the pgm file
 */
UArray2_T initialize_array(FILE *file) 
{
    assert(file != NULL);
    Pnmrdr_T rdr = Pnmrdr_new(file);
    Pnmrdr_mapdata data = Pnmrdr_data(rdr);
    if (data.type != Pnmrdr_gray) {
        Pnmrdr_free(&rdr);
        fclose(file);
        fprintf(stderr, "File Type Error\n");
        exit(EXIT_FAILURE);
    }
    int x = data.width;
    int y = data.height;
    UArray2_T array = UArray2_new(x, y, 4);
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            *(int *) UArray2_at(array, i, j) = Pnmrdr_get(rdr);
        }
    }
    Pnmrdr_free(&rdr);
    return array;
}

/* end_sudoku
 * Gets:  UArray2_T of the pgm file, "array"
 *        boolean correct which represents
 *        if the pgm passed in was a correct
 *        sudoku 
 * Returns: nothing
 * Does: frees all of the heap storage from the
 *       passed in structures and returns with an
 *       exit code 0 if correct, exit code 1 if
 *       incorrect
 */
void end_sudoku(UArray2_T array, bool correct) 
{
    if (correct == true) {
        //printf("Correct Solution\n");
        UArray2_free(&array);
        exit(0);
    } else {
        //printf("not correct\n");
        UArray2_free(&array);
        exit(1);
    }
}
