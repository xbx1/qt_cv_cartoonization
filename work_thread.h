#ifndef WORK_THREAD_H
#define WORK_THREAD_H
#include<iostream>
#include<chrono>
#include<QtCore/qthread.h>
#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
struct bilateralFilter_param {
	int d;
	double sigmaColor;
	double sigmaSpace;
	//int boardType;
	//void bilateralFilter(InputArray src, OutputArray dst, int d, double sigmaColor, douboe sigmaSpace, int borderType=BORDER_DEFAULT)
	bilateralFilter_param() :d(25), 
							sigmaColor(25 *2),
							sigmaSpace(25 /2) {

	}
};
struct canny_param {
	double thres1;
	double thres2;
	int apetureSize;
	bool L2gradient;
	//void Canny(InputArray image, OutputArray edges, double threshold1, double threshold2, int apertureSize=3, bool L2gradient=false)
	canny_param() :thres1(100), thres2(200), apetureSize(3), L2gradient(false) {

	}
};
class work_thread  :public QThread {
	Q_OBJECT
public:
	work_thread();
	~work_thread();
	bilateralFilter_param blur_param;
	canny_param edge_param;
	int k_cluster;
	void set_input_vec(std::vector<cv::Mat>& image_vec);
	std::vector<cv::Mat> get_result();
	bool is_finish();
	void img_pro_blur(const cv::Mat& src,cv::Mat& dst);
	void img_pro_canny(const cv::Mat& src,cv::Mat& dst);
	void img_pro_kmean(const cv::Mat& src,cv::Mat& dst);
	void img_pro_contour(const cv::Mat& edge, const cv::Mat& src, cv::Mat& dst);
	int get_cnt();
protected:
	void run();
private:
	int cnt{ 0 };
	int buf_size{0};
	int process_frame{ 0 };
	bool finish{false};
	std::vector<cv::Mat> image_buf;
	std::vector<cv::Mat> result_vec;
	
};
#endif