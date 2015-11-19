#include <iostream>
#include <string>
#include <vector>
#include <condition_variable>
#include <stdio.h>
#include <fstream>
#include <ctime>

std::vector<std::string> buffer1;
std::vector<std::string> buffer2;
std::mutex b1_Mutex;
std::mutex b2_Mutex;

int DEFAULT_CAPACITY = 2;
bool shutdown = false;
bool listening = true;

void listen() {
	while (true) {
		while (buffer1.size() > 0) {
			
			// -- manipulate logs here
			b1_Mutex.lock();
			b2_Mutex.lock();
			buffer2.push_back(buffer1.back());
			buffer1.pop_back();
			b2_Mutex.unlock();
			b1_Mutex.unlock();
			if (shutdown && buffer1.size() == 0) {
				listening = false;
				break;
			}
		}
		if (shutdown) {
			listening = false;
			break;
		}
	}
}


void write() {
	std::ofstream myfile;
	std::string fileName = "C:/log.txt";
	myfile.open(fileName);

	while (true) {
		if (buffer2.size() > 0) {
			b2_Mutex.lock();
			myfile << buffer2.back() << "\n";
			buffer2.pop_back();
			b2_Mutex.unlock();
		}
		if (shutdown && buffer2.size() == 0 && !listening) {
			break;
		}
	}
}

void main(std::string log) {


	std::clock_t start;
	double duration;

	start = std::clock();
	// Thread setup
	buffer1.reserve(DEFAULT_CAPACITY);
	buffer2.reserve(DEFAULT_CAPACITY);

	std::thread T1(listen);
	std::thread T2(write);

	int i = 0;
	while (i < 1000000) {
		b1_Mutex.lock();
		buffer1.push_back("Test" + std::to_string(i));
		b1_Mutex.unlock();
		i++;
	}
	shutdown = true;


	T1.join();
	T2.join();

	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	std::cout << "durration: " << duration << '\n';
	std::cin.get();
}