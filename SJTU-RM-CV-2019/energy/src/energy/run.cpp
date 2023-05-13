//
// Created by xixiliadorabarry on 3/5/19.
//
#include "energy/energy.h"
#include "log.h"
#include "config/setconfig.h"
#include "options.h"

using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// 此函数为能量机关模式主控制流函数，且步兵仅拥有云台摄像头
// ---------------------------------------------------------------------------------------------------------------------
void Energy::run(cv::Mat &src)
{
    clearAll();
    //initImage(src);
    if (show_energy || show_process)
        waitKey(1);
    // if (show_process)
    //     imshow("bin", src);
    findHitpoint(src);
    // if (show_energy)
    //     showHitpoint("energy", src);

    // if (!findFlowStripFan(src)) {
    //     if (!findFlowStripWeak(src)) return;
    // } else {
    //     if (show_energy)showFlowStripFan("strip fan", src);
    //     if (!findTargetInFlowStripFan()) return;
    //     if (!findFlowStrip(src)) return;
    // }

    // findCenterROI(src);

    // if (show_energy)showFlowStrip("strip", src);
    // if (!findCenterR(src)) return;
    // if (show_energy)showCenterR("R", src);
    // fans_cnt = findFans(src);
    // if (show_energy)showFans("fans", src);

    // changeTarget();
    // getTargetPolarAngle();

    // if (is_big && energy_rotation_init) {
    //     initRotation();
    //     return;
    // }
    // getPredictPoint(target_point);
    // getAimPoint(predict_point);
    // judgeShoot();
    // sendEnergy();
    // if (save_mark)writeDownMark(src);
}
