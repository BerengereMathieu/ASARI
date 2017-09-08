#include "limace.h"

/**
 * @file limace.c
 * @brief to read and write image and matrix
 * Copyright (C) 1999-2011 A. Crouzil  LIMaCE release 1.6 (07/06/2012)
 * Debugging : N. Begue (15/05/2001), G. Jaffre (08/05/2002),
 *             B. Pailhes (02/04/2003), F. Courteille (09/04/2003),
 *             J.-D. Durou (18/08/2005, 08/09/2005), G. Gales (02/03/2011).
 * @author A. Crouzil
 * @version 1.6
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>



/**
 * @brief Verbose Error messages switch on stderr
 *
 * Each call of the function switch from a state to an other
 * - ON : error message are printing on stdout (default state)
 * - OFF : no error message printing
 * @return the new state (ON or OFF)
 */
int Verbose(void)
{
    static int Etat=ON;

    Etat=!Etat;
    return Etat;
}



/**
 * @brief LimError print error messages
 * @param Function[i]
 * @param Format[i]
 */
static void LimError(const char *Function, const char *Format, ...)
{
    va_list Params;
    (void)Verbose(); if (!Verbose()) return;
    va_start(Params,Format);
    if (Function != NULL)
        fprintf(stderr, "[LIMaCE] (%s) : ",Function);
    vfprintf(stderr,Format,Params);
    fprintf(stderr, ".\n");
    va_end(Params);
    return;
}


/**************************************************************************/

/* Matrix handling */

/**
 * @union Data limace.c
 * @brief matrix element (double or integer)
 */
typedef union
{
    int **AccesInt;
    double **AccesDouble;
} Data;

/**
 * @struct sMatrix limace.c
 * @brief a matrix
 */
struct sMatrix
{
    MatrixType TypeMatrix;
    int NbRow;
    int NbCol;
    Data Info;
};

/**
 * @brief MatAllocUC create an matrix of unsigned char elements
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of columns
 * @param Init[i] to know if matrix must be initialize with 0
 * @return the matrix or NULL if something wrong
 */
static unsigned char **MatAllocUC(int NbRow, int NbCol, int Init)
{
    unsigned char **Matrice,**pM,**Fin,*pD,*Data;

    if (Init)
        Data=(unsigned char *)calloc(NbRow*NbCol,sizeof(unsigned char));
    else
        Data=(unsigned char *)malloc(NbRow*NbCol*sizeof(unsigned char));
    if (Data==NULL)
        return NULL;
    if ((Matrice=(unsigned char **)malloc(NbRow*sizeof(unsigned char*)))==NULL)
    {
        free(Data);
        return NULL;
    }
    Fin=Matrice+NbRow;
    for (pM=Matrice,pD=Data;pM<Fin;pD+=NbCol) *pM++=pD;
    return(Matrice);
}


/**
 * @brief MatFreeUC delete a matrix of unsigned char elements
 * @param pMatrice[i] the matrix
 */
static void MatFreeUC(unsigned char ***pMatrice)
{
    if (*pMatrice != NULL)
    {
        free(*(*pMatrice));
        free(*pMatrice);
        *pMatrice=NULL;
    }
}


/**
 * @brief fMatReadUC read in a binary file a matrix of unsigned char elements
 * @param Fid[i] file identifier
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of columns
 * @return the matrix or NULL if something wrong
 */
static unsigned char **fMatReadUC(FILE *Fid, int NbRow, int NbCol)
{
    unsigned char **I;

    if ((I=MatAllocUC(NbRow,NbCol,0))==NULL) return NULL;
    if (fread(*I,sizeof(unsigned char),NbRow*NbCol,Fid)!=((size_t)NbRow*NbCol))
    {
        MatFreeUC(&I);
        return NULL;
    }
    return I;
}


/**
 * @brief fMatWriteUC write a matrix of unsigned char elments in a binary file
 * @param Fid[i] file descriptor
 * @param I[i] data which must be written
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of columns
 * @return
 */
static int fMatWriteUC(FILE *Fid, unsigned char **I, int NbRow, int NbCol)
{
    return fwrite(*I,sizeof(unsigned char),NbRow*NbCol,Fid);
}


/**
 * @brief MatCopyUC copy a matrix of unsigned char elements
 * @param Source[i] data which must be copied
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of columns
 * @return a matrix of unsigned char elements or NULL if something wrong
 */
static unsigned char **MatCopyUC(unsigned char **Source, int NbRow, int NbCol)
{
    unsigned char **Dest;

    if ((Dest=MatAllocUC(NbRow,NbCol,0))==NULL) return NULL;
    memcpy(*Dest,*Source,NbRow*NbCol*sizeof(unsigned char));
    return Dest;
}


/**
 * @brief MatAllocInt create an integers matrix
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of columns
 * @param Init[i] to know if matrix must be initialize to 0
 * @return the matrix or NULL if something wrong
 */
static int **MatAllocInt(int NbRow, int NbCol, int Init)
{
    int **Matrice,**pM,**Fin,*pD,*Data;

    if (Init)
        Data=(int *)calloc(NbRow*NbCol,sizeof(int));
    else
        Data=(int *)malloc(NbRow*NbCol*sizeof(int));
    if (Data==NULL)
        return NULL;
    if ((Matrice=(int **)malloc(NbRow*sizeof(int*)))==NULL)
    {
        free(Data);
        return NULL;
    }
    Fin=Matrice+NbRow;
    for (pM=Matrice,pD=Data;pM<Fin;pD+=NbCol) *pM++=pD;
    return(Matrice);
}

/**
 * @brief MatFreeInt delete an integers matrix
 * @param pMatrice[i] the matrix
 */
static void MatFreeInt(int ***pMatrice)
{
    if (*pMatrice != NULL)
    {
        free(*(*pMatrice));
        free(*pMatrice);
        *pMatrice=NULL;
    }
}


/**
 * @brief MatAllocDouble create a double matrix
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of columns
 * @param Init[i] to know if matrix must be initialize to 0
 * @return a doubles matrix or NULL if something wrong
 */
static double **MatAllocDouble(int NbRow, int NbCol, int Init)
{
    double **Matrice,**pM,**Fin,*pD,*Data,*FinData;

    Data=(double *)malloc(NbRow*NbCol*sizeof(double));
    if (Data==NULL)
        return NULL;
    if ((Matrice=(double **)malloc(NbRow*sizeof(double*)))==NULL)
    {
        free(Data);
        return NULL;
    }
    Fin=Matrice+NbRow;
    for (pM=Matrice,pD=Data;pM<Fin;pD+=NbCol) *pM++=pD;
    FinData=Data+NbRow*NbCol;
    if (Init) for (pD=Data;pD<FinData;*pD++=0.0) ;
    return(Matrice);
}

/**
 * @brief MatFreeDouble delete double matrix
 * @param pMatrice[i] the matrix
 */
static void MatFreeDouble(double ***pMatrice)
{
    if (*pMatrice!=NULL)
    {
        free(*(*pMatrice));
        free(*pMatrice);
        *pMatrice=NULL;
    }
}

