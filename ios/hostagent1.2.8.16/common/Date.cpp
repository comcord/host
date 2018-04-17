#include <stdafx.h>
#include "Date.h"
#include "stdio.h"
#include "stdlib.h"


/*
int max(int i1, int i2)
{
	if(i1 >i2)
		return i1;
	return i2;
}*/

void Date::SetTimeT(time_t t)
{
	memcpy(&time_struct,  localtime(&t), sizeof(tm));
}
Date::Date()
{
	time_t clock;
	time(&clock);
	memcpy(&time_struct,  localtime(&clock), sizeof(tm));
}
Date::Date(time_t t)
{
	memcpy(&time_struct,  localtime(&t), sizeof(tm));
}

Date::Date(const Date &right)
{
	time_struct = right.time_struct;
}

Date::Date(const char* format, const char* str)
{
	memset(&time_struct, 0, sizeof(time_struct));
	if(strlen(str) == 0)
	{
		time_struct.tm_mday = 1;
		return;
	}

	if(strcmp(format, "yyyymmdd") == 0)
	{
		char year[5]; 
		char month[3]; 
		char day[3]; 
		memset(year, 0, sizeof(year));
		memset(month,0,  sizeof(month));
		memset(day, 0, sizeof(day));
		memcpy(year, str, 4);
		memcpy(month, str+4, 2);
		memcpy(month, str+6, 2);
		time_struct.tm_year = atoi(year)-1900;
		time_struct.tm_mon = atoi(month)-1;
		time_struct.tm_mday = atoi(day);
	}
	else if(strcmp(format, "yymmddhhmiss") == 0)
	{
		char year[3]; 
		char month[3]; 
		char day[3]; 
		char hour[3]; 
		char min[3]; 
		char sec[3]; 
		memset(year,0,  sizeof(year));
		memset(month,0,  sizeof(month));
		memset(day, 0, sizeof(day));
		memset(hour, 0, sizeof(hour));
		memset(min, 0, sizeof(min));
		memset(sec, 0, sizeof(sec));
		
		memcpy(year, str, 2);
		memcpy(month, str+2, 2);
		memcpy(day, str+4, 2);
		memcpy(hour, str+6, 2);
		memcpy(min, str+8, 2);
		memcpy(sec, str+10, 2);
		
		memset(&time_struct, 0, sizeof(time_struct));
		time_struct.tm_year = atoi(year)+100;
		time_struct.tm_mon = max(atoi(month)-1,0);
		time_struct.tm_mday = max(1,atoi(day));
		time_struct.tm_hour= atoi(hour);
		time_struct.tm_min= atoi(min);
		time_struct.tm_sec= atoi(sec);
	}
	//else throw();  
	
}

Date::~Date()
{
}

Date & Date::operator=(const Date &right)
{
	time_struct = right.time_struct;
	return *this;
}

int Date::getDay()
{
	return time_struct.tm_mday;
}

int Date::getHour()
{
	return time_struct.tm_hour;
}

int Date::getYear()
{
	return 1900+time_struct.tm_year;
}

int Date::getMonth()
{
	return time_struct.tm_mon+1;
}

int Date::getMinute()
{
	return time_struct.tm_min;
}

int Date::getSecond()
{
	return time_struct.tm_sec;
}

