#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_mul_thread_qt.h"
#include<QtWidgets/qprogressbar.h>
#include<QtWidgets/qpushbutton.h>
#include<QtCore/qstring.h>
#include<QtWidgets/qboxlayout.h>
#include<QtWidgets/qsplitter.h>
#include"work_thread.h"
#include<QtCore/qtimer.h>
#include<QtWidgets/qfiledialog.h>
#include<QtGui/qimage.h>
#include<QtMultimedia/qmediaplayer.h>
#include<QtMultimediaWidgets/qvideowidget.h>
#include<QtWidgets/qmessagebox.h>
#include<QtCore/qfile.h>
class mul_thread_qt : public QMainWindow
{
    Q_OBJECT

public:
    mul_thread_qt(QWidget *parent = Q_NULLPTR);
    QImage Mat2QImage(const cv::Mat& src);
public Q_SLOTS:
    void slotStart();
    void slotStop();
    void ontimer();
    void show_inputFile_dialog();
    void show_outputFolder_dialog();
    void startPlayVideo();
    void stopPlayVideo();
    void set_thread_num(int num);
    void set_blur_param(int d);
    void set_kmean_param(int k);
    void start_demo();
    void start_process();
private:
    Ui::mul_thread_qtClass ui;
    QTimer* timer;
    std::string inputVideo_path;
    std::string outputFolder_path;
    QMediaPlayer* video_play;
    QVideoWidget* video_play_widget;

    std::vector<work_thread*> workThread;
    cv::Mat demo;
    int video_frame_size{0};
    bool thread_start{ false };
    bool thread_stop{ false };
    int THREAD_NUM{1};
    int fps{ 0 };
    cv::Size frame_size;
    bilateralFilter_param blur_param;
    int k_cluster;
};
