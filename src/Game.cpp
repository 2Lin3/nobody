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
		// SDL库初始化
		if (SDL_Init(SDL_INIT_VIDEO) != 0)
		{
			SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
			return false;
		}

		// 窗口初始化
		mWindow = SDL_CreateWindow("Nobody", 400, 200, scene_width, scene_height, 0);
		if (!mWindow)
		{
			SDL_Log("Failed to create window: %s", SDL_GetError());
			return false;
		}

		// 渲染器初始化
		mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
		if (!mRenderer)
		{
			SDL_Log("Failed to create renderer: %s", SDL_GetError());
			return false;
		}

		// 初始化SDL_image库
		if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
		{
			SDL_Log("Failed to initialize SDL_image: %s", SDL_GetError());
			return false;
		}

		//初始化字库
		if (TTF_Init() == -1) {
			printf("TTF_Init: % s\n", TTF_GetError()); return false;
		}

		// 建立box2d世界
		mWorld = new b2World(b2Vec2(0.0f, 0.0f));
		if (!mWorld) {
			SDL_Log("Failed to create mWorld");
			return false;
		}
		
		// 设置鼠标的相对模式
		// 在相对模式下，鼠标的位置是相对于上一次的位置的偏移量
		// 这意味着鼠标不会出现在屏幕上，也无法移动出窗口
		SDL_SetRelativeMouseMode(SDL_TRUE);
		// 加载OTF字体文件
		font = TTF_OpenFont("SDL2_ttf-2.20.2/Mantinia.otf", 200);
		// 获取窗口表面
		SDL_Surface* windowSurface = SDL_GetWindowSurface(mWindow);
		// 设置碰撞监听器
		Collision* contactListener = new Collision();
		mWorld->SetContactListener(contactListener);

		// 加载数据
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
		// 如果当前正在更新，将该对象加入等待区
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
		// 先在等待区中寻找并移除物体
		auto iter = std::find(mPendingObjects.begin(), mPendingObjects.end(), gameObject);
		if (iter != mPendingObjects.end())
		{
			// 交换后删除末尾，效率高于erase，但会破坏顺序
			std::iter_swap(iter, mPendingObjects.end() - 1);
			mPendingObjects.pop_back();
		}

		// 在正式物体区中寻找并移除物体
		iter = std::find(mGameObjects.begin(), mGameObjects.end(), gameObject);
		if (iter != mGameObjects.end())
		{
			// 交换后删除末尾，效率高于erase，但会破坏顺序
			std::iter_swap(iter, mGameObjects.end() - 1);
			mGameObjects.pop_back();
		}
	}

	void Game::CreateSprite(SpriteComponent* sprite)
	{
		int order = sprite->GetDrawOrder();
		// 按照绘制顺序插入
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
		// 如果事件队列中有未处理的事件，按顺序处理
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				// 退出事件，按下窗口的叉
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

		// 获取键盘输入状态
		const uint8_t* keyState = SDL_GetKeyboardState(NULL);

		// 处理所有物体的输入逻辑
		mIsUpdating = true;
		for (auto gameObject : mGameObjects)
		{
			gameObject->ProcessInput(keyState);
			
		}
		int mouseX, mouseY;
		mIsUpdating = false;
	}
	int i = 1; //调试 后续删除
	void Game::Update()
	{
		// 设置帧率
		Tick(60);
		//当前时间
		Uint32 currentTime = SDL_GetTicks();
		string currentTimeString = to_string(currentTime/1000);
		//cout << currentTimeString << endl;

		// 更新开始
		mIsUpdating = true;
		// 遍历并执行所有物体的更新函数
		for (auto gameObject : mGameObjects)
		{
			gameObject->Update();
		}
		// 更新结束
		mIsUpdating = false;

		// 更新Box2D世界
		mWorld->Step(timeStep, velocityIterations, positionIterations);

		// 将所有等待区物体移动至正式的物体区
		for (auto pendingObject : mPendingObjects)
		{
			mGameObjects.emplace_back(pendingObject);
		}
		// 清空等待区
		mPendingObjects.clear();
		if (leftMousePressed) mPlayer->ProcessInputMouseDown(SDL_BUTTON_LEFT, mPlayer->GetBoostValue());
		if (rightMousePressed) mPlayer->ProcessInputMouseDown(SDL_BUTTON_RIGHT, mPlayer->GetBoostValue());
		else mPlayer->ProcessInputMouseDown(SDL_BUTTON_MIDDLE, mPlayer->GetBoostValue());
		// 将所有状态为EDead的物体添加至死亡区
		std::vector<GameObject*> deadObjects;
		for (auto deadObject : mGameObjects)
		{
			if (deadObject->GetState() == GameObject::State::EDead)
			{
				deadObjects.emplace_back(deadObject);
			}
		}
		// 释放掉所有死亡区的物体
		for (auto deadObject : deadObjects)
		{
			delete deadObject;
		}
	}

	void Game::Draw()
	{
		// 设置渲染器颜色（蓝）
		SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);

		// 清除缓冲区
		SDL_RenderClear(mRenderer);
		// 绘制调试图形
		mWorld->DebugDraw();

		// 绘制所有精灵
		for (auto sprite : mSprites)
		{
			sprite->Draw(mRenderer);
		}
		SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 0);
		SDL_RenderDrawLine(mRenderer,0,750,1280,750);
		SDL_RenderDrawLine(mRenderer, 50, 0, 50, 800);
		SDL_RenderDrawLine(mRenderer, 1230, 0, 1230, 800);
		SDL_RenderDrawCircle(mRenderer, mPlayer->GetPosition().x, mPlayer->GetPosition().y,20);
		//绘制加速气槽
		SDL_Rect boostBar = { 225, scene_height-50, static_cast<int>(scene_width/1.5 * (static_cast<float>(*mPlayer->GetBoostValue()) / static_cast<float>(100))), 50 };
		SDL_RenderFillRect(mRenderer, &boostBar);

		//绘制生命槽
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
		// 交换缓冲区
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
		// 根据设置的帧率计算每帧至少的时间
		int fpsTime = 1000 / fps;
		// 如果上一帧结束后还没有达到要求时间，则等待
		while (!SDL_TICKS_PASSED(SDL_GetTicks(), Timer::ticksCount + fpsTime))
		{
			SDL_Delay(1);
		}

		// 计算新的增量时间
		Timer::deltaTime = (SDL_GetTicks() - Timer::ticksCount) / 1000.0f;
		// 计算新的一帧开始时所经过的总时间S
		Timer::ticksCount = SDL_GetTicks();
	}

	void Game::UnloadData()
	{
		// 释放所有游戏物体
		while (!mGameObjects.empty())
		{
			delete mGameObjects.back();
		}

		// 释放所有贴图资源
		for (auto tex : mTextures)
		{
			SDL_DestroyTexture(tex.second);
		}
		mTextures.clear();
	}

	void Game::LoadTexture(const std::string& fileName, const std::string& newName)
	{
		// 判断该key值是否已被使用
		auto iter = mTextures.find(newName);
		if (iter != mTextures.end())
		{
			SDL_DestroyTexture(iter->second);
			mTextures.erase(iter);
		}

		// 读取图片存为surface形式
		SDL_Surface* surf = IMG_Load(fileName.c_str());
		if (!surf)
		{
			SDL_Log("Failed to load texture file %s", fileName.c_str());
		}

		// 将surface转换为texture形式
		SDL_Texture* tex = SDL_CreateTextureFromSurface(mRenderer, surf);
		// 释放surface的内存
		SDL_FreeSurface(surf);
		if (!tex)
		{
			SDL_Log("Failed to convert surface to texture for %s", fileName.c_str());
		}

		// 将贴图加入到哈希表中，并将新名字作为key值
		mTextures.emplace(newName, tex);
	}
}