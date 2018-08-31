#include "stdafx.h"
#include "imgEx.h"
#include "hdcMap.h"
#include "logger.h"  
//#include "global.h" 
#include "dlgHelper.h"
#include "resource.h"
#include "MathUtil.h"

extern SysConfig sys;
 



ImgEx::ImgEx()
{
}

ImgEx::~ImgEx()
{
}
  int ImgEx::hamiltonDistance(Mat matSrc, Mat matStd)
{

	//cv::Mat matSrc, matSrc, matSrc2;

	//IplImage *imgSrc = cvLoadImage(imgSrcPath);
	//IplImage *imgStd = cvLoadImage(imgStdPath);
	//if (imgSrc == NULL || imgStd == NULL)
	//{
		//return 40;
	//}
	if ( matSrc.empty() || matStd.empty())
	{
		return 40;
	}
	//cv::Mat matSrc(imgSrc, 0), matStd(imgStd, 0);

	//matSrc = cv::imread(imgSrc, CV_LOAD_IMAGE_COLOR); 
	//matStd= cv::imread(imgStd, CV_LOAD_IMAGE_COLOR);
	//CV_Assert(matSrc.channels() == 3);
	//CV_Assert(matStd.channels() == 3);

	//cv::resize(matSrc, matSrc1, cv::Size(357, 419), 0, 0, cv::INTER_NEAREST);
	//cv::flip(matSrc1, matSrc1, 1);
	//cv::resize(matSrc, matSrc2, cv::Size(2177, 3233), 0, 0, cv::INTER_LANCZOS4);

	cv::Mat matDst1, matDst2;
	 
	//cv::resize(matSrc, matDst1, cv::Size(16,16), 0, 0, cv::INTER_CUBIC);
	//cv::resize(matStd, matDst2, cv::Size(16,16), 0, 0, cv::INTER_CUBIC);
	//saveMatAsImg(matDst1, "D:\\client\\Records\\srcImgResize.bmp");
	//saveMatAsImg(matDst2, "D:\\client\\Records\\stdImgResize.bmp");
	matDst1 = matSrc;
	matDst2 = matStd;
	cv::cvtColor(matDst1, matDst1, CV_BGR2GRAY);
	cv::cvtColor(matDst2, matDst2, CV_BGR2GRAY);
	//saveImg(matDst1, "D:\\client\\Records\\srcImg2gray.bmp");
	//saveImg(matDst2, "D:\\client\\Records\\stdImg2gray.bmp");
	int iAvg1 = 0, iAvg2 = 0;
	int arr1[64], arr2[64];
	 
	uchar* data1 = NULL;
	uchar* data2 = NULL;
	for (int i = 0; i < 8; i++)
	{
		
		 data1 = matDst1.ptr<uchar>(i);
		 data2 = matDst2.ptr<uchar>(i);
		 
		int tmp = i * 8;

		//for (int j = 15; j >7; j--)
		for (int j = 0; j <8; j++)
		{
			int tmp1 = tmp + j;

			arr1[tmp1] = data1[j] / 4 * 4;
			arr2[tmp1] = data2[j] / 4 * 4;

			iAvg1 += arr1[tmp1];
			iAvg2 += arr2[tmp1];
		}
	}

	iAvg1 /= 64;
	iAvg2 /= 64;

	for (int i = 0; i < 64; i++)
	{
		arr1[i] = (arr1[i] >= iAvg1) ? 1 : 0;
		arr2[i] = (arr2[i] >= iAvg2) ? 1 : 0;
	}

	int iDiffNum = 0;

	for (int i = 0; i < 64; i++)
		if (arr1[i] != arr2[i])
			++iDiffNum;
	 
	//if (iDiffNum <= 5)
	//	cout << "two images are very similar!" << endl;
	//else if (iDiffNum > 10)
	//	cout << "they are two different images!" << endl;
	//else
	//	cout << "two image are somewhat similar!" << endl;
	//delete  data1;
	//delete data2;
	//cvReleaseImage(&imgSrc);
	//cvReleaseImage(&imgStd);
	//imgStd = NULL;
	data1 = NULL;
	data2 = NULL;
	return iDiffNum;
}

void ImgEx::saveMatAsImg(cv::Mat matrix, char * fileName)
{

	//IplImage qImg;
	//qImg = IplImage(matrix); // cv::Mat -> IplImage
	//cvSaveImage(fileName, &qImg);
	cvSaveImage(fileName, &(IplImage(matrix)));

}
void ImgEx::saveIplImageAsImg(IplImage *qImg, string fileName)
{

	//IplImage qImg;
	//qImg = IplImage(matrix); // cv::Mat -> IplImage
	//cvSaveImage(fileName, &qImg);
	cvSaveImage(fileName.c_str(), qImg);

}



