// header.h: включаемый файл для стандартных системных включаемых файлов
// или включаемые файлы для конкретного проекта
//

#pragma once
#pragma comment(lib,"winmm.lib")
#include "targetver.h"
// Файлы заголовков Windows
#include <windows.h>
#include <commctrl.h>
// Файлы заголовков среды выполнения C
#include <stdlib.h>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
// Вспомогательные решения
#include "midifile.cpp"
#include "macro.h"
