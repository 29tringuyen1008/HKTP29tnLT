#pragma once
#include <math.h>
#include <vector>

#include <d3d9.h>
#include <DirectXMath.h>

namespace tn {
	bool IsBetween(float value, float bound1, float bound2);

	float ConvertDegreeToRadian(float Degree);
	float ConvertRadianToDegree(float Radian);

	struct Vector2 {
		float X, Y;

		Vector2 operator+(Vector2 v1);
		Vector2 operator-(Vector2 v1);
		Vector2 operator*(float scalar);
		Vector2 operator/(float scalar);
		Vector2 operator*(Vector2 v1);
		Vector2 operator/(Vector2 v1);
		Vector2 operator+=(Vector2 v1);
		Vector2 operator-=(Vector2 v1);
		Vector2 operator*=(float scalar);
		Vector2 operator/=(float scalar);
		Vector2 operator*=(Vector2 v1);
		Vector2 operator/=(Vector2 v1);
		bool operator==(Vector2 v1);
		bool operator!=(Vector2 v1);
		bool operator>(Vector2 v1);
		bool operator<(Vector2 v1);
		bool operator>=(Vector2 v1);
		bool operator<=(Vector2 v1);

		float Magnitude();
	};

	struct UDim2 {
		Vector2 Scale;
		Vector2 Offset;
	};

	struct Vector3 {
		float X, Y, Z;

		Vector3 operator+(const Vector3& v1);
		Vector3 operator-(const Vector3& v1);
		Vector3 operator*(float v1);
		Vector3 operator/(float v1);
		Vector3 operator+=(const Vector3& v1);
		Vector3 operator-=(const Vector3& v1);
		Vector3 operator*=(float v1);
		Vector3 operator/=(float v1);
		bool operator==(Vector3 v1);
		bool operator!=(Vector3 v1);
		bool operator>(Vector3 v1);
		bool operator<(Vector3 v1);
		bool operator>=(Vector3 v1);
		bool operator<=(Vector3 v1);

		float Magnitude();
	};

	struct Color3 {
		float Red, Green, Blue;

		Color3 operator+(Color3 c1);
		Color3 operator-(Color3 c1);
		Color3 operator*(float Scalar);
		Color3 operator/(float Scalar);
		bool operator==(Color3 c1);
		bool operator!=(Color3 c1);
		bool operator>(Color3 c1);
		bool operator<(Color3 c1);
		bool operator>=(Color3 c1);
		bool operator<=(Color3 c1);
	};

	struct Rect
	{
		float Left = 0, Right = 0, Top = 0, Bottom = 0;
	};

	struct Vertex {
		Vector3 Position = { 0.0f, 0.0f, 0.0f };
		Color3 BackgroundColor = { 0,0,0 };
		float Alpha = 1.0f;
		Vector2 TextureCoordinates[2];
	};

	bool IsPointWithinRect(Vector2 point, Vector2 rectpos, Vector2 rectsize);
	bool IsPointWithinRect(Vector2 point, Rect rect);
	void TransformVector2(Vector2* vector, Vector2 transform);
	void RotateVector2(Vector2* vector, float RadianAngle);
	void ScaleVector2(Vector2* vector, float scalar);
	Vector2 GetPerpendicularClockwise(Vector2 vector);
	Vector2 GetPerpendicularCounterclockwise(Vector2 vector);

	struct Matrix4x4;

	void RotateVector3X(Vector3* vector, float RadianAngle);
	void RotateVector3Y(Vector3* vector, float RadianAngle);
	void RotateVector3Z(Vector3* vector, float RadianAngle);
	void TransformVector3(Vector3* vector, Vector3 transform);
	void ScaleVector3(Vector3* vector, float scalar);
	void TransformVector3(Vector3* vector, Matrix4x4 Matrix, float* ScalingFactorW = nullptr);
	void TransformVertex(Vertex* vertex, Matrix4x4 Matrix, float* ScalingFactorW = nullptr);

