/*
 *      ppmtrans.c
 *      Authors: Nick Murphy and Reed Kass-Mullet
 *      Assignment: Comp40 Homework 3, Locality
 *      Due: October 12
 * 
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "assert.h"
#include "cputiming.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"

#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (0)

typedef struct info {
        A2Methods_UArray2 output;
        A2Methods_T methods;
} info;

FILE *get_file(FILE **file, int argc, char *argv[], int i);
void rotate90(int col, int row, A2Methods_UArray2 array2b, void *elem,
         void *cl);
void rotate180(int col, int row, A2Methods_UArray2 array2b, void *elem,
         void *cl);
void rotate270(int col, int row, A2Methods_UArray2 array2b, void *elem,
         void *cl);
void flipHorizontal(int row, int col, A2Methods_UArray2 array2b, void *elem,
         void *cl);
void flipVertical(int row, int col, A2Methods_UArray2 array2b, void *elem,
         void *cl);


static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] [filename]\n",
                        progname);
        exit(1);
}

int main(int argc, char *argv[]) 
{
        char *time_file_name = NULL;
        FILE *timer = NULL;
        timer = fopen("Recorder.txt", "w");
        assert(timer != NULL);
        int rotation = 0;
        int i;
        CPUTime_T time;
        double time_used;

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        assert(map);

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major, 
                                    "row-major");
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major, 
                                    "column-major");
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                } else if (strcmp(argv[i], "-rotate") == 0) {
                        if (!(i + 1 < argc)) {
                                fclose(timer);/* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr, 
                                        "Rotation must be 0, 90 180 or 270\n");
                                fclose(timer);
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {
                                fclose(timer); /* Not a number */
                                usage(argv[0]);
                        } 
                        
                } else if (strcmp(argv[i], "-time") == 0) {
                        time_file_name = argv[++i];
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                        fclose(timer);
                }
                else if (argc - i > 1)
                {
                        fprintf(stderr, "Too many arguments\n");
                        fclose(timer);
                        usage(argv[0]);
                }
                else
                {
                        break;
                }
        }

        FILE *file = NULL;
        
        file = get_file(&file, argc, argv, i);
        if (file == NULL) {
                fclose(timer);
                fprintf(stderr, "Error: File unable to be opened.\n");
                exit(EXIT_FAILURE);        
        }
        assert(file != NULL);

        Pnm_ppm pic = Pnm_ppmread(file, methods);

        int picW = pic->width;
        int picH = pic->height;
        fclose(file);

        time = CPUTime_New();

        info *passedin = malloc(sizeof(*passedin));
        passedin -> methods = methods;
        A2Methods_UArray2 output;

        if(rotation == 90){
                output = methods -> new(pic->height, pic->width,
                        (sizeof (struct Pnm_rgb)));
                passedin->output = output;
                CPUTime_Start(time); 
                map(pic->pixels, rotate90, passedin);
                time_used = CPUTime_Stop(time);
                picW = methods -> height(pic->pixels);
                picH = methods -> width(pic->pixels);
        } else if (rotation == 180) {
                output = methods -> new(pic->width, pic->height,
                        (sizeof (struct Pnm_rgb)));
                passedin -> output = output;
                CPUTime_Start(time); 
                map(pic->pixels, rotate180, passedin);
                time_used = CPUTime_Stop(time);
        }
        else if (rotation == 270)
        {
                output = methods -> new(pic->height, pic->width,
                        (sizeof (struct Pnm_rgb)));
                passedin -> output = output;
                CPUTime_Start(time); 
                map(pic -> pixels, rotate270, passedin);
                time_used = CPUTime_Stop(time);
                picW = methods->height(pic->pixels);
                picH = methods->width(pic->pixels);
                
        }

        fprintf(timer, "Rotation completed in %f nanoseconds.\n", 
                                        time_used);
        fprintf(timer, "Number of Pixels = %d\n", picW * picH);
        fprintf(timer, "Average time per pixel: %f nanoseconds.\n", 
                                time_used/(picW * picH));
        if (time_file_name != NULL)
        {
                timer = fopen(time_file_name, "w");
                if (timer != NULL)
                {
                        fprintf(timer,
                                "Rotation completed in %f nanoseconds.\n",
                                time_used);
                        fprintf(timer, "Time per pixel: %f nanoseconds.\n",
                                time_used/(picW * picH));
                }
                else {
                        exit(EXIT_FAILURE);
                }
        }
        methods -> free(&pic->pixels);
        pic -> pixels = output;
        pic -> width = picW;
        pic -> height = picH;

        Pnm_ppmwrite(stdout, pic);

        fclose(timer);
        /* Free pixmap pointed to by ppmp. Does not free methods. */
        Pnm_ppmfree(&pic);
        free(passedin);
        CPUTime_Free(&time);
        return 0;
}

