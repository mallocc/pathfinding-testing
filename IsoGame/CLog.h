#pragma once

#include <stdio.h>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
#include <thread>
#include <chrono>

namespace debug
{
	namespace CLog
	{
		static std::string CDATE_TIME()
		{
			std::time_t t = std::time(0);
			std::tm* now = std::localtime(&t);
			std::stringstream ss;
			ss << (now->tm_hour) << ":";
			ss << (now->tm_min) << ":";
			ss << (now->tm_sec) << " ";
			ss << (now->tm_mday) << "-";
			ss << (now->tm_mon + 1) << "-";
			ss << (now->tm_year + 1900) << " > ";
			return ss.str();
		}

		static std::string getFilename(std::string str)
		{
			return str.substr(str.find_last_of("/\\") + 1);
		}

		static void CInfo(std::string str)
		{
			printf("%s[INFO ]: %s\n", CDATE_TIME().c_str(), str.c_str());
		}

		static void CError(std::string str, const char * file, int line)
		{
			printf("%s[ERROR]: <%s:%i> %s\n", CDATE_TIME().c_str(), getFilename(file).c_str(), line, str.c_str());
		}
	}
}

#define CERROR(str, file, line) debug::CLog::CError(str,file,line)

//namespace CLog_old
//{
//
//	// threaded
//	namespace type
//	{
//		struct Console_T
//		{
//			std::vector<std::string> messageQueue;
//			volatile bool terminated;
//			volatile bool queueing;
//			volatile bool free;
//			volatile bool LOG_WATCHDOG_TIMER;
//		};
//	}
//
//
//
//	type::Console_T * getLogger();
//
//	static std::string CDATE_TIME()
//	{
//		std::time_t t = std::time(0);
//		std::tm* now = std::localtime(&t);
//		std::stringstream ss;
//		ss << (now->tm_hour) << ":";
//		ss << (now->tm_min) << ":";
//		ss << (now->tm_sec) << " ";
//		ss << (now->tm_mday) << "-";
//		ss << (now->tm_mon + 1) << "-";
//		ss << (now->tm_year + 1900) << " > ";
//		return ss.str();
//	}
//
//	static void CQUEUE_MSG(type::Console_T * console_t, std::string msg)
//	{
//		while (!console_t->free)
//			if (console_t->LOG_WATCHDOG_TIMER++ > 1000)
//			{
//				printf("LOG ERROR: LOG_WATCHDOG_TIMER\n");
//				return;
//			}
//		console_t->queueing = true;
//		console_t->messageQueue.push_back(msg);
//		console_t->queueing = false;
//	}
//	static void CQUEUE(type::Console_T * console_t, std::string msg)
//	{
//		std::thread(std::bind(CQUEUE_MSG, console_t, msg)).detach();
//	}
//
//	static void CINFO(type::Console_T * console_t, const char * functionName, const char * varName, const char * value, const char * msg)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[INFO ]: ";
//		ss << functionName << " " << varName << " = " << value << " - " << msg << std::endl;
//		CQUEUE(console_t, ss.str());
//	}
//	static void CINFO(type::Console_T * console_t, const char * functionName, const char * varName, float value, const char * msg)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[INFO ]: ";
//		ss << functionName << " " << varName << " = " << value << " - " << msg << std::endl;
//		CQUEUE(console_t, ss.str());
//	}
//	static void CINFO(type::Console_T * console_t, const char * functionName, const char * varName, int value, const char * msg)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[INFO ]: ";
//		ss << functionName << " " << varName << " = " << value << " - " << msg << std::endl;
//		CQUEUE(console_t, ss.str());
//	}
//	static void CINFO(type::Console_T * console_t, const char * functionName, const char * msg)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[INFO ]: ";
//		ss << functionName << " - " << msg << std::endl;
//		CQUEUE(console_t, ss.str());
//	}
//	static void CINFO(type::Console_T * console_t, const char * msg)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[INFO ]: ";
//		ss << msg << std::endl;
//		CQUEUE(console_t, ss.str());
//	}
//
//	static void CERROR(type::Console_T * console_t, const char * functionName, const char * varName, const char * value, const char * msg)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[ERROR]: ";
//		ss << functionName << " " << varName << " = " << value << " - " << msg << std::endl;
//		CQUEUE(console_t, ss.str());
//	}
//	static void CERROR(type::Console_T * console_t, const char * functionName, const char * varName, float value, const char * msg)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[ERROR]: ";
//		ss << functionName << " " << varName << " = " << value << " - " << msg << std::endl;
//		CQUEUE(console_t, ss.str());
//	}
//	static void CERROR(type::Console_T * console_t, const char * functionName, const char * varName, int value, const char * msg)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[ERROR]: ";
//		ss << functionName << " " << varName << " = " << value << " - " << msg << std::endl;
//		CQUEUE(console_t, ss.str());
//	}
//	static void CERROR(type::Console_T * console_t, const char * functionName, const char * msg)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[ERROR]: ";
//		ss << functionName << " - " << msg << std::endl;
//		CQUEUE(console_t, ss.str());
//	}
//	static void CERROR(type::Console_T * console_t, const char * msg)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[ERROR]: ";
//		ss << msg << std::endl;
//		CQUEUE(console_t, ss.str());
//	}
//
//	static void CWRITER(type::Console_T * console_t)
//	{
//		while (!console_t->terminated)
//		{
//			if (!console_t->messageQueue.empty())
//			{
//				if (!console_t->queueing)
//				{
//					console_t->free = false;
//					printf(console_t->messageQueue.front().c_str());
//					console_t->messageQueue.erase(console_t->messageQueue.begin());
//					console_t->free = true;
//				}
//			}
//			std::this_thread::sleep_for(std::chrono::milliseconds(10));
//		}
//
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[INFO ]: ";
//		ss << "LOG ENDED." << std::endl;
//		printf(ss.str().c_str());
//	}
//	static void CTERMINATE(type::Console_T * console_t)
//	{
//		console_t->terminated = true;
//	}
//	static void CSTART(type::Console_T * console_t)
//	{
//		console_t->messageQueue = std::vector<std::string>();
//		console_t->terminated = false;
//		console_t->queueing = false;
//		console_t->free = true;
//		console_t->LOG_WATCHDOG_TIMER = 0;
//		std::thread(std::bind(CWRITER, console_t)).detach();
//		CINFO(console_t, "console logger has started.");
//	}
//
//
//	// static inline
//	static void CINFO(const char * functionName, const char * varName, const char * value, const char * msg)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[INFO ]: ";
//		ss << functionName << " " << varName << " = " << value << " - " << msg << std::endl;
//		printf(ss.str().c_str());
//	}
//	static void CINFO(const char * functionName, const char * varName, float value, const char * msg)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[INFO ]: ";
//		ss << functionName << " " << varName << " = " << value << " - " << msg << std::endl;
//		printf(ss.str().c_str());
//	}
//	static void CINFO(const char * functionName, const char * varName, int value, const char * msg)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[INFO ]: ";
//		ss << functionName << " " << varName << " = " << value << " - " << msg << std::endl;
//		printf(ss.str().c_str());
//	}
//	static void CINFO(const char * functionName, const char * msg1, const char * msg2)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[INFO ]: ";
//		ss << functionName << " - " << msg1 << " - " << msg2 << std::endl;
//		printf(ss.str().c_str());
//	}
//	static void CINFO(const char * functionName, const char * msg, int value)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[INFO ]: ";
//		ss << functionName << " - " << msg << " - " << value << std::endl;
//		printf(ss.str().c_str());
//	}
//	static void CINFO(const char * functionName, const char * msg)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[INFO ]: ";
//		ss << functionName << " - " << msg << std::endl;
//		printf(ss.str().c_str());
//	}
//	static void CINFO(const char * msg)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[INFO ]: ";
//		ss << msg << std::endl;
//		printf(ss.str().c_str());
//	}
//
//	static void CERROR(const char * functionName, const char * varName, const char * value, const char * msg)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[ERROR]: ";
//		ss << functionName << " " << varName << " = " << value << " - " << msg << std::endl;
//		printf(ss.str().c_str());
//	}
//	static void CERROR(const char * functionName, const char * varName, float value, const char * msg)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[ERROR]: ";
//		ss << functionName << " " << varName << " = " << value << " - " << msg << std::endl;
//		printf(ss.str().c_str());
//	}
//	static void CERROR(const char * functionName, const char * varName, int value, const char * msg)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[ERROR]: ";
//		ss << functionName << " " << varName << " = " << value << " - " << msg << std::endl;
//		printf(ss.str().c_str());
//	}
//	static void CERROR(const char * functionName, const char * msg1, const char * msg2)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[ERROR]: ";
//		ss << functionName << " - " << msg1 << " - " << msg2 << std::endl;
//		printf(ss.str().c_str());
//	}
//	static void CERROR(const char * functionName, const char * msg)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[ERROR]: ";
//		ss << functionName << " - " << msg << std::endl;
//		printf(ss.str().c_str());
//	}
//	static void CERROR(const char * msg)
//	{
//		std::stringstream ss;
//		ss << CDATE_TIME() << "[ERROR]: ";
//		ss << msg << std::endl;
//		printf(ss.str().c_str());
//	}
//
//}