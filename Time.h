#ifndef _TIME_H_
#define _TIME_H_

#include<string>

using namespace std;
class Time {
public:
	Time(int h, int m, int s, int ms) :h_(h), m_(m) , s_(s), ms_(ms) {}
	~Time() {}

	Time(Time*);

	void setHours(int h) { h_ = h; }
	void setMinutes(int m) { m_ = m; }
	void setSeconds(int s) { s_ = s; }
	void setMiliSecs(int ms) { ms_ = ms; }

	int getHours() { return h_; }
	int getMinutes() { return m_; }
	int getSeconds() {return s_; }
	int getMiliSecs() { return ms_; }

	int compareTimes(Time* t1, Time* t2);//if t1 > t2 returns 1, if t1 < t2 returns -1, if t1 = t2 returns 0

	Time* convertMiliSecsToTime(int ms);
	int convertTimeToMiliSecs(Time*);
	void shiftTimeForward(int ms);
	void shiftTimeBackward(int ms);

private:
	int h_, m_, s_, ms_;
};
#endif // !_TIME_H_
