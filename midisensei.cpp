#include "midisensei.h"

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HMIDIOUT Out;									// Ключ устройства вывода
UINT Channel;									// Номер канала (10 - drums, октава ~3-4)
UINT Patch;										// Номер тембра
UINT Octave;									// Октава - 0-10
HWND DlgKeyb;									// Ключ диалогового окна MIDI клавиатуры
HWND DlgPlay;									// Ключ диалогового окна проигрывания файла
HWND DlgRec;									// Ключ диалогового окна записи файла
MidiFile PlayMidifile;							// midi-файл для проигрывания
MidiFile RecordMidifile;						// midi-файл для записи
UINT CurrentRecordTrack;						// Текущий записываемый трек
BOOL LoadStaged = FALSE;						// Статус загрузки файла (файл загружен)
BOOL PlaybackStaged = FALSE;					// Статус подготовки Playback (файл проигрывается)
BOOL LoadMidiError = FALSE;						// Статус загрузки midi-файла
BOOL KeyboardWindowOpened = FALSE;				// Статус - открыто немодальное диалоговое окно с клавиатурой


// Константы:
char KeyToNote[] = "zsxdcvgbhnjmq2w3er5t6y7u"; // Таблица клавиши -> ноты

const wchar_t* PatchNames[] = {
  L"Acoustic Grand Piano",
  L"Bright Acoustic Piano",
  L"Electric Grand Piano",
  L"Honky-tonk Piano",
  L"Electric Piano 1",
  L"Electric Piano 2",
  L"Harpsichord",
  L"Clavinet",
  L"Celesta",
  L"Glockenspiel",
  L"Music Box",
  L"Vibraphone",
  L"Marimba",
  L"Xylophone",
  L"Tubular Bells",
  L"Dulcimer",
  L"Drawbar Organ",
  L"Percussive Organ",
  L"Rock Organ",
  L"Church Organ",
  L"Reed Organ",
  L"Accordion",
  L"Harmonica",
  L"Tango Accordion",
  L"Acoustic Guitar (nylon)",
  L"Acoustic Guitar (steel)",
  L"Electric Guitar (jazz)",
  L"Electric Guitar (clean)",
  L"Electric Guitar (muted)",
  L"Overdriven Guitar",
  L"Distortion Guitar",
  L"Guitar Harmonics",
  L"Acoustic Bass",
  L"Electric Bass (finger)",
  L"Electric Bass (pick)",
  L"Fretless Bass",
  L"Slap Bass 1",
  L"Slap Bass 2",
  L"Synth Bass 1",
  L"Synth Bass 2",
  L"Violin",
  L"Viola",
  L"Cello",
  L"Contrabass",
  L"Tremolo Strings",
  L"Pizzicato Strings",
  L"Orchestral Harp",
  L"Timpani",
  L"String Ensemble 1",
  L"String Ensemble 2",
  L"Synth Strings 1",
  L"Synth Strings 2",
  L"Choir Aahs",
  L"Voice Oohs",
  L"Synth Voice",
  L"Orchestra Hit",
  L"Trumpet",
  L"Trombone",
  L"Tuba",
  L"Muted Trumpet",
  L"French Horn",
  L"Brass Section",
  L"Synth Brass 1",
  L"Synth Brass 2",
  L"Soprano Sax",
  L"Alto Sax",
  L"Tenor Sax",
  L"Baritone Sax",
  L"Oboe",
  L"English Horn",
  L"Bassoon",
  L"Clarinet",
  L"Piccolo",
  L"Flute",
  L"Recorder",
  L"Pan Flute",
  L"Bottle Blow",
  L"Shakuhachi",
  L"Whistle",
  L"Ocarina",
  L"Lead 1 (square)",
  L"Lead 2 (sawtooth)",
  L"Lead 3 (calliope)",
  L"Lead 4 (chiff)",
  L"Lead 5 (charang)",
  L"Lead 6 (voice)",
  L"Lead 7 (fifths)",
  L"Lead 8 (bass + lead)",
  L"Pad 1 (new age)",
  L"Pad 2 (warm)",
  L"Pad 3 (polysynth)",
  L"Pad 4 (choir)",
  L"Pad 5 (bowed)",
  L"Pad 6 (metallic)",
  L"Pad 7 (halo)",
  L"Pad 8 (sweep)",
  L"FX 1 (rain)",
  L"FX 2 (soundtrack)",
  L"FX 3 (crystal)",
  L"FX 4 (atmosphere)",
  L"FX 5 (brightness)",
  L"FX 6 (goblins)",
  L"FX 7 (echoes)",
  L"FX 8 (sci-fi)",
  L"Sitar",
  L"Banjo",
  L"Shamisen",
  L"Koto",
  L"Kalimba",
  L"Bagpipe",
  L"Fiddle",
  L"Shanai",
  L"Tinkle Bell",
  L"Agogo",
  L"Steel Drums",
  L"Woodblock",
  L"Taiko Drum",
  L"Melodic Tom",
  L"Synth Drum",
  L"Reverse Cymbal",
  L"Guitar Fret Noise",
  L"Breath Noise",
  L"Seashore",
  L"Bird Tweet",
  L"Telephone Ring",
  L"Helicopter",
  L"Applause",
  L"Gunshot",
};

