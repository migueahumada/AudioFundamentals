#define _USE_MATH_DEFINES
#include "AudioFile.h"
#include <fstream>
#include <cmath>

void AudioFile::create(int sampleRate, int numChannels, int bitDepth, int duration) {
	m_sampleRate = sampleRate;
	m_numChannels = numChannels;
	m_bitDepth = bitDepth;
	m_duration = duration;

	m_samples = new unsigned char[getNumSamples()*(getBitDepth() >> 3)]();
}

//TODO: Create an implementation for multiple metadata chunks
void AudioFile::decode(const char* filename)
{
	fstream file(filename, std::ios::in | std::ios::beg | std::ios::binary);
	if (!file.is_open())
		return;

	unsigned long temp_buffer;

	file.read(reinterpret_cast<char*>(&temp_buffer), sizeof(unsigned long));
	if (temp_buffer != fourccRIFF)
		return;

	file.read(reinterpret_cast<char*>(&temp_buffer), sizeof(unsigned long));
	file.read(reinterpret_cast<char*>(&temp_buffer), sizeof(unsigned long));
	if (temp_buffer != fourccWAVE)
		return;

	while (file.peek() != EOF)
	{
		unsigned long chunkID;
		unsigned long chunkSize;

		//Read chunk ID
		file.read(reinterpret_cast<char*>(&chunkID), sizeof(unsigned long));
		file.read(reinterpret_cast<char*>(&chunkSize), sizeof(unsigned long));

		switch (chunkID)
		{
		case fourccFMT:
			WAVE_FORMAT waveFormat;
			memset(&waveFormat, 0, sizeof(WAVE_FORMAT));

			file.read(reinterpret_cast<char*>(&waveFormat), sizeof(WAVE_FORMAT));
			m_sampleRate = waveFormat.sampleRate;
			m_numChannels = waveFormat.numChannels;
			m_bitDepth = waveFormat.bitsPerSample;
			break;
		case fourccDATA:
			m_samples = new unsigned char[chunkSize];
			file.read(reinterpret_cast<char*>(m_samples), chunkSize);
			m_audioDataSize = chunkSize;
			break;

		default:

			file.seekg(chunkSize, std::ios::cur);
			break;
		}

	}
	file.close();
}

void AudioFile::encode(const char* filename)
{
	//audio file 44100/16 -> 5 segundos
	std::fstream file;
	file.open(filename, std::ios::binary | std::ios::out );

	if (!file.is_open())
	{
		return;
	}

	WAVEHEADER waveHeader;
	memset(&waveHeader,0,sizeof(WAVEHEADER));

	create(m_sampleRate,m_numChannels,m_bitDepth,m_duration);

	int soundDataSize = getNumSamples() * (getBitDepth() >> 3);

	// RIFF
	waveHeader.riff.chunkID = 0x46464952;		
	waveHeader.riff.chunkSize = sizeof(WAVEHEADER) + soundDataSize - 8;		
	waveHeader.riff.format = 0x45564157;			

	// FMT
	waveHeader.fmt.subchunk1ID = 0x20746D66;		
	waveHeader.fmt.subchunk1Size = sizeof(FMT_SUBCHUNK) - 8;	
	waveHeader.fmt.audioFormat = (m_bitDepth == 32) ? 3 : 1 ;	
	waveHeader.fmt.numChannels = m_numChannels;		
	waveHeader.fmt.sampleRate = m_sampleRate;		
	waveHeader.fmt.byteRate = m_sampleRate * m_numChannels * (m_bitDepth >> 3);
	waveHeader.fmt.blockAlign = m_numChannels * (m_bitDepth >> 3);
	waveHeader.fmt.bitsPerSample = m_bitDepth;		

	// DATA
	waveHeader.data.subchunk2ID = 0x61746164; // 'data' LITTLE ENDIAN
	waveHeader.data.subchunk2Size = soundDataSize;

	file.write(reinterpret_cast<char*>(&waveHeader),sizeof(WAVEHEADER));

	for (int i = 0; i < getNumSamples(); i++)
	{
		float normalizedSample = sin(2 * M_PI * 440 * i / m_sampleRate);

		if (m_bitDepth == 16) {
			int16_t sample = static_cast<int16_t>(normalizedSample * 32767);
			file.write(reinterpret_cast<char*>(&sample), sizeof(int16_t));
		}
		else if (m_bitDepth == 24) {
			int32_t sample = static_cast<int32_t>(normalizedSample * 8388607); // Max value for 24-bit
			file.write(reinterpret_cast<char*>(&sample), 3); // Write only 3 bytes (LSB first)
		}
		else if (m_bitDepth == 32) {
			float sample = static_cast<float>(normalizedSample);
			file.write(reinterpret_cast<char*>(&sample), sizeof(float));
		}
	}

	file.close();
}

void AudioFile::encodeNew(const char* filename)
{
	std::fstream file;
	file.open(filename, std::ios::binary | std::ios::out);

	if (!file.is_open())
	{
		return;
	}


	WAVEHEADER waveHeader;
	memset(&waveHeader, 0, sizeof(WAVEHEADER));

	// RIFF
	waveHeader.riff.chunkID = 0x46464952;				// 'RIFF' LITTLE ENDIAN
	waveHeader.riff.chunkSize = sizeof(WAVEHEADER) + m_audioDataSize - 8;						// TODO: set this correctly
	waveHeader.riff.format = 0x45564157;				//'WAVE' LITTLE ENDIAN

	// FMT
	waveHeader.fmt.subchunk1ID = 0x20746D66;			//'fmt ' LITTLE ENDIAN
	waveHeader.fmt.subchunk1Size = sizeof(FMT_SUBCHUNK) - 8;				// 16
	waveHeader.fmt.audioFormat = (m_bitDepth == 32) ? 3 : 1;						//1 for PCM
	waveHeader.fmt.numChannels = m_numChannels;		//2 STEREO LITTLE ENDIAN
	waveHeader.fmt.sampleRate = m_sampleRate;		//SampleRate
	waveHeader.fmt.byteRate = m_sampleRate * m_numChannels * (m_bitDepth >> 3);
	waveHeader.fmt.blockAlign = m_numChannels * (m_bitDepth >> 3);
	waveHeader.fmt.bitsPerSample = m_bitDepth;		//16

	// DATA
	waveHeader.data.subchunk2ID = 0x61746164; // 'data' LITTLE ENDIAN
	waveHeader.data.subchunk2Size = m_audioDataSize;

	file.write(reinterpret_cast<char*>(&waveHeader), sizeof(WAVEHEADER));
	file.write(reinterpret_cast<char*>(m_samples), m_audioDataSize);
	
	file.close();
}



