#include "Physics.h"
#include "Misc.h"
#include <iostream>

bool tn::IsBetween(float value, float bound1, float bound2) {
	return (value >= std::fmin(bound1, bound2) && value <= std::fmax(bound1, bound2));
}

#pragma region VECTOR2
tn::Vector2 tn::Vector2::operator+(tn::Vector2 v1) {
	return { this->X + v1.X, this->Y + v1.Y };
}

tn::Vector2 tn::Vector2::operator-(tn::Vector2 v1) {
	return { this->X - v1.X, this->Y - v1.Y };
}

tn::Vector2 tn::Vector2::operator*(tn::Vector2 v1) {
	return { this->X * v1.X, this->Y * v1.Y };
}

tn::Vector2 tn::Vector2::operator/(tn::Vector2 v1) {
	return { this->X / v1.X, this->Y / v1.Y };
}

tn::Vector2 tn::Vector2::operator*(float scalar)
{
	return { this->X * scalar, this->Y * scalar };
}

tn::Vector2 tn::Vector2::operator/(float scalar)
{
	return { this->X / scalar, this->Y / scalar };
}

tn::Vector2 tn::Vector2::operator+=(tn::Vector2 v1) {
	this->X += v1.X;
	this->Y += v1.Y;
	return { this->X, this->Y };
}

tn::Vector2 tn::Vector2::operator-=(tn::Vector2 v1) {
	this->X -= v1.X;
	this->Y -= v1.Y;
	return { this->X, this->Y };
}

tn::Vector2 tn::Vector2::operator*=(float scalar)
{
	this->X *= scalar;
	this->Y *= scalar;
	return { this->X, this->Y };
}

tn::Vector2 tn::Vector2::operator/=(float scalar)
{
	this->X /= scalar;
	this->Y /= scalar;
	return { this->X, this->Y };
}


tn::Vector2 tn::Vector2::operator*=(tn::Vector2 v1) {
	this->X *= v1.X;
	this->Y *= v1.Y;
	return { this->X, this->Y };
}

tn::Vector2 tn::Vector2::operator/=(tn::Vector2 v1) {
	this->X /= v1.X;
	this->Y /= v1.Y;
	return { this->X, this->Y };
}

bool tn::Vector2::operator==(Vector2 v1) {
	return (this->X == v1.X and this->Y == v1.Y);
}

bool tn::Vector2::operator!=(Vector2 v1) {
	return (this->X != v1.X or this->Y != v1.Y);
}

bool tn::Vector2::operator>(Vector2 v1) {
	return (this->X > v1.X and this->Y > v1.Y);
}

bool tn::Vector2::operator<(Vector2 v1) {
	return (this->X < v1.X and this->Y < v1.Y);
}

bool tn::Vector2::operator>=(Vector2 v1) {
	return (this->X >= v1.X and this->Y >= v1.Y);
}

bool tn::Vector2::operator<=(Vector2 v1) {
	return (this->X <= v1.X and this->Y <= v1.Y);
}

float tn::Vector2::Magnitude() {
	return sqrtf(powf(this->X, 2.0f) + powf(this->Y, 2.0f));
}

bool tn::IsPointWithinRect(Vector2 point, Vector2 rectpos, Vector2 rectsize)
{
	return point.X >= rectpos.X and point.Y >= rectpos.Y and point.X <= (rectpos.X + rectsize.X) and point.Y <= (rectpos.Y + rectsize.Y);
}

bool tn::IsPointWithinRect(Vector2 point, Rect rect)
{
	return (point.X >= rect.Left and point.Y >= rect.Top and point.X <= rect.Right and point.Y <= rect.Bottom);
}

void tn::TransformVector2(Vector2* vector, Vector2 transform)
{
	(*vector).X += transform.X;
	(*vector).Y += transform.Y;
}

void tn::RotateVector2(Vector2* vector, float RadianAngle)
{
	(*vector).X = (*vector).X * cos(RadianAngle) - (*vector).Y * sin(RadianAngle);
	(*vector).Y = (*vector).X * sin(RadianAngle) + (*vector).Y * cos(RadianAngle);
}

void tn::ScaleVector2(Vector2* vector, float scalar)
{
	(*vector).X *= scalar;
	(*vector).Y *= scalar;
}

tn::Vector2 tn::GetPerpendicularClockwise(tn::Vector2 vector)
{
	return { vector.Y, -vector.X };
}

tn::Vector2 tn::GetPerpendicularCounterclockwise(tn::Vector2 vector)
{
	return { -vector.Y, vector.X };
}

void tn::RotateVector3X(Vector3* vector, float RadianAngle)
{
	//(*vector).X = (*vector).X;
	(*vector).Y = (*vector).Y * cos(RadianAngle) - (*vector).Z * sin(RadianAngle);
	(*vector).Z = (*vector).Y * sin(RadianAngle) + (*vector).Z * cos(RadianAngle);
}

void tn::RotateVector3Y(Vector3* vector, float RadianAngle)
{
	(*vector).X = (*vector).X * cos(RadianAngle) + (*vector).Z * sin(RadianAngle);
	//(*vector).Y = (*vector).Y;
	(*vector).Z = -(*vector).X * sin(RadianAngle) + (*vector).Z * cos(RadianAngle);
}

void tn::RotateVector3Z(Vector3* vector, float RadianAngle)
{
	(*vector).X = (*vector).X * cos(RadianAngle) + (*vector).Y * sin(RadianAngle);
	(*vector).Y = (*vector).X * sin(RadianAngle) - (*vector).Y * cos(RadianAngle);
	//(*vector).Z = (*vector).Z;
}

void tn::TransformVector3(Vector3* vector, Vector3 transform)
{
	(*vector).X += transform.X;
	(*vector).Y += transform.Y;
	(*vector).Z += transform.Z;
}

void tn::ScaleVector3(Vector3* vector, float scalar)
{
	(*vector).X *= scalar;
	(*vector).Y *= scalar;
	(*vector).Z *= scalar;
}

void tn::TransformVector3(tn::Vector3* vector, tn::Matrix4x4 Matrix, float* ScalingFactorW)
{
	if (vector != nullptr)
	{
		(*vector).X = Matrix.a1 * (*vector).X + Matrix.a2 * (*vector).Y + Matrix.a3 * (*vector).Z + Matrix.a4;
		(*vector).Y = Matrix.b1 * (*vector).X + Matrix.b2 * (*vector).Y + Matrix.b3 * (*vector).Z + Matrix.b4;
		(*vector).Z = Matrix.c1 * (*vector).X + Matrix.c2 * (*vector).Y + Matrix.c3 * (*vector).Z + Matrix.c4;
		if (ScalingFactorW != nullptr)
		{
			(*ScalingFactorW) = Matrix.d1 * (*vector).X + Matrix.d2 * (*vector).Y + Matrix.d3 * (*vector).Z + Matrix.d4;
		}
	}
}

