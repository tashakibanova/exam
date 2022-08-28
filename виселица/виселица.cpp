// виселица.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <ctime>
#include <sys\locking.h>
#include <string.h>
#include <ctype.h>
#include <iomanip>
#include <conio.h>
using namespace std;
#pragma warning(disable : 4996)

//Максимальная длина слова
#define MAX_WORD_LENGTH 21
//Кол-во попыток
int Tries = 10;
//Кол-во угаданных слов
int CountWords = 0;
//Загрузка слова
bool LoadWord(FILE* file, char* word)
{
	int i = 0;
	char s[MAX_WORD_LENGTH] = { 0 };
	//Кол-во слов в файле
	static int count = -1;
	if (count == -1)
	{
		//Подсчет количества слов
		while (!feof(file))
		{
			fgets(s, MAX_WORD_LENGTH, file);
			count++;
		}
		//Слов нет?
		if (count == 0)
			return false;
		//Возврат файлового указателя в начало файла
		fseek(file, 0, 0);
	}
	//Случайное слово
	int n = rand() % count;
	//Поиск слова
	while (i <= n)
	{
		fgets(s, MAX_WORD_LENGTH, file);
		i++;
	}
	//Определяем длину слова
	int wordlen = strlen(s);
	//Минимальная длина слова 2 буквы
	if (wordlen <= 1)
		return false;
	//Убираем Enter (в DOS'е 2 байта 13 10)
	if (s[wordlen - 1] == 10)
		s[wordlen - 2] = 0;
	else if (s[wordlen - 1] == 13)
		s[wordlen - 1] = 0;
	//Копируем слово
	strcpy(word, s);
	//Получаем дескриптор файла
	int hFile = _fileno(file);
	//Вычисляем размер файла
	int size = _filelength(hFile);
	//Блокировка файла
	fseek(file, 0, 0);
	_locking(hFile, _LK_NBLCK, size);
	return true;
}
//Игра
void Game(char* word)
{
	//Таймер иггры
	srand(time(NULL));
	clock_t t = clock();
	//Перевод в большие буквы
	strupr(word);
	int len = strlen(word);
	//Строка-копия
	char* copy = new char[len + 1];
	memset(copy, '*', len);
	copy[len] = 0;
	//Алфавит + пробелы
	char letters[52];
	int i, j = 0;
	for (i = 0; i < 26; i++)
	{
		letters[j++] = i + 'A';
		letters[j++] = ' ';
	}
	//Замыкающий ноль
	letters[51] = 0;
	//Буква
	char letter;
	char* pos;
	bool replace = false;
	do {
		//Очистка экрана
		system("cls");
		cout << "========" << endl
			<< "ВИСЕЛИЦА" << endl
			<< "========" << endl << endl;
		cout << "Загаданное слово " << copy << endl << endl;
		cout << letters << endl << endl;
		cout << "Количество попыток: " << Tries << endl
			<< endl;
		cout << "Введите букву:\t";
		cin >> letter;
		int time = (clock() - t) / CLOCKS_PER_SEC;
		//Звуковой сигнал
		Beep(500, 200);
		//if(letter >= 'A' && letter <= 'Z'
		//|| letter >= 'a' && letter <= 'z')
		//Буква?
		if (!isalpha(letter))
		{
			cout << "Это не буква" << endl;
			//Задержка на 1 секунду
			Sleep(1000);
			continue;
		}
		//Перевод буквы в большую
		letter = toupper(letter);
		//Поиск буквы в алфавите
		pos = strchr(letters, letter);
		//Такая буква уже была
		if (pos == 0)
		{
			cout << "Такая буква уже была" << endl;
			Sleep(1000);
			continue;
		}
		else
		{
			//Убираем букву из алфавита
			pos[0] = ' ';
		}
		//Поиск буквы в слове
		for (i = 0; i < len; i++)
		{
			if (word[i] == letter)
			{
				copy[i] = letter;
				replace = true;
			}
		}
		if (replace == false)
			Tries--;
		else
			replace = false;
		//Условие победы
		if (strcmp(word, copy) == 0)
		{
			system("cls");
			cout << copy << endl << endl;
			cout << letters << endl << endl;

			cout << "Количество попыток: " << Tries << endl;
			cout << "Потраченное время: " << time << " сек." << endl;
			cout << "Победа!" << endl;
			CountWords++;
			break;
		}
	} while (Tries != 0);
	delete[] copy;
}
void main()
{
	setlocale(LC_ALL, "rus");

	//Открываем файл на чтение в двоичном режиме
	FILE* f = fopen("words.txt", "rb");
	//Если файл не открылся
	if (f == 0)
	{
		//Ошибка
		perror("Open");
		return;
	}
	srand(time(0));
	char Word[20];
	//Пытаемся загрузить слово
	if (!LoadWord(f, Word))
	{
		//Если неудачно
		cout << "Error!" << endl;
		fclose(f);
		return;
	}
	char answer;
	//Играем, пока не надоест

	do
	{
		Game(Word);
		//Если попыток не осталось, то выход
		if (Tries == 0)
		{
			cout << "Было загадано слово: " << Word << endl;
			cout << "Попытки закончились. Количество угаданных слов: " << CountWords << endl;
			cout << "Конец игры" << endl;
			break;
		}
		//Если остались
		cout << "Продолжить ??? (Y/N)\t";
		cin >> answer;
		//Еще играем?
		if (answer == 'Y' || answer == 'y')
			if (!LoadWord(f, Word))
			{
				cout << "Ошибка!" << endl;
				fclose(f);
				return;
			}
	} while (answer == 'Y' || answer == 'y');


	//получаем дескриптор
	int hFile = _fileno(f);
	//Разблокировка файла
	int size = _filelength(hFile);
	fseek(f, 0, 0);
	_locking(hFile, _LK_UNLCK, size);
	fclose(f);
}

