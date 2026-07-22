#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#pragma once
#include <math.h>
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <vector>
#include "canvas_gui.h"
#include <stringapiset.h>
#include <cmath> // for std::isfinite
#define RAD(degs) double(degs) * PI / 180
#define DEG(rads) double(rads) * 180 / PI

	constexpr auto PI = 3.1415926535897932;
	constexpr auto FLOAT_NON_FRACTIONAL = 8388608 /* All single-precision floating point numbers greater than or equal to this have no fractional value. */;
	constexpr auto INV_PI = 0.31830988618;
	constexpr auto HALF_PI = 1.57079632679;
	constexpr auto DEG_TO_RAD = PI / 180;
	constexpr auto RADS_DIVIDED_BY_2 = DEG_TO_RAD / 2;

struct FVector
{
	bool is_null() {
		if (X == 0 && Y == 0 && Z == 0)
			return true;

		return false;
	}

	double X, Y, Z;
	FVector GetSafeNormal(double Tolerance = 1e-6) const
	{
		const double squareSum = X * X + Y * Y + Z * Z;
		if (squareSum > Tolerance)
		{
			const double scale = 1 / sqrt(squareSum);
			return FVector(X * scale, Y * scale, Z * scale);
		}
		return FVector(0, 0, 0);
	}

	FVector() : X(0), Y(0), Z(0) {}

	FVector(double X, double Y, double z) : X(X), Y(Y), Z(z) {}

	FVector(double InF) : X(InF), Y(InF), Z(InF) { }

	FVector ClampFVector() {
		while (X < -88) X += 360;
		//    if (X == -89.f) X = !-89;
		while (X > 89) X -= 89;

		//if (X < 0)X >= 0;
		//if (X == -89.f) X != -89.f;
		while (Y < -180) Y += 360;
		while (Y > 180) Y -= 360;
		Z = 0;

		return FVector{ X, Y,Z };

	}
	bool IsFinite() const {
		return std::isfinite(X) && std::isfinite(Y) && std::isfinite(Z);
	}
	inline double Distance(FVector v)
	{
		double X = this->X - v.X;
		double Y = this->Y - v.Y;
		double z = this->Z - v.Z;

		return sqrt((X * X) + (Y * Y) + (z * z)) * 0.03048;
	}
	bool IsNearlyZero(double Tolerance = 1e-6) const
	{
		return (fabs(X) <= Tolerance) && (fabs(Y) <= Tolerance) && (fabs(Z) <= Tolerance);
	}
	bool IsNormalized(double Tolerance = 1e-4) const
	{
		return fabs(SizeSquared() - 1) <= Tolerance;
	}
	void normalize() {
		double len = sqrt(X * X + Y * Y + Z * Z);
		if (len > 0.0) {
			double inv_len = 1.0 / len;
			X = inv_len; Y = inv_len; Z *= inv_len;
		}
	}
	FVector normalize2() const {
		double len = std::sqrt(X * X + Y * Y + Z * Z);
		if (len == 0.0)
			return FVector{ 0, 0, 0 };
		double inv_len = 1.0 / len;
		return FVector{ X * inv_len, Y * inv_len, Z * inv_len };
	}


	float dot(const FVector& other) const
	{
		return X * other.X + Y * other.Y + Z * other.Z;
	}
	inline double Dot(const FVector& B) const { return (X * B.X) + (Y * B.Y) + (Z * B.Z); }

	inline double MagnitudeSqr() const { return Dot(*this); }

	inline double Magnitude() const { return std::sqrt(MagnitudeSqr()); }

	double Size() const { return sqrt(X * X + Y * Y + Z * Z); }

	double Sum() const { return X + Y + Z; }

	double Size2D() const { return sqrt(X * X + Y * Y); }

	double SizeSquared() const { return X * X + Y * Y + Z * Z; }

	double DistTo(const FVector& V) const { return (*this - V).Size(); }

	FVector operator+(const FVector& other) const { return FVector(X + other.X, Y + other.Y, Z + other.Z); }

	FVector operator-(const FVector& other) const { return FVector(X - other.X, Y - other.Y, Z - other.Z); }

	FVector operator*(const FVector& V) const { return FVector(X * V.X, Y * V.Y, Z * V.Z); }

	FVector operator/(const FVector& V) const { return FVector(X / V.X, Y / V.Y, Z / V.Z); }

	bool operator==(const FVector& V) const { return X == V.X && Y == V.Y && Z == V.Z; }

	bool operator!=(const FVector& V) const { return X != V.X || Y != V.Y || Z != V.Z; }

