﻿
#include "Timetable.h"
#include <Windows.h>
#include <fstream>
#include <vector>
#include <strsafe.h>
#include "include\json\json.h"
#include<format>
#include "CSVEditor.h"

//TimeTable类的实现函数
int TimeTable::mAddLesson(const std::string& week, const std::string& Lesson, const std::string& sBegin, const std::string& sEnd)
{
    return mAddLesson(week, Lesson, sBegin, sEnd, mLessonInfoPath);
}
int TimeTable::mAddLesson(const std::string& week, const std::string& Lesson, const std::string& sBegin, const std::string& sEnd, const std::string& TargetFilePath)
{
    if (!((bool)week.size() && (bool)Lesson.size() && (bool)sBegin.size() && (bool)sEnd.size() && (bool)TargetFilePath.size())) {
        return 0;
    }
    Json::Reader reader;
    Json::Value root;
    Json::StyledWriter sw;
    std::fstream os;
    Json::Value Current;
    os.open(TargetFilePath, std::ios::in);
    if (!os.is_open()) {
        os.close();
        os.clear();
        return 0;
    }
    if (!reader.parse(os, root)) {
        os.close();
        os.clear();
        return 0;
    }
    os.close();
    os.clear();
    os.open(TargetFilePath, std::ios::out | std::ios::trunc);
    Current.append(Lesson);
    Current.append(sBegin);
    Current.append(sEnd);
    root[week]["Lessons"].append(Current);
    os.seekp(std::ios::beg);
    os << sw.write(root);
    os.close();
    os.clear();
    return 1;
}

int TimeTable::mAddMoreInfo(const std::string& Days, const std::string& Info)
{
    std::fstream os;
    os.open(mLessonInfoPath, std::ios::out | std::ios::in);
    if (!os.is_open()) {
        return 0;
    }
    Json::Reader reader;
    Json::Value root;
    Json::StyledWriter sw;
    reader.parse(os, root);
    root[Days]["Infos"].append(Info);
    os.seekp(std::ios::beg);
    os << sw.write(root);
    os.close();
    os.clear();
    return 0;
}

int TimeTable::mTimeToMin(int input)
{
    return (input-input%100)/100*60+input%100;
}

std::string TimeTable::mReplacePath(const std::string& Path)
{
    std::string old = mLessonInfoPath;
    mLessonInfoPath = Path;
    return old;
}