void tn::TransformVertex(Vertex* vertex, tn::Matrix4x4 Matrix, float* ScalingFactorW)
{
	if (vertex != nullptr)
	{
		float x = vertex->Position.X;
		float y = vertex->Position.Y;
		float z = vertex->Position.Z;

		vertex->Position.X = Matrix.a1 * x + Matrix.a2 * y + Matrix.a3 * z + Matrix.a4;
		vertex->Position.Y = Matrix.b1 * x + Matrix.b2 * y + Matrix.b3 * z + Matrix.b4;
		vertex->Position.Z = Matrix.c1 * x + Matrix.c2 * y + Matrix.c3 * z + Matrix.c4;
		if (ScalingFactorW != nullptr)
		{
			(*ScalingFactorW) = Matrix.d1 * x + Matrix.d2 * y + Matrix.d3 * z + Matrix.d4;
		}
	}
}

void tn::TransformRect(Rect* InputOutput, tn::Vector2 Vector)
{
	(*InputOutput).Left += Vector.X;
	(*InputOutput).Right += Vector.X;
	(*InputOutput).Top += Vector.Y;
	(*InputOutput).Bottom += Vector.Y;
}

void tn::TransformRect(Rect* InputOutput, tn::Vector3 Vector)
{
	Vector2 v = { Vector.X, Vector.Y };
	tn::TransformRect(InputOutput, v);
}

void tn::Convert2DVertexIntoRect(const Vertex Input[4], Rect* Output)
{
	(*Output).Left = min(min(min(Input[0].Position.X, Input[1].Position.X), Input[2].Position.X), Input[3].Position.X);
	(*Output).Right = max(max(max(Input[0].Position.X, Input[1].Position.X), Input[2].Position.X), Input[3].Position.X);
	(*Output).Top = min(min(min(Input[0].Position.Y, Input[1].Position.Y), Input[2].Position.Y), Input[3].Position.Y);
	(*Output).Bottom = max(max(max(Input[0].Position.Y, Input[1].Position.Y), Input[2].Position.Y), Input[3].Position.Y);
}

void tn::ConvertRectTo4Vector2(const Rect* Input, Vector2** Output)
{
	if (Input != nullptr)
	{
		Vector2 v[4] = {
			{ Input->Left, Input->Top },
			{ Input->Right, Input->Top },
			{ Input->Left, Input->Bottom },
			{ Input->Right, Input->Bottom },
		};

		if (Output != nullptr)
		{
			Vector2* vp = v;
			(*Output) = vp;
		}
	}
}

void GetAABB(const tn::Vertex rect[4], float& minX, float& minY, float& maxX, float& maxY) {
	minX = maxX = rect[0].Position.X;
	minY = maxY = rect[0].Position.Y;

	for (int i = 1; i < 4; i++) {
		if (rect[i].Position.X < minX) minX = rect[i].Position.X;
		if (rect[i].Position.X > maxX) maxX = rect[i].Position.X;
		if (rect[i].Position.Y < minY) minY = rect[i].Position.Y;
		if (rect[i].Position.Y > maxY) maxY = rect[i].Position.Y;
	}
}

#pragma endregion

#pragma region COLOR3

tn::Color3 tn::Color3::operator+(tn::Color3 c1) {
	return { max(0, min(this->Red + c1.Red, 1)), max(0, min(this->Green + c1.Green, 1)), max(0, min(this->Blue + c1.Blue, 1)) };
}

tn::Color3 tn::Color3::operator-(tn::Color3 c1) {
	return { max(0, min(this->Red - c1.Red, 1)), max(0, min(this->Green - c1.Green, 1)), max(0, min(this->Blue - c1.Blue, 1)) };
}

tn::Color3 tn::Color3::operator*(float Scalar) {
	return { max(0, min(this->Red * Scalar, 1)), max(0, min(this->Green * Scalar, 1)), max(0, min(this->Blue * Scalar, 1)) };
}

tn::Color3 tn::Color3::operator/(float Scalar) {
	return { max(0, min(this->Red / Scalar, 1)), max(0, min(this->Green / Scalar, 1)), max(0, min(this->Blue / Scalar, 1)) };
}

bool tn::Color3::operator==(Color3 c1) {
	return (this->Red == c1.Red and this->Green == c1.Green and this->Blue == c1.Blue);
}

bool tn::Color3::operator!=(Color3 c1) {
	return (this->Red != c1.Red and this->Green != c1.Green and this->Blue != c1.Blue);
}

bool tn::Color3::operator>(Color3 c1) {
	return (this->Red > c1.Red and this->Green > c1.Green and this->Blue > c1.Blue);
}

bool tn::Color3::operator<(Color3 c1) {
	return (this->Red < c1.Red and this->Green < c1.Green and this->Blue < c1.Blue);
}

bool tn::Color3::operator>=(Color3 c1) {
	return (this->Red >= c1.Red and this->Green >= c1.Green and this->Blue >= c1.Blue);
}

bool tn::Color3::operator<=(Color3 c1) {
	return (this->Red <= c1.Red and this->Green <= c1.Green and this->Blue <= c1.Blue);
}

#pragma endregion

#pragma region VECTOR3
tn::Vector3 tn::Vector3::operator+(const Vector3& v1) {
	return { this->X + v1.X, this->Y + v1.Y, this->Z + v1.Z };
}

tn::Vector3 tn::Vector3::operator-(const Vector3& v1) {
	return { this->X - v1.X, this->Y - v1.Y, this->Z - v1.Z };
}

tn::Vector3 tn::Vector3::operator*(float v1) {
	return { this->X * v1, this->Y * v1, this->Z * v1 };
}

tn::Vector3 tn::Vector3::operator/(float v1) {
	return { this->X / v1, this->Y / v1, this->Z / v1 };
}

tn::Vector3 tn::Vector3::operator+=(const Vector3& v1) {
	return { this->X + v1.X, this->Y + v1.Y, this->Z + v1.Z };
}

tn::Vector3 tn::Vector3::operator-=(const Vector3& v1) {
	return { this->X - v1.X, this->Y - v1.Y, this->Z - v1.Z };
}

tn::Vector3 tn::Vector3::operator*=(float v1) {
	return { this->X * v1, this->Y * v1, this->Z * v1 };
}

tn::Vector3 tn::Vector3::operator/=(float v1) {
	return { this->X / v1, this->Y / v1, this->Z / v1 };
}

bool tn::Vector3::operator==(Vector3 v1) {
	return (this->X == v1.X and this->Y == v1.Y and this->Z == v1.Z);
}

bool tn::Vector3::operator!=(Vector3 v1) {
	return (this->X != v1.X or this->Y != v1.Y or this->Z != v1.Z);
}

bool tn::Vector3::operator>(Vector3 v1) {
	return (this->X > v1.X and this->Y > v1.Y and this->Z > v1.Z);
}

bool tn::Vector3::operator<(Vector3 v1) {
	return (this->X < v1.X and this->Y < v1.Y and this->Z < v1.Z);
}

bool tn::Vector3::operator>=(Vector3 v1) {
	return (this->X >= v1.X and this->Y >= v1.Y and this->Z >= v1.Z);
}

