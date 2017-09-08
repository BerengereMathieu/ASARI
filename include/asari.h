#ifndef ASARI_H
#define ASARI_H
#include <vector>

#include "limace.h"

#include "pixel.h"

#include "parameters.h"
#include "ltp.h"
#include <map>

using namespace std;

class FindUnionAlgo{
private:
    vector<int> parents;
public:
    void  initialize(int N){
        parents.clear();
        for(int i=0;i<N;i++){
            parents.push_back(i);
        }
    }

    int findCC(int x){
        if(parents[x]==x) return x;
        else return findCC(parents[x]);
    }

    void unionCC(int x,int y){
        unsigned int xRoot = findCC(x);
        unsigned int yRoot = findCC(y);
        parents[xRoot] = yRoot;
    }
};

/**@@
 * @brief The SlicModified class
 */
class Asari
{
private:
    //attributs
    FindUnionAlgo fuAlgo;    
    Image image;/*!< image to over-segment */
    Image result;/*!< over-segmentation result */
    vector<int> superpixelsLabels;
    int nbSuperpixels;/*!< number of superpixesl */
    map<int,SuperpixelAsari> superpixelsFeatures;
    vector<LTP_DATA> ltps;
    Parameters param;
    double spRefSize;
    map<int,int> equivalences;
    bool useTexture;
    double meanColorDist;
    double meanTextureDist;
    double stDevColorDist;
    double stDevTextureDist;

    //methods
    /**
     * @brief computeOverSegmentationUsingMerging
     */
    void computeOverSegmentationUsingMerging();

    /**
     * @brief mergeSuperpixels
     * @param idx1 index of the main superpixel
     * @param idx2 index of the merged superpixel
     */
    void mergeSuperpixels(int idx1,int idx2);

    /**
     * @brief updateSpRefSize update superpixel reference size
     */
    void updateSpRefSize();

    /**
     * @brief mergeUsingColor color merging criterion
     * @param spIdx
     */
    void mergeUsingColor(int spIdx);

    /**
     * @brief mergeUsingTexture color+texture merging criterion
     * @param spIdx
     */
    void mergeUsingTexture(int spIdx);

    /**
     * @brief clearResult
     */
    void clearResult();
    /**
     * @brief drawSuperpixelsBoundaries
     */
    void drawSuperpixelsBoundaries();

    Asari();
public:
    Asari(Parameters& param, Image image,bool useTexture=true);
    ~Asari();

    /**
     * @brief initializeOversegmntation compute an initial over-segmentation
     * with very small superpixels using slic algorithm
     */
    void initializeOversegmntation();
    /**
     * @brief initializeSuperpixelsFeatures compute for the first time features
     * of superpixels
     */
    void initializeSuperpixelsFeatures();

    /**
     * @brief compute over-segmentation
     */
    void compute();
    /**
     * @brief changeParam set parameters
     * @param param
     */
    void changeParam(Parameters& param);
    /**
     * @brief copy copy data
     * @return
     */
    Asari copy();
    /**
     * @brief getResult
     * @return
     */
    Image getResult();

    vector<int> getSuperpixels();

    int getNbSp();
};

#endif // SLICMODIFIED_H