//////////////////////////////////////////////////////////////////
//函数功能：用向量来做COSα=两向量之积/两向量模的乘积求两条线段夹角
//输入：   线段3个点坐标pt1,pt2,pt0,最后一个参数为公共点
//输出：   线段夹角，单位为角度
//////////////////////////////////////////////////////////////////
double angle(CvPoint* pt1, CvPoint* pt2, CvPoint* pt0)
{
	double dx1 = pt1->x - pt0->x;
	double dy1 = pt1->y - pt0->y;
	double dx2 = pt2->x - pt0->x;
	double dy2 = pt2->y - pt0->y;
	double angle_line = (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);//余弦值
	return acos(angle_line) * 180 / 3.141592653;
}
//////////////////////////////////////////////////////////////////
//函数功能：采用多边形检测，通过约束条件寻找矩形
//输入：   img 原图像
//          storage 存储
//          minarea，maxarea 检测矩形的最小/最大面积
//          minangle,maxangle 检测矩形边夹角范围，单位为角度
//输出：   矩形序列
//////////////////////////////////////////////////////////////////
CvSeq* findSquares4(IplImage* img, CvMemStorage* storage, int minarea, int maxarea, int minangle, int maxangle)
{
	CvSeq* contours;//边缘
	int N = 6;  //阈值分级
	CvSize sz = cvSize(img->width & -2, img->height & -2);
	IplImage* timg = cvCloneImage(img);//拷贝一次img
	IplImage* gray = cvCreateImage(sz, 8, 1); //img灰度图
	IplImage* pyr = cvCreateImage(cvSize(sz.width / 2, sz.height / 2), 8, 3);  //金字塔滤波3通道图像中间变量
	IplImage* tgray = cvCreateImage(sz, 8, 1); ;
	CvSeq* result;
	double s, t;
	CvSeq* squares = cvCreateSeq(0, sizeof(CvSeq), sizeof(CvPoint), storage);

	cvSetImageROI(timg, cvRect(0, 0, sz.width, sz.height));
	//金字塔滤波 
	cvPyrDown(timg, pyr, 7);
	cvPyrUp(pyr, timg, 7);
	//在3个通道中寻找矩形 
	for (int c = 0; c < 3; c++) //对3个通道分别进行处理 
	{
		cvSetImageCOI(timg, c + 1);
		cvCopy(timg, tgray, 0);  //依次将BGR通道送入tgray         
		for (int l = 0; l < N; l++)
		{
			//不同阈值下二值化
			cvThreshold(tgray, gray, (l + 1) * 255 / N, 255, CV_THRESH_BINARY);

			cvFindContours(gray, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
			while (contours)
			{ //多边形逼近             
				result = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0);
				//如果是凸四边形并且面积在范围内
				if (result->total == 4 && fabs(cvContourArea(result, CV_WHOLE_SEQ)) > minarea  && fabs(cvContourArea(result, CV_WHOLE_SEQ)) < maxarea &&  cvCheckContourConvexity(result))
				{
					s = 0;
					//判断每一条边
					for (int i = 0; i < 5; i++)
					{
						if (i >= 2)
						{   //角度            
							t = fabs(angle((CvPoint*)cvGetSeqElem(result, i), (CvPoint*)cvGetSeqElem(result, i - 2), (CvPoint*)cvGetSeqElem(result, i - 1)));
							s = s > t ? s : t;
						}
					}
					//这里的S为直角判定条件 单位为角度
					if (s > minangle && s < maxangle)
						for (int i = 0; i < 4; i++)
							cvSeqPush(squares, (CvPoint*)cvGetSeqElem(result, i));
				}
				contours = contours->h_next;
			}
		}
	}
	cvReleaseImage(&gray);
	cvReleaseImage(&pyr);
	cvReleaseImage(&tgray);
	cvReleaseImage(&timg);
	return squares;
}
//////////////////////////////////////////////////////////////////
//函数功能：画出所有矩形
//输入：   img 原图像
//          squares 矩形序列
//          wndname 窗口名称
//输出：   图像中标记矩形
//////////////////////////////////////////////////////////////////
void drawSquares(IplImage* img, CvSeq* squares, const char* wndname)
{
	CvSeqReader reader;
	IplImage* cpy = cvCloneImage(img);
	CvPoint pt[4];
	int i;
	cvStartReadSeq(squares, &reader, 0);
	for (i = 0; i < squares->total; i += 4)
	{
		CvPoint* rect = pt;
		int count = 4;
		memcpy(pt, reader.ptr, squares->elem_size);
		CV_NEXT_SEQ_ELEM(squares->elem_size, reader);
		memcpy(pt + 1, reader.ptr, squares->elem_size);
		CV_NEXT_SEQ_ELEM(squares->elem_size, reader);
		memcpy(pt + 2, reader.ptr, squares->elem_size);
		CV_NEXT_SEQ_ELEM(squares->elem_size, reader);
		memcpy(pt + 3, reader.ptr, squares->elem_size);
		CV_NEXT_SEQ_ELEM(squares->elem_size, reader);
		//cvPolyLine( cpy, &rect, &count, 1, 1, CV_RGB(0,255,0), 3, CV_AA, 0 );
		cvPolyLine(cpy, &rect, &count, 1, 1, CV_RGB(rand() & 255, rand() & 255, rand() & 255), 1, CV_AA, 0);//彩色绘制
	}
	cvShowImage(wndname, cpy);
	cvReleaseImage(&cpy);
}




