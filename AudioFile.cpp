#define _USE_MATH_DEFINES
#include "AudioFile.h"
#include <fstream>
#include <cmath>

void AudioFile::create(int sampleRate, int numChannels, int bitDepth, int duration) {
	m_sampleRate = sampleRate;
	m_numChannels = numChannels;
	m_bitDepth = bitDepth;
	m_duration = duration;

	m_samples = new int[getNumSamples()]();
}

void AudioFile::decode(const char* filename)
{
	std::fstream file;
	
	file.open(filename, std::ios::binary | std::ios::in | std::ios::ate);
	
	if (!file.is_open())
		return;

	int fileSize = file.tellg(); // Size of the whole file
	file.seekg(std::ios::beg);

	WAVEHEADER waveHeader;
	file.read(reinterpret_cast<char*>(&waveHeader),sizeof(WAVEHEADER));
	if (waveHeader.riff.chunkID != 0x46464952) //0x46464952 'RIFF' in big endian
		return;

	int soundDataSize = fileSize - file.tellg();

	int duration = waveHeader.data.subchunk2Size / (waveHeader.fmt.sampleRate *
					waveHeader.fmt.numChannels * 
					(waveHeader.fmt.bitsPerSample >> 3));



	create(	waveHeader.fmt.sampleRate,
			waveHeader.fmt.numChannels,
			waveHeader.fmt.bitsPerSample,
			duration);

	file.read(reinterpret_cast<char*>(m_samples), waveHeader.data.subchunk2Size);
	
	//printf("%d", m_samples[56]);

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
	waveHeader.riff.chunkID = 0x46464952;				// 'RIFF' LITTLE ENDIAN
	waveHeader.riff.chunkSize = sizeof(WAVEHEADER) + soundDataSize - 8;						// TODO: set this correctly
	waveHeader.riff.format = 0x45564157;				//'WAVE' LITTLE ENDIAN

	// FMT
	waveHeader.fmt.subchunk1ID = 0x20746D66;			//'fmt ' LITTLE ENDIAN
	waveHeader.fmt.subchunk1Size = sizeof(FMT_SUBCHUNK) - 8;				// 16
	waveHeader.fmt.audioFormat = 1;						//1 for PCM
	waveHeader.fmt.numChannels = m_numChannels;		//2 STEREO LITTLE ENDIAN
	waveHeader.fmt.sampleRate = m_sampleRate;		//SampleRate
	waveHeader.fmt.byteRate = m_sampleRate * m_numChannels * (m_bitDepth >> 3);
	waveHeader.fmt.blockAlign = m_numChannels * (m_bitDepth >> 3);
	waveHeader.fmt.bitsPerSample = m_bitDepth;		//16

	// DATA
	waveHeader.data.subchunk2ID = 0x61746164; // 'data' LITTLE ENDIAN
	waveHeader.data.subchunk2Size = soundDataSize;

	file.write(reinterpret_cast<char*>(&waveHeader),sizeof(WAVEHEADER));

	//file.write(reinterpret_cast<char*>(m_samples), soundDataSize);

	for (int samples = 0; samples < getNumSamples(); samples++)
	{
		

		int leftSample = 10000 * sin(2 * M_PI * 440 * samples / m_sampleRate);
		
		int rightSample = leftSample;  

		file.write(reinterpret_cast<char*>(&leftSample), sizeof(short));
		file.write(reinterpret_cast<char*>(&rightSample), sizeof(short));
	}





	file.close();
}



