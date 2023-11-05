// AudioManager.h

#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <SDL_mixer.h>
#include <string>
#include <vector>

class AudioManager {
public:
    static AudioManager& Instance() {
        static AudioManager instance;
        return instance;
    }
    AudioManager();
    ~AudioManager();

    bool Initialize(); // 初始化音频系统
    Mix_Chunk* LoadSoundEffect(const std::string& filePath); // 加载音效
    void PlaySoundEffect(Mix_Chunk* sound, int loops); // 播放音效
    void CalculateStereoPan(float sourceX, float sourceY, float listenerX, float listenerY, Uint8& left, Uint8& right); // 计算立体声平衡
    void CleanUp(); // 清理所有加载的音效
    void LoadSoundEffects(); // 预加载声音效果的函数
    Mix_Chunk* GetRandomSoundEffect(); // 随机获取声音效果的函数
    void FreeSoundEffects(); // 释放声音效果资源的函数
    Mix_Music* LoadMusic(const std::string& filePath); // 加载背景音乐
    void PlayMusic(Mix_Music* music, int loops); // 播放背景音乐
    void StopMusic(); // 停止背景音乐
    void PauseMusic(); // 暂停背景音乐
    void ResumeMusic(); // 恢复播放背景音乐

private:
    std::vector<Mix_Chunk*> soundEffects; // 存储声音效果
};

#endif // AUDIOMANAGER_H
