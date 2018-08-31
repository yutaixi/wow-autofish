#ifndef __IMGEX__
#define __IMGEX__
#include "stdafx.h"  
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>   
//#include <opencv2/features2d/features2d.hpp>   
#include <opencv2/highgui/highgui.hpp> 
#include "opencv2/nonfree/nonfree.hpp"   
using namespace cv;

class ImgEx
{
public:
	ImgEx();
	~ImgEx();
	static void saveMatAsImg(cv::Mat matrix, char * fileName);
	static void saveIplImageAsImg(IplImage *qImg, string fileName);
	static int hamiltonDistance(Mat matSrc, Mat matStd);
	static bool matchPictures(Mat  src1, Mat  src2);
	static bool matchTemplate(IplImage *img, IplImage *tpl, POINT * point, string filePath, double boundaryValue, bool displayLog, int dlgItemId);
	static BOOL  hBitmapToMat(HBITMAP& _hBmp, Mat& _mat);
	static bool iplImageToHbitmap(IplImage *pImg, HBITMAP *hBmp);
	static bool getFishingFloatFromCheckArea(POINT * fishPoint);
	static bool checkScreenFishStatus();
	static bool checkFishingFloatPoint();
	static IplImage*  hBitmap2Ipl(HBITMAP hBmp);
	static bool refreshResizedImgID();
	static IplImage * resizedIplImage(IplImage* src, double scaleX, double scaleY);
	static IplImage *   resizeIplImage(IplImage* src, int width, int height);
	static bool refreshFishStatusMatchResult(IplImage* sampleImg);

	static bool  redrawAndRefreshFishStatusPic();
private:

};
 
//int imgEx(IplImage *imgSrc, char * imgStdPath);
 

 

#endif
