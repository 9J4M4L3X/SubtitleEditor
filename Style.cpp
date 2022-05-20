#include "Style.h"

bool Style::checkOverlap(Style* st)
{
	if ((this->start_index_ > st->getStart()) && (this->start_index_ < st->getEnd())) return true;
	if ((this->end_index_ > st->getStart()) && (this->end_index_ < st->getEnd())) return true;
	return false;
}
