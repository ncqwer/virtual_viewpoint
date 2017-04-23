#ifndef _HSJ_HELP_FUNC_H_VIRTUAL_VIEWPOINT_
#define _HSJ_HELP_FUNC_H_VIRTUAL_VIEWPOINT_ 


#include <opencv2/opencv.hpp>

void fixDisparity(cv::Mat& disp, int numberOfDisparities);

void reprojection_inverse(const cv::Mat& src,
                  const cv::Mat& disp,
                  cv::Mat& dst,
                  bool toright,
                  int nod);

void reprojection_direct(const cv::Mat& src,
                  const cv::Mat& disp,
                  cv::Mat& dst,
                  bool to_right);
#endif