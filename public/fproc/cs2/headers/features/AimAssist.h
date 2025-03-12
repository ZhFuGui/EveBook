#include <Windows.h>
#include "../entity/EntityMgr.h"
//��ȡ��Χ��

//ѡ��Ŀ��2

//����Ƕ�

//ģ���ƶ�

bool Start(CS2Assist::Entity* entityList, CS2Assist::Entity& local) {
	while (true) {

		// ����һ�� INPUT �ṹ������
		INPUT input = {};
		// ����Ϊ����¼�
		input.type = INPUT_MOUSE;
		// ���� dwFlags ��ָ������һ������ƶ��¼�
		input.mi.dwFlags = MOUSEEVENTF_MOVE;
		float aaa = 95.86;
		float fo = 45.44474;
		std::cout << "localName: " << local.name
			<< ", HP: " << local.health
			<< ", Weapon: " << local.weaponName
			<< ", Team: " << local.teamId
			<< ", pitch: " << local.eyeAngle.yaw
			<< "("<< std::cos(local.eyeAngle.yaw * (3.1415926 / 180.0)) * 5<<","<< std::sin(local.eyeAngle.yaw * (3.1415926 / 180.0)) * 5<<")"
			<< ", X: " << local.position.x << std::endl;

		for (int i = 0; i < 64; i++) {

			if (entityList[i].isValid) {
				std::cout << "ID: " << i
					<< ", Name: " << entityList[i].name
					<< ", HP: " << entityList[i].health
					<< ", Weapon: " << entityList[i].weaponName
					<< ", Team: " << entityList[i].teamId
					<< ", yaw: " << entityList[i].eyeAngle.yaw
					<< ", velocity.x: " << entityList[i].velocity.x
					<< ", distance: " << local.position.DistanceXY(entityList[i].position)
					<< ", position.x: " << entityList[i].position.x << std::endl;
				if (entityList[i].health > 0&& entityList[i].pawnAddr!= local.pawnAddr&& entityList[i].teamId!= local.teamId) {
					CS2Assist::Angle correction = CS2Assist::CalcAimCorrection(local.position, entityList[i].position, local.eyeAngle, entityList[i].eyeAngle);
					// ���� dx �� dy Ϊ���λ��ֵ
					input.mi.dx = correction.yaw * fo;  // X�᷽������λ��
					input.mi.dy = correction.pitch * fo;  // Y�᷽������λ��
					// ���� SendInput ���������¼�
					SendInput(1, &input, sizeof(INPUT));
					break;
				};
				
			}
			
		}
		


		





		//std::this_thread::sleep_for(std::chrono::milliseconds(20));
		system("cls");
	}
	return 1;
}; // ������������