std::map<BYTE,std::wstring> EventTypesMap = {
	{0x80,L"Note Off"},
	{0x90,L"Note On"},
	{0xA0,L"Aftertouch"},
	{0xB0,L"Control Change"},
	{0xC0,L"Program Change"},
	{0xD0,L"Channel Pressure"},
	{0xE0,L"Pitch Bend"}
};

const wchar_t* EventNames[] = {
	L"Note Off",
	L"Note On",
	L"Aftertouch",
	L"Control Change",
	L"Program Change",
	L"Channel Pressure",
	L"Pitch Bend"
};

BYTE EventTypes[] = {
	0x80,
	0x90,
	0xA0,
	0xB0,
	0xC0,
	0xD0,
	0xE0
};
// Функции:

// Выдача сообщения об ошибке
void Error(const wchar_t* Text) {
	MessageBox(NULL, Text, MB_TITLE___ERROR, MB_ICONERROR | MB_OK);
}

// Информационное сообщение
void InfoMessage(const wchar_t* Text) {
	MessageBox(NULL, Text,MB_TITLE___INFO, MB_ICONINFORMATION | MB_OK);
}

// Добавление строки в ComboBox диалога
void AddStringToCB(HWND hDlg,UINT Ctl, const wchar_t* Str) {
	SendDlgItemMessage(hDlg,Ctl, CB_ADDSTRING, 0, (LPARAM)Str);
}

// Удаление строки из ComboBox диалога
void RemoveStringFromCB(HWND hDlg, UINT Ctl, UINT position) {
	SendDlgItemMessage(hDlg, Ctl, CB_DELETESTRING, position, 0);
}

// Посылка сообщения на открытое устройство
void MidiOut(DWORD Msg) {
	if (Out) {
		midiOutShortMsg(Out, Msg);
	}
}

// Посылка канального сообщения
void MidiOutChannel(BYTE status, BYTE data, BYTE velocity) {
	MidiOut((((velocity << 8) | data) << 8) | (status | BYTE(Channel)));
}

// Посылка сообщения о смене тембра
void SendPatchChange(void) {
	MidiOutChannel(0xC0, BYTE(Patch), 0); // Сообщение структуры Cn pp
}

// Посылка сообщения "All Notes Off"
void AllNotesOff(void) {
	MidiOutChannel(0xB0, 123, 0);   // Сообщение структуры Bn 7B
}

// Отработка смены тембра
void PatchChange(void) {
	if (!Out) {
		return;
	}
	int i;
	PATCHARRAY Patches;           // Массив включенных тембров
	for (i = 0; i < MIDIPATCHSIZE; ++i) {
		Patches[i] = 0;            // Обнуляем массив тембров
	}
	Patches[Patch] = 0xFFFF;     // Включим тембр для всех каналов
	midiOutCachePatches(Out, 0, Patches, MIDI_CACHE_ALL); // Загрузим тембры
	SendPatchChange();           // Пошлем сообщение о смене тембра
}

// Закрывание устройств
void CloseDevices(void) {
	if (Out) {
		midiOutReset(Out);         // Сбросим активность
		AllNotesOff();             // Пошлем сообщение сброса нот
		Sleep(10);                 // Подождем, пока уйдет
		midiOutClose(Out);         // Закроем устройство
		Out = NULL;
	}
}

