#include "poissonimageediting.h"
#include <iostream>
cv::Mat Editing(const cv::Mat &src_img,const cv::Mat &trgt_img,const cv::Mat &mask_img,const cv::Rect& area,bool mixture){
  cv::Mat dst_img=trgt_img.clone();
  cv::Mat work_dst(dst_img.rows,dst_img.cols,CV_64FC3);
  cv::Mat work_src(src_img.rows,src_img.cols,CV_64FC3);
  const double eps=1,omega=1.0;
  double error;
  for(int i=0;i<work_dst.rows;i++){
    for(int j=0;j<work_dst.cols;j++){
      work_dst.at<cv::Vec3d>(i,j)[0]=(double)dst_img.at<cv::Vec3b>(i,j)[0];
      work_dst.at<cv::Vec3d>(i,j)[1]=(double)dst_img.at<cv::Vec3b>(i,j)[1];
      work_dst.at<cv::Vec3d>(i,j)[2]=(double)dst_img.at<cv::Vec3b>(i,j)[2];
    }
  }
  cv::Mat work_trgt=work_dst.clone();
  for(int i=0;i<work_src.rows;i++){
    for(int j=0;j<work_src.cols;j++){
      work_src.at<cv::Vec3d>(i,j)[0]=(double)src_img.at<cv::Vec3b>(i,j)[0];
      work_src.at<cv::Vec3d>(i,j)[1]=(double)src_img.at<cv::Vec3b>(i,j)[1];
      work_src.at<cv::Vec3d>(i,j)[2]=(double)src_img.at<cv::Vec3b>(i,j)[2];
    }
  }

  do{
    error=0.0;
    for( int i=area.y+1;i<area.y+area.height-1;i++ ){
      for( int j=area.x+1;j<area.x+area.width-1;j++ ){

          if((uchar)(mask_img.data+mask_img.step.p[0]*(i-area.y))[j-area.x]) continue;
          cv::Vec3d newPixel,sum_dst,sum_src,sum_trgt_grad;
          int sum1=0,sum2=0;


          for(int rgb=0;rgb<3;rgb++){
              sum_dst[rgb]=
                 ((cv::Vec3d*)(work_dst.data+work_dst.step.p[0]*(i+1)))[j][rgb]
                +((cv::Vec3d*)(work_dst.data+work_dst.step.p[0]*(i-1)))[j][rgb]
                +((cv::Vec3d*)(work_dst.data+work_dst.step.p[0]*i))[j+1][rgb]
                +((cv::Vec3d*)(work_dst.data+work_dst.step.p[0]*i))[j-1][rgb];

              sum_src[rgb]=
                 4*((cv::Vec3d*)(work_src.data+work_src.step.p[0]*(i-area.y)))[j-area.x][rgb]
                -((cv::Vec3d*)(work_src.data+work_src.step.p[0]*(i-1-area.y)))[j-area.x][rgb]
                -((cv::Vec3d*)(work_src.data+work_src.step.p[0]*(i+1-area.y)))[j-area.x][rgb]
                -((cv::Vec3d*)(work_src.data+work_src.step.p[0]*(i-area.y)))[j-1-area.x][rgb]
                -((cv::Vec3d*)(work_src.data+work_src.step.p[0]*(i-area.y)))[j+1-area.x][rgb];

              if(mixture){
                  sum_trgt_grad[rgb]=
                     4*((cv::Vec3d*)(work_trgt.data+work_trgt.step.p[0]*i))[j][rgb]
                    -((cv::Vec3d*)(work_trgt.data+work_trgt.step.p[0]*(i-1)))[j][rgb]
                    -((cv::Vec3d*)(work_trgt.data+work_trgt.step.p[0]*(i+1)))[j][rgb]
                    -((cv::Vec3d*)(work_trgt.data+work_trgt.step.p[0]*i))[j-1][rgb]
                    -((cv::Vec3d*)(work_trgt.data+work_trgt.step.p[0]*i))[j+1][rgb];
                  sum1+=abs(sum_src[rgb]);
                  sum2+=abs(sum_trgt_grad[rgb]);
              }else{
                  newPixel[rgb] =omega*(sum_dst[rgb]+sum_src[rgb])/4.0;
                  if(newPixel[rgb]>255) newPixel[rgb]=255;
                  if(newPixel[rgb]<0) newPixel[rgb]=0;
                  error+=abs(((cv::Vec3d*)(work_dst.data+work_dst.step.p[0]*i))[j][rgb]-newPixel[rgb]);


                  ((cv::Vec3d*)(work_dst.data+work_dst.step.p[0]*i))[j][rgb]=newPixel[rgb];

              }

          }
          if(mixture){
              for(int rgb=0;rgb<3;rgb++){
                  newPixel[rgb] =omega*(sum_dst[rgb]+((sum1>sum2)?
                                                          sum_src[rgb]:sum_trgt_grad[rgb]))/4.0;
                  if(newPixel[rgb]>255) newPixel[rgb]=255;
                  if(newPixel[rgb]<0) newPixel[rgb]=0;
                  error+=abs(((cv::Vec3d*)(work_dst.data+work_dst.step.p[0]*i))[j][rgb]-newPixel[rgb]);


                  ((cv::Vec3d*)(work_dst.data+work_dst.step.p[0]*i))[j][rgb]=newPixel[rgb];


              }
          }
      }
    }
  }while( error>eps );
  for(int i=0;i<work_dst.rows;i++){
    for(int j=0;j<work_dst.cols;j++){
      dst_img.at<cv::Vec3b>(i,j)[0]=(unsigned char)abs(work_dst.at<cv::Vec3d>(i,j)[0]);
      dst_img.at<cv::Vec3b>(i,j)[1]=(unsigned char)abs(work_dst.at<cv::Vec3d>(i,j)[1]);
      dst_img.at<cv::Vec3b>(i,j)[2]=(unsigned char)abs(work_dst.at<cv::Vec3d>(i,j)[2]);
    }
  }
  return dst_img;
}

cv::Mat SimpleEditing(const cv::Mat &src_img,const cv::Mat &trgt_img,const cv::Mat &mask_img,const cv::Rect& area){
  cv::Mat dst_img=trgt_img.clone();
  for(int i=area.y;i<area.y+area.height;i++){
    for(int j=area.x;j<area.x+area.width;j++){

      if(mask_img.at<uchar>(i-area.y,j-area.x))continue;
      dst_img.at<cv::Vec3b>(i,j)=src_img.at<cv::Vec3b>(i-area.y,j-area.x);
    }
  }
  return dst_img;
}
