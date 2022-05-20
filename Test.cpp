// Ovde ukljuciti Vase .h fajlove
#include"SubtitleEditor.h"
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

int tests_passed = 0;

// Funkcija koja poredi Vas izlaz sa ocekivanim izlazom za odgovarajuci test primer
// Da biste lakse uocili gresku ukoliko Vas izlaz ne odgovara ocekivanom izlazu, uporediti fajlove na linku https://text-compare.com/
void checkOutput(string& subtitles, int test_number) {

	ifstream correct_output("output_" + to_string(test_number) + ".txt");
	stringstream ss_out;
	ss_out << correct_output.rdbuf(); correct_output.close();
	string correct_subtitles = ss_out.str();

	if (correct_subtitles != "" && correct_subtitles == subtitles) {
		cout << "Test " + to_string(test_number) + ": PASSED" << endl;
		tests_passed++;
	}
	else {
		cout << "Test " + to_string(test_number) + ": FAILED" << endl;
	}

}

void test(int test_number) {

	SubtitleEditor* editor = new SubtitleEditor;

	ifstream input("input.txt");
	stringstream ss;
	ss << input.rdbuf(); input.close();
	string subtitles = ss.str();

	ifstream input_8;
	stringstream ss_8;
	string subtitles_8;

	switch (test_number) {
	case 0:
		// MyExample
		editor->loadSubtitles(subtitles);
		subtitles = editor->saveSubtitles();
		break;
	case 1:
		// LOAD / SAVE
		editor->loadSubtitles(subtitles);
		subtitles = editor->saveSubtitles();
		break;
	case 2:
		// LOAD / ONE SELECTION / REMOVE STYLE / SAVE
		editor->loadSubtitles(subtitles);
		editor->addToSelection(1, 1000);
		editor->removeStyle();
		subtitles = editor->saveSubtitles();
		break;
	case 3:
		// LOAD / MULTIPLE SELECTIONS / ADD STYLE / SAVE
		editor->loadSubtitles(subtitles);
		editor->addToSelection(1, 1);
		editor->addToSelection(500, 500);
		editor->addToSelection(1500, 1510);
		editor->boldText(0, 100);
		subtitles = editor->saveSubtitles();
		break;
	case 4:
		// LOAD / SHIFT FORWARD / SAVE
		editor->loadSubtitles(subtitles);
		editor->shiftForward(1000);
		subtitles = editor->saveSubtitles();
		break;
	case 5:
		// LOAD / SHIFT BACKWARD / SAVE
		editor->loadSubtitles(subtitles);
		editor->shiftBackward(60000);
		subtitles = editor->saveSubtitles();
		break;
	case 6:
		// LOAD / INSERT SUBTITLE / SAVE
		editor->loadSubtitles(subtitles);
		editor->insertSubtitle(3334000, 2000, "This is sample text.\n");
		subtitles = editor->saveSubtitles();
		break;
	case 7:
		// LOAD / ONE SELECTION / DELETE SUBTITLES / SAVE
		editor->loadSubtitles(subtitles);
		editor->addToSelection(1000, 1100);
		editor->deleteSubtitles();
		subtitles = editor->saveSubtitles();
		break;
	case 8:
		// LOAD / MERGE / SAVE
		editor->loadSubtitles(subtitles);
		input_8.open("input_8_extra.txt");
		ss_8 << input_8.rdbuf(); input_8.close();
		subtitles_8 = ss_8.str();
		editor->merge(subtitles_8);
		subtitles = editor->saveSubtitles();
		break;
	case 9:
		// LOAD / FIX LONG LINES / SAVE
		editor->loadSubtitles(subtitles);
		editor->fixLongLines(42);
		subtitles = editor->saveSubtitles();
		break;
	case 10:
		// LOAD / ONE SELECTION / FIX LONG TIME / SAVE
		editor->loadSubtitles(subtitles);
		editor->addToSelection(100, 105);
		editor->fixLongTime(2750);
		subtitles = editor->saveSubtitles();
		break;
	case 11:
		// LOAD / SHIFT FORWARD / SHIFT BACKWARD / UNDO / SAVE
		editor->loadSubtitles(subtitles);
		editor->shiftForward(1000);
		editor->shiftBackward(500);
		editor->undo();
		subtitles = editor->saveSubtitles();
		break;
	}

	ofstream my_output("my_output_" + to_string(test_number) + ".txt");
	my_output << subtitles; my_output.close();

	checkOutput(subtitles, test_number);

	delete editor;
}

int main() {

	// Izmeniti elemente niza u zavisnosti od testova koje zelite da pokrenete!
	int active_tests[] = { 0 };
	int number_of_tests = sizeof(active_tests) / sizeof(int);

	cout << "============================================" << endl;
	cout << "TESTING SUBTITLE_EDITOR" << endl << endl;
	cout << "Executing tests: ";
	for (int i = 0; i < number_of_tests; i++) cout << active_tests[i] << (i < number_of_tests - 1 ? ", " : "");
	cout << endl;

	for (int i = 0; i < number_of_tests; i++) test(active_tests[i]);

	cout << "============================================" << endl;
	cout << "Tests passed count: " << tests_passed << endl;

	return 0;
}