// PEA1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

class TSP {
private:
	int n;									// liczba miast
	int** graph;							// macierz koszt�w

	vector<int> currentPath;
	vector<int> bestPath;
	int minCost;
	double duration;

public:
	int getNumCities() const { return n; }
	void setNumCities(int num) { n = num; }
	
	int** getGraph() const { return graph; }
	void setGraph(int** newGraph) { graph = newGraph; }
	
	TSP(int numCities) : n(numCities) {		// inicjalizacja macierzy koszt�w
		graph = new int*[n];				// dwuwymiarowa tablica wska�nik�w reprezentuj�ca macierz koszt�w mi�dzy miastami
		for (int i = 0; i < n; i++) {
			graph[i] = new int[n];
		}
	}

	~TSP() {
		for (int i = 0; i < n; i++) {
			delete[] graph[i];
		}
		delete[] graph;
	}

	void addEdge(int u, int v, int cost) {
		graph[u][v] = cost;
	}

	void printSolution() {
		cout << "Najkrotsza sciezka: ";
		for (int i : bestPath) {
			cout << i << " ";
		}
		cout << "0";
		cout << "\nKoszt: " << minCost;
		cout << "\nCzas: " << std::fixed << std::setprecision(6) << duration << " ms" << endl;
	}

	void solve() {
		currentPath.clear();											// wyczy�� bie��c� �cie�k�
		currentPath.push_back(0);										// zaczynamy od pierwszego miasta (0)
		minCost = INT_MAX;												// dowolna bardzo du�a warto��, kt�ra zostanie zast�piona prawdziwym kosztem najkr�tszej �cie�ki

		vector<int> citiesToVisit;										// tworzymy wektor miast do odwiedzenia
		for (int i = 1; i < n; i++) {
			citiesToVisit.push_back(i);
		}

		clock_t start = clock();										// rozpoczynamy pomiar czasu
		tspRecursion(citiesToVisit, 0);									// rozpoczynamy przegl�d zupe�ny
		clock_t end = clock();											// zako�cz pomiar czasu
		duration = double(end - start) / CLOCKS_PER_SEC * 1000;			// czas w milisekundach
	}

private:
	void tspRecursion(vector<int>& cities, int costSoFar) {
		if (cities.empty()) {											// jesli nie ma ju� miast do odwiedzenia
			int finalCost = graph[currentPath.back()][0];				// obliczamy koszt powrotu do pierwszego miasta (0) z ostatniego odwiedzonego miasta
			if (costSoFar + finalCost < minCost) {						// sprawdzamy, czy koszt dotychczasowej trasy jest mniejszy ni� aktualnie najlepszy znaleziony koszt
				minCost = costSoFar + finalCost;
				bestPath = currentPath;
			}
			return;
		}

		for (size_t i = 0; i < cities.size(); i++) {
			int nextCity = cities[i];									// wyb�r nastepnego do odwiedzenia miasta
			int costToNextCity = graph[currentPath.back()][nextCity];	// obliczamy koszt przej�cia z ostatniego miasta na �cie�ce do nast�pnego miasta

			//if (costSoFar + costToNextCity < minCost) {					// sprawdzamy, czy dodanie kosztu do bie��cego kosztu podr�y jest mniejsze ni� dotychczasowy najlepszy koszt
				currentPath.push_back(nextCity);						// dodajemy sprawdzane miasto do bie��cej �ciezki
				cities.erase(cities.begin() + i);						// usuwamy sprawdzane miasto z listy do odwiedzenia

				tspRecursion(cities, costSoFar + costToNextCity);		// wywo�ujemy tspRecursion dla nowej �cie�ki i zaktualizowanego kosztu podr�y

				cities.insert(cities.begin() + i, nextCity);			// po powrocie z tspRecursion przywracamy sprawdzane miato na odpowiednie miejsce w li�cie miast do odwiedzenia (aby mo�na by�o rozwa�y� je ponownie w przysz�ych iteracjach)
				currentPath.pop_back();									// cofamy si� do poprzedniego miasta na �cie�ce
			//}
		}
	}
};

