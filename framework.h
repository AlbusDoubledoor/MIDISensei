// header.h: включаемый файл для стандартных системных включаемых файлов
// или включаемые файлы для конкретного проекта
//

#pragma once

#include "targetver.h"
// Файлы заголовков Windows
#define NOMINMAX
#include <windows.h>
#include <commctrl.h>
// Файлы заголовков среды выполнения C
#include <stdlib.h>
#include <string>
#include <thread>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
// Вспомогательные решения
#include "midifile.cpp"