/**
 * @brief MatAllocAux create a doubles or an integers matrix
 * @param Type[i] matrix type
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of columns
 * @param Init[i] to know if matrix must be initialize to 0
 * @return
 */
static Matrix MatAllocAux(MatrixType Type, int NbRow, int NbCol, int Init)
{
    Matrix M;

    if ((Type!=Int)&&(Type!=Double))
    {
        LimError("MatAlloc","unknown matrix type");
        return NULL;
    }
    M=(Matrix)malloc(sizeof(struct sMatrix));
    if (M==NULL)
    {
        LimError("MatAlloc","not enough memory");
        return NULL;
    }
    M->TypeMatrix=Type;
    M->NbRow=NbRow;
    M->NbCol=NbCol;
    if (Type==Int)
    {
        M->Info.AccesInt=MatAllocInt(NbRow,NbCol,Init);
        if (M->Info.AccesInt==NULL)
        {
            LimError("MatAlloc","not enough memory");
            free(M);
            return NULL;
        }
    }
    else
    {
        M->Info.AccesDouble=MatAllocDouble(NbRow,NbCol,Init);
        if (M->Info.AccesDouble==NULL)
        {
            LimError("MatAlloc","not enough memory");
            free(M);
            return NULL;
        }
    }
    return(M);
}


/**
 * @brief MatAlloc create a matrix
 * @param Type[i] matrix type
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of colums
 * @return the matrix or NULL if something wrong
 */
Matrix MatAlloc(MatrixType Type, int NbRow, int NbCol)
{
    return MatAllocAux(Type,NbRow,NbCol,0);
}


/**
 * @brief MatCAlloc create a matrix initialize to 0
 * @param Type[i] matrix type
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of colums
 * @return the matrix or NULL if something wrong.
 */
Matrix MatCAlloc(MatrixType Type, int NbRow, int NbCol)
{
    return MatAllocAux(Type,NbRow,NbCol,1);
}


/**
 * @brief MatType getter for matrix type
 * @param Mat[i] the matrix
 * @return matrix type
 */
MatrixType MatType(Matrix Mat)
{
    if (Mat==NULL)
    {
        LimError("MatType","NULL pointer");
        return Int;
    }
    return Mat->TypeMatrix;
}


/**
 * @brief MatNbRow getter for the number of rows
 * @param Mat[i] the matrix
 * @return number of rows or 0 if something wrong
 */
int MatNbRow(Matrix Mat)
{
    if (Mat==NULL)
    {
        LimError("MatNbRow","NULL pointer");
        return 0;
    }
    return Mat->NbRow;
}


/**
 * @brief MatNbCol getter for the number of columns
 * @param Mat[i] the matrix
 * @return number of columns or 0 if something wrong
 */
int MatNbCol(Matrix Mat)
{
    if (Mat==NULL)
    {
        LimError("MatNbCol","NULL pointer");
        return 0;
    }
    return Mat->NbCol;
}


/**
 * @brief MatGetInt return access to the elements of a matrix of integers
 * @param Mat[i] the matrix
 * @return access to matrix elements or NULL if something wrong
 */
int **MatGetInt(Matrix Mat)
{
    if (MatType(Mat)!=Int)
    {
        LimError("MatGetInt","matrix of int required");
        return NULL;
    }
    else return Mat->Info.AccesInt;
}

/**
 * @brief MatGetDouble return access to elements of a matrix of double
 * @param Mat[i] the matrix
 * @return access to matrix elements or NULL if something wrong
 */
double **MatGetDouble(Matrix Mat)
{
    if (MatType(Mat)!=Double)
    {
        LimError("MatGetDouble","matrix of double required");
        return NULL;
    }
    else return Mat->Info.AccesDouble;
}


/**
 * @brief MatCopyInt copy an integers matrix
 * @param Source[i] data which must be copied
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of columns
 * @return[i] copied data or NULL if something wrong
 */
static int **MatCopyInt(int **Source, int NbRow, int NbCol)
{
    int **Dest;

    if ((Dest=MatAllocInt(NbRow,NbCol,0))==NULL) return NULL;
    memcpy(*Dest,*Source,NbRow*NbCol*sizeof(int));
    return Dest;
}



/**
 * @brief MatInt2Mat create an integers matrix thanks to an access to an other matrix
 * @param aM[i] access to other matrix element
 * @param Type[i] matrix type
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of columns
 * @return a matrix or NULL if something wrong
 */
static Matrix MatInt2Mat(int **aM, MatrixType Type, int NbRow, int NbCol)
{
    Matrix Mat;

    Mat=(Matrix)malloc(sizeof(struct sMatrix));
    if (Mat==NULL) return NULL;
    Mat->TypeMatrix=Type;
    Mat->NbRow=NbRow;
    Mat->NbCol=NbCol;
    Mat->Info.AccesInt=MatCopyInt(aM,NbRow,NbCol);
    if (Mat->Info.AccesInt==NULL)
    {
        free(Mat);
        return NULL;
    }
    return(Mat);
}


/**
 * @brief MatCopyDouble copy an doubles matrix
 * @param Source[i] data which must be copied
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of columns
 * @return[i] copied data or NULL if something wrong
 */
static double **MatCopyDouble(double **Source, int NbRow, int NbCol)
{
    double **Dest;
    if ((Dest=MatAllocDouble(NbRow,NbCol,0))==NULL) return NULL;
    memcpy(*Dest,*Source,NbRow*NbCol*sizeof(double));
    return Dest;
}

/**
 * @brief MatDouble2Mat create an doubles matrix thanks to an access to an other matrix
 * @param aM[i] access to other matrix element
 * @param Type[i] matrix type
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of columns
 * @return a matrix or NULL if something wrong
 */
static Matrix MatDouble2Mat(double **aM, MatrixType Type, int NbRow, int NbCol)
{
    Matrix Mat;

    Mat=(Matrix)malloc(sizeof(struct sMatrix));
    if (Mat==NULL) return NULL;
    Mat->TypeMatrix=Type;
    Mat->NbRow=NbRow;
    Mat->NbCol=NbCol;
    Mat->Info.AccesDouble=MatCopyDouble(aM,NbRow,NbCol);
    if (Mat->Info.AccesDouble==NULL)
    {
        free(Mat);
        return NULL;
    }
    return(Mat);
}


/**
 * @brief MatCopy copy an matrix
 * @param Mat[i] matric which must be copied
 * @return a copy of the matrix or NULL if something wrong
 */
Matrix MatCopy(Matrix Mat)
{
    Matrix Dest;

    if (Mat==NULL)
    {
        LimError("MatCopy","NULL pointer");
        return NULL;
    }
    if ((Mat->TypeMatrix!=Int) && (Mat->TypeMatrix!=Double))
    {
        LimError("MatCopy","unknown matrix type");
        return NULL;
    }
    if (Mat->TypeMatrix==Int)
        Dest=MatInt2Mat(MatGetInt(Mat),Mat->TypeMatrix,Mat->NbRow,Mat->NbCol);
    else
        Dest=MatDouble2Mat(MatGetDouble(Mat),Mat->TypeMatrix,Mat->NbRow,Mat->NbCol);
    if (Dest==NULL) LimError("MatCopy","not enough memory");
    return Dest;
}

