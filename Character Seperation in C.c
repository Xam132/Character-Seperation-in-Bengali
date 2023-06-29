// importing required files
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// maximum gray value
#define B_VAL 255

// structure to save data from the pgm image file
typedef struct PGM
{
    char type[3];
    char **data;
    int width;
    int height;
    int grayvalue;
} PGM;

// ignores the commented information in the input pgm image file
void ignoreComments(FILE *fp)
{
    int ch;
    char line[100];
    while ((ch = fgetc(fp)) != EOF && isspace(ch))
        ;
    if (ch == '#')
    {
        fgets(line, sizeof(line), fp);
        ignoreComments(fp);
    }
    else
    {
        fseek(fp, -1, SEEK_CUR);
    }
}

// opens the image file and takes care of commented items
int openPGM(PGM *pgm, char filename[]) {
    FILE *pgmfile = fopen(filename, "rb");
    if (pgmfile == NULL) {
        printf("File does not exist\n");
        return 0;
    }
    ignoreComments(pgmfile);
    fscanf(pgmfile, "%s", pgm->type);
    ignoreComments(pgmfile);
    fscanf(pgmfile, "%d %d", &(pgm->width),
           &(pgm->height));
    ignoreComments(pgmfile);
    fscanf(pgmfile, "%d", &(pgm->grayvalue));
    ignoreComments(pgmfile);
    pgm->data = malloc(pgm->height * sizeof(unsigned char *));
    if (pgm->type[1] == '5') {
        for (int i = 0; i < pgm->height; i++) {
            pgm->data[i] = malloc(pgm->width *
                                  sizeof(unsigned char));
            fread(pgm->data[i],
                  pgm->width * sizeof(unsigned char),
                  1, pgmfile);
        }
    }
    fclose(pgmfile);
    return 1;
}

// prints the details of image file
void printImageDetails(PGM *pgm, char filename[])
{
    FILE *pgmfile = fopen(filename, "rb");
    char *ext = strrchr(filename, '.');
    if (!ext)
        printf("No extension found in file %s", filename);
    else
        printf("File format     : %s\n", ext + 1);
    printf("PGM File type   : %s\n", pgm->type);
    printf("Width of img    : %d px\n", pgm->width);
    printf("Height of img   : %d px\n", pgm->height);
    printf("Max Gray value  : %d\n", pgm->grayvalue);
    fclose(pgmfile);
}

// saves the image into another file
void saveImage(PGM *pgm, char fname[])
{
    FILE *fp = fopen(fname, "wb");
    fprintf(fp, "%s\n", pgm->type);
    fprintf(fp, "%d %d\n", pgm->width, pgm->height);
    fprintf(fp, "%d\n", pgm->grayvalue);
    for (int i = 0; i < pgm->height; i++)
    {
        for (int j = 0; j < pgm->width; j++)
        {
            fprintf(fp, "%c", pgm->data[i][j]);
        }
    }
    fclose(fp);
}

// converts grayscale image to binary ( only meant to use on binary type images to take care of memory corruption ) 
void binaryPaint(PGM *pgm)
{
    for (int i = 0; i < pgm->height; i++)
    {
        for (int j = 0; j < pgm->width; j++)
        {
            if (pgm->data[i][j] != (char)0)
            {
                pgm->data[i][j] = (char)B_VAL;
            }
        }
    }
}

