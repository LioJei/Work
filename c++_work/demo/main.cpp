#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    // 检查OpenCV库是否正确加载
    if (!cv::haveImageReader("../lf.jpg")) {
        std::cerr << "OpenCV cannot read this format or file does not exist: ../lf.jpg" << std::endl;
        return -1;
    }

    cv::Mat image = cv::imread("../lf.jpg");
    if (image.empty()) {
        std::cerr << "Could not read the image: ../lf.jpg" << std::endl;
        return -1;
    }

    int width = image.cols;
    int height = image.rows;
    std::cout << "Image width: " << width << std::endl;
    std::cout << "Image height: " << height << std::endl;
    int channels = image.channels();
    std::cout << "Image channels: " << channels << std::endl;
    cv::Vec3b pixel = image.at<cv::Vec3b>(100, 100); // 访问(x, y)处的像素值
    std::cout << "Pixel value at (100, 100): " << (int)pixel[0] << ", " << (int)pixel[1] << ", " << (int)pixel[2] << std::endl;
    cv::Point2f center(static_cast<float>(image.cols / 2.0), static_cast<float>(image.rows / 2.0));
    cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, 60, 1.0);
    cv::Mat rotatedImage;
    cv::warpAffine(image, rotatedImage, rotationMatrix, image.size());
    cv::Mat flippedImage;
    cv::flip(image, flippedImage, 1); // 1表示水平翻转，0表示垂直翻转
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    cv::Mat hsvImage;
    cv::cvtColor(image, hsvImage, cv::COLOR_BGR2HSV);

    try {
        cv::imshow("Image", hsvImage);
        cv::waitKey(0);
    } catch (const cv::Exception& e) {
        std::cerr << "Error displaying or waiting for key: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
