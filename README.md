# Badanie Efektywności Implementacji Kolejki Priorytetowej

Projekt realizuje i porównuje wydajność czasową operacji w dwóch różnych implementacjach **kolejki priorytetowej (posortowanej)**:
1. Na bazie jednokierunkowej **listy wiązanej** (`PriorityQueueList`).
2. Na bazie **tablicy dynamicznej** (`PriorityQueueArray`).

Program przeprowadza testy wydajnościowe (benchmark) dla różnych rozmiarów danych wejściowych i zapisuje wyniki do pliku `.csv`.

## 🚀 Funkcje i Operacje

Obie struktury implementują kolejkę priorytetową, w której element o najwyższej wartości pola `priority` znajduje się na początku. Kod mierzy czas wykonania następujących operacji:
* **Insert** – wstawienie elementu w odpowiednie miejsce z zachowaniem porządku.
* **RemoveMax** – usunięcie elementu o najwyższym priorytecie.
* **Size** – zwrócenie aktualnej liczby elementów.
* **Peek** – podgląd elementu o najwyższym priorytecie bez usuwania go.
* **ChangePriority** – zmiana priorytetu istniejącego elementu (wymaga wyszukania elementu, usunięcia go i ponownego wstawienia).

## 📊 Metodologia Pomiarów

Aby pomiary były jak najbardziej rzetelne, w programie zastosowano zaawansowane podejście benchmarkowe:
1. **Głębokie kopiowanie struktur:** Każda operacja jest mierzona na świeżej kopii bazy danych (`measureProperly`), dzięki czemu stan kolejki nie ulega trwałej degradacji podczas pętli testowej.
2. **Wielokrotne powtórzenia:** Każdy pomiar powtarzany jest **100 razy**.
3. **Średnia ucinana (Trimmed Mean):** Skrajne 5% najszybszych i 5% najwolniejszych wyników (outliery/szumy systemowe) jest odrzucane przed wyliczeniem ostatecznej średniej.

Testy są uruchamiane dla następujących rozmiarów struktur ($N$):
* 1 000 elementów
* 2 500 elementów
* 5 000 elementów
* 7 500 elementów
* 10 000 elementów
* 25 000 elementów

## 🛠️ Jak uruchomić projekt

### Wymagania
* Kompilator wspierający standard **C++11** lub nowszy (np. GCC, Clang, MSVC).
* Narzędzie CMake (opcjonalnie, domyślne dla CLion).

### Uruchomienie z poziomu konsoli
1. Skompiluj program:
   ```bash
   g++ -O3 main.cpp -o benchmark
