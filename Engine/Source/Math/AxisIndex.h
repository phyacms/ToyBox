﻿// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

#define EnumerateAxisIndex(Enumerate)	\
    Enumerate(X, 0)						\
    Enumerate(Y, 1)						\
    Enumerate(Z, 2)						\
	Enumerate(W, 3)

#define DeclareAxisIndexOperations(Name, Index)							\
inline ValueType& Name() & noexcept { return At(Index); }				\
inline const ValueType& Name() const& noexcept { return At(Index); }	\
inline ValueType Name() const&& noexcept { return At(Index); }			\
template<																\
	typename ReturnType,												\
	typename = std::enable_if_t<										\
		(Dimension > Index) &&											\
		std::is_convertible_v<ValueType, ReturnType>>>					\
inline ReturnType Name() const noexcept { return static_cast<ReturnType>(At(Index)); }
