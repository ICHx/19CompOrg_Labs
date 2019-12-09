/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

#define BLOCKDIM 8/* 分块规模为8时性能最佳 */
#define HALFBLOCKDIM 4
#define MAX(x, y) x > y? x : y
#define MIN(x, y) x < y? x : y

/* 
 * Please fill in the following team struct 
 */
team_t team = {
    "solution",           /* Team name */

    "Your Name?",      /* First member full name */
    "Fill in your email",  /* First member email address */

    "other member?",                   /* Second member full name (leave blank if none) */
    "His/Her Email?"                    /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/* 
 * naive_rotate - The naive baseline version of rotate 
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
}

/* 
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char rotate_descr[] = "rotate: Current working version";
void rotate(int dim, pixel *src, pixel *dst)
{
    int i, m, n;
    for (m = 0; m < dim; m += BLOCKDIM){/* 分块行遍历 */
        for (n = 0; n < dim; n += BLOCKDIM){/* 分块列遍历 */
            for (i = m; i < m + BLOCKDIM; i++){/* 小块内行遍历 */
                /* 小块内列遍历（已循环展开） */
                dst[RIDX(dim-1-n, i, dim)] = src[RIDX(i, n, dim)];
                dst[RIDX(dim-2-n, i, dim)] = src[RIDX(i, n+1, dim)];
                dst[RIDX(dim-3-n, i, dim)] = src[RIDX(i, n+2, dim)];
                dst[RIDX(dim-4-n, i, dim)] = src[RIDX(i, n+3, dim)];
                dst[RIDX(dim-5-n, i, dim)] = src[RIDX(i, n+4, dim)];
                dst[RIDX(dim-6-n, i, dim)] = src[RIDX(i, n+5, dim)];
                dst[RIDX(dim-7-n, i, dim)] = src[RIDX(i, n+6, dim)];
                dst[RIDX(dim-8-n, i, dim)] = src[RIDX(i, n+7, dim)];
            }
        }
    }
}

/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_rotate_functions() 
{
    add_rotate_function(&naive_rotate, naive_rotate_descr);   
    add_rotate_function(&rotate, rotate_descr);   
    /* ... Register additional test functions here */
}


