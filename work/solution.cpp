#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

#include "../include/help_func.h"
#include "../include/module.h"
using namespace cv;
using namespace std;





int main(int argc, char const *argv[])
{
    enum { STEREO_BM=0, STEREO_SGBM=1, STEREO_HH=2 };
    int alg = STEREO_SGBM;
	//img read
	Mat imgl = imread("right.bmp");
	Mat imgr = imread("left.bmp");
	auto width = imgl.cols;
	auto height = imgl.rows;
	int cn = imgl.channels();
    //=======================sgbm parameters init=========
	//set sgbm parameters 
	StereoSGBM sgbm;
    double number_of_disparities = ((width/8) + 15) & -16;
	// double number_of_disparities = 720;
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
    // int hsj = 14.75;
    int hsj = 0;


    //======================left->right disparity============
    Mat imglb,imgrb;
    copyMakeBorder(imgl, imglb, 0, 0, number_of_disparities, 0, IPL_BORDER_REPLICATE);
    copyMakeBorder(imgr, imgrb, 0, 0, number_of_disparities, 0, IPL_BORDER_REPLICATE); 
	Mat disp_l;
	Mat disp_temp_l;
	Mat disp_show_l;
    // sgbm(imglb,imgrb,disp_temp_l);
    std::string message_g_l = "generate the disparity(left->right)";
    moduleCall(message_g_l,std::cout,sgbm,imglb,imgrb,disp_temp_l);
    // sgbm(imgl,imgr,disp_temp_l);
    disp_temp_l.convertTo(disp_l, CV_32FC1, 1.0/16);
    disp_l.convertTo(disp_show_l, CV_8U, 255.0/number_of_disparities);
    // namedWindow("left origin disparity",WINDOW_NORMAL);
    // imshow("left origin disparity", disp_show_l);
    

    //=======================fix the disparity(left->right)=============
    std::string message_f_l = "fix the disparity(left->right)";
    // fixDisparity_l(disp_l, number_of_disparities);
    moduleCall(message_f_l,std::cout,fixDisparity_l,disp_l, number_of_disparities);
    // disp_l.convertTo(disp_show_l, CV_8U, 255.0/number_of_disparities);
    // namedWindow("left fixed disparity",WINDOW_NORMAL);
    // imshow("left fixed disparity", disp_show_l);

    //====================reprojection to get middle_left================
    std::string message_m_l = "reprojection to get middle_left";
    Mat img_ml(imgl.rows,imgl.cols,CV_8UC3,Scalar(0,0,0));
    // reprojection_direct(imgl,disp_l/2-hsj,img_ml);
    moduleCall(message_m_l,std::cout,reprojection_direct,imgl,disp_l/2,img_ml);

    //==========================inverse the image===========
    Mat disp_r;
    Mat disp_temp_r;
    Mat disp_show_r;
    Mat imgl_i(imgl.rows,imgl.cols,CV_8UC3);
    Mat imgr_i(imgr.rows,imgr.cols,CV_8UC3);
    // inverse_img(imgl,imgl_i);
    moduleCall("inverse left image",std::cout,inverse_img,imgl,imgl_i);
    // inverse_img(imgr,imgr_i);
    moduleCall("inverse right image",std::cout,inverse_img,imgr,imgr_i);

    //==========================right->left disparity===============
    Mat imglb_i,imgrb_i;
    copyMakeBorder(imgl_i, imglb_i, 0, 0, number_of_disparities, 0, IPL_BORDER_REPLICATE);
    copyMakeBorder(imgr_i, imgrb_i, 0, 0, number_of_disparities, 0, IPL_BORDER_REPLICATE);
    // sgbm(imgrb_i,imglb_i,disp_temp_r);
    moduleCall("generate the disparity(right->left)",std::cout,sgbm,imgrb_i,imglb_i,disp_temp_r);
    disp_temp_r.convertTo(disp_r, CV_32FC1, 1.0/16);
    disp_r.convertTo(disp_show_r, CV_8U, 255.0/number_of_disparities);
    // namedWindow("right origin disparity",WINDOW_NORMAL);
    // imshow("right origin disparity", disp_show_r);

    //=======================fix the disparity(right->left)=============
    // fixDisparity_l(disp_r, number_of_disparities);
    moduleCall("fix the disparity(right->left)",std::cout,fixDisparity_l,disp_r, number_of_disparities);
    disp_r.convertTo(disp_show_r, CV_8U, 255.0/number_of_disparities);
    // namedWindow("right fixed disparity",WINDOW_NORMAL);
    // imshow("right fixed disparity", disp_show_r);

    //======================reprojection to get middle_right==================
    Mat img_mr_i(imgr.rows,imgr.cols,CV_8UC3,Scalar(0,0,0));
    // reprojection_direct(imgr_i,disp_r/2-hsj,img_mr_i);
    moduleCall("reprojection to get middle_right",std::cout,reprojection_direct,imgr_i,disp_r/2,img_mr_i);

    //======================inverse image=====================
    Mat img_mr(imgr.rows,imgr.cols,CV_8UC3);
    // inverse_img(img_mr_i,img_mr);
    moduleCall("inverse image to get normal middle's image from right",std::cout,
                inverse_img,img_mr_i,img_mr);


    //====================image fusion=================
	Mat img_m(imgr.rows,imgr.cols,CV_8UC3);
    moduleCall("fuse img_ml and img_mr to get middle's image",std::cout,
                imageFusion,img_ml,img_mr,img_m);


    //=================show image==================
    namedWindow("middle_left",WINDOW_NORMAL);
    imshow("middle_left",img_ml);
    namedWindow("middle_right",WINDOW_NORMAL);
    imshow("middle_right",img_mr_i);

    namedWindow("left",WINDOW_NORMAL);
    imshow("left",imgl);
    namedWindow("right",WINDOW_NORMAL);
    imshow("right",imgr);
    // cv::blur(img_m,img_m,cv::Size(3,3));
    namedWindow("middle_before",WINDOW_NORMAL);
    imshow("middle_before",img_m);


    //=================fill the hole================
	// fillHole(img_m);
    moduleCall("fill the hole",std::cout,
                fillHole,img_m);
    namedWindow("middle_after",WINDOW_NORMAL);
    imshow("middle_after",img_m);
    imwrite("Team01_Final.bmp",img_m);
    waitKey();
    // //now we get 3d points

	return 0;
}