	FVector operator-() const { return FVector(-X, -Y, -Z); }

	FVector operator+(double Bias) const { return FVector(X + Bias, Y + Bias, Z + Bias); }

	FVector operator-(double Bias) const { return FVector(X - Bias, Y - Bias, Z - Bias); }

	FVector operator*(double Scale) const { return FVector(X * Scale, Y * Scale, Z * Scale); } const

		FVector operator/(double Scale) const { const double RScale = 1 / Scale; return FVector(X * RScale, Y * RScale, Z * RScale); }

	FVector operator=(const FVector& V) { X = V.X; Y = V.Y; Z = V.Z; return *this; }

	FVector operator+=(const FVector& V) { X += V.X; Y += V.Y; Z += V.Z; return *this; }

	FVector operator-=(const FVector& V) { X -= V.X; Y -= V.Y; Z -= V.Z; return *this; }

	FVector operator*=(const FVector& V) { X *= V.X; Y *= V.Y; Z *= V.Z; return *this; }

	FVector operator/=(const FVector& V) { X /= V.X; Y /= V.Y; Z /= V.Z; return *this; }

	FVector operator*=(double Scale) { X *= Scale; Y *= Scale; Z *= Scale; return *this; }

	FVector operator/=(double V) { const double RV = 1 / V; X *= RV; Y *= RV; Z *= RV; return *this; }

	double operator|(const FVector& V) const { return X * V.X + Y * V.Y + Z * V.Z; }

	FVector operator^(const FVector& V) const { return FVector(Y * V.Z - Z * V.Y, Z * V.X - X * V.Z, X * V.Y - Y * V.X); }

	bool IsValid()
	{
		if (X == 0 || Y == 0 || Z == 0)
			return false;
		else
			return true;
	}
	static const FVector ZeroVector;

	static const FVector OneVector;
};
class FTextData
{
private:
	std::uint8_t Padding[0x28];

public:
	wchar_t* Name;
	std::int32_t* Length;
};

class FText
{
public:
	FTextData* Data;

private:
	std::uint8_t Padding[0x10];
};
namespace Math
{
	inline FVector ToRotator(const FVector& vec)
	{
		double pitch = atan2(vec.Z, sqrt(vec.X * vec.X + vec.Y * vec.Y)) * (180.0 / PI);
		double yaw = atan2(vec.Y, vec.X) * (180.0 / PI);
		return FVector(pitch, yaw, 0.0);
	}

	double RadianToDegree(double radian)
	{
		return radian * (180 / M_PI);
	}

	double DegreeToRadian(double degree)
	{
		return degree * (M_PI / 180);

	}

	FVector RadianToDegree(FVector radians)
	{
		FVector degrees;
		degrees.X = radians.X * (180 / M_PI);
		degrees.Y = radians.Y * (180 / M_PI);
		degrees.Z = radians.Z * (180 / M_PI);
		return degrees;
	}

	FVector DegreeToRadian(FVector degrees)
	{
		FVector radians;
		radians.X = degrees.X * (M_PI / 180);
		radians.Y = degrees.Y * (M_PI / 180);
		radians.Z = degrees.Z * (M_PI / 180);
		return radians;
	}

