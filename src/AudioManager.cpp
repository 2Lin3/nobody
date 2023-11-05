// AudioManager.cpp

#include "AudioManager.h"
#include <iostream>
#include <cmath>
#include <SDL.h>
#include <algorithm> 
using namespace std;
const float MAX_SOUND_DISTANCE = 1000.0f;

AudioManager::AudioManager() {
    // 构造函数
}

AudioManager::~AudioManager() {
    CleanUp();
}

bool AudioManager::Initialize() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return false;
    }
    return true;
}

Mix_Chunk* AudioManager::LoadSoundEffect(const std::string& filePath) {
    Mix_Chunk* sound = Mix_LoadWAV(filePath.c_str());
    if (sound == NULL) {
        SDL_Log("Failed to load sound effect! SDL_mixer Error: %s\n", Mix_GetError());
    }
    return sound;
}

void AudioManager::PlaySoundEffect(Mix_Chunk* sound, int loops) {
    if (sound != NULL) {
        Mix_PlayChannel(-1, sound, loops);
    }
}

void AudioManager::CalculateStereoPan(float sourceX, float sourceY, float listenerX, float listenerY, Uint8& left, Uint8& right) {
    // 计算声源和听众之间的角度或者水平距离
    float angle = atan2(listenerY - sourceY, listenerX - sourceX);
    float distance = sqrt((listenerX - sourceX) * (listenerX - sourceX) + (listenerY - sourceY) * (listenerY - sourceY));
    float volume = std::max(0.0f, 1.0f - distance / MAX_SOUND_DISTANCE); // 假设你有一个最大声音距离常数

    left = static_cast<Uint8>(volume * (0.5f * (1.0f - cos(angle))));
    right = static_cast<Uint8>(volume * (0.5f * (1.0f + cos(angle))));
}

void AudioManager::CleanUp() {
    // 清理代码，释放所有Mix_Chunk等资源
    Mix_CloseAudio();
}

// ...

Mix_Music* AudioManager::LoadMusic(const std::string& filePath) {
    Mix_Music* music = Mix_LoadMUS(filePath.c_str());
    if (!music) {
        SDL_Log("Failed to load background music! SDL_mixer Error: %s\n", Mix_GetError());
    }
    return music;
}

void AudioManager::PlayMusic(Mix_Music* music, int loops) {
    if (Mix_PlayingMusic() == 0) { // 如果当前没有音乐播放
        Mix_PlayMusic(music, loops); // loops = -1 是无限循环
    }
}

void AudioManager::StopMusic() {
    Mix_HaltMusic();
}

void AudioManager::PauseMusic() {
    if (Mix_PlayingMusic() != 0) {
        Mix_PauseMusic();
    }
}

void AudioManager::ResumeMusic() {
    if (Mix_PausedMusic() != 0) {
        Mix_ResumeMusic();
    }
}

void AudioManager::LoadSoundEffects() {
    // 加载声音文件并将它们添加到向量中
    soundEffects.push_back(Mix_LoadWAV("music/Sound effects/1.wav"));
    soundEffects.push_back(Mix_LoadWAV("music/Sound effects/2.wav"));
    soundEffects.push_back(Mix_LoadWAV("music/Sound effects/3.wav"));

    // 检查是否有加载失败的声音文件
    for (auto* effect : soundEffects) {
        if (!effect) {
            // 错误处理，例如打印日志
            std::cerr << "Failed to load sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
        }
    }
}

Mix_Chunk* AudioManager::GetRandomSoundEffect() {
    if (soundEffects.empty()) {
        return nullptr; // 如果没有加载声音效果，返回空
    }
    int index = rand() % soundEffects.size(); // 随机选择一个索引
    return soundEffects[index]; // 返回随机声音效果
}
void AudioManager::FreeSoundEffects() {
    for (auto* effect : soundEffects) {
        Mix_FreeChunk(effect);  // 释放声音样本
    }
    soundEffects.clear();  // 清空向量
}



