
#include<random>

#include<sstream>
#include<ctime>
#include<iomanip>
#include "shlobj.h"

#include"Utils.h"

namespace Prizm
{
	namespace StrUtils
	{
		std::string UnicodeToAscii(wchar_t* pwstr)
		{
			std::wstring wstr(pwstr);
			return std::string(wstr.begin(), wstr.end());
		}

		std::wstring AsciiToUnicode(const char* pstr)
		{
			std::string str(pstr);
			return std::wstring(str.begin(), str.end());
		}

		std::string TimeStr::GetCurrentTimeAsString(void)
		{
			const std::time_t now = std::time(0);
			std::tm current_time;	// current time
			localtime_s(&current_time, &now);

			// YYYY-MM-DD_HH-MM-SS
			std::stringstream ss;
			ss << (current_time.tm_year + 1900) << "_"
				<< std::setfill('0') << std::setw(2) << current_time.tm_mon + 1 << "_"
				<< std::setfill('0') << std::setw(2) << current_time.tm_mday << "-"
				<< std::setfill('0') << std::setw(2) << current_time.tm_hour << "_"
				<< std::setfill('0') << std::setw(2) << current_time.tm_min << "_"
				<< std::setfill('0') << std::setw(2) << current_time.tm_sec;
			return ss.str();
		}

		std::string TimeStr::GetCurrentTimeAsStringWithBrackets(void)
		{
			return "[" + GetCurrentTimeAsString() + "]";
		}
	}

	namespace DirectoryUtils
	{
		std::string GetSpecialFolderPath(FolderType folder)
		{
			const KNOWNFOLDERID& folder_id = [&]()
			{
				switch (folder)
				{
				case PROGRAM_FILE:
					return FOLDERID_ProgramFiles;
				case APPDATA:
					return FOLDERID_RoamingAppData;
				case LOCAL_APPDATA:
					return FOLDERID_LocalAppData;
				case USER_PROFILE:
					return FOLDERID_Profile;
				case DOCUMENTS:
					return FOLDERID_Documents;
				}

				return FOLDERID_RoamingAppData;
			}();

			PWSTR destination_path = {};
			if (failed(SHGetKnownFolderPath(folder_id, 0, nullptr, &destination_path)))
				return "";

			return StrUtils::UnicodeToAscii(destination_path);
		}
	}

	namespace Utils
	{
		float RandF(float low, float high)
		{
			if (low > high)
			{	// swap params
				float tmp = low;
				low = high;
				high = tmp;
			}

			thread_local std::mt19937_64 engine(std::random_device{}());

			std::uniform_real_distribution<float> distribution(low, high);

			return distribution(engine);
		}

		int	RandI(int low, int high)
		{
			if (low > high)
			{	// swap params
				int tmp = low;
				low = high;
				high = tmp;
			}

			thread_local std::mt19937_64 engine(std::random_device{}());

			std::uniform_int_distribution<> distribution(low, high);

			return distribution(engine);
		}

		size_t RandU(size_t low, size_t high)
		{
			if (low > high)
			{	// swap params
				size_t tmp = low;
				low = high;
				high = tmp;
			}

			thread_local std::mt19937_64 engine(std::random_device{}());

			std::uniform_int_distribution<size_t> distribution(low, high);

			return distribution(engine);
		}
	}
}