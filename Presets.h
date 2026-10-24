#pragma once
#include <array>

namespace souf
{
struct BandSettings
{
    float thresholdDb;
    float ratio;
    float attackMs;
    float releaseMs;
    float makeupDb;
};

struct Preset
{
    const char* name;
    const char* shortName;
    float crossover1Hz; // Low | Mid
    float crossover2Hz; // Mid | High
    std::array<BandSettings, 3> bands; // Low, Mid, High
    float outputDb;
    unsigned int glowColour; // 0xAARRGGBB, nur fuers Aussehen
};

inline constexpr int numPresets = 6;

//                                           Thr    Ratio Att  Rel  Makeup
inline const std::array<Preset, numPresets> presets {{
    { "A - Smooth", "A", 120.f, 3500.f,
      {{ { -18.f, 2.0f, 30.f, 200.f, 2.f },
         { -16.f, 2.0f, 20.f, 150.f, 2.f },
         { -18.f, 1.8f, 10.f, 100.f, 2.f } }},
      0.f, 0xff4fc3f7 },

    { "B - Punch", "B", 150.f, 4000.f,
      {{ { -20.f, 3.0f, 15.f, 120.f, 3.f },
         { -18.f, 3.0f, 10.f, 100.f, 3.f },
         { -20.f, 2.5f,  5.f,  80.f, 3.f } }},
      0.f, 0xffffb74d },

    { "C - Loud", "C", 100.f, 3000.f,
      {{ { -24.f, 4.0f, 10.f, 100.f, 5.f },
         { -22.f, 4.0f,  8.f,  80.f, 5.f },
         { -24.f, 3.5f,  3.f,  60.f, 5.f } }},
      -1.f, 0xffef5350 },

    { "D - Warm Bass", "D", 200.f, 5000.f,
      {{ { -26.f, 4.0f, 25.f, 250.f, 6.f },
         { -14.f, 2.0f, 25.f, 200.f, 1.f },
         { -12.f, 1.5f, 15.f, 150.f, 0.f } }},
      0.f, 0xffff8a65 },

    { "E - Air", "E", 150.f, 2500.f,
      {{ { -14.f, 2.0f, 30.f, 200.f, 0.f },
         { -16.f, 2.5f, 15.f, 120.f, 2.f },
         { -26.f, 4.0f,  3.f,  60.f, 5.f } }},
      0.f, 0xffba68c8 },

    { "F - Crush", "F", 120.f, 2500.f,
      {{ { -32.f, 8.0f,  3.f,  50.f, 8.f },
         { -30.f, 8.0f,  2.f,  40.f, 8.f },
         { -30.f, 6.0f,  1.f,  40.f, 7.f } }},
      -3.f, 0xff9ccc65 },
}};
} // namespace souf