	void Convert2DVertexIntoRect(const Vertex Input[4], Rect* Output); //Danh cho hinh chu nhat
	void ConvertRectTo4Vector2(const Rect* Input, Vector2** Output); //Danh cho hinh chu nhat
	void TransformRect(Rect* InputOutput, Vector2 Vector);
	void TransformRect(Rect* InputOutput, Vector3 Vector);
	bool AABB_Collision(const Vertex s1[4], const Vertex s2[4]);
	bool AABB_Collision(const Rect& s1, const Rect& s2);

	void ProjectPolygon(const Vertex* s1, size_t AmountOfVertices, const Vector3& axis, float& min, float& max);
	bool SAT2D_Collision(const Vertex* s1, size_t AmountOfVertices1, const Vertex* s2, size_t AmountOfVertices2);

	struct Matrix2x2 {
		float a1, a2;
		float b1, b2;

		Matrix2x2();
		Matrix2x2(
			float a1, float a2,
			float b1, float b2
		);

		Matrix2x2 operator+(Matrix2x2 m1);
		Matrix2x2 operator-(Matrix2x2 m1);
		Matrix2x2 operator*(Matrix2x2 m1);

		Matrix2x2 operator*(float number);
		Matrix2x2 operator/(float number);
	};

	struct Matrix2x3;
	struct Matrix3x2 {
		float a1, a2, a3;
		float b1, b2, b3;

		Matrix3x2();
		Matrix3x2(
			float a1, float a2, float a3,
			float b1, float b2, float b3
		);

		Matrix3x2 operator+(Matrix3x2 m1);
		Matrix3x2 operator-(Matrix3x2 m1);
		Matrix2x2 operator*(Matrix2x3 m1);

		Matrix3x2 operator*(float number);
		Matrix3x2 operator/(float number);
	};

	struct Matrix2x3 {
		float a1, a2;
		float b1, b2;
		float c1, c2;

		Matrix2x3();
		Matrix2x3(
			float a1, float b1, 
			float a2, float b2, 
			float a3, float b3
		);

		Matrix2x3 operator+(Matrix2x3 m1);
		Matrix2x3 operator-(Matrix2x3 m1);

		Matrix2x3 operator*(float number);
		Matrix2x3 operator/(float number);
	};

	struct Matrix4x4 {
		float a1 = 0.0f, a2 = 0.0f, a3 = 0.0f, a4 = 0.0f;
		float b1 = 0.0f, b2 = 0.0f, b3 = 0.0f, b4 = 0.0f;
		float c1 = 0.0f, c2 = 0.0f, c3 = 0.0f, c4 = 0.0f;
		float d1 = 0.0f, d2 = 0.0f, d3 = 0.0f, d4 = 0.0f;

		Matrix4x4();
		Matrix4x4(const Matrix4x4& c_m4x4);
		Matrix4x4(const Matrix4x4* const cptr_m4x4_c);
		Matrix4x4(float a1, float a2, float a3, float a4,
			float b1, float b2, float b3, float b4,
			float c1, float c2, float c3, float c4,
			float d1, float d2, float d3, float d4);
		Matrix4x4(float FLOAT4X4[16]);
		Matrix4x4(DirectX::XMMATRIX);
		Matrix4x4(D3DMATRIX);
		~Matrix4x4();

		operator DirectX::XMMATRIX();
		operator D3DMATRIX();

		operator DirectX::XMMATRIX* (); //Phải dùng delete sau khi dùng xong
		operator D3DMATRIX* (); //Phải dùng delete sau khi dùng xong

		operator float* ();

		Matrix4x4 operator+(Matrix4x4 m1);
		Matrix4x4 operator-(Matrix4x4 m1);
		Matrix4x4 operator*(Matrix4x4 m1);

		Matrix4x4 operator*(float number);
		Matrix4x4 operator/(float number);

		Matrix4x4 operator+=(Matrix4x4 m1);
		Matrix4x4 operator-=(Matrix4x4 m1);
		Matrix4x4 operator*=(Matrix4x4 m1);

