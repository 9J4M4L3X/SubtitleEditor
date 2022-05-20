#include "Time.h"

Time::Time(Time* t)
{
	h_ = t->getHours();
	m_ = t->getMinutes();
	s_ = t->getSeconds();
	ms_ = t->getMiliSecs();
}

int Time::compareTimes(Time* t1, Time* t2)
{
	if (t1->h_ > t2->h_) return 1;
	else if(t1->h_ < t2->h_) return -1;
	else {
		if (t1->m_ > t2->m_) return 1;
		else if (t1->m_ < t2->m_) return -1;
		else {
			if (t1->s_ > t2->s_) return 1;
			else if (t1->s_ < t2->s_) return -1;
			else {
				if (t1->ms_ > t2->ms_) return 1;
				else if (t1->ms_ < t2->ms_) return -1;
				else return 0;
			}
		}
	}
}

Time* Time::convertMiliSecsToTime(int ms)
{
	Time* time;
	if (ms < 0) return nullptr;
	int real_ms = ms % 1000;
	int s = ms / 1000;
	if (s > 59) {
		int m = s / 60;
		s = s % 60;
		if (m > 59) {
			int h = m / 60;
			m = m % 60;
			time = new Time(h, m, s, real_ms);
		}
		else 
			time = new Time(0, m, s, real_ms);
	}
	else 
		time = new Time(0, 0, s, real_ms);
	
	return time;
}

int Time::convertTimeToMiliSecs(Time* t)
{
	int ms = t->getHours() * 3600 * 1000 + t->getMinutes() * 60 * 1000 + t->getSeconds() * 1000 + t->getMiliSecs();
	return ms;
}

void Time::shiftTimeForward(int ms)
{
	int time_ms = this->convertTimeToMiliSecs(this) + ms;
	Time* time = this->convertMiliSecsToTime(time_ms);
	this->h_ = time->getHours();
	this->m_ = time->getMinutes();
	this->s_ = time->getSeconds();
	this->ms_ = time->getMiliSecs();
}

void Time::shiftTimeBackward(int ms)
{
	int time_ms = this->convertTimeToMiliSecs(this) - ms;
	Time* time = this->convertMiliSecsToTime(time_ms);
	this->h_ = time->getHours();
	this->m_ = time->getMinutes();
	this->s_ = time->getSeconds();
	this->ms_ = time->getMiliSecs();
}
