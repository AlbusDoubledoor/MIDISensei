#include <fstream>
#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <list>
#include <string>
#include "macro.h"

struct MidiEvent
{
	enum class Type
	{
		NoteOff,
		NoteOn,
		Other
	} event;
	uint8_t nStatus = 0;
	uint8_t nKey = 0;
	uint8_t nVelocity = 0;
	uint32_t nDeltaTick = 0;
};

struct MidiTrack
{
	std::string sName;
	std::string sInstrument;
	std::vector<MidiEvent> vecEvents;
};


class MidiFile
{
public:
	enum EventName : uint8_t
	{
		VoiceNoteOff = 0x80,
		VoiceNoteOn = 0x90,
		VoiceAftertouch = 0xA0,
		VoiceControlChange = 0xB0,
		VoiceProgramChange = 0xC0,
		VoiceChannelPressure = 0xD0,
		VoicePitchBend = 0xE0,
		SystemExclusive = 0xF0,
	};

	enum MetaEventName : uint8_t
	{
		MetaSequence = 0x00,
		MetaText = 0x01,
		MetaCopyright = 0x02,
		MetaTrackName = 0x03,
		MetaInstrumentName = 0x04,
		MetaLyrics = 0x05,
		MetaMarker = 0x06,
		MetaCuePoint = 0x07,
		MetaChannelPrefix = 0x20,
		MetaEndOfTrack = 0x2F,
		MetaSetTempo = 0x51,
		MetaSMPTEOffset = 0x54,
		MetaTimeSignature = 0x58,
		MetaKeySignature = 0x59,
		MetaSequencerSpecific = 0x7F,
	};

public:
	MidiFile()
	{
	}

	MidiFile(const std::wstring& sFileName)
	{
		ParseFile(sFileName);
	}

