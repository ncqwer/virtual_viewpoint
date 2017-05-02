#include "../include/help_func.h"
#include <iostream>
void fixDisparity_l(cv::Mat& disp, int numberOfDisparities ) 
{
	cv::Mat disp_temp;
	float minDisparity =23;// algorithm parameters that can be modified
	// for (int i = 0; i < disp.rows; i++)
	// {
	// 	for (int j = numberOfDisparities; j < disp.cols; j++)
	// 	{
	// 		if (disp.at(i,j) <= minDisparity) disp.at(i,j) = lastPixel;
	// 		else lastPixel = disp.at(i,j);
	// 	}
	// }
	float lastPixel = 0;
    auto cn = disp.channels();
    auto width = disp.cols;
    auto height = disp.rows; 
    for(int j = 0; j < height; ++j)
    {
        float *p =disp.ptr<float>(j);   
        for(int i = numberOfDisparities; i < width; ++i)
        {
            if (*(p + i*cn) <= minDisparity) *(p + i*cn) = lastPixel;
            // if (abs(*(p + i*cn)-lastPixel) < 3) *(p + i*cn) = lastPixel;
            else lastPixel = *(p + i*cn);
        }
    }
	// int an = 3;	// algorithm parameters that can be modified
	// cv::copyMakeBorder(disp, disp_temp, an,an,an,an, cv::BORDER_REPLICATE);
	// cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(an*2+1, an*2+1));
	// cv::morphologyEx(disp_temp, disp_temp, CV_MOP_OPEN, element);
	// cv::morphologyEx(disp_temp, disp_temp, CV_MOP_CLOSE, element);
	// disp = disp_temp(cv::Range(an, disp_temp.rows-an), cv::Range(an, disp_temp.cols-an)).clone();

    // cv::GaussianBlur(disp,disp,cv::Size(5,5),0.2);
    disp_temp = disp(cv::Range(0,disp.rows),cv::Range(numberOfDisparities,disp.cols)).clone();
    disp = disp_temp;
}

void fixDisparity_r(cv::Mat& disp, int numberOfDisparities ) 
{
    cv::Mat disp_temp;
    float minDisparity =23;// algorithm parameters that can be modified
    // for (int i = 0; i < disp.rows; i++)
    // {
    //  for (int j = numberOfDisparities; j < disp.cols; j++)
    //  {
    //      if (disp.at(i,j) <= minDisparity) disp.at(i,j) = lastPixel;
    //      else lastPixel = disp.at(i,j);
    //  }
    // }
    float lastPixel = 0;
    auto cn = disp.channels();
    auto width = disp.cols;
    auto height = disp.rows; 
    for(int j = 0; j < height; ++j)
    {
        float *p =disp.ptr<float>(j);   
        for(int i = width; i < numberOfDisparities; i--)
        {
            if (*(p + i*cn) <= minDisparity) *(p + i*cn) = lastPixel;
            // if (abs(*(p + i*cn)-lastPixel) < 3) *(p + i*cn) = lastPixel;
            else lastPixel = *(p + i*cn);
        }
    }
    int an = 3; // algorithm parameters that can be modified
    cv::copyMakeBorder(disp, disp_temp, an,an,an,an, cv::BORDER_REPLICATE);
    cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(an*2+1, an*2+1));
    cv::morphologyEx(disp_temp, disp_temp, CV_MOP_OPEN, element);
    cv::morphologyEx(disp_temp, disp_temp, CV_MOP_CLOSE, element);
    disp = disp_temp(cv::Range(an, disp_temp.rows-an), cv::Range(an, disp_temp.cols-an)).clone();

    cv::GaussianBlur(disp,disp,cv::Size(5,5),0.2);
}

