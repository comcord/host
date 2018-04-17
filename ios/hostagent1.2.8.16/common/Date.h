
#ifndef __FILEREADEDCOUNTER_Date_H_
#define __FILEREADEDCOUNTER_Date_H_
 
#include <time.h>
//#include <sys/time.h>
#include <string>

//using namespace std;

class Date 
{
public:
	Date();
	Date(time_t t);
	Date(const Date &right);
	Date (const char* format, const char* str);

	~Date();

	Date & operator=(const Date &right);
	int getDay ();
	int getHour ();
	int getYear ();
	int getMonth ();
	int getMinute ();
	int getSecond ();
	int DayofWeek();
	std::string toString (const char* format);
	time_t getTime();
	void SetTimeT(time_t t);
	//static time_t getCurrentTime();
	///SYSTEMTIME getSystemTime();o
	//time_t operator-(const Date &right);
	//static std::string diffSpend(time_t pt);
	//static std::string FormatTime(unsigned long long elapsed_time );

private:
	tm time_struct;	
};

#endif
