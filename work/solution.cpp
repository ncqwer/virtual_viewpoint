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
	//img read
	Mat imgl = imread("leftview.bmp");
	Mat imgr = imread("rightview.bmp");
	auto width = imgl.cols;
	auto height = imgl.rows;
	int cn = imgl.channels();
	//set sgbm parameters
	StereoSGBM sgbm;
	double number_of_disparities = ((width/8) + 15) & -16;
	double sad_window_size = 5;
	sgbm.preFilterCap = 63;
    sgbm.SADWindowSize = sad_window_size;
	sgbm.P1 = 8*cn*sad_window_size*sad_window_size;
	sgbm.P2 = 32*cn*sad_window_size*sad_window_size;
	sgbm.minDisparity = 0;
    sgbm.numberOfDisparities = number_of_disparities;
    sgbm.uniquenessRatio = 10;

	//get disparity img here maybe we should know overlap 
	//also we should smooth it
	Mat disp_l;
	Mat disp_temp_l;
	Mat disp_show;
	sgbm(imgl,imgr,disp_temp_l);
	disp_temp_l.convertTo(disp_l, CV_32FC1, 1.0/16);
	disp_l.convertTo(disp_show, CV_8U, 255.0/number_of_disparities);
	namedWindow("origin disparity",WINDOW_NORMAL);
	imshow("origin disparity", disp_show);
	//waitKey();

	fixDisparity(disp_l, number_of_disparities);
	disp_l.convertTo(disp_show, CV_8U, 255.0/number_of_disparities);
	namedWindow("fixed disparity",WINDOW_NORMAL);
	imshow("fixed disparity", disp_show);
	// waitKey();
	//we need to know f,and we can set 1 to b
	//=============
	//here we should make clear the relationship base on our own coordinate
	//z=f*b/z
	//y=
	//x=
	//left-middle disparties and middle-right disparties is d/2
	Mat disp_use = disp_l/2;
	Mat img_ml(imgl.rows,imgl.cols,CV_8UC3);
    reprojection_inverse(imgl,disp_use,img_ml,true,number_of_disparities);
    // reprojection_direct(imgl,disp_use,img_ml,true);
	// int cn_disp = disp_use.channels();
	// for ( int j = 0; j < imgl.rows; j++ )
 //    {
 //    	uchar* p_l = imgl.ptr<uchar>(j);
 //    	uchar* p_m = img_ml.ptr<uchar>(j);
 //    	float* p_d = disp_use.ptr<float>(j);
 //    	for (int i = 0; i < imgl.cols; i++)
 //        {
 //            // int x_m = i + *(p_d+i*cn_disp);
 //            float x_l = i + *(p_d+i*cn_disp);
 //            int x_1 = int(x_l);
 //            int x_2 = int(x_l + 0.5);
 //            float w_1 = x_2 - x_l;
 //            float w_2 = x_l - x_1; 
 //            if(x_m < img_ml.cols && x_m > 0)
 //            {
 //            	// *(p_m + x_m*cn + 0 ) = *(p_r + i*cn + 0);
 //            	// *(p_m + x_m*cn + 1 ) = *(p_r + i*cn + 1);
 //            	// *(p_m + x_m*cn + 2 ) = *(p_r + i*cn + 2);
 //            	*(p_m + i*cn + 0) = *(p_l + x_1*cn + 0)*w_1 + *(p_l + x_2*cn + 0)*w_2 ;
 //            	*(p_m + i*cn + 1) = *(p_l + x_1*cn + 1)*w_1 + *(p_l + x_2*cn + 1)*w_2;
 //            	*(p_m + i*cn + 2) = *(p_l + x_1*cn + 2)*w_1 + *(p_l + x_2*cn + 2)*w_2;            
 //            }
 //        }
 //    }
	namedWindow("middle_left",WINDOW_NORMAL);
	imshow("middle_left",img_ml);


	// Mat imgl_i(imgl.rows,imgl.cols,CV_8UC3);
	// Mat imgr_i(imgl.rows,imgl.cols,CV_8UC3);
	// inverse_img(imgl,imgl_i);
	// inverse_img(imgr,imgr_i);
	// Mat disp_r;
	// Mat disp_temp_r;
	// // Mat disp_show;
	// sgbm(imgr_i,imgl_i,disp_temp_r);
	// disp_temp_r.convertTo(disp_r, CV_32FC1, 1.0/16);
	// // disp_l.convertTo(disp_show, CV_8U, 255.0/number_of_disparities);
	// // namedWindow("origin disparity",WINDOW_NORMAL);
	// // imshow("origin disparity", disp_show);
	// //waitKey();

	// fixDisparity(disp_r, number_of_disparities);
	// Mat disp_use_r = disp_r/2;
	// Mat img_mr_i(imgr.rows,imgr.cols,CV_8UC3);
	// for ( int j = 0; j < imgr.rows; j++ )
 //    {
 //    	uchar* p_r = imgr_i.ptr<uchar>(j);
 //    	uchar* p_m = img_mr_i.ptr<uchar>(j);
 //    	float* p_d = disp_use_r.ptr<float>(j);
 //    	for (int i = 0; i < imgr.cols; i++)
 //        {
 //            // int x_m = i + *(p_d+i*cn_disp);
 //            float x_m = i + *(p_d+i*cn_disp);
 //            int x_l = int(x_m);
 //            int x_r = int(x_m + 0.5);
 //            float w_l = x_r - x_m;
 //            float w_r = x_m - x_l; 
 //            if(x_m < img_mr_i.cols && x_m > 0)
 //            {
 //            	// *(p_m + x_m*cn + 0 ) = *(p_r + i*cn + 0);
 //            	// *(p_m + x_m*cn + 1 ) = *(p_r + i*cn + 1);
 //            	// *(p_m + x_m*cn + 2 ) = *(p_r + i*cn + 2);
 //            	*(p_m + i*cn + 0) = *(p_r + x_l*cn + 0)*w_l + *(p_r + x_r*cn + 0)*w_r ;
 //            	*(p_m + i*cn + 1) = *(p_r + x_l*cn + 1)*w_l + *(p_r + x_r*cn + 1)*w_r;
 //            	*(p_m + i*cn + 2) = *(p_r + x_l*cn + 2)*w_l + *(p_r + x_r*cn + 2)*w_r;            
 //            }
 //        }
 //    }
    Mat img_mr(imgr.rows,imgr.cols,CV_8UC3);
    reprojection_inverse(imgr,disp_use,img_mr,false,number_of_disparities);
    // reprojection_direct(imgr,disp_use,img_mr,false);
    // for ( int j = 0; j < img_mr.rows; j++ )
    // {
    //     uchar* p_l = imgr.ptr<uchar>(j);
    //     uchar* p_m = img_mr.ptr<uchar>(j);
    //     float* p_d = disp_use.ptr<float>(j);
    //     for (int i = 0; i < img_mr.cols; i++)
    //     {
    //         // int x_m = i + *(p_d+i*cn_disp);
    //         float x_r = i - *(p_d+i*cn_disp);
    //         int x_1 = int(x_r);
    //         int x_2 = int(x_m + 0.5);
    //         float w_1 = x_2 - x_m;
    //         float w_2 = x_r - x_1; 
    //         if(x_m < img_ml.cols && x_m > 0)
    //         {
    //             // *(p_m + x_m*cn + 0 ) = *(p_r + i*cn + 0);
    //             // *(p_m + x_m*cn + 1 ) = *(p_r + i*cn + 1);
    //             // *(p_m + x_m*cn + 2 ) = *(p_r + i*cn + 2);
    //             *(p_m + i*cn + 0) = *(p_l + x_l*cn + 0)*w_l + *(p_l + x_r*cn + 0)*w_r ;
    //             *(p_m + i*cn + 1) = *(p_l + x_l*cn + 1)*w_l + *(p_l + x_r*cn + 1)*w_r;
    //             *(p_m + i*cn + 2) = *(p_l + x_l*cn + 2)*w_l + *(p_l + x_r*cn + 2)*w_r;            
    //         }
    //     }
    // }
    // inverse_img(img_mr_i,img_mr);
	Mat img_m(imgr.rows,imgr.cols,CV_8UC3);
	for ( int j = 0; j < imgr.rows; j++ )
    {
    	uchar* p_l = img_ml.ptr<uchar>(j);
    	uchar* p_r = img_mr.ptr<uchar>(j);
    	uchar* p_m = img_m.ptr<uchar>(j);
    	uchar last_value_l_B = 0;
    	uchar last_value_r_B = 0;
    	for (int i = 0; i < imgr.cols; i++)
        {
            uchar v_l = *(p_l + i*cn);
            uchar v_r = *(p_r + i*cn);
            if(v_l == 0)
            {
            	v_l = last_value_l_B;
            }
            if(v_r == 0)
            {
            	v_r = last_value_r_B;
            }
            if(i > (img_m.cols/2))
            {
            	*(p_m + i*cn) = v_r;

            }
            else
            {
	            *(p_m + i*cn) = v_l;
            }
            // *(p_m + i*cn) = v_l == 0 ? v_r : v_l;
            last_value_l_B = v_l;
            last_value_r_B = v_r;
        }
    	uchar last_value_l_G = 0;
    	uchar last_value_r_G = 0;
    	for (int i = 0; i < imgr.cols; i++)
        {
            uchar v_l = *(p_l + i*cn + 1);
            uchar v_r = *(p_r + i*cn + 1);
            if(v_l == 0)
            {
            	v_l = last_value_l_G;
            }
            if(v_r == 0)
            {
            	v_r = last_value_r_G;
            }
            if(i > (img_m.cols/2))
            {
            	*(p_m + i*cn + 1) = v_r;

            }
            else
            {
	            *(p_m + i*cn + 1) = v_l;
            }
            // *(p_m + i*cn + 1) = v_l == 0 ? v_r : v_l;
            last_value_l_G = v_l;
            last_value_r_G = v_r;
        }
    	uchar last_value_l_R = 0;
    	uchar last_value_r_R = 0;
    	for (int i = 0; i < imgr.cols; i++)
        {
            uchar v_l = *(p_l + i*cn + 2);
            uchar v_r = *(p_r + i*cn + 2);
            if(v_l == 0)
            {
            	v_l = last_value_l_R;
            }
            if(v_r == 0)
            {
            	v_r = last_value_r_R;
            }
            if(i > (img_m.cols/2))
            {
            	*(p_m + i*cn + 2) = v_r;

            }
            else
            {
	            *(p_m + i*cn + 2) = v_l;
            }
            // *(p_m + i*cn +2) = v_l == 0 ? v_r : v_l;
            last_value_l_R = v_l;
            last_value_r_R = v_r;
        }
    }
	namedWindow("middle_right",WINDOW_NORMAL);
	imshow("middle_right",img_mr);
	namedWindow("left",WINDOW_NORMAL);
	imshow("left",imgl);
	namedWindow("right",WINDOW_NORMAL);
	imshow("right",imgr);
	namedWindow("middle",WINDOW_NORMAL);
	imshow("middle",img_m);
	waitKey();
	//now we get 3d points
	

	return 0;
}