	bool ParseFile(const std::wstring& sFileName)
	{
		std::ifstream ifs;
		std::ofstream log;
		ifs.open(sFileName, std::fstream::in | std::ios::binary);
		if (!ifs.is_open())
			return false;
		log.open(REPORT_FILE_NAME, std::fstream::out);
		// Базовый каст wstring в string (иероглифы не сработают)
		std::string fileName;
		for (uint16_t idx = 0; idx < sFileName.length(); ++idx) {
			fileName += (char)sFileName[idx];
		}
		log << "Filename: " << fileName << std::endl;
		// Функции смены нужны для того, чтобы выставить биты в нужном порядке, 
		// так как в .MID файлах используется Big-Endian (сначала старший байт, потом младший)
		// Смена порядка следования битов для 32-битного числа
		auto Swap32 = [](uint32_t n)
		{
			return (((n >> 24) & 0xff) | ((n << 8) & 0xff0000) | ((n >> 8) & 0xff00) | ((n << 24) & 0xff000000));
		};

		// Смена порядка следования битов для 16-битного числа
		auto Swap16 = [](uint16_t n)
		{
			return ((n >> 8) | (n << 8));
		};

		// Чтение строки из файлового потока [побайтово]
		auto ReadString = [&ifs](uint32_t nLength)
		{
			std::string s;
			for (uint32_t i = 0; i < nLength; i++) s += ifs.get();
			return s;
		};

		// Чтение так называемого Variable Length-объекта
		// Используется для записи delta-time информации, длин треков, длин sysex, etc..
		// Может быть до 4 байтов (max)
		// Если старший бит сброшен, то это последний байт в VL-объекте
		auto ReadValue = [&ifs]()
		{
			uint32_t nValue = 0;
			uint8_t nByte = 0;
			nValue = ifs.get();

			// Проверяем старший бит (если установлен, то нужно считывать ещё один байт)
			if (nValue & 0x80)
			{
				nValue &= 0x7F;
				do
				{
					nByte = ifs.get();
					nValue = (nValue << 7) | (nByte & 0x7F);
				} while (nByte & 0x80); // Считываем, пока старший бит установлен
			}
			return nValue;
		};

		uint32_t n32 = 0;
		uint16_t n16 = 0;

		// Считываем MIDI заголовок
		ifs.read((char*)&n32, sizeof(uint32_t));
		uint32_t nFileID = Swap32(n32);
		ifs.read((char*)&n32, sizeof(uint32_t));
		uint32_t nHeaderLength = Swap32(n32);
		ifs.read((char*)&n16, sizeof(uint16_t));
		m_nFormat = Swap16(n16);
		log << "MIDI Format: " << std::to_string(m_nFormat) << std::endl;
		ifs.read((char*)&n16, sizeof(uint16_t));
		uint16_t nTrackChunks = Swap16(n16);
		ifs.read((char*)&n16, sizeof(uint16_t));
		m_nDivision = Swap16(n16);

		// Считываем MIDI треки
		for (uint16_t nChunk = 0; nChunk < nTrackChunks; nChunk++)
		{
			log << "===== NEW TRACK" << std::endl;
			// Заголовок MIDI трека
			ifs.read((char*)&n32, sizeof(uint32_t));
			uint32_t nTrackID = Swap32(n32);
			ifs.read((char*)&n32, sizeof(uint32_t));
			uint32_t nTrackLength = Swap32(n32);

			bool bEndOfTrack = false;
			vecTracks.push_back(MidiTrack());
			uint8_t nPreviousStatus = 0;

			while (!ifs.eof() && !bEndOfTrack)
			{
				// Все MIDI события содержат время (которое нужно подождать) и status-байт
				uint32_t nStatusTimeDelta = 0;
				uint8_t nStatus = 0;

				nStatusTimeDelta = ReadValue();
				nStatus = ifs.get();

				// Все MIDI статусные события имеют установленный старший бит
				// Для данных внутри стандартного события он сброшен
				// Это позволяет избежать избыточной отправки статусных байтов 
				// для событий, которые относятся к одному MIDI-статусу
				// (такой режим называется Running status)
				if (nStatus < 0x80)
				{
					nStatus = nPreviousStatus;
					// Возвращаем указатель потока обратно, иначе произойдет
					// десинхронизация всего потока (так как мы уже считали байт для проверки)
					ifs.seekg(-1, std::ios_base::cur);
				}

				// Событие отжатия ноты
				if ((nStatus & 0xF0) == EventName::VoiceNoteOff)
				{
					nPreviousStatus = nStatus;
					uint8_t nNoteID = ifs.get();	// номер ноты (id)
					uint8_t nNoteVelocity = ifs.get(); // скорость отжатия
					vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::NoteOff, nStatus, nNoteID, nNoteVelocity, nStatusTimeDelta });
				}
				// Нажатие ноты
				else if ((nStatus & 0xF0) == EventName::VoiceNoteOn)
				{
					nPreviousStatus = nStatus;
					uint8_t nNoteID = ifs.get();
					uint8_t nNoteVelocity = ifs.get();
					// Если скорость нажатия = 0, то это эквивалентно отжатию ноты
					if (nNoteVelocity == 0)
						vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::NoteOff, nStatus, nNoteID, nNoteVelocity, nStatusTimeDelta });
					else
						vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::NoteOn, nStatus, nNoteID, nNoteVelocity, nStatusTimeDelta });
				}
				// так называемый звук Aftertouch ("послекасание")
				else if ((nStatus & 0xF0) == EventName::VoiceAftertouch)
				{
					nPreviousStatus = nStatus;
					uint8_t nNoteID = ifs.get();
					uint8_t nNoteVelocity = ifs.get();
					vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::Other, nStatus, nNoteID, nNoteVelocity, nStatusTimeDelta });
				}
				// Смена устройства
				else if ((nStatus & 0xF0) == EventName::VoiceControlChange)
				{
					nPreviousStatus = nStatus;
					uint8_t nControlID = ifs.get();
					uint8_t nControlValue = ifs.get();
					vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::Other, nStatus, nControlID, nControlValue, nStatusTimeDelta });
				}
				// Смена программы (патч, тембр)
				else if ((nStatus & 0xF0) == EventName::VoiceProgramChange)
				{
					nPreviousStatus = nStatus;
					uint8_t nProgramID = ifs.get();
					vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::Other, nStatus,nProgramID,0,nStatusTimeDelta });
				}
				// Параметр силы нажатия нот в канале
				else if ((nStatus & 0xF0) == EventName::VoiceChannelPressure)
				{
					nPreviousStatus = nStatus;
					uint8_t nChannelPressure = ifs.get();
					vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::Other,nStatus,nChannelPressure,0,nStatusTimeDelta });
				}
				// Параметры высоты звука в канале (увеличение/уменьшение базовой частоты нот), он же Pitch Wheel
				else if ((nStatus & 0xF0) == EventName::VoicePitchBend)
				{
					nPreviousStatus = nStatus;
					uint8_t nLS7B = ifs.get();
					uint8_t nMS7B = ifs.get();
					vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::Other,nStatus,nLS7B,nMS7B,nStatusTimeDelta });

				}
				// Meta- события (и SysEx)
				else if ((nStatus & 0xF0) == EventName::SystemExclusive)
				{
					nPreviousStatus = 0;

					if (nStatus == 0xFF)
					{
						// Meta сообщение
						uint8_t nType = ifs.get();
						uint8_t nLength = ReadValue();
						switch (nType)
						{
						case MetaSequence:
							log << "Sequence Number: " << ifs.get() << ifs.get() << std::endl;
							break;
						case MetaText:
							log << "Text: " << ReadString(nLength) << std::endl;
							break;
						case MetaCopyright:
							log << "Copyright: " << ReadString(nLength) << std::endl;
							break;
						case MetaTrackName:
							vecTracks[nChunk].sName = ReadString(nLength);
							log << "Track Name: " << vecTracks[nChunk].sName << std::endl;
							break;
						case MetaInstrumentName:
							vecTracks[nChunk].sInstrument = ReadString(nLength);
							log << "Instrument Name: " << vecTracks[nChunk].sInstrument << std::endl;
							break;
						case MetaLyrics:
							log << "Lyrics: " << ReadString(nLength) << std::endl;
							break;
						case MetaMarker:
							log << "Marker: " << ReadString(nLength) << std::endl;
							break;
						case MetaCuePoint:
							log << "Cue: " << ReadString(nLength) << std::endl;
							break;
						case MetaChannelPrefix:
							log << "Prefix: " << ifs.get() << std::endl;
							break;
						case MetaEndOfTrack:
							bEndOfTrack = true;
							break;
						case MetaSetTempo:
							// Темп в микросекундах на четверть
							if (m_nTempo == 0)
							{
								(m_nTempo |= (ifs.get() << 16));
								(m_nTempo |= (ifs.get() << 8));
								(m_nTempo |= (ifs.get() << 0));
								m_nBPM = (60000000 / m_nTempo);
								log << "Tempo: " << m_nTempo << " (" << m_nBPM << "bpm)" << std::endl;
							}
							break;
						case MetaSMPTEOffset:
							log << "SMPTE: H:" << ifs.get() << " M:" << ifs.get() << " S:" << ifs.get() << " FR:" << ifs.get() << " FF:" << ifs.get() << std::endl;
							break;
						case MetaTimeSignature:
							log << "Time Signature: " << ifs.get() << "/" << (2 << ifs.get()) << std::endl;
							log << "ClocksPerTick: " << ifs.get() << std::endl;

							// MIDI бит - 24 тика, сколько 32-ых будет в одном бите
							log << "32per24Clocks: " << ifs.get() << std::endl;
							break;
						case MetaKeySignature:
							log << "Key Signature: " << ifs.get() << std::endl;
							log << "Minor Key: " << ifs.get() << std::endl;
							break;
						case MetaSequencerSpecific:
							log << "Sequencer Specific: " << ReadString(nLength) << std::endl;
							break;
						default:
							log << "Unrecognised MetaEvent: " << nType << std::endl;
						}
					}

					if (nStatus == 0xF0)
					{
						// Начало SysEx
						log << "System Exclusive Begin: " << ReadString(ReadValue()) << std::endl;
					}

					if (nStatus == 0xF7)
					{
						// Конец SysEx
						log << "System Exclusive End: " << ReadString(ReadValue()) << std::endl;
					}
				}
				else
				{
					log << "Unrecognised Status Byte: " << nStatus << std::endl;
				}
			}
		}
		ifs.close();
		log.close();
		return true;
	}

public:
	std::vector<MidiTrack> vecTracks;
	uint32_t m_nTempo = 0;
	uint32_t m_nBPM = 0;
	uint16_t m_nDivision = 0;
	uint16_t m_nFormat = 0;
};
