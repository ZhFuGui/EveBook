#include <Windows.h>
#include "../entity/EntityMgr.h"
//读取周围人

//选定目标2

//计算角度

//模拟移动

bool Start(CS2Assist::Entity* entityList, CS2Assist::Entity& local) {
	while (true) {

		// 创建一个 INPUT 结构体数组
		INPUT input = {};
		// 设置为鼠标事件
		input.type = INPUT_MOUSE;
		// 设置 dwFlags 来指定这是一个相对移动事件
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
					// 设置 dx 和 dy 为相对位移值
					input.mi.dx = correction.yaw * fo;  // X轴方向的相对位移
					input.mi.dy = correction.pitch * fo;  // Y轴方向的相对位移
					// 调用 SendInput 发送输入事件
					SendInput(1, &input, sizeof(INPUT));
					break;
				};
				
			}
			
		}
		


		





		//std::this_thread::sleep_for(std::chrono::milliseconds(20));
		system("cls");
	}
	return 1;
}; // 修正参数类型