// Открывание устройств
int OpenDevices(HWND hDlg,int controlId) {
	MMRESULT Res;
	Out = NULL;
	// Получаем номер выбранного устройства в списке
	int  DevN = SendDlgItemMessage(hDlg,controlId, CB_GETCURSEL, 0, 0);

	Res = midiOutOpen(&Out, DevN - 1, 0, 0, 0);

	if (Res != MMSYSERR_NOERROR) {
		Error(ERR_MSG_MIDI___DEVICE);
		return FALSE;
	}
	
	if (controlId == IDC_MIDIOUT) {
		PatchChange(); // Выберем текущий тембр
	}
	return TRUE;
}

// Инициализация комбобокса с устройствами
void InitMidiDevices(HWND hDlg,int controlId) {
	int NumDevs;
	int i;
	// Заполняем списки устройств ввода, начиная с MIDI Mapper
	NumDevs = midiOutGetNumDevs();
	for (i = 0; i < NumDevs + 1; i++) {
		MIDIOUTCAPS DevCaps;
		midiOutGetDevCaps(i - 1, &DevCaps, sizeof(DevCaps));
		SendDlgItemMessage(hDlg,controlId, CB_ADDSTRING, 0, (LPARAM)DevCaps.szPname);
	}
}

// Обновление списка событий
void RefreshTrackEvents(HWND hDlg) {
	SendDlgItemMessage(hDlg, IDC_TRACKEVENTS_CB, CB_RESETCONTENT, 0, 0);
	int trkEventsNum = RecordMidifile.vecTracks[CurrentRecordTrack].vecEvents.size();
	std::wstring trackEvents;
	for (int i = 0; i < trkEventsNum; ++i) {
		MidiEvent nEvent = RecordMidifile.vecTracks[CurrentRecordTrack].vecEvents[i];
		uint8_t nStatus = nEvent.nStatus;
		trackEvents.clear();
		trackEvents = EventTypesMap[(nStatus & 0xF0)];
		trackEvents += L" | Канал: " + std::to_wstring((nStatus & 0x0F) + 1);
		trackEvents += L" | Задержка: " + std::to_wstring(nEvent.nDelayTime) + L" ms";
		trackEvents += L" | Данные: " + std::to_wstring(nEvent.nDataByteFirst);
		trackEvents += L" | " + std::to_wstring(nEvent.nDataByteSecond);
		AddStringToCB(hDlg, IDC_TRACKEVENTS_CB, trackEvents.c_str());
	}
}

std::atomic<bool> playbackCancel;	// флаг завершения проигрывания
std::atomic<int> activeTracks;		// количество активных треков
std::mutex awakeMutex;				// mutex для слипов в потоках
std::condition_variable sleepLock;	// слип в потоках
// Класс функции проигрывания трека (для использования потоками)
class PlaybackTrack {
public:
	void operator()(const MidiTrack& midiTrk, const uint32_t tempo, const uint32_t division, bool tickTransformation) {
		long int eventsNum = midiTrk.vecEvents.size();
		for (long int nEvent = 0; nEvent < eventsNum; ++nEvent)
		{
			uint32_t sleepTime = midiTrk.vecEvents[nEvent].nDelayTime;
			if (sleepTime != 0 && tickTransformation) {
				sleepTime = tempo / (uint32_t)1000 * sleepTime / division;
			}
			std::unique_lock<std::mutex> lock(awakeMutex);
			sleepLock.wait_for(lock,
				std::chrono::milliseconds(sleepTime),
				[]() { return playbackCancel.load() ? CloseDevices(), true : false; });
			BYTE nStatus = midiTrk.vecEvents[nEvent].nStatus;
			BYTE nKey = midiTrk.vecEvents[nEvent].nDataByteFirst;
			BYTE nVelocity = midiTrk.vecEvents[nEvent].nDataByteSecond;
			midiOutShortMsg(Out, (((nVelocity << 8) | nKey) << 8) | nStatus);
		}
		activeTracks--;
		if (activeTracks.load() == 0) {
			CloseDevices();
			playbackCancel.store(true);
			PlaybackStaged = FALSE;
		}
	}
};