//检测两张图片匹配的点数
//入参img_filename1，img_filename2图片文件地址
//输出int：匹配点的个数
bool ImgEx::matchPictures(Mat  src1, Mat  src2)
{
	//char img_filename1[] = "D:\\client\\fishstatus\\fishstatus3.bmp";
	//char img_filename2[] = "D:\\client\\fishstatus\\fishstatus2.bmp";
	//Mat src1, src2;
	//src1 = imread(img_filename1, 1);
	//src2 = imread(img_filename2, 1);

	// vector of keyPoints
	vector<KeyPoint> keys1(200);
	vector<KeyPoint> keys2(200);
	// construction of the fast feature detector object
	FastFeatureDetector fast1 = FastFeatureDetector(80);	// 检测的阈值为40
	FastFeatureDetector fast2 = FastFeatureDetector(80);

	// feature point detection
	fast1.detect(src1, keys1);
	fast2.detect(src2, keys2);


	SurfDescriptorExtractor Extractor;//Run:BruteForceMatcher< L2<float> > matcher
									  //ORB Extractor;//Not Run;
									  //BriefDescriptorExtractor Extractor;//RUN:BruteForceMatcher< Hamming > matcher
	Mat descriptors1, descriptors2;
	Extractor.compute(src1, keys1, descriptors1);
	Extractor.compute(src2, keys2, descriptors2);

	//BruteForceMatcher< Hamming > matcher;
	//BruteForceMatcher< L2<float> > matcher;
	FlannBasedMatcher matcher;
	vector< DMatch > matches(200);
	if (descriptors1.cols == 0 || descriptors1.rows == 0 || descriptors2.cols == 0 || descriptors2.rows == 0)
	{ 
		return 0;
	}
	matcher.match(descriptors1, descriptors2, matches);
	//Mat img_matches;
	//drawMatches(src1, keys1, src2, keys2, matches, img_matches,
	//	Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	//imshow("draw", img_matches); 
	int stdKeyNum = keys2.size();
	int sampleKeyNum = keys1.size();
	int matchesNum = matches.size();
	return abs(matchesNum - stdKeyNum) <= 8 || matchesNum >= stdKeyNum;
}

