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
#include	<TrickPawn.h>
#include	<random>
#include	<Collision.h>
#include	<Background.h>
#include	<iostream>
#include	<BossPawn.h>
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
		mWindow = SDL_CreateWindow("Nobody", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scene_width, scene_height, 0);
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
			printf("TTF_Init: % s\n", TTF_GetError()); 
			return false;
		}

		// ����box2d����
		mWorld = new b2World(b2Vec2(0.0f, 0.0f));
		if (!mWorld) {
			SDL_Log("Failed to create mWorld");
			return false;
		}
		
		//// ����OpenGL
		//SDL_GLContext context = SDL_GL_CreateContext(mWindow);
		//if (!context) {
		//	SDL_Log("Failed to create OpenGL");
		//	return false;
		//}

		// �����������ģʽ
		// �����ģʽ�£�����λ�����������һ�ε�λ�õ�ƫ����
		// ����ζ����겻���������Ļ�ϣ�Ҳ�޷��ƶ�������
		SDL_SetRelativeMouseMode(SDL_TRUE);
		// ����OTF�����ļ�
		font = TTF_OpenFont("SDL2_ttf-2.20.2/Mantinia.otf", 50);
		// ��ȡ���ڱ���
		SDL_Surface* windowSurface = SDL_GetWindowSurface(mWindow);
		// ������ײ������
		Collision* contactListener = new Collision();
		mWorld->SetContactListener(contactListener);
		
		//������ӳ�ʼ��
		std::random_device rd;
		mRngEngine = std::mt19937(rd());

		// ��������
		LoadData();
		// ��ʼ�ȴ���Ұ��������
		StartWaiting();
		return true;
	}

	void Game::StartWaiting() {
		// ��Ⱦ��ʼ�ı�
		SDL_Color textColor = { 255, 255, 255, 255 };
		SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Press any key to start", textColor);
		SDL_Texture* textTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
		SDL_FreeSurface(textSurface);

		SDL_Rect textRect;
		SDL_QueryTexture(textTexture, NULL, NULL, &textRect.w, &textRect.h);
		textRect.x = (scene_width - textRect.w) / 2;
		textRect.y = (scene_height - textRect.h) / 2;

		// ���ƿ�ʼ�ı�
		SDL_RenderClear(mRenderer);
		SDL_RenderCopy(mRenderer, textTexture, NULL, &textRect);
		SDL_RenderPresent(mRenderer);

		// �ȴ��û����������
		bool gameStarted = false;
		while (!gameStarted)
		{
			SDL_Event event;
			while (SDL_PollEvent(&event))
			{
				switch (event.type)
				{
				case SDL_QUIT:
					mIsRunning = false;
					gameStarted = true;
					break;
				case SDL_KEYDOWN:
					gameStarted = true;
					mSpawnTimer = 0;//��ʱ������
					score = 0;  // ��������
					break;
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(3));
		}
	};

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

	void Game::ResetGame()
	{
		UnloadData();
		LoadData();
		StartWaiting();
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
	bool test = false;
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
			LimitUpBorder(gameObject);//upborder air wall
		}
		mSpawnTimer += Timer::deltaTime;
		if (mSpawnTimer >= 3.0f) {
			mSpawnTimer = 0.0f; // ���ü�ʱ��
			for (int i = 1; i < 2; i++) {
				Vector2 randomPosition = GenRandomPosition(Vector2(600, -300));
				//std::cout << randomPosition.x << randomPosition.y << endl;
				testEnemy = new Pawn(this, mWorld, randomPosition);
				mEnemies.push_back(testEnemy);
				randomPosition = GenRandomPosition(Vector2(300, -300));
				trickEnemy = new TrickPawn(this, mWorld, randomPosition);
				mEnemies.push_back(trickEnemy);
				if (!test) {
					bossEnemy = new BossPawn(this, mWorld, Vector2(500, -500), mEnemies);
					mEnemies.push_back(bossEnemy);
					test = true;
				}

			}
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
		// ɾ������״̬ΪEDead�ĵ���
		mEnemies.erase(
			std::remove_if(
				mEnemies.begin(),
				mEnemies.end(),
				[this](GameObject* enemy) { 
					if (enemy->GetState() == GameObject::State::EDead) {
						score++;  // ������ҵķ���
						return true;  // ɾ���������
					}
		return false;  // ��ɾ���������
				}
			),
			mEnemies.end()
					);
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

		RenderText(mRenderer, font, "ACCELERATE", textColor, textRect);
		// ������ת��Ϊ�ַ���
		std::string scoreText ="Score  " + std::to_string(score);

		// ʹ�� RenderText ������ָ����λ����ʾ����
		RenderText(mRenderer, font, scoreText, scoreColor, scoreTextRect);

		for (GameObject* enemy : mEnemies)
		{
			if (enemy->GetState() == GameObject::State::EActive)
			{
				enemy->Draw(mRenderer);
			}
			
			//SDL_RenderDrawCircle(mRenderer, enemy->GetPosition().x, enemy->GetPosition().y, 15);
			//SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 0);
			//SDL_Rect enemyLife = { enemy->GetPosition().x - enemy->GetLife()/2, enemy->GetPosition().y - 30,enemy->GetLife(), 3  };
			//SDL_RenderFillRect(mRenderer, &enemyLife);
		}
		// ����������
		SDL_RenderPresent(mRenderer);
	}

	void Game::LoadData()
	{

		LoadTexture("sprites/chrA07.png", "player");

		LoadTexture("sprites/chrA07.png", "BossPawn");
		LoadTexture("sprites/background_image.png", "background");
		// ��������
		mBackground = new Background(this, mWorld);
		
		mPlayer = new Player(this,mWorld);

		mBoundary = new Boundary(this, mWorld);
		//for (int i = 1; i < 3; i++) {
		//Vector2 randomPosition = GenRandomPosition(Vector2(600, 100));
		//std::cout << randomPosition.x << randomPosition.y << endl;
		//testEmeny = new Pawn(this, mWorld, randomPosition);
		//mEnemies.push_back(testEmeny);
		//}

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

		// �����µ�����ʱ��+`	1
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
	int Game::GetScreenWidth() {
		return scene_width;
	}

	int Game::GetScreenHeight() {
		return scene_height;
	}

	Player* Game::GetPlayer() const
	{
		return mPlayer;
	}
	float Game::GetDeltaTime() const
	{
		return Timer::deltaTime;
	}

	Vector2 Game::GenRandomPosition(const Vector2& playerPos) {
		// ����x��y����������Χ
		std::uniform_real_distribution<float> distX(playerPos.x - 100, playerPos.x + 100);
		std::uniform_real_distribution<float> distY(playerPos.y - 100, playerPos.y + 100);

		// �������x��y����
		float randomX = distX(mRngEngine);
		float randomY = distY(mRngEngine);

		// ����һ�����λ������
		Vector2 randomPosition(randomX, randomY);

		return randomPosition;
	}

	void Game::RenderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text,
		SDL_Color color, SDL_Rect& textRect) {
		SDL_Surface* textSurface = TTF_RenderUTF8_Solid(font, text.c_str(), color);
		SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

		// Always remember to free the surface after creating texture
		SDL_FreeSurface(textSurface);

		// Render the texture
		SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

		// Remember to destroy the texture after rendering
		SDL_DestroyTexture(textTexture);
	}

	void Game::LimitUpBorder(GameObject* gameObject) {
		try {
			RigidComponent* rigidComponent = gameObject->GetComponent<RigidComponent>();

			// Check if the game object has a RigidComponent
			if (rigidComponent) {
				b2Body* body = rigidComponent->GetmBody();

				// Make sure the body is valid and dynamic (can move)
				if (body && body->GetType() == b2_dynamicBody)
				{
					b2Vec2 velocity = body->GetLinearVelocity();

					// If object is moving upward and is at the top of the screen
					if (velocity.y < 0 && body->GetPosition().y <= 0 && body->GetPosition().y >= -10)
					{
						// Reverse its velocity along y axis
						velocity.y *= -1;
						body->SetLinearVelocity(velocity);
					}
				}
			}
		}
		catch (const std::exception& e) {
			std::cerr << "Caught exception in LimitObjectVelocity: " << e.what() << '\n';
		}
		catch (...) {
			std::cerr << "Caught unknown exception in LimitObjectVelocity\n";
		}
	}
}