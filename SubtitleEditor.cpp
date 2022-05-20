#include "SubtitleEditor.h"

SubtitleEditor::SubtitleEditor()
{
	subtitles_ = nullptr;
	ed_states_ = nullptr;
}

SubtitleEditor::~SubtitleEditor()
{
	delete [] subtitles_;
	delete[] ed_states_;
}

int SubtitleEditor::loadSubtitles(string subtitles)
{
	int cnt = 0, i = 0, NumOfTitles = 100;
	bool firstTitle = true;
	
	//u slucaju da je vec ucitana sekvenca prvo se obrise:
	if (subtitles_){
		int i = 0;
		delete[] subtitles_;
		while (subtitles_[i]) 
			subtitles_[i++] = nullptr;
		subtitles_ = nullptr;
	}
		
	while ((cnt < subtitles.size()) || (firstTitle == 1)) {
		//izadje iz while ako naidje na novi red(to bi bio drugi procitan)
		//prvi put udje u while mozda naidje na znak '\n', preko flag-a firstTitle koji se odmah postavi na false
		firstTitle = false;
		if (subtitles[cnt] == EOF) break;

		while ((subtitles[cnt] == ' ') || (subtitles[cnt] == '\n')) cnt++; //preskoci blanko znak i novi red
		
		//ovde udje da alocira memoriju i inicijalizuje niz, ako je prvi put usao u petlju
		if (!subtitles_) {
			subtitles_ = new Title * [NumOfTitles];
			int cnt3 = 0;
			while (cnt3 < NumOfTitles) {
				Time* tmp_time1 = new Time(0, 0, 0, 0); 
				Time* tmp_time2 = new Time(0, 0, 0, 0);
				Style** styles = new Style * [2];
				styles[0] = new Style(0, 0, none);
				styles[1] = (Style*)nullptr;
				subtitles_[cnt3++] = new Title(0, tmp_time1, tmp_time2, "", false, styles);
			}
		}

		int sub_id = 0;
		//ucita id
		while (subtitles[cnt] != '\n') {
			while (subtitles[cnt] == ' ') cnt++;
			if(subtitles[cnt + 1] == '\n')
				sub_id = (sub_id + subtitles[cnt] - '0');
			else sub_id = (sub_id + subtitles[cnt] - '0') * 10;
			cnt++;
		}
		if (sub_id < 1) return sub_id;
		subtitles_[i]->setId(sub_id); 

		while ((subtitles[cnt] == ' ') || (subtitles[cnt] == '\n'))	cnt++;

		int h, m, s, ms;
		h = (subtitles[cnt] - '0') * 10 + (subtitles[cnt + 1] - '0'); cnt += 3;
		m = (subtitles[cnt] - '0') * 10 + (subtitles[cnt + 1] - '0'); cnt += 3;
		s = (subtitles[cnt] - '0') * 10 + (subtitles[cnt + 1] - '0'); cnt += 3;
		ms = (subtitles[cnt] - '0') * 100 + (subtitles[cnt + 1] - '0') * 10 + (subtitles[cnt + 2] - '0'); 
		cnt += 8;
		
		if ((h < 0) || (h > 99)) return subtitles_[i]->getId();
		if ((m < 0) || (m > 59)) return subtitles_[i]->getId();
		if ((s < 0) || (s > 59)) return subtitles_[i]->getId();
		if ((ms < 0) || (ms > 999)) return subtitles_[i]->getId();

		Time* time1 = new Time(h, m, s, ms);
		subtitles_[i]->setStart(time1);
		
		h = (subtitles[cnt] - '0') * 10 + (subtitles[cnt + 1] - '0'); cnt += 3;
		m = (subtitles[cnt] - '0') * 10 + (subtitles[cnt + 1] - '0'); cnt += 3;
		s = (subtitles[cnt] - '0') * 10 + (subtitles[cnt + 1] - '0'); cnt += 3;
		ms = (subtitles[cnt] - '0') * 100 + (subtitles[cnt + 1] - '0') * 10 + (subtitles[cnt + 2] - '0');
		cnt += 3;

		if ((h < 0) || (h > 99)) return subtitles_[i]->getId();
		if ((m < 0) || (m > 59)) return subtitles_[i]->getId();
		if ((s < 0) || (s > 59)) return subtitles_[i]->getId();
		if ((ms < 0) || (ms > 999)) return subtitles_[i]->getId();

		Time* time2 = new Time(h, m, s, ms);
		subtitles_[i]->setEnd(time2);

		//provera da li je krajnje vreme titla vece od pocetnog
		Time* time_check_object = new Time(0, 0, 0, 0);
		time_check_object->compareTimes(time1, time2);
		int correct_interval = time1->compareTimes(time1, time2);
		if (correct_interval != -1) return subtitles_[i]->getId();

		//treba proveriti da li id ide u rastucem redosledu sa razlikom 1, i da li vremena idu rastuce
		int correct_times;
		if (i != 0) {
			correct_times = time_check_object->compareTimes(subtitles_[i]->getStart(),subtitles_[i-1]->getEnd());
			if (correct_times != 1) return subtitles_[i]->getId();
			int id_difference = subtitles_[i]->getId() - subtitles_[i - 1]->getId();
			if (id_difference != 1) return subtitles_[i]->getId();
		}

		while ((subtitles[cnt] == ' ') || (subtitles[cnt] == '\n')) cnt++;
		
		string title;
		//citanje titla
		while (true) {
			//ucitavanje stilova, ako naleti na <b> || <i> || <u>
			if((subtitles[cnt] == '<') && (subtitles[cnt + 1] == 'b') && (subtitles[cnt + 2] == '>')){
				//iscitamo tekst dok ne stignemo do </b> sto ce nam oznacavati poziciju kraja stila
				//a onda pravimo obj. klase Style
				int start_index, end_index;
				start_index = title.length();//svi karakteri ciji su indeksi >= start_index se stilizuju
				cnt += 3;
				//sledeci while cita dok ne dodje do kraja stilizovanog teksta
				while (!((subtitles[cnt] == '<') && (subtitles[cnt + 1] == '/') 
					&& (subtitles[cnt + 2] == 'b') && (subtitles[cnt + 3] == '>'))) {
					if ((subtitles[cnt] == '<') && (subtitles[cnt + 1] == 'i') && (subtitles[cnt + 2] == '>')) {
						return subtitles_[i]->getId();
					}
					if ((subtitles[cnt] == '<') && (subtitles[cnt + 1] == '/')
						&& (subtitles[cnt + 2] == 'i') && (subtitles[cnt + 3] == '>')) {
						return subtitles_[i]->getId();
					}
					if ((subtitles[cnt] == '<') && (subtitles[cnt + 1] == '/')
						&& (subtitles[cnt + 2] == 'u') && (subtitles[cnt + 3] == '>')) {
						return subtitles_[i]->getId();
					}
					if ((subtitles[cnt] == '<') && (subtitles[cnt + 1] == 'u') && (subtitles[cnt + 2] == '>')) {
						return subtitles_[i]->getId();
					}
					if ((subtitles[cnt] == '<') && (subtitles[cnt + 1] == 'b') && (subtitles[cnt + 2] == '>')) {
						return subtitles_[i]->getId();
					}
					title.push_back(subtitles[cnt++]); 
				}
				end_index = title.length() + 3;//svi karakteri ciji su indeksi manji od end_index se stilizuju
				Style* st = new Style(start_index, end_index, bold);
				subtitles_[i]->addStyle(st);
				cnt += 4;//pozicioniramo se na sledeci karakter nakon oznake za kraj stila
			}
			if ((subtitles[cnt] == '<') && (subtitles[cnt + 1] == 'i') && (subtitles[cnt + 2] == '>')) {
				//iscitamo tekst dok ne stignemo do </b> sto ce nam oznacavati poziciju kraja stila
				//a onda pravimo obj. klase Style
				int start_index, end_index;
				start_index = title.length();//svi karakteri ciji su indeksi >= start_index se stilizuju
				cnt += 3;
				//sledeci while cita dok ne dodje do kraja stilizovanog teksta
				while (!((subtitles[cnt] == '<') && (subtitles[cnt + 1] == '/')
					&& (subtitles[cnt + 2] == 'i') && (subtitles[cnt + 3] == '>')))
				{	
					if ((subtitles[cnt] == '<') && (subtitles[cnt + 1] == 'b') && (subtitles[cnt + 2] == '>')) 
						return subtitles_[i]->getId();
					
					if ((subtitles[cnt] == '<') && (subtitles[cnt + 1] == '/')
						&& (subtitles[cnt + 2] == 'b') && (subtitles[cnt + 3] == '>')) 
						return subtitles_[i]->getId();
					
					if ((subtitles[cnt] == '<') && (subtitles[cnt + 1] == 'u') && (subtitles[cnt + 2] == '>')) 
						return subtitles_[i]->getId();
					
					if ((subtitles[cnt] == '<') && (subtitles[cnt + 1] == '/')
						&& (subtitles[cnt + 2] == 'u') && (subtitles[cnt + 3] == '>')) 
						return subtitles_[i]->getId();
					
					if ((subtitles[cnt] == '<') && (subtitles[cnt + 1] == 'i') && (subtitles[cnt + 2] == '>')) 
						return subtitles_[i]->getId();
					
					title.push_back(subtitles[cnt++]);
				}
				end_index = title.length() + 3;//svi karakteri ciji su indeksi manji od end_index se stilizuju
				Style* st = new Style(start_index, end_index, italic);
				subtitles_[i]->addStyle(st);
				cnt += 4;//pozicioniramo se na sledeci karakter nakon oznake za kraj stila
			}
			if ((subtitles[cnt] == '<') && (subtitles[cnt + 1] == 'u') && (subtitles[cnt + 2] == '>')) {
				//iscitamo tekst dok ne stignemo do </b> sto ce nam oznacavati poziciju kraja stila
					//a onda pravimo obj. klase Style
				int start_index, end_index;
				start_index = title.length();//svi karakteri ciji su indeksi >= start_index se stilizuju
				cnt += 3;
				//sledeci while cita dok ne dodje do kraja stilizovanog teksta
				while (!((subtitles[cnt] == '<') && (subtitles[cnt + 1] == '/')
					&& (subtitles[cnt + 2] == 'i') && (subtitles[cnt + 3] == '>')))
				{
					if ((subtitles[cnt] == '<') && (subtitles[cnt + 1] == 'b') && (subtitles[cnt + 2] == '>'))
						return subtitles_[i]->getId();

					if ((subtitles[cnt] == '<') && (subtitles[cnt + 1] == '/')
						&& (subtitles[cnt + 2] == 'b') && (subtitles[cnt + 3] == '>'))
						return subtitles_[i]->getId();

					if ((subtitles[cnt] == '<') && (subtitles[cnt + 1] == 'i') && (subtitles[cnt + 2] == '>'))
						return subtitles_[i]->getId();

					if ((subtitles[cnt] == '<') && (subtitles[cnt + 1] == '/')
						&& (subtitles[cnt + 2] == 'i') && (subtitles[cnt + 3] == '>'))
						return subtitles_[i]->getId();

					if ((subtitles[cnt] == '<') && (subtitles[cnt + 1] == 'u') && (subtitles[cnt + 2] == '>'))
						return subtitles_[i]->getId();

					title.push_back(subtitles[cnt]); cnt++;
				}
				end_index = title.length() + 3;//svi karakteri ciji su indeksi manji od end_index se stilizuju
				Style* st = new Style(start_index, end_index, underline);
				subtitles_[i]->addStyle(st);
				cnt += 4;//pozicioniramo se na sledeci karakter nakon oznake za kraj stila
			}
			title.push_back(subtitles[cnt]);
			if ((subtitles[cnt] == '\n') && (subtitles[cnt + 1] == '\n')) break;
			cnt++;
		}
		
		while(subtitles[cnt] == '\n') 
		cnt++;//preskace u novi prazan red

		subtitles_[i]->setTitle(title); 
		i++;
		//ako dostigne kapacitet alocirane memorije, alociramo jos lokacija za titlove
		if ((NumOfTitles - i) < 5) {
			int cnt3 = NumOfTitles;//broj unetih podataka
			NumOfTitles += 50;
			Title** temp_subs = new Title*[NumOfTitles];
			int cnt4 = 0;
			//u while-u inicijalizuje novi niz sa dodatno alociranim mestima
			while (cnt4 < NumOfTitles) {
				if(cnt4 < cnt3 - 1) {
					//inicijalizuje pomocni niz sa vec ucitanim nizom titlova
					int id_tmp = subtitles_[cnt4]->getId();
					Time* s = subtitles_[cnt4]->getStart();
					Time* e = subtitles_[cnt4]->getEnd();
					string t = subtitles_[cnt4]->getTitle();
					Style** st = subtitles_[cnt4]->getStyles();
					temp_subs[cnt4++] = new Title(id_tmp, s, e, t, false, st);
				}
				else {
					//u else grani inicijalizuje ostatak niza nulama i praznim stringom, bez stilova
					Time* tmp_time1 = new Time(0, 0, 0, 0);
					Time* tmp_time2 = new Time(0, 0, 0, 0);
					Style** styles = new Style * [2];
					styles[0] = new Style(0, 0, none);
					styles[1] = nullptr;
					temp_subs[cnt4++] = new Title(0, tmp_time1, tmp_time2, "", false, styles);
				}
			}
			delete[] subtitles_;//obrisemo stari niz
			subtitles_ = temp_subs;//postavimo stari niz da pokazuje na novi koji ima alocirano vise mesta
		}
	}

	this->addState(subtitles_);
	
	return -1;
}