/**
 * @brief LireLigne read a string in a file
 *
 * Separators at the beginning of the string are ignored.
 * Reading stop when'\n' occurs or at the end of the file.
 * '\n' is no stored in the string.
 * '\0' is added to the end of the string.
 * Memory allocation is automaticaly done.
 * Memory must be cleaned up.
 * @param Flux[i] file descriptor
 * @return the first character address or NULL if something wrong
 */
static char *LireLigne(FILE *Flux)
{
    char *Ligne,*NouvelleLigne;
    int Car;
    unsigned long int i=0,Taille=BUFSIZ;

    Ligne=(char *)malloc(BUFSIZ);
    if (Ligne==NULL) return NULL;
    while ((isspace(Car=fgetc(Flux))) && (Car!=EOF)) ;
    while ((Car!='\n') && (Car!=EOF))
    {
        if (i==Taille)
        {
            Taille+=BUFSIZ;
            NouvelleLigne=(char *)realloc(Ligne,Taille);
            if (NouvelleLigne==NULL)
            {
                free(Ligne);
                return NULL;
            }
            Ligne=NouvelleLigne;
        }
        Ligne[i++]=Car;
        Car=fgetc(Flux);
    }
    Ligne=(char *)realloc(Ligne,i+1);
    Ligne[i]='\0';
    return Ligne;
}


/**
 * @brief MatReadAscInt read an integers matrix in an ASCII file
 *
 * If filename is empty, matrix is read on stdin.
 * @param Fid[i] file
 * @param FileName[i] file path
 * @return the matrix or NULL if something wrong
 */
static Matrix MatReadAscInt(FILE *Fid, const char FileName[])
{
    int NbRow,NbCol=0,NbColCour,First=1,Dim,*pMat,x;
    Matrix Mat;
    char *Buffer,*p;

    NbRow=0;
    while (strcmp(Buffer=LireLigne(Fid),""))
    {
        NbRow++;
        p=Buffer;
        NbColCour=0;
        p=strtok(Buffer," \t\n\r,;:");
        while (p)
        {
            NbColCour++;
            sscanf(p,"%d",&x);
            p=strtok(NULL," \t\n\r,;:");
        }
        if (NbColCour)
        {
            if (First) { NbCol=NbColCour; First=0; }
            else
            {
                if (NbColCour!=NbCol)
                {
                    LimError("MatReadAsc for Int","%s, line %d : wrong number of elements",FileName,NbRow);
                    return NULL;
                }
            }
        }
        else NbRow--;
        free(Buffer);
    }
    rewind(Fid);
    Mat=MatAlloc(Int,NbRow,NbCol);
    if (Mat==NULL)
    {
        LimError("MatReadAsc for Int","not enough memory");
        return NULL;
    }
    pMat=*(MatGetInt(Mat));
    Dim=NbRow*NbCol;
    while (Dim--) fscanf(Fid,"%d%*[,;:]",pMat++);
    return Mat;
}


/**
 * @brief MatReadAscInt read an doubles matrix in an ASCII file
 *
 * If filename is empty, matrix is read on stdin.
 * @param Fid[i] file
 * @param FileName[i] file path
 * @return the matrix or NULL if something wrong
 */
static Matrix MatReadAscDouble(FILE *Fid, const char FileName[])
{
    int NbRow,NbCol=0,NbColCour,First=1,Dim;
    char *Buffer,*p;
    Matrix Mat;
    double *pMat,x;

    NbRow=0;
    while (strcmp(Buffer=LireLigne(Fid),""))
    {
        NbRow++;
        p=Buffer;
        NbColCour=0;
        p=strtok(Buffer," \t\n\r,;:");
        while (p)
        {
            NbColCour++;
            sscanf(p,"%lf",&x);
            p=strtok(NULL," \t\n\r,;:");
        }
        if (NbColCour)
        {
            if (First) { NbCol=NbColCour; First=0; }
            else
            {
                if (NbColCour!=NbCol)
                {
                    LimError("MatReadAsc for Double","%s, line %d : wrong number of elements",FileName,NbRow);
                    return NULL;
                }
            }
        }
        else NbRow--;
        free(Buffer);
    }
    rewind(Fid);
    Mat=MatAlloc(Double,NbRow,NbCol);
    if (Mat==NULL)
    {
        LimError("MatReadAsc for Double","not enough memory");
        return NULL;
    }
    pMat=*(MatGetDouble(Mat));
    Dim=NbRow*NbCol;
    while (Dim--) fscanf(Fid,"%lf%*[,;:]",pMat++);
    return Mat;
}

/**
 * @brief fMatReadAsc read a matrix in an ASCII file.
 *
 * Matrix is an integers or doubles matrix.
 * @param Fid[i] ACSII file descriptor
 * @return  matrix or NULL if something wrong
 */
static Matrix fMatReadAsc(FILE *Fid)
{
    char *Buffer,Nombre[BUFSIZ+1],Format[13];
    int NbLig,NbCol,Dim,*pMatI;
    double *pMatD;
    Matrix Mat;

    if ((Buffer=LireLigne(Fid))==NULL)
    {
        LimError("MatReadAsc","error while reading");
        return NULL;
    }
    if ((Buffer[0]!='M')||(Buffer[1]!='a')||(Buffer[2]!='t'))
    {
        LimError("MatReadAsc","unknown matrix format, Matrix expected");
        free(Buffer);
        return NULL;
    }
    free(Buffer);
    if (fscanf(Fid,"%d",&NbLig)!=1)
    {
        LimError("MatReadAsc","error while reading matrix size");
        return NULL;
    }
    if (fscanf(Fid,"%d",&NbCol)!=1)
    {
        LimError("MatReadAsc","error while reading matrix size");
        return NULL;
    }
    Dim=NbLig*NbCol;
    sprintf(Format,"%%%ds",BUFSIZ);
    if (fscanf(Fid,Format,Nombre)!=1)
    {
        LimError("MatReadAsc","error while reading matrix elements");
        return NULL;
    }
    if ((strchr(Nombre,'.')==NULL)&&
            (strchr(Nombre,'e')==NULL)&&
            (strchr(Nombre,'E')==NULL))
    {
        if ((Mat=MatAlloc(Int,NbLig,NbCol))==NULL)
        {
            LimError("MatReadAsc","not enough memory");
            return NULL;
        }
        pMatI=*(MatGetInt(Mat));
        *pMatI=(int)strtol(Nombre,NULL,0); pMatI++; Dim--;
        while (Dim--)
            if (fscanf(Fid,"%d",pMatI++)!=1)
            {
                LimError("MatReadAsc","error while reading matrix elements");
                MatFree(&Mat);
                return NULL;
            }
    }
    else
    {
        if ((Mat=MatAlloc(Double,NbLig,NbCol))==NULL)
        {
            LimError("MatReadAsc","not enough memory");
            return NULL;
        }
        pMatD=*(MatGetDouble(Mat));
        *pMatD=strtod(Nombre,NULL); pMatD++; Dim--;
        while (Dim--)
            if (fscanf(Fid,"%lf",pMatD++)!=1)
            {
                LimError("MatReadAsc","error while reading matrix elements");
                MatFree(&Mat);
                return NULL;
            }
    }
    return Mat;
}