bool ImgEx::matchTemplate(IplImage *img, IplImage *tpl, POINT * point,string filePath,double boundaryValue,bool displayLog,int dlgItemId)
{
	if (!img || !tpl)
	{
		return false;
	}
	//IplImage    *img;
	//IplImage    *tpl;
	IplImage    *res=NULL;
	CvPoint     minloc, maxloc;
	double      minval, maxval;
	//int         img_width, img_height;
	//int         tpl_width, tpl_height;
	//int         res_width, res_height;
	//const char* filename1 = "D:\\client\\cursor\\sample.bmp";
	//const char* filename2 = "D:\\client\\cursor\\cursor3.bmp";
	//img = cvLoadImage(filename1, CV_LOAD_IMAGE_COLOR);
	//tpl = cvLoadImage(filename2, CV_LOAD_IMAGE_COLOR);



	//cvNamedWindow("src", CV_WINDOW_AUTOSIZE);
	//cvShowImage("src", img);
	//cvNamedWindow("template", CV_WINDOW_AUTOSIZE);
	//cvShowImage("template", tpl);

	/* get image's properties */
	//img_width = img->width;
	//img_height = img->height;
	//tpl_width = tpl->width;
	//tpl_height = tpl->height;
	//res_width = img->width - tpl->width + 1;
	//res_height = img->height - tpl->height + 1;

	/* create new image for template matching computation */
	//res = cvCreateImage(cvSize(res_width, res_height), IPL_DEPTH_32F, 1);
	res = cvCreateImage(cvSize(img->width - tpl->width + 1, img->height - tpl->height + 1), IPL_DEPTH_32F, 1);

	/* choose template matching method to be used */
	cvMatchTemplate(img, tpl, res, CV_TM_CCORR_NORMED);
	/**/
	//cvMatchTemplate( img, tpl, res, CV_TM_SQDIFF_NORMED );
	//cvMatchTemplate( img, tpl, res, CV_TM_CCORR );
	//cvMatchTemplate( img, tpl, res, CV_TM_SQDIFF );
	//cvMatchTemplate( img, tpl, res, CV_TM_CCOEFF );
	//cvMatchTemplate( img, tpl, res, CV_TM_CCOEFF_NORMED );
	//cvNamedWindow("res", CV_WINDOW_AUTOSIZE);
	//cvShowImage("res", res);

	cvMinMaxLoc(res, &minval, &maxval, &minloc, &maxloc, 0);

	/* draw red rectangle */
	/*cvRectangle( img,
	cvPoint( minloc.x, minloc.y ),
	cvPoint( minloc.x + tpl_width, minloc.y + tpl_height ),
	cvScalar( 0, 0, 255, 0 ), 1, 0, 0 );*/
	if (!filePath.empty())
	{
		CvPoint pt1;
		CvPoint pt2;
		CvRect rect;
		rect = cvRect(maxloc.x, maxloc.y, tpl->width, tpl->height);//最佳的匹配区域
		pt1 = cvPoint(rect.x, rect.y);
		pt2 = cvPoint(rect.x + rect.width, rect.y + rect.height);
		cvRectangle(img, pt1, pt2, cvScalar(0, 0, 255), 1, 8, 0);
		saveIplImageAsImg(img, filePath);
	}
	
	/* display images */
	//cvNamedWindow("reference", CV_WINDOW_AUTOSIZE);
	//cvShowImage("reference", img);
	if (point)
	{
		point->x = maxloc.x + tpl->width /2;
		point->y = maxloc.y + tpl->height /2;
	}
	
	if (dlgItemId > 0)
	{
		DlgHelper::refreshCheckScreenFishStatusValueItem(NULL, MathUtil::Round(maxval,2));
	}

	/* wait until user press a key to exit */
	//cvWaitKey(0);

	/* free memory */
	//cvDestroyWindow("reference");
	//cvDestroyWindow("template");
	//cvReleaseImage(&img);
	//cvReleaseImage(&tpl);
	cvReleaseImage(&res);
	res = NULL;
	if (displayLog)
	{ 
		Logger::displayLog("边界值%f,匹配值%f\r\n", boundaryValue,maxval);
	}
	return maxval >= boundaryValue;
	 
}
/*
bool ImgEx::checkFishingFloatPoint()
{  
	// HBITMAP   hBitMap = drawCursor(NULL);
	HBITMAP   hBitMap=NULL; 
	CURSORINFO pci;//定义光标结构体信息
	ICONINFO iconinfo;
	//获取结构体大小，这一步必须要。否则后面就无法获取句柄
	pci.cbSize = sizeof(CURSORINFO);
	int iDiffNum=40;
	Mat sampleMat;
	cv::Mat stdMat;
	try
	{
		getCurrentCursorBitmap(&pci,&iconinfo,sys.saveCursorImg? sys.RECORD_FISHING_CURSOR_FILE:NULL);
		
		ImgEx::hBitmapToMat(iconinfo.hbmColor, sampleMat);
		stdMat = cv::imread(sys.FISHING_CURSOR_IMGID_FILE);
		//sampleMat = cv::imread(sys.FISHING_CURSOR_IMGID_FILE);
		iDiffNum = ImgEx::hamiltonDistance(sampleMat, stdMat);
	}
	catch (...)
	{
		LOG(INFO) << " 异常：checkFishingFloatPoint 异常 ";
	} 
	 ::DeleteObject(hBitMap);
	 ::DeleteObject(iconinfo.hbmColor);
	 ::DeleteObject(iconinfo.hbmMask);
	 ::DeleteObject(pci.hCursor);
	 ::DeleteObject(hBitMap);  
	 hBitMap = NULL;
	 //cvReleaseImage(&sampleImg);
	 //sampleImg = NULL;
	 if (iDiffNum <= 5)
	 {
		 ImgEx::saveMatAsImg(sampleMat,"D:\\client\\Records\\cursor.bmp");
	 }
	 return iDiffNum <= 5;
}
*/

bool ImgEx::checkFishingFloatPoint()
{
	CURSORINFO pci;//定义光标结构体信息
	ICONINFO iconinfo;
	//获取结构体大小，这一步必须要。否则后面就无法获取句柄
	pci.cbSize = sizeof(CURSORINFO);
	Mat sampleMat;
	bool matches = false;
	IplImage* sampleImg = NULL;
	string templateCursorFile = string(sys.FISHING_CURSOR_IMGID_FILE_PATH).append(sys.FISHING_CURSOR_IMGID_FILE_NAME);
	IplImage* tplImg = cvLoadImage(templateCursorFile.c_str(), CV_LOAD_IMAGE_COLOR);
	getCurrentCursorBitmap(&pci, &iconinfo, sys.saveCursorImg ? sys.RECORD_FISHING_CURSOR_FILE : "");
	//ImgEx::hBitmapToMat(iconinfo.hbmColor, sampleMat);
	  sampleImg = ImgEx::hBitmap2Ipl(iconinfo.hbmColor);
		//sampleImg = new IplImage(sampleMat);
	  
	if (sampleImg != NULL && sampleImg->width >= tplImg->width && sampleImg->height >= tplImg->height)
	{
		matches = ImgEx::matchTemplate(sampleImg, tplImg, NULL,
				sys.saveMatchTemplateCursorFishingFloatImg?sys.RECORD_MATCH_TEMPLAE_CURSOR_RESULT_PATH: "",
				sys.checkCursorFishingFloatBoundaryValue,
			sys.isDisplayMatchTemplateCursorLog,0);
	}
	 
	
	::DeleteObject(iconinfo.hbmColor);
	::DeleteObject(iconinfo.hbmMask);
	::DeleteObject(pci.hCursor); 
	if (sampleImg != NULL)
	{
		cvReleaseImage(&sampleImg);
		sampleImg = NULL;
	}
	if (tplImg != NULL)
	{
		cvReleaseImage(&tplImg);
		tplImg = NULL;
	}
	
	return matches;


}