	void normalize(FVector& in)
	{
		if (in.X > 89) in.X -= 360;
		else if (in.X < -89.f) in.X += 360;

		// in.Y = fmodf(in.Y, 360.0f);
		while (in.Y > 180)in.Y -= 360;
		while (in.Y < -180)in.Y += 360;
		in.Z = 0;
	}
	__forceinline void Clamp(FVector& Ang) {
		if (Ang.X < 0.f)
			Ang.X += 360;

		if (Ang.X > 360)
			Ang.X -= 360;

		if (Ang.Y < 0.f) Ang.Y += 360;
		if (Ang.Y > 360) Ang.Y -= 360;
		Ang.Z = 0.f;
	}
	FVector SmoothAim(const FVector& target, const FVector& delta_rotation, float smooth) {
		FVector diff = target - delta_rotation;
		normalize(diff);
		return delta_rotation + (diff / smooth);
	}
	FVector CalcAngle(FVector src, FVector dst)
	{
		FVector angle;
		angle.X = -atan2(dst.X - src.X, dst.Y - src.Y) / M_PI * 180 + 180;
		angle.Y = asin((dst.Z - src.Z) / src.Distance(dst)) * 180. / M_PI;
		angle.Z = 0;

		return angle;
	}
	void NormalizeAngles(FVector& angle)
	{
		while (angle.X > 89)
			angle.X -= 180;

		while (angle.X < -89)
			angle.X += 180;

		while (angle.Y > 180)
			angle.Y -= 360;

		while (angle.Y < -180)
			angle.Y += 360;
	}
	FVector CalcAngleBackSideRotation(FVector src, FVector dst)
	{
		FVector angle;
		FVector delta = FVector((src.X - dst.X), (src.Y - dst.Y), (src.Z - dst.Z));

		double hyp = sqrt(delta.X * delta.X + delta.Y * delta.Y);

		angle.X = atan(delta.Z / hyp) * (180 / hyp);
		angle.Y = atan(delta.Y / delta.X) * (180 / M_PI);
		angle.Z = 0;
		if (delta.X >= 0.0) angle.Y += 180;

		return angle;
	}
	float GetFov(const FVector viewAngle, const FVector aimAngle)
	{
		FVector Delta = FVector(aimAngle.X - viewAngle.X, aimAngle.Y - viewAngle.Y, aimAngle.Z - viewAngle.Z);
		NormalizeAngles(Delta);
		return sqrt(pow(Delta.X, 2) + pow(Delta.Y, 2));
	}
	FVector RCS(FVector Target, FVector Camera_rotation, double SmoothFactor, FVector control_rot) {
		FVector ConvertRotation = Camera_rotation;
		normalize(ConvertRotation);
		FVector DeltaRotation = ConvertRotation - control_rot;
		normalize(DeltaRotation);

		ConvertRotation = Target - (DeltaRotation * SmoothFactor);
		normalize(ConvertRotation);

		FVector Smoothed = SmoothAim(Camera_rotation, ConvertRotation, SmoothFactor);
		Smoothed -= (DeltaRotation / SmoothFactor);
		Clamp(Smoothed);
		return Smoothed;
	}
	double CalculateDistance(int p1x, int p1y, int p2x, int p2y)
	{
		double diffY = p1y - p2y;
		double diffX = p1x - p2x;
		return sqrt((diffY * diffY) + (diffX * diffX));
	}

}
class FLinearColor
{
public:
	float R, G, B, A;
	FLinearColor() : R(0.f), G(0.f), B(0.f), A(1.f) {}
	FLinearColor(float r, float g, float b, float a = 1.f) : R(r), G(g), B(b), A(a) {}
	

};

struct FVector2D
{
	double X, Y;

	FVector2D() : X(0), Y(0) {}

	FVector2D(double x, double y) : X(x), Y(y) {}

	double Size() const;

	FVector2D operator + (const FVector2D& other) const { return FVector2D(X + other.X, Y + other.Y); }

	FVector2D operator- (const FVector2D& other) const { return FVector2D(X - other.X, Y - other.Y); }

	FVector2D operator* (double scalar) const { return FVector2D(X * scalar, Y * scalar); }

	FVector2D& operator=  (const FVector2D& other) { X = other.X; Y = other.Y; return *this; }

	FVector2D& operator+= (const FVector2D& other) { X += other.X; Y += other.Y; return *this; }

	FVector2D& operator-= (const FVector2D& other) { X -= other.X; Y -= other.Y; return *this; }

	FVector2D& operator*= (const double other) { X *= other; Y *= other; return *this; }

	friend bool operator==(const FVector2D& one, const FVector2D& two) { return one.X == two.X && one.Y == two.Y; }

	friend bool operator!=(const FVector2D& one, const FVector2D& two) { return !(one == two); }

	friend bool operator>(const FVector2D& one, const FVector2D& two) { return one.X > two.X && one.Y > two.Y; }

	friend bool operator<(const FVector2D& one, const FVector2D& two) { return one.X < two.X && one.Y < two.Y; }
	bool IsValid()
	{
		if (X == 0 || Y == 0)
			return false;
		else
			return true;
	}
};
struct alignas(16) FPlane : public FVector {
	double W;
};
enum class EAresAlliance : uint8_t {
	Alliance_Ally = 0,
	Alliance_Enemy = 1,
	Alliance_Neutral = 2,
	Alliance_Any = 3,
	Alliance_Count = 4,
	Alliance_MAX = 5
};
enum class EAresOutlineMode : uint8_t {
	None = 0,
	Outline = 1,
	Block = 2,
	Enemy = 3,
	AlwaysOutline = 4,
	AlwaysEnemy = 5,
	EAresOutlineMode_MAX = 6
};
struct FMatrix {

	struct FPlane XPlane;
	struct FPlane YPlane;
	struct FPlane ZPlane;
	struct FPlane WPlane;

};

