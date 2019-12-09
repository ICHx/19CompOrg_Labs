/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

#define BLOCKDIM 8/* when block dimension equals to 8, we get the best performance */
#define MIN(x, y) (((x) < (y))? (x) : (y))
#define MAX(x, y) (((x) > (y))? (x) : (y))

/*
 * Please fill in the following team struct
 */
team_t team = {
        "LongTimeNoSee",          /* Team name */

        "WilliamHe",              /* First member full name */
        "2018053277",             /* First member email address */

        "RTsui",                  /* Second member full name (leave blank if none) */
        "2018058445"              /* Second member email addr (leave blank if none) */
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
void naive_rotate(int dim, pixel *src, pixel *dst) {
    int i, j;

    for (i = 0; i < dim; i++)
        for (j = 0; j < dim; j++)
            dst[RIDX(dim - 1 - j, i, dim)] = src[RIDX(i, j, dim)];
}

/*
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char rotate_descr[] = "rotate: Partial blocks with unrolling the loop";
void rotate(int dim, pixel *src, pixel *dst) {
    int i, m, n;
    for (m = 0; m < dim; m += BLOCKDIM) {/* partial row traversal */
        for (n = 0; n < dim; n += BLOCKDIM) {/* partial column traversal */
            for (i = m; i < m + BLOCKDIM; i++) {/* row traversal inside one block */
                /* column traversal with loop unrolling */
                dst[RIDX(dim - 1 - n, i, dim)] = src[RIDX(i, n, dim)];
                dst[RIDX(dim - 2 - n, i, dim)] = src[RIDX(i, n + 1, dim)];
                dst[RIDX(dim - 3 - n, i, dim)] = src[RIDX(i, n + 2, dim)];
                dst[RIDX(dim - 4 - n, i, dim)] = src[RIDX(i, n + 3, dim)];
                dst[RIDX(dim - 5 - n, i, dim)] = src[RIDX(i, n + 4, dim)];
                dst[RIDX(dim - 6 - n, i, dim)] = src[RIDX(i, n + 5, dim)];
                dst[RIDX(dim - 7 - n, i, dim)] = src[RIDX(i, n + 6, dim)];
                dst[RIDX(dim - 8 - n, i, dim)] = src[RIDX(i, n + 7, dim)];
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

void register_rotate_functions() {
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
static void initialize_pixel_sum(pixel_sum *sum) {
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/*
 * accumulate_sum - Accumulates field values of p in corresponding
 * fields of sum
 */
static void accumulate_sum(pixel_sum *sum, pixel p) {
    sum->red += (int) p.red;
    sum->green += (int) p.green;
    sum->blue += (int) p.blue;
    sum->num++;
    return;
}

/*
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel
 */

static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum) {
    current_pixel->red = (unsigned short) (sum.red / sum.num);
    current_pixel->green = (unsigned short) (sum.green / sum.num);
    current_pixel->blue = (unsigned short) (sum.blue / sum.num);
}

static void assign_sum_to_pixel10(pixel *current_pixel, pixel_sum sum) {
    switch (sum.num) { //not done yet
        case 4:
	        current_pixel->red = (sum.red >> 2);
		    current_pixel->green = (sum.green >> 2);
		    current_pixel->blue = (sum.blue >> 2);
            break;
        case 6:
	        current_pixel->red = (sum.red / 6);
		    current_pixel->green = (sum.green / 6);
		    current_pixel->blue = (sum.blue / 6);
            break;
        default:
	        current_pixel->red = (sum.red / 9);
		    current_pixel->green = (sum.green / 9);
		    current_pixel->blue = (sum.blue / 9);
            break;
    }
}
/*
 * avg - Returns averaged pixel value at (i,j)
 */
static pixel avg10(int dim, int i, int j, pixel *src) {
    int ii, jj, ii_lim, jj_lim;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);

    ii_lim = MIN(i + 1, dim - 1);
    jj_lim = MIN(j + 1, dim - 1);

    ii = MAX(i - 1, 0);
    for (; ii <= ii_lim; ii++) {
        jj = MAX(j - 1, 0);

        for (; jj <= jj_lim; jj++) {
//			accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);   //CPE=10.2,10.1x,10.3x
            sum.red += (int) src[RIDX(ii, jj, dim)].red;    //CPE= 10.7x,10.3x,10.6x
            sum.green += (int) src[RIDX(ii, jj, dim)].green;
            sum.blue += (int) src[RIDX(ii, jj, dim)].blue;
            sum.num++;
        }
    }
    assign_sum_to_pixel10(&current_pixel, sum);
    return current_pixel;
}
/*
 * avg - Returns averaged pixel value at (i,j)
 */
static pixel avg(int dim, int i, int j, pixel *src) {
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for (ii = max(i - 1, 0); ii <= min(i + 1, dim - 1); ii++)
        for (jj = max(j - 1, 0); jj <= min(j + 1, dim - 1); jj++) {
            accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);
        }

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
void naive_smooth(int dim, pixel *src, pixel *dst) {
    int i, j;

    for (i = 0; i < dim; i++)
        for (j = 0; j < dim; j++)
            dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
}

/*
 * smooth - Your current working version of smooth.
 * IMPORTANT: This is the version you will be graded on
 */
char smooth_descr[] = "smooth10: original working version with improved avg() function";
void smooth(int dim, pixel *src, pixel *dst) {
    int i, j;

    for (i = 0; i < dim; i++)
        for (j = 0; j < dim; j++)
            dst[RIDX(i, j, dim)] = avg10(dim, i, j, src);
}

char smooth_descr2[] = "smooth2: Removed unnecessary function call and using binary shift";
void smooth2(int dim, pixel *src, pixel *dst)
{
    int i, j;
    int targetPosition;
	int r, g, b;
    {
        {
            targetPosition = 0;// position of target pixel
            r = src[0].red + src[1].red + src[dim].red + src[dim + 1].red;
            g = src[0].green + src[1].green + src[dim].green + src[dim + 1].green;
            b = src[0].blue + src[1].blue + src[dim].blue + src[dim + 1].blue;
	
	        dst[targetPosition].red = (r >> 2);
	        dst[targetPosition].green = (g >> 2);
	        dst[targetPosition].blue = (b >> 2);
        }//end of up-left corner
        {
            targetPosition = dim - 1;// position of target pixel
            r = src[dim - 1].red + src[dim - 2].red + src[dim + dim - 2].red + src[dim + dim - 1].red;
            g = src[dim - 1].green + src[dim - 2].green + src[dim + dim - 2].green + src[dim + dim - 1].green;
            b = src[dim - 1].blue + src[dim - 2].blue + src[dim + dim - 2].blue + src[dim + dim - 1].blue;
	
	        dst[targetPosition].red = (r >> 2);
	        dst[targetPosition].green = (g >> 2);
	        dst[targetPosition].blue = (b >> 2);
        }//end of up-right corner

        for (j = 1; j < dim - 1; j++) {// column traversal
            targetPosition = j;// position of target pixel
            r = src[targetPosition - 1].red + src[targetPosition].red + src[targetPosition + 1].red +
                src[targetPosition + dim - 1].red + src[targetPosition + dim].red + src[targetPosition + dim + 1].red;
            g = src[targetPosition - 1].green + src[targetPosition].green + src[targetPosition + 1].green +
                src[targetPosition + dim - 1].green + src[targetPosition + dim].green +
                src[targetPosition + dim + 1].green;
            b = src[targetPosition - 1].blue + src[targetPosition].blue + src[targetPosition + 1].blue +
                src[targetPosition + dim - 1].blue + src[targetPosition + dim].blue +
                src[targetPosition + dim + 1].blue;
	
	        dst[targetPosition].red = (r / 6);
	        dst[targetPosition].green = (g / 6);
	        dst[targetPosition].blue = (b / 6);
        }//first row remaining
    }//end of first row

    {
        {
            targetPosition = dim * (dim - 1);// position of target pixel
            r = src[dim * (dim - 1)].red + src[dim * (dim - 1) + 1].red + src[dim * (dim - 2)].red +
                src[dim * (dim - 2) + 1].red;
            g = src[dim * (dim - 1)].green + src[dim * (dim - 1) + 1].green + src[dim * (dim - 2)].green +
                src[dim * (dim - 2) + 1].green;
            b = src[dim * (dim - 1)].blue + src[dim * (dim - 1) + 1].blue + src[dim * (dim - 2)].blue +
                src[dim * (dim - 2) + 1].blue;
	
	        dst[targetPosition].red = (r >> 2);
	        dst[targetPosition].green = (g >> 2);
	        dst[targetPosition].blue = (b >> 2);
        }//end of low-left corner
        {
            targetPosition = dim * dim - 1;// position of target pixel
            r = src[dim * dim - 1].red + src[dim * dim - 2].red + src[dim * (dim - 1) - 1].red +
                src[dim * (dim - 1) - 2].red;
            g = src[dim * dim - 1].green + src[dim * dim - 2].green + src[dim * (dim - 1) - 1].green +
                src[dim * (dim - 1) - 2].green;
            b = src[dim * dim - 1].blue + src[dim * dim - 2].blue + src[dim * (dim - 1) - 1].blue +
                src[dim * (dim - 1) - 2].blue;
	
	        dst[targetPosition].red = (r >> 2);
	        dst[targetPosition].green = (g >> 2);
	        dst[targetPosition].blue = (b >> 2);
        }//end of low-right corner

        for (j = 1; j < dim - 1; j++) {// column traversal
            targetPosition = dim * (dim - 1) + j;// position of target pixel
            r = src[targetPosition - dim - 1].red + src[targetPosition - dim].red + src[targetPosition - dim + 1].red +
                src[targetPosition - 1].red + src[targetPosition].red + src[targetPosition + 1].red;
            g = src[targetPosition - dim - 1].green + src[targetPosition - dim].green +
                src[targetPosition - dim + 1].green +
                src[targetPosition - 1].green + src[targetPosition].green + src[targetPosition + 1].green;
            b = src[targetPosition - dim - 1].blue + src[targetPosition - dim].blue +
                src[targetPosition - dim + 1].blue +
                src[targetPosition - 1].blue + src[targetPosition].blue + src[targetPosition + 1].blue;
	
	        dst[targetPosition].red = (r / 6);
	        dst[targetPosition].green = (g / 6);
	        dst[targetPosition].blue = (b / 6);
        }
    }//end last row

    {
        for (i = 1; i < dim - 1; i++) {
            targetPosition = dim * i;// position of target pixel
            r = src[targetPosition - dim].red + src[targetPosition - dim + 1].red + src[targetPosition].red +
                src[targetPosition + 1].red + src[targetPosition + dim].red + src[targetPosition + dim + 1].red;
            g = src[targetPosition - dim].green + src[targetPosition - dim + 1].green + src[targetPosition].green +
                src[targetPosition + 1].green + src[targetPosition + dim].green + src[targetPosition + dim + 1].green;
            b = src[targetPosition - dim].blue + src[targetPosition - dim + 1].blue + src[targetPosition].blue +
                src[targetPosition + 1].blue + src[targetPosition + dim].blue + src[targetPosition + dim + 1].blue;
	
	        dst[targetPosition].red = (r / 6);
	        dst[targetPosition].green = (g / 6);
	        dst[targetPosition].blue = (b / 6);
        }
    }// end of left most column

    {
        for (i = 1; i < dim - 1; i++) {
            targetPosition = dim * i + dim - 1;// position of target pixel
            r = src[targetPosition - dim - 1].red + src[targetPosition - dim].red + src[targetPosition].red +
                src[targetPosition - 1].red + src[targetPosition + dim - 1].red + src[targetPosition + dim].red;
            g = src[targetPosition - dim - 1].green + src[targetPosition - dim].green + src[targetPosition].green +
                src[targetPosition - 1].green + src[targetPosition + dim - 1].green + src[targetPosition + dim].green;
            b = src[targetPosition - dim - 1].blue + src[targetPosition - dim].blue + src[targetPosition].blue +
                src[targetPosition - 1].blue + src[targetPosition + dim - 1].blue + src[targetPosition + dim].blue;
	
	        dst[targetPosition].red = (r / 6);
	        dst[targetPosition].green = (g / 6);
	        dst[targetPosition].blue = (b / 6);
        }
    }// end of right most column

    for (i = 1; i < dim - 1; i++) {// row traversal
        for (j = 1; j < dim - 1; j++) {// column traversal
            targetPosition = dim * i + j;// position of target pixel
            // loop unrolling that calculate the sum of RGB value respectively
            r = src[targetPosition - dim - 1].red + src[targetPosition - dim].red + src[targetPosition - dim + 1].red +
                src[targetPosition - 1].red + src[targetPosition].red + src[targetPosition + 1].red +
                src[targetPosition + dim - 1].red + src[targetPosition + dim].red + src[targetPosition + dim + 1].red;
            g = src[targetPosition - dim - 1].green + src[targetPosition - dim].green +
                src[targetPosition - dim + 1].green +
                src[targetPosition - 1].green + src[targetPosition].green + src[targetPosition + 1].green +
                src[targetPosition + dim - 1].green + src[targetPosition + dim].green +
                src[targetPosition + dim + 1].green;
            b = src[targetPosition - dim - 1].blue + src[targetPosition - dim].blue +
                src[targetPosition - dim + 1].blue +
                src[targetPosition - 1].blue + src[targetPosition].blue + src[targetPosition + 1].blue +
                src[targetPosition + dim - 1].blue + src[targetPosition + dim].blue +
                src[targetPosition + dim + 1].blue;
	
	        dst[targetPosition].red = (r / 9);
	        dst[targetPosition].green = (g / 9);
	        dst[targetPosition].blue = (b / 9);   //combined mean: 72.2, 10.1x
        }
    }//end traversal of inner pixels
}

char smooth_descr2u[] = "smooth2u: Preserved unsigned";
void smooth2u(int dim, pixel *src, pixel *dst) {
	int i, j;
	int targetPosition;
	unsigned r, g, b;
	{
		{
			targetPosition = 0;// position of target pixel
			r = src[0].red + src[1].red + src[dim].red + src[dim + 1].red;
			g = src[0].green + src[1].green + src[dim].green + src[dim + 1].green;
			b = src[0].blue + src[1].blue + src[dim].blue + src[dim + 1].blue;
			
			dst[targetPosition].red = (unsigned short) (r >> 2);
			dst[targetPosition].green = (unsigned short) (g >> 2);
			dst[targetPosition].blue = (unsigned short) (b >> 2);
		}//end of up-left corner
		{
			targetPosition = dim - 1;// position of target pixel
			r = src[dim - 1].red + src[dim - 2].red + src[dim + dim - 2].red + src[dim + dim - 1].red;
			g = src[dim - 1].green + src[dim - 2].green + src[dim + dim - 2].green + src[dim + dim - 1].green;
			b = src[dim - 1].blue + src[dim - 2].blue + src[dim + dim - 2].blue + src[dim + dim - 1].blue;
			
			dst[targetPosition].red = (unsigned short) (r >> 2);
			dst[targetPosition].green = (unsigned short) (g >> 2);
			dst[targetPosition].blue = (unsigned short) (b >> 2);
		}//end of up-right corner
		
		for (j = 1; j < dim - 1; j++) {// column traversal
			targetPosition = j;// position of target pixel
			r = src[targetPosition - 1].red + src[targetPosition].red + src[targetPosition + 1].red +
			    src[targetPosition + dim - 1].red + src[targetPosition + dim].red + src[targetPosition + dim + 1].red;
			g = src[targetPosition - 1].green + src[targetPosition].green + src[targetPosition + 1].green +
			    src[targetPosition + dim - 1].green + src[targetPosition + dim].green +
			    src[targetPosition + dim + 1].green;
			b = src[targetPosition - 1].blue + src[targetPosition].blue + src[targetPosition + 1].blue +
			    src[targetPosition + dim - 1].blue + src[targetPosition + dim].blue +
			    src[targetPosition + dim + 1].blue;
			
			dst[targetPosition].red = (unsigned short) (r / 6);
			dst[targetPosition].green = (unsigned short) (g / 6);
			dst[targetPosition].blue = (unsigned short) (b / 6);
		}//first row remaining
	}//end of first row
	
	{
		{
			targetPosition = dim * (dim - 1);// position of target pixel
			r = src[dim * (dim - 1)].red + src[dim * (dim - 1) + 1].red + src[dim * (dim - 2)].red +
			    src[dim * (dim - 2) + 1].red;
			g = src[dim * (dim - 1)].green + src[dim * (dim - 1) + 1].green + src[dim * (dim - 2)].green +
			    src[dim * (dim - 2) + 1].green;
			b = src[dim * (dim - 1)].blue + src[dim * (dim - 1) + 1].blue + src[dim * (dim - 2)].blue +
			    src[dim * (dim - 2) + 1].blue;
			
			dst[targetPosition].red = (unsigned short) (r >> 2);
			dst[targetPosition].green = (unsigned short) (g >> 2);
			dst[targetPosition].blue = (unsigned short) (b >> 2);
		}//end of low-left corner
		{
			targetPosition = dim * dim - 1;// position of target pixel
			r = src[dim * dim - 1].red + src[dim * dim - 2].red + src[dim * (dim - 1) - 1].red +
			    src[dim * (dim - 1) - 2].red;
			g = src[dim * dim - 1].green + src[dim * dim - 2].green + src[dim * (dim - 1) - 1].green +
			    src[dim * (dim - 1) - 2].green;
			b = src[dim * dim - 1].blue + src[dim * dim - 2].blue + src[dim * (dim - 1) - 1].blue +
			    src[dim * (dim - 1) - 2].blue;
			
			dst[targetPosition].red = (unsigned short) (r >> 2);
			dst[targetPosition].green = (unsigned short) (g >> 2);
			dst[targetPosition].blue = (unsigned short) (b >> 2);
		}//end of low-right corner
		
		for (j = 1; j < dim - 1; j++) {// column traversal
			targetPosition = dim * (dim - 1) + j;// position of target pixel
			r = src[targetPosition - dim - 1].red + src[targetPosition - dim].red + src[targetPosition - dim + 1].red +
			    src[targetPosition - 1].red + src[targetPosition].red + src[targetPosition + 1].red;
			g = src[targetPosition - dim - 1].green + src[targetPosition - dim].green +
			    src[targetPosition - dim + 1].green +
			    src[targetPosition - 1].green + src[targetPosition].green + src[targetPosition + 1].green;
			b = src[targetPosition - dim - 1].blue + src[targetPosition - dim].blue +
			    src[targetPosition - dim + 1].blue +
			    src[targetPosition - 1].blue + src[targetPosition].blue + src[targetPosition + 1].blue;
			
			dst[targetPosition].red = (unsigned short) (r / 6);
			dst[targetPosition].green = (unsigned short) (g / 6);
			dst[targetPosition].blue = (unsigned short) (b / 6);
		}
	}//end last row
	
	{
		for (i = 1; i < dim - 1; i++) {
			targetPosition = dim * i;// position of target pixel
			r = src[targetPosition - dim].red + src[targetPosition - dim + 1].red + src[targetPosition].red +
			    src[targetPosition + 1].red + src[targetPosition + dim].red + src[targetPosition + dim + 1].red;
			g = src[targetPosition - dim].green + src[targetPosition - dim + 1].green + src[targetPosition].green +
			    src[targetPosition + 1].green + src[targetPosition + dim].green + src[targetPosition + dim + 1].green;
			b = src[targetPosition - dim].blue + src[targetPosition - dim + 1].blue + src[targetPosition].blue +
			    src[targetPosition + 1].blue + src[targetPosition + dim].blue + src[targetPosition + dim + 1].blue;
			
			dst[targetPosition].red = (unsigned short) (r / 6);
			dst[targetPosition].green = (unsigned short) (g / 6);
			dst[targetPosition].blue = (unsigned short) (b / 6);
		}
	}// end of left most column
	
	{
		for (i = 1; i < dim - 1; i++) {
			targetPosition = dim * i + dim - 1;// position of target pixel
			r = src[targetPosition - dim - 1].red + src[targetPosition - dim].red + src[targetPosition].red +
			    src[targetPosition - 1].red + src[targetPosition + dim - 1].red + src[targetPosition + dim].red;
			g = src[targetPosition - dim - 1].green + src[targetPosition - dim].green + src[targetPosition].green +
			    src[targetPosition - 1].green + src[targetPosition + dim - 1].green + src[targetPosition + dim].green;
			b = src[targetPosition - dim - 1].blue + src[targetPosition - dim].blue + src[targetPosition].blue +
			    src[targetPosition - 1].blue + src[targetPosition + dim - 1].blue + src[targetPosition + dim].blue;
			
			dst[targetPosition].red = (unsigned short) (r / 6);
			dst[targetPosition].green = (unsigned short) (g / 6);
			dst[targetPosition].blue = (unsigned short) (b / 6);
		}
	}// end of right most column
	
	for (i = 1; i < dim - 1; i++) {// row traversal
		for (j = 1; j < dim - 1; j++) {// column traversal
			targetPosition = dim * i + j;// position of target pixel
			// loop unrolling that calculate the sum of RGB value respectively
			r = src[targetPosition - dim - 1].red + src[targetPosition - dim].red + src[targetPosition - dim + 1].red +
			    src[targetPosition - 1].red + src[targetPosition].red + src[targetPosition + 1].red +
			    src[targetPosition + dim - 1].red + src[targetPosition + dim].red + src[targetPosition + dim + 1].red;
			g = src[targetPosition - dim - 1].green + src[targetPosition - dim].green +
			    src[targetPosition - dim + 1].green +
			    src[targetPosition - 1].green + src[targetPosition].green + src[targetPosition + 1].green +
			    src[targetPosition + dim - 1].green + src[targetPosition + dim].green +
			    src[targetPosition + dim + 1].green;
			b = src[targetPosition - dim - 1].blue + src[targetPosition - dim].blue +
			    src[targetPosition - dim + 1].blue +
			    src[targetPosition - 1].blue + src[targetPosition].blue + src[targetPosition + 1].blue +
			    src[targetPosition + dim - 1].blue + src[targetPosition + dim].blue +
			    src[targetPosition + dim + 1].blue;
			
			dst[targetPosition].red = (unsigned short) (r / 9);
			dst[targetPosition].green = (unsigned short) (g / 9);
			dst[targetPosition].blue = (unsigned short) (b / 9);   //combined mean: 72.2, 10.1x
		}
	}//end traversal of inner pixels
}

char smooth_descr3[] = "smooth3: ICS";// best optimization reference found on the Internet
void smooth3(int dim, pixel *src, pixel *dst) {
	int i, j, myJ;
	
	// up-left corner
	dst[0].red = (src[0].red + src[1].red + src[dim].red + src[dim + 1].red) >> 2;
	dst[0].blue = (src[0].blue + src[1].blue + src[dim].blue + src[dim + 1].blue) >> 2;
	dst[0].green = (src[0].green + src[1].green + src[dim].green + src[dim + 1].green) >> 2;
	
	i = dim * 2 - 1;// up-right corner
	dst[dim - 1].red = (src[dim - 2].red + src[dim - 1].red + src[i - 1].red + src[i].red) >> 2;
	dst[dim - 1].blue = (src[dim - 2].blue + src[dim - 1].blue + src[i - 1].blue + src[i].blue) >> 2;
	dst[dim - 1].green = (src[dim - 2].green + src[dim - 1].green + src[i - 1].green + src[i].green) >> 2;
	
	j = dim * (dim - 1);
	i = dim * (dim - 2);// low-left corner
	dst[j].red = (src[j].red + src[j + 1].red + src[i].red + src[i + 1].red) >> 2;
	dst[j].blue = (src[j].blue + src[j + 1].blue + src[i].blue + src[i + 1].blue) >> 2;
	dst[j].green = (src[j].green + src[j + 1].green + src[i].green + src[i + 1].green) >> 2;
	
	j = dim * dim - 1;
	i = dim * (dim - 1) - 1;// low-right corner
	dst[j].red = (src[j - 1].red + src[j].red + src[i - 1].red + src[i].red) >> 2;
	dst[j].blue = (src[j - 1].blue + src[j].blue + src[i - 1].blue + src[i].blue) >> 2;
	dst[j].green = (src[j - 1].green + src[j].green + src[i - 1].green + src[i].green) >> 2;
	
	//deal with the sides
	i = dim - 1;
	for (j = 1; j < i; j++) {// highest row
		dst[j].red = (src[j].red + src[j - 1].red + src[j + 1].red + src[j + dim].red + src[j + 1 + dim].red +
		              src[j - 1 + dim].red) / 6;
		dst[j].green =
				(src[j].green + src[j - 1].green + src[j + 1].green + src[j + dim].green + src[j + 1 + dim].green +
				 src[j - 1 + dim].green) / 6;
		dst[j].blue = (src[j].blue + src[j - 1].blue + src[j + 1].blue + src[j + dim].blue + src[j + 1 + dim].blue +
		               src[j - 1 + dim].blue) / 6;
	}
	
	i = dim * dim - 1;
	for (j = i - dim + 2; j < i; j++) {// lowest row
		dst[j].red = (src[j].red + src[j - 1].red + src[j + 1].red + src[j - dim].red + src[j + 1 - dim].red +
		              src[j - 1 - dim].red) / 6;
		dst[j].green =
				(src[j].green + src[j - 1].green + src[j + 1].green + src[j - dim].green + src[j + 1 - dim].green +
				 src[j - 1 - dim].green) / 6;
		dst[j].blue = (src[j].blue + src[j - 1].blue + src[j + 1].blue + src[j - dim].blue + src[j + 1 - dim].blue +
		               src[j - 1 - dim].blue) / 6;
	}
	
	for (j = dim + dim - 1; j < dim * dim - 1; j += dim) {// right most column
		dst[j].red = (src[j].red + src[j - 1].red + src[j - dim].red + src[j + dim].red + src[j - dim - 1].red +
		              src[j - 1 + dim].red) / 6;
		dst[j].green =
				(src[j].green + src[j - 1].green + src[j - dim].green + src[j + dim].green + src[j - dim - 1].green +
				 src[j - 1 + dim].green) / 6;
		dst[j].blue = (src[j].blue + src[j - 1].blue + src[j - dim].blue + src[j + dim].blue + src[j - dim - 1].blue +
		               src[j - 1 + dim].blue) / 6;
	}
	
	i = i - (dim - 1);
	for (j = dim; j < i; j += dim) {// left most column
		dst[j].red = (src[j].red + src[j - dim].red + src[j + 1].red + src[j + dim].red + src[j + 1 + dim].red +
		              src[j - dim + 1].red) / 6;
		dst[j].green =
				(src[j].green + src[j - dim].green + src[j + 1].green + src[j + dim].green + src[j + 1 + dim].green +
				 src[j - dim + 1].green) / 6;
		dst[j].blue = (src[j].blue + src[j - dim].blue + src[j + 1].blue + src[j + dim].blue + src[j + 1 + dim].blue +
		               src[j - dim + 1].blue) / 6;
	}
	
	myJ = dim;
	
	for (i = 1; i < dim - 1; i++) {
		for (j = 1; j < dim - 1; j++) {
			myJ++;
			dst[myJ].red =
					(src[myJ - 1].red + src[myJ].red + src[myJ + 1].red + src[myJ - dim - 1].red + src[myJ - dim].red +
					 src[myJ - dim + 1].red + src[myJ + dim - 1].red + src[myJ + dim].red + src[myJ + dim + 1].red) / 9;
			dst[myJ].green = (src[myJ - 1].green + src[myJ].green + src[myJ + 1].green + src[myJ - dim - 1].green +
			                  src[myJ - dim].green + src[myJ - dim + 1].green + src[myJ + dim - 1].green +
			                  src[myJ + dim].green + src[myJ + dim + 1].green) / 9;
			dst[myJ].blue = (src[myJ - 1].blue + src[myJ].blue + src[myJ + 1].blue + src[myJ - dim - 1].blue +
			                 src[myJ - dim].blue + src[myJ - dim + 1].blue + src[myJ + dim - 1].blue +
			                 src[myJ + dim].blue + src[myJ + dim + 1].blue) / 9;
		}
		myJ += 2;
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
    add_smooth_function(&naive_smooth, naive_smooth_descr);
    add_smooth_function(&smooth, smooth_descr);
	add_smooth_function(&smooth2u, smooth_descr2u);
	add_smooth_function(&smooth2, smooth_descr2);
	add_smooth_function(&smooth3, smooth_descr3);
    /* ... Register additional test functions here */
}