/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct {
    int red;
    int green;
    int blue;
    int num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
static int min(int a, int b) { return (a < b ? a : b); }
static int max(int a, int b) { return (a > b ? a : b); }

/* 
 * initialize_pixel_sum - Initializes all fields of sum to 0 
 */
static void initialize_pixel_sum(pixel_sum *sum) 
{
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/* 
 * accumulate_sum - Accumulates field values of p in corresponding 
 * fields of sum 
 */
static void accumulate_sum(pixel_sum *sum, pixel p) 
{
    sum->red += (int) p.red;
    sum->green += (int) p.green;
    sum->blue += (int) p.blue;
    sum->num++;
    return;
}

/* 
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel 
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum) 
{
    current_pixel->red = (unsigned short) (sum.red/sum.num);
    current_pixel->green = (unsigned short) (sum.green/sum.num);
    current_pixel->blue = (unsigned short) (sum.blue/sum.num);
    return;
}

/* 
 * avg - Returns averaged pixel value at (i,j) 
 */
static pixel avg(int dim, int i, int j, pixel *src) 
{
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for(ii = max(i-1, 0); ii <= min(i+1, dim-1); ii++) 
	for(jj = max(j-1, 0); jj <= min(j+1, dim-1); jj++) 
	    accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}

/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth 
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
}

/*
 * smooth - Your current working version of smooth. 
 * IMPORTANT: This is the version you will be graded on
 */
char smooth_descr[] = "smooth: Current working version";
void smooth(int dim, pixel *src, pixel *dst) 
{
//    naive_smooth(dim, src, dst);
    int i, j;

    dst[0].red = (src[0].red + src[1].red + src[RIDX(1, 0, dim)].red + src[RIDX(1, 1, dim)].red)>>2;
    dst[0].green = (src[0].green + src[1].green + src[RIDX(1, 0, dim)].green + src[RIDX(1, 1, dim)].green)>>2;
    dst[0].blue = (src[0].blue + src[1].blue + src[RIDX(1, 0, dim)].blue + src[RIDX(1, 1, dim)].blue)>>2;

    int m, n;
    for (m = 0; m < dim; m += dim - 1){
        for (n = 0; n < dim; n += dim - 1){/* 自己+上下+左右+斜线 */
            dst[RIDX(m, n, dim)].red = (src[RIDX(m, n, dim)].red + src[RIDX(MIN(m + 1, dim - 2), n, dim)].red + \
            src[RIDX(m, MIN(n + 1, dim - 2), dim)].red + src[RIDX(MIN(m + 1, dim - 2), MIN(n + 1, dim - 2), dim)].red)>>2;
            dst[RIDX(m, n, dim)].green = (src[RIDX(m, n, dim)].green + src[RIDX(MIN(m + 1, dim - 2), n, dim)].green + \
            src[RIDX(m, MIN(n + 1, dim - 2), dim)].green + src[RIDX(MIN(m + 1, dim - 2), MIN(n + 1, dim - 2), dim)].green)>>2;
            dst[RIDX(m, n, dim)].blue = (src[RIDX(m, n, dim)].blue + src[RIDX(MIN(m + 1, dim - 2), n, dim)].blue + \
            src[RIDX(m, MIN(n + 1, dim - 2), dim)].blue + src[RIDX(MIN(m + 1, dim - 2), MIN(n + 1, dim - 2), dim)].blue)>>2;
        }
    }

    int k, l;
    for (k = 0; k < 4; k++){
        for (l = 1; l < dim - 2; l++){/* 自己+左+右+下+左下+右下 */
            dst[RIDX(0, l, dim)].red = (src[RIDX(0, l, dim)].red + src[RIDX(0, l - 1, dim)].red + src[RIDX(0, l + 1, dim)].red + \
            src[RIDX(1, l, dim)].red + src[RIDX(1, l - 1, dim)].red + src[RIDX(1, l + 1, dim)].reda>>16*10923;
            dst[RIDX(0, l, dim)].green = (src[RIDX(0, l, dim)].green + src[RIDX(0, l - 1, dim)].green + src[RIDX(0, l + 1, dim)].green + \
            src[RIDX(1, l, dim)].green + src[RIDX(1, l - 1, dim)].green + src[RIDX(1, l + 1, dim)].greena>>16*10923;
            dst[RIDX(0, l, dim)].blue = (src[RIDX(0, l, dim)].blue + src[RIDX(0, l - 1, dim)].blue + src[RIDX(0, l + 1, dim)].blue + \
            src[RIDX(1, l, dim)].blue + src[RIDX(1, l - 1, dim)].blue + src[RIDX(1, l + 1, dim)].bluea>>16*10923; // div 6 ~= a>>16*10923
        }
        for (l = 1; l < dim - 2; l++){/* 自己+左+右+上+左上+右上 */
            dst[RIDX(dim - 1, l, dim)].red = (src[RIDX(dim - 1, l, dim)].red + src[RIDX(dim - 1, l - 1, dim)].red + src[RIDX(dim - 1, l + 1, dim)].red + \
            src[RIDX(dim - 2, l, dim)].red + src[RIDX(dim - 2, l - 1, dim)].red + src[RIDX(dim - 2, l + 1, dim)].reda>>16*10923;
            dst[RIDX(dim - 1, l, dim)].green = (src[RIDX(dim - 1, l, dim)].green + src[RIDX(dim - 1, l - 1, dim)].green + src[RIDX(dim - 1, l + 1, dim)].green + \
            src[RIDX(dim - 2, l, dim)].green + src[RIDX(dim - 2, l - 1, dim)].green + src[RIDX(dim - 2, l + 1, dim)].greena>>16*10923;
            dst[RIDX(dim - 1, l, dim)].blue = (src[RIDX(dim - 1, l, dim)].blue + src[RIDX(dim - 1, l - 1, dim)].blue + src[RIDX(dim - 1, l + 1, dim)].blue + \
            src[RIDX(dim - 2, l, dim)].blue + src[RIDX(dim - 2, l - 1, dim)].blue + src[RIDX(dim - 2, l + 1, dim)].bluea>>16*10923;
        }
    }

    for (i = 0; i < dim; i++){
        for (j = 0; j < dim; j++){

        }
    }
}


/********************************************************************* 
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_smooth_functions() {
    add_smooth_function(&smooth, smooth_descr);
    add_smooth_function(&naive_smooth, naive_smooth_descr);
    /* ... Register additional test functions here */
}