bool ImgEx::redrawAndRefreshFishStatusPic()
{
	HBITMAP hBitMap = NULL;
	drawFishStatusHdc(false, &hBitMap);
	IplImage*	sampleImg=	hBitmapToIpl(hBitMap);
	IplImage*  resizedIplImg = ImgEx::resizeIplImage(sampleImg, 150, 20);
	HBITMAP btimap;
	iplImageToHbitmap(resizedIplImg, &btimap);
	DlgHelper::refreshMatchTemplateFishStatusPic(&btimap);

	DeleteObject(hBitMap);
	cvReleaseImage(&sampleImg);
	sampleImg = NULL;
	DeleteObject(btimap);
	cvReleaseImage(&resizedIplImg);
	resizedIplImg = NULL;
	return true;

}

bool ImgEx::refreshFishStatusMatchResult(IplImage* sampleImg)
{
	IplImage*  resizedIplImg=ImgEx::resizeIplImage(sampleImg,150,20);
	HBITMAP btimap;
	iplImageToHbitmap(resizedIplImg, &btimap);
	DlgHelper::refreshMatchTemplateFishStatusPic(&btimap);
	DeleteObject(btimap);
	cvReleaseImage(&resizedIplImg);
	return true;
}

bool ImgEx::checkScreenFishStatus()
{
	IplImage* sampleImg=NULL; 
	HBITMAP hBitMap=NULL; 
	//hBitMap= drawFishHdc();
	bool matches = false;
	IplImage* sampleImgCopy = NULL;
	try
	{
		
		drawFishStatusHdc(true,&hBitMap);
		LOG(INFO) << " checkScreenFishStatus drawFishStatusHdc ";
		sampleImg = hBitmapToIpl(hBitMap);
		LOG(INFO) << " checkScreenFishStatus hBitmapToIpl ";
		//fishStatusTemplate = cvLoadImage(sys.FISHING_STATUS_IMGID_FILE, CV_LOAD_IMAGE_COLOR);
		
		if (refreshResizedImgID())
		{
			LOG(INFO) << " checkScreenFishStatus cvLoadImage fishStatusTemplate";
			if (sampleImg != NULL && sampleImg->width >= sys.resizedFishStatusTemplate->width && sampleImg->height >= sys.resizedFishStatusTemplate->height)
			{
				sampleImgCopy = cvCreateImage(cvGetSize(sampleImg), sampleImg->depth, sampleImg->nChannels);
				//将图片1复制到图片2
				cvCopy(sampleImg, sampleImgCopy, NULL);
				matches = ImgEx::matchTemplate(sampleImgCopy, sys.resizedFishStatusTemplate, NULL,
					sys.saveMatchTemplateFishStatusImg ? sys.RECORD_MATCH_TEMPLAE_FISH_STATUS_RESULT_PATH : "",
					sys.checkScreenFishStatusBoundaryValue, sys.isDisplayMatchTemplateCheckFishStatusLog, STATIC_CHECK_SCREEN_FISH_STATUS_VALUE);
				refreshFishStatusMatchResult(sampleImgCopy); 
				cvReleaseImage(&sampleImgCopy);
				sampleImgCopy = NULL;
				if (!matches)
				{
					sampleImgCopy = cvCreateImage(cvGetSize(sampleImg), sampleImg->depth, sampleImg->nChannels);
					//将图片1复制到图片2
					cvCopy(sampleImg, sampleImgCopy, NULL);
					matches = ImgEx::matchTemplate(sampleImgCopy, sys.resizedFishStatusTemplate2, NULL,
						sys.saveMatchTemplateFishStatusImg ? sys.RECORD_MATCH_TEMPLAE_FISH_STATUS_RESULT_PATH : "",
						sys.checkScreenFishStatusBoundaryValue, sys.isDisplayMatchTemplateCheckFishStatusLog, STATIC_CHECK_SCREEN_FISH_STATUS_VALUE);
					refreshFishStatusMatchResult(sampleImgCopy);
					cvReleaseImage(&sampleImgCopy);
					sampleImgCopy = NULL;
				}
				if (!matches)
				{
					sampleImgCopy = cvCreateImage(cvGetSize(sampleImg), sampleImg->depth, sampleImg->nChannels);
					//将图片1复制到图片2
					cvCopy(sampleImg, sampleImgCopy, NULL);
					matches = ImgEx::matchTemplate(sampleImgCopy, sys.resizedFishStatusTemplate3, NULL,
						sys.saveMatchTemplateFishStatusImg ? sys.RECORD_MATCH_TEMPLAE_FISH_STATUS_RESULT_PATH : "",
						sys.checkScreenFishStatusBoundaryValue, sys.isDisplayMatchTemplateCheckFishStatusLog, STATIC_CHECK_SCREEN_FISH_STATUS_VALUE);
					refreshFishStatusMatchResult(sampleImgCopy);
					cvReleaseImage(&sampleImgCopy);
					sampleImgCopy = NULL;
				}
				if (!matches)
				{
					sampleImgCopy = cvCreateImage(cvGetSize(sampleImg), sampleImg->depth, sampleImg->nChannels);
					//将图片1复制到图片2
					cvCopy(sampleImg, sampleImgCopy, NULL);
					matches = ImgEx::matchTemplate(sampleImgCopy, sys.resizedFishStatusTemplate4, NULL,
						sys.saveMatchTemplateFishStatusImg ? sys.RECORD_MATCH_TEMPLAE_FISH_STATUS_RESULT_PATH : "",
						sys.checkScreenFishStatusBoundaryValue, sys.isDisplayMatchTemplateCheckFishStatusLog, STATIC_CHECK_SCREEN_FISH_STATUS_VALUE);
					refreshFishStatusMatchResult(sampleImgCopy);
					cvReleaseImage(&sampleImgCopy);
					sampleImgCopy = NULL;
				}

				if (!matches)
				{
					sampleImgCopy = cvCreateImage(cvGetSize(sampleImg), sampleImg->depth, sampleImg->nChannels);
					//将图片1复制到图片2
					cvCopy(sampleImg, sampleImgCopy, NULL);
					matches = ImgEx::matchTemplate(sampleImgCopy, sys.resizedFishStatusTemplate5, NULL,
						sys.saveMatchTemplateFishStatusImg ? sys.RECORD_MATCH_TEMPLAE_FISH_STATUS_RESULT_PATH : "",
						sys.checkScreenFishStatusBoundaryValue, sys.isDisplayMatchTemplateCheckFishStatusLog, STATIC_CHECK_SCREEN_FISH_STATUS_VALUE);
					refreshFishStatusMatchResult(sampleImgCopy);
					cvReleaseImage(&sampleImgCopy);
					sampleImgCopy = NULL;
				}

				LOG(INFO) << " checkScreenFishStatus matchTemplate=" << (int)matches;

			}
		}
		
		
	}
	catch (...)
	{
		LOG(ERROR) << " 异常：checkScreenFishStatus 异常 ";
	} 
	::DeleteObject(hBitMap);
	hBitMap = NULL;
	cvReleaseImage(&sampleImg);
	sampleImg = NULL;   
	cvReleaseImage(&sampleImgCopy);
	sampleImgCopy = NULL;
	return matches;
}
bool ImgEx::getFishingFloatFromCheckArea(POINT * fishPoint)
{
	HBITMAP   hBitMap = NULL;
	IplImage* sampleImg = NULL;
	IplImage* fishing_float_std_template = NULL;
	try
	{
		drawFishingFloatField(false, &hBitMap);
		sampleImg = hBitmapToIpl(hBitMap);
		fishing_float_std_template = cvLoadImage(sys.FISHING_FLOAT_IMGID_PATH.c_str(), CV_LOAD_IMAGE_COLOR);
		ImgEx::matchTemplate(sampleImg, fishing_float_std_template, fishPoint,
			sys.saveMatchTemplateFishingFloatImg?sys.RECORD_MATCH_TEMPLATE_FISHING_FLOAT_AREA_RESULT_PATH : "",
			sys.checkScreenFishingFloatBoundaryValue,
			sys.isDisplayMatchTemplateFishingFloatAreaLog, 0);
		fishPoint->x = fishPoint->x + sys.SEARCH_FISHING_CURSOR_X_MIN;
		fishPoint->y = fishPoint->y + sys.SEARCH_FISHING_CURSOR_Y_MIN;
	}
	catch (...)
	{
		LOG(ERROR) << "异常：getFishingFloatFromCheckArea 异常";
	}
	cvReleaseImage(&fishing_float_std_template);
	cvReleaseImage(&sampleImg);
	::DeleteObject(hBitMap);
	return true;
}

