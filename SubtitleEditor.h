#ifndef _SUBTITLEEDITOR_H_
#define _SUBTITLEEDITOR_H_

#include"Title.h"

using namespace std;

//lista stanja editora
struct Editor_states {
	Title** sub;
	struct Editor_states* next, * prev;
};

class SubtitleEditor {
public:
	SubtitleEditor();
	~SubtitleEditor();

	int loadSubtitles(string subtitles);
	string saveSubtitles();

	void addToSelection(int start, int end);
	void deselect();

	void boldText(int start, int end);
	void italicText(int start, int end);
	void underlineText(int start, int end);
	void removeStyle();

	void shiftForward(int ms);
	void shiftBackward(int ms);

	void insertSubtitle(int start, int length, string text);
	void deleteSubtitles();
	void merge(string subtitles);

	void fixLongLines(int max);
	void fixLongTime(int ms);
	void undo();//brise poslednji clan u listi ed_states_ i postavi subtitles_ na novi poslednji clan te liste
private:

	void addState(Title**);//ova funkcija doda tekuci niz titlova u listu stanja, poziva se na kraju svake funkcije editora
	int getNumOfTitles();//vraca broj ucitanih titlova

	Title** subtitles_;
	Editor_states* ed_states_;//lista stanja(titlova) u kojima se nalazi editor 
};
#endif // !_SUBTITLEEDITOR_H_

