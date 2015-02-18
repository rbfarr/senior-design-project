#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include <sstream>

using namespace std;
using namespace cv;

int main()
{
Mat image;

image = imread("cutepuppy.jpeg",1);
if(! image.data)
{
cout << "Could not open or find the image" << std::endl ;
return -1;
}


namedWindow("Image", CV_WINDOW_AUTOSIZE );
imshow("Image", image);


   // get the image data
 int height = image.rows;
 int width = image.cols;
 int num;
 double check;


//printf("Processing a %dx%d image\n",height,width);
cout << "the height of image is " << height << " the width of image is " << width << endl;
cout << "input the number of squares you want"<< endl;
cin >> num;
check = num/sqrt(num);
while (num % 2 == 1 && floor(check)!=check ){
cout << "that is not a possible number. Please enter another one" <<endl;
cin >> num;
check = num/sqrt(num);
}

int y = 0;
int z = 1;
int current = 0;
int check2 = 0;
int widthdiv = 0;
int heightdiv = 0;
// Checks to see if it is a square root.
if (floor(check) != check)
{
while (y == 0){ // If it is not a square root, this finds the middle two divisors of the number.
    while (num%z != 0){
        z = z + 1;
    }

    if (z == current) {
        y = 1;
        widthdiv = check2;
        heightdiv = z;
    }

    current = num/z;
    check2 = z;
    z = z + 1;
}
}

else {
widthdiv = check;
heightdiv = check;
}

cout << widthdiv << endl;
cout << heightdiv << endl;

int hdiv;
int wdiv;
hdiv = height/heightdiv;
wdiv = width/widthdiv;

stringstream name;
stringstream namejpg;

//making matrix
cv :: Size smallSize(wdiv, hdiv);
std :: vector < Mat > smallImages[hdiv];
cv :: Rect rect;


for (int h = 0; h < heightdiv; h++) {
    for (int w = 0; w < widthdiv; w++) {

        //actually cutting the images
        rect =   cv :: Rect (w * wdiv, h *
        hdiv , smallSize.width , smallSize.height );
        smallImages[h].push_back ( cv :: Mat ( image , rect ));

        //showing the images
        name << "smallimages" << h << w;
        cout << name<< std::endl;
        namedWindow(name.str(), CV_WINDOW_AUTOSIZE);
        imshow(name.str(), smallImages[h][w]);

        //saving the images
        namejpg << name.str() << ".jpg";
        cout << namejpg << std::endl;
        imwrite(namejpg.str(), smallImages[h][w]);

        //clear strings before next loop
        name.str(std::string());
        namejpg.str(std::string());

    }
}
waitKey(0);
return 0;
}
