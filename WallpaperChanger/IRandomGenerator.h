#pragma once
class IRandomGenerator
{
public:
	virtual ~IRandomGenerator() = default;
	virtual int GetRandomIndex(int maxIndex) = 0;
};