/**
 * @brief dMatReadAsc read a matrix in an ASCII file
 *
 * Matrix is an integers or doubles matrix.
 * @param FileName[i] file path
 * @return the matrix or NULL if something wrong
 */
static Matrix dMatReadAsc(const char FileName[])
{
    FILE *Fid;
    Matrix Mat;
    char *Buffer,*p,Nombre[BUFSIZ+1],Format[13];

    if ((Fid=fopen(FileName,"rt"))==NULL)
    {
        LimError("MatReadAsc","unable to open file %s",FileName);
        return NULL;
    }
    if ((Buffer=LireLigne(Fid))==NULL)
    {
        LimError("MatReadAsc","error while reading file %s",FileName);
        fclose(Fid);
        return NULL;
    }
    rewind(Fid);
    if((Buffer[0]=='M')&&(Buffer[1]=='a')&&(Buffer[2]=='t'))
    {
        Mat=fMatReadAsc(Fid);
        free(Buffer);
        fclose(Fid);
        return Mat;
    }
    p=strtok(Buffer," \t\n\r,;:");
    sprintf(Format,"%%%ds",BUFSIZ);
    if (sscanf(p,Format,Nombre)!=1)
    {
        LimError("MatReadAsc","error while reading file %s",FileName);
        free(Buffer);
        fclose(Fid);
        return NULL;
    }
    free(Buffer);
    rewind(Fid);
    if ((strchr(Nombre,'.')==NULL)&&
            (strchr(Nombre,'e')==NULL)&&
            (strchr(Nombre,'E')==NULL))
        Mat=MatReadAscInt(Fid,FileName);
    else
        Mat=MatReadAscDouble(Fid,FileName);
    fclose(Fid);
    return Mat;
}




/**
 * @brief MatReadAsc read a matrix (integer or double) in an ASCII file
 *
 * if filename is empty matrix is reading on stding
 * @param FileName[i] path to ASCII file
 * @return the matrix or NULL if something wrong
 */
Matrix MatReadAsc(const char FileName[])
{
    if (FileName[0]=='\0')
        return fMatReadAsc(stdin);
    else
        return dMatReadAsc(FileName);
}



/**
 * @brief MatFree delete matrix
 * @param pMat[i] matrix
 */
void MatFree(Matrix *pMat)
{
    MatrixType Type;
    Matrix M=*pMat;
    int **MI;
    double **MD;

    Type=MatType(*pMat);
    if (M != NULL)
    {
        if (Type==Int)
        {
            MI=MatGetInt(*pMat);
            MatFreeInt(&MI);
        }
        else
        {
            if (Type==Double)
            {
                MD=MatGetDouble(*pMat);
                MatFreeDouble(&MD);
            }
            else
            {
                LimError("Matfree","unknown matrix type");
                return;
            }
        }
    }
    free(M);
    *pMat=NULL;
    return;
}

/**
 * @brief MatWriteAscInt write a integers matrix in a file
 *
 * if filename is empty, matrix is writing on stdout
 * @param Mat[i] the matrix
 * @param FileName[i] file path
 */
static void MatWriteAscInt(Matrix Mat, const char FileName[])
{
    FILE *Fid;
    int i,j,NbLig,NbCol;
    int **MM;

    if (Mat==NULL)
    {
        LimError("MatWriteAsc for Int","NULL pointer");
        return;
    }
    if (FileName[0]!='\0')
    {
        Fid=fopen(FileName,"wt");
        if (Fid==NULL)
        {
            LimError("MatWriteAsc for Int","unable to open %s",FileName);
            return;
        }
    }
    else Fid=stdout;

    if ((MM=MatGetInt(Mat))==NULL)
    {
        LimError("MatWriteAsc for Int","memory access problem");
        return;
    }
    NbLig=MatNbRow(Mat);
    NbCol= MatNbCol(Mat);
    fprintf(Fid,"Matrix\n%d %d\n",NbLig,NbCol);
    for (i=0;i<NbLig;i++)
    {
        fprintf(Fid,"%d",MM[i][0]);
        for (j=1;j<NbCol;j++)
            fprintf(Fid," %d",MM[i][j]);
        fprintf(Fid,"\n");
    }
    if (FileName[0]!='\0') fclose(Fid);
    return;
}

/**
 * @brief MatWriteAscDouble write a double matrix in a file
 *
 * if filename is empty matrix is writing on stdout
 * @param Mat[i] the matrix
 * @param FileName[i] file path
 */
static void MatWriteAscDouble(Matrix Mat, const char FileName[])
{
    FILE *Fid;
    int i,j,NbLig,NbCol;
    double **MM;

    if (Mat==NULL)
    {
        LimError("MatWriteAsc for Double","NULL pointer");
        return;
    }
    if (FileName[0]!='\0')
    {
        Fid=fopen(FileName,"wt");
        if (Fid==NULL)
        {
            LimError("MatWriteAsc for Double","unable to open %s",FileName);
            return;
        }
    }
    else Fid=stdout;

    if ((MM=MatGetDouble(Mat))==NULL)
    {
        LimError("MatWriteAsc for Double","memory access problem");
        return;
    }
    NbLig=MatNbRow(Mat);
    NbCol= MatNbCol(Mat);
    fprintf(Fid,"Matrix\n%d %d\n",NbLig,NbCol);
    for (i=0;i<NbLig;i++)
    {
        fprintf(Fid,"% .16e",MM[i][0]);
        for (j=1;j<NbCol;j++)
            fprintf(Fid," % .16e",MM[i][j]);
        fprintf(Fid,"\n");
    }
    if (FileName[0]!='\0') fclose(Fid);
    return;
}



/**
 * @brief MatWriteAsc write a matrix (integer or double) in an ASCII file
 *
 * if filename is empty matrix is writing on stdout
 * @param Mat[i] the matrix
 * @param FileName[i] path to ASCII file
 */
void MatWriteAsc(Matrix Mat, const char FileName[])
{
    MatrixType Type=MatType(Mat);

    if (Type==Int) MatWriteAscInt(Mat,FileName);
    else
        if (Type==Double) MatWriteAscDouble(Mat,FileName);
        else
            LimError("MatWriteAsc","unknown matrix type");
    return;
}


/**************************************************************************/

/* Image handling */
/**
 * @struct ColImage limace.c
 * @brief color image
  */
typedef struct
{
    unsigned char **R;
    unsigned char **G;
    unsigned char **B;
} ColImage;

/**
  *@union Contenu limace.c
  * @brief an image data (color, graylevel or black and white image)
  */
typedef union
{
    unsigned char **I;
    ColImage CI;
} Contenu;

/**
 * @struct sImage limace.c
 * @brief an image
 */
struct sImage
{
    ImageType TypeImage;
    int NbRow;
    int NbCol;
    Contenu Info;
};

