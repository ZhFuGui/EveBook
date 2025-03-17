#ifndef CS2ASSIST_CONSTS_UTIL_H
#define CS2ASSIST_CONSTS_UTIL_H
#include <Windows.h>
#include <cmath>

namespace CS2Assist {
	namespace Consts {
		namespace Math {
			constexpr double PI = 3.14159265358979323846;
		};
		namespace Factor {
			constexpr float camera2head_Factor = 6.2f;
			constexpr float Fov_Angle = 95.8807f;
			constexpr float PerAngleInFov_scalingFactor = Fov_Angle * 1920 * 4 / 16200;
			
		};
		namespace SignCode {
			
			constexpr const char* BlueXray = "32 C0 48 8B B4 24 ?? 00 00 00 48 8B 9C 24 ?? 00 00 00 48 8B AC 24 ?? 00 00 00 48 81 C4 ?? 00 00 00 41 5F 41 5E 5F";
			constexpr const char* ServerShotAngle = "F3 ?? ?? ?? 10 83 49 10 01 8B 41 10 F3 0F 11 41 18 83 C8 02 F3";
		};
	};
}; 
#endif // CS2ASSIST_CONSTS_UTIL_H