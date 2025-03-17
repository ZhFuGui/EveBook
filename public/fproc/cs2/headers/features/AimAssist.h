#ifndef CS2ASSIST_AIM_ASSIST_H
#define CS2ASSIST_AIM_ASSIST_H
#include <Windows.h>
#include <iostream>
#include "../entity/EntityMgr.h"
#include "../utils/ConstsUtil.h"

namespace CS2Assist {

	// AimAssist �࣬����ʵ�ֶ������鹦��
	class AimAssist {
	public:
		AimAssist(HANDLE processHandle, HMODULE clientModule, Entity* entityList, Entity& local);
		~AimAssist();

		// ���ģ�����飺ͨ��ģ������ƶ�ʵ����׼
		void MouseAim();

		// �ڴ�ı��ӽ����飺ͨ���޸��ڴ��е��ӽ�����ʵ����׼
		void MemoryAim();

		// ��Ĭ���飺�ڲ��ı�����ӽǵ�������޸��������
		void SilentAim();

	private:
		HANDLE hProcess;            // ���̾��
		uint64_t ClientModuleAddress; // client.dll ģ���ַ
		Entity* entityList;         // ȫ��ʵ���б�
		Entity& local;              // ȫ�ֱ������ʵ��

		// ��������
		Angle Angle2Arc(Angle& enemyEyeAngle) const;
		Vec3 PosCalibrated(const Vec3& enemyPos, const Angle& enemyEyeArc) const;
		Angle CalcAimAngle(const Angle& Target2Me, const Angle& myEyeAngle) const;
		Angle Target2Me(const Vec3& myPos, const Vec3& targetPos) const;
	};

} // namespace CS2Assist

#endif // CS2ASSIST__AIMASSIST_H