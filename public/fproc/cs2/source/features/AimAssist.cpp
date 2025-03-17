#include "../../headers/features/AimAssist.h"
#include "../../headers/utils/ConstsUtil.h"
#include "../../headers/core/ProcessUtil.h"
#include "../../output/offsets.hpp"
#include "../../output/client_dll.hpp"
#include <cmath>

namespace CS2Assist {

    AimAssist::AimAssist(HANDLE processHandle, HMODULE clientModule, Entity* entityList, Entity& local)
        : hProcess(processHandle), ClientModuleAddress((uint64_t)clientModule),
        entityList(entityList), local(local) {
        // 初始化代码（如果需要）
    }

    AimAssist::~AimAssist() {
        // 清理代码（如果需要）
    }

    void AimAssist::MouseAim() {

        INPUT input = {};// 创建一个 INPUT 结构体数组
        input.type = INPUT_MOUSE;// 设置为鼠标事件
        input.mi.dwFlags = MOUSEEVENTF_MOVE;// 设置 dwFlags 来指定这是一个相对移动事件
        int closest_index = 0;
         do{
            float closestDistance = FLT_MAX;
            Entity target;
            for (int i = 0; i < 64; i++) {
                if (entityList[i].isValid && entityList[i].teamId != local.teamId && entityList[i].health > 0) {

                    float distance = local.cameraPosition.DistanceXY(entityList[i].cameraPosition);

                    if (distance < closestDistance) {
                        closestDistance = distance;
                        target = entityList[i];
                    }
                }
            };
            if (!target.isValid|| target.health==0) {
                continue;
            }
            
            CS2Assist::Angle correction = CS2Assist::AimAssist::CalcAimAngle(
                Target2Me(local.cameraPosition, PosCalibrated(target.cameraPosition, Angle2Arc(target.eyeAngle))),
                local.eyeAngle
            );
            //std::cout << target.name << std::endl;
            //std::cout << correction.yaw << "  " << correction.pitch << std::endl;
            input.mi.dx = correction.yaw * Consts::Factor::PerAngleInFov_scalingFactor;  // X轴方向的相对位移
            input.mi.dy = correction.pitch * Consts::Factor::PerAngleInFov_scalingFactor;  // Y轴方向的相对位移
            SendInput(1, &input, sizeof(INPUT));
            //std::this_thread::sleep_for(std::chrono::milliseconds(20));
            system("cls");
            
        }while (1);
       
        return;
    }