string SubtitleEditor::saveSubtitles()
{
	string subs; int cnt = 0;
	while (subtitles_[cnt]->getTitle() != "") { 
		string id = to_string(subtitles_[cnt]->getId());

		string h;
		if(subtitles_[cnt]->getStart()->getHours() < 10)
			h.append("0" + to_string(subtitles_[cnt]->getStart()->getHours()));
		else
			h.append(to_string(subtitles_[cnt]->getStart()->getHours()));
		
		string m;
		if(subtitles_[cnt]->getStart()->getMinutes() < 10)
			m.append("0" + to_string(subtitles_[cnt]->getStart()->getMinutes()));
		else
			m.append(to_string(subtitles_[cnt]->getStart()->getMinutes()));
		
		string s;
		if(subtitles_[cnt]->getStart()->getSeconds() < 10) 
			s.append("0" + to_string(subtitles_[cnt]->getStart()->getSeconds()));
		else
			s.append(to_string(subtitles_[cnt]->getStart()->getSeconds()));
		
		string ms;
		if(subtitles_[cnt]->getStart()->getMiliSecs() < 10)
			ms.append("00" + to_string(subtitles_[cnt]->getStart()->getMiliSecs()));
		else if(subtitles_[cnt]->getStart()->getMiliSecs() < 100)
			ms.append("0" + to_string(subtitles_[cnt]->getStart()->getMiliSecs()));
		else
			ms.append(to_string(subtitles_[cnt]->getStart()->getMiliSecs()));


		subs.append(id + "\n" + h + ":" + m + ":" + s + "," + ms + " --> "); 

		string h1;
		if (subtitles_[cnt]->getEnd()->getHours() < 10)
			h1.append("0" + to_string(subtitles_[cnt]->getEnd()->getHours()));
		else
			h1.append(to_string(subtitles_[cnt]->getEnd()->getHours()));

		string m1;
		if (subtitles_[cnt]->getEnd()->getMinutes() < 10)
			m1.append("0" + to_string(subtitles_[cnt]->getEnd()->getMinutes()));
		else
			m1.append(to_string(subtitles_[cnt]->getEnd()->getMinutes()));
		
		string s1;
		if (subtitles_[cnt]->getEnd()->getSeconds() < 10)
			s1.append("0" + to_string(subtitles_[cnt]->getEnd()->getSeconds()));
		else
			s1.append(to_string(subtitles_[cnt]->getEnd()->getSeconds()));

		string ms1;
		if (subtitles_[cnt]->getEnd()->getMiliSecs() < 10)
			ms1.append("00" + to_string(subtitles_[cnt]->getEnd()->getMiliSecs()));
		else if (subtitles_[cnt]->getEnd()->getMiliSecs() < 100)
			ms1.append("0" + to_string(subtitles_[cnt]->getEnd()->getMiliSecs()));
		else
			ms1.append(to_string(subtitles_[cnt]->getEnd()->getMiliSecs()));

		subs.append(h1 + ":" + m1 + ":" + s1 + "," + ms1 + "\n");
		string title = subtitles_[cnt]->getTitle();
		Style** st = subtitles_[cnt]->getStyles();
		int num_of_styles = 0;
		if (st) num_of_styles = subtitles_[cnt]->getNumOfStyles();
		
		int cnt1 = 0;
		while (cnt1 < num_of_styles) {
			string style;
			if (!st[cnt1]) break;
			if (st[cnt1]->getType() == bold)  style = "<b>";
			if (st[cnt1]->getType() == underline)  style = "<u>";
			if (st[cnt1]->getType() == italic)  style = "<i>";
			if (st[cnt1]->getType() == none) {
				cnt1++; continue;
			}
			title.insert(st[cnt1]->getStart(), style);
			style.insert(1, "/");
			title.insert(st[cnt1]->getEnd(), style);
			cnt1++;
		}
		subs.append(title + "\n");
		cnt++;
	}
	return subs;
}

