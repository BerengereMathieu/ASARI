#include "asari.h"
#include "SLIC.h"
#include "ltp.h"

#include <iostream>
#include <fstream>
#include <limits>

Asari::Asari(){
    this->image=NULL;
    this->result=NULL;

}

Asari::~Asari(){
    if(this->image) ImFree(&(this->image));
    if(this->result) ImFree(&(this->result));
}

Asari::Asari(Parameters &param, Image image, bool useTexture) : param(param), useTexture(useTexture)
{
    this->image=ImCopy(image);
    this->result=ImCopy(image);
    initializeOversegmntation();
    initializeSuperpixelsFeatures();

}

void Asari::changeParam(Parameters &param){
    this->param=param;
}

Asari Asari::copy(){
    Asari res;
    res.image=ImCopy(image);
    res.result=ImCopy(result);
    res.superpixelsLabels=this->superpixelsLabels;
    res.nbSuperpixels=this->nbSuperpixels;/*!< number of superpixesl */
    res.superpixelsFeatures=this->superpixelsFeatures;
    res.param=this->param;
    res.spRefSize=this->spRefSize;

    return res;
}

void Asari::compute(){

    int nbSp=superpixelsFeatures.size();
    int i=0;

    do{
        nbSp=superpixelsFeatures.size();
        computeOverSegmentationUsingMerging();
        i++;
    }while(nbSp!=int(superpixelsFeatures.size())&& i<10 && superpixelsFeatures.size()>=500);

    int spI=0;
    int width=ImNbCol(image);
    for(map<int,SuperpixelAsari>::iterator sp=superpixelsFeatures.begin();sp!=superpixelsFeatures.end();sp++){
        //update pixels labels
        for(int i=0;i<int((sp->second).pixelsCoordinates.size());i++){
            int x =(sp->second).pixelsCoordinates[i].x();
            int y = (sp->second).pixelsCoordinates[i].y();
            int j=x+y*width;
            //change index
            superpixelsLabels[j]=spI;
        }
        spI++;
    }
}

int Asari::getNbSp(){
    return superpixelsFeatures.size();
}

void Asari::clearResult(){
    result=ImCopy(image);
}

void Asari::drawSuperpixelsBoundaries(){
    int height=ImNbRow(image);
    int width=ImNbCol(image);

    unsigned char** red=ImGetR(result);
    unsigned char** green=ImGetG(result);
    unsigned char** blue=ImGetB(result);
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            int iLabel=superpixelsLabels[x+y*width];

            int vMin=max(y-1,0);
            int vMax=min(y+1,height-1);
            int uMin=max(x-1,0);
            int uMax=min(x+1,width-1);
            bool edge=false;
            for(int v=vMin;v<=vMax;v++){
                for(int u=uMin;u<=uMax;u++){
                    if(iLabel!=superpixelsLabels[u+v*width]){
                        red[y][x]=255;
                        green[y][x]=255;
                        blue[y][x]=255;
                    }
                }
            }

        }
    }


}
Image Asari::getResult(){
    clearResult();
    drawSuperpixelsBoundaries();
    return result;
}


vector<int> Asari::getSuperpixels(){
    return superpixelsLabels;
}

void Asari::mergeUsingColor(int spIdx){
    double sp1RedMean=superpixelsFeatures[spIdx].red/superpixelsFeatures[spIdx].nbPixels;
    double sp1GreenMean=superpixelsFeatures[spIdx].green/superpixelsFeatures[spIdx].nbPixels;
    double sp1BlueMean=superpixelsFeatures[spIdx].blue/superpixelsFeatures[spIdx].nbPixels;
    int minIdx=-1;
    double minDc=256;
    for(auto idx=superpixelsFeatures[spIdx].neighboors.begin();idx!=superpixelsFeatures[spIdx].neighboors.end();idx++){
        if(superpixelsFeatures[*idx].pixelsCoordinates.size() + superpixelsFeatures[spIdx].pixelsCoordinates.size()<spRefSize){
            if(superpixelsFeatures[*idx].homogeneous){
                double sp2RedMean=superpixelsFeatures[*idx].red/superpixelsFeatures[*idx].nbPixels;
                double sp2GreenMean=superpixelsFeatures[*idx].green/superpixelsFeatures[*idx].nbPixels;
                double sp2BlueMean=superpixelsFeatures[*idx].blue/superpixelsFeatures[*idx].nbPixels;
                double dc=sqrt(pow(sp1RedMean-sp2RedMean,2)+pow(sp1GreenMean-sp2GreenMean,2)+pow(sp1BlueMean-sp2BlueMean,2));
                dc/=sqrt(pow(255,2)+pow(255,2)+pow(255,2));
                if(dc<minDc){
                    minDc=dc;
                    minIdx=*idx;
                }
            }
        }

    }
    if(minIdx>=0){
        if(minDc<param.similarityThreshold){
            mergeSuperpixels(spIdx,minIdx);
        }
    }
}


