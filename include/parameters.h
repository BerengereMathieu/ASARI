#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <set>
#include <map>
#include <assert.h>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

class Point{
    double _x;
    double _y;
public:
    Point(double x,double y):_x(x),_y(y){};
    double x(){return _x;};
    double y(){return _y;};


};

/**
 * @brief Algorithm parameters
 */
struct Parameters{

    int ltpThr=19;/*!< ltp threshold to decide if intensities are similar */
    int ltpUniThr=8;/*!< number of 0 to decide if ltp is uniform */
    double spUnTexturedThreshold=0.8;/*!< threshold to decide if a superpixels is untextured or textured*/
    double similarityThreshold=0.05;/*!< similarity criterion parameter */
    double regularityParam=4;/*! regularity criterion parameter */
    double slicSpSizeFactor=0.00015;/*!< average superpixel size for slic algorithm is slicSpSizeFactor*nbPixels */
    double minSizeFactor=60;/*! slic superpixels minimum size factor: average size computed with slicSpSizeFactor must be greather than or equals to minSizeFactor */
    double slicCompacity=10;/*!< slic compacity paramert */

    /**
     * @brief Parameters default constructor :  check if parameters are consistent
     */

    Parameters(){
        assert(ltpThr>=0);
        assert(ltpThr<256);
        assert(ltpUniThr>=0);
        assert(ltpUniThr<=8);
        assert(spUnTexturedThreshold>=0 && spUnTexturedThreshold<=1);
        assert(slicSpSizeFactor>=0 && slicSpSizeFactor<=1);
        assert(similarityThreshold>=0 && similarityThreshold<=1);


    }

    void print(){
        cout << "Parameters : " << endl;
        cout << "ltp threshold : " << ltpThr << endl;
        cout << "texture detection threshold : " << spUnTexturedThreshold << endl;
        cout << "slic superpixel size factor : " << slicSpSizeFactor << endl;
        cout << "slic compacity parameter: " << slicCompacity << endl;
        cout << "similarity threshold : " << similarityThreshold << endl;
        cout << "regularity parameter : " << regularityParam << endl;
        cout << "minimal size: " << minSizeFactor << endl;
    }

};


struct SuperpixelAsari{
public:
    double red;
    double green;
    double blue;
    std::vector<int> ltpHistN;
    std::vector<int> ltpHistP;
    vector<Point> pixelsCoordinates;
    double nbPixels;
    std::set<int> neighboors;
    double nbHomogeneous;
    bool homogeneous;
    SuperpixelAsari():red(0),green(0),blue(0),nbPixels(0),homogeneous(true){
        ltpHistN.assign(256,0);
        ltpHistP.assign(256,0);
        nbHomogeneous=0;

    }
};
#endif // PARAMETERS_H
