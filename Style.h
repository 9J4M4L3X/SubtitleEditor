#ifndef _STYLE_H_
#define _STYLE_H_

enum style { bold, underline, italic, none };

class Style {
public:
	
	Style(int si, int ei, style st):start_index_(si), end_index_(ei), type_(st){}
	~Style() { start_index_ = end_index_ = 0; type_ = none; }

	void setStart(int si) { start_index_ = si; }
	void setEnd(int ei) { end_index_ = ei; }
	void setType(style st) { type_ = st; }

	style getType() { return type_; }
	int getStart() { return start_index_; }
	int getEnd() { return end_index_; }
	bool checkOverlap(Style* st);//proverava da li se stilovi preklapaju, vraca true ako se preklapaju
private:
	int start_index_, end_index_;
	style type_;
};
#endif // !_STYLE_H_
