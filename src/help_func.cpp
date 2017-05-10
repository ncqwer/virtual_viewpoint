#include "../include/help_func.h"
#include <iostream> 

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


bool operator< (const PointInfo& lhs, const PointInfo& rhs)
{
    return lhs.value < rhs.value;
}
void fixDisparity_l(cv::Mat& disp, int numberOfDisparities ) 
{
	cv::Mat disp_temp;
	float minDisparity =13;// algorithm parameters that can be modified
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
    // for(int j = 0; j < height; ++j)
    // {
    //     float *p =disp.ptr<float>(j);   
    //     for(int i = numberOfDisparities; i < width; ++i)
    //     {
    //         if (*(p + i*cn) <= minDisparity) *(p + i*cn) = lastPixel;
    //         // if (abs(*(p + i*cn)-lastPixel) < 3) *(p + i*cn) = lastPixel;
    //         else lastPixel = *(p + i*cn);
    //     }
    // }
	int an = 4;	// algorithm parameters that can be modified
	cv::copyMakeBorder(disp, disp_temp, an,an,an,an, cv::BORDER_REPLICATE);
	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(an*2+1, an*2+1));
	cv::morphologyEx(disp_temp, disp_temp, CV_MOP_OPEN, element);
	cv::morphologyEx(disp_temp, disp_temp, CV_MOP_CLOSE, element);
	disp = disp_temp(cv::Range(an, disp_temp.rows-an), cv::Range(an, disp_temp.cols-an)).clone();

    cv::GaussianBlur(disp,disp,cv::Size(5,5),0.2);
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


void imageFusion(const cv::Mat& img_ml,
                 const cv::Mat& img_mr,
                 cv::Mat& img_m)
{
    int cn = img_m.channels();
    for ( int j = 0; j < img_m.rows; j++ )
    {   
        const uchar* p_l = img_ml.ptr<uchar>(j);
        const uchar* p_r = img_mr.ptr<uchar>(j);
        uchar* p_m = img_m.ptr<uchar>(j);
        for (int i = 0; i < img_m.cols; i++)
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
        for (int i = 0; i < img_m.cols; i++)
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
        for (int i = 0; i < img_m.cols; i++)
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
}


void fillHole(cv::Mat& img)
{
    for(int i=0;i<10;++i)
    {
        for(int n = 1; n < 9; ++n)
        {
            // Mat dst(img.rows,img.cols,CV_8U3C,cv::Scalar(0,0,0));
            cv::Mat dst = img.clone();
            fillHole_help(img,dst,9-n);
            // cv::namedWindow("middle_before",cv::WINDOW_NORMAL);
            // cv::imshow("middle_before",img);
            // cv::namedWindow("middle_after",cv::WINDOW_NORMAL);
            // cv::imshow("middle_after",dst);
            // cv::waitKey();
            img = dst.clone();
        }
    }
}

void fillHole_help(const cv::Mat& src,
                   cv::Mat& dst,
                   int num)
{
    int cn = src.channels();
    int width = src.cols;
    int height = src.rows;
    for(int y = 1; y < height-1; ++y)
    {
        const uchar* p_src = src.ptr<uchar>(y);
        uchar* p_dst = dst.ptr<uchar>(y);
        for(int x = 1; x < width-1; ++x)
        {
            uchar b = *(p_src + cn*x + 0);   
            uchar g = *(p_src + cn*x + 1);   
            uchar r = *(p_src + cn*x + 2);   
            if(b != 0 || g != 0 || r != 0)
                continue;
            cv::Mat roi(src,cv::Rect(x-1,y-1,3,3));
            if(x == 225 && y ==1)
                int hsj=1;
            int i=0; int j=0;
            if(getMiddlePos(roi,&i,&j,num))
            {
                // std::cout<<"i succeed"<<std::endl;
                if(i<0 || j<0 || i>2 || j>2)
                    int hsj=1;
                i += (x-1);
                j += (y-1);
                const uchar* p_src_use = src.ptr<uchar>(j);
                // *(p_dst + cn*x + 0) = *(p_src_use + cn*i + 0);
                // *(p_dst + cn*x + 1) = *(p_src_use + cn*i + 1);
                // *(p_dst + cn*x + 2) = *(p_src_use + cn*i + 2);
                auto b = *(p_src_use + cn*i + 0);
                auto g = *(p_src_use + cn*i + 1);
                auto r = *(p_src_use + cn*i + 2);
                *(p_dst + cn*x + 0) = b;
                *(p_dst + cn*x + 1) = g;
                *(p_dst + cn*x + 2) = r;
            }
        }
    }
}

bool getMiddlePos(const cv::Mat& roi, int* p_i, int* p_j,const int num)
{
    std::vector<PointInfo> pts;
    int cn = roi.channels();
    int width = roi.cols;
    int height = roi.rows;
    int non_zero_num = 0;
    for(int y = 0; y < height; ++y)
    {
        const uchar* p_roi = roi.ptr<uchar>(y);
        for(int x = 0; x < width; ++x)
        {
            uchar b = *(p_roi + cn*x + 0);   
            uchar g = *(p_roi + cn*x + 0);   
            uchar r = *(p_roi + cn*x + 0);   
            if(b != 0 || g != 0 || r != 0)
            {
                ++non_zero_num;
                pts.push_back(PointInfo(b,x,y));
            }
        }
    }
    if(non_zero_num < num)
        return false;
    std::sort(pts.begin(),pts.end());
    auto sz = pts.size();
    if(sz == 0)
        int hsj=1;
    int mid = sz/2;
    *p_i = pts[mid].x;
    *p_j = pts[mid].y;
    return true;
}