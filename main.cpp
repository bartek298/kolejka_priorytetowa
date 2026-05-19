#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>
#include <fstream>

using namespace std;
using namespace chrono;

// Struktura reprezentująca element w kolejce priorytetowej
struct Element {
    int value;
    int priority; // wyższa liczba = wyższy priorytet
};

// --- 1. KOLEJKA PRIORYTETOWA NA LIŚCIE WIĄZANEJ (Posortowana) 
struct Node {
    Element data;
    Node* next;
    Node(Element val) {
        data = val;
        next = nullptr;
    }
};

class PriorityQueueList {
private:
    Node* head;
    int current_size;
public:
    PriorityQueueList() {
        head = nullptr;
        current_size = 0;
    }

    PriorityQueueList(const PriorityQueueList& other) {
        head = nullptr;
        current_size = other.current_size;
        Node* temp = other.head;
        Node* tail = nullptr;
        while (temp) {
            Node* newNode = new Node(temp->data);
            if (!head) {
                head = newNode;
                tail = newNode;
            } else {
                tail->next = newNode;
                tail = newNode;
            }
            temp = temp->next;
        }
    }

    ~PriorityQueueList() {
        while (head) {
            Node* tmp = head;
            head = head->next;
            delete tmp;
        }
    }

    // Dodawanie elementu w odpowiednie miejsce (wielkość priorytetu malejąco)
    void insert(int val, int priority) {
        Element el = {val, priority};
        Node* newNode = new Node(el);
        current_size++;

        if (!head || head->data.priority < priority) {
            newNode->next = head;
            head = newNode;
            return;
        }

        Node* temp = head;
        while (temp->next && temp->next->data.priority >= priority) {
            temp = temp->next;
        }
        newNode->next = temp->next;
        temp->next = newNode;
    }

    // Usuwanie elementu o najwyższym priorytecie (z początku)
    void removeMax() {
        if (!head) return;
        Node* temp = head;
        head = head->next;
        delete temp;
        current_size--;
    }

    // Zwracanie rozmiaru kolejki
    int size() const {
        return current_size;
    }

    // Peek - podgląd elementu o najwyższym priorytecie
    Element peek() const {
        if (!head) return {-1, -1};
        return head->data;
    }

    // Modyfikacja priorytetu określonego elementu (wyszukiwanego po wartości)
    void changePriority(int val, int newPriority) {
        //  usuń stary i dodaj nowy z nowym priorytetem
        Node* temp = head;
        Node* prev = nullptr;
        bool found = false;

        while (temp) {
            if (temp->data.value == val) {
                found = true;
                if (prev) {
                    prev->next = temp->next;
                } else {
                    head = temp->next;
                }
                delete temp;
                current_size--;
                break;
            }
            prev = temp;
            temp = temp->next;
        }

        if (found) {
            insert(val, newPriority);
        }
    }
};


// --- 2. KOLEJKA PRIORYTETOWA NA TABLICY DYNAMICZNEJ (Posortowana) 
class PriorityQueueArray {
private:
    Element* data;
    int current_size;
    int capacity;

    void resize() {
        capacity *= 2;
        Element* newData = new Element[capacity];
        for (int i = 0; i < current_size; i++)
            newData[i] = data[i];
        delete[] data;
        data = newData;
    }

public:
    PriorityQueueArray(int cap = 10) {
        capacity = cap;
        current_size = 0;
        data = new Element[capacity];
    }

    PriorityQueueArray(const PriorityQueueArray& other) {
        current_size = other.current_size;
        capacity = other.capacity;
        data = new Element[capacity];
        for (int i = 0; i < current_size; i++)
            data[i] = other.data[i];
    }

    ~PriorityQueueArray() {
        delete[] data;
    }

    // Dodawanie elementu z zachowaniem porządku (od najwyższego priorytetu do najniższego)
    void insert(int val, int priority) {
        if (current_size == capacity) resize();

        int i = current_size - 1;
        // Przesuwamy elementy w tył, dopóki mają mniejszy priorytet niż nowy element
        while (i >= 0 && data[i].priority < priority) {
            data[i + 1] = data[i];
            i--;
        }
        data[i + 1] = {val, priority};
        current_size++;
    }

