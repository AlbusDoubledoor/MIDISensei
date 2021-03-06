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
#define DEFAULT_NUMERATOR 4
#define DEFAULT_DENOMINATOR 4
// MIDI Format
#define MIDI_FILE_HEADER "MThd"
#define MIDI_TRACK_HEADER "MTrk"
#define MIDI_FORMAT_SINGLE 0
#define MIDI_FORMAT_SYNC_MULTI 1
#define MIDI_FORMAT_ASYNC 2
// Errors & Info
#define ERR_MSG_LOAD___OVER_PLAYBACK L"�� ������� ��������� ����, ��� ��� ��������������� ��� � ��������\n������� ������ ����������"
#define ERR_MSG_LOAD___OTHER L"�� ������� ��������� ����\n��������� ������������� ����� � ������������ �������" 
#define ERR_MSG_MIDI___DEVICE L"���������� ������� ���������� ������"
#define ERR_MSG_PLAY L"���������� ��������� ����"
#define ERR_MSG_PLAY___LOAD_ERROR L"���������� ��������� ����\n������ �������� �����"
#define ERR_MSG_PLAY___OVER_PLAYBACK L"���������� ��������� ����\n���� ��� �������������"
#define ERR_MSG_PLAY___LOAD_NOT_STAGED L"���������� ��������� ����\n���� �� ��������"
#define ERR_MSG_REPORT L"�� ������� ������� ����� �� �����"
#define ERR_MSG_DELETELAST L"������ ������� ������������ ����"
#define ERR_MSG_BYTEINPUT L"������������ �������� ������ ���������� � �������� 0..127"
#define ERR_MSG_TICKS_NULL L"���� '����� �� ��������' �� ����� ���� ������"
#define ERR_MSG_TEMPO_NULL L"���� 'BPM' �� ����� ���� ������"
#define INFO_MSG_DRUMS L"�������� �������� �� ������� 2-4"
#define INFO_MSG_REPORT L"���� ������ �����������\n��������� ����"
#define INFO_MSG_FILE_LOADED L"���� ��������"
#define INFO_MSG_FILE_CREATED L"���� ������"
// Titles
#define MB_TITLE___ERROR L"������"
#define MB_TITLE___INFO L"����������"
// Labels
#define BTN_LABEL___MKEYB L"MIDISensei ����������"
#define BTN_LABEL___PLAYFILE L"������ ����"
#define BTN_LABEL___RECORDFILE L"�������� ����"
// Frequent transformations
#define wstr(target) std::to_wstring(target) 
#define str(target) std::to_string(target) 
// Shell commands
#define SHELL_COMMAND_OPEN "open"
// Positionate winAPI
#define ScreenX GetSystemMetrics(SM_CXSCREEN)
#define ScreenY GetSystemMetrics(SM_CYSCREEN)
#define WND_WIDTH 610
#define WND_HEIGHT 125
#define CenterX (ScreenX-WND_WIDTH)/2
#define CenterY (ScreenY-WND_HEIGHT)/2