void Asari::mergeUsingTexture(int spIdx){
    double minIdx=-1;
    double minDt=numeric_limits<double>::max();

    double sp1RedMean=superpixelsFeatures[spIdx].red/superpixelsFeatures[spIdx].nbPixels;
    double sp1GreenMean=superpixelsFeatures[spIdx].green/superpixelsFeatures[spIdx].nbPixels;
    double sp1BlueMean=superpixelsFeatures[spIdx].blue/superpixelsFeatures[spIdx].nbPixels;
    for(auto idx=superpixelsFeatures[spIdx].neighboors.begin();idx!=superpixelsFeatures[spIdx].neighboors.end();idx++){
        if(superpixelsFeatures[*idx].pixelsCoordinates.size() + superpixelsFeatures[spIdx].pixelsCoordinates.size()<spRefSize){
            if(!superpixelsFeatures[*idx].homogeneous){
                //compute  texture distance (chi2 distance between LTP histograms)
                double dt=0;
                double nbNZero=0;
                for(unsigned int i=0;i<superpixelsFeatures[spIdx].ltpHistN.size();i++){
                    double h1=superpixelsFeatures[*idx].ltpHistN[i]/double(superpixelsFeatures[*idx].pixelsCoordinates.size());
                    double h2=superpixelsFeatures[spIdx].ltpHistN[i]/double(superpixelsFeatures[spIdx].pixelsCoordinates.size());
                    if(h1>0||h2>0){
                        dt+=pow(h1-h2,2)/(h1+h2);
                        nbNZero++;
                    }
                }
                for(unsigned int i=0;i<superpixelsFeatures[spIdx].ltpHistP.size();i++){
                    double h1=superpixelsFeatures[*idx].ltpHistP[i]/double(superpixelsFeatures[*idx].pixelsCoordinates.size());
                    double h2=superpixelsFeatures[spIdx].ltpHistP[i]/double(superpixelsFeatures[spIdx].pixelsCoordinates.size());
                    if(h1>0||h2>0){
                        dt+=pow(h1-h2,2)/(h1+h2);
                        nbNZero++;
                    }
                }

                dt/=double(nbNZero);

                //compute color distance (euclidian distance between average RGB color)
                double sp2RedMean=superpixelsFeatures[*idx].red/superpixelsFeatures[*idx].nbPixels;
                double sp2GreenMean=superpixelsFeatures[*idx].green/superpixelsFeatures[*idx].nbPixels;
                double sp2BlueMean=superpixelsFeatures[*idx].blue/superpixelsFeatures[*idx].nbPixels;
                double dc=sqrt(pow(sp1RedMean-sp2RedMean,2)+pow(sp1GreenMean-sp2GreenMean,2)+pow(sp1BlueMean-sp2BlueMean,2));
                dc/=sqrt(pow(255,2)+pow(255,2)+pow(255,2));

                //compute simalirarity distance
                dt+=dc;
                if(dt<param.similarityThreshold){
                    minDt=dt;
                    minIdx=*idx;
                }
            }
        }

    }
    if(minIdx>=0){
        if(minDt<param.similarityThreshold){
            mergeSuperpixels(spIdx,minIdx);
        }
    }


}

void Asari::updateSpRefSize(){
    spRefSize=0;
    for(auto it=superpixelsFeatures.begin();it!=superpixelsFeatures.end();it++){
        spRefSize+=it->second.pixelsCoordinates.size();
    }
    spRefSize/=superpixelsFeatures.size();
    spRefSize=spRefSize*param.regularityParam;
}

void Asari::computeOverSegmentationUsingMerging(){
    //inference
    map<int,SuperpixelAsari>::iterator it=superpixelsFeatures.begin();

    while(it!=superpixelsFeatures.end()){

        if(superpixelsFeatures[it->first].homogeneous){
            mergeUsingColor(it->first);
        }else{
            mergeUsingTexture(it->first);
        }
        it++;
    }

    updateSpRefSize();

}