BOOL  ImgEx::hBitmapToMat(HBITMAP& _hBmp, Mat& _mat)
{
	if (_hBmp == NULL)
	{
		return false;
	}
	BITMAP bmp;
	GetObject(_hBmp, sizeof(BITMAP), &bmp);
	int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
	int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;
	Mat v_mat;
	v_mat.create(cvSize(bmp.bmWidth, bmp.bmHeight), CV_MAKETYPE(CV_8U, nChannels));
	GetBitmapBits(_hBmp, bmp.bmHeight*bmp.bmWidth*nChannels, v_mat.data);
	_mat = v_mat;
	return TRUE;
}

 
IplImage* ImgEx::hBitmap2Ipl(HBITMAP hBmp)
{
	if(hBmp==NULL)
	{
		return NULL; 
	}
	BITMAP bmp; 
	bmp.bmWidth = 0;
	bmp.bmHeight = 0;
	GetObject(hBmp, sizeof(BITMAP), &bmp);
	 
	if (bmp.bmWidth < 1 || bmp.bmHeight < 1 || bmp.bmBitsPixel<1 || bmp.bmBitsPixel<1)
	{
		return NULL;
	} 
	int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;

	int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;
	IplImage* img = cvCreateImage(cvSize(bmp.bmWidth, bmp.bmHeight), depth, nChannels);

	BYTE *pBuffer = new BYTE[bmp.bmHeight*bmp.bmWidth*nChannels];

	GetBitmapBits(hBmp, bmp.bmHeight*bmp.bmWidth*nChannels, pBuffer);

	memcpy(img->imageData, pBuffer, bmp.bmHeight*bmp.bmWidth*nChannels);
	delete pBuffer; 
	IplImage *dst = cvCreateImage(cvGetSize(img), img->depth, 3); 
	cvCvtColor(img, dst, CV_BGRA2BGR);
	cvReleaseImage(&img);
	img = NULL;
	return dst;
}

