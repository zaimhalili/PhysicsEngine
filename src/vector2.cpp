#include "vector2.hpp"
#include <cmath>

bool Collides(float radius1, float radius2, float speed,
                float center1_X, float center2_X, 
                float center1_Y, float center2_Y)
{
    float d = sqrt((center2_X - center1_X) * (center2_X - center1_X) + (center2_Y - center1_Y) * (center2_Y - center1_Y));
    return d <= radius1 + radius2;
}
