#pragma once

// System macros
#define NOMINMAX
// Max and min macros
#define MAX_LOADSTRING 100
// Filenames
#define REPORT_FILE_NAME "report.log"
// Channels
#define DRUM_CHANNEL 9
// Default tempo params
#define DEFAULT_TEMPO 500000
#define DEFAULT_BPM 120
// MIDI Formats
#define MIDI_FORMAT_SINGLE 0
#define MIDI_FORMAT_SYNC_MULTI 1
#define MIDI_FORMAT_ASYNC 2
// Errors & Info
#define ERR_MSG_LOAD___OVER_PLAYBACK L"Не удалось загрузить файл, так как воспроизведение уже в процессе.\nНажмите кнопку Сброс."
#define ERR_MSG_LOAD___OTHER L"Не удалось загрузить файл.\nПроверьте существование файла и корректность формата." 
#define ERR_MSG_MIDI___DEVICE L"Невозможно открыть устройство вывода"
#define INFO_MSG_DRUMS L"Барабаны работают на октавах 2-4"
// Titles
#define MB_TITLE___ERROR L"Ошибка"
#define MB_TITLE___INFO L"Информация"
// Labels
#define BTN_LABEL___MKEYB L"MIDISensei Клавиатура"
#define BTN_LABEL___PLAYFILE L"Играть файл"
// Often transformations
#define wstr(target) std::to_wstring(target) 