//in: src
//in: disp
//out: dst
//bool: isleft
void reprojection_inverse(const cv::Mat& src,
                  const cv::Mat& disp,
                  cv::Mat& dst,
                  bool to_right,
                  int nod)
{
    int cn = dst.channels();
    int cn_disp = disp.channels();
    if(to_right)
    {
        for ( int j = 0; j < dst.rows; j++ )
        {
            uchar* p_dst = dst.ptr<uchar>(j);
            const uchar* p_src = src.ptr<uchar>(j);
            const float* p_disp = disp.ptr<float>(j);
            for (int i = 0; i < dst.cols; i++)
            {
                // int x_m = i + *(p_d+i*cn_disp);
                float x_src;
                x_src = i + *(p_disp+(i + nod)*cn_disp)/2;
                if(i<2 && j<2)
                    std::cout<<"the left ("<<i<<","<<j<<"): "<<x_src<<std::endl;
                int x_l = int(x_src);
                int x_r = x_l + 1;
                float w_l = 1 - (x_src - x_l);
                float w_r = 1 - (x_r - x_src); 
                if(x_src < src.cols && x_src > 0)
                {
                    // *(p_m + x_m*cn + 0 ) = *(p_r + i*cn + 0);
                    // *(p_m + x_m*cn + 1 ) = *(p_r + i*cn + 1);
                    // *(p_m + x_m*cn + 2 ) = *(p_r + i*cn + 2);
                    *(p_dst + i*cn + 0) = *(p_src + x_l*cn + 0)*w_l + *(p_src + x_r*cn + 0)*w_r;
                    *(p_dst + i*cn + 1) = *(p_src + x_l*cn + 1)*w_l + *(p_src + x_r*cn + 1)*w_r;
                    *(p_dst + i*cn + 2) = *(p_src + x_l*cn + 2)*w_l + *(p_src + x_r*cn + 2)*w_r;            
                }
            }
        }

    }
    else
    {
        for ( int j = 0; j < dst.rows; j++ )
        {
            uchar* p_dst = dst.ptr<uchar>(j);
            const uchar* p_src = src.ptr<uchar>(j);
            const float* p_disp = disp.ptr<float>(j);
            for (int i = 0; i < dst.cols; i++)
            {
                // int x_m = i + *(p_d+i*cn_disp);
                float x_src;
                x_src = i - *(p_disp+i*cn_disp); 
                if(i< nod+2 && j< 2)
                    std::cout<<"the right ("<<i<<","<<j<<"): "<<x_src<<std::endl;
                int x_l = int(x_src);
                int x_r = x_l + 1;
                float w_l = 1 - (x_src - x_l);
                float w_r = 1 - (x_r - x_src);  
                if(x_src < src.cols && x_src > 0)
                {
                    // *(p_m + x_m*cn + 0 ) = *(p_r + i*cn + 0);
                    // *(p_m + x_m*cn + 1 ) = *(p_r + i*cn + 1);
                    // *(p_m + x_m*cn + 2 ) = *(p_r + i*cn + 2);
                    *(p_dst + i*cn + 0) = *(p_src + x_l*cn + 0)*w_l + *(p_src + x_r*cn + 0)*w_r;
                    *(p_dst + i*cn + 1) = *(p_src + x_l*cn + 1)*w_l + *(p_src + x_r*cn + 1)*w_r;
                    *(p_dst + i*cn + 2) = *(p_src + x_l*cn + 2)*w_l + *(p_src + x_r*cn + 2)*w_r;            
                }
            }
        }
    }
}


//in: src
//in: disp
//out: dst
//bool: isleft
void reprojection_direct(const cv::Mat& src,
                  const cv::Mat& disp,
                  cv::Mat& dst)
{
    int cn = dst.channels();
    int cn_disp = disp.channels();
    for ( int j = 0; j < src.rows; j++ )
    {
        uchar* p_dst = dst.ptr<uchar>(j);
        const uchar* p_src = src.ptr<uchar>(j);
        const float* p_disp = disp.ptr<float>(j);
        for (int i = 0; i < src.cols; i++)
        {
            // int x_m = i + *(p_d+i*cn_disp);
            int x_dst;
            x_dst = i - *(p_disp+i*cn_disp);
            if(x_dst < dst.cols && x_dst > 0)
            {
                *(p_dst + x_dst*cn + 0) = *(p_src + i*cn + 0);
                *(p_dst + x_dst*cn + 1) = *(p_src + i*cn + 1);
                *(p_dst + x_dst*cn + 2) = *(p_src + i*cn + 2);            
            }
        }
    }
    // cv::blur(dst,dst,cv::Size(5,5));
}