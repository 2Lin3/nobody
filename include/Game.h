//! @file Game.h

#ifndef __Nobody_Game__
#define __Nobody_Game__

#include	<string>
#include	<vector>
#include	<unordered_map>
#include	<box2d/box2d.h>
#include	<SDL_ttf.h>
#include	<memory>
#include	<Mathg.h>
#include	<random>
#include	<gl/gl.h>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

const int scene_width = 1280;
const int scene_height = 800;	//!<	窗口规格
const float SCALE = 10.f;		//!<	像素与米对应比例
namespace Nobody
{
	class GameObject;
	class SpriteComponent;
	class Player;
	class Boundary;
	class FooDraw;
	class Pawn;
	class Collision;
	class Background;
	class TrickPawn;
	class BossPawn;
	class Game
	{
	public:
		//!	构造函数
		Game();

		//!	初始化
		bool	Initialize();
		//!	主循环
		void	Loop();
		//!	游戏结束
		void	Shutdown();
		Vector2 GenRandomPosition(const Vector2& playerPos);

		//! 创建并存储游戏物体
		void	CreateGameObject(GameObject* gameObject);
		//! 移除游戏对象
		void	RemoveGameObject(GameObject* gameObject);
		//! 创建并按绘制顺序存储精灵
		void	CreateSprite(SpriteComponent* sprite);
		//! 移除精灵
		void	RemoveSprite(SpriteComponent* sprite);
		//! 获取贴图
		SDL_Texture* GetTexture(const std::string& fileName);
		//!	获得窗口宽度与高度
		int GetScreenWidth();
		int GetScreenHeight();
		//!	获得player
		Player* GetPlayer() const;
		//!	获得时间
		float GetDeltaTime() const;
		void RenderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text,
			SDL_Color color, SDL_Rect& textRect);
		void LimitUpBorder(GameObject* gameObject);
	private:
		//!	处理事件
		void	Event();
		//!	更新各种状态
		void	Update();
		//!	渲染
		void	Draw();
		//! 设置帧率
		void	Tick(int fps);
		//! 加载数据
		void	LoadData();
		//!	释放数据
		void	UnloadData();
		//!	加载贴图并重命名
		void	LoadTexture(const std::string& fileName, const std::string& newName);


		std::vector<GameObject*> mGameObjects;		//!<	游戏物体容器
		std::vector<GameObject*> mPendingObjects;	//!<	等待状态的游戏物体容器
		std::vector<SpriteComponent*> mSprites;		//!<	存放精灵的容器
		std::vector<GameObject*> mEnemies;				//!<	存放敌人的容器

		std::mt19937 mRngEngine;					//!<	随机种子
		bool leftMousePressed = false;
		bool rightMousePressed = false;

		std::unordered_map<std::string, SDL_Texture*> mTextures;	//!<	存放贴图的容器

		SDL_Window* mWindow;		//!<	窗口
		SDL_Surface* windowSurface; //!<	窗口表面
		TTF_Font* font;				//!<	字体
		SDL_Color textColor = { 255, 100, 0 };	//!<	字体颜色
		SDL_Rect textRect = { 640, 750, 100, 50 };	//!<	加速字体框体位置

		SDL_Renderer* mRenderer;		//!<	渲染器
		bool			mIsRunning;		//!<	运行状态
		bool			mIsUpdating;	//!<	是否在更新状态

		Player* mPlayer;		//!<	玩家角色
		Boundary* mBoundary;		//!<	边界
		Background* mBackground;	//!<	背景
		//测试敌人
		Pawn* testEnemy = nullptr;
		TrickPawn* trickEnemy = nullptr;
		BossPawn* bossEnemy = nullptr;
		// Box2D世界
		b2World* mWorld;
		// 指定每秒模拟的步数
		const float timeStep = 1.0f / 60.0f;
		// 迭代次数
		const int velocityIterations = 6;
		const int positionIterations = 2;

		float mSpawnTimer = 0;//计时器

		int score = 0;  // 分数
		SDL_Rect scoreTextRect = { 1000, 20, 100, 50 };
		SDL_Color scoreColor = { 255, 255, 255 };
	};
}

#endif