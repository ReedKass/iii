/*
 *     filename: unblackedges.c
 *     by Reed Kass-Mullet and Jerry Wang, 10/1/20
 *     HW2 - iii
 *
 *     Contains the functions and main for unblackedges, a function
 *     that will remove all black edge pixels from given pgm file
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <bit2.h>
#include <stack.h>
#include <set.h>
#include <pnmrdr.h>
#include <assert.h>

struct coord {
    int x;
    int y;
};

void get_file(FILE **file, int argc, char *argv[]);
struct coord *make_coord(int x, int y);
void pbmwrite(Bit2_T array);
void analyze_Bit2(Bit2_T visited, Stack_T perimeter, Bit2_T array);
void unblack(Bit2_T visited, Stack_T perimeter, Bit2_T array);
void check_neighbor(Bit2_T array, Bit2_T visited, Stack_T perimeter, 
                    int x, int y);
void update_array(Bit2_T array, Bit2_T visited);
void end_unblackedges(Bit2_T visited, Stack_T perimeter, Bit2_T array);
Bit2_T initialize_array(FILE *file);
Bit2_T initialize_blank(Bit2_T array);

/*
 * Main
 */
int main(int argc, char *argv[]) 
{
    FILE *file;
    get_file(&file, argc, argv);
    Bit2_T array = initialize_array(file);
    Stack_T perimeter = Stack_new();
    Bit2_T visited = initialize_blank(array);
    unblack(visited, perimeter, array);
    update_array(array, visited);
    pbmwrite(array);
    end_unblackedges(array, perimeter, visited);
}

/* unblack
 * Gets: 2D bitmap to track visited locations, Stack to track the
 *       perimeter of our search, 2D bitmap of the pbm file
 * Returns: Nothing
 * Does: adds the entire perimeter of the pbm file to the
 *       perimeter Stack, calls our BFS search on with the
 *       initialized perimeter
 */
void unblack(Bit2_T visited, Stack_T perimeter, Bit2_T array)
{
    for (int j = 0; j < Bit2_height(array); j++) {
        struct coord *loc = make_coord(0, j);
        struct coord *loc2 = make_coord(Bit2_width(array) - 1, j);
        Stack_push(perimeter, loc);
        Stack_push(perimeter, loc2);
    }
    for (int j = 0; j < Bit2_width(array); j++) {
        struct coord *loc = make_coord(j, 0);
        struct coord *loc2 = make_coord(j, Bit2_height(array) - 1);
        Stack_push(perimeter, loc);
        Stack_push(perimeter, loc2);
    }
    analyze_Bit2(visited, perimeter, array);
}

/* analyze_Bit2
 * Gets: 2D bitmap to track visited locations, Stack to track the
 *       perimeter of our search, 2D bitmap of the pbm file
 * Returns: Nothing
 * Does: Uses a breadth first search to scour the pbm file for all
 *       black pixels which are connected to the perimeter of the
 *       image
 */
void analyze_Bit2(Bit2_T visited, Stack_T perimeter, Bit2_T array)
{
    while (!Stack_empty(perimeter)) {
        struct coord *current = Stack_pop(perimeter);
        if (Bit2_get(array, current->x, current->y) == 1) {
            if (Bit2_get(visited, current->x, current->y) == 0) {
                Bit2_put(visited, current->x, current->y, 1);
            }
            if (current->x != 0) {
                check_neighbor(array, visited, perimeter, current->x - 1, 
                               current->y);
            }
            if (current->y != 0) {
                check_neighbor(array, visited, perimeter, current->x, 
                               current->y - 1);
            }
            if (current->y != Bit2_height(array) - 1) {
                check_neighbor(array, visited, perimeter, current->x, 
                               current->y + 1);
            }
            if (current->x != Bit2_width(array) - 1) {
                check_neighbor(array, visited, perimeter, current->x + 1, 
                               current->y);
            }
        }
        free(current);
        current = NULL;
    }
}

/* check_neighbor
 * Gets: 2D bitmap to track visited locations "visited", 
 *       Stack to track the perimeter of our search, "perimeter"
 *       2D bitmap of the pbm file "array"
 *       two int values which represent the neighbor coordinate
 *       which is being checked
 * Returns: Nothing
 * Does: checks if the given pixel is black and not previously
 *       visited. If it is, it is added to the perimeter to be
 *       inspected in a primary capacity later
 */
