#include "ltp.h"
#include <iostream>
#include <assert.h>
using namespace std;




vector<LTP_DATA>  LTP::computeLTP(Image image){

    //Result
    vector<LTP_DATA> ltps;

    //Get image properties
    int heightIm=ImNbRow(image);
    int widthIm=ImNbCol(image);

    //Get pixels colors
    unsigned char** red=ImGetR(image);
    unsigned char** green=ImGetG(image);
    unsigned char** blue=ImGetB(image);


    //Create a more larger image to compute LTP on the all original image
    int heightLTP=heightIm+2;
    int widthLTP=widthIm+2;
    vector<int> data;
    //copy image
    //and extand border
    for(int y=0;y<heightLTP;y++){
        for(int x=0;x<widthLTP;x++){
            int u=min(max(x-1,0),widthIm-1);
            int v=min(max(y-1,0),heightIm-1);
            int gray=0.2126*red[v][u] + 0.7152*green[v][u] + 0.0722*blue[v][u];
            data.push_back(gray);
        }
    }

    for(int y=1;y<heightLTP-1;y++){
        for(int x=1;x<widthLTP-1;x++){
            int p1=data[x+y*widthLTP];
            //positiv ltp
            int ltpP=0;
            //negativ ltp
            int ltpN=0;

            vector<int> signs;
            signs.assign(8,1);
            //north
            int s=sign(p1,data[x+(y-1)*widthLTP]);
            if(s==0){
                signs.push_back(0);
            }else if(s==1){
                ltpP+=1;
            }else{
                ltpN+=1;
            }
            //north-east
            s=sign(p1,data[(x+1)+(y-1)*widthLTP]);
            if(s==0){
                signs.push_back(0);
            }else if(s==1){
                ltpP+=2;
            }else{
                ltpP+=0;
            }
            //east
            s=sign(p1,data[(x+1)+y*widthLTP]);
            if(s==0){
                signs.push_back(0);
            }else if(s==1){
                ltpP+=4;
            }else{
                ltpN+=4;
            }
            //south-east
            s=sign(p1,data[(x+1)+(y+1)*widthLTP]);
            if(s==0){
                signs.push_back(0);
            }else if(s==1){
                ltpP+=8;
            }else{
                ltpN+=8;
            }
            //south
            s=sign(p1,data[x+(y+1)*widthLTP]);
            if(s==0){
                signs.push_back(0);
            }else if(s==1){
                ltpP+=16;
            }else{
                ltpN+=16;
            }
            //south-west
            s=sign(p1,data[(x-1)+(y+1)*widthLTP]);
            if(s==0){
                signs.push_back(0);
            }else if(s==1){
                ltpP+=32;
            }else{
                ltpN+=32;
            }
            //west
            s=sign(p1,data[(x-1)+y*widthLTP]);
            if(s==0){
                signs.push_back(0);
            }else if(s==1){
                ltpP+=64;
            }else{
                ltpN+=64;
            }
            //north-west
            s=sign(p1,data[(x-1)+(y-1)*widthLTP]);
            if(s==0){
                signs.push_back(0);
            }else if(s==1){
                ltpP+=128;
            }else{
                ltpN+=128;
            }


            //count number of consecutive similar pixels
            int nbSim=0;
            for(int i=0;i<signs.size();i++){
                if(signs[i]==0){
                    //increment number of consecutive similar pixels
                    nbSim++;
                }else{
                    //reset number of consecutive similar pixels;
                    nbSim=0;
                }
            }
            if(nbSim<signs.size() && nbSim>0){
                int i=0;
                while(signs[i]==0){
                    nbSim++;
                    i++;
                }
            }

            LTP_DATA res;
            res.ltpN=ltpN;
            res.ltpP=ltpP;
            res.homogeneous=nbSim>=thresholdHomogeneous;
            ltps.push_back(res);
        }
    }

    return ltps;
}
