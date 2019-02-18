#include "pch.h"
#include "ExceptionCount.h"

ExceptionCount::ExceptionCount()
	: count{ uncaught_exceptions() }
{
}

bool ExceptionCount::IsSafeToThrow() const
{
	return count == uncaught_exceptions();
}