    void AimAssist::MemoryAim() {
        // 内存改变视角自瞄逻辑
        
        do {
            float closestDistance = FLT_MAX;
            Entity target;
            for (int i = 0; i < 64; i++) {
                if (entityList[i].isValid && entityList[i].teamId != local.teamId && entityList[i].health > 0) {

                    float distance = local.cameraPosition.DistanceXY(entityList[i].cameraPosition);

                    if (distance < closestDistance) {
                        closestDistance = distance;
                        target = entityList[i];
                    }
                }
            };
            if (!target.isValid || target.health == 0) {
                continue;
            }

            CS2Assist::Angle correction = Target2Me(local.cameraPosition, PosCalibrated(target.cameraPosition, Angle2Arc(target.eyeAngle)));

            WriteProcessMemory(hProcess, (LPVOID)(ClientModuleAddress+ cs2_dumper::offsets::client_dll::dwViewAngles), &correction.pitch,sizeof(float),0);
            WriteProcessMemory(hProcess, (LPVOID)(ClientModuleAddress + cs2_dumper::offsets::client_dll::dwViewAngles+0x4), &correction.yaw, sizeof(float), 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            system("cls");

        } while (1);

        return;
    }

    void AimAssist::SilentAim() {

        LPVOID shellcodeAddr = VirtualAllocEx(hProcess, NULL, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (!shellcodeAddr) return;
        std::cout << std::hex << shellcodeAddr << std::endl;
        // Shellcode 字节码
        BYTE shellcode[] = {
            0x50,                               // push rax
            0x66, 0x0F, 0xD6, 0x04, 0x24,      // movdqa [rsp], xmm0
            0x66, 0x0F, 0xD6, 0x4C, 0x24, 0x10, // movdqa [rsp+16], xmm1
            0x83, 0x49, 0x10, 0x01,            // or dword ptr [rcx+10], 01
            0x8B, 0x41, 0x10,                  // mov eax, [rcx+10]
            0x83, 0xC8, 0x02,                  // or eax, 02
            0x89, 0x41, 0x10,                  // mov [rcx+10], eax
            0x83, 0xC8, 0x04,                  // or eax, 04
            0xF3, 0x0F, 0x10, 0x05, 0x00, 0x00, 0x00, 0x00, // movss xmm0, [rip + pitchOffset]
            0xF3, 0x0F, 0x10, 0x0D, 0x00, 0x00, 0x00, 0x00, // movss xmm1, [rip + yawOffset]
            0xF3, 0x0F, 0x11, 0x49, 0x18,      // movss [rcx+18], xmm1 (yaw)
            0xF3, 0x0F, 0x11, 0x41, 0x1C,      // movss [rcx+1C], xmm0 (pitch)
            0xF3, 0x0F, 0x10, 0x47, 0x18,      // movss xmm0, [rdi+18]
            0x66, 0x0F, 0xD6, 0x4C, 0x24, 0x10, // movdqa xmm1, [rsp+16]
            0x66, 0x0F, 0xD6, 0x04, 0x24,      // movdqa xmm0, [rsp]
            0x58,                               // pop rax
            0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, returnAddr
            0xFF, 0xE0                          // jmp rax
        };

        // 数据存储位置
        uintptr_t pitchAddr = (uintptr_t)shellcodeAddr + 0x500; // pitch 存储位置
        uintptr_t yawAddr = (uintptr_t)shellcodeAddr + 0x504;   // yaw 存储位置，相差 4 字节

        // 计算 rip 相对寻址偏移
        int pitchOffset = 0x500 - 36; // rip = shellcodeAddr + 36 (movss xmm0 指令后)
        int yawOffset = 0x504 - 44;   // rip = shellcodeAddr + 44 (movss xmm1 指令后)

        // 写入偏移到 shellcode
        memcpy(shellcode + 32, &pitchOffset, 4); // movss xmm0 的偏移部分
        memcpy(shellcode + 40, &yawOffset, 4);   // movss xmm1 的偏移部分

        // 计算注入点和返回地址
        uintptr_t targetAddr = 0x0; // 替换为实际地址
        CS2Assist::ProcessUtil::ScanSignature(hProcess, (HMODULE)ClientModuleAddress, CS2Assist::Consts::SignCode::ServerShotAngle, targetAddr);
        uintptr_t returnAddr = targetAddr + 41;              // 假设覆盖 41 字节后返回
        memcpy(shellcode + 73, &returnAddr, sizeof(uintptr_t)); // 写入 returnAddr

        // 写入 shellcode
        SIZE_T bytesWritten;
        if (!WriteProcessMemory(hProcess, shellcodeAddr, shellcode, sizeof(shellcode), &bytesWritten)) {
            std::cerr << "Failed to write shellcode!" << std::endl;
            VirtualFreeEx(hProcess, shellcodeAddr, 0, MEM_RELEASE);
            CloseHandle(hProcess);
            return;
        }

        // 写入跳转补丁
        BYTE jmpPatch[41] = {
            0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax, shellcodeAddr
            0xFF, 0xE0,                                                 // jmp rax
            0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // nop x10
            0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // nop x10
            0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90        // nop x9
        };
        memcpy(jmpPatch + 2, &shellcodeAddr, sizeof(uintptr_t));
        if (!WriteProcessMemory(hProcess, (LPVOID)targetAddr, jmpPatch, 41, &bytesWritten)) {
            std::cerr << "Failed to write jmp patch!" << std::endl;
            VirtualFreeEx(hProcess, shellcodeAddr, 0, MEM_RELEASE);
            CloseHandle(hProcess);
            return;
        }

        std::cout << "Shellcode injected at " << shellcodeAddr << ", return to " << std::hex << returnAddr << std::endl;
        do {
            float closestDistance = FLT_MAX;
            Entity target;
            for (int i = 0; i < 64; i++) {
                if (entityList[i].isValid && entityList[i].controllerAddr != local.controllerAddr && entityList[i].health > 0) {

                    float distance = local.cameraPosition.DistanceXY(entityList[i].cameraPosition);

                    if (distance < closestDistance) {
                        closestDistance = distance;
                        target = entityList[i];
                    }
                }
            };
            if (!target.isValid || target.health == 0) {
                continue;
            }

            CS2Assist::Angle correction = Target2Me(
                local.cameraPosition, 
                PosCalibrated(target.cameraPosition, Angle2Arc(target.eyeAngle)));

            //WriteProcessMemory(hProcess, (LPVOID)(ClientModuleAddress + cs2_dumper::offsets::client_dll::dwViewAngles), &correction.pitch, sizeof(float), 0);
            //WriteProcessMemory(hProcess, (LPVOID)(ClientModuleAddress + cs2_dumper::offsets::client_dll::dwViewAngles + 0x4), &correction.yaw, sizeof(float), 0);
            // 写入示例目标角度（偏航角和俯仰角）
            //float yaw = 90.0f;   // 示例偏航角
            //float pitch = 45.0f; // 示例俯仰角
            //std::cout<< correction.pitch << correction.yaw <<std::endl;
            WriteProcessMemory(hProcess, (LPVOID)pitchAddr, &correction.yaw, sizeof(float), NULL);
            WriteProcessMemory(hProcess, (LPVOID)yawAddr, &correction.pitch, sizeof(float), NULL);
        } while (1);
    }

    // 角度弧度化
    Angle AimAssist::Angle2Arc(Angle& enemyEyeAngle) const {
        return Angle(
            enemyEyeAngle.pitch * (static_cast<float>(Consts::Math::PI) / 180.0),
            enemyEyeAngle.yaw * (static_cast<float>(Consts::Math::PI) / 180.0)
        );
    }

    // 校准位置
    Vec3 AimAssist::PosCalibrated(const Vec3& enemyPos, const Angle& enemyEyeArc) const {
        return Vec3(
            enemyPos.x + (std::cos(enemyEyeArc.yaw) * Consts::Factor::camera2head_Factor),
            enemyPos.y + (std::sin(enemyEyeArc.yaw) * Consts::Factor::camera2head_Factor),
            enemyPos.z
        );
    }

    // 瞄准角度计算
    Angle AimAssist::CalcAimAngle(const Angle& Target2Me, const Angle& myEyeAngle) const {

        // 计算角度差
        float dYaw = (Target2Me.yaw < 0?Target2Me.yaw+360.0f: Target2Me.yaw )- myEyeAngle.yaw;
        float dPitch = Target2Me.pitch - myEyeAngle.pitch;

        // 归一化水平角度差到 [-180, 180]
        if (dYaw > 180.0f) {
            dYaw -= 360.0f;
        }
        else if (dYaw < -180.0f) {
            dYaw += 360.0f;
        }

        return Angle(dPitch, -dYaw);
    }

    // 目标与我在3维世界角度
    Angle AimAssist::Target2Me(const Vec3& myPos, const Vec3& targetPos) const {
        // 计算水平角度 (Yaw)
        float dX = targetPos.x - myPos.x;
        float dY = targetPos.y - myPos.y;
        float targetYaw = std::atan2(dY, dX) * 180.0f / static_cast<float>(Consts::Math::PI);

        // 计算垂直角度 (Pitch)
        float dZ = targetPos.z - myPos.z;
        float dXY = myPos.DistanceXY(targetPos); // 直接计算 XY 平面距离
        float targetPitch = std::atan2(-dZ, dXY) * 180.0f / static_cast<float>(Consts::Math::PI);

        return Angle(targetPitch, targetYaw);
    }

} // namespace CS2Assist