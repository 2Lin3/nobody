//! @file Game.cpp

#include	<SDL.h>
#include	<SDL_image.h>
#include	<Game.h>
#include	<memory>
#include	<GameObject.h>
#include	<SpriteComponent.h>
#include	<Player.h>
#include	<Timer.h>
#include	<Boundary.h>
#include	<RigidComponent.h>
#include	<chrono>
#include	<thread>
#include	<SDL_ttf.h>
#include	<Pawn.h>
#include	<Collision.h>
using namespace std;

namespace Nobody
{
	Game::Game() :
		mWindow(nullptr),
		mRenderer(nullptr),
		mIsRunning(true),
		mIsUpdating(false),
		mPlayer(nullptr),
		mBoundary(nullptr),
		mWorld(nullptr)
	{

	}

	bool Game::Initialize()
	{
		// SDL���ʼ��
		if (SDL_Init(SDL_INIT_VIDEO) != 0)
		{
			SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
			return false;
		}

		// ���ڳ�ʼ��
		mWindow = SDL_CreateWindow("Nobody", 400, 200, scene_width, scene_height, 0);
		if (!mWindow)
		{
			SDL_Log("Failed to create window: %s", SDL_GetError());
			return false;
		}

		// ��Ⱦ����ʼ��
		mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
		if (!mRenderer)
		{
			SDL_Log("Failed to create renderer: %s", SDL_GetError());
			return false;
		}

		// ��ʼ��SDL_image��
		if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
		{
			SDL_Log("Failed to initialize SDL_image: %s", SDL_GetError());
			return false;
		}

		//��ʼ���ֿ�
		if (TTF_Init() == -1) {
			printf("TTF_Init: % s\n", TTF_GetError()); return false;
		}

		// ����box2d����
		mWorld = new b2World(b2Vec2(0.0f, 0.0f));
		if (!mWorld) {
			SDL_Log("Failed to create mWorld");
			return false;
		}
		
		// �����������ģʽ
		// �����ģʽ�£�����λ�����������һ�ε�λ�õ�ƫ����
		// ����ζ����겻���������Ļ�ϣ�Ҳ�޷��ƶ�������
		SDL_SetRelativeMouseMode(SDL_TRUE);
		// ����OTF�����ļ�
		font = TTF_OpenFont("SDL2_ttf-2.20.2/Mantinia.otf", 200);
		// ��ȡ���ڱ���
		SDL_Surface* windowSurface = SDL_GetWindowSurface(mWindow);
		// ������ײ������
		Collision* contactListener = new Collision();
		mWorld->SetContactListener(contactListener);

		// ��������
		LoadData();

		return true;
	}

	void Game::Loop()
	{
		while (mIsRunning)
		{
			Event();
			Update();
			Draw();
			std::this_thread::sleep_for(std::chrono::milliseconds(3));
		}
	}

	void Game::Shutdown()
	{
		UnloadData();
		TTF_CloseFont(font);
		TTF_Quit();
		IMG_Quit();
		SDL_DestroyRenderer(mRenderer);
		SDL_DestroyWindow(mWindow);
		SDL_Quit();
		delete mWorld;
	}

	void Game::CreateGameObject(GameObject* gameObject)
	{
		// �����ǰ���ڸ��£����ö������ȴ���
		if (mIsUpdating)
		{
			mPendingObjects.push_back(gameObject);
		}
		else
		{
			mGameObjects.push_back(gameObject);
		}
	}

	void Game::RemoveGameObject(GameObject* gameObject)
	{
		// ���ڵȴ�����Ѱ�Ҳ��Ƴ�����
		auto iter = std::find(mPendingObjects.begin(), mPendingObjects.end(), gameObject);
		if (iter != mPendingObjects.end())
		{
			// ������ɾ��ĩβ��Ч�ʸ���erase�������ƻ�˳��
			std::iter_swap(iter, mPendingObjects.end() - 1);
			mPendingObjects.pop_back();
		}

		// ����ʽ��������Ѱ�Ҳ��Ƴ�����
		iter = std::find(mGameObjects.begin(), mGameObjects.end(), gameObject);
		if (iter != mGameObjects.end())
		{
			// ������ɾ��ĩβ��Ч�ʸ���erase�������ƻ�˳��
			std::iter_swap(iter, mGameObjects.end() - 1);
			mGameObjects.pop_back();
		}
	}