int SubtitleEditor::getNumOfTitles()
{
	int numOfTitles = 0, cnt = 0;
	while (subtitles_[cnt]->getTitle() != "") {
		numOfTitles++; cnt++;
	}
	return numOfTitles;
}

void SubtitleEditor::addToSelection(int start, int end)
{
	if (start < 1) start = 1;
	if (end > this->getNumOfTitles()) end = this->getNumOfTitles();

	int counter = start;
	while (counter <= end) {
		if (!(subtitles_[counter - 1]->checkSelection()))
			subtitles_[counter - 1]->select(true);
		counter++;
	}

	this->addState(subtitles_);
}

void SubtitleEditor::deselect() {
	int cnt = 0;
	while (cnt < this->getNumOfTitles()) {
		if (subtitles_[cnt]->checkSelection()) 
			subtitles_[cnt]->select(false);	
		cnt++;
	}

	this->addState(subtitles_);
}

void SubtitleEditor::boldText(int start, int end)
{
	int cnt = 0;
	while (subtitles_[cnt]->getTitle() != "") {
		int s = start, e = end;
		if (subtitles_[cnt]->checkSelection()) {
			if ((s < 0) || (s > (subtitles_[cnt]->getTitleLength() - 1))) 
				s = 0;
			if((e < 0) || (e > subtitles_[cnt]->getTitleLength() - 1)) 
				e = subtitles_[cnt]->getTitleLength() - 1;
			
			Style* style = new Style(s, e + 3, bold);
			Style** styles = subtitles_[cnt]->getStyles();
			bool overlap = false; 
			int i = 0;
			
			while (styles[i]) {
				overlap = style->checkOverlap(styles[i++]);
				if (overlap) break;
			}
			if (!overlap) subtitles_[cnt]->addStyle(style);
		}
		cnt++;
	}

	this->addState(subtitles_);
}

void SubtitleEditor::italicText(int start, int end)
{
	int cnt = 0;
	while (subtitles_[cnt]->getTitle() != "") {
		int s = start, e = end;
		if (subtitles_[cnt]->checkSelection()) {
			if ((s < 0) || (s > subtitles_[cnt]->getTitleLength() - 1))
				s = 0;
			if ((e < 0) || (e > subtitles_[cnt]->getTitleLength() - 1))
				e = subtitles_[cnt]->getTitleLength() - 1;
			Style* style = new Style(s, e + 3, italic);
			Style** styles = subtitles_[cnt]->getStyles();
			bool overlap = false;
			int i = 0;
			while (styles[i]) {
				overlap = style->checkOverlap(styles[i++]);
				if (overlap) break;
			}
			if (!overlap) subtitles_[cnt]->addStyle(style);
		}
		cnt++;
	}

	this->addState(subtitles_);
}

void SubtitleEditor::underlineText(int start, int end) {
	int cnt = 0;
	while (subtitles_[cnt]->getTitle() != "") {
		int s = start, e = end;
		if (subtitles_[cnt]->checkSelection()) {
			if ((s < 0) || (s > subtitles_[cnt]->getTitleLength() - 1))
				s = 0;
			if ((e < 0) || (e > subtitles_[cnt]->getTitleLength() - 1))
				e = subtitles_[cnt]->getTitleLength() - 1;
			Style* style = new Style(s, e + 3, underline);
			Style** styles = subtitles_[cnt]->getStyles();
			bool overlap = false;
			int i = 0;
			while (styles[i]) {
				overlap = style->checkOverlap(styles[i++]);
				if (overlap) break;
			}
			if (!overlap) subtitles_[cnt]->addStyle(style);
		}
		cnt++;
	}

	this->addState(subtitles_);
}

void SubtitleEditor::removeStyle()
{
	int cnt = 0;
	while (subtitles_[cnt]->getTitle() != "") {
		if (subtitles_[cnt]->checkSelection()) 
			subtitles_[cnt]->removeTitleStyles();
		cnt++;
	}

	this->addState(subtitles_);
}

