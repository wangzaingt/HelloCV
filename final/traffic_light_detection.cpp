#include <opencv2/opencv.hpp>
#include <iostream>

// 函数：调整视频大小并保存
void resizeAndSaveVideo(const std::string& inputPath, const std::string& outputPath, double scaleFactor) {
    cv::VideoCapture cap(inputPath);
    if (!cap.isOpened()) {
        std::cerr << "错误：无法打开视频文件 " << inputPath << std::endl;
        return;
    }

    int frameWidth = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    double fps = cap.get(cv::CAP_PROP_FPS);

    int newWidth = static_cast<int>(frameWidth * scaleFactor);
    int newHeight = static_cast<int>(frameHeight * scaleFactor);

    cv::VideoWriter writer(outputPath, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, cv::Size(newWidth, newHeight));
    if (!writer.isOpened()) {
        std::cerr << "错误：无法创建视频写入器 " << outputPath << std::endl;
        return;
    }

    cv::Mat frame;
    while (cap.read(frame)) {
        cv::Mat resizedFrame;
        cv::resize(frame, resizedFrame, cv::Size(newWidth, newHeight));
        writer.write(resizedFrame);
    }

    cap.release();
    writer.release();
    std::cout << "视频已成功缩小并保存到 " << outputPath << std::endl;
}

// 函数：处理特定颜色的边缘检测和圆形检测
void processColor(cv::Mat& frame, const std::string& colorName, const cv::Scalar& lowerBound, const cv::Scalar& upperBound,
                  std::vector<std::tuple<cv::Rect, std::string>>& detectedShapes) {
    // 1. 颜色遮罩
    cv::Mat hsvFrame;
    cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);

    cv::Mat mask;
    cv::inRange(hsvFrame, lowerBound, upperBound, mask);

    // 2. 形态学操作去除噪声
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

    // 3. Canny 边缘检测
    cv::Mat cannyOutput;
    cv::Canny(mask, cannyOutput, 50, 150);

    // 4. 高斯模糊并增厚边缘
    cv::GaussianBlur(cannyOutput, cannyOutput, cv::Size(5, 5), 0);
    cv::Mat thickenedEdges = cannyOutput;
    // 可以通过膨胀操作来增厚边缘，这里选择直接使用高斯模糊后的结果，因为它已经有一定平滑和增厚效果
    // cv::dilate(cannyOutput, thickenedEdges, cv::Mat(), cv::Point(-1,-1), 2);

    // 5. 查找轮廓
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thickenedEdges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // 6. 多边形检测并选择近似圆形
    for (const auto& contour : contours) {
        double area = cv::contourArea(contour);
        if (area<8500 && area>9600) continue; // 忽略小面积轮廓

        std::vector<cv::Point> approxCurve;
        cv::approxPolyDP(contour, approxCurve, cv::arcLength(contour, true) * 0.04, true);

        if (approxCurve.size() > 6 && approxCurve.size() < 15) { // 简单判断，圆形通常有较多边
            double perimeter = cv::arcLength(contour, true);
            if (perimeter == 0) continue;
            double circularity = 4 * CV_PI * area / (perimeter * perimeter);
            if (circularity > 0.6) { // 较高的圆形度
                cv::Rect boundingBox = cv::boundingRect(contour);
                detectedShapes.emplace_back(boundingBox, colorName);
            }
        }
    }
}

