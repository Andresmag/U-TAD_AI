#pragma once

#include <uslscore/USVec2D.h>

class Character;
struct Enemy;

struct LinAngAcceleration
{
    USVec2D linearAcceleration;
    float angularAcceleration;

    LinAngAcceleration()
        : linearAcceleration(0, 0)
        , angularAcceleration(0)
    {};

    LinAngAcceleration(const USVec2D& linear, float angular)
        : linearAcceleration(linear)
        , angularAcceleration(angular)
    {};
};

class ISteering
{
protected:
    Character* mCharacter;
    LinAngAcceleration* mAccelerations;

public:
    ISteering()
        : mAccelerations(new LinAngAcceleration())
        , mCharacter(nullptr)
    {}

    virtual ~ISteering() { delete mAccelerations; }

    virtual LinAngAcceleration* GetSteering(Character* character, const USVec2D target) { return mAccelerations; }
    virtual LinAngAcceleration* GetSteering(Character* character, float target) { return mAccelerations; }

    // Because enemy is not character type
    virtual LinAngAcceleration* GetSteering(Enemy* enemy, const USVec2D target) { return mAccelerations; }
    virtual LinAngAcceleration* GetSteering(Enemy* enemy, float target) { return mAccelerations; }

    virtual void DrawDebug() {};
    float lerp(float x0, float x1, float t);

    template<typename T>
    const T& clamp(const T& v, const T& lo, const T& hi);
};

inline float ISteering::lerp(float x0, float x1, float t)
{
    return x0 + ((x1 - x0) * t);
}

template <typename T>
const T& ISteering::clamp(const T& v, const T& lo, const T& hi)
{
    assert(!(hi < lo));
    return (v < lo) ? lo : (hi < v) ? hi : v;
}