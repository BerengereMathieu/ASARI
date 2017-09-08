/**
 * @file limace.h
 * @brief to read and write image and matrix
 *
 * Copyright (C) 1999-2011 A. Crouzil  LIMaCE release 1.6 (07/06/2012)
 * Debugging : N. Begue (15/05/2001), G. Jaffre (08/05/2002),
 *             B. Pailhes (02/04/2003), F. Courteille (09/04/2003),
 *             J.-D. Durou (18/08/2005, 08/09/2005), G. Gales (02/03/2011).
 * @author A. Crouzil
 * @version 1.6
 */

#ifndef __limace_h_
#define __limace_h_


#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ON 1 /*< error messages display management */
#define OFF 0 /*< error messages display management  */


/**
 * @brief Verbose Error messages switch on stderr.
 *
 * Each call of the function switch from a state to an other
 * - ON : error message are printing on stdout (default state)
 * - OFF : no error message printing
 * @return the new state (ON or OFF)
 */
extern int Verbose(void);


#ifdef __cplusplus
}
#endif


/************************** Image handling ****************************/

/**
 * @struct Image limace.h
 * @brief Image
 */
typedef struct sImage *Image;

/**
  * @enum ImageType
  * @brief ImageType Image type
  */
typedef enum
{
    LimaceBitMap=0,    /**< black(=0) and white(=1) image [0,1] */
    GrayLevel=1, /**< grayscale image [0,255] */
    Col0r=2      /**< color image (RGB) [0,255]x[0,255]x[0,255] */
} ImageType;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ImAlloc Create an image
 * @param Type[i] image type
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of colums
 * @return the image or NULL if something wrong.
 */
extern Image ImAlloc(ImageType Type, int NbRow, int NbCol);

/**
 * @brief ImCAlloc Create an image initialize to 0
 * @param Type[i] image type
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of colums
 * @return an image or NULL if something wrong.
 */
extern Image ImCAlloc(ImageType Type, int NbRow, int NbCol);

/**
 * @brief ImType getter for image type (BitMap, GrayLevel, Color)
 * @param Im[i] the image
 * @return image type
 */
extern ImageType ImType(Image Im);

/**
 * @brief ImNbRow getter for image number of rows
 * @param Im[i] image
 * @return number of rows
 */
extern int ImNbRow(Image Im);

/**
 * @brief ImNbCol getter for image number of columns
 * @param Im[i] image
 * @return number of columns
 */
extern int ImNbCol(Image Im);

/**
 * @brief ImGetI return access to gray scale matrix of a graylevel or a bitmap image
 * @param Im[i] graylebel or bitmap image
 * @return access to gray scale matrix or NULL if something wrong
 */
extern unsigned char **ImGetI(Image Im);


/**
 * @brief ImGetR return access to red values matrix of an color image
 * @param Im[i] color image
 * @return access to red values matrix or NULL if something wrong
 */
extern unsigned char **ImGetR(Image Im);


/**
 * @brief ImGetR return access to green values matrix of an color image
 * @param Im[i] color image
 * @return access to green values matrix or NULL if something wrong
 */
extern unsigned char **ImGetG(Image Im);

/**
 * @brief ImGetR return access to blue values matrix of an color image
 * @param Im[i] color image
 * @return access to blue values matrix or NULL if something wrong
 */
extern unsigned char **ImGetB(Image Im);


/**
 * @brief ImCopy copy an image
 * @param Im[i] image which must be copied
 * @return a copy of the image or NULL if something wrong
 */
extern Image ImCopy(Image Im);


/**
 * @brief ImRead read a ppm, pgm or pbm image
 * @param FileName[i] image path
 * @return an image or NULL if something wrong
 */
extern Image ImRead(const char FileName[]);

/**
 * @brief ImWrite write a ppm, pgm or pbm image (binary format)
 *
 * if no filename is empty, image is printing on stdout
 * Warning : some troubles can occur on DOS
 * @param Im[i] image
 * @param FileName[i] image path
 */
extern void ImWrite(Image Im, const char FileName[]);

/**
 * @brief ImWrite write a ppm, pgm or pbm image (ASCII format)
 *
 * if no filename is empty, image is printing on stdout
 * Warning : some troubles can occur on DOS
 * @param Im[i] image
 * @param FileName[i] image path
 */
extern void ImWriteAsc(Image Im, const char FileName[]);

/**
 * @brief ImFree delete image
 * @param pIm[i] image
 */
extern void ImFree(Image *pIm);


#ifdef __cplusplus
}
#endif



/************************** Matrix handling *************************/

/**
 * @struct Matrix limace.h
 * @brief Matrix
 */
typedef struct sMatrix *Matrix;

/**
  * @enum MatrixType
  * @brief MatrixType Matrix type (integer or double)
  */
typedef enum
{
    Int=3,
    Double=4
} MatrixType;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MatAlloc create a matrix
 * @param Type[i] matrix type
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of colums
 * @return the matrix or NULL if something wrong
 */
extern Matrix MatAlloc(MatrixType Type, int NbRow, int NbCol);

/**
 * @brief MatCAlloc create a matrix initialize to 0
 * @param Type[i] matrix type
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of colums
 * @return the matrix or NULL if something wrong.
 */
extern Matrix MatCAlloc(MatrixType Type, int NbRow, int NbCol);

/**
 * @brief MatType getter for matrix type
 * @param Mat[i] the matrix
 * @return matrix type
 */
extern MatrixType MatType(Matrix Mat);

/**
 * @brief MatNbRow getter for the number of rows
 * @param Mat[i] the matrix
 * @return number of rows or 0 if something wrong
 */
extern int MatNbRow(Matrix Mat);

/**
 * @brief MatNbCol getter for the number of columns
 * @param Mat[i] the matrix
 * @return number of columns or 0 if something wrong
 */
extern int MatNbCol(Matrix Mat);

/**
 * @brief MatGetInt return access to the elements of a matrix of integers
 * @param Mat[i] the matrix
 * @return access to matrix elements or NULL if something wrong
 */
extern int **MatGetInt(Matrix Mat);

/**
 * @brief MatGetDouble return access to elements of a matrix of double
 * @param Mat[i] the matrix
 * @return access to matrix elements or NULL if something wrong
 */
extern double **MatGetDouble(Matrix Mat);


/**
 * @brief MatCopy copy an matrix
 * @param Mat[i] matric which must be copied
 * @return a copy of the matrix or NULL if something wrong
 */
extern Matrix MatCopy(Matrix Mat);


/**
 * @brief MatReadAsc read a matrix (integer or double) in an ASCII file.
 *
 * If filename is empty matrix is reading on stding.
 * @param FileName[i] path to ASCII file
 * @return the matrix or NULL if something wrong
 */
extern Matrix MatReadAsc(const char FileName[]);



/**
 * @brief MatWriteAsc write a matrix (integer or double) in an ASCII file.
 *
 * If filename is empty matrix is writing on stdout.
 * @param Mat[i] the matrix
 * @param FileName[i] path to ASCII file
 */
extern void MatWriteAsc(Matrix Mat, const char FileName[]);

/**
 * @brief MatFree delete matrix
 * @param pMat[i] the matrix
 */
extern void MatFree(Matrix *pMat);


#ifdef __cplusplus
}
#endif


#endif /* !__limace_h_ */