bool tn::Vector3::operator<=(Vector3 v1) {
	return (this->X <= v1.X and this->Y <= v1.Y and this->Z <= v1.Z);
}

float tn::Vector3::Magnitude() {
	return sqrtf(powf(this->X, 2.0f) + powf(this->Y, 2.0f) + powf(this->Z, 2.0f));
}
#pragma endregion

#pragma region MATRIX 2X2

tn::Matrix2x2::Matrix2x2() {
	this->a1 = 0; this->a2 = 0;
	this->b1 = 0; this->b2 = 0; 
}

tn::Matrix2x2::Matrix2x2(float a1, float a2, float b1, float b2) {
	this->a1 = a1; this->a2 = a2;
	this->b1 = b1; this->b2 = b2;
}

tn::Matrix2x2 tn::Matrix2x2::operator+(Matrix2x2 m1) {
	this->a1 += m1.a1; this->a2 += m1.a2;
	this->b1 += m1.b1; this->b2 += m1.b2;

	return (*this);
}

tn::Matrix2x2 tn::Matrix2x2::operator-(Matrix2x2 m1) {
	this->a1 -= m1.a1; this->a2 -= m1.a2; 
	this->b1 -= m1.b1; this->b2 -= m1.b2; 

	return (*this);
}

tn::Matrix2x2 tn::Matrix2x2::operator*(Matrix2x2 m1) {
	Matrix2x2 r = {};
	r.a1 = this->a1 * m1.a1 + this->a2 * m1.b1;
	r.a2 = this->a1 * m1.a2 + this->a2 * m1.b2;
	r.b1 = this->b1 * m1.a1 + this->b2 * m1.b1;
	r.b2 = this->b1 * m1.a2 + this->b2 * m1.b2;
	return r;
}

tn::Matrix2x2 tn::Matrix2x2::operator*(float number) {
	this->a1 *= number; this->a2 *= number;
	this->b1 *= number; this->b2 *= number;

	return (*this);
}

tn::Matrix2x2 tn::Matrix2x2::operator/(float number) {
	this->a1 /= number; this->a2 /= number;
	this->b1 /= number; this->b2 /= number;

	return (*this);
}

#pragma endregion

#pragma region MATRIX 3X2

tn::Matrix3x2::Matrix3x2() {
	this->a1 = 0; this->a2 = 0; this->a3 = 0;
	this->b1 = 0; this->b2 = 0; this->b3 = 0;
}

tn::Matrix3x2::Matrix3x2(float a1, float a2, float a3, float b1, float b2, float b3) {
	this->a1 = a1; this->a2 = a2; this->a3 = a3;
	this->b1 = b1; this->b2 = b2; this->b3 = b3;
}

tn::Matrix3x2 tn::Matrix3x2::operator+(Matrix3x2 m1) {
	this->a1 += m1.a1; this->a2 += m1.a2; this->a3 += m1.a3;
	this->b1 += m1.b1; this->b2 += m1.b2; this->b3 += m1.b3;

	return (*this);
}

tn::Matrix3x2 tn::Matrix3x2::operator-(Matrix3x2 m1) {
	this->a1 -= m1.a1; this->a2 -= m1.a2; this->a3 -= m1.a3;
	this->b1 -= m1.b1; this->b2 -= m1.b2; this->b3 -= m1.b3;

	return (*this);
}

tn::Matrix2x2 tn::Matrix3x2::operator*(Matrix2x3 m1) {
	Matrix2x2 r = {};
	r.a1 = this->a1 * m1.a1 + this->a2 * m1.b1 + this->a3 * m1.c1;
	r.a2 = this->a1 * m1.a2 + this->a2 * m1.b2 + this->a3 * m1.c2;
	r.b1 = this->b1 * m1.a1 + this->b2 * m1.b1 + this->b3 * m1.c1;
	r.b2 = this->b1 * m1.a2 + this->b2 * m1.b2 + this->b3 * m1.c2;
	return r;
}

tn::Matrix3x2 tn::Matrix3x2::operator*(float number) {
	Matrix3x2 r = {};
	r.a1 = this->a1 * number; r.a2 = this->a2 * number; r.a3 = this->a3 * number;
	r.b1 = this->b1 * number; r.b2 = this->b2 * number; r.b3 = this->b3 * number;

	return r;
}

tn::Matrix3x2 tn::Matrix3x2::operator/(float number) {
	Matrix3x2 r = {};
	r.a1 = this->a1 / number; r.a2 = this->a2 / number; r.a3 = this->a3 / number;
	r.b1 = this->b1 / number; r.b2 = this->b2 / number; r.b3 = this->b3 / number;

	return r;
}

#pragma endregion

#pragma region MATRIX 2x3

tn::Matrix2x3::Matrix2x3() {
	this->a1 = 0; this->a2 = 0;
	this->b1 = 0; this->b2 = 0;
	this->c1 = 0; this->c2 = 0;
}

tn::Matrix2x3::Matrix2x3(float a1, float a2, float b1, float b2, float c1, float c2) {
	this->a1 = a1; this->a2 = a2;
	this->b1 = b1; this->b2 = b2; 
	this->c1 = c1; this->c2 = c2;
}

tn::Matrix2x3 tn::Matrix2x3::operator+(Matrix2x3 m1) {
	this->a1 += m1.a1; this->a2 += m1.a2;
	this->b1 += m1.b1; this->b2 += m1.b2;
	this->c1 += m1.c1; this->c2 += m1.c2;

	return (*this);
}

tn::Matrix2x3 tn::Matrix2x3::operator-(Matrix2x3 m1) {
	this->a1 -= m1.a1; this->a2 -= m1.a2;
	this->b1 -= m1.b1; this->b2 -= m1.b2;
	this->c1 -= m1.c1; this->c2 -= m1.c2;

	return (*this);
}

/*
tn::Matrix2x2 tn::Matrix2x3::operator*(Matrix3x2 m1) {
	Matrix2x2 r = {};
	r.a1 = this->a1 * m1.a1 + this->b1 * m1.a2 + this->c1 * m1.a3;
	r.a2 = this->a1 * m1.b1 + this->b1 * m1.b2 + this->c1 * m1.b3;
	r.b1 = this->a2 * m1.a1 + this->b2 * m1.a2 + this->c2 * m1.a3;
	r.b2 = this->a2 * m1.b1 + this->b2 * m1.b2 + this->c2 * m1.b3;
	return r;
}
*/

tn::Matrix2x3 tn::Matrix2x3::operator*(float number) {
	this->a1 *= number; this->a2 *= number;
	this->b1 *= number; this->b2 *= number;
	this->c1 *= number; this->c2 *= number;

	return (*this);
}

tn::Matrix2x3 tn::Matrix2x3::operator/(float number) {
	this->a1 *= number; this->a2 *= number;
	this->b1 *= number; this->b2 *= number;
	this->c1 *= number; this->c2 *= number;

	return (*this);
}

#pragma endregion

#pragma region MATRIX 4X4
tn::Matrix4x4::Matrix4x4() {
	
}

