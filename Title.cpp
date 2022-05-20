#include "Title.h"

Title::~Title() {
	delete start_; 
	delete end_;
	id_ = 0; title_ = ""; 
	selected_ = false;
	delete[] styles_;
}

Title::Title(Title* tmp)
{
	this->start_ = tmp->start_;
	this->end_ = tmp->end_;
	this->id_ = tmp->id_;
	this->title_ = tmp->title_;
	this->selected_ = tmp->selected_;
	this->styles_ = tmp->styles_;
}

void Title::setStart(Time* st)
{
	start_->setHours(st->getHours());
	start_->setMinutes(st->getMinutes());
	start_->setSeconds(st->getSeconds());
	start_->setMiliSecs(st->getMiliSecs());
}

void Title::setEnd(Time* end)
{
	end_->setHours(end->getHours());
	end_->setMinutes(end->getMinutes());
	end_->setSeconds(end->getSeconds());
	end_->setMiliSecs(end->getMiliSecs());
}

int Title::getTitleLength() {
	return title_.size();
}

int Title::getNumOfStyles() { 
	int i = 0;
	while (styles_[i]) 
		i++; 
	return i;
}

Time* Title::getTime()
{
	int s_ms = start_->convertTimeToMiliSecs(start_);
	int e_ms = end_->convertTimeToMiliSecs(end_);
	int time_ms = e_ms - s_ms;
	Time* time_checker = new Time(0, 0, 0, 0);
	Time* title_time = new Time(0, 0, 0, 0);
	title_time = time_checker->convertMiliSecsToTime(time_ms);
	return title_time;
}

int Title::wordCount()
{
	int num_of_words = 0;
	int counter = 0;
	while (counter < title_.size()) {
		if (((title_[counter] == ' ') || (title_[counter] == '\n'))
			&& ((title_[counter + 1] != ' ') && (title_[counter + 1] != '\n'))) 
			num_of_words++;
		counter++;
	}
	return num_of_words;
}

void Title::addStyle(Style* st)
{
	int i = 0;
	if(!styles_){ 
		styles_ = new Style * [2];
		styles_[0] = new Style(st->getStart(), st->getEnd(), st->getType());
		styles_[1] = new Style(0, 0, none);
	}
	else {
		int num_of_styles = 0;
		if(styles_) num_of_styles = this->getNumOfStyles();
		while (styles_[i]->getStart() != styles_[i]->getEnd()) i++;
		
		//sledeci blok izvrsava alokaciju niza stilova
		if (i >= num_of_styles) {
			Style** tmp_styles = new Style * [num_of_styles + 2];
			
			int cnt = 0; 
			while (cnt < num_of_styles + 2) {
				tmp_styles[cnt] = new Style(0, 0, none);
				cnt++;
			}
			cnt = 0;
			while (styles_[cnt]) {
				if (styles_[cnt]->getStart() != styles_[cnt]->getEnd())
					tmp_styles[cnt] = styles_[cnt];
				else {
					tmp_styles[cnt] = st; cnt++;
					tmp_styles[cnt] = nullptr; cnt++;
					break;
				}
				cnt++;
			}

			cnt = 0;
			while (cnt < num_of_styles - 1) {
				delete[] styles_[cnt++];
			}
			delete[] styles_;

			styles_ = tmp_styles;
		}
		else {
			styles_[i] = new Style(st->getStart(), st->getEnd(), st->getType());
			styles_[i + 1] = nullptr;
		}
	}
}

void Title::removeTitleStyles()
{
	int cnt = 0;
	if (styles_[0]->getType() != none) {
		delete[] styles_;
		styles_ = new Style * [2];
		styles_[0] = new Style(0, 0, none);
		styles_[1] = nullptr;
	}
}

void Title::shiftTitleForward(int ms)
{
	start_->shiftTimeForward(ms);
	end_->shiftTimeForward(ms);
}

void Title::shiftTitleBackward(int ms)
{
	start_->shiftTimeBackward(ms);
	end_->shiftTimeBackward(ms);
}

