#include "mul_thread_qt.h"
#include "stdafx.h"
QImage mul_thread_qt::Mat2QImage(const cv::Mat& src) {
    QImage dest(src.cols, src.rows, QImage::Format_ARGB32);

    const float scale = 255.0;

    if (src.depth() == CV_8U) {
        if (src.channels() == 1) {
            for (int i = 0; i < src.rows; ++i) {
                for (int j = 0; j < src.cols; ++j) {
                    int level = src.at<quint8>(i, j);
                    dest.setPixel(j, i, qRgb(level, level, level));
                }
            }
        }
        else if (src.channels() == 3) {
            for (int i = 0; i < src.rows; ++i) {
                for (int j = 0; j < src.cols; ++j) {
                    cv::Vec3b bgr = src.at<cv::Vec3b>(i, j);
                    dest.setPixel(j, i, qRgb(bgr[2], bgr[1], bgr[0]));
                }
            }
        }
    }
    else if (src.depth() == CV_32F) {
        if (src.channels() == 1) {
            for (int i = 0; i < src.rows; ++i) {
                for (int j = 0; j < src.cols; ++j) {
                    int level = scale * src.at<float>(i, j);
                    dest.setPixel(j, i, qRgb(level, level, level));
                }
            }
        }
        else if (src.channels() == 3) {
            for (int i = 0; i < src.rows; ++i) {
                for (int j = 0; j < src.cols; ++j) {
                    cv::Vec3f bgr = scale * src.at<cv::Vec3f>(i, j);
                    dest.setPixel(j, i, qRgb(bgr[2], bgr[1], bgr[0]));
                }
            }
        }
    }

    return dest;
}
mul_thread_qt::mul_thread_qt(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    
    video_play = new QMediaPlayer;
    video_play_widget = new QVideoWidget;

    timer = new QTimer(this);
    //timer->start(33);

    connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(show_inputFile_dialog()));
    connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(show_outputFolder_dialog()));
    connect(timer,SIGNAL(timeout()),this,SLOT(ontimer()));
    connect(ui.listWidget,SIGNAL(currentRowChanged(int)),ui.stackedWidget,SLOT(setCurrentIndex(int)));
    connect(ui.startPlaybut,SIGNAL(clicked()),this,SLOT(startPlayVideo()));
    connect(ui.stopPlaybut, SIGNAL(clicked()), this, SLOT(stopPlayVideo()));
    connect(ui.spinBox_3,SIGNAL(valueChanged(int)),this,SLOT(set_thread_num(int)));
    connect(ui.spinBox, SIGNAL(valueChanged(int)), this, SLOT(set_blur_param(int)));
    connect(ui.spinBox_2, SIGNAL(valueChanged(int)), this, SLOT(set_kmean_param(int)));
    connect(ui.pushButton_3,SIGNAL(clicked()),this,SLOT(start_demo()));
    connect(ui.pushButton_4,SIGNAL(clicked()),this,SLOT(start_process()));
}

