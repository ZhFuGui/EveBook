#ifndef CS2ASSIST_AIM_ASSIST_H
#define CS2ASSIST_AIM_ASSIST_H
#include <Windows.h>
#include <iostream>
#include "../entity/EntityMgr.h"
#include "../utils/ConstsUtil.h"

namespace CS2Assist {

	// AimAssist 类，用于实现多种自瞄功能
	class AimAssist {
	public:
		AimAssist(HANDLE processHandle, HMODULE clientModule, Entity* entityList, Entity& local);
		~AimAssist();

		// 鼠标模拟自瞄：通过模拟鼠标移动实现瞄准
		void MouseAim();

		// 内存改变视角自瞄：通过修改内存中的视角数据实现瞄准
		void MemoryAim();

		// 静默自瞄：在不改变玩家视角的情况下修改射击方向
		void SilentAim();

	private:
		HANDLE hProcess;            // 进程句柄
		uint64_t ClientModuleAddress; // client.dll 模块基址
		Entity* entityList;         // 全局实体列表
		Entity& local;              // 全局本地玩家实体

		// 辅助函数
		Angle Angle2Arc(Angle& enemyEyeAngle) const;
		Vec3 PosCalibrated(const Vec3& enemyPos, const Angle& enemyEyeArc) const;
		Angle CalcAimAngle(const Angle& Target2Me, const Angle& myEyeAngle) const;
		Angle Target2Me(const Vec3& myPos, const Vec3& targetPos) const;
	};

} // namespace CS2Assist

#endif // CS2ASSIST__AIMASSIST_H