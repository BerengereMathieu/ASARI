
#ifndef LTP_H
#define LTP_H

#include "limace.h"
#include <cmath>
#include <vector>

using namespace std;

struct LTP_DATA{
    int ltpP;
    int ltpN;
    bool homogeneous;
    LTP_DATA(){
        ltpP=-1;
        ltpN=-1;
        homogeneous=false;
    }

    LTP_DATA(const LTP_DATA& other){
        this->ltpN=other.ltpN;
        this->ltpP=other.ltpP;
        this->homogeneous=other.homogeneous;
    }
};

/**
 * @brief compute LTP (Local Ternary Pattern) for a given image
 *
 *
 */
class LTP
{
private:
    int thresholdLTP;
    int thresholdHomogeneous;
public:

    /**
     * @brief LTP default constructor
     * @param[in] thresholdLTP threshold to decide if gray scale are similar
     * @param[in] thresholdHomogeneous threshold to decide if LTP is homogenous
     *
     *  A LTP is homogenous, if a majority of neighbors of the current pixel has similar gray scale
     */
    LTP(int thresholdLTP,int thresholdHomogeneous) : thresholdLTP(thresholdLTP), thresholdHomogeneous(thresholdHomogeneous){};

    /**
     * @brief computeLTP compute LTP for a given image
     * @param image
     * @return
     */
    vector<LTP_DATA>  computeLTP(Image image);




    inline int sign(int p1, int p2){
        if(abs(double(p1-p2))<thresholdLTP){
            return 0;
        }
        if(p1<p2) return -1;
        return 1;
    }
};

#endif // LTP_H