int _tmain(int argc, _TCHAR* argv[]) {
	TSP tsp(0);															// tworzymy obiekt TSP, ale jeszcze nie ma danych

	int choice;
	bool dataLoaded = false;											// flaga, kt�ra m�wi nam, czy dane zosta�y ju� wczytane lub wygenerowane

	do {
		cout << "\nMenu Programu:\n";
		cout << "1. Wczytaj dane z pliku\n";
		cout << "2. Wygeneruj dane losowe\n";
		cout << "3. Wyswietl ostatnio wczytane lub wygenerowane dane\n";
		cout << "4. Uruchom algorytm i wyswietl wyniki\n";
		cout << "5. Wyjdz z programu\n";
		cout << "Wybierz opcje: ";
		cin >> choice;

		switch (choice) {
		case 1: {
					cout << "Podaj nazwe pliku :";
					string filename;
					cin >> filename;
					ifstream file(filename);						// otw�rz plik
					if (!file.is_open()) {
						cout << "Nie udalo sie otworzyc pliku." << endl;
						break;
					}

					int numCities;
					file >> numCities;									// wczytaj liczb� miast z pliku
					tsp.setNumCities(numCities);						

					int** newGraph = new int*[numCities];
					for (int i = 0; i < numCities; i++) {
						newGraph[i] = new int[numCities];
					}

																		// wczytaj macierz koszt�w
					for (int i = 0; i < numCities; i++) {
						for (int j = 0; j < numCities; j++) {
							file >> newGraph[i][j];
						}
					}
					tsp.setGraph(newGraph);								// ustaw now� macierz koszt�w
					dataLoaded = true;
					cout << "Dane zostaly wczytane." << endl;

					file.close();										// zamknij plik 
					break;
		}

		case 2: {
					int numCities;
					cout << "Podaj liczbe miast: ";
					cin >> numCities;

					tsp.setNumCities(numCities);						
					int** newGraph = new int*[numCities];
					for (int i = 0; i < numCities; i++) {
						newGraph[i] = new int[numCities];
					}
								
																		// generowanie losowych danych
					for (int i = 0; i < numCities; i++) {
						for (int j = 0; j < numCities; j++) {
							if (i != j) {
								newGraph[i][j] = rand() % 1000 + 1;		// losowe koszty z przedzia�u 1-1000

							}
							else {
								newGraph[i][j] = -1;					// na przek�tnej -1
							}
						}
					}
					tsp.setGraph(newGraph);								// ustaw now� macierz koszt�w
					dataLoaded = true;
					cout << "Dane zostaly wygenerowane." << endl;
					break;
		}
		case 3: {
					if (dataLoaded) {
						int** graph = tsp.getGraph();
						cout << "Ostatnio wczytane lub wygenerowane dane:" << endl;
						for (int i = 0; i < tsp.getNumCities(); i++) {
							for (int j = 0; j < tsp.getNumCities(); j++) {
								cout << graph[i][j] << " ";
							}
							cout << endl;
						}

					}
					else {
						cout << "Nie wczytano jeszcze zadnych danych." << endl;
					}
					break;
		}
		case 4: {
					if (dataLoaded) {
						tsp.solve();
						tsp.printSolution();
					}
					else {
						cout << "Nie wczytano jeszcze zadnych danych." << endl;
					}
					break;
		}
		case 5: {
					break;
		}
		default: {
					 cout << "Nieprawidlowy wybor. Sprobuj ponownie." << endl;
					 break;
		}
		}
	} while (choice != 5);

	/*
	vector<int> values_of_N = {10, 20, 30, 40, 50, 75, 100};			// r�ne warto�ci N

	for (int N : values_of_N) {
	double total_time = 0.0;

	for (int i = 0; i < 100; i++) {										// generowanie 100 losowych instancji
	TSP tsp(N);

	for (int i = 0; i < N; i++) {
	for (int j = 0; j < N; j++) {
	if (i != j) {
	tsp.addEdge(i, j, rand() % 1000 + 1);								// dodaj losowe koszty
	}
	else {
	tsp.addEdge(i, j, -1);
	}
	}
	}

	clock_t start = clock();
	tsp.solve();
	clock_t end = clock();

	double duration = double(end - start) / CLOCKS_PER_SEC * 1000;
	total_time += duration;
	}

	double avg_time = total_time / 100;
	cout << "Sredni czas dla N = " << N << ": " << std::fixed << std::setprecision(3) << avg_time << " ms" << endl;
	}
	system("pause");
	*/
	
	return 0;
}