tn::Matrix4x4::Matrix4x4(const Matrix4x4& c_m4x4)
{
	this->a1 = c_m4x4.a1;
	this->a2 = c_m4x4.a3;
	this->a3 = c_m4x4.a3;
	this->a4 = c_m4x4.a4;
	this->b1 = c_m4x4.b1;
	this->b2 = c_m4x4.b2;
	this->b3 = c_m4x4.b3;
	this->b4 = c_m4x4.b4;
	this->c1 = c_m4x4.c1;
	this->c2 = c_m4x4.c2;
	this->c3 = c_m4x4.c3;
	this->c4 = c_m4x4.c4;
	this->d1 = c_m4x4.d1;
	this->d2 = c_m4x4.d2;
	this->d3 = c_m4x4.d3;
	this->d4 = c_m4x4.d4;
}

tn::Matrix4x4::Matrix4x4(const Matrix4x4* const cptr_m4x4_c)
{
	this->a1 = cptr_m4x4_c->a1;
	this->a2 = cptr_m4x4_c->a3;
	this->a3 = cptr_m4x4_c->a3;
	this->a4 = cptr_m4x4_c->a4;
	this->b1 = cptr_m4x4_c->b1;
	this->b2 = cptr_m4x4_c->b2;
	this->b3 = cptr_m4x4_c->b3;
	this->b4 = cptr_m4x4_c->b4;
	this->c1 = cptr_m4x4_c->c1;
	this->c2 = cptr_m4x4_c->c2;
	this->c3 = cptr_m4x4_c->c3;
	this->c4 = cptr_m4x4_c->c4;
	this->d1 = cptr_m4x4_c->d1;
	this->d2 = cptr_m4x4_c->d2;
	this->d3 = cptr_m4x4_c->d3;
	this->d4 = cptr_m4x4_c->d4;
}

tn::Matrix4x4::Matrix4x4(float a1, float a2, float a3, float a4,
	float b1, float b2, float b3, float b4,
	float c1, float c2, float c3, float c4,
	float d1, float d2, float d3, float d4) {
	this->a1 = a1;
	this->a2 = a2;
	this->a3 = a3;
	this->a4 = a4;
	this->b1 = b1;
	this->b2 = b2;
	this->b3 = b3;
	this->b4 = b4;
	this->c1 = c1;
	this->c2 = c2;
	this->c3 = c3;
	this->c4 = c4;
	this->d1 = d1;
	this->d2 = d2;
	this->d3 = d3;
	this->d4 = d4;
}

tn::Matrix4x4::Matrix4x4(float FLOAT4X4[16]) {
	this->a1 = FLOAT4X4[0];
	this->a2 = FLOAT4X4[1];
	this->a3 = FLOAT4X4[2];
	this->a4 = FLOAT4X4[3];
	this->b1 = FLOAT4X4[4];
	this->b2 = FLOAT4X4[5];
	this->b3 = FLOAT4X4[6];
	this->b4 = FLOAT4X4[7];
	this->c1 = FLOAT4X4[8];
	this->c2 = FLOAT4X4[9];
	this->c3 = FLOAT4X4[10];
	this->c4 = FLOAT4X4[11];
	this->d1 = FLOAT4X4[12];
	this->d2 = FLOAT4X4[13];
	this->d3 = FLOAT4X4[14];
	this->d4 = FLOAT4X4[15];
}

tn::Matrix4x4::Matrix4x4(DirectX::XMMATRIX xm1) {
	this->a1 = xm1.r[0].m128_f32[0];
	this->a2 = xm1.r[0].m128_f32[1];
	this->a3 = xm1.r[0].m128_f32[2];
	this->a4 = xm1.r[0].m128_f32[3];
	this->b1 = xm1.r[1].m128_f32[0];
	this->b2 = xm1.r[1].m128_f32[1];
	this->b3 = xm1.r[1].m128_f32[2];
	this->b4 = xm1.r[1].m128_f32[3];
	this->c1 = xm1.r[2].m128_f32[0];
	this->c2 = xm1.r[2].m128_f32[1];
	this->c3 = xm1.r[2].m128_f32[2];
	this->c4 = xm1.r[2].m128_f32[3];
	this->d1 = xm1.r[3].m128_f32[0];
	this->d2 = xm1.r[3].m128_f32[1];
	this->d3 = xm1.r[3].m128_f32[2];
	this->d4 = xm1.r[3].m128_f32[3];
}

tn::Matrix4x4::Matrix4x4(D3DMATRIX m1) {
	this->a1 = m1._11;
	this->a2 = m1._12;
	this->a3 = m1._13;
	this->a4 = m1._14;
	this->b1 = m1._21;
	this->b2 = m1._22;
	this->b3 = m1._23;
	this->b4 = m1._24;
	this->c1 = m1._31;
	this->c2 = m1._32;
	this->c3 = m1._33;
	this->c4 = m1._34;
	this->d1 = m1._41;
	this->d2 = m1._42;
	this->d3 = m1._43;
	this->d4 = m1._44;
}

tn::Matrix4x4::~Matrix4x4()
{

}

tn::Matrix4x4::operator DirectX::XMMATRIX() {
	DirectX::XMMATRIX xmt = DirectX::XMMATRIX(
		this->a1, this->b1, this->c1, this->d1,
		this->a2, this->b2, this->c2, this->d2,
		this->a3, this->b3, this->c3, this->d3,
		this->a4, this->b4, this->c4, this->d4
	);
	return xmt;
}

tn::Matrix4x4::operator D3DMATRIX() {
	D3DMATRIX mt = {};
	mt.m[0][0] = this->a1;
	mt.m[0][1] = this->b1;
	mt.m[0][2] = this->c1;
	mt.m[0][3] = this->d1;
	mt.m[1][0] = this->a2;
	mt.m[1][1] = this->b2;
	mt.m[1][2] = this->c2;
	mt.m[1][3] = this->d2;
	mt.m[2][0] = this->a3;
	mt.m[2][1] = this->b3;
	mt.m[2][2] = this->c3;
	mt.m[3][3] = this->d3;
	mt.m[3][0] = this->a4;
	mt.m[3][1] = this->b4;
	mt.m[3][2] = this->c4;
	mt.m[3][3] = this->d4;

	mt._11 = this->a1;
	mt._12 = this->b1;
	mt._13 = this->c1;
	mt._14 = this->d1;
	mt._21 = this->a2;
	mt._22 = this->b2;
	mt._23 = this->c2;
	mt._24 = this->d2;
	mt._31 = this->a3;
	mt._32 = this->b3;
	mt._33 = this->c3;
	mt._34 = this->d3;
	mt._41 = this->a4;
	mt._42 = this->b4;
	mt._43 = this->c4;
	mt._44 = this->d4;
	
	return mt;
}

tn::Matrix4x4::operator DirectX::XMMATRIX* () {
	return new DirectX::XMMATRIX(
		this->a1, this->a2, this->a3, this->a4,
		this->b1, this->b2, this->b3, this->b4,
		this->c1, this->c2, this->c3, this->c4,
		this->d1, this->d2, this->d3, this->d4
	);
}