void SubtitleEditor::shiftForward(int ms)
{
	int cnt = 0, flag = 0;
	while (subtitles_[cnt]->getTitle() != "") {
		if (subtitles_[cnt]->checkSelection()) {
			flag = 1;
			subtitles_[cnt]->shiftTitleForward(ms);			
		}
		cnt++;
	}
	if (!flag) {
		cnt = 0;
		while (subtitles_[cnt]->getTitle() != "") {
			subtitles_[cnt]->shiftTitleForward(ms);
			cnt++;
		}
	}

	this->addState(subtitles_);
}

void SubtitleEditor::shiftBackward(int ms)
{
	int cnt = 0, flag = 0;
	while (subtitles_[cnt]->getTitle() != "") {
		if (subtitles_[cnt]->checkSelection()) {
			flag = 1;
			subtitles_[cnt]->shiftTitleBackward(ms);
		}
		cnt++;
	}
	if (!flag) {
		cnt = 0;
		while (subtitles_[cnt]->getTitle() != "") {
			subtitles_[cnt]->shiftTitleBackward(ms);
			cnt++;
		}
	}

	this->addState(subtitles_);
}

void SubtitleEditor::insertSubtitle(int start, int length, string text)
{
	int old_num_of_titles = this->getNumOfTitles();

	//ubacuje titl sa pocetnim vremenom start, krajnje vreme start + length
	Time* start_time = new Time(0, 0, 0, 0);
	Time* end_time = new Time(0, 0, 0, 0);
	start_time = start_time->convertMiliSecsToTime(start);
	end_time = end_time->convertMiliSecsToTime(start + length);

	//tekst moze biti stilizovan, pa onda treba to proveriti
	Style** styles = new Style * [2];
	styles[0] = new Style(0, 0, none);
	styles[1] = nullptr;
	Title* tmp_title = new Title(0, start_time, end_time, "", false, styles);

	int cnt = 0, text_length = text.size();
	string title;
	bool error = false;
	
	//citanje teksta
	while (cnt < text_length) {
		if((text[cnt] == '<') && (text[cnt + 1] == 'b') && (text[cnt + 2] == '>')){
			int style_start = cnt;
			cnt += 3;
			while (!((text[cnt] == '<') && (text[cnt + 1] == '/') && (text[cnt + 2] == 'b') && (text[cnt + 3] == '>'))) {
				if ((text[cnt] == '<') && (text[cnt + 1] == 'i') && (text[cnt + 2] == '>'))
					error = true;
				if ((text[cnt] == '<') && (text[cnt + 1] == '/') && (text[cnt + 2] == 'i') && (text[cnt + 3] == '>'))
					error = true;
				if ((text[cnt] == '<') && (text[cnt + 1] == '/') && (text[cnt + 2] == 'u') && (text[cnt + 3] == '>'))
					error = true;
				if ((text[cnt] == '<') && (text[cnt + 1] == 'u') && (text[cnt + 2] == '>')) 
					error = true;
				if ((text[cnt] == '<') && (text[cnt + 1] == 'b') && (text[cnt + 2] == '>')) 
					error = true;
				
				if (cnt == text_length) {
					error = true;
					break;
				}
				title.push_back(text[cnt++]);
			}
			if (error)  break; 
			int style_end = title.size() + 3;
			Style* bold_style = new Style(style_start, style_end, bold);
			tmp_title->addStyle(bold_style);
			cnt += 4;
		}
		if ((text[cnt] == '<') && (text[cnt + 1] == 'i') && (text[cnt + 2] == '>')) {
			int style_start = cnt;
			cnt += 3;
			while (!((text[cnt] == '<') && (text[cnt + 1] == '/') && (text[cnt + 2] == 'i') && (text[cnt + 3] == '>'))) {
				if ((text[cnt] == '<') && (text[cnt + 1] == 'i') && (text[cnt + 2] == '>'))
					error = true;
				if ((text[cnt] == '<') && (text[cnt + 1] == '/') && (text[cnt + 2] == 'b') && (text[cnt + 3] == '>'))
					error = true;
				if ((text[cnt] == '<') && (text[cnt + 1] == '/') && (text[cnt + 2] == 'u') && (text[cnt + 3] == '>'))
					error = true;
				if ((text[cnt] == '<') && (text[cnt + 1] == 'u') && (text[cnt + 2] == '>'))
					error = true;
				if ((text[cnt] == '<') && (text[cnt + 1] == 'b') && (text[cnt + 2] == '>'))
					error = true;

				if (cnt == text_length) {
					error = true;
					break;
				}
				title.push_back(text[cnt++]);
			}
			if (error)  break;
			int style_end = title.size() + 3;
			Style* italic_style = new Style(style_start, style_end, italic);
			tmp_title->addStyle(italic_style);
			cnt += 4;
		}
		if ((text[cnt] == '<') && (text[cnt + 1] == 'u') && (text[cnt + 2] == '>')) {
			int style_start = cnt;
			cnt += 3;
			while (!((text[cnt] == '<') && (text[cnt + 1] == '/') && (text[cnt + 2] == 'u') && (text[cnt + 3] == '>'))) {
				if ((text[cnt] == '<') && (text[cnt + 1] == 'i') && (text[cnt + 2] == '>'))
					error = true;
				if ((text[cnt] == '<') && (text[cnt + 1] == '/') && (text[cnt + 2] == 'i') && (text[cnt + 3] == '>'))
					error = true;
				if ((text[cnt] == '<') && (text[cnt + 1] == '/') && (text[cnt + 2] == 'b') && (text[cnt + 3] == '>'))
					error = true;
				if ((text[cnt] == '<') && (text[cnt + 1] == 'u') && (text[cnt + 2] == '>'))
					error = true;
				if ((text[cnt] == '<') && (text[cnt + 1] == 'b') && (text[cnt + 2] == '>'))
					error = true;

				if (cnt == text_length) {
					error = true;
					break;
				}
				title.push_back(text[cnt++]);
			}
			if (error)  break;
			int style_end = title.size() + 3;
			Style* underline_style = new Style(style_start, style_end, underline);
			tmp_title->addStyle(underline_style);
			cnt += 4;
		}
		if (error) break;
		title.push_back(text[cnt++]);
	}

	Title** new_subs = new Title * [old_num_of_titles + 3];
	//pravimo novi niz(new_subs) koji ima umetnut titl
	if (!error) {
		Title* inserted = new Title(tmp_title);
		//subtitles_[cnt - 1]  - inserted - subtitles[cnt] => na vremenskoj osi ovako poredjani
		cnt = 0;
		int flag = 0;
		while (cnt <= this->getNumOfTitles()) {
			if(end_time->compareTimes(subtitles_[cnt]->getStart(), end_time) == 1){
				if (!flag) {
					new_subs[cnt] = new Title(inserted);
					new_subs[cnt]->setId(subtitles_[cnt]->getId());
					new_subs[cnt]->setTitle(title);
					if (end_time->compareTimes(start_time, subtitles_[cnt - 1]->getEnd()) == -1) {
						error = true;
						break;
					}
				}
				else {
					new_subs[cnt] = new Title(subtitles_[cnt - 1]);
					new_subs[cnt]->setId(new_subs[cnt]->getId() + 1);
				}
				if (error) break;
				flag = 1;
			}
			else new_subs[cnt] =  new Title(subtitles_[cnt]);
			if (error) break;
			cnt++;
		}
	}

	new_subs[cnt - 1] = new Title(subtitles_[cnt - 2]);
	new_subs[cnt - 1]->setId(new_subs[cnt - 1]->getId() + 1);

	//smestamo new_subs u subtitles_
	if (!error) {
		subtitles_ = new_subs;
		
		Style** styles1 = new Style * [2]; 
		
		styles1[0] = new Style(0, 0, none);
		styles1[1] = nullptr;
		
		Time* stime = new Time(0, 0, 0, 0), *etime = new Time(0, 0, 0, 0);
		
		Title* tmp_title1 = new Title(subtitles_[cnt - 1]->getId() + 1, stime, etime, "", false, styles1);
		
		subtitles_[cnt] = new Title(tmp_title1);
		subtitles_[cnt + 1] = nullptr;
	}

	this->addState(subtitles_);
}

