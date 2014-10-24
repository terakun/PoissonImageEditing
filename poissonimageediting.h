#ifndef POISSONIMAGEEDITING_H
#define POISSONIMAGEEDITING_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

cv::Mat Editing(const cv::Mat &src_img,const cv::Mat &trgt_img,const cv::Mat &mask_img,const cv::Rect& area,bool mixture=false);
cv::Mat SimpleEditing(const cv::Mat &src_img,const cv::Mat &trgt_img,const cv::Mat &mask_img,const cv::Rect& area);
#endif // POISSONIMAGEEDITING_H