int main() {
    std::string inputVideoPath = "../traffic_light.mp4"; 
    std::string resizedVideoPath = "../file2.avi";
    std::string outputVideoPath = "../output_combined.avi";

    // --- 步骤 1：缩小视频并保存为 file2.avi ---
    double scaleFactor = 0.5; // 缩小为原来 1/2
    resizeAndSaveVideo(inputVideoPath, resizedVideoPath, scaleFactor);
    std::cout << "步骤 1 完成：视频已缩小并保存。" << std::endl;
    

    // --- 步骤 2 & 3 & 4 & 5 & 6：颜色遮罩、Canny、高斯模糊、多边形检测、圆形选择、绘制方框和文字 ---
    cv::VideoCapture capFile2(resizedVideoPath);
    if (!capFile2.isOpened()) {
        std::cerr << "错误：无法打开缩小后的视频文件 " << resizedVideoPath << std::endl;
        return -1;
    }

    int frameWidth = static_cast<int>(capFile2.get(cv::CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(capFile2.get(cv::CAP_PROP_FRAME_HEIGHT));
    double fps = capFile2.get(cv::CAP_PROP_FPS);

    cv::VideoWriter outputWriter(outputVideoPath, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, cv::Size(frameWidth, frameHeight));
    if (!outputWriter.isOpened()) {
        std::cerr << "错误：无法创建最终输出视频写入器 " << outputVideoPath << std::endl;
        return -1;
    }

    // 定义绿色和红色的 HSV 范围
    // 绿色（示例范围，根据实际视频调整）
    cv::Scalar lowerGreen = cv::Scalar(30, 100, 100);
    cv::Scalar upperGreen = cv::Scalar(90, 255, 255);

    // 红色（需要处理跨越 0 度的色相环）
    cv::Scalar lowerRed1 = cv::Scalar(100, 100, 100);
    cv::Scalar upperRed1 = cv::Scalar(179, 255, 255);
    cv::Scalar lowerRed2 = cv::Scalar(100, 100, 100);
    cv::Scalar upperRed2 = cv::Scalar(179, 255, 255);

    cv::Mat frame;
    int frameCount = 0;

    while (capFile2.read(frame)) {
        frameCount++;
        std::vector<std::tuple<cv::Rect, std::string>> detectedShapes;

        // 处理绿色
        processColor(frame, "green", lowerGreen, upperGreen, detectedShapes);

        // 处理红色（需要组合两个范围）
        cv::Mat hsvFrame;
        cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);
        cv::Mat maskRed1, maskRed2, maskRedCombined;
        cv::inRange(hsvFrame, lowerRed1, upperRed1, maskRed1);
        cv::inRange(hsvFrame, lowerRed2, upperRed2, maskRed2);
        cv::bitwise_or(maskRed1, maskRed2, maskRedCombined);

        // 形态学操作去除噪声
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
        cv::morphologyEx(maskRedCombined, maskRedCombined, cv::MORPH_OPEN, kernel);
        cv::morphologyEx(maskRedCombined, maskRedCombined, cv::MORPH_CLOSE, kernel);

        // Canny 边缘检测
        cv::Mat cannyOutputRed;
        cv::Canny(maskRedCombined, cannyOutputRed, 50, 150);

        // 高斯模糊并增厚边缘
        cv::GaussianBlur(cannyOutputRed, cannyOutputRed, cv::Size(5, 5), 0);

        // 查找轮廓
        std::vector<std::vector<cv::Point>> contoursRed;
        cv::findContours(cannyOutputRed, contoursRed, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // 多边形检测并选择近似圆形
        for (const auto& contour : contoursRed) {
            double area = cv::contourArea(contour);
            if (area < 500) continue; // 忽略小面积轮廓

            std::vector<cv::Point> approxCurve;
            cv::approxPolyDP(contour, approxCurve, cv::arcLength(contour, true) * 0.04, true);

            if (approxCurve.size() > 6 && approxCurve.size() < 15) { // 简单判断，圆形通常有较多边
                double perimeter = cv::arcLength(contour, true);
                if (perimeter == 0) continue;
                double circularity = 4 * CV_PI * area / (perimeter * perimeter);
                if (circularity > 0.6) { // 较高的圆形度
                    cv::Rect boundingBox = cv::boundingRect(contour);
                    detectedShapes.emplace_back(boundingBox, "red");
                }
            }
        }

        // 在原始帧上绘制结果
        for (const auto& shape : detectedShapes) {
            cv::Rect bbox = std::get<0>(shape);
            std::string colorText = std::get<1>(shape);

            cv::Scalar drawColor = (colorText == "green") ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255); // 绿色或红色

            cv::rectangle(frame, bbox, drawColor, 2);
            cv::putText(frame, colorText, cv::Point(bbox.x, bbox.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.7, drawColor, 2);
        }

        outputWriter.write(frame);

        // 可选：显示帧
        // cv::imshow("Processed Frame", frame);
        // if (cv::waitKey(1) == 'q') break;
    }

    capFile2.release();
    outputWriter.release();
    // cv::destroyAllWindows();

    std::cout << "步骤 2-6 完成：颜色识别、边缘检测、圆形检测和结果绘制已完成，并保存到 " << outputVideoPath << std::endl;
    std::cout << "所有操作完成！" << std::endl;

    return 0;
}
