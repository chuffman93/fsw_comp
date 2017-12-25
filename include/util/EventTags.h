/*
 * EventTags.h
 *
 *  Created on: Dec 24, 2017
 *      Author: cyborg9
 */

#ifndef EVENTTAGS_H_
#define EVENTTAGS_H_

#include <map>
#include <string>

typedef std::pair<std::string, std::string> EventTag;

class EventTags: public std::map<std::string, std::string>{
public:
	EventTags operator+(const EventTag& tag){
		EventTags newtags;
		newtags.insert(this->begin(), this->end());
		newtags.insert(tag);
		return newtags;
	}
	void operator+=(const EventTag& tag){
		this->insert(tag);
	}
	EventTags operator+(const EventTags& tags2){
		EventTags newtags;
		newtags.insert(this->begin(), this->end());
		newtags.insert(tags2.begin(), tags2.end());
		return newtags;
	}
};



#endif /* EVENTTAGS_H_ */