/**
 * @brief ImAllocAux create an image
 * @param Type[i] image type
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of columns
 * @param Init[i] to know if image must be initialize to 0
 * @return an image or NULL if something wrong
 */
static Image ImAllocAux(ImageType Type, int NbRow, int NbCol, int Init)
{
    Image Im;
    const char *FctName;

    FctName=Init?"ImCAlloc":"ImAlloc";

    if ((Type!=LimaceBitMap)&&(Type!=GrayLevel)&&(Type!=Col0r))
    {
        LimError(FctName,"unknown image type");
        return NULL;
    }
    Im=(Image)malloc(sizeof(struct sImage));
    if (Im==NULL)
    {
        LimError(FctName,"not enough memory");
        return NULL;
    }
    Im->TypeImage=Type;
    Im->NbRow=NbRow;
    Im->NbCol=NbCol;
    if ((Type==GrayLevel)||(Type==LimaceBitMap))
    {
        Im->Info.I=MatAllocUC(NbRow,NbCol,Init);
        if (Im->Info.I==NULL)
        {
            LimError(FctName,"not enough memory");
            free(Im);
            return NULL;
        }
    }
    else
    {
        Im->Info.CI.R=MatAllocUC(NbRow,NbCol,Init);
        if (Im->Info.CI.R==NULL)
        {
            LimError(FctName,"not enough memory");
            free(Im);
            return NULL;
        }
        Im->Info.CI.G=MatAllocUC(NbRow,NbCol,Init);
        if (Im->Info.CI.G==NULL)
        {
            LimError(FctName,"not enough memory");
            MatFreeUC(&(Im->Info.CI.R));
            free(Im);
            return NULL;
        }
        Im->Info.CI.B=MatAllocUC(NbRow,NbCol,Init);
        if (Im->Info.CI.B==NULL)
        {
            LimError(FctName,"not enough memory");
            MatFreeUC(&(Im->Info.CI.R));
            MatFreeUC(&(Im->Info.CI.G));
            free(Im);
            return NULL;
        }
    }
    return(Im);
}


/**
 * @brief ImAlloc Create an image
 * @param Type[i] image type
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of colums
 * @return the image or NULL if something wrong.
 */
Image ImAlloc(ImageType Type, int NbRow, int NbCol)
{
    return ImAllocAux(Type,NbRow,NbCol,0);
}

/**
 * @brief ImCAlloc Create an image initialize to 0
 * @param Type[i] image type
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of colums
 * @return an image or NULL if something wrong.
 */
Image ImCAlloc(ImageType Type, int NbRow, int NbCol)
{
    return ImAllocAux(Type,NbRow,NbCol,1);
}


/**
 * @brief Mat2Image create a gray level image from a matrix
 * @param I[i] data
 * @param Type[i] image type
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of columns
 * @return an image or NULL if something wrong
 */
static Image Mat2Image(unsigned char **I,ImageType Type, int NbRow, int NbCol)
{
    Image Im;

    Im=(Image)malloc(sizeof(struct sImage));
    if (Im==NULL) return NULL;
    Im->TypeImage=Type;
    Im->NbRow=NbRow;
    Im->NbCol=NbCol;
    Im->Info.I=MatCopyUC(I,NbRow,NbCol);
    if (Im->Info.I==NULL)
    {
        free(Im);
        return NULL;
    }
    return(Im);
}



/**
 * @brief RGB2Image create a color image from 3 matrix
 * @param R[i] data for image red values
 * @param G[i] data for image green values
 * @param B[i] data for image blue values
 * @param NbRow[i] number of rows
 * @param NbCol[i] number of columns
 * @return an image or NULL if something wrong
 */
static Image RGB2Image(unsigned char **R, unsigned char **G, unsigned char **B, int NbRow, int NbCol)
{
    Image Im;

    Im=(Image)malloc(sizeof(struct sImage));
    if (Im==NULL) return NULL;
    Im->TypeImage=Col0r;
    Im->NbRow=NbRow;
    Im->NbCol=NbCol;
    Im->Info.CI.R=MatCopyUC(R,NbRow,NbCol);
    if (Im->Info.CI.R==NULL)
    {
        free(Im);
        return NULL;
    }
    Im->Info.CI.G=MatCopyUC(G,NbRow,NbCol);
    if (Im->Info.CI.G==NULL)
    {
        MatFreeUC(&(Im->Info.CI.R));
        free(Im);
        return NULL;
    }
    Im->Info.CI.B=MatCopyUC(B,NbRow,NbCol);
    if (Im->Info.CI.B==NULL)
    {
        MatFreeUC(&(Im->Info.CI.R));
        MatFreeUC(&(Im->Info.CI.G));
        free(Im);
        return NULL;
    }
    return(Im);
}


/**
 * @brief ImType getter for image type (LimaceBitMap, GrayLevel, Color)
 * @param Im[i] the image
 * @return image type
 */
ImageType ImType(Image Im)
{
    if (Im==NULL)
    {
        LimError("ImType","NULL pointer");
        return GrayLevel;
    }
    return Im->TypeImage;
}


/**
 * @brief ImNbRow getter for image number of rows
 * @param Im[i] image
 * @return number of rows
 */
int ImNbRow(Image Im)
{
    if (Im==NULL)
    {
        LimError("ImNbRow","NULL pointer");
        return 0;
    }
    return Im->NbRow;
}

/**
 * @brief ImNbCol getter for image number of columns
 * @param Im[i] image
 * @return number of columns
 */
int ImNbCol(Image Im)
{
    if (Im==NULL)
    {
        LimError("ImNbCol","NULL pointer");
        return 0;
    }
    return Im->NbCol;
}

/**
 * @brief ImGetI return access to gray scale matrix of a graylevel or a bitmap image
 * @param Im[i] graylebel or bitmap image
 * @return access to gray scale matrix or NULL if something wrong
 */
unsigned char **ImGetI(Image Im)
{
    if (Im==NULL)
    {
        LimError("ImGetI","NULL pointer");
        return NULL;
    }
    if ((Im->TypeImage!=GrayLevel)&&(Im->TypeImage!=LimaceBitMap))
    {
        LimError("ImGetI","wrong image type");
        return NULL;
    }
    return Im->Info.I;
}


/**
 * @brief ImGetR return access to red values matrix of an color image
 * @param Im[i] color image
 * @return access to red values matrix or NULL if something wrong
 */
unsigned char **ImGetR(Image Im)
{
    if (Im==NULL)
    {
        LimError("ImGetR","NULL pointer");
        return NULL;
    }
    if (Im->TypeImage!=Col0r)
    {
        LimError("ImGetR","wrong image type");
        return NULL;
    }
    return Im->Info.CI.R;
}

/**
 * @brief ImGetR return access to green values matrix of an color image
 * @param Im[i] color image
 * @return access to green values matrix or NULL if something wrong
 */
unsigned char **ImGetG(Image Im)
{
    if (Im==NULL)
    {
        LimError("ImGetG","NULL pointer");
        return NULL;
    }
    if (Im->TypeImage!=Col0r)
    {
        LimError("ImGetG","wrong image type");
        return NULL;
    }
    return Im->Info.CI.G;
}

