#pragma once

class ExceptionCount
{
public:
	ExceptionCount();
	ExceptionCount(const ExceptionCount& rhs) = default;
	ExceptionCount(ExceptionCount&& rhs) = default;
	~ExceptionCount() = default;

	ExceptionCount& operator=(const ExceptionCount& rhs) = default;
	ExceptionCount& operator=(ExceptionCount&& rhs) = default;

	bool IsSafeToThrow() const;

private:
	int count = 0;
};
