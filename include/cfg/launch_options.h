#pragma once

#include "core/WebsterEngine.h"

//  ===== WINDOW OPTIONS =====
constexpr WE::WINDOW_RESOLUTION WE_LAUNCH_WINDOW_RESOLUTION = WE::WINDOW_RESOLUTION::HD;
constexpr bool WE_LAUNCH_VSYNC = false;
constexpr bool WE_LAUNCH_FULLSCREEN = false;
constexpr float WE_LAUNCH_FPS_CAP = 60.0f;

//  ===== STARTING STATE =====
constexpr WE::STATE WE_LAUNCH_STATE = WE::STATE::EDITOR;