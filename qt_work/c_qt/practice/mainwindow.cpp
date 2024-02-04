//
// Created by Lio on 2022/10/24.
//

#include "mainwindow.h"
#include "OpenGLWidget.h"

MainWindow::MainWindow(QMainWindow *parent) : QMainWindow(parent) {
    QWidget *mainWidget = new QWidget(this);
    this->setCentralWidget(mainWidget);
    openCVBtn = new QPushButton("openCVTest");
    openCVBtn->resize(200,100);
    openALBtn = new QPushButton("openALTesT");
    openGLBtn = new QPushButton("openGLTesT");
    quitBtn = new QPushButton("quit");
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->setSpacing(50);
    mainLayout->addWidget(openCVBtn);
    mainLayout->addWidget(openALBtn);
    mainLayout->addWidget(openGLBtn);
    mainLayout->addWidget(quitBtn);
    mainWidget->setLayout(mainLayout);
    connect(openCVBtn, &QPushButton::clicked, this, &MainWindow::on_openCVBtn_clicked);
    connect(openALBtn, &QPushButton::clicked, this, &MainWindow::on_openALBtn_clicked);
    connect(openGLBtn, &QPushButton::clicked, this, &MainWindow::on_openGLBtn_clicked);
    connect(quitBtn, &QPushButton::clicked, this, &MainWindow::close);
}

MainWindow::~MainWindow() {

}

void MainWindow::on_openCVBtn_clicked(){
    // Load an image
    cv::Mat image = cv::imread("../resourcelist/image.jpg");

    // Convert the image to a QImage
    QImage qImage = QImage((const unsigned char*)image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);

    // Display the image in a label
    QLabel *label = new QLabel;
    label->setPixmap(QPixmap::fromImage(qImage));
    QPushButton *qBtn = new QPushButton("quit");
    // Add the label to the layout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    layout->addWidget(qBtn);

    // Set the layout as the central widget
    QWidget *widget = new QWidget;
    connect(qBtn,&QPushButton::clicked, widget, &QWidget::close);
    widget->setLayout(layout);
    widget->show();

//    cv::VideoCapture cap(0);
//    cv::Mat frame;
//    QImage qimage;
//    QLabel label;
//    while (true) {
//        cap >> frame;
//        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
//        qimage = QImage(frame.data, frame.cols, frame.rows, QImage::Format_RGB888);
//        label.setPixmap(QPixmap::fromImage(qimage));
//        label.show();
//        cv::waitKey(1);
//    }
}

void MainWindow::on_openALBtn_clicked(){
    // 初始化OpenAL
    alutInit(NULL, NULL);
    // 创建OpenAL设备
    ALCdevice *device = alcOpenDevice(NULL);
    if (device) {
        // 创建上下文
        ALCcontext *context = alcCreateContext(device, NULL);
        alcMakeContextCurrent(context);
        // 加载wav文件
        ALuint buffer;
        alGenBuffers(1, &buffer);
        ALenum error;
        ALuint source;
        source = alutCreateBufferFromFile("example.wav");
        alGenSources(1, &source);
        alSourcei(source, AL_BUFFER, buffer);
        // 播放音频
        alSourcePlay(source);
        // 等待音频播放完成
        ALint state = AL_PLAYING;
        while (state == AL_PLAYING) {
            alGetSourcei(source, AL_SOURCE_STATE, &state);
        }
        // 清理资源
        alDeleteSources(1, &source);
        alDeleteBuffers(1, &buffer);
        // 关闭上下文和设备
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context);
        alcCloseDevice(device);
    }else {
        qDebug()<<"error!!!";
    }
    // 卸载OpenAL
    alutExit();
}

void MainWindow::on_openGLBtn_clicked(){

}