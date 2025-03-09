#pragma once

#ifdef _WIN32 //Little Endian
	#define fourccRIFF 'FFIR'
	#define fourccDATA 'atad'
	#define fourccFMT ' tmf'
	#define fourccWAVE 'EVAW'

#endif // define _WIN32

#include <fstream>

using std::fstream;

//To write
struct RIFF_CHUNK {
	long	chunkID; //BIG
	long	chunkSize;
	long	format; //BIG
};

struct FMT_SUBCHUNK {
	long	subchunk1ID;
	long	subchunk1Size;
	short	audioFormat;
	short	numChannels;
	long	sampleRate;
	long	byteRate;
	short	blockAlign;
	short	bitsPerSample;
};

struct DATA_SUBCHUNK {
	long	subchunk2ID; //BIG
	long	subchunk2Size;
};

struct WAVEHEADER {
	RIFF_CHUNK riff;
	FMT_SUBCHUNK fmt;
	DATA_SUBCHUNK data;
};


//To read
struct WAVE_FORMAT {
	unsigned short	audioFormat;
	unsigned short	numChannels;
	unsigned long	sampleRate;
	unsigned long	byteRate;
	unsigned short	blockAlign;
	unsigned short	bitsPerSample;
};


namespace BIT_DEPTH {
	enum E {
		B8,
		B16,
		B24,
		B32,
	};
};

struct FloatSample {
	FloatSample() = default;
	FloatSample(unsigned char data) {
		
	}

	float m_sample;
};

class AudioFile
{

public:
	AudioFile() = default;

	AudioFile(int sampleRate, int numChannels, BIT_DEPTH::E bitDepth, int duration) {
		
		m_sampleRate= sampleRate;
		m_numChannels = numChannels;
		m_duration = duration;
		
		switch (bitDepth)
		{
			case BIT_DEPTH::B8:
				m_bitDepth = 8;
				break;
			case BIT_DEPTH::B16:
				m_bitDepth = 16;
				break;
			case BIT_DEPTH::B24:
				m_bitDepth = 24;
				break;
			case BIT_DEPTH::B32:
				m_bitDepth = 32;
				break;
			default:
				break;
		}
		
	}
	
	~AudioFile() {
		if (m_samples) {
			delete[] m_samples;
			m_samples = nullptr;
		}
	
	}

	//Stores info about the audio file
	void create(int sampleRate, int numChannels, int bitDepth, int duration);

	//Reads a wav file
	void decode(const char* filename);

	
	//Creates a new wav file with audio info
	void encode(const char* filename);

	void encodeNew(const char* filename);

	//How many bytes are the per sample? 32 | 24 | 16 | 8
	inline int getBitDepth() const{
		return m_bitDepth;
	}

	//Get the sample rate of the audio file
	inline int getSampleRate() const {
		return m_sampleRate;
	}

	//Get the number of channels
	inline int getNumChannels() const {
		return m_numChannels;
	}

	//Duration of the sound in seconds
	float getDuration() const {
		return m_duration;
	}

	//Get samples = duration * sample rate
	int getNumSamples() const {
		return getDuration() * getSampleRate() * getNumChannels();
	}

	unsigned char* getSampleData() const {
		return m_samples;
	}


	/*
		- 44100 Hz
		- 24 bits = 3 bytes
		- 2 canales
		- 60 segundos

		44100 * 3 * 2 *  60= 15,876,000 bytes
		44100 * 2 * 60 = 5,292,000 samples
		44100Hz / 1s * 2canales 
		44100 * numchannels * bitdepth / 8 = bytes for 1 second
		samplerate * numchannels * bytespersample * duration = bytes

		DURATION = BYTES/ SAMPLERATE * NUMCHANNEL * BYTESPERSAMPLE
	*/

	//DATA BYTES -> 568496

protected:
	unsigned char* m_samples = nullptr;
	int m_bitDepth = 0;
	int m_sampleRate = 0;
	int m_numChannels = 0;
	int m_duration = 0;
	int m_audioDataSize = 0;

	//int audioBlock = 0;
};

