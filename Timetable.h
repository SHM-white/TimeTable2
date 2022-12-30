#pragma once

#include<string>
#include<string_view>
#include<format>
#include<vector>
#include "resource.h"

//WindowSettings类，用于保存设置
class WindowSettings {
public:
	int iWindowHeight{ 120 };
	int iWindowWeight{ 250 };
	int iWindowX{ 20 };
	int iWindowY{ 20 };
	int iFontSize{ 25 };
	int iLineDistance{ 25 };
	int iLessonInLine{ 2 };
	std::string sFontName{ "黑体" };
	std::vector<std::string> sTextFormat;
	std::string sLessonNull{ "无" };
};
//Lesson类，用来保存课程信息，包括名称，开始时间，结束时间
class Lesson {
public:
	Lesson(std::string s, int b, int e) :sName{ s }, iBeginTime{ b }, iEndTime{ e } {}
	Lesson(Lesson& OtherLesson)
	{
		iBeginTime = OtherLesson.iBeginTime;
		iEndTime = OtherLesson.iEndTime;
		sName = OtherLesson.sName;
	}
	Lesson() {}
	std::string mGetName() {return sName;}
	int mGetBeginTime() {return iBeginTime;}
	int mGetEndTime() {return iEndTime;}
	int mSetValue(std::string_view s, int b, int e) {
		iBeginTime = b;
		iEndTime = e;
		sName = s;
		return 0;
	}
private:
	int iBeginTime{};
	int iEndTime{};
	std::string sName{};
};
//TimeTable类，函数全在这，需初始化配置文件路径
class TimeTable {
public:
	TimeTable() = delete;
	TimeTable(std::string path) : mConfig_path{path} {};
	int mAddLesson(std::string Days,std::string Lesson,std::string sBegin,std::string sEnd);
	int mAddMoreInfo(std::string Days, std::string Info);
	int mGetLesson(std::vector<std::string>& input);
	int mGetAbout(std::string& input);
	int mGetTodayMoreInfo(std::vector<std::string>& input);
	int mGetWindowSettings(WindowSettings& windowsettings);
	int mGetTextItem(std::string Item, std::string& input);
	std::string mGetCurrentLesson(std::string& LessonNull);
	std::string mGetCurrentTime(std::string TextFormat);	
private:
	Lesson CurrentLesson;
	std::string mConfig_path;
	int mTimeToMin(int input);
};