    // Usuwanie elementu o najwyższym priorytecie (z początku tablicy)
    void removeMax() {
        if (current_size == 0) return;
        // Przesunięcie w lewo
        for (int i = 0; i < current_size - 1; i++) {
            data[i] = data[i + 1];
        }
        current_size--;
    }

    // Zwracanie rozmiaru
    int size() const {
        return current_size;
    }

    // Peek - najwyższy priorytet jest na indeksie 0
    Element peek() const {
        if (current_size == 0) return {-1, -1};
        return data[0];
    }

    // Modyfikacja priorytetu określonego elementu
    void changePriority(int val, int newPriority) {
        int index = -1;
        for (int i = 0; i < current_size; i++) {
            if (data[i].value == val) {
                index = i;
                break;
            }
        }
        if (index != -1) {
            // Usuwamy element poprzez przesunięcie reszty tablicy
            for (int i = index; i < current_size - 1; i++) {
                data[i] = data[i + 1];
            }
            current_size--;
            // Wstawiamy go ponownie z nowym priorytetem, aby zachować sortowanie
            insert(val, newPriority);
        }
    }
};


// --- POMIARY ---

double averageTrim(vector<long long> times) {
    sort(times.begin(), times.end());
    int trim = times.size() * 0.05;

    vector<long long> trimmed(
        times.begin() + trim,
        times.end() - trim
    );

    double sum = accumulate(trimmed.begin(), trimmed.end(), 0.0);
    return sum / trimmed.size();
}

template<typename T, typename Op>
double measureProperly(const T& original, Op operation) {
    vector<long long> times;

    for (int i = 0; i < 100; i++) {
        T temp = original; // Kopiowanie poza pomiarem

        auto start = high_resolution_clock::now();
        operation(temp);
        auto end = high_resolution_clock::now();

        times.push_back(duration_cast<nanoseconds>(end - start).count());
    }

    return averageTrim(times);
}

int main() {

    vector<int> sizes = {1000, 2500, 5000, 7500, 10000,25000};
    ofstream file("priority_queue_results5.csv");
    file << "Structure,Operation,Size,Time(ns)\n";

    for (int n : sizes) {
        PriorityQueueArray pqArray;
        PriorityQueueList pqList;

        // Wypełnianie struktur losowymi/przykładowymi priorytetami
        for (int i = 0; i < n; i++) {
            pqArray.insert(i, i); // wartość i, priorytet i
            pqList.insert(i, i);
        }

        cout << "Mierzenie dla rozmiaru bazy danych: " << n << endl;

        // --- POMIARY DLA TABLICY ---
        file << "PriorityQueueArray,Insert," << n << "," << measureProperly(pqArray, [](PriorityQueueArray& q) { q.insert(99999, 500); }) << "\n";
        file << "PriorityQueueArray,RemoveMax," << n << "," << measureProperly(pqArray, [](PriorityQueueArray& q) { q.removeMax(); }) << "\n";
        file << "PriorityQueueArray,Size," << n << "," << measureProperly(pqArray, [](PriorityQueueArray& q) { q.size(); }) << "\n";
        file << "PriorityQueueArray,Peek," << n << "," << measureProperly(pqArray, [](PriorityQueueArray& q) { q.peek(); }) << "\n";
        file << "PriorityQueueArray,ChangePriority," << n << "," << measureProperly(pqArray, [n](PriorityQueueArray& q) { q.changePriority(n / 2, 99999); }) << "\n";

        // --- POMIARY DLA LISTY ---
        file << "PriorityQueueList,Insert," << n << "," << measureProperly(pqList, [](PriorityQueueList& q) { q.insert(99999, 500); }) << "\n";
        file << "PriorityQueueList,RemoveMax," << n << "," << measureProperly(pqList, [](PriorityQueueList& q) { q.removeMax(); }) << "\n";
        file << "PriorityQueueList,Size," << n << "," << measureProperly(pqList, [](PriorityQueueList& q) { q.size(); }) << "\n";
        file << "PriorityQueueList,Peek," << n << "," << measureProperly(pqList, [](PriorityQueueList& q) { q.peek(); }) << "\n";
        file << "PriorityQueueList,ChangePriority," << n << "," << measureProperly(pqList, [n](PriorityQueueList& q) { q.changePriority(n / 2, 99999); }) << "\n";
    }

    file.close();
    cout << "Pomiar zakonczony. Wyniki zapisano w pliku priority_queue_results5.csv\n";
    return 0;
}
