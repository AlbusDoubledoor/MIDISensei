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
// MIDI Format
#define MIDI_FILE_HEADER "MThd"
#define MIDI_TRACK_HEADER "MTrk"
#define MIDI_FORMAT_SINGLE 0
#define MIDI_FORMAT_SYNC_MULTI 1
#define MIDI_FORMAT_ASYNC 2
// Errors & Info
#define ERR_MSG_LOAD___OVER_PLAYBACK L"Не удалось загрузить файл, так как воспроизведение уже в процессе\nНажмите кнопку Прекратить"
#define ERR_MSG_LOAD___OTHER L"Не удалось загрузить файл\nПроверьте существование файла и корректность формата" 
#define ERR_MSG_MIDI___DEVICE L"Невозможно открыть устройство вывода"
#define ERR_MSG_PLAY L"Невозможно проиграть файл"
#define ERR_MSG_PLAY___LOAD_ERROR L"Невозможно проиграть файл\nОшибка загрузки файла"
#define ERR_MSG_PLAY___OVER_PLAYBACK L"Невозможно проиграть файл\nФайл уже проигрывается"
#define ERR_MSG_PLAY___LOAD_NOT_STAGED L"Невозможно проиграть файл\nФайл не загружен"
#define ERR_MSG_REPORT L"Не удалось открыть отчёт по файлу"
#define ERR_MSG_DELETELAST L"Нельзя удалить единственный трек"
#define ERR_MSG_BYTEINPUT L"Однобайтовые значения должны помещаться в диапазон 0..127"
#define ERR_MSG_TICKS_NULL L"Поле 'Тиков на четверть' не может быть пустым"
#define ERR_MSG_TEMPO_NULL L"Поле 'BPM' не может быть пустым"
#define INFO_MSG_DRUMS L"Барабаны работают на октавах 2-4"
#define INFO_MSG_REPORT L"Файл отчёта отсутствует\nЗагрузите файл"
#define INFO_MSG_FILE_LOADED L"Файл загружен"
// Titles
#define MB_TITLE___ERROR L"Ошибка"
#define MB_TITLE___INFO L"Информация"
// Labels
#define BTN_LABEL___MKEYB L"MIDISensei Клавиатура"
#define BTN_LABEL___PLAYFILE L"Играть файл"
#define BTN_LABEL___RECORDFILE L"Записать файл"
// Frequent transformations
#define wstr(target) std::to_wstring(target) 
#define str(target) std::to_string(target) 
// Shell commands
#define SHELL_COMMAND_OPEN "open"
