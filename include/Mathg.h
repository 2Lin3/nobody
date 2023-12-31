
//! @file Math.h

#ifndef __Nobody_Math__
#define __Nobody_Math__

namespace Nobody
{
	class Vector2
	{
	public:
		float x;
		float y;

		Vector2();
		Vector2(float X, float Y);
		static const Vector2 Zero;		//!<	(0 , 0)
		static const Vector2 One;		//!<	(1 , 1)
		static const Vector2 X;			//!<	(1 , 0)
		static const Vector2 Y;			//!<	(0 , 1)
		static const Vector2 NX;		//!<	(-1, 0)
		static const Vector2 NY;		//!<	(0 ,-1)

		//! 向量加法
		friend Vector2 operator+(const Vector2& a, const Vector2& b);
		//! 向量减法
		friend Vector2 operator-(const Vector2& a, const Vector2& b);
		//! 向量乘法（均为向量，对应元素相乘）
		friend Vector2 operator*(const Vector2& a, const Vector2& b);
		//! 向量乘法（向量与标量相乘）
		friend Vector2 operator*(const Vector2& vec, float scalar);
		//! 向量乘法（标量与向量相乘）
		friend Vector2 operator*(float scalar, const Vector2& vec);
		//! 向量相等
		friend bool operator==(const Vector2& a, const Vector2& b);
		// 添加到Vector2类
		float Distance(const Vector2& other) const;

		Vector2 Normalized() const;
		static float Dot(const Vector2& a, const Vector2& b) {
			return a.x * b.x + a.y * b.y;
		}
	};
}

#endif	// __Nobody_Math__
