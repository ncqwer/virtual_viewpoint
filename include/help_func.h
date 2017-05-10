#ifndef _HSJ_HELP_FUNC_H_VIRTUAL_VIEWPOINT_
#define _HSJ_HELP_FUNC_H_VIRTUAL_VIEWPOINT_ 


#include <opencv2/opencv.hpp>


struct PointInfo
{
    uchar value;
    int x;
    int y;
    PointInfo(const uchar _v,
              const int _x,
              const int _y):
        value(_v),
        x(_x),
        y(_y)
    {}
};

void inverse_img(const cv::Mat& src,
                 cv::Mat& dst);

void fixDisparity_l(cv::Mat& disp, int numberOfDisparities);
void fixDisparity_r(cv::Mat& disp, int numberOfDisparities);

void reprojection_inverse(const cv::Mat& src,
                  const cv::Mat& disp,
                  cv::Mat& dst,
                  bool toright,
                  int nod);

void reprojection_direct(const cv::Mat& src,
                  const cv::Mat& disp,
                  cv::Mat& dst);

void imageFusion(const cv::Mat& img_ml,
                 const cv::Mat& img_mr,
                 cv::Mat& img_m);

void fillHole(cv::Mat& img);

void fillHole_help(const cv::Mat& src,
				   cv::Mat& dst,
				   int num);

bool getMiddlePos(const cv::Mat& roi, 
				  int* p_i, int* p_j,
				  const int num);

#endif