void SubtitleEditor::deleteSubtitles()
{
	int cnt = 0, i = 0;
	int title_number = this->getNumOfTitles();
	Title** new_sub = new Title * [title_number];

	//ovde prodje kroz sve titlove i one koji nisu u selekciji ubaci u new_sub
	while (cnt < title_number) {
		if (!(subtitles_[cnt]->checkSelection())){
			new_sub[i] = subtitles_[cnt];
			if (new_sub[i]->getId() != 1) 
				new_sub[i]->setId(new_sub[i - 1]->getId() + 1);
			i++;
		}
		cnt++;
	}
	
	delete[] subtitles_;
	Title** help_sub = new Title * [i + 1];
	//realociramo sad new_sub da ima isto onoliko elemenata koliko je ubaceno u taj niz, tj. i
	cnt = 0;
	while (cnt < i) {
		help_sub[cnt] = new Title(new_sub[cnt]);
		cnt++;
	}
	Time* t1 = new Time(0, 0, 0, 0), * t2 = new Time(0, 0, 0, 0);
	Style** styles = new Style * [2]; styles[0] = new Style(0, 0, none); styles[1] = nullptr;
	Title* tmp = new Title(0, t1, t2, "", false, styles);
	help_sub[cnt] = new Title(tmp);

	delete[] new_sub;

	subtitles_ = help_sub;

	this->addState(subtitles_);
}

void SubtitleEditor::merge(string subtitles)
{
	//subtitles je nova sekvenca titlova koju treba nadograditi na prethodno ucitanu sekvencu
	int num1 = this->getNumOfTitles(), num1help = num1 + 1;
	string subtitles0 = this->saveSubtitles();//ovde cuva prethodno ucitane titlove
	//mora prvo da se obezbedi da id ide u korektnom redosledu inace load funkcija nece raditi kako treba
	int cnt = 0; string subt;
	//ovaj while prodje kroz subtitles i ucita ga u subt samo sa korigovanim id clanovima
	while (cnt < subtitles.size()) {
		string new_id = to_string(num1help);
		num1help++;
		subt.append(new_id);
		
		//preskoci id
		while (subtitles[cnt] != '\n')
			cnt++;
		
		//kad izadje iz while subtitles[cnt] = '\n'
		subt.push_back(subtitles[cnt]);
		cnt++;//ovde skace na novi red(vreme)

		//u while-u cita vreme
		while (subtitles[cnt] != '\n') 
			subt.push_back(subtitles[cnt++]);
		
		subt.push_back(subtitles[cnt]); //ucita '\n'
		cnt++;//skace na novi red(tekst)
		
		while ((subtitles[cnt] != '\n') || (subtitles[cnt + 1] != '\n')) 
			subt.push_back(subtitles[cnt++]);
		
		subt.push_back(subtitles[cnt]);//ubaci newline
		cnt++;//skoci na 2.newline
		subt.push_back(subtitles[cnt]);//ubaci newline
		cnt++;//skoci na id
	}

	string subtitles1 = subtitles0.append(subt);//dodajemo novi string na prethodno ucitani
	this->loadSubtitles(subtitles1);//ucita u subtitles_ novi titl koji je samo nadogradjen ovim drugim ucitanim

	this->addState(subtitles_);
}