int TimeTable::mGetLesson(std::vector<std::string>& input)
{
    Json::Reader reader;
    Json::Value root;
    std::ifstream in(mLessonInfoPath, std::ios::in);
    if (!in.is_open())
    {
        return 0;
    };
    std::string Days[]{ "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
    if (reader.parse(in, root)) {
        for (auto a : Days) {
            const Json::Value Lessons = root[a]["Lessons"];
            for (unsigned int i = 0; i < Lessons.size(); ++i) {
                std::string result{ a + "   " + Lessons[i][0].asString() + "   " + Lessons[i][1].asString() + "   " + Lessons[i][2].asString() };
                input.push_back(result);
            }
        }
    }
    in.close();
    in.clear();
    return 1;
}

int TimeTable::mGetTodayMoreInfo(std::vector<std::string>& input)
{
    std::ifstream in(mLessonInfoPath, std::ios::in);
    if (!in.is_open())
    {
        return 0;
    };
    Json::Reader reader;
    Json::Value root;
    std::string week{ mGetCurrentTime("%a") };
    if (reader.parse(in, root)) {
        const Json::Value Infos = root[week]["Infos"];
        for (unsigned int i = 0; i < Infos.size(); ++i) {
            input.push_back(Infos[i].asString());
        }
    }
    in.close();
    return 1;
}

std::string TimeTable::mGetCurrentLesson(std::string& LessonNull)
{
    std::string timeCurrentTime{ mGetCurrentTime("%H%M") };
    int iCurrentTime = mTimeToMin(atoi(timeCurrentTime.c_str()));
    if (!((CurrentLesson.mGetBeginTime() <= iCurrentTime) && (iCurrentTime <= CurrentLesson.mGetEndTime()))) {
        Json::Reader reader;
        Json::Value root;
        std::ifstream in(mLessonInfoPath, std::ios::in);
        if (!in.is_open())
        {
            return std::string(std::format("找不到{}",mLessonInfoPath));
        };
        if (reader.parse(in, root)) {
            std::string week{ mGetCurrentTime("%a") };
            const Json::Value Lessons = root[week]["Lessons"];
            for (unsigned int i = 0; i < Lessons.size(); ++i) {
                std::string sBeginTime = Lessons[i][1].asString();
                std::string sEndTime = Lessons[i][2].asString();
                int iBeginTime = mTimeToMin(atoi(sBeginTime.c_str()));
                int iEndTime = mTimeToMin(atoi(sEndTime.c_str()));
                if ((iBeginTime <= iCurrentTime) && (iEndTime >= iCurrentTime)) {
                    in.close();
                    CurrentLesson.mSetValue(Lessons[i][0].asString(), iBeginTime, iEndTime);
                    return CurrentLesson.mGetName();
                }
            }
            CurrentLesson.mSetValue(LessonNull, 0, 0);
        }
        in.close();
        in.clear();
    }
    return CurrentLesson.mGetName();
}

std::string TimeTable::mGetCurrentTime(const std::string& TextFormat)
{
    char tmp[256];
    tm structm;
    mGetCurrentTime(structm);
    strftime(tmp, sizeof(tmp), TextFormat.c_str(), &structm);
    return std::string(tmp);
}

int TimeTable::mImportLessonsFromCsv(const std::string& path, const std::string& TargetFileName)
{
    CSVEditor CsvEditor{ path };
    if (CsvEditor.mGetCsvData()) {
        for (int i{ 0 }; i < CsvEditor.mGetLineCount(); i++) {
            mAddLesson(CsvEditor[i][0], CsvEditor[i][1], CsvEditor[i][2], CsvEditor[i][3],TargetFileName);
        }
        return 1;
    }
    return 0;
}
const std::string& TimeTable::mGetLessonInfoPath()
{
    return mLessonInfoPath;
}
int TimeTable::mGetCurrentTime(tm& tmTime)
{
    time_t timep;
    time(&timep);
    localtime_s(&tmTime, &timep);
    return 0;
}
std::string TimeTable::mGetCountDown(tm tmIn, const std::string& TimeFormat)
{
    tm tmCurrent;
    time_t timeIn{ mktime(&tmIn) };
    time_t timeCurrent;
    time(&timeCurrent);
    timeIn -= timeCurrent;
    if (timeIn < 0) {
        timeIn = 0;
    }
    gmtime_s(&tmIn, &timeIn);
    //mGetCurrentTime(tmCurrent);
    /*tmIn.tm_year -= tmCurrent.tm_year;
    tmIn.tm_mon -= tmCurrent.tm_mon;
    tmIn.tm_mday -= tmCurrent.tm_mday;
    tmIn.tm_hour -= tmCurrent.tm_hour;
    tmIn.tm_min -= tmCurrent.tm_min;
    tmIn.tm_sec -= tmCurrent.tm_sec;
    if (tmIn.tm_sec < 0) {
        tmIn.tm_sec += 60;
        tmIn.tm_min -= 1;
    }
    if (tmIn.tm_min < 0) {
        tmIn.tm_min += 60;
        tmIn.tm_hour -= 1;
    }
    if (tmIn.tm_hour < 0) {
        tmIn.tm_hour += 24;
        tmIn.tm_mday -= 1;
    }
    if (tmIn.tm_mday < 0) {
        tmIn.tm_mday += 30;
        tmIn.tm_mon -= 1;
    }
    if (tmIn.tm_mon < 0) {
        tmIn.tm_mon += 12;
        tmIn.tm_year -= 1;
    }
    if (tmIn.tm_year < 0) {
        tmIn = {0};
    }*/
    
    char tmp[256];
    StringCbPrintf(tmp, sizeof(tmp), TimeFormat.c_str(), tmIn.tm_mon, tmIn.tm_mday-1, tmIn.tm_hour, tmIn.tm_min, tmIn.tm_sec);
    //strftime(tmp, sizeof(tmp), TimeFormat.c_str(), &tmIn);
    return tmp;
}