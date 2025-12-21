#include <core/Logger.h>
#include <iostream>

static void LoadState(const std::string function, const WE_LOAD_STATE state) {
    switch (state) {
        case WE_LOAD_STATE::SUCCESS:
            std::cout << "[LOAD STATE] [" << function << "]: LOAD SUCCESSFUL";
            break;
        case WE_LOAD_STATE::FAIL:
            std::cout << "[LOAD STATE] [" << function << "]: LOAD FAILED";
            break;
        case WE_LOAD_STATE::PATH_NOT_FOUND:
            std::cout << "[LOAD STATE] [" << function << "]: LOAD PATH NOT FOUND";
            break;
        case WE_LOAD_STATE::UNKNOWN_FILE_TYPE:
            std::cout << "[LOAD STATE] [" << function << "]: UNKNOWN FILE TYPE PASSED";
            break;
        case WE_LOAD_STATE::INCORRECT_FILE_TYPE:
            std::cout << "[LOAD STATE] [" << function << "]: INCORRECT FILE TYPE PASSED";
            break;
    }
}