tn::Matrix4x4::operator D3DMATRIX* () {
	return new D3DMATRIX(
		{
			this->a1, this->a2, this->a3, this->a4,
			this->b1, this->b2, this->b3, this->b4,
			this->c1, this->c2, this->c3, this->c4,
			this->d1, this->d2, this->d3, this->d4
		}
	);
}

tn::Matrix4x4::operator float* () {
	float m4x4[16] = {
		this->a1, this->a2, this->a3, this->a4,
		this->b1, this->b2, this->b3, this->b4,
		this->c1, this->c2, this->c3, this->c4,
		this->d1, this->d2, this->d3, this->d4
	};
	return m4x4;
}

tn::Matrix4x4 tn::Matrix4x4::operator+(tn::Matrix4x4 m1) {
	return {
		this->a1 + m1.a1, this->a2 + m1.a2, this->a3 + m1.a3, this->a4 + m1.a4,
		this->b1 + m1.b1, this->b2 + m1.b2, this->b3 + m1.b3, this->b4 + m1.b4,
		this->c1 + m1.c1, this->c2 + m1.c2, this->c3 + m1.c3, this->c4 + m1.c4,
		this->d1 + m1.d1, this->d2 + m1.d2, this->d3 + m1.d3, this->d4 + m1.d4,
	};
}

tn::Matrix4x4 tn::Matrix4x4::operator-(tn::Matrix4x4 m1) {
	return {
		this->a1 - m1.a1, this->a2 - m1.a2, this->a3 - m1.a3, this->a4 - m1.a4,
		this->b1 - m1.b1, this->b2 - m1.b2, this->b3 - m1.b3, this->b4 - m1.b4,
		this->c1 - m1.c1, this->c2 - m1.c2, this->c3 - m1.c3, this->c4 - m1.c4,
		this->d1 - m1.d1, this->d2 - m1.d2, this->d3 - m1.d3, this->d4 - m1.d4,
	};
}

tn::Matrix4x4 tn::Matrix4x4::operator*(tn::Matrix4x4 m1) {
	tn::Matrix4x4 calculated = {};
	calculated.a1 = this->a1 * m1.a1 + this->a2 * m1.b1 + this->a3 * m1.c1 + this->a4 * m1.d1;
	calculated.a2 = this->a1 * m1.a2 + this->a2 * m1.b2 + this->a3 * m1.c2 + this->a4 * m1.d2;
	calculated.a3 = this->a1 * m1.a3 + this->a2 * m1.b3 + this->a3 * m1.c3 + this->a4 * m1.d3;
	calculated.a4 = this->a1 * m1.a4 + this->a2 * m1.b4 + this->a3 * m1.c4 + this->a4 * m1.d4;

	calculated.b1 = this->b1 * m1.a1 + this->b2 * m1.b1 + this->b3 * m1.c1 + this->b4 * m1.d1;
	calculated.b2 = this->b1 * m1.a2 + this->b2 * m1.b2 + this->b3 * m1.c2 + this->b4 * m1.d2;
	calculated.b3 = this->b1 * m1.a3 + this->b2 * m1.b3 + this->b3 * m1.c3 + this->b4 * m1.d3;
	calculated.b4 = this->b1 * m1.a4 + this->b2 * m1.b4 + this->b3 * m1.c4 + this->b4 * m1.d4;

	calculated.c1 = this->c1 * m1.a1 + this->c2 * m1.b1 + this->c3 * m1.c1 + this->c4 * m1.d1;
	calculated.c2 = this->c1 * m1.a2 + this->c2 * m1.b2 + this->c3 * m1.c2 + this->c4 * m1.d2;
	calculated.c3 = this->c1 * m1.a3 + this->c2 * m1.b3 + this->c3 * m1.c3 + this->c4 * m1.d3;
	calculated.c4 = this->c1 * m1.a4 + this->c2 * m1.b4 + this->c3 * m1.c4 + this->c4 * m1.d4;

	calculated.d1 = this->d1 * m1.a1 + this->d2 * m1.b1 + this->d3 * m1.c1 + this->d4 * m1.d1;
	calculated.d2 = this->d1 * m1.a2 + this->d2 * m1.b2 + this->d3 * m1.c2 + this->d4 * m1.d2;
	calculated.d3 = this->d1 * m1.a3 + this->d2 * m1.b3 + this->d3 * m1.c3 + this->d4 * m1.d3;
	calculated.d4 = this->d1 * m1.a4 + this->d2 * m1.b4 + this->d3 * m1.c4 + this->d4 * m1.d4;

	return calculated;
}

tn::Matrix4x4 tn::Matrix4x4::operator*(float number) {
	return {
		this->a1 * number, this->a2 * number, this->a3 * number, this->a4 * number,
		this->b1 * number, this->b2 * number, this->b3 * number, this->b4 * number,
		this->c1 * number, this->c2 * number, this->c3 * number, this->c4 * number,
		this->d1 * number, this->d2 * number, this->d3 * number, this->d4 * number,
	};
}

tn::Matrix4x4 tn::Matrix4x4::operator/(float number) {
	return {
		this->a1 / number, this->a2 / number, this->a3 / number, this->a4 / number,
		this->b1 / number, this->b2 / number, this->b3 / number, this->b4 / number,
		this->c1 / number, this->c2 / number, this->c3 / number, this->c4 / number,
		this->d1 / number, this->d2 / number, this->d3 / number, this->d4 / number,
	};
}

tn::Matrix4x4 tn::Matrix4x4::operator+=(Matrix4x4 m1)
{
	this->a1 += m1.a1; this->a2 += m1.a2; this->a3 += m1.a3; this->a4 += m1.a4;
	this->b1 += m1.b1; this->b2 += m1.b2; this->b3 += m1.b3; this->b4 += m1.b4;
	this->c1 += m1.c1; this->c2 += m1.c2; this->c3 += m1.c3; this->c4 += m1.c4;
	this->d1 += m1.d1; this->d2 += m1.d2; this->d3 += m1.d3; this->d4 += m1.d4;

	return {
		this->a1, this->a2, this->a3, this->a4,
		this->b1, this->b2, this->b3, this->b4,
		this->c1, this->c2, this->c3, this->c4,
		this->d1, this->d2, this->d3, this->d4,
	};
}

tn::Matrix4x4 tn::Matrix4x4::operator-=(Matrix4x4 m1)
{
	this->a1 -= m1.a1; this->a2 -= m1.a2; this->a3 -= m1.a3; this->a4 -= m1.a4;
	this->b1 -= m1.b1; this->b2 -= m1.b2; this->b3 -= m1.b3; this->b4 -= m1.b4;
	this->c1 -= m1.c1; this->c2 -= m1.c2; this->c3 -= m1.c3; this->c4 -= m1.c4;
	this->d1 -= m1.d1; this->d2 -= m1.d2; this->d3 -= m1.d3; this->d4 -= m1.d4;

	return {
		this->a1, this->a2, this->a3, this->a4,
		this->b1, this->b2, this->b3, this->b4,
		this->c1, this->c2, this->c3, this->c4,
		this->d1, this->d2, this->d3, this->d4,
	};
}