void SubtitleEditor::fixLongLines(int max)
{
	/*
	Implementirati operaciju void SubtitleEditor::fixLongLines(int max);
	koja sve titlove u sekvenci (ili selekciji, ukoliko postoji)
	koji u barem jednom svom redu sadrže veći broj nebelih karaktera 
	od parametra max ispravlja tako što tekst titla prelama u veći broj redova 
	tako da svi redovi sadrže jednak ili manji broj karaktera od parametra max.
	Prelom se vrši na granici reči tako da u svakom redu (osim, eventualno, poslednjem) stane maksimalan mogući broj karaktera.
	Voditi računa da se razmaci između reči koje se prelamaju uklone.
	*/
	int cnt = 0;
	bool flag = false;
	int title_number = this->getNumOfTitles();
	while (cnt < title_number) {
		//ovaj while obradjuje samo ako ima neka selekcija
		if (subtitles_[cnt]->checkSelection()) {
			flag = true;
			string title = subtitles_[cnt]->getTitle();
			int char_index = 0, char_counter;
			while (char_index < title.size()) {
				char_counter = 0;//resetujemo broj slova u redu na nulu
				//bool shorten = false;//flag koji je true ako smo skratili red
				//bool lengthen = false;//ovaj flag je dualan sa shorten
				//sledeci while obradjuje samo jedan red titla
				while (title[char_index] != '\n') {
					//ako dodje do: (char_index == max) = true => npr: max = 13 => mora da se'\n'prelomi red  
					if (char_counter == max) {
						//shorten = true;
						if (title[char_index + 1] == ' ') {
							//moze da prelomi red odmah
							title[char_index + 1] == '\n';
						}
						else {
							//posto se max(char_counter) nasao unutar reci treba odseci tu rec u novi red
							//tako da idemo unazad dok ne dodjemo do blanko znaka
							//radimo dodelu newline na indeksu gde je blanko
							while (title[char_index] != ' ') char_index--;
							title[char_index] = '\n'; char_index--;//vratimo indeks jedan unazad da bi iskocio iz while
						}
					}
					
					/*
					//pokusaj produzenja reda do maksimalnog broja znakova, ne radi ako je usao u prvi if
					if ((title[char_index + 1] == '\n') && (char_counter < max) && !shorten) {
						//stigli smo do novog reda a recenica je kraca od maksimalne duzine	
						char_index++;
						title[char_index++] = ' ';//umesto novog reda ide blanko znak
						lengthen = true;
						bool in = false;//ovaj flag je true samo ako smo stigli do maksimuma nebelih znakova
						while (title[char_index] != '\n') {

							if (title[char_index] != ' ') char_counter++;
							
							if (char_counter == max) {
								in = true;
								if (title[char_index + 1] == ' ') {
									//moze da prelomi red odmah
									title[char_index + 1] == '\n';
								}
								else {
									//posto je na granici rec treba odseci tu rec u novi red
									//tako da idemo unazad dok ne dodjemo do blanko znaka
									//radimo dodelu newline na indeksu gde je blanko
									while (title[char_index] != ' ') char_index--;
									title[char_index] = '\n'; char_index--;//vratimo indeks jedan unazad da bi iskocio iz while
								}
							}
							if (!in && (title[char_index + 1] == '\n')) 
								title[char_index + 1] = ' ';
							char_index++;
							bool last = false;
							//ako je stigao do poslednjeg karaktera(a pritom nije stigao do max) vraca newline i izlazi iz while-a
							if (char_index == title.size() - 1) {
								title[char_index] = '\n';
								last = true;
								break;
							}
							if (last) break;
						}
					}*/
					
					if (title[char_index] != ' ') char_counter++;//broji znakove koji nisu razmak
					char_index++;
				}
				while (title[char_index] == '\n') 
					char_index++;
			}
			subtitles_[cnt]->setTitle(title);
		}
		cnt++;
	}

	if (!flag) {
		cnt = 0;
		while (cnt < title_number) {
			string title = subtitles_[cnt]->getTitle();
			int char_index = 0, char_counter;
			while (char_index < title.size()) {
				char_counter = 0;//resetujemo broj slova u redu na nulu
				//bool shorten = false;//flag koji je true ako smo skratili red
				//bool lengthen = false;//ovaj flag je dualan sa shorten
				
				//sledeci while obradjuje samo jedan red titla
				while (title[char_index] != '\n') {
					//bool last = false;
					//ako dodje do: (char_index == max) = true => npr: max = 13 => mora da se'\n'prelomi red  
					if (char_counter == max) {
						//shorten = true;
						if (title[char_index + 1] == ' ') {
							//moze da prelomi red odmah
							title[char_index + 1] == '\n';
						}
						else {
							//posto se max(char_counter) nasao unutar reci treba odseci tu rec u novi red
							//tako da idemo unazad dok ne dodjemo do blanko znaka
							//radimo dodelu newline na indeksu gde je blanko
							while (title[char_index] != ' ') char_index--;
							title[char_index] = '\n'; char_index--;//vratimo indeks jedan unazad da bi iskocio iz while
						}
					}

					//pokusaj produzenja reda do maksimalnog broja znakova, ne radi ako je usao u prvi if
					/*
					if ((title[char_index + 1] == '\n') && (char_counter < max) && !shorten && (char_index < title.size() - 2)) {
						//stigli smo do novog reda a recenica je kraca od maksimalne duzine	
						char_index++;
						title[char_index++] = ' ';//umesto novog reda ide blanko znak
						lengthen = true;
						bool in = false;//ovaj flag je true samo ako smo stigli do maksimuma nebelih znakova
						while (title[char_index] != '\n') {

							if (title[char_index] != ' ') char_counter++;

							if (char_counter == max) {
								in = true;
								if (title[char_index + 1] == ' ') {
									//moze da prelomi red odmah
									title[char_index + 1] == '\n';
								}
								else {
									//posto je na granici rec treba odseci tu rec u novi red
									//tako da idemo unazad dok ne dodjemo do blanko znaka
									//radimo dodelu newline na indeksu gde je blanko
									while (title[char_index] != ' ') char_index--;
									title[char_index] = '\n'; char_index--;//vratimo indeks jedan unazad da bi iskocio iz while
								}
							}
							//ovde udje samo ako nije dosao do max pa hoce da doda blanko,
							//ali to radi samo u slucaju da sledeci char jeste novi red
							if (!in && (title[char_index + 1] == '\n'))
								title[char_index + 1] = ' ';
							char_index++;
							
							//sledeci blok naredbi se zamenjuje uslovom u if -> char_index < title.size() - 2
							//(char_index < title.size() - 2) ovaj uslov radi samo ako je u pitanju titl sa jednim redom
							//ako ima vise od 1 reda onda mora sledeci blok da postoji da obezbedi newline na kraju da bi izasao iz petlje
							last = false;
							//ako je stigao do poslednjeg karaktera(a pritom nije stigao do max) vraca newline i izlazi iz while-a
							if (char_index == title.size() - 1) {
								title[char_index] = '\n';
								last = true;
								break;
							}
						}
					}
					*/
					//if (last) break;
					
					if (title[char_index] != ' ') char_counter++;//broji znakove koji nisu razmak
					char_index++;
				}
				while (title[char_index] == '\n')
					char_index++;
			}
			subtitles_[cnt]->setTitle(title);
			cnt++;
		}
	}
	
	this->addState(subtitles_);
}

