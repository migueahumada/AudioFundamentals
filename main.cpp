#include <iostream>
#include "AudioFile.h"



struct DoubleBuffer {
	DoubleBuffer(int samples) {
		m_read = new float[samples * 2]();
		m_write = m_read + samples;
	}
	~DoubleBuffer(){
		delete[] m_read;
		m_read = nullptr;
	}

	void swap() {
		std::swap(m_read, m_write);
	}

	float* m_read = nullptr;
	float* m_write = nullptr;
};

void callback(float* buffer, int channels, int frames, void* cookie) {
	DoubleBuffer* dBuffer = reinterpret_cast<DoubleBuffer*>(cookie);
	int bytes = sizeof(float) * channels * frames;
	
}

int main() {

	/*const int samples = 8;

	float* read = new float[samples * 2];
	float* write;

	memset(read,0,samples * 2 * sizeof(float));

	write = read + samples;*/

	AudioFile audiofile(48000,2,BIT_DEPTH::B16,10);
	//audiofile.decode("Audio.wav");
	audiofile.encode("Prueba.wav");

	return 0;
}

/*int* array = new int[10] {4,34,26,6765,8,1,5777,39,3,90000};
array += 5;
int* array2 = array+2;

int e1 = *array;
int e2 = *array2;*/

//Cuando se compara punteros sobre un mismo arreglo
// p < q es verdadero si p est�
/*bool condition = array < array2;

std::cout << array << std::endl;
std::cout << array2 << std::endl;*/