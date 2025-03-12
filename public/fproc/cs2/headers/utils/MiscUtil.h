#ifndef CS2ASSIST_MISC_UTIL_H
#define CS2ASSIST_MISC_UTIL_H
#include <thread>
#include "../entity/EntityMgr.h"
#include <cmath>

namespace CS2Assist {

	constexpr double M_PI = 3.14159265358979323846;

	// 计算自瞄修正角度
	Angle CalcAimCorrection(const Vec3& myPos, const Vec3& enemyPos, const Angle& myviewAngle, const Angle& enemyviewAngle);

} // namespace CS2Assist

#endif // CS2ASSIST_MISC_UTIL_H