IplImage *  ImgEx::resizedIplImage(IplImage* src, double scaleX,double scaleY)
{
	IplImage *desc = NULL;
	CvSize sz;
	if (src)
	{
		sz.width = src->width*scaleX;
		sz.height = src->height*scaleY;
		desc = cvCreateImage(sz, src->depth, src->nChannels);
		//cvResize(src, desc, CV_INTER_CUBIC);
		//cvResize(src, desc, CV_INTER_LINEAR); 
	    cvResize(src, desc, CV_INTER_AREA);
	}
	return desc;
}

IplImage *  ImgEx::resizeIplImage(IplImage* src, int width, int height)
{
	IplImage *desc = NULL;
	CvSize sz;
	if (src)
	{
		sz.width = width;
		sz.height = height;
		desc = cvCreateImage(sz, src->depth, src->nChannels);
		//cvResize(src, desc, CV_INTER_CUBIC);
		//cvResize(src, desc, CV_INTER_LINEAR); 
		cvResize(src, desc, CV_INTER_AREA);
	}
	return desc;
}


bool  ImgEx::refreshResizedImgID()
{   
	IplImage* imgId = NULL;
	IplImage* imgId2= NULL;
	IplImage* imgId3= NULL;
	IplImage* imgId4 = NULL;
	IplImage* imgId5 = NULL;
	double scaleX = 1;
	double scaleY = 1;
	if (sys.wowWindowChanged() || sys.resizedFishStatusTemplate ==NULL || sys.resizedFishStatusTemplate2 ==NULL
		|| sys.resizedFishStatusTemplate3 ==NULL || sys.resizedFishStatusTemplate4 == NULL || sys.resizedFishStatusTemplate5 == NULL)
	{
		try
		{
			sys.releaseFishStatusTemplate();

			double scale = (double)(sys.wowWindowRect.right - sys.wowWindowRect.left) / (double)(sys.wowWindowRect.bottom - sys.wowWindowRect.top) / ((double)sys.maxWindowWidht / sys.maxWindowHeight);
  
			scaleX = (double)(sys.wowWindowRect.right - sys.wowWindowRect.left) / sys.maxWindowWidht/ scale; 
			scaleY = (double)(sys.wowWindowRect.bottom - sys.wowWindowRect.top) / sys.maxWindowHeight;
			imgId = cvLoadImage(sys.FISHING_STATUS_IMGID_FILE.c_str(), CV_LOAD_IMAGE_COLOR);
			sys.resizedFishStatusTemplate = resizedIplImage(imgId, scaleX, scaleY);
			cvReleaseImage(&imgId);

			imgId2 = cvLoadImage(sys.FISHING_STATUS2_IMGID_FILE.c_str(), CV_LOAD_IMAGE_COLOR);
			sys.resizedFishStatusTemplate2 = resizedIplImage(imgId2, scaleX, scaleY);
			cvReleaseImage(&imgId2);

			imgId3 = cvLoadImage(sys.FISHING_STATUS3_IMGID_FILE.c_str(), CV_LOAD_IMAGE_COLOR);
			sys.resizedFishStatusTemplate3 = resizedIplImage(imgId3, scaleX, scaleY);
			cvReleaseImage(&imgId3);

			imgId4 = cvLoadImage(sys.FISHING_STATUS4_IMGID_FILE.c_str(), CV_LOAD_IMAGE_COLOR);
			sys.resizedFishStatusTemplate4 = resizedIplImage(imgId4, scaleX, scaleY);
			cvReleaseImage(&imgId4);

			imgId5 = cvLoadImage(sys.FISHING_STATUS5_IMGID_FILE.c_str(), CV_LOAD_IMAGE_COLOR);
			sys.resizedFishStatusTemplate5 = resizedIplImage(imgId5, scaleX, scaleY);
			cvReleaseImage(&imgId5);


			sys.synWowWindowInfo();
		}
		catch (const std::exception&)
		{
			cvReleaseImage(&imgId);
			cvReleaseImage(&imgId2);
			cvReleaseImage(&imgId3);
			cvReleaseImage(&imgId4);
			cvReleaseImage(&imgId5);
		}
		
	} 
	imgId = NULL;
	imgId2 = NULL;
	imgId3 = NULL;
	imgId4 = NULL;
	imgId5 = NULL;
	if (sys.resizedFishStatusTemplate == NULL || sys.resizedFishStatusTemplate2 == NULL || sys.resizedFishStatusTemplate3 == NULL
		|| sys.resizedFishStatusTemplate4 == NULL || sys.resizedFishStatusTemplate5 == NULL)
	{
		LOG(ERROR) << "读取钓鱼状态检测图片失败";  
		return false;
	}
	
	return true;
}
  