/**
 * @brief ImGetR return access to blue values matrix of an color image
 * @param Im[i] color image
 * @return access to blue values matrix or NULL if something wrong
 */
unsigned char **ImGetB(Image Im)
{
    if (Im==NULL)
    {
        LimError("ImGetB","NULL pointer");
        return NULL;
    }
    if (Im->TypeImage!=Col0r)
    {
        LimError("ImGetB","wrong image type");
        return NULL;
    }
    return Im->Info.CI.B;
}

/**
 * @brief ImCopy copy an image
 * @param Im[i] image which must be copied
 * @return a copy of the image or NULL if something wrong
 */
Image ImCopy(Image Im)
{
    Image Dest;

    if (Im==NULL)
    {
        LimError("ImCopy","NULL pointer");
        return NULL;
    }
    if ((Im->TypeImage!=LimaceBitMap)&&
            (Im->TypeImage!=GrayLevel)&&
            (Im->TypeImage!=Col0r))
    {
        LimError("ImCopy","unknown image type");
        return NULL;
    }
    if ((Im->TypeImage==GrayLevel)||(Im->TypeImage==LimaceBitMap))
        Dest=Mat2Image(ImGetI(Im),Im->TypeImage,Im->NbRow,Im->NbCol);
    else
        Dest=RGB2Image(ImGetR(Im),ImGetG(Im),ImGetB(Im),Im->NbRow,Im->NbCol);
    if (Dest==NULL) LimError("ImCopy","not enough memory");
    return Dest;
}

/**
 * @brief ImFree delete image
 * @param pIm[i] image
 */
void ImFree(Image *pIm)
{
    Image I=*pIm;

    if (I==NULL)
    {
        LimError("ImFree","NULL pointer");
        return;
    }
    if ((I->TypeImage!=LimaceBitMap)&&
            (I->TypeImage!=GrayLevel)&&
            (I->TypeImage!=Col0r))
    {
        LimError("ImFree","unknown image type");
        return;
    }
    if ((I->TypeImage==GrayLevel)||(I->TypeImage==LimaceBitMap))
        MatFreeUC(&(I->Info.I));
    else
    {
        MatFreeUC(&(I->Info.CI.R));
        MatFreeUC(&(I->Info.CI.G));
        MatFreeUC(&(I->Info.CI.B));
    }
    free(I);
    *pIm=NULL;
    return;
}



/* to read bit of an octet */
#define BITGET(Byte,Position) (((Byte)>>(7^(Position)))&1)
#define BITPUT(Byte,Position) ((Byte)=((128>>(Position))|(Byte)))


/**
 * @brief GetC read the next character in the flux Fid
 *
 * ignore comment starting with the character '#'
 * the comment end with the line
 * @param Fid[i] file identifier
 * @param pCar[o] to store character
 * @return 0 if something wrong or if end of file has been reached, 1 otherwise
 */
static int GetC(FILE *Fid, char *pCar)
{
    int i;
    char c;

    i=getc(Fid);
    if (i==EOF) return 0;
    c=(char)i;
    if (c=='#')
    {
        do
        {
            i= getc(Fid);
            if (i==EOF) return 0;
            c=(char)i;
        } while ((c!='\n') && (c!='\r'));
    }
    *pCar=c;
    return 1;
}

/**
 * @brief GetInt read next integer
 *
 * ignore comment starting with the character '#'
 * the comment end with the line
 * @param Fid[i] file identifier
 * @param pInt[o] to store integer
 * @return  0 if something wrong or if end of file has been reached, 1 otherwise
 */
static int GetInt(FILE* Fid, int *pInt)
{
    char c;
    int i;

    do
    {
        if (GetC(Fid,&c)==0) return 0;
    } while ((c==' ') || (c=='\t') || (c=='\n') || (c=='\r'));
    if ((c<'0') || (c>'9')) return 0;
    i=0;
    do
    {
        i=i*10+c-'0';
        if (GetC(Fid,&c)==0) return 0;
    } while ((c>='0') && (c<='9'));
    *pInt=i;
    return 1;
}


/**
 * @brief fImRead read an image in a opened file
 *
 * file can be closed and opened again by the function
 * at the end file must be closed by used
 * if filename is empty, data are read on stdin
 * @param pFid[i] file descriptor
 * @param FileName[i] file path
 * @return image or NULL if something wrong
 */
