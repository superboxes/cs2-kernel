#include "utils.h"

bool Utils::is_in_bounds(const Vector3& pos, int width, int height) {
    return pos.x >= 0 && pos.x <= width && pos.y >= 0 && pos.y <= height;
}