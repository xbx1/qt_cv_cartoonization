#include "mul_thread_qt.h"
#include"work_thread.h"
#include "stdafx.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mul_thread_qt w;
    w.show();
    return a.exec();
    //cv::Mat image = cv::imread("C:/Users/25830/Pictures/Saved Pictures/4.jpg",cv::IMREAD_COLOR);
    //cv::Mat blur;
    //work_thread().img_pro_blur(image, blur);
    //
    //cv::Mat edge;
    //work_thread().img_pro_canny(blur,edge);

    //cv::Mat kmean;
    //work_thread().img_pro_kmean(blur,kmean);

    //cv::Mat dst;
    //work_thread().img_pro_contour(edge, kmean, dst);

    //cv::Mat dst_src;
    //std::vector<cv::Mat> src_concat;
    //cv::resize(image,image,cv::Size(int(image.cols/4), int(image.rows/4)));
    //cv::resize(dst, dst, cv::Size(int(dst.cols / 4), int(dst.rows / 4)));
    //src_concat.push_back(image);
    //src_concat.push_back(dst);
    //cv::hconcat(image,dst,dst_src);
    //cv::imshow("blur", dst_src);
    //cv::waitKey(0);
}
