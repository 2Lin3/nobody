//! @file Game.h

#ifndef __Nobody_Game__
#define __Nobody_Game__

#include	<string>
#include	<vector>
#include	<unordered_map>
#include	<box2d/box2d.h>
#include	<SDL_ttf.h>
#include	<memory>


struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

const int scene_width = 1280;
const int scene_height = 800;	//!<	���ڹ��
const float SCALE = 10.f;		//!<	�������׶�Ӧ����
namespace Nobody
{
	class GameObject;
	class SpriteComponent;
	class Player;
	class Boundary;
	class FooDraw;
	class Pawn;
	class Collision;

	class Game
	{
	public:
		//!	���캯��
		Game();

		//!	��ʼ��
		bool	Initialize();
		//!	��ѭ��
		void	Loop();
		//!	��Ϸ����
		void	Shutdown();

		//! �������洢��Ϸ����
		void	CreateGameObject(GameObject* gameObject);
		//! �Ƴ���Ϸ����
		void	RemoveGameObject(GameObject* gameObject);
		//! ������������˳��洢����
		void	CreateSprite(SpriteComponent* sprite);
		//! �Ƴ�����
		void	RemoveSprite(SpriteComponent* sprite);
		//! ��ȡ��ͼ
		SDL_Texture* GetTexture(const std::string& fileName);


	private:
		//!	�����¼�
		void	Event();
		//!	���¸���״̬
		void	Update();
		//!	��Ⱦ
		void	Draw();
		//! ����֡��
		void	Tick(int fps);
		//! ��������
		void	LoadData();
		//!	�ͷ�����
		void	UnloadData();
		//!	������ͼ��������
		void	LoadTexture(const std::string& fileName, const std::string& newName);

		std::vector<GameObject*> mGameObjects;		//!<	��Ϸ��������
		std::vector<GameObject*> mPendingObjects;	//!<	�ȴ�״̬����Ϸ��������
		std::vector<SpriteComponent*> mSprites;		//!<	��ž��������
		std::vector<Pawn*> mEnemies;				//!<	��ŵ��˵�����


		bool leftMousePressed = false;
		bool rightMousePressed = false;

		std::unordered_map<std::string, SDL_Texture*> mTextures;	//!<	�����ͼ������

		SDL_Window* mWindow;		//!<	����
		SDL_Surface* windowSurface; //!<	���ڱ���
		TTF_Font* font;				//!<	����
		SDL_Color textColor = { 255, 100, 0 };	//!<	������ɫ
		SDL_Rect textRect = { 640, 750, 100, 50 };	//!<	�����������λ��

		SDL_Renderer* mRenderer;		//!<	��Ⱦ��
		bool			mIsRunning;		//!<	����״̬
		bool			mIsUpdating;	//!<	�Ƿ��ڸ���״̬

		Player* mPlayer;		//!<	��ҽ�ɫ
		Boundary* mBoundary;		//!<	�߽�

		//���Ե���
		Pawn* testEmeny = nullptr;
		// Box2D����
		b2World* mWorld;
		// ָ��ÿ��ģ��Ĳ���
		const float timeStep = 1.0f / 60.0f;
		// ��������
		const int velocityIterations = 6;
		const int positionIterations = 2;

		//string currentTimeString;		//!<	�ۼ�ʱ��
	};
}

#endif