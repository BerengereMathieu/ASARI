
#include "asari.h"
#include "limace.h"
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{

    if(argc !=3 ){
        cerr << "Wrong parameters number" <<endl;
        cerr << argv[0] << ": imagePath resPath"<<endl;
        return -1;
    }

    //load image
    Image image=ImRead(argv[1]);
    if(ImType(image)!=Col0r){
        cerr << "Give a color image" << endl;
        ImFree(&image);
        return -1;
    }

    //oversegment
    Parameters param;
    Asari asari(param,image);
    asari.compute();
    //display number of superpixels
    cout << asari.getNbSp() << " superpixels" << endl;
    //get an image with boundaries of superpixels drawn in white
    Image res=asari.getResult();//memory allocated for the result image is cleaned by asari algorithm

    //save result
    ImWrite(res,argv[2]);

    // free memory
    ImFree(&image);

    return 0;
}
