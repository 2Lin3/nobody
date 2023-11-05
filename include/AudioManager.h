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

    bool Initialize(); // ��ʼ����Ƶϵͳ
    Mix_Chunk* LoadSoundEffect(const std::string& filePath); // ������Ч
    void PlaySoundEffect(Mix_Chunk* sound, int loops); // ������Ч
    void CalculateStereoPan(float sourceX, float sourceY, float listenerX, float listenerY, Uint8& left, Uint8& right); // ����������ƽ��
    void CleanUp(); // �������м��ص���Ч
    void LoadSoundEffects(); // Ԥ��������Ч���ĺ���
    Mix_Chunk* GetRandomSoundEffect(); // �����ȡ����Ч���ĺ���
    void FreeSoundEffects(); // �ͷ�����Ч����Դ�ĺ���
    Mix_Music* LoadMusic(const std::string& filePath); // ���ر�������
    void PlayMusic(Mix_Music* music, int loops); // ���ű�������
    void StopMusic(); // ֹͣ��������
    void PauseMusic(); // ��ͣ��������
    void ResumeMusic(); // �ָ����ű�������

private:
    std::vector<Mix_Chunk*> soundEffects; // �洢����Ч��
};

#endif // AUDIOMANAGER_H