static FMatrix ToMatrix(FVector rot) {
	FVector origin = FVector{};
	double radPitch = (rot.X * double(M_PI) / 180);
	double radYaw = (rot.Y * double(M_PI) / 180);
	double radRoll = (rot.Z * double(M_PI) / 180);

	double SP = sin(radPitch);
	double CP = cos(radPitch);
	double SY = sin(radYaw);
	double CY = cos(radYaw);
	double SR = sin(radRoll);
	double CR = cos(radRoll);

	FMatrix matrix;
	matrix.XPlane.X = CP * CY;
	matrix.XPlane.Y = CP * SY;
	matrix.XPlane.Z = SP;
	matrix.XPlane.W = 0;

	matrix.YPlane.X = SR * SP * CY - CR * SY;
	matrix.YPlane.Y = SR * SP * SY + CR * CY;
	matrix.YPlane.Z = -SR * CP;
	matrix.YPlane.W = 0;

	matrix.ZPlane.X = -(CR * SP * CY + SR * SY);
	matrix.ZPlane.Y = CY * SR - CR * SP * SY;
	matrix.ZPlane.Z = CR * CP;
	matrix.ZPlane.W = 0;

	matrix.WPlane.X = origin.X;
	matrix.WPlane.Y = origin.Y;
	matrix.WPlane.Z = origin.Z;
	matrix.WPlane.W = 1;

	return matrix;
}
template<class type> class TArray {
public: friend class FString;
public:
	TArray() : Data(nullptr), Count(std::int32_t()), Max(std::int32_t()) {}
	TArray(type* Data, std::int32_t Count, std::int32_t maxx) : Data(Data), Count(Count), Max(maxx) {}
	TArray(std::initializer_list<type> list) {
		Count = Max = static_cast<std::int32_t>(list.size());
		Data = new type[Max];
		std::int32_t i = 0;
		for (const auto& item : list)
			Data[i++] = item;
	}


	type& front() { return Data[0]; }
	const type& front() const { return Data[0]; }

	std::int32_t Num() const { return Count; }
	const bool is_valid() const noexcept
	{
		return !(this->Data == nullptr);
	}
	void Add(const type& value)
	{
		if (Count >= Max) // Expand
		{
			std::int32_t newMax = (Max == 0) ? 1 : Max * 2;
			type* newData = new type[newMax];
			for (std::int32_t i = 0; i < Count; i++)
				newData[i] = Data[i];
			delete[] Data;
			Data = newData;
			Max = newMax;
		}
		Data[Count++] = value;
	}
	const std::int32_t size() const noexcept
	{
		return this->Count;
	}

	type& operator[](std::int32_t index) noexcept
	{
		return this->Data[index];
	}

	const type& operator[](std::int32_t index) const noexcept
	{
		return this->Data[index];
	}

	bool is_valid_index(std::int32_t index) const noexcept
	{
		return index < this->size();
	}
	// ✅ Range-based loops
	type* begin() noexcept { return Data; }
	type* end() noexcept { return Data + Count; }
	const type* begin() const noexcept { return Data; }
	const type* end() const noexcept { return Data + Count; }

	// ✅ back()
	type& back() { return Data[Count - 1]; }
	const type& back() const { return Data[Count - 1]; }
protected:
	type* Data;
	std::int32_t Count;
	std::int32_t Max;
};
class FString : public TArray<wchar_t>
{
public:
	inline FString()
	{
	};
	inline int Len() const
	{
		return IsValid() ? static_cast<int>(std::wcslen(Data)) : 0;
	}

	
	FString(const wchar_t* other)
	{
		Max = Count = *other ? static_cast<int>(std::wcslen(other)) + 1 : 0;

		if (Count)
		{
			Data = const_cast<wchar_t*>(other);
		}
	};
	FString(const wchar_t* other, int count)
	{
		Data = const_cast<wchar_t*>(other);;
		Max = Count = count;
	};

	inline bool IsValid() const
	{
		return Data != nullptr;
	}

	inline const wchar_t* wide() const
	{
		return Data;
	}
	const wchar_t* c_str() const
	{
		return this->Data;
	}
	int multi(char* name, int size) const
	{
		return WideCharToMultiByte(CP_UTF8, 0, Data, Count, name, size, nullptr, nullptr) - 1;
	}

	std::string ToString() const
	{
		auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}
	bool operator<(const FString& other) const {
		return std::wcscmp(this->Data, other.Data) < 0;
	}

	bool contains(const wchar_t* substr) const
	{
		if (!IsValid() || !substr)
			return false;

		return std::wcsstr(Data, substr) != nullptr;
	}

};