#include"work_thread.h"

work_thread::work_thread() {
	
}
void work_thread::set_input_vec(std::vector<cv::Mat>& image_vec) {
	for (auto image : image_vec) {
		image_buf.push_back(image);
	}
	buf_size = int(image_vec.size());
}
work_thread::~work_thread() {

}
void work_thread::img_pro_blur(const cv::Mat& src, cv::Mat& dst) {
	cv::Mat src_,r,g,b,r_,g_,b_;
	src.copyTo(src_);
	std::vector<cv::Mat> channels;
	cv::split(src_,channels);
	b = channels.at(0);
	g = channels.at(1);
	r = channels.at(2);
	cv::bilateralFilter(b, b_, blur_param.d, blur_param.sigmaColor, blur_param.sigmaSpace, cv::BORDER_DEFAULT);
	cv::bilateralFilter(g, g_, blur_param.d, blur_param.sigmaColor, blur_param.sigmaSpace, cv::BORDER_DEFAULT);
	cv::bilateralFilter(r, r_, blur_param.d, blur_param.sigmaColor, blur_param.sigmaSpace, cv::BORDER_DEFAULT);

	channels.clear();
	channels.push_back(b_);
	channels.push_back(g_);
	channels.push_back(r_);
	cv::merge(channels,dst);
}
void work_thread::img_pro_contour(const cv::Mat& edge,const cv::Mat& src,cv::Mat& dst) {
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(edge, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point());
	//dst = cv::Mat::zeros(edge.size(), CV_8UC1);
	src.copyTo(dst);
	cv::Mat Contours = cv::Mat::zeros(edge.size(), CV_8UC1);  //绘制
	for (int i = 0; i < contours.size(); i++)
	{
		//contours[i]代表的是第i个轮廓，contours[i].size()代表的是第i个轮廓上所有的像素点数
		for (int j = 0; j < contours[i].size(); j++)
		{
			//绘制出contours向量内所有的像素点
			cv::Point P = cv::Point(contours[i][j].x, contours[i][j].y);
			Contours.at<uchar>(P) = 255;
		}

		//绘制轮廓
		cv::drawContours(dst, contours, i, cv::Scalar(0,0,0), 1, 8, hierarchy);
	}
	cv::Mat b, g, r,b_,g_,r_;
	std::vector<cv::Mat> channels;

	cv::split(dst,channels);
	b = channels.at(0);
	g = channels.at(1);
	r = channels.at(2);

	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
	cv::erode(b, b_, element);
	cv::erode(g, g_, element);
	cv::erode(r, r_, element);

	channels.clear();
	channels.push_back(b_);
	channels.push_back(g_);
	channels.push_back(r_);

	cv::merge(channels,dst);
}
void work_thread::img_pro_canny(const cv::Mat& src, cv::Mat& dst) {
	cv::Mat src_;
	src.copyTo(src_);
	cv::Canny(src_,dst,edge_param.thres1,edge_param.thres2,edge_param.apetureSize,edge_param.L2gradient);
}
void work_thread::img_pro_kmean(const cv::Mat& src, cv::Mat& dst) {
	int width = src.cols;
	int height = src.rows;

	//init
	cv::Mat src_;
	cv::cvtColor(src,src_,cv::COLOR_BGR2HSV);
	int sampleCount = width * height;
	cv::Mat labels;//Input/output integer array that stores the cluster indices for every sample
	cv::Mat centers;//Output matrix of the cluster centers, one row per each cluster center.

	// convert image to kmeans data
	cv::Mat sampleData = src_.reshape(3, sampleCount);//every pixel is a sample
	cv::Mat data;
	sampleData.convertTo(data, CV_32F);

	//K-Means
	cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 5, 0.1);
	cv::kmeans(data, k_cluster, labels, criteria, k_cluster, cv::KMEANS_PP_CENTERS, centers);

	//create a color map
	std::vector<cv::Scalar> colorMaps;
	uchar h, s, v;
	//clusterCount is equal to centers.rows
	for (int i = 0; i < centers.rows; i++)
	{
		h = (uchar)centers.at<float>(i, 0);
		s = (uchar)centers.at<float>(i, 1);
		v = (uchar)centers.at<float>(i, 2);
		colorMaps.push_back(cv::Scalar(h, s, v));
	}
	// Show  result
	int index = 0;
	dst = cv::Mat::zeros(src_.size(), src_.type());
	uchar* ptr = NULL;
	int* label = NULL;
	for (int row = 0; row < height; row++) {
		ptr = dst.ptr<uchar>(row);
		for (int col = 0; col < width; col++) {
			index = row * width + col;
			label = labels.ptr<int>(index);
			*(ptr + col * 3) = colorMaps[*label][0];
			*(ptr + col * 3 + 1) = colorMaps[*label][1];
			*(ptr + col * 3 + 2) = colorMaps[*label][2];
		}
	}
	cv::cvtColor(dst,dst,cv::COLOR_HSV2BGR);
}
std::vector<cv::Mat> work_thread::get_result() {
	if (finish) {
		return result_vec;
	}
}
bool work_thread::is_finish() {
	return finish;
}
void work_thread::run() {
	for (auto image : image_buf) {
		cv::Mat edge;
		cv::Mat blur;
		cv::Mat kmean;
		cv::Mat dst;
		img_pro_blur(image,blur);
		img_pro_canny(blur,edge);
		img_pro_kmean(blur,kmean);
		img_pro_contour(edge,kmean,dst);
		result_vec.push_back(dst);
		process_frame++;
	}
	finish = true;
}
int work_thread::get_cnt() {
	return process_frame;
}