// simple compare function for qsort in C
int cmp(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

// function to find header height
int header(PGM *pgm)
{
    int *arr = malloc(sizeof(int) * pgm->width);
    for (int j = 0; j < pgm->width; j++)
    {
        for (int i = 0; i < pgm->height; i++)
        {
            if (i == pgm->height / 2)
            {
                arr[j] = pgm->height / 2;
                break;
            }
            if (pgm->data[i][j] == (char)0)
            {
                arr[j] = i;
                break;
            }
        }
    }
    qsort(arr, pgm->width, sizeof(int), cmp);
    int a = arr[pgm->width / 3];
    free(arr);
    return a;
}

// function to find height of letter
int letter_height(PGM *pgm)
{
    int a = header(pgm);
    int *arr = malloc(sizeof(int) * pgm->width);
    for (int j = 0; j < pgm->width; j++)
    {
        for (int i = a; i < pgm->height; i++)
        {
            if (i == pgm->height / 2)
            {
                arr[j] = pgm->height / 2;
                break;
            }
            if (pgm->data[i][j] != (char)0)
            {
                arr[j] = i;
                break;
            }
        }
    }
    qsort(arr, pgm->width, sizeof(int), cmp);
    int b = arr[pgm->width - 1];
    free(arr);
    return b - a;
}

// function to find spacing between two lines of words
int spacing(PGM *pgm,int let_len)
{
    int a = header(pgm);
    int b = let_len;
    int *arr = malloc(sizeof(int) * pgm->width);
    for (int j = 0; j < pgm->width; j++)
    {
        for (int i = a + b; i < pgm->height; i++)
        {
            if (i == pgm->height / 2)
            {
                arr[j] = pgm->height / 2;
                break;
            }
            if (pgm->data[i][j] != (char)B_VAL)
            {
                arr[j] = i;
                break;
            }
        }
    }
    qsort(arr, pgm->width, sizeof(int), cmp);
    int c = arr[pgm->width / 3];
    free(arr);
    return c - a;
}

// recursively provides rightmost index when going in up and right direction
int uright(PGM *pgm,int i,int j,int a){
    if(pgm->data[i][j] == (char)B_VAL || i <= a){
        return j;
    }
    int arr[] = {uright(pgm,i,j+1,a),uright(pgm,i-1,j,a),};
    qsort(arr,2,sizeof(int),cmp);
    return arr[1];
}

// recursively provides rightmost index when going in down and right direction
int dright(PGM *pgm,int i,int j,int a){
    if(pgm->data[i][j] == (char)B_VAL || i <= a){
        return j;
    }
    int arr[] = {dright(pgm,i,j+1,a),dright(pgm,i+1,j,a),};
    qsort(arr,2,sizeof(int),cmp);
    return arr[1];
}

// returns max of two values
int max(int a,int b){
    if(a > b){return a;}
    return b;
}

// To find rightmost position when passing through a letter
int right(PGM *pgm,int i,int j,int a){
    int x = uright(pgm,i,j,a);
    int y = dright(pgm,i,j,a);
    return max(x,y);
}

// To find rightmost position 
int right1(PGM *pgm,int i,int j,int a){
    int ax[5];
    for(int k = -2;k <= 2;k++){
        ax[k + 2] = right(pgm,i+k,j,a);
    }
    qsort(ax,5,sizeof(int),cmp);
    return ax[4];
}

// To seperate connected lines
void blank(PGM *pgm,int i,int z){                         
    pgm->data[i][z + 1] = (char) B_VAL;
    pgm->data[i + 1][z + 1] = (char) B_VAL;
    pgm->data[i + 2][z + 1] = (char) B_VAL;
    pgm->data[i][z + 2] = (char) B_VAL;
    pgm->data[i + 1][z + 2] = (char) B_VAL;
    pgm->data[i + 2][z + 2] = (char) B_VAL;
    pgm->data[i][z + 3] = (char) B_VAL;
    pgm->data[i + 1][z + 3] = (char) B_VAL;
    pgm->data[i + 2][z + 3] = (char) B_VAL;
}

// Function scans the letters and makes gaps where the individual letters end
void oper1(PGM *pgm){
    int a = header(pgm);
    int b = letter_height(pgm);
    int c = spacing(pgm,b);
    int y,z,k;
    for(int i = a;i < pgm->height;i = i+c){
        for(int j = 0;j < pgm->width;j++) {
            z = right1(pgm, i + b / 2 + 2, j, i + 2);
            if (z > j+1) {
                k = 0;
                for (int l = 3; l < 7; l++) {
                    if (pgm->data[i + b / 2 - 1][z + l] == (char) 0) {
                        y = right(pgm, i + b / 2 - 1, z + l, i + 2);    // second check to ensure any breaked characters are properly checked
                        k = l;
                        if(y <= z+k+5) {
                            z = y;
                            blank(pgm, i, z);
                        }
                        else{ blank(pgm,i,z);}
                        break;
                    }
                }
                if(k == 0){
                    blank(pgm,i,z);
                }
            }
            j = z;
        }
    }
}

int main()
{
    PGM *pgm = malloc(sizeof(PGM));
    printf("Enter the PGM file name with extension\n");
    char ch[30];
    scanf("%s", ch);
    if (openPGM(pgm, ch))
    {
        printImageDetails(pgm, ch);
        binaryPaint(pgm);
        oper1(pgm);
        saveImage(pgm,"r.pgm");
        free(pgm->data);
        free(pgm);
    }
    return 0;
}
