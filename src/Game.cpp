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
#include    <SDL_mixer.h>

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
		// SDL库初始化 // English: SDL library initialization
		if (SDL_Init(SDL_INIT_VIDEO) != 0)
		{	
			SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
			return false;
		}

		// 窗口初始化 // English: Window initialization
		mWindow = SDL_CreateWindow("Nobody", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scene_width, scene_height, 0);
		if (!mWindow)
		{
			SDL_Log("Failed to create window: %s", SDL_GetError());
			return false;
		}

		// 渲染器初始化 // English: Renderer initialization 
		mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
		if (!mRenderer)
		{
			SDL_Log("Failed to create renderer: %s", SDL_GetError());
			return false;
		}

		// 初始化SDL_image库 // English: Initialize the SDL_image library
		if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
		{
			SDL_Log("Failed to initialize SDL_image: %s", SDL_GetError());
			return false;
		}

		//初始化字库 // English: Initialize the font library
		if (TTF_Init() == -1) {
			printf("TTF_Init: % s\n", TTF_GetError()); 
			return false;
		}

		// 初始化音频管理器 // English: Initialize audio manager
		if (!mAudioManager.Initialize()) {
			SDL_Log("Failed to initialize AudioManager");
			return false;
		}

		// 建立box2d世界 // English: Build box2d world
		mWorld = new b2World(b2Vec2(0.0f, 0.0f));
		if (!mWorld) {
			SDL_Log("Failed to create mWorld");
			return false;
		}
		
		// item初始化 // English: item initialization
		try
		{
			for (int i = 0; i < static_cast<int>(BoostType::End); ++i) {
				items.push_back(Item(static_cast<BoostType>(i)));
			}
		}
		catch (const std::exception&)
		{
			SDL_Log("Failed to create items");
			return false;
		}

		//// 创建OpenGL
		//SDL_GLContext context = SDL_GL_CreateContext(mWindow);
		//if (!context) {
		//	SDL_Log("Failed to create OpenGL");
		//	return false;
		//}

		// 设置鼠标的相对模式 // English: Set the mouse to relative mode
		// 在相对模式下，鼠标的位置是相对于上一次的位置的偏移量
		// 这意味着鼠标不会出现在屏幕上，也无法移动出窗口
		SDL_SetRelativeMouseMode(SDL_TRUE);
		// 加载OTF字体文件 // English: Load OTF font file
		font = TTF_OpenFont("SDL2_ttf-2.20.2/Mantinia.otf", 50);
		// 获取窗口表面 // English: Get window surface
		SDL_Surface* windowSurface = SDL_GetWindowSurface(mWindow);
		// 设置碰撞监听器 // English: Set collision listener
		Collision* contactListener = new Collision(&mAudioManager);
		mWorld->SetContactListener(contactListener);
		
		//随机种子初始化 // English: Random seed initialization
		std::random_device rd;
		mRngEngine = std::mt19937(rd());

		// 加载数据 // English: Load data
		LoadData();
		// 开始等待玩家按下任意键 // English: Start waiting for the player to press any key
		StartWaiting();
		return true;
	}

	void Game::StartWaiting() {
		// 渲染开始文本 // English: Render start text
		SDL_Color textColor = { 255, 255, 255, 255 };
		SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Press any key to start", textColor);
		SDL_Texture* textTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
		SDL_FreeSurface(textSurface);

		SDL_Rect textRect;
		SDL_QueryTexture(textTexture, NULL, NULL, &textRect.w, &textRect.h);
		textRect.x = (scene_width - textRect.w) / 2;
		textRect.y = (scene_height - textRect.h) / 2;

		// 绘制开始文本 // English: Draw the start text
		SDL_RenderClear(mRenderer);
		SDL_RenderCopy(mRenderer, textTexture, NULL, &textRect);
		SDL_RenderPresent(mRenderer);

		// 等待用户按下任意键 // English: Wait for the user to press any key
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
					SDL_DestroyTexture(textTexture);//清理创建的纹理 // English: Clean up the created texture
					mSpawnTimer = 0;//计时器归零 // English: Timer zeroing
					score = 0;  // 分数归零	// English: Score zeroing
					break;
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(3));
		}
	};

	void Game::RenderDeathMessage()
	{
		SDL_Color color = { 0, 0, 0, 255 };  
		SDL_Surface* textSurface = TTF_RenderText_Solid(font, "snuff out", color);
		SDL_Texture* textTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
		SDL_FreeSurface(textSurface);
		SDL_Rect textRect;
		SDL_QueryTexture(textTexture, NULL, NULL, &textRect.w, &textRect.h);
		textRect.x = (scene_width - textRect.w) / 2;
		textRect.y = (scene_height - textRect.h) / 2;

		/// 绘制噶掉文本 // English: Draw the text
		SDL_RenderCopy(mRenderer, textTexture, NULL, &textRect);
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

	void Game::ResetGame()
	{
		UnloadData();
		LoadData();
		StartWaiting();
	}


	void Game::Shutdown()
	{
		UnloadData();
		AudioManager::Instance().FreeSoundEffects();
		Mix_FreeMusic(bgMusic);
		mAudioManager.CleanUp();
		Mix_Quit();
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
		// 如果当前正在更新，将该对象加入等待区 // English: If the current update is in progress, add the object to the waiting area
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
		// 先在等待区中寻找并移除物体 // English: First find and remove objects in the waiting area
		auto iter = std::find(mPendingObjects.begin(), mPendingObjects.end(), gameObject);
		if (iter != mPendingObjects.end())
		{
			// 交换后删除末尾，效率高于erase，但会破坏顺序 // English: Swap and delete the end, which is more efficient than erase, but will destroy the order
			std::iter_swap(iter, mPendingObjects.end() - 1);
			mPendingObjects.pop_back();
		}

		/// 在正式物体区中寻找并移除物体 // English: Find and remove objects in the formal object area
		iter = std::find(mGameObjects.begin(), mGameObjects.end(), gameObject);
		if (iter != mGameObjects.end())
		{
			// 交换后删除末尾，效率高于erase，但会破坏顺序 // English: Swap and delete the end, which is more efficient than erase, but will destroy the order
			std::iter_swap(iter, mGameObjects.end() - 1);
			mGameObjects.pop_back();
		}
	}

	void Game::CreateSprite(SpriteComponent* sprite)
	{
		int order = sprite->GetDrawOrder();
		// 按照绘制顺序插入 // English: Insert according to the drawing order
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
		// 如果事件队列中有未处理的事件，按顺序处理 // English: If there are unprocessed events in the event queue, process them in order
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				// 退出事件，按下窗口的叉 或者 按下ESC键 时触发 // English: Exit event, press the cross of the window or press the ESC key to trigger
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
				if (!mIsDrawingChoosingProps) {
					SDL_WarpMouseInWindow(mWindow, scene_width / 2, scene_height / 2);
				}
				else
				{
					SDL_Point mMousePos = { event.motion.x, event.motion.y };
					// 选中道具的矩形 // English: Rect of selected items
					for (int i = 0; i < 3; ++i) {
						mIsHovered[i] = SDL_PointInRect(&mMousePos, &itemRects[i]);
					}
				}

				break;
			}
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT) {
					leftMousePressed = true;
					if (mIsDrawingChoosingProps) {
						for (int i = 0; i < 3; ++i) {
							selectedItems[i].ApplyToPlayer(*mPlayer);
							mIsDrawingChoosingProps = false;
							SDL_ShowCursor(SDL_DISABLE);
							SDL_SetRelativeMouseMode(SDL_TRUE);
						}
					}
				}
				if (event.button.button == SDL_BUTTON_RIGHT) rightMousePressed = true;

				break;
			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT) leftMousePressed = false;
				if (event.button.button == SDL_BUTTON_RIGHT) rightMousePressed = false;
				break;

			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					mIsPaused = !mIsPaused;
				}
				break;
			default:
				break;
			}
		}

		// 获取键盘输入状态		// English: Get keyboard input status
		const uint8_t* keyState = SDL_GetKeyboardState(NULL);

		// 处理所有物体的输入逻辑 // English: Process the input logic of all objects
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
		// 如果游戏暂停，直接返回 // English: If the game is paused, return directly
		if (mIsPaused) return;
		mAudioManager.ResumeMusic();
		// 设置帧率 // English: Set frame rate
		Tick(60);
		if (mIsChoosingProps) {
			// 显示鼠标 // English: Show mouse
			SDL_SetRelativeMouseMode(SDL_FALSE);
			SDL_ShowCursor(SDL_ENABLE);
			std::shuffle(items.begin(), items.end(), mRngEngine);
			selectedItems.assign(items.begin(), items.begin() + 3);
			mIsDrawingChoosingProps = true;
			mIsChoosingProps = false;
			for (int i = 0; i < 3; ++i) {
				mIsHovered[i] = false;
			}
		}
		if (mIsDrawingChoosingProps) return;

		//当前时间 // English: Current time
		Uint32 currentTime = SDL_GetTicks();
		string currentTimeString = to_string(currentTime/1000);
		//cout << currentTimeString << endl;
		if (needUpdate) {
			currentLevel++;
			enemiesToSpawn = 6 + currentLevel * 4; 
			enemiesDead = 0;
			needUpdate = false;
		}
		// 更新开始 // English: Update start
		mIsUpdating = true;
		// 遍历并执行所有物体的更新函数 // English: Traverse and execute the update function of all objects
		for (auto gameObject : mGameObjects)
		{
			gameObject->Update();
			LimitUpBorder(gameObject);//upborder air wall
		}
		mSpawnTimer += Timer::deltaTime;
		if (mSpawnTimer >= 6.0f && enemiesToSpawn) {
			mSpawnTimer = 0.0f; // 重置计时器 // English: Reset timer
			Vector2 randomPosition = GenRandomPosition(Vector2(600, -300));
			//std::cout << randomPosition.x << randomPosition.y << endl;
			//testEnemy = new Pawn(this, mWorld, randomPosition);
			//mEnemies.push_back(testEnemy);
			randomPosition = GenRandomPosition(Vector2(300, -300));
			trickEnemy = new TrickPawn(this, mWorld, randomPosition);
			mEnemies.push_back(trickEnemy);
			enemiesToSpawn--;
			if (!test && currentLevel >= 3) {
				bossEnemy = new BossPawn(this, mWorld, Vector2(500, -500), mEnemies);
				mEnemies.push_back(bossEnemy);
				test = true;
			}
		}

		if (mPlayer->GetLife() <= 0)
		{	
			mIsDEAD = true;
		}

		// 更新Box2D世界 // English: Update Box2D world
		mWorld->Step(timeStep, velocityIterations, positionIterations);

		// 更新结束 // English: Update end
		mIsUpdating = false;


		// 将所有等待区物体移动至正式的物体区 // English: Move all waiting area objects to the formal object area
		for (auto pendingObject : mPendingObjects)
		{
			mGameObjects.emplace_back(pendingObject);
		}
		// 清空等待区 // English: Clear the waiting area
		mPendingObjects.clear();
		if (leftMousePressed) mPlayer->ProcessInputMouseDown(SDL_BUTTON_LEFT, mPlayer->GetBoostValue());
		if (rightMousePressed) mPlayer->ProcessInputMouseDown(SDL_BUTTON_RIGHT, mPlayer->GetBoostValue());
		else mPlayer->ProcessInputMouseDown(SDL_BUTTON_MIDDLE, mPlayer->GetBoostValue());

		// 将所有状态为EDead的物体添加至死亡区 // English: Add all objects with a state of EDead to the death zone
		std::vector<GameObject*> deadObjects;
		for (auto deadObject : mGameObjects)
		{
			if (deadObject->GetState() == GameObject::State::EDead)
			{
				deadObjects.emplace_back(deadObject);
			}
		}
		// 删除所有状态为EDead的敌人 // English: Delete all enemies with a state of EDead
		mEnemies.erase(
			std::remove_if(
				mEnemies.begin(),
				mEnemies.end(),
				[this](GameObject* enemy) { 
					if (enemy->GetState() == GameObject::State::EDead) {
						score++;  
						mPlayer->SetExp(*mPlayer->GetExp() + 1); 
						enemiesDead++;
						if(enemiesDead == currentLevel * 4 + 6 )needUpdate = true;
						return true;  
					}
		return false;  
				}
			),
			mEnemies.end()
					);
		// 释放掉所有死亡区的物体	// English: Release all objects in the death zone
		for (auto deadObject : deadObjects)
		{
			if (deadObject->isPlayer) continue;
			delete deadObject;
		}
	}

	void Game::Draw()
	{
		// 设置渲染器颜色（蓝） // English: Set renderer color (blue)
		SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);

		// 清除缓冲区	 // English: Clear the buffer
		SDL_RenderClear(mRenderer);
		// 绘制调试图形 // English: Draw debug graphics
		mWorld->DebugDraw();
		// 恢复混合模式 // English: Restore blending mode
		SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_NONE);
		// 绘制所有精灵 // English: Draw all sprites
		for (auto sprite : mSprites)
		{
			sprite->Draw(mRenderer);
		}
		SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 0);
		SDL_RenderDrawLine(mRenderer,0,750,1280,750);
		SDL_RenderDrawLine(mRenderer, 50, 0, 50, 800);
		SDL_RenderDrawLine(mRenderer, 1230, 0, 1230, 800);
		SDL_RenderDrawCircle(mRenderer, mPlayer->GetPosition().x, mPlayer->GetPosition().y,20);
		//绘制加速气槽 // English: Draw boost bar
		int boostValue = static_cast<int>(scene_width / 1.5 * (static_cast<float>(*mPlayer->GetBoostValue()) / static_cast<float>(100)));
		boostValue = std::max(0, boostValue);
		SDL_Rect boostBar = { 225, scene_height-50, boostValue, 50 };
		SDL_RenderFillRect(mRenderer, &boostBar);
		 
		//绘制生命槽 // English: Draw life bar
		SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 0);
		SDL_Rect lifeBar = { 0, scene_height- mPlayer->GetLife() * 3 - 50,50, mPlayer->GetLife()*3};
		SDL_RenderFillRect(mRenderer, &lifeBar);

		RenderText(mRenderer, font, "ACCELERATE", textColor, textRect);

		// 绘制蓝色Exp条		// English: Draw blue Exp bar
		SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 0);
		SDL_Rect expBarFrame = { scene_width - 50, scene_height - 300 - 50,50, 300 };
		SDL_RenderDrawRect(mRenderer, &expBarFrame);
		int expValue = static_cast<int>(300 * (static_cast<float>(*mPlayer->GetExp()) / static_cast<float>(*mPlayer->GetNeededExp())));
		expValue = std::max(0, expValue);
		SDL_Rect expBar = { scene_width - 50, scene_height - expValue - 50,50, expValue };
		SDL_RenderFillRect(mRenderer, &expBar);

		SDL_Rect expTextRect = { scene_width - 50, scene_height - 300 - 50 - 50,50, 50 };
		RenderText(mRenderer, font, "Exp", textColor, expTextRect);
		
		// 将分数转换为字符串 // English: Convert the score to a string
		std::string scoreText ="Score  " + std::to_string(score);
		std::string levelText = "Current Wave  " + std::to_string(currentLevel);
		// 使用 RenderText 函数在指定的位置显示分数 // English: Use the RenderText function to display the score at the specified position
		RenderText(mRenderer, font, scoreText, scoreColor, scoreTextRect);
		RenderText(mRenderer, font, levelText, scoreColor, levelTextRect);
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


		if (mIsDEAD) {

			// 显示死亡信息 // English: Display death information
			RenderDeathMessage();
			// 绘制一个透明度逐渐增加的黑色矩形来实现渐变效果 // English: Draw a black rectangle with increasing opacity to achieve the gradient effect

			SDL_Rect rect = { 0, 0, scene_width, scene_height };
			SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, mFadeAlpha);  // 假设mFadeAlpha是当前的透明度 // English: Assume mFadeAlpha is the current opacity
			SDL_RenderFillRect(mRenderer, &rect);

			// 透明度逐渐增加
			mFadeAlpha += 255 / (3.5f * 60);  // 假设每秒60帧 // English: The opacity gradually increases

			// 渐变结束，重启游戏 // English: The gradient ends and the game restarts
			if (mFadeAlpha >= 255)
			{
				mFadeAlpha = 0;
				// 重启游戏 
				ResetGame();
				mIsDEAD = false;
			}
		}
		if (mIsDrawingChoosingProps)	//绘制道具选择界面 // English: Draw the props selection interface
		{
			int fontSize = 48;  
			TTF_Font* fontTemp = TTF_OpenFont("SDL2_ttf-2.20.2/Mantinia.otf", fontSize);

			for (int i = 0; i < 3; ++i) {
				
				SDL_Rect itemRect = { (scene_width - 3 * 200 - 2 * 50) / 2 + (200 + 50) * i, (scene_height - 400) / 2, 200, 400 };
				itemRects[i] = itemRect;
				if (mIsHovered[i]) {
					SDL_SetRenderDrawColor(mRenderer, blackColor.r, blackColor.g, blackColor.b, blackColor.a);
				}
				else {
					SDL_SetRenderDrawColor(mRenderer, grayColor.r, grayColor.g, grayColor.b, grayColor.a);
				}
				SDL_RenderFillRect(mRenderer, &itemRect);

				SDL_Surface* textSurface = NULL;
				SDL_Texture* textTexture = NULL;
				SDL_Rect textRect;
				do {
					textSurface = TTF_RenderText_Solid(fontTemp, selectedItems[i].getName().c_str(), whiteColor);
					textTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
					SDL_QueryTexture(textTexture, NULL, NULL, &textRect.w, &textRect.h);
					if (textRect.w > itemRect.w || textRect.h > itemRect.h) {  // 释放资源 
						SDL_DestroyTexture(textTexture);  
						SDL_FreeSurface(textSurface); 
						TTF_CloseFont(fontTemp);  
						fontSize--; 
						fontTemp = TTF_OpenFont("SDL2_ttf-2.20.2/Mantinia.otf", fontSize); 
					}
				} while (textRect.w > itemRect.w || textRect.h > itemRect.h);

				textRect.x = (scene_width - 3 * 200 - 2 * 50) / 2 + 100 + (200 + 50) * i - textRect.w / 2;
				textRect.y = (scene_height - 400) / 2 + 200 - textRect.h / 2;

				SDL_RenderCopy(mRenderer, textTexture, NULL, &textRect);

				// 清理创建的纹理和表面
				// English: Clean up created textures and surfaces
				SDL_DestroyTexture(textTexture);
				SDL_FreeSurface(textSurface);
							
			}

			TTF_CloseFont(fontTemp); 
		}
		if (mIsPaused) {
			// 显示Paused 在屏幕中央 
			// English: Display "Paused" in the center of the screen
			SDL_Color color = { 0, 0, 0, 255 };  
			SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Paused", color);
			SDL_Texture* textTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
			SDL_FreeSurface(textSurface);
			SDL_Rect textRect;
			SDL_QueryTexture(textTexture, NULL, NULL, &textRect.w, &textRect.h);
			textRect.x = (scene_width - textRect.w) / 2;
			textRect.y = (scene_height - textRect.h) / 2;
			mAudioManager.PauseMusic();
			// 绘制噶掉文本
			// English: Draw the "Paused" text
			SDL_RenderCopy(mRenderer, textTexture, NULL, &textRect);
		}
		// 交换缓冲区
		// English: Swap the buffer
		SDL_RenderPresent(mRenderer);
	}

	void Game::LoadData()
	{

		LoadTexture("sprites/chrA07.png", "player");

		LoadTexture("sprites/chrA07.png", "BossPawn");
		LoadTexture("sprites/background_image.png", "background");
		// 创建背景 // English: Create background
		if (mBackground) delete mBackground;
		mBackground = new Background(this, mWorld);

		if (mPlayer) delete mPlayer;
		mPlayer = new Player(this, mWorld);

		if (mBoundary) delete mBoundary;
		mBoundary = new Boundary(this, mWorld);

		Mix_Music* bgMusic = mAudioManager.LoadMusic("music/Magic Escape Room.mp3");
		mAudioManager.PlayMusic(bgMusic, -1); // -1 代表音乐无限循环 // English: -1 means infinite loop of music
		mAudioManager.AudioManager::LoadSoundEffects();
	}

	void Game::Tick(int fps)
	{
		// 根据设置的帧率计算每帧至少的时间 // English: Calculate the minimum time per frame based on the set frame rate
		int fpsTime = 1000 / fps;
		// 如果上一帧结束后还没有达到要求时间，则等待 // English: If the required time has not been reached after the last frame is finished, wait
		while (!SDL_TICKS_PASSED(SDL_GetTicks(), Timer::ticksCount + fpsTime))
		{
			SDL_Delay(1);
		}

		// 计算新的增量时间+`	1 // English: Calculate the new incremental time + `1
		Timer::deltaTime = (SDL_GetTicks() - Timer::ticksCount) / 1000.0f;
		// 计算新的一帧开始时所经过的总时间S // English: Calculate the total time S when the new frame starts
		Timer::ticksCount = SDL_GetTicks();
	}

	void Game::UnloadData()
	{
		// 释放所有敌人
		while (!mEnemies.empty())
		{
			delete mEnemies.back();
			mEnemies.pop_back();  // 从容器中移除这个元素 // English: Remove this element from the container
		}

		// 释放所有游戏物体 // English: Release all game objects
		while (!mGameObjects.empty())
		{
			delete mGameObjects.back();
		}
		mGameObjects.clear();
		// 释放指针 // English: Release pointer
		mBackground = nullptr;
		mPlayer = nullptr;
		mBoundary = nullptr;

		// 释放所有贴图资源 // English: Release all texture resources
		for (auto tex : mTextures)
		{
			SDL_DestroyTexture(tex.second);
		}
		mTextures.clear();
	}

	void Game::LoadTexture(const std::string& fileName, const std::string& newName)
	{
		// 判断该key值是否已被使用 // English: Determine if the key value has been used
		auto iter = mTextures.find(newName);
		if (iter != mTextures.end())
		{
			SDL_DestroyTexture(iter->second);
			mTextures.erase(iter);
		}

		// 读取图片存为surface形式 // English: Read the picture and store it in surface form
		SDL_Surface* surf = IMG_Load(fileName.c_str());
		if (!surf)
		{
			SDL_Log("Failed to load texture file %s", fileName.c_str());
		}

		// 将surface转换为texture形式		// English: Convert surface to texture form
		SDL_Texture* tex = SDL_CreateTextureFromSurface(mRenderer, surf);
		// 释放surface的内存 // English: Release the memory of the surface
		SDL_FreeSurface(surf);
		if (!tex)
		{
			SDL_Log("Failed to convert surface to texture for %s", fileName.c_str());
		}

		// 将贴图加入到哈希表中，并将新名字作为key值 // English: Add the texture to the hash table and use the new name as the key value
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
		// 定义x和y坐标的随机范围 // English: Define the random range of x and y coordinates
		std::uniform_real_distribution<float> distX(playerPos.x - 100, playerPos.x + 100);
		std::uniform_real_distribution<float> distY(playerPos.y - 100, playerPos.y + 100);

		// 生成随机x和y坐标 // English: Generate random x and y coordinates
		float randomX = distX(mRngEngine);
		float randomY = distY(mRngEngine);
		 
		// 创建一个随机位置向量 // English: Create a random position vector
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