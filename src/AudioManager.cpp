// AudioManager.cpp

#include "AudioManager.h"
#include <iostream>
#include <cmath>
#include <SDL.h>
#include <algorithm> 
using namespace std;
const float MAX_SOUND_DISTANCE = 1000.0f;

AudioManager::AudioManager() {
    // ���캯��
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
    // ������Դ������֮��ĽǶȻ���ˮƽ����
    float angle = atan2(listenerY - sourceY, listenerX - sourceX);
    float distance = sqrt((listenerX - sourceX) * (listenerX - sourceX) + (listenerY - sourceY) * (listenerY - sourceY));
    float volume = std::max(0.0f, 1.0f - distance / MAX_SOUND_DISTANCE); // ��������һ������������볣��

    left = static_cast<Uint8>(volume * (0.5f * (1.0f - cos(angle))));
    right = static_cast<Uint8>(volume * (0.5f * (1.0f + cos(angle))));
}

void AudioManager::CleanUp() {
    // ������룬�ͷ�����Mix_Chunk����Դ
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
    if (Mix_PlayingMusic() == 0) { // �����ǰû�����ֲ���
        Mix_PlayMusic(music, loops); // loops = -1 ������ѭ��
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
    // ���������ļ�����������ӵ�������
    soundEffects.push_back(Mix_LoadWAV("music/Sound effects/1.wav"));
    soundEffects.push_back(Mix_LoadWAV("music/Sound effects/2.wav"));
    soundEffects.push_back(Mix_LoadWAV("music/Sound effects/3.wav"));

    // ����Ƿ��м���ʧ�ܵ������ļ�
    for (auto* effect : soundEffects) {
        if (!effect) {
            // �����������ӡ��־
            std::cerr << "Failed to load sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
        }
    }
}

Mix_Chunk* AudioManager::GetRandomSoundEffect() {
    if (soundEffects.empty()) {
        return nullptr; // ���û�м�������Ч�������ؿ�
    }
    int index = rand() % soundEffects.size(); // ���ѡ��һ������
    return soundEffects[index]; // �����������Ч��
}
void AudioManager::FreeSoundEffects() {
    for (auto* effect : soundEffects) {
        Mix_FreeChunk(effect);  // �ͷ���������
    }
    soundEffects.clear();  // �������
}