// Проиграть файл
BOOL PlaybackFile() {
	if (PlaybackStaged || LoadMidiError || !LoadStaged) {
		if (PlaybackStaged) {
			Error(ERR_MSG_PLAY___OVER_PLAYBACK);
		}
		else if (LoadMidiError) {
			Error(ERR_MSG_PLAY___LOAD_ERROR);
		}
		else if (!LoadStaged) {
			Error(ERR_MSG_PLAY___LOAD_NOT_STAGED);
		}
		else{
			Error(ERR_MSG_PLAY);
		}
		return FALSE;
	}

	PlaybackStaged = TRUE;
	CloseDevices();
	BOOL successOpen = OpenDevices(DlgPlay,IDC_MIDIOUT_PLAY);
	if (!successOpen) {
		return PlaybackStaged = FALSE;
	}

	if (PlayMidifile.m_nTempo == 0)
		PlayMidifile.m_nTempo = DEFAULT_TEMPO;
	if (PlayMidifile.m_nBPM == 0)
		PlayMidifile.m_nBPM = DEFAULT_BPM;

	std::vector<std::thread> threads;
	std::vector<MidiTrack>::iterator track_it;
	playbackCancel.store(false);
	activeTracks.store(0);
	for (track_it = PlayMidifile.vecTracks.begin(); track_it != PlayMidifile.vecTracks.end(); ++track_it) {
		activeTracks++;
		PlaybackTrack playbackTrackFunction;
		threads.push_back(std::thread(playbackTrackFunction, (*track_it),PlayMidifile.m_nTempo,PlayMidifile.m_nDivision,true));
	}
	std::vector<std::thread>::iterator thread_it;
	for (thread_it = threads.begin(); thread_it != threads.end(); ++thread_it) {
		(*thread_it).detach();
	}
	return TRUE;
}

// Загрузка файла
BOOL LoadFile(const wchar_t* filename) {
	if (PlaybackStaged) {
		return FALSE;
	}
	LoadStaged = FALSE;
	std::wstring wFilename = std::wstring(filename);
	PlayMidifile = MidiFile();
	LoadMidiError = FALSE;
	if (LoadMidiError = !PlayMidifile.ParseFile(wFilename))
	{
		return FALSE;
	}
	LoadStaged = TRUE;
	return TRUE;
}

// Прекращение воспроизведения файла
void PlaybackCancel() {
	{
		std::lock_guard<std::mutex> lock(awakeMutex);
		playbackCancel.store(true);
		sleepLock.notify_all();
	}
	activeTracks.store(0);
	PlaybackStaged = FALSE;
}


// Получение значения числового поля
long int GetEditFieldInt(HWND hDlg,int controlId) {
	HWND fieldHandle = GetDlgItem(hDlg, controlId);
	int fieldLen = GetWindowTextLength(fieldHandle)+1;
	if (fieldLen == 1) {
		return 0;
	}
	wchar_t* fieldValue = new wchar_t[fieldLen];
	GetWindowText(fieldHandle, fieldValue, fieldLen);
	fieldValue[fieldLen - 1] = 0;
	long int fieldValueIntS = std::stoi(fieldValue);
	delete[] fieldValue;
	return fieldValueIntS;
}

// Проверка на байт при вводе числа
BOOL IsByteInput(long int input) {
	return input >= 0 && input <= 127;
}

