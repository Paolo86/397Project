#pragma once
#include <iostream>
#include <chrono>
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <string>  // string

#if defined(_WIN32) || defined(_WIN64)
#include<Windows.h>
#endif



class Logger
{
public:
	static void Initialize();


	template<class T, class...Args>
	static void LogInfo(T&& f, Args&&...pack);
	template<class T>
	static void LogInfo(T&& f);

	template<class T, class...Args>
	static void LogError(T&& f, Args&&...pack);
	template<class T>
	static void LogError(T&& f);

private:
	static bool first;

};



template<class T, class...Args>
void Logger::LogInfo(T&& f, Args&&...pack)
{
#if defined(_WIN32) || defined(_WIN64)
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

#endif	

	if (first)
	{
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::string s(20, '\0');
	std::strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
	first = 0;
	std::cout << s << f << " ";
	}
	else
		std::cout << f << " ";

	LogInfo(std::forward<Args>(pack)...);


}

template<class T>
void Logger::LogInfo(T&& f)
{
#if defined(_WIN32) || defined(_WIN64)
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

#endif
	if (first)
	{
		std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::string s(20, '\0');
		std::strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
		first = 0;
		std::cout << s << f << "\n";
	}
	else
		std::cout << f << "\n";

	first = 1;

}

template<class T, class...Args>
void Logger::LogError(T&& f, Args&&...pack)
{
#if defined(_WIN32) || defined(_WIN64)
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout,FOREGROUND_RED| FOREGROUND_INTENSITY);
#endif	

	if (first)
	{
		std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::string s(20, '\0');
		std::strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
		first = 0;
		std::cout << s << f << " ";
	}
	else
		std::cout << f << " ";

	LogError(std::forward<Args>(pack)...);


}

template<class T>
void Logger::LogError(T&& f)
{
#if defined(_WIN32) || defined(_WIN64)
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);

#endif
	if (first)
	{
		std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::string s(20, '\0');
		std::strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
		first = 0;
		std::cout << s << f << "\n";
	}
	else
	std::cout << f << "\n";
	first = 1;

}