void mul_thread_qt::start_demo() {
    int ui_width = ui.label_3->size().width();
    int ui_height = ui.label_3->size().height();

    std::vector<cv::Mat> vec;
    work_thread object;

    object.blur_param = blur_param;
    object.k_cluster = k_cluster;

    cv::Mat blur,edge,kmean,dst;
    object.img_pro_blur(demo,blur);
    object.img_pro_canny(demo,edge);
    object.img_pro_kmean(blur,kmean);
    object.img_pro_contour(edge,kmean,dst);

    cv::Mat dst_src;
    cv::hconcat(demo,dst,dst_src);

    cv::cvtColor(dst_src, dst_src,cv::COLOR_BGR2RGB);

    QImage display_image = QImage((const unsigned char*)(dst_src.data), dst_src.cols, dst_src.rows, QImage::Format_RGB888);
    ui.label_3->setPixmap(QPixmap::fromImage(display_image.scaled(ui.label_3->size(), Qt::KeepAspectRatio)));
}
void mul_thread_qt::set_thread_num(int num) {
    THREAD_NUM = num;
}
void mul_thread_qt::start_process() {
    cv::VideoCapture cap;
    cap.open(inputVideo_path);
    std::vector<cv::Mat> vec;
    
    while (true) {
        cv::Mat image_stream;
        cap >> image_stream;
        if (image_stream.empty())
            break;
        vec.push_back(image_stream);
        /*cv::imshow("image",image_stream);
        if (cv::waitKey(100) == 27)
            break;*/
    }
    /*for (auto image : vec) {
        cv::imshow("image", image);
        cv::waitKey(100);
    }*/
    //for (int i = 0; i<int(vec.size()); i++) {
    //    cv::imshow("image",vec.at(i));
    //    if (cv::waitKey(100) == 27) {
    //        break;
    //    }
    //}
    fps = cap.get(cv::CAP_PROP_FPS);
    frame_size = cv::Size(cap.get(cv::CAP_PROP_FRAME_WIDTH), cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    cap.release();
    std::vector<cv::Mat> *image_vec_thread = new std::vector<cv::Mat>[THREAD_NUM];
    int length = int(vec.size());
    video_frame_size = length;
    int seg_length = length / THREAD_NUM;
    for (int i = 0; i < THREAD_NUM; i++) {
        if (i == THREAD_NUM - 1) {
            int num = length - seg_length * (THREAD_NUM - 1);
            int start = seg_length*(THREAD_NUM-1);
            for (int j = 0; j < num; j++) {
                image_vec_thread[THREAD_NUM - 1].push_back(vec.at(start+j));
            }
        }
        else {
            int start_ = seg_length * i;
            for (int j = 0; j < seg_length; j++) {
                image_vec_thread[i].push_back(vec.at(start_+j));
            }
        }
    }
    //QMessageBox::information(NULL, "Title", "done!", QMessageBox::Ok, QMessageBox::Ok);
    for (int i = 0; i < THREAD_NUM; i++) {
        work_thread* temp(new work_thread());
        temp->blur_param = blur_param;
        temp->k_cluster = k_cluster;
        temp->set_input_vec(image_vec_thread[i]);
        workThread.push_back(temp);
    }
    for (int i = 0; i < THREAD_NUM; i++) {
        workThread.at(i)->start();
    }
    thread_start = true;
    timer->start(1000);
    delete []image_vec_thread;
    //QMessageBox::information(NULL, "Title", "timer on", QMessageBox::Ok, QMessageBox::Ok);
}
void mul_thread_qt::set_blur_param(int d) {
    bilateralFilter_param temp;
    temp.d = d;
    temp.sigmaColor = d * 2;
    temp.sigmaSpace = d * 2;
    blur_param = temp;
}
void mul_thread_qt::set_kmean_param(int k) {
    k_cluster = k;
}
void mul_thread_qt::startPlayVideo() {
    if (video_play != nullptr) {
        video_play->play();
    }
}
void mul_thread_qt::stopPlayVideo() {
    if (video_play != nullptr) {
        video_play->pause();
    }
}
void mul_thread_qt::show_inputFile_dialog() {
    QString qtString = QFileDialog::getOpenFileName(this,"select video file","/","video file(*.mp4)");
    ui.lineEdit->setText(qtString);
    inputVideo_path = qtString.toStdString();

    //QFile file(qtString);
    //if (!file.open(QIODevice::ReadOnly)) {
    //    QMessageBox::information(NULL, "Title", "open file failed", QMessageBox::Ok, QMessageBox::Ok);
    //}
    //else
    //    QMessageBox::information(NULL, "Title", "open file successfully", QMessageBox::Ok, QMessageBox::Ok);
    

    //video_play_widget->resize(ui.label_6->size());
    
    video_play_widget->setAspectRatioMode(Qt::IgnoreAspectRatio);
    //video_play_widget->setFullScreen(true);
    
    video_play->setMedia(QUrl::fromLocalFile(qtString));
    video_play->setVideoOutput(video_play_widget);
    ui.gridLayout_3->addWidget(video_play_widget);

    cv::VideoCapture cap;

    cap.open(inputVideo_path);

    if (cap.isOpened()) {
        cap >> demo;
        /*cv::cvtColor(image_cv,image_cv,cv::COLOR_BGR2RGB);

        QImage display_image = QImage((const unsigned char*)(image_cv.data), image_cv.cols, image_cv.rows, QImage::Format_RGB888);
        ui.label_3->setPixmap(QPixmap::fromImage(display_image.scaled(ui.label_3->size(), Qt::KeepAspectRatio)));*/
        cap.release();
    }
    //video_play->play();

    
    //video_play_widget->show();
    
}
void mul_thread_qt::show_outputFolder_dialog() {
    QString qtString = QFileDialog::getExistingDirectory(this, "select output folder", "./");
    ui.lineEdit_2->setText(qtString);
    outputFolder_path = qtString.toStdString();
}
void mul_thread_qt::ontimer() {
    bool is_finish = true;
    for (int i = 0; i < THREAD_NUM; i++) {
        if (!workThread.at(i)->is_finish()) {
            is_finish = false;
            break;
        }
    }
    int process_frame = 0;
    for (int i = 0; i < THREAD_NUM; i++) {
        process_frame = process_frame +workThread.at(i)->get_cnt();
    }
    ui.progressBar->setValue(float(process_frame)/float(video_frame_size)*100);
    if (is_finish) {
        QMessageBox::information(NULL, "Title", "finish translating", QMessageBox::Ok, QMessageBox::Ok);
        cv::VideoWriter video_writter=cv::VideoWriter(outputFolder_path + "/out.avi", cv::VideoWriter::fourcc('M', 'P', '4', '2'), fps, frame_size, true);
        for (int i = 0; i < THREAD_NUM; i++) {
            std::vector<cv::Mat> image_vec_temp= workThread.at(i)->get_result();
            for (int index = 0; index<int(image_vec_temp.size());index++) {
                video_writter.write(image_vec_temp.at(index));
                
            }
            workThread.at(i)->terminate();
            workThread.at(i)->wait();
            delete workThread.at(i);
        }
        video_writter.release();
        timer->stop();
    }
    
}
void mul_thread_qt::slotStart() {
    //workThread = new work_thread();
    //workThread->start();
}
void mul_thread_qt::slotStop() {
    //workThread->terminate();
    //workThread->wait();
    //thread_stop = true;
}
