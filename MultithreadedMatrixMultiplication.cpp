#include <vector>
#include <random>
#include <thread>
#include <iostream>
#include <chrono>


bool areMatricesEqual(const std::vector<std::vector<int>> & matrixA, const std::vector<std::vector<int>> & matrixB){
	const auto size = matrixA.size();
	for (int i = 0; i < size; ++i){
		for (int j = 0; j < size; ++j){
			if (matrixA[i][j] != matrixB[i][j]) return false;
		}
	}
	return true;
}

void initializeMatrix(std::vector<std::vector<int>> & matrix, int size, int upperRange){
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> dist(0, upperRange);

	for (int i = 0; i < size; ++i){
		for (int j = 0; j < size; ++j){
			matrix[i][j] = dist(mt);
		}
	}
}

std::vector<std::vector<int>> singleThreadedMatrixMultiply(const std::vector<std::vector<int>> & matrixA, const std::vector<std::vector<int>> & matrixB, int size){
	std::vector<std::vector<int>> matrixC(size, std::vector<int>(size, 0));

	for (int i = 0; i < size; ++i){
		for (int k = 0; k < size; ++k){
			for (int j = 0; j < size; ++j){
				matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
			}
		}
	}

	return matrixC;
}

void runMultithreadedAlgorithm(const std::vector<std::vector<int>> & matrixA, const std::vector<std::vector<int>> & matrixB, std::vector<std::vector<int>> & matrixC, int size, int start, int end){
	for (int i = start; i < end; ++i){
		for (int k = 0; k < size; ++k){
			for (int j = 0; j < size; ++j){
				matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
			}
		}
	}
}

std::vector<std::vector<int>> multiThreadedMatrixMultiply(const std::vector<std::vector<int>> & matrixA, const std::vector<std::vector<int>> & matrixB, int size){
	std::vector<std::vector<int>> matrixC(size, std::vector<int>(size, 0));

	std::vector<std::thread> threads;
	unsigned int hardwareThreads = std::thread::hardware_concurrency();
	unsigned int numberOfThreads = hardwareThreads != 0 ? hardwareThreads : 2;

	const int step = size / numberOfThreads;
	for (int i = 0; i < numberOfThreads; ++i){
		threads.emplace_back(runMultithreadedAlgorithm, std::cref(matrixA), std::cref(matrixB), std::ref(matrixC), size, i * step, i * step + step);
	}

	//wait for all the threads to finish
	for (auto & t : threads) t.join();

	return matrixC;
}


int main(){

	const int size = 1000;
	const int upperRange = 10;

	//using a vector of vectors instead of int** to represent a matrix
	std::vector<std::vector<int>> matrixA(size, std::vector<int>(size, 0));
	std::vector<std::vector<int>> matrixB(size, std::vector<int>(size, 0));

	initializeMatrix(matrixA, size, upperRange);
	initializeMatrix(matrixB, size, upperRange);

	auto startOne = std::chrono::high_resolution_clock::now();
	auto resultOne = singleThreadedMatrixMultiply(matrixA, matrixB, size);
	auto endOne = std::chrono::high_resolution_clock::now();
	std::cout << "Single threaded took: " << std::chrono::duration_cast<std::chrono::seconds>(endOne - startOne).count() << " seconds.\n";

	auto startTwo = std::chrono::high_resolution_clock::now();
	auto resultTwo = multiThreadedMatrixMultiply(matrixA, matrixB, size);
	auto endTwo = std::chrono::high_resolution_clock::now();
	std::cout << "Multithreaded took: " << std::chrono::duration_cast<std::chrono::seconds>(endTwo - startTwo).count() << " seconds.\n";

	if (areMatricesEqual(resultOne, resultTwo)){
		std::cout << "2 matrices are equal.\n";
	}
	else{
		std::cout << "2 matrices are not equal.\n";
	}

	return 0;
}