std::string Date::toString(const char* format)
{

	if(strcmp(format, "yyyymmddhh")==0)
	{
		char s[15];
		memset(s, 0, sizeof(s));
		sprintf(s, "%04u%02u%02u%02u", getYear(), getMonth(), 
			getDay(), getHour());
		return s;
	}
	else if(strcmp(format, "yyyymmdd")==0)
	{
		char s[9];
		memset(s, 0, sizeof(s));
		sprintf(s, "%04u%02u%02u", getYear(), getMonth(), getDay());
		return s;
	}
	else if(strcmp(format, "yyyy-mm-dd")==0)
	{
		char s[21];
		memset(s, 0, sizeof(s));
		sprintf(s, "%04u-%02u-%02u", getYear(), getMonth(),	getDay());
		return s;
	}
	else if(strcmp(format, "mmddhhmm")==0)
	{
		char s[9];
		memset(s, 0, sizeof(s));
		sprintf(s, "%02u%02u%02u%02u",  getMonth(), getDay(), getHour(),
			getMinute());
		return s;
	}
	else if(strcmp(format, "mmddhhmiss")==0)
	{
		char s[11];
		memset(s, 0, sizeof(s));
		sprintf(s, "%02u%02u%02u%02u%02u",  getMonth(), getDay(), getHour(),
			getMinute(), getSecond());
		return s;
	}
	else if(strcmp(format, "yyyymmddhhmi")==0)
	{
		char s[15];
		memset(s, 0, sizeof(s));
		sprintf(s, "%04u%02u%02u%02u%02u", getYear(), getMonth(), 
			getDay(), getHour(), getMinute());
		return s;
	}
	else if(strcmp(format, "yyyymmddhhmiss")==0)
	{
		char s[15];
		memset(s, 0, sizeof(s));
		sprintf(s, "%04u%02u%02u%02u%02u%02u", getYear(), getMonth(), 
			getDay(), getHour(), getMinute(), getSecond());
		return s;
	}
	else if(strcmp(format, "yyyy-mm-dd hh:mi:ss")==0)
	{
		char s[21];
		memset(s, 0, sizeof(s));
		sprintf(s, "%04u-%02u-%02u %02u:%02u:%02u", getYear(), getMonth(), 
			getDay(), getHour(), getMinute(), getSecond());
		return s;
	}
	else if(strcmp(format, "yyyy-mm-dd_hh:mi:ss")==0)
	{
		char s[21];
		memset(s, 0, sizeof(s));
		sprintf(s, "%04u-%02u-%02u_%02u:%02u:%02u", getYear(), getMonth(), 
			getDay(), getHour(), getMinute(), getSecond());
		return s;
	}
	else if(strcmp(format,"hhmmss")==0)
	{
		char s[21];
		memset(s, 0, sizeof(s));
		sprintf(s, "%02u%02u%02u", getHour(), getMinute(), getSecond());
		return s;
	}
	else if(strcmp(format,"hh:mi:ss")==0)
	{
		char s[21];
		memset(s, 0, sizeof(s));
		sprintf(s, "%02u:%02u:%02u", getHour(), getMinute(), getSecond());
		return s;
	}
	else if(strcmp(format,"dd")==0)
	{
		char s[21];
		memset(s, 0, sizeof(s));
		sprintf(s, "%02u", getDay());
		return s;
	}	

	return "";
}
/*
SYSTEMTIME Date::getSystemTime()
{
	
	SYSTEMTIME sysDate;
	sysDate.wYear = getYear();
	sysDate.wMonth=getMonth();
	sysDate.wDay=getDay();
	sysDate.wHour=getHour();
	sysDate.wMinute=getMinute();
	sysDate.wSecond=getSecond();
	sysDate.wMilliseconds= 0;
	
	return sysDate;
}*/
/*
time_t  Date::operator-(const Date &right)
{
	long timespan = (getTime() - right.getTime());
	return long;
}
*/
time_t Date::getTime()
{
	return mktime(&time_struct);
}

int Date::DayofWeek()
{
   struct tm *newtime;
   char tmpbuf[2];
   time_t lt;
   lt=time(NULL);
   newtime=localtime(&lt);
   strftime( tmpbuf, 2, "%w", newtime);
   return atoi(tmpbuf);
}
//
//time_t Date::getCurrentTime()
//{
//	time_t clock;
//	time(&clock);
//	return (U64)clock;
//}
//
//std::string Date::diffSpend(time_t pt)
//{
//	time_t clock;
//	time(&clock);
//	return FormatTime((unsigned long) long (  difftime(clock,pt) ) );
//}
//std::string Date::FormatTime(unsigned long long elapsed_time )
//{
//	char tmpbuf[200]={0};
//	
//	unsigned long long mint = elapsed_time/60;
//
//	if( mint > 0 )
//	{
//		elapsed_time = elapsed_time % 60 ;
//		unsigned long long hour = mint / 60;
//		if(hour > 0)
//		{
//			mint = mint % 60;
//			unsigned long long day = hour/24;
//			if( day > 0 )
//			{
//				hour = hour % 24;
//				unsigned long long year = day/365;
//				if(year > 0)
//				{
//					day = day % 365;
//					sprintf(tmpbuf,"%lld 年 %lld天 %lld 小时 %lld 分 %lld 秒.",
//						year,day, hour,mint,elapsed_time);
//				}
//				else
//				{
//					sprintf(tmpbuf,"%lld天 %lld 小时 %lld 分 %lld 秒.",
//						day, hour,mint,elapsed_time);
//				}
//			}
//			else
//			{
//				sprintf(tmpbuf,"%lld 小时 %lld 分 %lld 秒.",
//					hour,mint,elapsed_time);
//			}
//		}
//		else
//		{
//			sprintf(tmpbuf,"%lld 分 %lld 秒.",
//				mint,elapsed_time);
//		}
//	}
//	else
//	{
//		sprintf(tmpbuf,"%lld 秒.",elapsed_time);
//	}
//	return std::string(tmpbuf);
//}