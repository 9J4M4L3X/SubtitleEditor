#ifndef _TITLE_H_
#define _TITLE_H_

#include"Time.h"
#include"Style.h"

class Title {
public:
	Title(int id, Time* st, Time* end, string title, bool sel, Style** style) :
		id_(id), start_(st), end_(end), title_(title), selected_(sel), styles_(style){}
	~Title();

	Title(Title* tmp);
	void setId(int id) { id_ = id; }
	void setStart(Time* st);
	void setEnd(Time* end);
	void setTitle(string title) { title_ = title; }
	void select(bool sel) { selected_ = sel; }

	void addStyle(Style* st);//dodaje stil titlu
	void removeTitleStyles();//brise sve stilove
	void shiftTitleForward(int ms);
	void shiftTitleBackward(int ms);

	int getId() { return id_; }
	Time* getStart() { return start_; }
	Time* getEnd() { return end_; }
	string& getTitle() { return title_; }
	int getTitleLength();//broj karaktera u tekstu titla
	bool checkSelection() { return selected_; }//vraca true ako je selektovan
	Style** getStyles() { return styles_; }//vraca niz stilova
	int getNumOfStyles();//vraca broj stilova titla
	Time* getTime();//vraca vreme trajanja titla
	int wordCount();//vraca broj reci u recenici
private:
	int id_;
	Time* start_, * end_;
	string title_;
	bool selected_;
	Style** styles_;
};


#endif // !_TITLE_H_