void SubtitleEditor::fixLongTime(int ms)
{
	/*
	npr: ms = 1000
	subtitles_[1886]->getTime() = 2s 378ms moramo da podelimo na minimum 3 dela da bi bilo vreme manje od 1000ms
	sad znamo da je broj titlova 3 pa onda ukupan broj reci podelimo na 3 i smestamo redom u 3 nova titla:vremena i tekst
	te titlove ubacimo u nas niz pozivanjem funkcije insert sa odgovarajucim parametrima,
	tako da se prikazuju uzastopno bez vremenskih razmaka sa istom duzinom trajanja(eventualno neka ms razlika, ako nije deljivo)
	*/

	int cnt = 0;
	bool selection = false;
	int num_of_subs = this->getNumOfTitles();
	
	while (cnt < num_of_subs) {
		if (subtitles_[cnt]->checkSelection()) {
			selection = true;
			int sub_time_in_ms = (subtitles_[cnt]->getTime())->convertTimeToMiliSecs(subtitles_[cnt]->getTime());
			if (sub_time_in_ms > ms) {
				int sub_time_tmp = sub_time_in_ms;
				int num_of_new_titles = 1;

				while (sub_time_tmp > ms) {
					num_of_new_titles++;
					sub_time_tmp = sub_time_in_ms;
					sub_time_tmp /= num_of_new_titles;
				}

				string title = subtitles_[cnt]->getTitle();
				int word_count = subtitles_[cnt]->wordCount();
				int time_interval, time_interval_plus;
				int new_word_count, new_word_count_plus;

				new_word_count = word_count / num_of_new_titles;
				new_word_count_plus = word_count % num_of_new_titles;
		
				time_interval = sub_time_in_ms / num_of_new_titles;
				time_interval_plus = sub_time_in_ms % num_of_new_titles;
				
				Title** divided_titles = new Title * [num_of_new_titles];
				int i = 0;
				
				//pravimo niz titlova koji su dobijeni deljenjem na jednake vremenske intervale manje od ms => divided_titles
				while (i < num_of_new_titles) {
					if (i == num_of_new_titles - 1) {
						int start_id = subtitles_[cnt]->getId();
						Time* start_time = subtitles_[cnt]->getStart();
						Time* end_time = subtitles_[cnt]->getStart();

						int st_ms = start_time->convertTimeToMiliSecs(start_time);
						int ms_add_start = i * time_interval, ms_add_end = (i + 1) * time_interval + time_interval_plus;

						int ms_start = st_ms + ms_add_start, ms_end = st_ms + ms_add_end;

						start_time = start_time->convertMiliSecsToTime(ms_start);
						end_time = end_time->convertMiliSecsToTime(ms_end);

						Style** st = new Style * [2]; st[0] = new Style(0, 0, none); st[1] = nullptr;
						divided_titles[i] = new Title(start_id + i, start_time, end_time, "", false, st);
					}
					else {
						int start_id = subtitles_[cnt]->getId();
						Time* start_time = subtitles_[cnt]->getStart();
						Time* end_time = subtitles_[cnt]->getStart();

						int st_ms = start_time->convertTimeToMiliSecs(start_time);
						int ms_add_start = i * time_interval, ms_add_end = (i + 1) * time_interval;

						int ms_start = st_ms + ms_add_start, ms_end = st_ms + ms_add_end;

						start_time = start_time->convertMiliSecsToTime(ms_start);
						end_time = end_time->convertMiliSecsToTime(ms_end);

						Style** st = new Style * [2]; st[0] = new Style(0, 0, none); st[1] = nullptr;
						divided_titles[i] = new Title(start_id + i, start_time, end_time, "", false, st);
					}
					i++;
				}

				int char_index = 0, nofn_titles = 0; 
				i = 0;
				//deljenje teksta titla i smestanje u odgovarajuce elemente niza divided_titles
				while (char_index < subtitles_[cnt]->getTitleLength()) {
					int num_of_words = 0;
					if (nofn_titles < num_of_new_titles - 1) {
						string title_tmp;
						while (num_of_words < new_word_count + new_word_count_plus) {
							if (((title[char_index] == ' ') || (title[char_index] == '\n'))
								&& ((title[char_index + 1] != ' ') && (title[char_index + 1] != '\n')))
								num_of_words++;
							if (num_of_words != new_word_count + new_word_count_plus) title_tmp.push_back(title[char_index]);
							else title_tmp.push_back('\n');
							char_index++;
							if (char_index == subtitles_[cnt]->getTitleLength()) break;
						}
						divided_titles[i++]->setTitle(title_tmp);
					}
					else {
						string title_tmp;//ovde ce biti poslednji
						while (num_of_words < new_word_count) {
							if (((title[char_index] == ' ') || (title[char_index] == '\n'))
								&& ((title[char_index + 1] != ' ') && (title[char_index + 1] != '\n')))
								num_of_words++;
							if (num_of_words != new_word_count) title_tmp.push_back(title[char_index]);
							else title_tmp.push_back('\n');
							char_index++;
							if (char_index == subtitles_[cnt]->getTitleLength()) break;
						}
						divided_titles[i++]->setTitle(title_tmp);
					}
					nofn_titles++;
				}

				//napravimo novi niz titlova koji ce da sadrzi stari ali ima alocirana mesta za nove titlove 
				Title** new_sub = new Title * [num_of_subs - 1];
				int brojac = 0;
				while (brojac < num_of_subs - 1) {
					if(brojac < cnt) new_sub[brojac] = subtitles_[brojac];
					if (brojac >= cnt) {
						new_sub[brojac] = subtitles_[brojac + 1];//preskoci sadasnji titl, kao da ga obrise
						new_sub[brojac]->setId(new_sub[brojac]->getId() - 1);//smanji id da bi se poklopili
					}
					brojac++;
				}

				//inicijalizuje kraj niza nulama
				/*
				while (brojac < num_of_subs + num_of_new_titles - 1) {
					Time* ts = new Time(0, 0, 0, 0), * te = new Time(0, 0, 0, 0);
					Style** st = new Style * [2]; st[0] = new Style(0, 0, none); st[1] = nullptr;
					new_sub[brojac++] = new Title(0, ts, te, "", false, st);
				}
				*/
				
				//obrisemo stari subtitles_
				int counter2 = 0;
				delete[] subtitles_;
				while (subtitles_[counter2])
					subtitles_[counter2] = nullptr;
				subtitles_ = nullptr;

				subtitles_ = new Title * [num_of_subs];
				subtitles_ = new_sub; 
				
				Time* ts = new Time(0, 0, 0, 0), *te = new Time(0, 0, 0, 0);
				Style** st = new Style * [2]; st[0] = new Style(0, 0, none); st[1] = nullptr;

				subtitles_[num_of_subs - 1] = new Title(0, ts, te, "", false, st);
				
				/*
				Time* ts1 = new Time(0, 0, 0, 0), * te1 = new Time(0, 0, 0, 0);
				Style** st1 = new Style * [2]; st1[0] = new Style(0, 0, none); st1[1] = nullptr;

				subtitles_[num_of_subs] = new Title(0, ts1, te1, "", false, st1);

				*/
				
				i = 0;
				//i sada samo pozovemo insert za sve nove titlove
				while (i < num_of_new_titles) {
					int ms_start = (divided_titles[i]->getStart())->convertTimeToMiliSecs(divided_titles[i]->getStart());
					int ms_length = (divided_titles[i]->getEnd())->convertTimeToMiliSecs(divided_titles[i]->getTime());
					this->insertSubtitle(ms_start, ms_length, divided_titles[i]->getTitle());
					i++;
				}
			}
		}
		cnt++;
	}

	if (!selection) {
		cnt = 0;
		while (cnt < num_of_subs) {
			int sub_time_in_ms = (subtitles_[cnt]->getTime())->convertTimeToMiliSecs(subtitles_[cnt]->getTime());
			if (sub_time_in_ms > ms) {
				int sub_time_tmp = sub_time_in_ms;
				int num_of_new_titles = 1;

				while (sub_time_tmp > ms) {
					num_of_new_titles++;
					sub_time_tmp = sub_time_in_ms;
					sub_time_tmp /= num_of_new_titles;
				}

				string title = subtitles_[cnt]->getTitle();
				int word_count = subtitles_[cnt]->wordCount();
				int time_interval, time_interval_plus;
				int new_word_count, new_word_count_plus;

				new_word_count = word_count / num_of_new_titles;
				new_word_count_plus = word_count % num_of_new_titles;

				time_interval = sub_time_in_ms / num_of_new_titles;
				time_interval_plus = sub_time_in_ms % num_of_new_titles;

				Title** divided_titles = new Title * [num_of_new_titles];
				int i = 0;

				//pravimo niz titlova koji su dobijeni deljenjem na jednake vremenske intervale manje od ms => divided_titles
				while (i < num_of_new_titles) {
					if (i == num_of_new_titles - 1) {
						int start_id = subtitles_[cnt]->getId();
						Time* start_time = subtitles_[cnt]->getStart();
						Time* end_time = subtitles_[cnt]->getStart();

						int st_ms = start_time->convertTimeToMiliSecs(start_time);
						int ms_add_start = i * time_interval, ms_add_end = (i + 1) * time_interval + time_interval_plus;

						int ms_start = st_ms + ms_add_start, ms_end = st_ms + ms_add_end;

						start_time = start_time->convertMiliSecsToTime(ms_start);
						end_time = end_time->convertMiliSecsToTime(ms_end);

						Style** st = new Style * [2]; st[0] = new Style(0, 0, none); st[1] = nullptr;
						divided_titles[i] = new Title(start_id + i, start_time, end_time, "", false, st);
					}
					else {
						int start_id = subtitles_[cnt]->getId();
						Time* start_time = subtitles_[cnt]->getStart();
						Time* end_time = subtitles_[cnt]->getStart();

						int st_ms = start_time->convertTimeToMiliSecs(start_time);
						int ms_add_start = i * time_interval, ms_add_end = (i + 1) * time_interval;

						int ms_start = st_ms + ms_add_start, ms_end = st_ms + ms_add_end;

						start_time = start_time->convertMiliSecsToTime(ms_start);
						end_time = end_time->convertMiliSecsToTime(ms_end);

						Style** st = new Style * [2]; st[0] = new Style(0, 0, none); st[1] = nullptr;
						divided_titles[i] = new Title(start_id + i, start_time, end_time, "", false, st);
					}
					i++;
				}

				int char_index = 0, nofn_titles = 0;
				i = 0;
				//deljenje teksta titla i smestanje u odgovarajuce elemente niza divided_titles
				while (char_index < subtitles_[cnt]->getTitleLength()) {
					int num_of_words = 0;
					if (nofn_titles < num_of_new_titles - 1) {
						string title_tmp;
						while (num_of_words < new_word_count + new_word_count_plus) {
							if (((title[char_index] == ' ') || (title[char_index] == '\n'))
								&& ((title[char_index + 1] != ' ') && (title[char_index + 1] != '\n')))
								num_of_words++;
							if (num_of_words != new_word_count + new_word_count_plus) title_tmp.push_back(title[char_index]);
							else title_tmp.push_back('\n');
							char_index++;
							if (char_index == subtitles_[cnt]->getTitleLength()) break;
						}
						divided_titles[i++]->setTitle(title_tmp);
					}
					else {
						string title_tmp;//ovde ce biti poslednji
						while (num_of_words < new_word_count) {
							if (((title[char_index] == ' ') || (title[char_index] == '\n'))
								&& ((title[char_index + 1] != ' ') && (title[char_index + 1] != '\n')))
								num_of_words++;
							if (num_of_words != new_word_count) title_tmp.push_back(title[char_index]);
							else title_tmp.push_back('\n');
							char_index++;
							if (char_index == subtitles_[cnt]->getTitleLength()) break;
						}
						divided_titles[i++]->setTitle(title_tmp);
					}
					nofn_titles++;
				}

				//napravimo novi niz titlova koji ce da sadrzi stari ali ima alocirana mesta za nove titlove 
				Title** new_sub = new Title * [num_of_subs - 1];
				int brojac = 0;
				while (brojac < num_of_subs - 1) {
					if (brojac < cnt) new_sub[brojac] = subtitles_[brojac];
					if (brojac >= cnt) {
						new_sub[brojac] = subtitles_[brojac + 1];//preskoci sadasnji titl, kao da ga obrise
						new_sub[brojac]->setId(new_sub[brojac]->getId() - 1);//smanji id da bi se poklopili
					}
					brojac++;
				}

				//inicijalizuje kraj niza nulama
				/*
				while (brojac < num_of_subs + num_of_new_titles - 1) {
					Time* ts = new Time(0, 0, 0, 0), * te = new Time(0, 0, 0, 0);
					Style** st = new Style * [2]; st[0] = new Style(0, 0, none); st[1] = nullptr;
					new_sub[brojac++] = new Title(0, ts, te, "", false, st);
				}
				*/

				//obrisemo stari subtitles_
				int counter2 = 0;
				delete[] subtitles_;
				while (subtitles_[counter2])
					subtitles_[counter2] = nullptr;
				subtitles_ = nullptr;

				subtitles_ = new Title * [num_of_subs];
				subtitles_ = new_sub;

				Time* ts = new Time(0, 0, 0, 0), * te = new Time(0, 0, 0, 0);
				Style** st = new Style * [2]; st[0] = new Style(0, 0, none); st[1] = nullptr;

				subtitles_[num_of_subs - 1] = new Title(0, ts, te, "", false, st);

				/*
				Time* ts1 = new Time(0, 0, 0, 0), * te1 = new Time(0, 0, 0, 0);
				Style** st1 = new Style * [2]; st1[0] = new Style(0, 0, none); st1[1] = nullptr;

				subtitles_[num_of_subs] = new Title(0, ts1, te1, "", false, st1);

				*/

				i = 0;
				//i sada samo pozovemo insert za sve nove titlove
				while (i < num_of_new_titles) {
					int ms_start = (divided_titles[i]->getStart())->convertTimeToMiliSecs(divided_titles[i]->getStart());
					int ms_length = (divided_titles[i]->getEnd())->convertTimeToMiliSecs(divided_titles[i]->getTime());
					this->insertSubtitle(ms_start, ms_length, divided_titles[i]->getTitle());
					i++;
				}
			}
			cnt++;
		}
	}

	this->addState(subtitles_);
}