tn::Matrix4x4 tn::Matrix4x4::operator*=(Matrix4x4 m1)
{
	this->a1 = this->a1 * m1.a1 + this->a2 * m1.b1 + this->a3 * m1.c1 + this->a4 * m1.d1;
	this->a2 = this->a1 * m1.a2 + this->a2 * m1.b2 + this->a3 * m1.c2 + this->a4 * m1.d2;
	this->a3 = this->a1 * m1.a3 + this->a2 * m1.b3 + this->a3 * m1.c3 + this->a4 * m1.d3;
	this->a4 = this->a1 * m1.a4 + this->a2 * m1.b4 + this->a3 * m1.c4 + this->a4 * m1.d4;

	this->b1 = this->b1 * m1.a1 + this->b2 * m1.b1 + this->b3 * m1.c1 + this->b4 * m1.d1;
	this->b2 = this->b1 * m1.a2 + this->b2 * m1.b2 + this->b3 * m1.c2 + this->b4 * m1.d2;
	this->b3 = this->b1 * m1.a3 + this->b2 * m1.b3 + this->b3 * m1.c3 + this->b4 * m1.d3;
	this->b4 = this->b1 * m1.a4 + this->b2 * m1.b4 + this->b3 * m1.c4 + this->b4 * m1.d4;

	this->c1 = this->c1 * m1.a1 + this->c2 * m1.b1 + this->c3 * m1.c1 + this->c4 * m1.d1;
	this->c2 = this->c1 * m1.a2 + this->c2 * m1.b2 + this->c3 * m1.c2 + this->c4 * m1.d2;
	this->c3 = this->c1 * m1.a3 + this->c2 * m1.b3 + this->c3 * m1.c3 + this->c4 * m1.d3;
	this->c4 = this->c1 * m1.a4 + this->c2 * m1.b4 + this->c3 * m1.c4 + this->c4 * m1.d4;

	this->d1 = this->d1 * m1.a1 + this->d2 * m1.b1 + this->d3 * m1.c1 + this->d4 * m1.d1;
	this->d2 = this->d1 * m1.a2 + this->d2 * m1.b2 + this->d3 * m1.c2 + this->d4 * m1.d2;
	this->d3 = this->d1 * m1.a3 + this->d2 * m1.b3 + this->d3 * m1.c3 + this->d4 * m1.d3;
	this->d4 = this->d1 * m1.a4 + this->d2 * m1.b4 + this->d3 * m1.c4 + this->d4 * m1.d4;

	return {
		this->a1, this->a2, this->a3, this->a4,
		this->b1, this->b2, this->b3, this->b4,
		this->c1, this->c2, this->c3, this->c4,
		this->d1, this->d2, this->d3, this->d4,
	};
}

tn::Matrix4x4 tn::Matrix4x4::operator*=(float number)
{
	this->a1 *= number; this->a2 *= number; this->a3 *= number; this->a4 *= number;
	this->b1 *= number; this->b2 *= number; this->b3 *= number; this->b4 *= number;
	this->c1 *= number; this->c2 *= number; this->c3 *= number; this->c4 *= number;
	this->d1 *= number; this->d2 *= number; this->d3 *= number; this->d4 *= number;

	return {
		this->a1, this->a2, this->a3, this->a4,
		this->b1, this->b2, this->b3, this->b4,
		this->c1, this->c2, this->c3, this->c4,
		this->d1, this->d2, this->d3, this->d4,
	};
}

tn::Matrix4x4 tn::Matrix4x4::operator/=(float number)
{
	this->a1 /= number; this->a2 /= number; this->a3 /= number; this->a4 /= number;
	this->b1 /= number; this->b2 /= number; this->b3 /= number; this->b4 /= number;
	this->c1 /= number; this->c2 /= number; this->c3 /= number; this->c4 /= number;
	this->d1 /= number; this->d2 /= number; this->d3 /= number; this->d4 /= number;

	return {
		this->a1, this->a2, this->a3, this->a4,
		this->b1, this->b2, this->b3, this->b4,
		this->c1, this->c2, this->c3, this->c4,
		this->d1, this->d2, this->d3, this->d4,
	};
}

#pragma endregion

tn::Matrix2x2 tn::MatrixTranspose(Matrix2x2 Matrix) {
	Matrix2x2 r = {};
	r.a1 = Matrix.a1; r.a2 = Matrix.b1;
	r.b1 = Matrix.a2; r.b2 = Matrix.b2;
	return r;
}

tn::Matrix3x2 tn::MatrixTranspose(Matrix2x3 Matrix) {
	Matrix3x2 r = {};
	r.a1 = Matrix.a1; r.a2 = Matrix.b1; r.a3 = Matrix.c1;
	r.b1 = Matrix.a2; r.b2 = Matrix.b2; r.b2 = Matrix.c1;
	return r;
}

tn::Matrix2x3 tn::MatrixTranspose(Matrix3x2 Matrix) {
	Matrix2x3 r = {};
	r.a1 = Matrix.a1; r.a2 = Matrix.b1; 
	r.b1 = Matrix.a2; r.b2 = Matrix.b2; 
	r.c1 = Matrix.a3; r.c2 = Matrix.b3;
	return r;
}

tn::Matrix4x4 tn::MatrixTranspose(Matrix4x4 Matrix) {
	tn::Matrix4x4 r = {};
	r.a1 = Matrix.a1; r.a2 = Matrix.b1; r.a3 = Matrix.c1; r.a4 = Matrix.d1;
	r.b1 = Matrix.a2; r.b2 = Matrix.b2; r.b3 = Matrix.c2; r.b4 = Matrix.d2;
	r.c1 = Matrix.a3; r.c2 = Matrix.b3; r.c3 = Matrix.c3; r.c4 = Matrix.d3;
	r.d1 = Matrix.a4; r.d2 = Matrix.b4; r.d3 = Matrix.c4; r.d4 = Matrix.d4;

	return r;
}