void check_neighbor(Bit2_T array, Bit2_T visited, Stack_T perimeter,
                    int x, int y)
{
    if (Bit2_get(array, x, y) == 1) {
        if (Bit2_get(visited, x, y) == 0) {
            struct coord *add = make_coord(x, y);
            Stack_push(perimeter, add);
        } 
    }
}

/* update_array
 * Gets:  2D bitmap of the pbm file: "array"
 *        2D bitmap of identifed pixels which are black
 *        and connected to the perimeter: "visited"
 * Returns: Nothing
 * Does: changes all identified pixels to be white
 */
void update_array(Bit2_T array, Bit2_T visited)
{
    for (int i = 0; i < Bit2_height(array); i++) {
        for (int j = 0; j < Bit2_width(array); j++) {
            if (Bit2_get(visited, j, i) == 1) {
                Bit2_put(array, j, i, 0);            
            }
        }
    }
}

/* make_coord
 * Gets:  two int values which represent a coordinate
 *        of a point in the pbm
 * Returns: a new struct coord * created with from the
 *          parameter values
 * Does: Creates and allocated heap memory for
 *       a coord struct with the given x and y values
 */
struct coord *make_coord(int x, int y)
{
    struct coord *loc = (struct coord *) malloc(sizeof(*loc));
    assert(loc != NULL);
    loc->x = x;
    loc->y = y;
    return loc;
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
        fprintf(stderr, "File Error\n");
        exit(EXIT_FAILURE);
    }
}

/* Initialize_array
 * Gets:  FILE *file which is a file pointer which
 *        is open and points to the (in theory) pbm
 *        file to be read in.
 * Returns: Nothing
 * Does: Ensures that the file points to a correct
 *       pbm file. Initializes a Bit2_T to represent
 *       the values in the pbm file
 */
Bit2_T initialize_array(FILE *file)
{
    Pnmrdr_T rdr = Pnmrdr_new(file);
    Pnmrdr_mapdata data = Pnmrdr_data(rdr);
    assert(rdr != NULL);
    if (data.type != Pnmrdr_bit) {
        Pnmrdr_free(&rdr);
        fclose(file);
        fprintf(stderr, "File Type Error\n");
        exit(EXIT_FAILURE);
    }
    int width = data.width;
    int height = data.height;

    assert((width != 0) && (height != 0));

    Bit2_T array = Bit2_new(width, height);
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Bit2_put(array, j, i, Pnmrdr_get(rdr));
        }
    }
    Pnmrdr_free(&rdr);
    fclose(file);
    return array;
}

/* initialize_blank
 * Gets:  2D bitmap of the pbm file, "array"
 * Returns: a Bit2_T which as all 0 of the same
 *          size of the passed in array
 * Does: initialized a blank, all 0, bitmap which
 *       is the same size as the array bit2_T
 */
Bit2_T initialize_blank(Bit2_T array)
{
    Bit2_T visited = Bit2_new(Bit2_width(array), Bit2_height(array));
    for (int i = 0; i < Bit2_width(array); i++) {
        for (int j = 0; j < Bit2_height(array); j++) {
            Bit2_put(visited, i, j, 0);
        }
    }
    return visited;
}

/* pbmwhite
 * Gets:  2D bitmap of the pbm file, "array"
 * Returns: nothing
 * Does: prints out the information located in
 *       the Bit2_T array in the format of a
 *       pbm file.
 */
void pbmwrite(Bit2_T array)
{
    fprintf(stdout, "P1\n");
    fprintf(stdout, "%d %d\n", Bit2_width(array), Bit2_height(array));
    
    for(int i = 0; i < Bit2_height(array); i++) {
        for(int j = 0; j < Bit2_width(array); j++) {
            fprintf(stdout, "%d", Bit2_get(array, j, i));
        }
        fprintf(stdout, "\n");
    }
}

/* end_unblackedges
 * Gets:  2D bitmap of the pbm file, "array"
 *        Stack_T which is now empty, "perimeter"
 *        2D bitmap of the visited locations
 * Returns: nothing
 * Does: frees all of the heap storage from the
 *       passed in structures
 */
void end_unblackedges(Bit2_T visited, Stack_T perimeter, Bit2_T array)
{
    Bit2_free(&visited);
    Bit2_free(&array);
    Stack_free(&perimeter);
}
