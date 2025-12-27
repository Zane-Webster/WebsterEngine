#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <iostream>

#include <vector>
#include <algorithm>

class Utils {
public:
    template<typename T>
    static bool VectorContains(const std::vector<T>& vec, const T& value) { return std::find(vec.begin(), vec.end(), value) != vec.end(); };

private:

};

#endif // SRC_UTILS_H_