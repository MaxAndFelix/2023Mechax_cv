//
// Created by xixiliadorabarry on 1/24/19.
//
#include "energy/energy.h"
#include "log.h"

using namespace cv;
using std::cout;
using std::endl;
using std::vector;

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于寻找图像内所有的大风车扇叶
// ---------------------------------------------------------------------------------------------------------------------
int Energy::findFans(const cv::Mat &src)
{
    if (src.empty())
    {
        if (show_info)
            cout << "empty!" << endl;
        return 0;
    }
    static Mat src_bin;
    src_bin = src.clone();
    if (src.type() == CV_8UC3)
    {
        cvtColor(src_bin, src_bin, CV_BGR2GRAY); // 若读取三通道视频文件，需转换为单通道
    }
    std::vector<vector<Point>> fan_contours;
    FanStruct(src_bin); // 图像膨胀，防止图像断开并更方便寻src找
    if (show_process)
        imshow("fan struct", src_bin);
    findContours(src_bin, fan_contours, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
    for (auto &fan_contour : fan_contours)
    {
        cout<<fan_contours.size()<<endl;
        if (!isValidFanContour(src_bin, fan_contour))
        {
            continue;
        }
        fans.emplace_back(cv::minAreaRect(fan_contour));
    }
    if (fans.size() < last_fans_cnt)
    {
        last_fans_cnt = static_cast<int>(fans.size());
        return -1; // 寻找到的扇叶比上一帧少，说明该帧有误，返回-1
    }
    last_fans_cnt = static_cast<int>(fans.size());
    return static_cast<int>(fans.size());
}

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于寻找图像内所有的大风车模块
// ---------------------------------------------------------------------------------------------------------------------
int Energy::findHitpoint(const cv::Mat &src)
{
    if (src.empty())
    {
        if (show_info)
            cout << "empty!" << endl;
        return 0;
    }
    static Mat src_bin;
    src_bin = src.clone();
    if (src.type() == CV_8UC3)
        cvtColor(src_bin, src_bin, CV_BGR2GRAY); // 若读取三通道视频文件，需转换为单通道

    if (mcu_data.enemy_color == ENEMY_BLUE)
        threshold(src_bin, src_bin, energy_part_param_.BLUE_GRAY_THRESH, 255, THRESH_BINARY);
    else if(mcu_data.enemy_color == ENEMY_RED)
        threshold(src_bin, src_bin, energy_part_param_.RED_GRAY_THRESH, 255, THRESH_BINARY);
    
   
    // ArmorStruct(src_bin);

    imwrite(PROJECT_DIR "/images/raw.jpg",src_bin);

    Mat templateImage = cv::imread(PROJECT_DIR "/images/tem.jpg");
    if (templateImage.type() == CV_8UC3)
    {
        cvtColor(templateImage, templateImage, CV_BGR2GRAY); // 若读取三通道视频文件，需转换为单通道
    }

    // imshow("template",templateImage);

    // 调用 matchTemplate 函数计算匹配度
    Mat matchResult;
    matchTemplate(src_bin, templateImage, matchResult, CV_TM_CCOEFF_NORMED);

    // 查找匹配度最高的位置
    double minValue, maxValue;
    Point minLocation, maxLocation;
    minMaxLoc(matchResult, &minValue, &maxValue, &minLocation, &maxLocation);
    // 绘制矩形框标注匹配位置
    rectangle(src, maxLocation, Point(maxLocation.x + templateImage.cols, maxLocation.y + templateImage.rows), Scalar(0, 255, 0), 2);
    std::vector<cv::Point2f> points;
    
    points.push_back(cv::Point2f(maxLocation.x, maxLocation.y));
    points.push_back(cv::Point2f(maxLocation.x+templateImage.cols, maxLocation.y));
    points.push_back(cv::Point2f(maxLocation.x, maxLocation.y+templateImage.rows));
    points.push_back(cv::Point2f(maxLocation.x+templateImage.cols, maxLocation.y+templateImage.rows));
    armors.emplace_back(cv::minAreaRect(points));
    target_armors.emplace_back(cv::minAreaRect(points));
    for (int i = 0; i < target_armors.size(); ++i)
    {
        target_armor = target_armors.at(i);
        target_point = target_armor.center;
    }
    // cout<<target_point<<endl;
    // RotatedRect rotatedRect = minAreaRect(points);
    // Point2f rectPoints[4];
    // rotatedRect.points(rectPoints);
    // for(int j=0;j< 4;j++)
    // {
    //     cout<<rectPoints[j]<<endl;
    // }
    // cout<<"****"<<endl;
    // 显示结果图像
    imshow("eeeeee", src);
    return static_cast<int>(armors.size());
}

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于寻找图像内大风车中心字母“R”
// ---------------------------------------------------------------------------------------------------------------------
bool Energy::findCenterR(const cv::Mat &src)
{
    if (src.empty())
    {
        if (show_info)
            cout << "empty!" << endl;
        return false;
    }
    static Mat src_bin;
    src_bin = src.clone();
    if (src.type() == CV_8UC3)
    {
        cvtColor(src_bin, src_bin, CV_BGR2GRAY);
    }
    std::vector<vector<Point>> center_R_contours;
    CenterRStruct(src_bin);
    findContours(src_bin, center_R_contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    // if (show_process)
    //     imshow("R struct raw", src_bin);
    //drawContours(src_bin, center_R_contours, -1, Scalar(255, 0, 0), 2);
    for (auto &center_R_contour : center_R_contours)
    {
        if (!isValidCenterRContour(center_R_contour))
        {
            continue;
        }
        centerR = cv::minAreaRect(center_R_contour);
        // if (show_process)
        //     imshow("R struct", src_bin);
        float target_length =
            target_armor.size.height > target_armor.size.width ? target_armor.size.height : target_armor.size.width;
        circle_center_point = centerR.center;
        circle_center_point.y += target_length / 7.5; // 实际最小二乘得到的中心在R的下方
        return true;
    }
    if (show_info)
        cout << "find center R false!" << endl;
    return false;
}

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于判断找到的矩形候选区是否为含流动条的扇叶
// ---------------------------------------------------------------------------------------------------------------------
bool Energy::findFlowStripFan(const cv::Mat &src)
{
    if (src.empty())
    {
        if (show_info)
            cout << "empty!" << endl;
        return false;
    }
    static Mat src_bin;
    static Mat src_copy;
    src_bin = src.clone();
    src_copy = src.clone();
    if (src.type() == CV_8UC3)
    {
        cvtColor(src_bin, src_bin, CV_BGR2GRAY); // 若读取三通道视频文件，需转换为单通道
    }
    threshold(src_bin, src_bin, energy_part_param_.RED_GRAY_THRESH, 255, THRESH_BINARY);
    std::vector<vector<Point>> flow_strip_fan_contours;
    FlowStripFanStruct(src_bin); // 图像膨胀，防止图像断开并更方便寻找
    if (show_process)
        imshow("flow strip fan struct", src_bin);

    findContours(src_bin, flow_strip_fan_contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    std::vector<cv::RotatedRect> candidate_flow_strip_fans;
    cout<<flow_strip_fan_contours.size()<<endl;
    int id=0;
    for (auto &flow_strip_fan_contour : flow_strip_fan_contours)
    {
        cout<<id<<endl;
        id++;
        if (!isValidFlowStripFanContour(src_bin, flow_strip_fan_contour))
        {
            continue;
        }
        flow_strip_fans.emplace_back(cv::minAreaRect(flow_strip_fan_contour));
    }
    if (flow_strip_fans.empty())
    {
        if (show_info)
            cout << "flow strip fan false!" << endl;
        return false;
    }
    return true;
}

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于寻找流动条
// ---------------------------------------------------------------------------------------------------------------------
bool Energy::findFlowStrip(const cv::Mat &src)
{
    if (src.empty())
    {
        if (show_info)
            cout << "empty!" << endl;
        return false;
    }
    cv::Mat src_bin;
    src_bin = src.clone();

    if (src_bin.type() == CV_8UC1) // 黑白图像
    {
        cvtColor(src_bin, src_bin, COLOR_GRAY2RGB);
    }
    std::vector<cv::RotatedRect> candidate_target_armors = target_armors;
    for (auto &candidate_target_armor : candidate_target_armors)
    {
        Point2f vertices[4];
        candidate_target_armor.size.height *= 1.3;
        candidate_target_armor.size.width *= 1.3;
        candidate_target_armor.points(vertices); // 计算矩形的4个顶点
        for (int i = 0; i < 4; i++)
        {
            line(src_bin, vertices[i], vertices[(i + 1) % 4], Scalar(0, 0, 0), 20);
        }
    }

    cvtColor(src_bin, src_bin, CV_BGR2GRAY); // 若读取三通道视频文件，需转换为单通道
    threshold(src_bin, src_bin, energy_part_param_.RED_GRAY_THRESH, 255, THRESH_BINARY);
    FlowStripStruct(src_bin); // 图像膨胀，防止图像断开并更方便寻找
    if (show_process)
        imshow("flow strip struct", src_bin);

    std::vector<vector<Point>> flow_strip_contours;
    findContours(src_bin, flow_strip_contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    int id=0;
    for (auto candidate_flow_strip_fan : flow_strip_fans)
    {
        for (auto &flow_strip_contour : flow_strip_contours)
        {   
            cout<<id<<endl;
            id++;
            if (!isValidFlowStripContour(flow_strip_contour))
            {
                continue;
            }

            std::vector<cv::Point2f> intersection;
            RotatedRect cur_rect = minAreaRect(flow_strip_contour);

            if (rotatedRectangleIntersection(cur_rect, candidate_flow_strip_fan, intersection) == 0)
            {
                continue;
            }
            else if (contourArea(intersection) > energy_part_param_.FLOW_STRIP_CONTOUR_INTERSETION_AREA_MIN)
            {
                flow_strips.emplace_back(cv::minAreaRect(flow_strip_contour));
                //                cout << "intersection: " << contourArea(intersection) << '\t' << cur_rect.center << endl;
            }
            else
            {
                continue;
            }
        }
    }
    if (flow_strips.empty())
    {
        if (show_info)
            cout << "flow strip false!" << endl;
        return false;
    }
    else if (flow_strips.size() > 1)
    {
        if (show_info)
            cout << "Too many flow strips!" << endl;
        return false;
    }
    else
    {
        flow_strip = flow_strips.at(0);
        for (auto &candidate_flow_strip_fan : flow_strip_fans)
        {
            std::vector<cv::Point2f> intersection;
            if (rotatedRectangleIntersection(flow_strip, candidate_flow_strip_fan, intersection) == 0)
            {
                continue;
            }
            else if (contourArea(intersection) > energy_part_param_.FLOW_STRIP_CONTOUR_INTERSETION_AREA_MIN)
            {
                flow_strip_fan = candidate_flow_strip_fan;
            }
        }
        int i = 0;
        for (i = 0; i < target_armors.size(); ++i)
        {
            std::vector<cv::Point2f> intersection;
            if (rotatedRectangleIntersection(target_armors.at(i), flow_strip_fan, intersection) == 0)
                continue; // 返回0表示没有重合面积
            double cur_contour_area = contourArea(intersection);
            if (cur_contour_area > energy_part_param_.TARGET_INTERSETION_CONTOUR_AREA_MIN)
            {
                target_armor = target_armors.at(i);
                target_point = target_armor.center;
            }
        }
    }
    return true;
}

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于弱识别寻找流动条
// ---------------------------------------------------------------------------------------------------------------------
bool Energy::findFlowStripWeak(const cv::Mat &src)
{
    if (src.empty())
    {
        if (show_info)
            cout << "empty!" << endl;
        return false;
    }
    cv::Mat src_bin;
    src_bin = src.clone();

    if (src_bin.type() == CV_8UC1) // 黑白图像
    {
        cvtColor(src_bin, src_bin, COLOR_GRAY2RGB);
    }
    std::vector<cv::RotatedRect> candidate_armors = armors;
    for (auto &candidate_armor : candidate_armors)
    {
        Point2f vertices[4];
        candidate_armor.size.height *= 1.3;
        candidate_armor.size.width *= 1.3;
        candidate_armor.points(vertices); // 计算矩形的4个顶点
        for (int i = 0; i < 4; i++)
        {
            line(src_bin, vertices[i], vertices[(i + 1) % 4], Scalar(0, 0, 0), 20);
        }
    }

    cvtColor(src_bin, src_bin, CV_BGR2GRAY); // 若读取三通道视频文件，需转换为单通道

    FlowStripStruct(src_bin); // 图像膨胀，防止图像断开并更方便寻找
    if (show_process)
        imshow("weak struct", src_bin);

    std::vector<vector<Point>> flow_strip_contours;
    findContours(src_bin, flow_strip_contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    for (auto &flow_strip_contour : flow_strip_contours)
    {
        if (!isValidFlowStripContour(flow_strip_contour))
        {
            continue;
        }
        flow_strips.emplace_back(cv::minAreaRect(flow_strip_contour));
    }
    if (flow_strips.empty())
    {
        if (show_info)
            cout << "weak flow strip false!" << endl;
        //        waitKey(0);
        return false;
    }
    else
    {
        for (const auto &candidate_flow_strip : flow_strips)
        {
            for (const auto &candidate_armor : armors)
            {
                if (pointDistance(candidate_flow_strip.center, candidate_armor.center) <
                        energy_part_param_.STRIP_ARMOR_DISTANCE_MIN ||
                    pointDistance(candidate_flow_strip.center, candidate_armor.center) >
                        energy_part_param_.STRIP_ARMOR_DISTANCE_MAX)
                {
                    continue;
                }
                float angle_armor = candidate_armor.size.width > candidate_armor.size.height ? candidate_armor.angle : candidate_armor.angle - 90;
                float angle_strip = candidate_flow_strip.size.width > candidate_flow_strip.size.height ? candidate_flow_strip.angle : candidate_flow_strip.angle - 90;

                if (abs(angle_armor - angle_strip) < 60 || abs(angle_armor - angle_strip) > 120)
                {
                    continue;
                }
                target_armor = candidate_armor;
                target_point = candidate_armor.center;
                flow_strip = candidate_flow_strip;
                return true;
            }
        }
        if (show_info)
            cout << "weak flow strip false!" << endl;
        return false;
    }
}

//----------------------------------------------------------------------------------------------------------------------
// 此函数用于框取中心R的寻找范围
// ---------------------------------------------------------------------------------------------------------------------
bool Energy::findCenterROI(const cv::Mat &src)
{
    float length = target_armor.size.height > target_armor.size.width ? target_armor.size.height : target_armor.size.width;
    cout<<length<<endl;
    Point2f p2p(flow_strip.center.x - target_point.x,
                flow_strip.center.y - target_point.y);
    p2p = p2p / pointDistance(flow_strip.center, target_point); // 单位化
    center_ROI = cv::RotatedRect(cv::Point2f(flow_strip.center + p2p * length * 1.7),
                                 Size2f(length * 1.7, length * 1.7), -90);
    return true;
}
