//
// Created by xinyang on 19-7-11.
//

#include <armor_finder/armor_finder.h>
#include <config/setconfig.h>
#include <log.h>

static bool sendTarget(Serial &serial, double x, double y, double z, uint16_t shoot_delay)
{
    static short x_tmp, y_tmp, z_tmp;
    uint8_t buff[10];
    // if(z > 0 && z <= 100){
    //     y = y + 3;
    // }
    // else if(z > 100 && z <= 200){
    //     y = y + 4;
    // }
    // else if(z > 200 && z <= 300){
    //     y = y + 5;
    // }
    // else if(z > 300 && z <= 400){
    //     y = y + 6;
    // }
    // else if(z > 400 && z <= 500){
    //     y = y + 7;
    // }
    // else if(z > 500 && z <= 600){
    //     y = y + 8;
    // }
    // else if(z > 600 && z <= 700){
    //     y = y + 9;
    // }
    // else if(z > 700 && z <= 800){
    //     y = y + 10;
    // }
    // else{
    //     y = y;
    // }

#ifdef WITH_COUNT_FPS
    static time_t last_time = time(nullptr);
    static int fps;
    time_t t = time(nullptr);
    // cout << "Armor: fps:" << fps << ", (x:" << x << ",y:" << y << ",z:" << z << ")" << endl;
    if (last_time != t)
    {
        last_time = t;
        cout << "Armor: fps:" << fps << ", (x:" << x << ",y:" << y << ",z:" << z << ")" << endl;
        fps = 0;
    }
    // else
    // {
    //     cout << "Armor: fps:0,(x:0 , y:0 , z:0)"  << endl;
    //     x = 0;
    //     y = 0;
    //     z = 0;
    // }
    fps += 1;
#endif

    x_tmp = static_cast<short>(x * (32768 - 1) / 100);
    y_tmp = static_cast<short>(y * (32768 - 1) / 100);
    z_tmp = static_cast<short>(z * (32768 - 1) / 1000);

    buff[0] = 0x01;
    buff[1] = static_cast<char>((x_tmp >> 8) & 0xFF);
    buff[2] = static_cast<char>((x_tmp >> 0) & 0xFF);
    buff[3] = static_cast<char>((y_tmp >> 8) & 0xFF);
    buff[4] = static_cast<char>((y_tmp >> 0) & 0xFF);
    buff[5] = static_cast<char>((z_tmp >> 8) & 0xFF);
    buff[6] = static_cast<char>((z_tmp >> 0) & 0xFF);
    buff[7] = static_cast<char>((shoot_delay >> 8) & 0xFF);
    buff[8] = static_cast<char>((shoot_delay >> 0) & 0xFF);
    buff[9] = 'e';
    //    if(buff[7]<<8 | buff[8])
    //        cout << (buff[7]<<8 | buff[8]) << endl;
    return serial.WriteData(buff, sizeof(buff));
}

bool ArmorFinder::sendBoxPosition(uint16_t shoot_delay)
{
    if (target_box.rect == cv::Rect2d())
        return false;
    if (shoot_delay)
    {
        LOGM(STR_CTR(WORD_BLUE, "next box %dms"), shoot_delay);
    }
    auto rect = target_box.rect;
    double dist = DISTANCE_HEIGHT / rect.height;
#ifdef WITH_COUNT_FPS
    static time_t last_time = time(nullptr);
    static int fps;
    static double last_time_x = 0, last_time_y = 0, next_time_x = 0, next_time_y = 0;
    time_t t = time(nullptr);
    // cout << "Armor: fps:" << fps << ", (x:" << x << ",y:" << y << ",z:" << z << ")" << endl;

    fps += 1;
    cout << fps << endl;
    if (fps == 5)
    {
        last_time_x = rect.x + rect.width / 2;
        last_time_y = rect.y + rect.height / 2;
    }
    if (fps == 10)
    {
        next_time_x = rect.x + rect.width / 2;
        next_time_y = rect.y + rect.height / 2;
    }

    if (last_time != t)
    {
        last_time = t;
        // cout << "Armor: fps:" << fps << ", (x:" << x << ",y:" << y << ",z:" << z << ")" << endl;
        fps = 0;

        double addTime = dist / 2500.0;
        double errX = last_time_x - next_time_x;
        double aimSpeed = errX / (5.0/fps);
        double addDist = addTime * aimSpeed;

        double dx = (rect.x + rect.width / 2 - IMAGE_CENTER_X) + addDist;
        double dy = (rect.y + rect.height / 2 - IMAGE_CENTER_Y);
        double yaw = atan(dx / FOCUS_PIXAL) * 180 / PI;
        double pitch = atan(dy / FOCUS_PIXAL) * 180 / PI;

        last_time_x = 0, last_time_y = 0, next_time_x = 0, next_time_y = 0;
        return sendTarget(serial, yaw, -pitch, dist, shoot_delay);
    }
#endif
    // cout<<rect.height<<endl;
    return false;
}

bool ArmorFinder::sendBoxPosition_0(uint16_t shoot_delay)
{
    // cout << "0" << endl;
    // if (target_box.rect == cv::Rect2d()) return false;
    if (shoot_delay)
    {
        LOGM(STR_CTR(WORD_BLUE, "next box %dms"), shoot_delay);
    }
    auto rect = target_box.rect;
    double dx = (rect.x + rect.width / 2 - IMAGE_CENTER_X);
    double dy = (rect.y + rect.height / 2 - IMAGE_CENTER_Y);
    double yaw = 0;
    double pitch = 0;
    double dist = 0;
    // cout << "1" << endl;
    return sendTarget(serial, yaw, -pitch, dist, shoot_delay);
}