ATOM                MidiSenseiRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DefaultDlgHandler(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK    MidiKeyboard(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK    PlayFileHandler(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK    RecordFileHandler(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Инициализация глобальных строк
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MIDISENSEI, szWindowClass, MAX_LOADSTRING);
	MidiSenseiRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MIDISENSEI));

	MSG msg;

	// Цикл основного сообщения:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		UINT Msg = msg.message;
		UINT wParam = msg.wParam;
		UINT lParam = msg.lParam;
		if (KeyboardWindowOpened&&(Msg == WM_KEYUP || Msg == WM_KEYDOWN))
		{
			/*2 3   5 6 7 - на октаву выше
				Q W E R T Y U

				S D   G H J - текущая октава
				Z X C V B N M
				Всего 11 возможных октав на MIDI (0-10, 128 нот)
				10-я октава содержит только 8 нот*/
			// Обработка сообщений о нажатии/отпускании клавиш
			int VK = wParam;              // Код клавиши
			BOOL repeatFlag = (lParam & 0x40000000); // Признак повторения
			// Ищем клавишу в таблице соответствия
			char* p = strchr(KeyToNote, tolower(VK));
			if (p) {
				if (!repeatFlag) {             // Повторение пропускаем
					// MIDI-сообщения по событию ноты:
					// 9c nn vv - нажать
					// 8c nn vv - отпустить
					// c - номер канала
					// nn - номер ноты
					// vv - скорость нажатия (0 - отпускание)
					if (Msg == WM_KEYDOWN)
						MidiOutChannel(0x90, BYTE(Octave * 12 + (UINT)p), BYTE(127));
				}
				if (Msg == WM_KEYUP)
					MidiOutChannel(0x80, BYTE(Octave * 12 + (UINT)p), BYTE(0));
				continue;
			}
		}
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

ATOM MidiSenseiRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MIDISENSEI));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MIDISENSEI);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной
	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd)
	{
		return FALSE;
	}
	// Кнопка открытия клавиатуры/записи файла
	HWND hKeyboardButton = CreateWindowW(WC_BUTTONW, BTN_LABEL___MKEYB, 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 
		10, 10, 200, 50, hWnd, (HMENU)IDM_OPENKEYBOARD, 
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
	// Кнопка открытия проигрывателя
	HWND hPlayFileButton = CreateWindowW(WC_BUTTONW, BTN_LABEL___PLAYFILE,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		220, 10, 175, 50, hWnd, (HMENU)IDM_PLAYFILE,
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
	// Кнопка открытия проигрывателя
	HWND hRecFileButton = CreateWindowW(WC_BUTTONW, BTN_LABEL___RECORDFILE,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		405, 10, 175, 50, hWnd, (HMENU)IDM_RECORDFILE,
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, DefaultDlgHandler);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_OPENKEYBOARD:
			if (KeyboardWindowOpened)
			{
				break; // Если открыто немодальное окно с клавиатурой, то не реагируем
			}
			// Делаем немодальное окно для обработки клавиш в фоновом режиме в основном окне
			DlgKeyb = CreateDialog(hInst, MAKEINTRESOURCE(IDD_KEYBOARDDIALOG), hWnd, MidiKeyboard);
			ShowWindow(DlgKeyb, SW_SHOW);
			KeyboardWindowOpened = TRUE;
			break;
		case IDM_PLAYFILE:
			if (KeyboardWindowOpened)
			{
				break; 
			}
			DialogBox(hInst, MAKEINTRESOURCE(IDD_PLAYFILEDIALOG), hWnd, PlayFileHandler);
			break;
		case IDM_RECORDFILE:
			if (KeyboardWindowOpened)
			{
				break;
			}
			DialogBox(hInst, MAKEINTRESOURCE(IDD_RECORDFILEDIALOG), hWnd, RecordFileHandler);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Ввести код отрисовок
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "Запись файла"
BOOL CALLBACK RecordFileHandler(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	DlgRec = hDlg;
	UINT controlId = LOWORD(wParam);
	switch (message) {
		case WM_INITDIALOG:
		{
			// Инициализация устройств
			InitMidiDevices(hDlg, IDC_MIDIOUT_RECORD);
			// Инициализация файла
		    RecordMidifile = MidiFile();
			// Заполняем список каналов
			for (int i = 0; i < 16; ++i) {
				AddStringToCB(hDlg, IDC_CHANNELS_CB, wstr(i + 1).c_str());
			}
			SendDlgItemMessage(hDlg, IDC_CHANNELS_CB, CB_SETCURSEL, 0, 0);
			// Добавляем трек в список треков
			AddStringToCB(hDlg, IDC_TRACKS_CB, wstr(1).c_str());
			SendDlgItemMessage(hDlg, IDC_TRACKS_CB, CB_SETCURSEL, 0, 1);
			RecordMidifile.vecTracks.push_back(MidiTrack());
			// Добавляем типы событий
			for (int i = 0; i < 7; ++i) {
				AddStringToCB(hDlg, IDC_EVENTS_CB, EventNames[i]);
			}
			SendDlgItemMessage(hDlg, IDC_EVENTS_CB, CB_SETCURSEL, 0, 0);
			break;
		}
		case WM_COMMAND:
			switch (controlId) {
				case IDOK:
				case IDCANCEL:
					{
						EndDialog(hDlg, LOWORD(lParam));
						return TRUE;
					}
					break;
				case IDC_ADDTRACK_BTN:
				{
					RecordMidifile.vecTracks.push_back(MidiTrack());
					AddStringToCB(hDlg, IDC_TRACKS_CB, wstr(RecordMidifile.vecTracks.size()).c_str());
					break;
				}
				case IDC_DELTRACK_BTN:
				{
					if (RecordMidifile.vecTracks.size() == 1) {
						Error(ERR_MSG_DELETELAST);
						break;
					}
					UINT trackToRemove = SendDlgItemMessage(hDlg, IDC_TRACKS_CB, CB_GETCURSEL, 0, 0);
					RecordMidifile.vecTracks.erase(RecordMidifile.vecTracks.begin() + trackToRemove);
					SendDlgItemMessage(hDlg, IDC_TRACKS_CB, CB_RESETCONTENT, 0, 0);
					// Обновляем список треков
					for (size_t i = 0; i < RecordMidifile.vecTracks.size(); ++i) {
						AddStringToCB(hDlg, IDC_TRACKS_CB, wstr(i + 1).c_str());
					}
					SendDlgItemMessage(hDlg, IDC_TRACKS_CB, CB_SETCURSEL, trackToRemove,0);
					break;
				}
				case IDC_TRACKS_CB: {
					if (HIWORD(wParam) != CBN_SELENDOK) {
						break;
					}
					CurrentRecordTrack = SendDlgItemMessage(hDlg, IDC_TRACKS_CB, CB_GETCURSEL, 0, 0);
					RefreshTrackEvents(hDlg);
					UINT countCombo = SendDlgItemMessage(hDlg, IDC_TRACKEVENTS_CB, CB_GETCOUNT, 0, 0);
					SendDlgItemMessage(hDlg, IDC_TRACKEVENTS_CB, CB_SETCURSEL, countCombo-1, 0);
					break;
				}
				case IDC_MIDIOUT_RECORD:
				{
					if (HIWORD(wParam) != CBN_SELENDOK || !playbackCancel.load()) {
						break;
					}
					CloseDevices();
					OpenDevices(hDlg, IDC_MIDIOUT_RECORD);
					break;
				}
				case IDC_INSERTEVENT_BTN: {
					UINT nChannel = SendDlgItemMessage(hDlg, IDC_CHANNELS_CB, CB_GETCURSEL, 0, 0);
					UINT nEventType = SendDlgItemMessage(hDlg, IDC_EVENTS_CB, CB_GETCURSEL, 0, 0);
					uint8_t nStatus = EventTypes[nEventType]|nChannel;
					long int tmpFieldValue = 0;
					tmpFieldValue = GetEditFieldInt(hDlg, IDC_EDIT_DATABYTE_FIRST);
					if (!IsByteInput(tmpFieldValue)) {
						Error(ERR_MSG_BYTEINPUT);
						break;
					};
					uint8_t nDataByteFirst = tmpFieldValue & 0xff;

					tmpFieldValue = GetEditFieldInt(hDlg, IDC_EDIT_DATABYTE_SECOND);
					if (!IsByteInput(tmpFieldValue)) {
						Error(ERR_MSG_BYTEINPUT);
						break;
					};
					uint8_t nDataByteSecond = tmpFieldValue & 0xff;

					uint32_t nDelayTime = GetEditFieldInt(hDlg,IDC_EDIT_DELAYTIME);
					UINT posInsert = 0;
					if (RecordMidifile.vecTracks[CurrentRecordTrack].vecEvents.size() == 0) {
						RecordMidifile.vecTracks[CurrentRecordTrack].vecEvents.push_back({ MidiEvent::Type::Other, nStatus, nDataByteFirst, nDataByteSecond, nDelayTime });
					}
					else {
						posInsert = SendDlgItemMessage(hDlg, IDC_TRACKEVENTS_CB, CB_GETCURSEL, 0, 0);
						if (posInsert == -1)
							posInsert = 0;
						++posInsert;
						RecordMidifile.vecTracks[CurrentRecordTrack].vecEvents.insert(RecordMidifile.vecTracks[CurrentRecordTrack].vecEvents.begin() + posInsert, { MidiEvent::Type::Other, nStatus, nDataByteFirst, nDataByteSecond, nDelayTime });
					}
					RefreshTrackEvents(hDlg);
					SendDlgItemMessage(hDlg, IDC_TRACKEVENTS_CB, CB_SETCURSEL, posInsert, 0);
					break;
				}
				case IDC_DELEVENT_BUTTON:
				{
					if (RecordMidifile.vecTracks[CurrentRecordTrack].vecEvents.size() == 0)
					{
						break;
					}
					UINT eventToRemove = SendDlgItemMessage(hDlg, IDC_TRACKEVENTS_CB, CB_GETCURSEL, 0, 0);
					RecordMidifile.vecTracks[CurrentRecordTrack].vecEvents.erase(RecordMidifile.vecTracks[CurrentRecordTrack].vecEvents.begin() + eventToRemove);
					RemoveStringFromCB(hDlg, IDC_TRACKEVENTS_CB, eventToRemove);
					SendDlgItemMessage(hDlg, IDC_TRACKEVENTS_CB, CB_SETCURSEL, eventToRemove, 0);
					break;
				}
				case IDC_CREATEFILE_BTN:
				{
					RecordMidifile.m_nDivision = (uint16_t)GetEditFieldInt(hDlg, IDC_EDIT_TICKSPERQUARTER);
					if (RecordMidifile.m_nDivision == 0) {
						Error(ERR_MSG_TICKS_NULL);
						break;
					}
					long int bpm = GetEditFieldInt(hDlg, IDC_EDIT_BPM);
					if (bpm == 0) {
						Error(ERR_MSG_TEMPO_NULL);
						break;
					}
					RecordMidifile.m_nTempo = (uint32_t)(60000000/bpm);
					RecordMidifile.m_nTimeSignature.nNumerator = (uint8_t)GetEditFieldInt(hDlg, IDC_EDIT_TIMESIGNATURE_ENUM);
					if (RecordMidifile.m_nTimeSignature.nNumerator == 0) {
						RecordMidifile.m_nTimeSignature.nNumerator = DEFAULT_NUMERATOR;
					}
					RecordMidifile.m_nTimeSignature.nDenominator = (uint8_t)(sqrt((uint8_t)GetEditFieldInt(hDlg, IDC_EDIT_TIMESIGNATURE_DENUM)));
					if (RecordMidifile.m_nTimeSignature.nDenominator == 0) {
						RecordMidifile.m_nTimeSignature.nDenominator = DEFAULT_DENOMINATOR;
					}
					HWND fnHandle = GetDlgItem(hDlg, IDC_RECORD_FILENAME);
					int fnLength = GetWindowTextLength(fnHandle) + 1;
					wchar_t* filename = new wchar_t[fnLength];
					GetWindowText(fnHandle, filename, fnLength);
					filename[fnLength - 1] = 0;
					if (!RecordMidifile.ExplodeFile(std::wstring(filename)))
					{
						Error(ERR_MSG_LOAD___OTHER);
					}
					delete[] filename;
					break;
				}
				case IDC_PLAYTRACK_BTN:
				{
					if (PlaybackStaged)
					{
						Error(ERR_MSG_PLAY___OVER_PLAYBACK);
						break;
					}
					CloseDevices();
					if (!OpenDevices(hDlg, IDC_MIDIOUT_RECORD)) {
						break;
					};
					PlaybackTrack playOneTrackFunction;
					PlaybackStaged = TRUE;
					playbackCancel.store(false);
					activeTracks.store(1);
					std::thread(playOneTrackFunction, RecordMidifile.vecTracks[CurrentRecordTrack],0, 0, false).detach();
					break;
				}
				case IDC_CANCELTRACK_BTN:
				{
					PlaybackCancel();
					break;
				}
			}
			break;
	}
	return FALSE;
}
// Обработчик сообщений для окна "Играть файл".
BOOL CALLBACK PlayFileHandler(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	DlgPlay = hDlg;
	UINT controlId = LOWORD(wParam);
	switch (message)
	{
	case WM_INITDIALOG: {
		// Инициализация списка MIDI-устройств
		InitMidiDevices(hDlg,IDC_MIDIOUT_PLAY);
		// Инициализация переменных состояния
		playbackCancel.store(false);
		PlaybackStaged = FALSE;
		LoadStaged = FALSE;
		return TRUE;
	}
	case WM_COMMAND:
		{
			switch (controlId) {
				case IDOK:
				case IDCANCEL:
					{
						PlaybackCancel();
						EndDialog(hDlg, LOWORD(wParam));
						return TRUE;
					}
					break;
				case IDC_LOADFILE_BUTTON:
					{
						HWND fnHandle = GetDlgItem(hDlg, IDC_FILENAMEINPUT);
						int fnLength= GetWindowTextLength(fnHandle) + 1;
						wchar_t* filename = new wchar_t[fnLength];
						GetWindowText(fnHandle, filename, fnLength);
						filename[fnLength-1] = 0;
						if (!LoadFile(filename)) {
							Error(PlaybackStaged?ERR_MSG_LOAD___OVER_PLAYBACK:ERR_MSG_LOAD___OTHER);
						}
						else {
							InfoMessage(INFO_MSG_FILE_LOADED);
						}
						delete[] filename;
						break;
					}
				case IDC_PLAYFILE_BUTTON:
				{
					PlaybackFile();
					break;
				}
				case IDC_PLAYBACKCANCEL_BUTTON:
					{
						PlaybackCancel();
						break;
					}
				case IDC_MIDIOUT_PLAY:
				{
					if (HIWORD(wParam) != CBN_SELENDOK || !playbackCancel.load()) {
						break;
					}
					CloseDevices();
					OpenDevices(hDlg,IDC_MIDIOUT_PLAY);
					break;
				}
				case IDC_VIEWREPORT_BUTTON:
					{
						std::ifstream checkReport;
						checkReport.open(REPORT_FILE_NAME, std::fstream::in);
						if (checkReport.is_open())
						{
							checkReport.close();
							ShellExecuteA(GetDesktopWindow(), SHELL_COMMAND_OPEN, REPORT_FILE_NAME, NULL, NULL, SW_SHOW);
						}
						else {
							InfoMessage(INFO_MSG_REPORT);
						}
						break;
					}
				break;
			}
		}

	}
	return FALSE;
}

// Стандартный обработчик для диалогового окна
INT_PTR CALLBACK DefaultDlgHandler(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}

	return (INT_PTR)FALSE;
}

// Обработчик сообщений для окна "MIDISensei Клавиатура".
BOOL CALLBACK MidiKeyboard(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	DlgKeyb = hDlg;
	UINT controlId = LOWORD(wParam);
	switch (message)
	{
		case WM_INITDIALOG: {
			// Инициализация списка MIDI-устройств
			InitMidiDevices(hDlg,IDC_MIDIOUT);
			int i;
			// Заполняем список каналов
			for (i = 0; i < 16; ++i) {
				AddStringToCB(hDlg,IDC_CHANLIST, wstr(i+1).c_str());
			}
			Channel = SendDlgItemMessage(hDlg,IDC_CHANLIST, CB_SETCURSEL, 0, 0);
			// Заполняем список тембров
			for (i = 0; i < 128; ++i) {
				AddStringToCB(hDlg,IDC_PATCHLIST, PatchNames[i]);
			}
			Patch = SendDlgItemMessage(hDlg,IDC_PATCHLIST, CB_SETCURSEL, 0, 0);
			// Заполняем список октав
			for (i = 0; i < 11; ++i) {
				AddStringToCB(hDlg,IDC_OCTLIST, wstr(i).c_str());
			}
			// В начале установим 6-ю октаву
			Octave = SendDlgItemMessage(hDlg,IDC_OCTLIST, CB_SETCURSEL, 6, 0); 
			return TRUE;
		}

		case WM_COMMAND:
		{
			switch (controlId) {
				case IDOK:
				case IDCANCEL:
				{
					CloseDevices();
					EndDialog(hDlg, LOWORD(wParam));
					KeyboardWindowOpened = FALSE;
					return TRUE;
				}
				case IDC_RESET:
				{
					CloseDevices();
					OpenDevices(hDlg,IDC_MIDIOUT);
					AllNotesOff();
					return TRUE;
				}
				case IDC_MIDIOUT:
				{
					if (HIWORD(wParam) != CBN_SELENDOK) {
						break;
					}
					CloseDevices();
					OpenDevices(hDlg,IDC_MIDIOUT);
					break;
				}
				case IDC_CHANLIST:
				{
					if (HIWORD(wParam) != CBN_SELENDOK) { 
						break;
					}
					Channel = SendDlgItemMessage(hDlg,controlId, CB_GETCURSEL, 0, 0);
					if (Channel == DRUM_CHANNEL && (Octave < 2 || Octave > 4))
						InfoMessage(INFO_MSG_DRUMS);
					SendMessage(DlgKeyb, WM_SETFOCUS, 0, 0);
					PatchChange();   
					break;
				}
				case IDC_PATCHLIST:
				{
					if (HIWORD(wParam) != CBN_SELCHANGE) {
						break;
					}
					Patch = SendDlgItemMessage(hDlg,controlId, CB_GETCURSEL, 0, 0);
					SendMessage(DlgKeyb, WM_SETFOCUS, 0, 0);
					PatchChange();    
					break;
				}
				case IDC_OCTLIST:
				{
					if (HIWORD(wParam) != CBN_SELCHANGE) {
						break;
					}
					Octave = SendDlgItemMessage(hDlg,controlId, CB_GETCURSEL, 0, 0);
					if (Channel == DRUM_CHANNEL && (Octave < 2 || Octave > 4))
						InfoMessage(INFO_MSG_DRUMS);
					SendMessage(DlgKeyb, WM_SETFOCUS, 0, 0);
					break;
				}
			}
		break;
		}
	}
	return FALSE;
}
