#ifndef POST_PROCESS_SETTINGS_H
#define POST_PROCESS_SETTINGS_H

struct PostProcessSettings
{
    int enableVignette;
    int enableChromaticAberration;
    float deltaTime{};
    int pad1{};
};

#endif