static Image fImRead(FILE **pFid, const char FileName[])
{
    Image Im=NULL;
    unsigned char Byte=0,**I=NULL,**R=NULL,**G=NULL,**B=NULL;
    char Format;
    int i,j,k,MaxVal=0,Val,NbLig,NbCol;
    long Debut;
    double Coeff;

    if (GetC(*pFid,&Format)==0)
    {
        fclose(*pFid);
        LimError("ImRead","error while reading %s",FileName);
        return NULL;
    }
    if (Format!='P')
    {
        LimError("ImRead","%s not a pnm (pbm, pgm, ppm) file",FileName);
        return NULL;
    }
    if (GetC(*pFid,&Format)==0)
    {
        LimError("ImRead","error while reading %s",FileName);
        return NULL;
    }
    if ((Format<'1')||(Format>'6'))
    {
        LimError("ImRead","%s not a pnm (pbm, pgm, ppm) file",FileName);
        return NULL;
    }
    if (GetInt(*pFid,&NbCol)==0)
    {
        LimError("ImRead","error while reading %s",FileName);
        return NULL;
    }
    if (GetInt(*pFid,&NbLig)==0)
    {
        LimError("ImRead","error while reading %s",FileName);
        return NULL;
    }
    if ((Format!='1')&&(Format!='4'))
    {
        if (GetInt(*pFid,&MaxVal)==0)
        {
            LimError("ImRead","error while reading %s",FileName);
            return NULL;
        }
        if (MaxVal<0)
        {
            LimError("ImRead","bad value for maxval in %s",FileName);
            return NULL;
        }
        if (MaxVal>255)
        {
            LimError("ImRead","%s: maxval > 255 not supported",FileName);
            return NULL;
        }
    }

    switch (Format)
    {
    /* Image de niveaux de gris */
    case '2':
        Im=ImAlloc(GrayLevel,NbLig,NbCol);
        if (Im==NULL)
        {
            LimError("ImRead","error while reading %s",FileName);
            return NULL;
        }
        I=ImGetI(Im);
        for (i=0;i<NbLig;i++)
            for (j=0;j<NbCol;j++)
            {
                if (GetInt(*pFid,&Val)==0)
                {
                    LimError("ImRead","error while reading %s",FileName);
                    ImFree(&Im);
                    return NULL;
                }
                I[i][j]=(unsigned char)Val;
            }
        break;
    case '5':
        /*if (FileName[0]!='\0') 12/12/2009 */
        if (strcmp(FileName,"stdin"))
        {
            Debut=ftell(*pFid);
            fclose(*pFid);
            *pFid=fopen(FileName,"rb");
            if (*pFid==NULL)
            {
                LimError("ImRead","%s: file not found",FileName);
                return NULL;
            }
            fseek(*pFid,Debut,SEEK_SET);
        }
        I=fMatReadUC(*pFid,NbLig,NbCol);
        if (I==NULL)
        {
            LimError("ImRead","error while reading %s",FileName);
            return NULL;
        }
        if ((Im=Mat2Image(I,GrayLevel,NbLig,NbCol))==NULL)
        {
            LimError("ImRead","not enough memory");
            return NULL;
        }
        MatFreeUC(&I);
        break;

        /* Image noir et blanc */
    case '1':
        Im=ImAlloc(LimaceBitMap,NbLig,NbCol);
        if (Im==NULL)
        {
            LimError("ImRead","error while reading %s",FileName);
            return NULL;
        }
        I=ImGetI(Im);
        for (i=0;i<NbLig;i++)
            for (j=0;j<NbCol;j++)
            {
                if (GetInt(*pFid,&Val)==0)
                {
                    LimError("ImRead","error while reading %s",FileName);
                    ImFree(&Im);
                    return NULL;
                }
                if ((Val!=0) && (Val!=1))
                {
                    LimError("ImRead","%s: value differ from 0 and 1",FileName);
                    ImFree(&Im);
                    return NULL;
                }
                I[i][j]=(unsigned char)(!Val);
            }
        break;
    case '4':
        Im=ImAlloc(LimaceBitMap,NbLig,NbCol);
        if (Im==NULL)
        {
            LimError("ImRead","error while reading %s",FileName);
            return NULL;
        }
        I=ImGetI(Im);
        /*if (FileName[0]!='\0') 23/02/2010 */
        if (strcmp(FileName,"stdin"))
        {
            Debut=ftell(*pFid);
            fclose(*pFid);
            *pFid=fopen(FileName,"rb");
            if (*pFid==NULL)
            {
                LimError("ImRead","%s: file not found",FileName);
                return NULL;
            }
            fseek(*pFid,Debut,SEEK_SET);
        }
        for (i=0;i<NbLig;i++)
        {
            for (j=0,k=8;j<NbCol;j++)
            {
                if (k==8)
                {
                    if ((Val=getc(*pFid))==EOF)
                    {
                        LimError("ImRead","error while reading %s",FileName);
                        ImFree(&Im);
                        return NULL;
                    }
                    Byte=(unsigned char)Val;
                    k=0;
                }
                I[i][j]=(!(BITGET(Byte,k)));
                k++;
            }
        }
        break;

        /* Image couleur */
    case '3':
        Im=ImAlloc(Col0r,NbLig,NbCol);
        if (Im==NULL)
        {
            LimError("ImRead","error while reading %s",FileName);
            return NULL;
        }
        R=ImGetR(Im);
        G=ImGetG(Im);
        B=ImGetB(Im);
        for (i=0;i<NbLig;i++)
            for (j=0;j<NbCol;j++)
            {
                if (GetInt(*pFid,&Val)==0)
                {
                    LimError("ImRead","error while reading %s",FileName);
                    ImFree(&Im);
                    return NULL;
                }
                R[i][j]=(unsigned char)Val;
                if (GetInt(*pFid,&Val)==0)
                {
                    LimError("ImRead","error while reading %s",FileName);
                    ImFree(&Im);
                    return NULL;
                }
                G[i][j]=(unsigned char)Val;
                if (GetInt(*pFid,&Val)==0)
                {
                    LimError("ImRead","error while reading %s",FileName);
                    ImFree(&Im);
                    return NULL;
                }
                B[i][j]=(unsigned char)Val;
            }
        break;
    case '6':
        if (FileName[0]!='\0')
        {
            Debut=ftell(*pFid);
            fclose(*pFid);
            *pFid=fopen(FileName,"rb");
            if (*pFid==NULL)
            {
                LimError("ImRead","%s: file not found",FileName);
                return NULL;
            }
            fseek(*pFid,Debut,SEEK_SET);
        }
        Im=ImAlloc(Col0r,NbLig,NbCol);
        if (Im==NULL)
        {
            LimError("ImRead","error while reading %s",FileName);
            return NULL;
        }
        R=ImGetR(Im);
        G=ImGetG(Im);
        B=ImGetB(Im);
        for (i=0;i<NbLig;i++)
            for (j=0;j<NbCol;j++)
            {
                if ((Val=getc(*pFid))==EOF)
                {
                    LimError("ImRead","error while reading %s",FileName);
                    ImFree(&Im);
                    return NULL;
                }
                R[i][j]=(unsigned char)Val;
                if ((Val=getc(*pFid))==EOF)
                {
                    LimError("ImRead","error while reading %s",FileName);
                    ImFree(&Im);
                    return NULL;
                }
                G[i][j]=(unsigned char)Val;
                if ((Val=getc(*pFid))==EOF)
                {
                    LimError("ImRead","error while reading %s",FileName);
                    ImFree(&Im);
                    return NULL;
                }
                B[i][j]=(unsigned char)Val;
            }
        break;
    }

    if ((MaxVal!=255)&&(MaxVal!=0))
    {
        Coeff=255.0/(double)MaxVal;
        if (ImType(Im)==GrayLevel)
        {
            I=ImGetI(Im);
            for (i=0;i<NbLig;i++)
                for (j=0;j<NbCol;j++)
                    I[i][j]=(unsigned char)floor(Coeff*I[i][j]+0.5);
        }
        else
        {
            R=ImGetR(Im);
            for (i=0;i<NbLig;i++)
                for (j=0;j<NbCol;j++)
                {
                    R[i][j]=(unsigned char)floor(Coeff*R[i][j]+0.5);
                    G[i][j]=(unsigned char)floor(Coeff*G[i][j]+0.5);
                    B[i][j]=(unsigned char)floor(Coeff*B[i][j]+0.5);
                }
        }
    }

    return Im;
}


/**
 * @brief ImRead read a ppm, pgm or pbm image
 * @param FileName[i] image path
 * @return an image or NULL if something wrong
 */
Image ImRead(const char FileName[])
{
    Image Im;
    FILE *Fid;

    if (FileName[0]=='\0')
    {
        Fid=stdin;
        Im=fImRead(&Fid,"stdin");
    }
    else
    {
        Fid=fopen(FileName,"rb");
        if (Fid==NULL)
        {
            LimError("ImRead","%s: file not found",FileName);
            return NULL;
        }
        Im=fImRead(&Fid,FileName);
        fclose(Fid);
    }
    return Im;
}




/**
 * @brief ImWrite write a ppm, pgm or pbm image (binary format)
 *
 * if no filename is empty, image is printing on stdout
 * Warning : some troubles can occur on DOS
 * @param Im[i] image
 * @param FileName[i] image path
 */