void rotate90(int col, int row, A2Methods_UArray2 array2b, void *elem,
                void *cl)
{
        *(Pnm_rgb) ((info *) cl) -> methods -> 
                at((A2Methods_UArray2 *)((info *) cl) -> output, 
                (((info *) cl)->methods->height(array2b)) - row - 1,
                col) = *(Pnm_rgb)elem;
}

/*      
 *      Rotates an inputted UArray2 180 degrees
 *      Gets: Row, col, a UArray2, the file to be written to and
 *      our methods suite
 *      Returns: Nothing
 */
void rotate180(int col, int row, A2Methods_UArray2 array2b,
                void *elem, void *cl)
{

        *(Pnm_rgb) ((info *)cl)->methods -> 
                at((A2Methods_UArray2 *)((info *) cl) -> output,
                (((info *) cl)->methods -> width(array2b)) - col - 1, // COL
                (((info *) cl)->methods -> height(array2b)) - row - 1) //ROW
                = *(Pnm_rgb)elem;
}
/*      
 *      Rotates an inputted UArray2 270 degrees
 *      Gets: Row, col, a UArray2, the file to be written to and
 *      our methods suite
 *      Returns: Nothing
 */
void rotate270(int col, int row, A2Methods_UArray2 array2b,
                void *elem, void *cl)
{
        *(Pnm_rgb)((info *)cl)->methods -> 
                at((A2Methods_UArray2 *)((info *)cl) -> output,
                row,   //COLUMN
                (((info *)cl)->methods->width(array2b)) - col - 1) //ROW
                = *(Pnm_rgb)elem;
}

/*      
 *      Rotates an inputted UArray2 0 degrees
 *      Gets: Row, col, a UArray2, the file to be written to and
 *      our methods suite
 *      Returns: Nothing
 */
void rotate0(int row, int col, A2Methods_UArray2 array2b,
                void *elem, void *cl)
{
        (void) row;
        (void) col;
        (void) array2b;
        (void) elem;
        (void) cl;
}
/*      
 *      Flips an image horizontally
 *      Gets: Row, col, a UArray2, the file to be written
 *      to and our methods suite
 *      Returns: Nothing
 */
void flipHorizontal(int row, int col, A2Methods_UArray2 array2b,
                        void *elem, void *cl)
{
        *(Pnm_rgb) ((info *) cl)->methods -> 
        at((A2Methods_UArray2 *)((info *) cl) -> output,
         (((info *) cl)->methods -> height(array2b)) - row - 1, col) 
        = *(Pnm_rgb)elem;
}

/*      
 *      Flips an image vertically
 *      Gets: Row, col, a UArray2, the file to be written to and
 *      our methods suite
 *      Returns: Nothing
 */
void flipVertical(int row, int col, A2Methods_UArray2 array2b,
         void *elem, void *cl)
{
        (void) array2b;
        *(Pnm_rgb) uarray2_methods_plain -> at((A2Methods_UArray2 *)cl, col,
                (uarray2_methods_plain -> 
                height((A2Methods_UArray2 *) cl) - row - 1)) = *(Pnm_rgb)elem;
}


/*
 *      Checks if the input is valid. Returns NULL if not. Returns 
 *      pointer to file if it is acceptable. If too many files are
 *      inputted, an error is printed and program exits with code
 *      EXIT_FAILURE. 
 *      Gets: argc (number of arguments), argv (command line arguments)
 *      Returns: Pointer to file
 */
FILE *get_file(FILE **file, int argc, char *argv[], int i)
{
    if(argc - i == 0){
            *file = stdin;
            return *file;
    } else {
            *file = fopen(argv[argc- 1], "r");
            return *file;
    } 
    return NULL;
}