tn::Matrix4x4 tn::MatrixTranslation(Vector3 Vector3D) {
	tn::Matrix4x4 r = {
		1.0f, 0.0f, 0.0f, Vector3D.X,
		0.0f, 1.0f, 0.0f, Vector3D.Y,
		0.0f, 0.0f, 1.0f, Vector3D.Z,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
	return r;
}

tn::Matrix4x4 tn::MatrixScaling(Vector3 Vector3D) {
	tn::Matrix4x4 r = {
		Vector3D.X, 0.0f, 0.0f, 0.0f,
		0.0f, Vector3D.Y, 0.0f, 0.0f,
		0.0f, 0.0f, Vector3D.Z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
	return r;
}

tn::Matrix4x4 tn::MatrixOrthographicProjection(Vector2 LeftTop, Vector2 RightBottom, float Near, float Far) {
	tn::Matrix4x4 r = {
		2.0f / (RightBottom.X - LeftTop.X), 0.0f, 0.0f, -1.0f * ((RightBottom.X + LeftTop.X) / (RightBottom.X - LeftTop.X)),
		0.0f, 2.0f / (LeftTop.Y - RightBottom.Y), 0.0f, -1.0f * ((LeftTop.Y + RightBottom.Y) / (LeftTop.Y - RightBottom.Y)),
		0.0f, 0.0f, 2.0f / (Far - Near), -1.0f * (Near / (Far - Near)),
		0.0f, 0.0f, 0.0f, 1.0f,
	};
	return r;
}

tn::Matrix4x4 tn::MatrixPerspectiveProjectionLH(float FOV, float AspectRatio, float Near, float Far) {
	tn::Matrix4x4 r = DirectX::XMMatrixPerspectiveFovLH(FOV, AspectRatio, Near, Far);
	return r;
}

tn::Matrix4x4 tn::MatrixPerspectiveProjectionRH(float FOV, float AspectRatio, float Near, float Far) {
	tn::Matrix4x4 r = DirectX::XMMatrixPerspectiveFovRH(FOV, AspectRatio, Near, Far);
	return r;
}

tn::Matrix4x4 tn::MatrixLookAtLH(Vector3 EyePosition, Vector3 TargetPosition, Vector3 Up) {
	tn::Matrix4x4 r = DirectX::XMMatrixLookAtLH({ EyePosition.X, EyePosition.Y, EyePosition.Z }, { TargetPosition.X, TargetPosition.Y, TargetPosition.Z }, { TargetPosition.X, TargetPosition.Y, TargetPosition.Z });
	return r;
}

tn::Matrix4x4 tn::MatrixLookAtRH(Vector3 EyePosition, Vector3 TargetPosition, Vector3 Up) {
	tn::Matrix4x4 r = DirectX::XMMatrixLookAtRH({ EyePosition.X, EyePosition.Y, EyePosition.Z }, { TargetPosition.X, TargetPosition.Y, TargetPosition.Z }, { TargetPosition.X, TargetPosition.Y, TargetPosition.Z });
	return r;
}

tn::Matrix4x4 tn::MatrixRotationX(float Angle) {
	tn::Matrix4x4 r = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cosf(Angle), -sinf(Angle), 0.0f,
		0.0f, sinf(Angle), cosf(Angle), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
	return r;
}

tn::Matrix4x4 tn::MatrixRotationY(float Angle) {
	tn::Matrix4x4 r = {
		cosf(Angle), 0.0f, sinf(Angle), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sinf(Angle), 0.0f, cosf(Angle), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
	return r;
}

tn::Matrix4x4 tn::MatrixRotationZ(float Angle) {
	tn::Matrix4x4 r = {
		cosf(Angle), -sinf(Angle), 0.0f, 0.0f,
		sinf(Angle), cosf(Angle), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
	return r;
}

void tn::ConvertFromXMMATRIXtoD3DMATRIX(DirectX::XMMATRIX* input, D3DMATRIX* output) {
	if (output != nullptr) {
		output->m[0][0] = input->r[0].m128_f32[0];
		output->m[0][1] = input->r[0].m128_f32[1];
		output->m[0][2] = input->r[0].m128_f32[2];
		output->m[0][3] = input->r[0].m128_f32[3];
		output->m[1][0] = input->r[1].m128_f32[0];
		output->m[1][1] = input->r[1].m128_f32[1];
		output->m[1][2] = input->r[1].m128_f32[2];
		output->m[1][3] = input->r[1].m128_f32[3];
		output->m[2][0] = input->r[2].m128_f32[0];
		output->m[2][1] = input->r[2].m128_f32[1];
		output->m[2][2] = input->r[2].m128_f32[2];
		output->m[2][3] = input->r[2].m128_f32[3];
		output->m[3][0] = input->r[3].m128_f32[0];
		output->m[3][1] = input->r[3].m128_f32[1];
		output->m[3][2] = input->r[3].m128_f32[2];
		output->m[3][3] = input->r[3].m128_f32[3];
	}
}

void tn::ConvertFromD3DMATRIXtoXMMATRIX(D3DMATRIX* input, DirectX::XMMATRIX* output) {
	if (output != nullptr) {
		*output = DirectX::XMMATRIX(
			input->_11, input->_12, input->_13, input->_14,
			input->_21, input->_22, input->_23, input->_24,
			input->_31, input->_32, input->_33, input->_34,
			input->_41, input->_42, input->_43, input->_44
		);
	}
}

float tn::ConvertDegreeToRadian(float Degree) {
	const float pi = 3.14159265359f;
	return (Degree * (pi / 180.0f));
}

float tn::ConvertRadianToDegree(float Radian) {
	const float pi = 3.14159265359f;
	return Radian * (180.0f / pi);
}

tn::Vector3 operator+(const tn::Vector3& v1, const tn::Vector3& v2)
{
	tn::Vector3 v = { v1.X + v2.X, v1.Y + v2.Y, v1.Z + v2.Z };
	return v;
}

tn::Vector3 operator-(const tn::Vector3& v1, const tn::Vector3& v2)
{
	tn::Vector3 v = { v1.X - v2.X, v1.Y - v2.Y, v1.Z - v2.Z };
	return v;
}

tn::Linear2Alt tn::Linear2Alt::operator=(const Linear2& linear2)
{
	return { linear2.Slope, -1, linear2.Y_Intercept };
}

tn::Linear2 tn::Linear2::operator=(const Linear2Alt& linearalt)
{
	return { -linearalt.a / linearalt.b, -linearalt.c / linearalt.b };
}

float tn::InputLinearY(Linear2 Line, float X)
{
	return Line.Slope * X + Line.Y_Intercept;
}

float tn::InputLinear(Linear2Alt Line, float X, float Y)
{
	return Line.a * X + Line.b * Y + Line.c;
}

float tn::InputLinearY(Linear2Alt Line, float X)
{
	return -(Line.c + Line.a * X) / Line.b;
}

float tn::InputParabolicY(Parabolic2 Parabol, float X)
{
	return Parabol.a * X * X + Parabol.b * X + Parabol.c;
}

float tn::LineSegment::GetLength()
{
	return (Point2 - Point1).Magnitude();
}

tn::Linear2Alt tn::LineSegment::GetLinear()
{
	tn::Linear2Alt linear = {};

	const float dx = this->Point2.X - this->Point1.X;
	const float dy = this->Point2.Y - this->Point1.Y;

	linear.a = this->Point1.Y - this->Point2.Y;
	linear.b = this->Point2.X - this->Point1.X;
	linear.c = -(linear.a * this->Point1.X + linear.b * this->Point1.Y);

	if (dx == 0.0f) {
		linear.a = 1.0f;
		linear.b = 0.0f;
		linear.c = -this->Point1.X;
	}
	else if (dy == 0.0f) {
		linear.a = 0.0f;
		linear.b = 1.0f;
		linear.c = -this->Point1.Y;
	}

	return linear;
}

int tn::GetIntersection(Linear2* Line1, Linear2* Line2, Vector2* Output)
{
	if (Line1 == nullptr or Line2 == nullptr)
	{
		return tn::GetIntersection_InvalidParameters;
	}

	if (Line1->Slope == Line2->Slope)
	{
		if (Line1->Y_Intercept == Line2->Y_Intercept)
		{
			return tn::GetIntersection_BothLinesAreExactLines;
		}
		return tn::GetIntersection_BothLinesParallel;
	}

	float X = (Line2->Y_Intercept - Line1->Y_Intercept) / (Line1->Slope - Line2->Slope);
	float Y = Line1->Slope * X + Line1->Y_Intercept;

	if (Output)
	{
		Output->X = X;
		Output->Y = Y;
	}
	return tn::GetIntersection_BothLinesHaveAnIntersection;
}

int tn::GetIntersection(Linear2Alt* Line1, Linear2Alt* Line2, Vector2* Output)
{
	if (Line1 == nullptr or Line2 == nullptr)
	{
		return tn::GetIntersection_InvalidParameters;
	}

	float det = Line1->a * Line2->b - Line2->a * Line1->b;

	if (det == 0 or (Line1->a == Line2->a and Line1->b == Line2->b))
	{
		if (Line1->c == Line2->c)
		{
			return tn::GetIntersection_BothLinesAreExactLines;
		}
		return tn::GetIntersection_BothLinesParallel;
	}

	float x = (Line1->b * Line2->c - Line2->b * Line1->c) / det;
	float y = (Line2->a * Line1->c - Line1->a * Line2->c) / det;

	if (Output)
	{
		Output->X = x;
		Output->Y = y;
	}
	return tn::GetIntersection_BothLinesHaveAnIntersection;
}

int tn::GetIntersection(Linear2Alt* Line, LineSegment* LineSegment, Vector2* Output)
{
	if (Line == nullptr or LineSegment == nullptr)
	{
		return tn::GetIntersection_InvalidParameters;
	}

	Linear2Alt segLine = LineSegment->GetLinear();

	float det = Line->a * segLine.b - segLine.a * Line->b;

	if (det == 0)
	{
		return tn::GetIntersection_NoIntersections;
	}

	float x = (Line->b * segLine.c - segLine.b * Line->c) / det;
	float y = (segLine.a * Line->c - Line->a * segLine.c) / det;

	// Check if intersection point (x, y) lies within the segment
	if (IsBetween(x, LineSegment->Point1.X, LineSegment->Point2.X) and
		IsBetween(y, LineSegment->Point1.Y, LineSegment->Point2.Y)) 
	{
		if (Output)
		{
			Output->X = x;
			Output->Y = y;
		}
	}
	else
	{
		return tn::GetIntersection_NoIntersections;
	}

	return tn::GetIntersection_BothLinesHaveAnIntersection;
}

int tn::GetIntersection(Linear2Alt* Line, Rect* Rect, LineSegment* Output)
{
	LineSegment edges[4] = {
		{ { Rect->Left,  Rect->Top },    { Rect->Left,  Rect->Bottom } },  // Left
		{ { Rect->Right, Rect->Top },    { Rect->Right, Rect->Bottom } },  // Right
		{ { Rect->Left,  Rect->Top },    { Rect->Right, Rect->Top } },     // Top
		{ { Rect->Left,  Rect->Bottom }, { Rect->Right, Rect->Bottom } }   // Bottom
	};

	tn::Vector2 intersections[2];
	int count = 0;

	for (int i = 0; i < 4; ++i)
	{
		if (count >= 2) break;

		tn::Vector2 pt;
		int result = tn::GetIntersection(Line, &edges[i], &pt);
		if (result == tn::GetIntersection_BothLinesHaveAnIntersection)
		{
			if (count == 0 ||
				(tn::math::absolute(pt.X - intersections[0].X) > 0 ||
					tn::math::absolute(pt.Y - intersections[0].Y) > 0))
			{
				intersections[count++] = pt;
			}
		}
	}

	if (count == 2)
	{
		if (Output)
		{
			Output->Point1 = intersections[0];
			Output->Point2 = intersections[1];
		}
		return tn::GetIntersection_BothLineAndRectHaveLineSegmentIntersection;
	}
	else if (count == 1)
	{
		if (Output)
		{
			Output->Point1 = Output->Point2 = intersections[0];
		}
		return tn::GetIntersection_BetweenLineAndRect_OnlyHavePointIntersection;
	}
	else
	{
		return tn::GetIntersection_NoIntersections;
	}
}

int tn::ConvertRayToLinear2(Ray2D ray, Linear2* Output)
{
	if (Output)
	{
		if (ray.Direction.X == 0.0f) {
			return -1;
		}

		float a = ray.Direction.Y / ray.Direction.X;
		float b = ray.Origin.Y - a * ray.Origin.X;

		Output->Slope = a;
		Output->Y_Intercept = b;
	}

	return 0;
}

void tn::ConvertRayToLinear2(Ray2D ray, Linear2Alt* Output)
{
	if (Output)
	{
		// Normal vector to direction
		float a = -ray.Direction.Y;
		float b = ray.Direction.X;

		// Compute c using the origin
		float c = -(a * ray.Origin.X + b * ray.Origin.Y);

		// Fill in the output
		Output->a = a;
		Output->b = b;
		Output->c = c;
	}
}

bool tn::AABB_Collision(const Vertex s1[4], const Vertex s2[4])
{
	float minA_X, minA_Y, maxA_X, maxA_Y;
	float minB_X, minB_Y, maxB_X, maxB_Y;

	GetAABB(s1, minA_X, minA_Y, maxA_X, maxA_Y);
	GetAABB(s2, minB_X, minB_Y, maxB_X, maxB_Y);

	// Standard AABB check using min/max values
	return (minA_X < maxB_X && maxA_X > minB_X && minA_Y < maxB_Y && maxA_Y > minB_Y);
}

bool tn::AABB_Collision(const Rect& s1, const Rect& s2)
{
	if (s1.Left <= s2.Right and s1.Right >= s2.Left and s1.Top <= s2.Bottom and s1.Bottom >= s2.Top)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool tn::AABB_Collision(Ray2D* Ray, Rect* Rect)
{
	// Ray direction
	float dx = Ray->Direction.X;
	float dy = Ray->Direction.Y;

	// Avoid division by zero
	float invDx = (dx != 0.0f) ? 1.0f / dx : 1e30f;
	float invDy = (dy != 0.0f) ? 1.0f / dy : 1e30f;

	float tx1 = (Rect->Left - Ray->Origin.X) * invDx;
	float tx2 = (Rect->Right - Ray->Origin.X) * invDx;
	float ty1 = (Rect->Top - Ray->Origin.Y) * invDy;
	float ty2 = (Rect->Bottom - Ray->Origin.Y) * invDy;

	float tmin = max(min(tx1, tx2), min(ty1, ty2));
	float tmax = min(max(tx1, tx2), max(ty1, ty2));

	if (tmax < 0 || tmin > tmax)
		return false;

	//outDistance = tmin;
	return true;
}