		Matrix4x4 operator*=(float number);
		Matrix4x4 operator/=(float number);
	};

	Matrix2x2 MatrixTranspose(Matrix2x2 Matrix);
	Matrix3x2 MatrixTranspose(Matrix2x3 Matrix);
	Matrix2x3 MatrixTranspose(Matrix3x2 Matrix);
	Matrix4x4 MatrixTranspose(Matrix4x4 Matrix);

	Matrix4x4 MatrixTranslation(Vector3 Vector3D);
	Matrix4x4 MatrixScaling(Vector3 Vector3D);
	Matrix4x4 MatrixOrthographicProjection(Vector2 LeftTop, Vector2 RightBottom, float Near, float Far);
	Matrix4x4 MatrixPerspectiveProjectionLH(float FOV, float AspectRatio, float Near, float Far);
	Matrix4x4 MatrixPerspectiveProjectionRH(float FOV, float AspectRatio, float Near, float Far);
	Matrix4x4 MatrixLookAtLH(Vector3 EyePosition, Vector3 TargetPosition, Vector3 Up);
	Matrix4x4 MatrixLookAtRH(Vector3 EyePosition, Vector3 TargetPosition, Vector3 Up);
	Matrix4x4 MatrixRotationX(float Angle);
	Matrix4x4 MatrixRotationY(float Angle);
	Matrix4x4 MatrixRotationZ(float Angle);

	void ConvertFromXMMATRIXtoD3DMATRIX(DirectX::XMMATRIX* input, D3DMATRIX* output);
	void ConvertFromD3DMATRIXtoXMMATRIX(D3DMATRIX* input, DirectX::XMMATRIX* output);

	struct Linear2Alt;
	//y = a*x + b
	struct Linear2
	{
		float Slope = 0.0f;
		float Y_Intercept = 0.0f;
		Linear2 operator=(const Linear2Alt& linearalt);
	};

	//ax + by + c = 0
	struct Linear2Alt
	{
		float a, b, c;
		Linear2Alt operator=(const Linear2& linear2);
	};

	//y = a*x^2 + b*x + c
	struct Parabolic2
	{
		float a;
		float b;
		float c;
	};

	constexpr int GetIntersection_InvalidParameters = -1;

	constexpr int GetIntersection_BothLinesHaveAnIntersection = 0;
	constexpr int GetIntersection_BothLineAndRectHaveLineSegmentIntersection = 0;

	constexpr int GetIntersection_BothLinesParallel = 1;
	constexpr int GetIntersection_BothLinesAreExactLines = 2;
	constexpr int GetIntersection_NoIntersections = 3;
	constexpr int GetIntersection_BetweenLineAndRect_OnlyHavePointIntersection = 4;

	float InputLinearY(Linear2 Line, float X);
	float InputLinear(Linear2Alt Line, float X, float Y);
	float InputLinearY(Linear2Alt Line, float X);
	float InputParabolicY(Parabolic2 Parabol, float X);

	struct LineSegment
	{
		Vector2 Point1;
		Vector2 Point2;
		float GetLength();
		Linear2Alt GetLinear();
	};

	int GetIntersection(Linear2* Line1, Linear2* Line2, Vector2* Output);
	int GetIntersection(Linear2Alt* Line1, Linear2Alt* Line2, Vector2* Output);
	int GetIntersection(Linear2Alt* Line, LineSegment* LineSegment, Vector2* Output);
	int GetIntersection(Linear2Alt* Line, Rect* Rect, LineSegment* Output);

	struct Ray2D
	{
		Vector2 Origin;
		Vector2 Direction;
	};

	int ConvertRayToLinear2(Ray2D ray, Linear2* Output);
	void ConvertRayToLinear2(Ray2D ray, Linear2Alt* Output);

	bool AABB_Collision(Ray2D* Ray, Rect* Rect);
}

tn::Vector3 operator+(const tn::Vector3& v1, const tn::Vector3& v2);
tn::Vector3 operator-(const tn::Vector3& v1, const tn::Vector3& v2);