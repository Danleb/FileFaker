#pragma once
#include <stdio.h>
#include <Windows.h>

extern FILE* (*fopen_Original)(char const*, char const*);

FILE* fopen_fake(char const* fileName, char const* mode);
