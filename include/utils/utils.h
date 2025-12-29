#ifndef WE_UTILS_UTILS_H_
#define WE_UTILS_UTILS_H_

#include <iostream>

#include <vector>
#include <algorithm>

class Utils {
public:
    template<typename T>
    static bool VectorContains(const std::vector<T>& vec, const T& value) { return std::find(vec.begin(), vec.end(), value) != vec.end(); };

private:

};

#endif // WE_UTILS_UTILS_H_