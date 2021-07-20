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
    * @brief 图片类型转化   cvMat/QImage/QPixmap
    */
    class ImageConversion
    {
        public:
        /** @brief cvMat转为QImage */
        QImage  cvMatToQImage( const cv::Mat &inMat );
        /** @brief cvMat转为QPixmap */
        QPixmap cvMatToQPixmap( const cv::Mat &inMat );
        /** 
        * @brief QImage转为cvMat 
        * attention
        *  输出格式可能为 1/3/4 通道，若想保存为视频，要转为三通道。  
        */
        cv::Mat QImageToCvMat( const QImage &inImage, bool inCloneImageData = true );
        /** 
        * @brief QPixmap转为cvMat 
        * attention
        *  输出格式可能为 1/3/4 通道，若想保存为视频，要转为三通道。  
        */
        cv::Mat QPixmapToCvMat( const QPixmap &inPixmap, bool inCloneImageData = true );
    };

}
#endif;