void Asari::mergeSuperpixels(int idx1, int idx2){
    if(idx1 != idx2){
        int prevNbSp=superpixelsFeatures.size() ;

        SuperpixelAsari sp2=superpixelsFeatures[idx2];

        //update average color
        //and number of pixel
        superpixelsFeatures[idx1].red+=sp2.red;
        superpixelsFeatures[idx1].green+=sp2.green;
        superpixelsFeatures[idx1].blue+=sp2.blue;
        superpixelsFeatures[idx1].nbPixels+=sp2.nbPixels;

        //add pixel coordinates of the second superpixels
        for(int i=0;i<int(sp2.pixelsCoordinates.size());i++){
            superpixelsFeatures[idx1].pixelsCoordinates.push_back(sp2.pixelsCoordinates[i]);
        }
        if(useTexture){
            //merge ltp histograms
            for(unsigned int i=0;i<sp2.ltpHistN.size();i++){
                superpixelsFeatures[idx1].ltpHistN[i]+=sp2.ltpHistN[i];
                superpixelsFeatures[idx1].ltpHistP[i]+=sp2.ltpHistP[i];
            }
            superpixelsFeatures[idx1].nbHomogeneous+=sp2.nbHomogeneous;
            //test if superpixel is homogeneous
            superpixelsFeatures[idx1].homogeneous=(superpixelsFeatures[idx1].nbHomogeneous/double(sp2.pixelsCoordinates.size()))>=param.spUnTexturedThreshold;
        }
        assert(superpixelsFeatures[idx1].pixelsCoordinates.size()==superpixelsFeatures[idx1].nbPixels);

        //update neighboors of the second superpixels
        for(auto it=sp2.neighboors.begin();it!=sp2.neighboors.end();it++){
            if(*it!=idx1){
                superpixelsFeatures[*it].neighboors.erase(idx2);
                superpixelsFeatures[*it].neighboors.insert(idx1);
                superpixelsFeatures[idx1].neighboors.insert(*it);
            }
        }


        //remove sp2 from neighbors of sp1
        superpixelsFeatures[idx1].neighboors.erase(idx2);
        //remove sp2
        assert(superpixelsFeatures.find(idx2)!=superpixelsFeatures.end());
        superpixelsFeatures.erase(idx2);
        assert((int)superpixelsFeatures.size()<prevNbSp);
    }

}

void Asari::initializeOversegmntation(){
    int height=ImNbRow(image);
    int width=ImNbCol(image);
    int nbPixels=width*height;
    unsigned int* data=new unsigned int[nbPixels];

    unsigned char** red=ImGetR(image);
    unsigned char** green=ImGetG(image);
    unsigned char** blue=ImGetB(image);
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            unsigned int compactedColor=red[y][x];
            compactedColor=compactedColor<<8;
            compactedColor+=green[y][x];
            compactedColor=compactedColor<<8;
            compactedColor+=blue[y][x];

            data[x+y*width]=compactedColor;

        }
    }


    int* labelsSlic;

    int numSegm;

    SLIC slic;

    int spSize=max(width*height*param.slicSpSizeFactor,param.minSizeFactor);
    slic.DoSuperpixelSegmentation_ForGivenSuperpixelSize(data,width,height,labelsSlic,numSegm,spSize,param.slicCompacity);

    superpixelsLabels.assign(labelsSlic,labelsSlic+width*height);
    nbSuperpixels=numSegm;
    delete data;
    delete labelsSlic;

}


void Asari::initializeSuperpixelsFeatures(){
    superpixelsFeatures.clear();

    int height=ImNbRow(image);
    int width=ImNbCol(image);

    unsigned char** red=ImGetR(image);
    unsigned char** green=ImGetG(image);
    unsigned char** blue=ImGetB(image);


    for(int i=0;i<nbSuperpixels;i++){
        superpixelsFeatures.insert(pair<int,SuperpixelAsari>(i,SuperpixelAsari()));
    }

    if(useTexture){
        LTP ltpAlgo(param.ltpThr,param.ltpUniThr);
        ltps=ltpAlgo.computeLTP(image);
    }

    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            int iLabel = superpixelsLabels[x+y*width];
            //compute average colore
            superpixelsFeatures[iLabel].red+= red[y][x];
            superpixelsFeatures[iLabel].green+= green[y][x];
            superpixelsFeatures[iLabel].blue+= blue[y][x];
            superpixelsFeatures[iLabel].nbPixels++;
            superpixelsFeatures[iLabel].pixelsCoordinates.push_back(Point(x,y));

            if(useTexture){
                //update ltp histograms
                superpixelsFeatures[iLabel].ltpHistN[ltps[x+y*width].ltpN]++;
                superpixelsFeatures[iLabel].ltpHistP[ltps[x+y*width].ltpP]++;
                if( ltps[x+y*width].homogeneous){
                    superpixelsFeatures[iLabel].nbHomogeneous++;
                }
            }

            //neighboors
            int vMin=max(y-1,0);
            int vMax=min(y+1,height-1);
            int uMin=max(x-1,0);
            int uMax=min(x+1,width-1);
            for(int v=vMin;v<=vMax;v++){
                for(int u=uMin;u<=uMax;u++){
                    if(iLabel!=superpixelsLabels[u+v*width]) superpixelsFeatures[iLabel].neighboors.insert(superpixelsLabels[u+v*width]);
                }
            }
        }
    }

    if(useTexture){
        for(int i=0;i<nbSuperpixels;i++){
            superpixelsFeatures[i].homogeneous=(superpixelsFeatures[i].nbHomogeneous/=double(superpixelsFeatures[i].pixelsCoordinates.size()))>=param.spUnTexturedThreshold;
        }
    }


    updateSpRefSize();

}