void SubtitleEditor::undo()
{
	//undo samo obrise poslednji element iz liste ed_states_ i postavi subtitles_ na titlove novog poslednjeg stanja
	Editor_states* curr = ed_states_;
	while (curr->next) curr = curr->next;
	
	//curr sad pokazuje na poslednji element u listi
	subtitles_ = curr->prev->sub;//vrati subtitles_ na prethodno stanje
	curr = curr->prev;
	delete curr->next->sub;
	curr->next->sub = nullptr;
	curr->next = nullptr;
}

void SubtitleEditor::addState(Title** sub)
{
	Editor_states* new_state = new Editor_states();

	new_state->sub = new Title * [this->getNumOfTitles() + 2];
	
	int cnt = 0;
	while (cnt < this->getNumOfTitles())
	{
		int id = sub[cnt]->getId();
		Time* ts = new Time(sub[cnt]->getStart()), * te = new Time(sub[cnt]->getEnd());
		string title = sub[cnt]->getTitle();
		
		Style** st = new Style*[sub[cnt]->getNumOfStyles()  + 1];
		int counter = 0;
		Style** tmp_st = sub[cnt]->getStyles();
		while (counter < sub[cnt]->getNumOfStyles()) {
			st[counter] = new Style(tmp_st[counter]->getStart(),
				tmp_st[counter]->getEnd(), tmp_st[counter]->getType());
			counter++;
		}
		st[counter] = nullptr;
		Title* tmp = new Title(id, ts, te, title, false, st);
		new_state->sub[cnt++] = new Title(tmp);
	}
	
	Style** styles1 = new Style * [2];

	styles1[0] = new Style(0, 0, none);
	styles1[1] = nullptr;

	Time* stime = new Time(0, 0, 0, 0), * etime = new Time(0, 0, 0, 0);

	Title* tmp_title1 = new Title(0, stime, etime, "", false, styles1);

	new_state->sub[this->getNumOfTitles()] = new Title(tmp_title1);
	new_state->sub[this->getNumOfTitles() + 1] = nullptr;
	new_state->next = nullptr; 
	new_state->prev = nullptr;

	Editor_states* curr_state = ed_states_;

	if (!ed_states_) {
		ed_states_ = new_state;
	}
	else {
		while (curr_state->next) 
			curr_state = curr_state->next;

		curr_state->next = new_state;
		new_state->prev = curr_state;
		curr_state = new_state; 
	}
}
