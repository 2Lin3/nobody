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
		// SDL踱場宎趙
		if (SDL_Init(SDL_INIT_VIDEO) != 0)
		{	
			SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
			return false;
		}

		// 敦諳場宎趙
		mWindow = SDL_CreateWindow("Nobody", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scene_width, scene_height, 0);
		if (!mWindow)
		{
			SDL_Log("Failed to create window: %s", SDL_GetError());
			return false;
		}

		// 馺場宎趙
		mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
		if (!mRenderer)
		{
			SDL_Log("Failed to create renderer: %s", SDL_GetError());
			return false;
		}

		// 場宎趙SDL_image踱
		if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
		{
			SDL_Log("Failed to initialize SDL_image: %s", SDL_GetError());
			return false;
		}

		//場宎趙趼踱
		if (TTF_Init() == -1) {
			printf("TTF_Init: % s\n", TTF_GetError()); 
			return false;
		}

		// 膘蕾box2d岍賜
		mWorld = new b2World(b2Vec2(0.0f, 0.0f));
		if (!mWorld) {
			SDL_Log("Failed to create mWorld");
			return false;
		}
		
		// 場宎趙耋撿蹈桶
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

		//// 斐膘OpenGL
		//SDL_GLContext context = SDL_GL_CreateContext(mWindow);
		//if (!context) {
		//	SDL_Log("Failed to create OpenGL");
		//	return false;
		//}

		// 扢离扷梓腔眈勤耀宒
		// 婓眈勤耀宒狟ㄛ扷梓腔弇离岆眈勤衾奻珨棒腔弇离腔痄講
		// 涴砩庤覂扷梓祥頗堤珋婓躉奻ㄛ珩拸楊痄雄堤敦諳
		SDL_SetRelativeMouseMode(SDL_TRUE);
		// 樓婥OTF趼极恅璃
		font = TTF_OpenFont("SDL2_ttf-2.20.2/Mantinia.otf", 50);
		// 鳳敦諳桶醱
		SDL_Surface* windowSurface = SDL_GetWindowSurface(mWindow);
		// 扢离癲袉潼泭
		Collision* contactListener = new Collision();
		mWorld->SetContactListener(contactListener);
		
		//呴儂笱赽場宎趙
		std::random_device rd;
		mRngEngine = std::mt19937(rd());

		// 樓婥杅擂
		LoadData();
		// 羲宎脹渾俙模偌狟砩瑩
		StartWaiting();
		return true;
	}

	void Game::StartWaiting() {
		// 馺羲宎恅掛
		SDL_Color textColor = { 255, 255, 255, 255 };
		SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Press any key to start", textColor);
		SDL_Texture* textTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
		SDL_FreeSurface(textSurface);

		SDL_Rect textRect;
		SDL_QueryTexture(textTexture, NULL, NULL, &textRect.w, &textRect.h);
		textRect.x = (scene_width - textRect.w) / 2;
		textRect.y = (scene_height - textRect.h) / 2;

		// 餅秶羲宎恅掛
		SDL_RenderClear(mRenderer);
		SDL_RenderCopy(mRenderer, textTexture, NULL, &textRect);
		SDL_RenderPresent(mRenderer);

		// 脹渾蚚誧偌狟砩瑩
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
					SDL_DestroyTexture(textTexture);//燴斐膘腔恇燴
					mSpawnTimer = 0;//數奀寥錨
					score = 0;  // 煦杅寥錨
					break;
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(3));
		}
	};

	void Game::RenderDeathMessage()
	{
		SDL_Color color = { 0, 0, 0, 255 };  // 晇伎
		SDL_Surface* textSurface = TTF_RenderText_Solid(font, "snuff out", color);
		SDL_Texture* textTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
		SDL_FreeSurface(textSurface);
		SDL_Rect textRect;
		SDL_QueryTexture(textTexture, NULL, NULL, &textRect.w, &textRect.h);
		textRect.x = (scene_width - textRect.w) / 2;
		textRect.y = (scene_height - textRect.h) / 2;

		// 餅秶蜂裁恅掛
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
		// 彆絞淏婓載陔ㄛ蔚蜆勤砓樓脹渾
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
		// 珂婓脹渾笢扆梑甜痄壺昜极
		auto iter = std::find(mPendingObjects.begin(), mPendingObjects.end(), gameObject);
		if (iter != mPendingObjects.end())
		{
			// 蝠遙綴刉壺藺帣ㄛ虴薹詢衾eraseㄛ筍頗輓佼唗
			std::iter_swap(iter, mPendingObjects.end() - 1);
			mPendingObjects.pop_back();
		}

		// 婓淏宒昜极笢扆梑甜痄壺昜极
		iter = std::find(mGameObjects.begin(), mGameObjects.end(), gameObject);
		if (iter != mGameObjects.end())
		{
			// 蝠遙綴刉壺藺帣ㄛ虴薹詢衾eraseㄛ筍頗輓佼唗
			std::iter_swap(iter, mGameObjects.end() - 1);
			mGameObjects.pop_back();
		}
	}

	void Game::CreateSprite(SpriteComponent* sprite)
	{
		int order = sprite->GetDrawOrder();
		// 偌桽餅秶佼唗脣
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
		// 彆岈璃勦蹈笢衄帤揭燴腔岈璃ㄛ偌佼唗揭燴
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				// 豖堤岈璃ㄛ偌狟敦諳腔脫
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
					// 載陔藩跺縐腔唑礿袨怓
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

		// 鳳瑩攫怀袨怓
		const uint8_t* keyState = SDL_GetKeyboardState(NULL);

		// 揭燴垀衄昜极腔怀軀憮
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
		// 彆蚔牁婃礿ㄛ眻諉殿隙
		if (mIsPaused) return;

		// 扢离痋薹
		Tick(60);
		if (mIsChoosingProps) {
			// 秏扷梓腔眈勤耀宒
			SDL_SetRelativeMouseMode(SDL_FALSE);
			// 珆尨扷梓
			SDL_ShowCursor(SDL_ENABLE);
			std::shuffle(items.begin(), items.end(), mRngEngine);
			selectedItems.assign(items.begin(), items.begin() + 3);
			mIsDrawingChoosingProps = true;
			mIsChoosingProps = false;
			for (int i = 0; i < 3; ++i) {
				mIsHovered[i] = false;
			}
			////笐紲扷梓
			//SDL_ShowCursor(SDL_DISABLE);
			//// 扢离扷梓峈眈勤耀宒
			//SDL_SetRelativeMouseMode(SDL_TRUE);
		}
		if (mIsDrawingChoosingProps) return;

		//絞奀潔
		Uint32 currentTime = SDL_GetTicks();
		string currentTimeString = to_string(currentTime/1000);
		//cout << currentTimeString << endl;

		// 載陔羲宎
		mIsUpdating = true;
		// 梢盪甜硒俴垀衄昜极腔載陔滲杅
		for (auto gameObject : mGameObjects)
		{
			gameObject->Update();
			LimitUpBorder(gameObject);//upborder air wall
		}
		mSpawnTimer += Timer::deltaTime;
		if (mSpawnTimer >= 3.0f) {
			mSpawnTimer = 0.0f; // 笭离數奀
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

		if (mPlayer->GetLife() <= 0)
		{	
			mIsDEAD = true;
		}

		// 載陔Box2D岍賜
		mWorld->Step(timeStep, velocityIterations, positionIterations);

		// 載陔賦旰
		mIsUpdating = false;


		// 蔚垀衄脹渾昜极痄雄祫淏宒腔昜极
		for (auto pendingObject : mPendingObjects)
		{
			mGameObjects.emplace_back(pendingObject);
		}
		// 諾脹渾
		mPendingObjects.clear();
		if (leftMousePressed) mPlayer->ProcessInputMouseDown(SDL_BUTTON_LEFT, mPlayer->GetBoostValue());
		if (rightMousePressed) mPlayer->ProcessInputMouseDown(SDL_BUTTON_RIGHT, mPlayer->GetBoostValue());
		else mPlayer->ProcessInputMouseDown(SDL_BUTTON_MIDDLE, mPlayer->GetBoostValue());

		// 蔚垀衄袨怓峈EDead腔昜极氝樓祫侚厗
		std::vector<GameObject*> deadObjects;
		for (auto deadObject : mGameObjects)
		{
			if (deadObject->GetState() == GameObject::State::EDead)
			{
				deadObjects.emplace_back(deadObject);
			}
		}
		// 刉壺垀衄袨怓峈EDead腔菩
		mEnemies.erase(
			std::remove_if(
				mEnemies.begin(),
				mEnemies.end(),
				[this](GameObject* enemy) { 
					if (enemy->GetState() == GameObject::State::EDead) {
						score++;  // 崝樓俙模腔煦杅
						mPlayer->SetExp(*mPlayer->GetExp() + 1);  // 崝樓俙模腔冪桄硉
						return true;  // 刉壺涴跺菩
					}
		return false;  // 祥刉壺涴跺菩
				}
			),
			mEnemies.end()
					);
		// 庋溫裁垀衄侚厗腔昜极
		for (auto deadObject : deadObjects)
		{
			if (deadObject->isPlayer) continue;
			delete deadObject;
		}
	}

	void Game::Draw()
	{
		// 扢离馺晇伎ㄗ懦ㄘ
		SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);

		// 壺遣喳
		SDL_RenderClear(mRenderer);
		// 餅秶覃彸芞倛
		mWorld->DebugDraw();
		// 閥葩髦磁耀宒
		SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_NONE);
		// 餅秶垀衄儕鍾
		for (auto sprite : mSprites)
		{
			sprite->Draw(mRenderer);
		}
		SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 0);
		SDL_RenderDrawLine(mRenderer,0,750,1280,750);
		SDL_RenderDrawLine(mRenderer, 50, 0, 50, 800);
		SDL_RenderDrawLine(mRenderer, 1230, 0, 1230, 800);
		SDL_RenderDrawCircle(mRenderer, mPlayer->GetPosition().x, mPlayer->GetPosition().y,20);
		//餅秶樓厒羞
		int boostValue = static_cast<int>(scene_width / 1.5 * (static_cast<float>(*mPlayer->GetBoostValue()) / static_cast<float>(100)));
		boostValue = std::max(0, boostValue);
		SDL_Rect boostBar = { 225, scene_height-50, boostValue, 50 };
		SDL_RenderFillRect(mRenderer, &boostBar);

		//餅秶汜韜羞
		SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 0);
		SDL_Rect lifeBar = { 0, scene_height- mPlayer->GetLife() * 3 - 50,50, mPlayer->GetLife()*3};
		SDL_RenderFillRect(mRenderer, &lifeBar);

		RenderText(mRenderer, font, "ACCELERATE", textColor, textRect);

		// 餅秶懦伎Exp沭
		SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 0);
		SDL_Rect expBarFrame = { scene_width - 50, scene_height - 300 - 50,50, 300 };
		SDL_RenderDrawRect(mRenderer, &expBarFrame);
		int expValue = static_cast<int>(300 * (static_cast<float>(*mPlayer->GetExp()) / static_cast<float>(*mPlayer->GetNeededExp())));
		expValue = std::max(0, expValue);
		SDL_Rect expBar = { scene_width - 50, scene_height - expValue - 50,50, expValue };
		SDL_RenderFillRect(mRenderer, &expBar);

		SDL_Rect expTextRect = { scene_width - 50, scene_height - 300 - 50 - 50,50, 50 };
		RenderText(mRenderer, font, "Exp", textColor, expTextRect);
		
		// 蔚煦杅蛌遙峈趼睫揹
		std::string scoreText ="Score  " + std::to_string(score);

		// 妏蚚 RenderText 滲杅婓硌隅腔弇离珆尨煦杅
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


		if (mIsDEAD) {

			// 珆尨侚厗陓洘
			RenderDeathMessage();
			// 餅秶珨跺芵隴僅紨膝崝樓腔窪伎撻倛懂妗珋膝曹虴彆

			SDL_Rect rect = { 0, 0, scene_width, scene_height };
			SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, mFadeAlpha);  // 樑扢mFadeAlpha岆絞腔芵隴僅
			SDL_RenderFillRect(mRenderer, &rect);

			// 芵隴僅紨膝崝樓
			mFadeAlpha += 255 / (3.5f * 60);  // 樑扢藩鏃60痋

			// 膝曹賦旰ㄛ笭蚔牁
			if (mFadeAlpha >= 255)
			{
				mFadeAlpha = 0;
				// 笭蚔牁
				ResetGame();
				mIsDEAD = false;
			}
		}
		if (mIsDrawingChoosingProps)
		{


			int fontSize = 48;  // 扢隅斕腔場宎趼极湮苤
			TTF_Font* fontTemp = TTF_OpenFont("SDL2_ttf-2.20.2/Mantinia.otf", fontSize);

			for (int i = 0; i < 3; ++i) {
				// 婓躉笢栝餅秶items腔縐峈閡伎腔掖劓
				SDL_Rect itemRect = { (scene_width - 3 * 200 - 2 * 50) / 2 + (200 + 50) * i, (scene_height - 400) / 2, 200, 400 };
				itemRects[i] = itemRect;
				if (mIsHovered[i]) {
					SDL_SetRenderDrawColor(mRenderer, blackColor.r, blackColor.g, blackColor.b, blackColor.a);
				}
				else {
					SDL_SetRenderDrawColor(mRenderer, grayColor.r, grayColor.g, grayColor.b, grayColor.a);
				}
				SDL_RenderFillRect(mRenderer, &itemRect);

				// 蔚selectedItems笢腔耋撿啞伎靡趼珆尨婓縐奻
				SDL_Surface* textSurface = NULL;
				SDL_Texture* textTexture = NULL;
				SDL_Rect textRect;
				do {
					textSurface = TTF_RenderText_Solid(fontTemp, selectedItems[i].getName().c_str(), whiteColor);
					textTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
					SDL_QueryTexture(textTexture, NULL, NULL, &textRect.w, &textRect.h);
					if (textRect.w > itemRect.w || textRect.h > itemRect.h) {  // 彆恅趼喜渡湮衾縐
						SDL_DestroyTexture(textTexture);  // 种障眳腔texture
						SDL_FreeSurface(textSurface);  // 庋溫眳腔surface
						TTF_CloseFont(fontTemp);  // 壽敕眳腔趼极
						fontSize--;  // 趼极湮苤熬苤
						fontTemp = TTF_OpenFont("SDL2_ttf-2.20.2/Mantinia.otf", fontSize);  // 笭陔湖羲趼极
					}
				} while (textRect.w > itemRect.w || textRect.h > itemRect.h);

				textRect.x = (scene_width - 3 * 200 - 2 * 50) / 2 + 100 + (200 + 50) * i - textRect.w / 2;
				textRect.y = (scene_height - 400) / 2 + 200 - textRect.h / 2;

				// 餅秶耋撿靡趼
				SDL_RenderCopy(mRenderer, textTexture, NULL, &textRect);

				// 庋溫訧埭
				SDL_DestroyTexture(textTexture);
				SDL_FreeSurface(textSurface);
							
			}

			TTF_CloseFont(fontTemp); // 祥猁咭暮壽敕斕腔趼极
		}
		if (mIsPaused) {
			// 珆尨Paused 婓躉笢栝
			SDL_Color color = { 0, 0, 0, 255 };  // 晇伎
			SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Paused", color);
			SDL_Texture* textTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
			SDL_FreeSurface(textSurface);
			SDL_Rect textRect;
			SDL_QueryTexture(textTexture, NULL, NULL, &textRect.w, &textRect.h);
			textRect.x = (scene_width - textRect.w) / 2;
			textRect.y = (scene_height - textRect.h) / 2;

			// 餅秶蜂裁恅掛
			SDL_RenderCopy(mRenderer, textTexture, NULL, &textRect);
		}
		// 蝠遙遣喳
		SDL_RenderPresent(mRenderer);
	}

	void Game::LoadData()
	{

		LoadTexture("sprites/chrA07.png", "player");

		LoadTexture("sprites/chrA07.png", "BossPawn");
		LoadTexture("sprites/background_image.png", "background");
		// 斐膘掖劓
		if (mBackground) delete mBackground;
		mBackground = new Background(this, mWorld);

		if (mPlayer) delete mPlayer;
		mPlayer = new Player(this, mWorld);

		if (mBoundary) delete mBoundary;
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
		// 跦擂扢离腔痋薹數呾藩痋祫屾腔奀潔
		int fpsTime = 1000 / fps;
		// 彆奻珨痋賦旰綴遜羶衄湛善猁奀潔ㄛ寀脹渾
		while (!SDL_TICKS_PASSED(SDL_GetTicks(), Timer::ticksCount + fpsTime))
		{
			SDL_Delay(1);
		}

		// 數呾陔腔崝講奀潔+`	1
		Timer::deltaTime = (SDL_GetTicks() - Timer::ticksCount) / 1000.0f;
		// 數呾陔腔珨痋羲宎奀垀冪徹腔軞奀潔S
		Timer::ticksCount = SDL_GetTicks();
	}

	void Game::UnloadData()
	{
		// 庋溫垀衄菩
		while (!mEnemies.empty())
		{
			delete mEnemies.back();
			mEnemies.pop_back();  // 植笢痄壺涴跺啋匼
		}

		// 庋溫垀衄蚔牁昜极
		while (!mGameObjects.empty())
		{
			delete mGameObjects.back();
		}
		mGameObjects.clear();
		// 庋溫硌渀
		mBackground = nullptr;
		mPlayer = nullptr;
		mBoundary = nullptr;

		// 庋溫垀衄泂芞訧埭
		for (auto tex : mTextures)
		{
			SDL_DestroyTexture(tex.second);
		}
		mTextures.clear();
	}

	void Game::LoadTexture(const std::string& fileName, const std::string& newName)
	{
		// 瓚剿蜆key硉岆瘁眒掩妏蚚
		auto iter = mTextures.find(newName);
		if (iter != mTextures.end())
		{
			SDL_DestroyTexture(iter->second);
			mTextures.erase(iter);
		}

		// 黍芞湔峈surface倛宒
		SDL_Surface* surf = IMG_Load(fileName.c_str());
		if (!surf)
		{
			SDL_Log("Failed to load texture file %s", fileName.c_str());
		}

		// 蔚surface蛌遙峈texture倛宒
		SDL_Texture* tex = SDL_CreateTextureFromSurface(mRenderer, surf);
		// 庋溫surface腔囀湔
		SDL_FreeSurface(surf);
		if (!tex)
		{
			SDL_Log("Failed to convert surface to texture for %s", fileName.c_str());
		}

		// 蔚泂芞樓善慇洷桶笢ㄛ甜蔚陔靡趼釬峈key硉
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
		// 隅砱x睿y釴梓腔呴儂毓峓
		std::uniform_real_distribution<float> distX(playerPos.x - 100, playerPos.x + 100);
		std::uniform_real_distribution<float> distY(playerPos.y - 100, playerPos.y + 100);

		// 汜傖呴儂x睿y釴梓
		float randomX = distX(mRngEngine);
		float randomY = distY(mRngEngine);

		// 斐膘珨跺呴儂弇离砃講
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