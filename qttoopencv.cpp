#include <QDebug>
#include <QImage>
#include <QPixmap>
#include "qttoopencv.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"



namespace QtToOpencv
{
   // NOTE: This does not cover all cases - it should be easy to add new ones as required.
	QImage  ImageConversion::cvMatToQImage( const cv::Mat &inMat )
   {
      switch ( inMat.type() )
      {
         // 8-bit, 4 channel
         case CV_8UC4:
         {
            QImage image( inMat.data,
                          inMat.cols, inMat.rows,
                          static_cast<int>(inMat.step),
                          QImage::Format_ARGB32 );

            return image;
         }

         // 8-bit, 3 channel
         case CV_8UC3:
         {
            QImage image( inMat.data,
                          inMat.cols, inMat.rows,
                          static_cast<int>(inMat.step),
                          QImage::Format_RGB888 );

            return image.rgbSwapped();
         }

         // 8-bit, 1 channel
         case CV_8UC1:
         {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
            QImage image( inMat.data,
                          inMat.cols, inMat.rows,
                          static_cast<int>(inMat.step),
                          QImage::Format_Grayscale8 );
#else
            static QVector<QRgb>  sColorTable;

            // only create our color table the first time
            if ( sColorTable.isEmpty() )
            {
               sColorTable.resize( 256 );

               for ( int i = 0; i < 256; ++i )
               {
                  sColorTable[i] = qRgb( i, i, i );
               }
            }

            QImage image( inMat.data,
                          inMat.cols, inMat.rows,
                          static_cast<int>(inMat.step),
                          QImage::Format_Indexed8 );

            image.setColorTable( sColorTable );
#endif

            return image;
         }

         default:
            qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
            break;
      }

      return QImage();
   }


   QPixmap ImageConversion::cvMatToQPixmap( const cv::Mat &inMat )
   {
      return QPixmap::fromImage( cvMatToQImage( inMat ) );
   }


   cv::Mat ImageConversion::QImageToCvMat(const QImage &inImage, bool inCloneImageData)
   {
	   switch (inImage.format())
	   {
		   // 8-bit, 4 channel
	   case QImage::Format_ARGB32:
	   case QImage::Format_ARGB32_Premultiplied:
	   {
		   cv::Mat  mat(inImage.height(), inImage.width(),
			   CV_8UC4,
			   const_cast<uchar*>(inImage.bits()),
			   static_cast<size_t>(inImage.bytesPerLine())
		   );

		   return (inCloneImageData ? mat.clone() : mat);
	   }

	   // 8-bit, 3 channel
	   case QImage::Format_RGB32:
	   case QImage::Format_RGB888:
	   {
		   if (!inCloneImageData)
		   {
			   qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning because we use a temporary QImage";
		   }

		   QImage   swapped = inImage;

		   if (inImage.format() == QImage::Format_RGB32)
		   {
			   swapped = swapped.convertToFormat(QImage::Format_RGB888);
		   }

		   swapped = swapped.rgbSwapped();

		   return cv::Mat(swapped.height(), swapped.width(),
			   CV_8UC3,
			   const_cast<uchar*>(swapped.bits()),
			   static_cast<size_t>(swapped.bytesPerLine())
		   ).clone();
	   }

	   // 8-bit, 1 channel
	   case QImage::Format_Indexed8:
	   {
		   cv::Mat  mat(inImage.height(), inImage.width(),
			   CV_8UC1,
			   const_cast<uchar*>(inImage.bits()),
			   static_cast<size_t>(inImage.bytesPerLine())
		   );

		   return (inCloneImageData ? mat.clone() : mat);
	   }

	   default:
		   qWarning() << "ASM::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
		   break;
	   }

	   return cv::Mat();
   }


    //inline 表示该函数是内联函数 就是用 用函数内部的东西代替该函数名，这样可以节省 栈空间
   cv::Mat ImageConversion::QPixmapToCvMat( const QPixmap &inPixmap, bool inCloneImageData )
   {
      return QImageToCvMat( inPixmap.toImage(), inCloneImageData );
   }
}