bool  ImgEx::iplImageToHbitmap(IplImage* pImg, HBITMAP *hBmp)
{
	BYTE tmp[sizeof(BITMAPINFO) + 1024];
	BITMAPINFO *bmi = (BITMAPINFO*)tmp; 
	int i;
	memset(bmi, 0, sizeof(BITMAPINFO));
	bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi->bmiHeader.biWidth = pImg->width;
	bmi->bmiHeader.biHeight = pImg->height;
	bmi->bmiHeader.biPlanes = 1;
	bmi->bmiHeader.biBitCount = pImg->nChannels * pImg->depth;
	bmi->bmiHeader.biCompression = BI_RGB;
	bmi->bmiHeader.biSizeImage = 0;//if biCompression is BI_RGB,this can be 0
	bmi->bmiHeader.biClrImportant = 0;

	switch (pImg->nChannels * pImg->depth)
	{
	case 8:
		for (i = 0; i < 256; i++) {
			bmi->bmiColors[i].rgbBlue = i;
			bmi->bmiColors[i].rgbGreen = i;
			bmi->bmiColors[i].rgbRed = i;
		}
		break;
	case 32:
	case 24:
		((DWORD*)bmi->bmiColors)[0] = 0x00FF0000;
		((DWORD*)bmi->bmiColors)[1] = 0x0000FF00;
		((DWORD*)bmi->bmiColors)[2] = 0x000000FF;
		break;
	}
	*hBmp = ::CreateDIBSection(NULL, bmi, DIB_RGB_COLORS, NULL, 0, 0);

	SetDIBits(NULL, *hBmp, 0, pImg->height, pImg->imageData, bmi, DIB_RGB_COLORS);

	return true;
}
/*
bool ImgEx::iplImageToHbitmap(IplImage *pImg,HBITMAP *hBmp)
{
	HDC hDC = ::CreateCompatibleDC(0);
	BYTE tmp[sizeof(BITMAPINFO) + 255 * 4];
	BITMAPINFO *bmi = (BITMAPINFO*)tmp;
	int i;
	memset(bmi,0,sizeof(BITMAPINFO));
	bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi->bmiHeader.biWidth = pImg->width;
	bmi->bmiHeader.biHeight = pImg->height;
	bmi->bmiHeader.biPlanes = 1;
	bmi->bmiHeader.biBitCount = pImg->nChannels * pImg->depth;
	bmi->bmiHeader.biCompression = BI_RGB;
	bmi->bmiHeader.biSizeImage = pImg->width*pImg->height * 1;
	bmi->bmiHeader.biClrImportant = 0;
	switch (pImg->nChannels * pImg->depth)
	{
	case 8:
	for (i = 0; i < 256; i++) {
		bmi->bmiColors[i].rgbBlue = i;
		bmi->bmiColors[i].rgbGreen = i;
		bmi->bmiColors[i].rgbRed = i;
	}
	break;
	case 32:
	case 24:
	((DWORD*)bmi->bmiColors)[0] = 0x00FF0000;
	((DWORD*)bmi->bmiColors)[1] = 0x0000FF00;
	((DWORD*)bmi->bmiColors)[2] = 0x000000FF;
	break;
	}
	*hBmp = ::CreateDIBSection(hDC,bmi,DIB_RGB_COLORS,NULL,0,0);
	SetDIBits(hDC,*hBmp,0,pImg->height,pImg->imageData,bmi,DIB_RGB_COLORS);
	::DeleteDC(hDC);
	return true;
}*/