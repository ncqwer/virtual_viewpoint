#include <iostream>
#include <opencv2/opencv.hpp>

#include "../include/help_func.h"
using namespace cv;
using namespace std;


void inverse_img(const cv::Mat& src,
		  cv::Mat& dst)
{
	int cn = src.channels();
	for ( int j = 0; j < src.rows; j++ )
    {
    	const uchar* p_s = src.ptr<uchar>(j);
    	uchar* p_d = dst.ptr<uchar>(j);
    	for (int i = 0; i < src.cols; i++)
        {
            int x_m = src.cols - 1 - i;
        	*(p_d + x_m*cn + 0) = *(p_s + i*cn + 0);
        	*(p_d + x_m*cn + 1) = *(p_s + i*cn + 1);
        	*(p_d + x_m*cn + 2) = *(p_s + i*cn + 2);
        }
    }
}



int main(int argc, char const *argv[])
{
    enum { STEREO_BM=0, STEREO_SGBM=1, STEREO_HH=2 };
    int alg = STEREO_SGBM;
	//img read
	Mat imgl = imread("leftview.bmp");
	Mat imgr = imread("rightview.bmp");
	auto width = imgl.cols;
	auto height = imgl.rows;
	int cn = imgl.channels();
	//set sgbm parameters
	StereoSGBM sgbm;
    // double number_of_disparities = ((width/8) + 15) & -16;
	double number_of_disparities = 144;
	double sad_window_size = 5;
	sgbm.preFilterCap = 63;
    sgbm.SADWindowSize = sad_window_size;
	sgbm.P1 = 4*cn*sad_window_size*sad_window_size;
	sgbm.P2 = 32*cn*sad_window_size*sad_window_size;
    sgbm.numberOfDisparities = number_of_disparities;
    sgbm.minDisparity = 0;  
    sgbm.uniquenessRatio = 10;  
    sgbm.speckleWindowSize = 100;  
    sgbm.speckleRange = 32;  
    sgbm.disp12MaxDiff = 1;
    sgbm.fullDP = alg == STEREO_HH;  


    Mat imglb,imgrb;
    copyMakeBorder(imgl, imglb, 0, 0, number_of_disparities, 0, IPL_BORDER_REPLICATE);
    copyMakeBorder(imgr, imgrb, 0, 0, number_of_disparities, 0, IPL_BORDER_REPLICATE); 
	Mat disp_l;
	Mat disp_temp_l;
	Mat disp_show_l;
    sgbm(imglb,imgrb,disp_temp_l);
	// sgbm(imgl,imgr,disp_temp_l);
	disp_temp_l.convertTo(disp_l, CV_32FC1, 1.0/16);
	disp_l.convertTo(disp_show_l, CV_8U, 255.0/number_of_disparities);
	namedWindow("left origin disparity",WINDOW_NORMAL);
    imshow("left origin disparity", disp_show_l);

    fixDisparity_l(disp_l, number_of_disparities);
    disp_l.convertTo(disp_show_l, CV_8U, 255.0/number_of_disparities);
    namedWindow("left fixed disparity",WINDOW_NORMAL);
    imshow("left fixed disparity", disp_show_l);

    Mat img_ml(imgl.rows,imgl.cols,CV_8UC3,Scalar(0,0,0));
    reprojection_direct(imgl,disp_l/2,img_ml);
    namedWindow("middle_left",WINDOW_NORMAL);
    imshow("middle_left",img_ml);

    Mat disp_r;
    Mat disp_temp_r;
    Mat disp_show_r;
    Mat imgl_i(imgl.rows,imgl.cols,CV_8UC3);
    Mat imgr_i(imgr.rows,imgr.cols,CV_8UC3);
    inverse_img(imgl,imgl_i);
    inverse_img(imgr,imgr_i);

    Mat imglb_i,imgrb_i;
    copyMakeBorder(imgl_i, imglb_i, 0, 0, number_of_disparities, 0, IPL_BORDER_REPLICATE);
    copyMakeBorder(imgr_i, imgrb_i, 0, 0, number_of_disparities, 0, IPL_BORDER_REPLICATE);
    sgbm(imgrb_i,imglb_i,disp_temp_r);
    disp_temp_r.convertTo(disp_r, CV_32FC1, 1.0/16);
    disp_r.convertTo(disp_show_r, CV_8U, 255.0/number_of_disparities);
    namedWindow("right origin disparity",WINDOW_NORMAL);
    imshow("right origin disparity", disp_show_r);

    fixDisparity_l(disp_r, number_of_disparities);
    disp_r.convertTo(disp_show_r, CV_8U, 255.0/number_of_disparities);
    namedWindow("right fixed disparity",WINDOW_NORMAL);
    imshow("right fixed disparity", disp_show_r);

    Mat img_mr_i(imgr.rows,imgr.cols,CV_8UC3,Scalar(0,0,0));
    reprojection_direct(imgr_i,disp_r/2,img_mr_i);
    Mat img_mr(imgr.rows,imgr.cols,CV_8UC3);
    inverse_img(img_mr_i,img_mr);
    namedWindow("middle_right",WINDOW_NORMAL);
    imshow("middle_right",img_mr_i);


	Mat img_m(imgr.rows,imgr.cols,CV_8UC3);
	for ( int j = 0; j < imgr.rows; j++ )
    {   
    	uchar* p_l = img_ml.ptr<uchar>(j);
    	uchar* p_r = img_mr.ptr<uchar>(j);
    	uchar* p_m = img_m.ptr<uchar>(j);
    	for (int i = 0; i < imgr.cols; i++)
        {
            uchar v_l = *(p_l + i*cn);
            uchar v_r = *(p_r + i*cn);
            uchar v = v_l/2 + v_r/2;
            if( v_l == 0 )
                v = v_r;
            if( v_r == 0 )
                v = v_l;
            *(p_m + i*cn) = v;
        }
    	for (int i = 0; i < imgr.cols; i++)
        {
            uchar v_l = *(p_l + i*cn+1);
            uchar v_r = *(p_r + i*cn+1);
            uchar v = v_l/2 + v_r/2;
            if( v_l == 0 )
                v = v_r;
            if( v_r == 0 )
                v = v_l;
            *(p_m + i*cn+1) = v;
        }
    	for (int i = 0; i < imgr.cols; i++)
        {
            uchar v_l = *(p_l + i*cn+2);
            uchar v_r = *(p_r + i*cn+2);
            uchar v = v_l/2 + v_r/2;
            if( v_l == 0 )
                v = v_r;
            if( v_r == 0 )
                v = v_l;
            *(p_m + i*cn+2) = v;
        }
    }

	namedWindow("left",WINDOW_NORMAL);
	imshow("left",imgl);
	namedWindow("right",WINDOW_NORMAL);
	imshow("right",imgr_i);
    cv::blur(img_m,img_m,cv::Size(3,3));
	namedWindow("middle",WINDOW_NORMAL);
	imshow("middle",img_m);
    imwrite("dst.bmp",img_m);
	waitKey();
	// //now we get 3d points
	

	return 0;
}
