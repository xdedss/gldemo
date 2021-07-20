#ifndef QTTOOPENCV_H
#define QTTOOPENCV_H

#include <QImage>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"

namespace QtToOpencv
{
    /**
    * @class ImageConversion
    * @brief ͼƬ����ת��   cvMat/QImage/QPixmap
    */
    class ImageConversion
    {
        public:
        /** @brief cvMatתΪQImage */
        QImage  cvMatToQImage( const cv::Mat &inMat );
        /** @brief cvMatתΪQPixmap */
        QPixmap cvMatToQPixmap( const cv::Mat &inMat );
        /** 
        * @brief QImageתΪcvMat 
        * attention
        *  �����ʽ����Ϊ 1/3/4 ͨ�������뱣��Ϊ��Ƶ��ҪתΪ��ͨ����  
        */
        cv::Mat QImageToCvMat( const QImage &inImage, bool inCloneImageData = true );
        /** 
        * @brief QPixmapתΪcvMat 
        * attention
        *  �����ʽ����Ϊ 1/3/4 ͨ�������뱣��Ϊ��Ƶ��ҪתΪ��ͨ����  
        */
        cv::Mat QPixmapToCvMat( const QPixmap &inPixmap, bool inCloneImageData = true );
    };

}
#endif;