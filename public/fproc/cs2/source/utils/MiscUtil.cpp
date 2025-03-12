#include "../../headers/utils/MiscUtil.h"

namespace CS2Assist {

    Angle CalcAimCorrection(const Vec3& myPos, const Vec3& enemyPos, const Angle& myviewAngle, const Angle& enemyviewAngle) {
        Angle aimAngle;

        // 计算水平角度 (Yaw)
        float dX = (enemyPos.x+std::cos(enemyviewAngle.yaw * (M_PI / 180.0))*10) - myPos.x;
        float dY = (enemyPos.y+std::sin(enemyviewAngle.yaw * (M_PI / 180.0)) * 10) - myPos.y;
        float targetYaw = std::atan2(dY, dX) * 180.0f / static_cast<float>(M_PI);

        // 归一化目标角度到 [0, 360)
        if (targetYaw < 0) {
            targetYaw += 360.0f;
        }

        // 计算垂直角度 (Pitch)
        float dZ = enemyPos.z - myPos.z;
        float dXY = std::sqrt(dX * dX + dY * dY); // XY 平面距离
        float targetPitch = std::atan2(-dZ, dXY) * 180.0f / static_cast<float>(M_PI);

        // 计算角度差
        float dYaw = myviewAngle.yaw - targetYaw;
        float dPitch =  targetPitch - myviewAngle.pitch;

        // 归一化水平角度差到 [-180, 180]
        if (dYaw > 180.0f) {
            dYaw -= 360.0f;
        }
        else if (dYaw < -180.0f) {
            dYaw += 360.0f;
        }

        // 垂直角度无需额外归一化，直接取差值
        aimAngle.yaw = dYaw;
        aimAngle.pitch = dPitch;

        return aimAngle;
    }

} // namespace CS2Assist