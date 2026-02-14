#pragma once

#include "core/WebsterEngine.h"

//  ===== WINDOW OPTIONS =====
constexpr WE::WINDOW_RESOLUTION WE_LAUNCH_WINDOW_RESOLUTION = WE::WINDOW_RESOLUTION::FHD;
constexpr bool WE_LAUNCH_VSYNC = false;
constexpr bool WE_LAUNCH_FULLSCREEN = false;
constexpr float WE_LAUNCH_FPS_CAP = 180.0f;

//  ===== SHADOW OPTIONS =====
constexpr int WE_LAUNCH_SHADOW_QUALITY = 2056;

//  ===== STARTING STATE =====
constexpr WE::STATE WE_LAUNCH_STATE = WE::STATE::MAIN;