	void Game::CreateSprite(SpriteComponent* sprite)
	{
		int order = sprite->GetDrawOrder();
		// ���ջ���˳�����
		auto iter = mSprites.begin();
		for (; iter != mSprites.end(); ++iter)
		{
			if (order < (*iter)->GetDrawOrder())
			{
				break;
			}
		}
		mSprites.insert(iter, sprite);
	}

	void Game::RemoveSprite(SpriteComponent* sprite)
	{
		auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
		mSprites.erase(iter);
	}

	SDL_Texture* Game::GetTexture(const std::string& fileName)
	{
		SDL_Texture* tex = nullptr;
		auto iter = mTextures.find(fileName);
		if (iter != mTextures.end())
		{
			tex = iter->second;
		}
		return tex;
	}
	void Game::Event()
	{
		SDL_Event event;
		// ����¼���������δ������¼�����˳����
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				// �˳��¼������´��ڵĲ�
			case SDL_QUIT:
				mIsRunning = false;
				break;
			case SDL_MOUSEMOTION:
			{
				float mouse_x = event.motion.x;
				float mouse_y = event.motion.y;
				mIsUpdating = true;
				mPlayer->ProcessInputMouse(mouse_x, mouse_y);
				mIsUpdating = false;
				SDL_WarpMouseInWindow(mWindow, scene_width / 2, scene_height / 2);
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT) leftMousePressed = true;
				if (event.button.button == SDL_BUTTON_RIGHT) rightMousePressed = true;
				break;
			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT) leftMousePressed = false;
				if (event.button.button == SDL_BUTTON_RIGHT) rightMousePressed = false;
				break;
			default:
				break;
			}
		}

		// ��ȡ��������״̬
		const uint8_t* keyState = SDL_GetKeyboardState(NULL);

		// ������������������߼�
		mIsUpdating = true;
		for (auto gameObject : mGameObjects)
		{
			gameObject->ProcessInput(keyState);
			
		}
		int mouseX, mouseY;
		mIsUpdating = false;
	}
	int i = 1; //���� ����ɾ��
	void Game::Update()
	{
		// ����֡��
		Tick(60);
		//��ǰʱ��
		Uint32 currentTime = SDL_GetTicks();
		string currentTimeString = to_string(currentTime/1000);
		//cout << currentTimeString << endl;

		// ���¿�ʼ
		mIsUpdating = true;
		// ������ִ����������ĸ��º���
		for (auto gameObject : mGameObjects)
		{
			gameObject->Update();
		}
		// ���½���
		mIsUpdating = false;

		// ����Box2D����
		mWorld->Step(timeStep, velocityIterations, positionIterations);

		// �����еȴ��������ƶ�����ʽ��������
		for (auto pendingObject : mPendingObjects)
		{
			mGameObjects.emplace_back(pendingObject);
		}
		// ��յȴ���
		mPendingObjects.clear();
		if (leftMousePressed) mPlayer->ProcessInputMouseDown(SDL_BUTTON_LEFT, mPlayer->GetBoostValue());
		if (rightMousePressed) mPlayer->ProcessInputMouseDown(SDL_BUTTON_RIGHT, mPlayer->GetBoostValue());
		else mPlayer->ProcessInputMouseDown(SDL_BUTTON_MIDDLE, mPlayer->GetBoostValue());
		// ������״̬ΪEDead�����������������
		std::vector<GameObject*> deadObjects;
		for (auto deadObject : mGameObjects)
		{
			if (deadObject->GetState() == GameObject::State::EDead)
			{
				deadObjects.emplace_back(deadObject);
			}
		}
		// �ͷŵ�����������������
		for (auto deadObject : deadObjects)
		{
			delete deadObject;
		}
	}

	void Game::Draw()
	{
		// ������Ⱦ����ɫ������
		SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);

		// ���������
		SDL_RenderClear(mRenderer);
		// ���Ƶ���ͼ��
		mWorld->DebugDraw();

		// �������о���
		for (auto sprite : mSprites)
		{
			sprite->Draw(mRenderer);
		}
		SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 0);
		SDL_RenderDrawLine(mRenderer,0,750,1280,750);
		SDL_RenderDrawLine(mRenderer, 50, 0, 50, 800);
		SDL_RenderDrawLine(mRenderer, 1230, 0, 1230, 800);
		SDL_RenderDrawCircle(mRenderer, mPlayer->GetPosition().x, mPlayer->GetPosition().y,20);
		//���Ƽ�������
		SDL_Rect boostBar = { 225, scene_height-50, static_cast<int>(scene_width/1.5 * (static_cast<float>(*mPlayer->GetBoostValue()) / static_cast<float>(100))), 50 };
		SDL_RenderFillRect(mRenderer, &boostBar);

		//����������
		SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 0);
		SDL_Rect lifeBar = { 0, scene_height- mPlayer->GetLife() * 3 - 50,50, mPlayer->GetLife()*3};
		SDL_RenderFillRect(mRenderer, &lifeBar);

		SDL_Surface* textSurface = TTF_RenderUTF8_Solid(font, "ACCELERATE", textColor);
		SDL_Texture* textTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
		SDL_RenderCopy(mRenderer, textTexture, NULL, &textRect);
		for (Pawn* enemy : mEnemies)
		{
			SDL_RenderDrawCircle(mRenderer, enemy->GetPosition().x, enemy->GetPosition().y, 15);
			SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 0);
			SDL_Rect enemyLife = { enemy->GetPosition().x - enemy->GetLife()/2, enemy->GetPosition().y - 30,enemy->GetLife(), 3  };
			SDL_RenderFillRect(mRenderer, &enemyLife);
		}
		// ����������
		SDL_RenderPresent(mRenderer);
	}

	void Game::LoadData()
	{

		LoadTexture("sprites/chrA07.png", "player");

		mPlayer = new Player(this,mWorld);
		mBoundary = new Boundary(this, mWorld);
		for (int i = 1; i < 2; i++) {
		testEmeny = new Pawn(this, mWorld, mPlayer->GetPosition());
		mEnemies.push_back(testEmeny);
		}

	}

	void Game::Tick(int fps)
	{
		// �������õ�֡�ʼ���ÿ֡���ٵ�ʱ��
		int fpsTime = 1000 / fps;
		// �����һ֡������û�дﵽҪ��ʱ�䣬��ȴ�
		while (!SDL_TICKS_PASSED(SDL_GetTicks(), Timer::ticksCount + fpsTime))
		{
			SDL_Delay(1);
		}

		// �����µ�����ʱ��
		Timer::deltaTime = (SDL_GetTicks() - Timer::ticksCount) / 1000.0f;
		// �����µ�һ֡��ʼʱ����������ʱ��S
		Timer::ticksCount = SDL_GetTicks();
	}

	void Game::UnloadData()
	{
		// �ͷ�������Ϸ����
		while (!mGameObjects.empty())
		{
			delete mGameObjects.back();
		}

		// �ͷ�������ͼ��Դ
		for (auto tex : mTextures)
		{
			SDL_DestroyTexture(tex.second);
		}
		mTextures.clear();
	}

	void Game::LoadTexture(const std::string& fileName, const std::string& newName)
	{
		// �жϸ�keyֵ�Ƿ��ѱ�ʹ��
		auto iter = mTextures.find(newName);
		if (iter != mTextures.end())
		{
			SDL_DestroyTexture(iter->second);
			mTextures.erase(iter);
		}

		// ��ȡͼƬ��Ϊsurface��ʽ
		SDL_Surface* surf = IMG_Load(fileName.c_str());
		if (!surf)
		{
			SDL_Log("Failed to load texture file %s", fileName.c_str());
		}

		// ��surfaceת��Ϊtexture��ʽ
		SDL_Texture* tex = SDL_CreateTextureFromSurface(mRenderer, surf);
		// �ͷ�surface���ڴ�
		SDL_FreeSurface(surf);
		if (!tex)
		{
			SDL_Log("Failed to convert surface to texture for %s", fileName.c_str());
		}

		// ����ͼ���뵽��ϣ���У�������������Ϊkeyֵ
		mTextures.emplace(newName, tex);
	}
}