#include "../../headers/utils/MiscUtil.h"

namespace CS2Assist {

    Angle CalcAimCorrection(const Vec3& myPos, const Vec3& enemyPos, const Angle& myviewAngle, const Angle& enemyviewAngle) {
        Angle aimAngle;

        // ����ˮƽ�Ƕ� (Yaw)
        float dX = (enemyPos.x+std::cos(enemyviewAngle.yaw * (M_PI / 180.0))*10) - myPos.x;
        float dY = (enemyPos.y+std::sin(enemyviewAngle.yaw * (M_PI / 180.0)) * 10) - myPos.y;
        float targetYaw = std::atan2(dY, dX) * 180.0f / static_cast<float>(M_PI);

        // ��һ��Ŀ��Ƕȵ� [0, 360)
        if (targetYaw < 0) {
            targetYaw += 360.0f;
        }

        // ���㴹ֱ�Ƕ� (Pitch)
        float dZ = enemyPos.z - myPos.z;
        float dXY = std::sqrt(dX * dX + dY * dY); // XY ƽ�����
        float targetPitch = std::atan2(-dZ, dXY) * 180.0f / static_cast<float>(M_PI);

        // ����ǶȲ�
        float dYaw = myviewAngle.yaw - targetYaw;
        float dPitch =  targetPitch - myviewAngle.pitch;

        // ��һ��ˮƽ�ǶȲ [-180, 180]
        if (dYaw > 180.0f) {
            dYaw -= 360.0f;
        }
        else if (dYaw < -180.0f) {
            dYaw += 360.0f;
        }

        // ��ֱ�Ƕ���������һ����ֱ��ȡ��ֵ
        aimAngle.yaw = dYaw;
        aimAngle.pitch = dPitch;

        return aimAngle;
    }

} // namespace CS2Assist