void ImWrite(Image Im, const char FileName[])
{
    FILE *Fid;
    ImageType Type;
    int i,j,k,NbEcrits,NbLig,NbCol;
    unsigned char **I,**R,**G,**B,Byte=0;

    if (Im==NULL)
    {
        LimError("ImWrite","NULL pointer");
        return;
    }

    if (FileName[0]=='\0') { Fid=stdout;  FileName="stdout"; }
    else
    {
        Fid=fopen(FileName,"wb");
        if (Fid==NULL)
        {
            LimError("ImWrite","%s: unable to open",FileName);
            return;
        }
    }
    Type=ImType(Im);
    NbLig=ImNbRow(Im);
    NbCol=ImNbCol(Im);
    switch (Type)
    {
    case LimaceBitMap:
        NbEcrits=fprintf(Fid,"%s\n%d %d\n",
                         "P4",NbCol,NbLig);
        if (NbEcrits<=0)
        {
            LimError("ImWrite","error while writing %s",FileName);
            if (FileName[0]!='\0') fclose(Fid);
            return;
        }
        /*if (FileName[0]!='\0')
      {
      fclose(Fid);
      Fid=fopen(FileName,"ab");
      if (Fid==NULL)
      {
      LimError("ImWrite","error while writing %s",FileName);
      return;
      }
      }*/
        I=ImGetI(Im);
        for (i=0,k=0,Byte=0;i<NbLig;i++)
        {
            for (j=0;j<NbCol;j++)
            {
                if (!I[i][j]) BITPUT(Byte,k);
                k++;
                if (k==8)
                {
                    if (putc(Byte,Fid)==EOF)
                    {
                        LimError("ImWrite","error while writing %s",FileName);
                        return;
                    }
                    k=0;
                    Byte=0;
                }
            }
            if (k)
            {
                if (putc(Byte,Fid)==EOF)
                {
                    LimError("ImWrite","error while writing %s",FileName);
                    return;
                }
                k=0;
                Byte=0;
            }
        }
        break;

    case GrayLevel:
        NbEcrits=fprintf(Fid,"%s\n%d %d\n%d\n",
                         "P5",NbCol,NbLig,255);
        if (NbEcrits<=0)
        {
            LimError("ImWrite","error while writing %s",FileName);
            if (FileName[0]!='\0') fclose(Fid);
            return;
        }
        /*if (FileName[0]!='\0')
      {
      fclose(Fid);
      Fid=fopen(FileName,"ab");
      if (Fid==NULL)
      {
      LimError("ImWrite","error while writing %s",FileName);
      return;
      }
      }*/
        NbEcrits=fMatWriteUC(Fid,ImGetI(Im),NbLig,NbCol);
        if (NbEcrits!=(NbLig*NbCol))
        {
            LimError("ImWrite","error while writing %s",FileName);
            if (FileName[0]!='\0') fclose(Fid);
            return;
        }
        break;

    case Col0r:
        NbEcrits=fprintf(Fid,"%s\n%d %d\n%d\n",
                         "P6",NbCol,NbLig,255);
        if (NbEcrits<=0)
        {
            LimError("ImWrite","error while writing %s",FileName);
            if (FileName[0]!='\0') fclose(Fid);
            return;
        }
        /*if (FileName[0]!='\0')
      {
      fclose(Fid);
      Fid=fopen(FileName,"ab");
      if (Fid==NULL)
      {
      LimError("ImWrite","error while writing %s",FileName);
      return;
      }
      }*/
        R=ImGetR(Im);
        G=ImGetG(Im);
        B=ImGetB(Im);
        for (i=0;i<NbLig;i++)
            for (j=0;j<NbCol;j++)
            {
                if (putc(R[i][j],Fid)==EOF)
                {
                    LimError("ImWrite","error while writing %s",FileName);
                    if (FileName[0]!='\0') fclose(Fid);
                    return;
                }
                if (putc(G[i][j],Fid)==EOF)
                {
                    LimError("ImWrite","error while writing %s",FileName);
                    if (FileName[0]!='\0') fclose(Fid);
                    return;
                }
                if (putc(B[i][j],Fid)==EOF)
                {
                    LimError("ImWrite","error while writing %s",FileName);
                    if (FileName[0]!='\0') fclose(Fid);
                    return;
                }
            }
        break;
    }

    if (FileName[0]!='\0') fclose(Fid);
    return;
}


/**
 * @brief ImWrite write a ppm, pgm or pbm image (ASCII format)
 *
 * if no filename is empty, image is printing on stdout
 * Warning : some troubles can occur on DOS
 * @param Im[i] image
 * @param FileName[i] image path
 */
void ImWriteAsc(Image Im, const char FileName[])
{
    FILE *Fid;
    ImageType Type;
    int i,j,NbEcrits,NbLig,NbCol;
    unsigned char **I,**R,**G,**B;

    if (Im==NULL)
    {
        LimError("ImWriteAsc","NULL pointer");
        return;
    }
    if (FileName[0]=='\0') { Fid=stdout; FileName="stdout"; }
    else
    {
        Fid=fopen(FileName,"wb");
        if (Fid==NULL)
        {
            LimError("ImWriteAsc","%s: unable to open",FileName);
            return;
        }
    }
    Type=ImType(Im);
    NbLig=ImNbRow(Im);
    NbCol=ImNbCol(Im);
    switch (Type)
    {
    case LimaceBitMap:
        NbEcrits=fprintf(Fid,"%s\n%d %d\n",
                         "P1",NbCol,NbLig);
        if (NbEcrits<=0)
        {
            LimError("ImWriteAsc","error while writing %s",FileName);
            if (FileName[0]!='\0') fclose(Fid);
            return;
        }
        I=ImGetI(Im);
        for (i=0;i<NbLig;i++)
            for (j=0;j<NbCol;j++)
                if (fprintf(Fid,"%d ",!I[i][j])<=0)
                {
                    LimError("ImWriteAsc","error while writing %s",FileName);
                    if (FileName[0]!='\0') fclose(Fid);
                    return;
                }
        break;

    case GrayLevel:
        NbEcrits=fprintf(Fid,"%s\n%d %d\n%d\n",
                         "P2",NbCol,NbLig,255);
        if (NbEcrits<=0)
        {
            LimError("ImWriteAsc","error while writing %s",FileName);
            if (FileName[0]!='\0') fclose(Fid);
            return;
        }
        I=ImGetI(Im);
        for (i=0;i<NbLig;i++)
            for (j=0;j<NbCol;j++)
                if (fprintf(Fid,"%d ",I[i][j])<=0)
                {
                    LimError("ImWriteAsc","error while writing %s",FileName);
                    if (FileName[0]!='\0') fclose(Fid);
                    return;
                }
        break;

    case Col0r:
        NbEcrits=fprintf(Fid,"%s\n%d %d\n%d\n",
                         "P3",NbCol,NbLig,255);
        if (NbEcrits<=0)
        {
            LimError("ImWriteAsc","error while writing %s",FileName);
            if (FileName[0]!='\0') fclose(Fid);
            return;
        }
        R=ImGetR(Im);
        G=ImGetG(Im);
        B=ImGetB(Im);
        for (i=0;i<NbLig;i++)
            for (j=0;j<NbCol;j++)
                if (fprintf(Fid,"%d %d %d ",R[i][j],G[i][j],B[i][j])<=0)
                {
                    LimError("ImWriteAsc","error while writing %s",FileName);
                    if (FileName[0]!='\0') fclose(Fid);
                    return;
                }
        break;
    }

    if (FileName[0]!='\0') fclose(Fid);
    return;
}
