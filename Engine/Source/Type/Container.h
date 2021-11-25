// Copyrights 2021 by phyacms. All Rights Reserved.

#pragma once

#include "Engine.h"

#define EnumerateNonConstIterators(Container, Enumerate)	\
	Enumerate(Container, begin)								\
	Enumerate(Container, end)

#define EnumerateConstIterators(Container, Enumerate)	\
	EnumerateNonConstIterators(Container, Enumerate)	\
	Enumerate(Container, cbegin)						\
	Enumerate(Container, cend)

#define EnumerateNonConstReverseIterators(Container, Enumerate)	\
	Enumerate(Container, rbegin)								\
	Enumerate(Container, rend)									\

#define EnumerateConstReverseIterators(Container, Enumerate)	\
	EnumerateNonConstReverseIterators(Container, Enumerate)		\
	Enumerate(Container, crbegin)								\
	Enumerate(Container, crend)

#define ExposeConstIterators(Container, Fn)	\
decltype(auto) Fn() const& noexcept { return std::##Fn(Container); };

#define ExposeNonConstIterators(Container, Fn)	\
decltype(auto) Fn() & noexcept { return std::##Fn(Container); }

// Exposes standard container iterators.
#define ExposeIterators(Container)							\
EnumerateConstIterators(Container, ExposeConstIterators)	\
EnumerateNonConstIterators(Container, ExposeNonConstIterators)

// Exposes standard container reverse iterators.
#define ExposeReverseIterators(Container)						\
EnumerateConstReverseIterators(Container, ExposeConstIterators)	\
EnumerateNonConstReverseIterators(Container, ExposeNonConstIterators)

// Chain-exposes underlying container's iterators.
#define DeclareIterators(Container)							\
EnumerateConstIterators(Container, ExposeConstIterators)	\
EnumerateNonConstIterators(Container, ExposeNonConstIterators)

// Chain-exposes underlying container's reverse iterators.
#define DeclareReverseIterators(Container)						\
EnumerateConstReverseIterators(Container, ExposeConstIterators)	\
EnumerateNonConstReverseIterators(Container, ExposeNonConstIterators)
