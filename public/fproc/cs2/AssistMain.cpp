#include <Windows.h>
#include <iostream>
#include "headers\AssistMain.h"
#include "output\offsets.hpp"
#include "output\client_dll.hpp"
#include "headers\core\ProcessUtil.h"
#include "headers\entity\EntityMgr.h"
#include "headers\utils\MiscUtil.h"
#include "headers\features\AimAssist.h"
#include <thread>
DWORD Pid = CS2Assist::ProcessUtil::GetProcessPid(L"cs2.exe");

HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);

HMODULE ClientModuleAddress = CS2Assist::ProcessUtil::GetModuleHandleW(hProcess, "client.dll");

CS2Assist::EntityMgr entityMgr(hProcess, ClientModuleAddress);

CS2Assist::Entity entityList[64];

CS2Assist::Entity local;


int main() {

	SetConsoleOutputCP(CP_UTF8);
	
	entityMgr.StartEntityUpdateThread(entityList, local);

	Start(entityList, local);
	system("pause");
	while (true) {

		std::cout << "localName: " << local.name
			<< ", HP: " << local.health
			<< ", Weapon: " << local.weaponName
			<< ", Team: " << local.teamId
			<< ", pitch: " << local.eyeAngle.pitch
			<< ", X: " << local.position.x << std::endl;
		for (int i = 0; i < 64; i++) {

			if (entityList[i].isValid) {
				std::cout << "ID: " << i
					<< ", Name: " << entityList[i].name
					<< ", HP: " << entityList[i].health
					<< ", Weapon: " << entityList[i].weaponName
					<< ", Team: " << entityList[i].teamId
					<< ", pitch: " << entityList[i].eyeAngle.pitch
					<< ", velocity.x: " << entityList[i].velocity.x
					<< ", position.x: " << entityList[i].position.x << std::endl;
			}
		}

		//PauseMilliseconds(200);
		system("cls");
	}
}