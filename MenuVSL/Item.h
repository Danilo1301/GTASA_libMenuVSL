#pragma once

#include <iostream>
#include <vector>

#include "menu/IItem.h"

template<class T>
class ValueRange {
public:
	T* value = NULL;
	T min = 0;
	T max = 255;
	T addBy = 1;

	void ChangeValueBy(T amount)
	{
		*value += amount;

		if (*value < min) *value = min;
		if (*value > max) *value = max;
	}
};

class Item : public IItem {
public:
    ValueRange<int> m_IntValueRange;
	ValueRange<float> m_FloatValueRange;

	Item* m_ButtonLeft = NULL;
	Item* m_ButtonRight = NULL;

    Item(eItemType type);
    
	/*1.0.0*/

	/*1.2.0*/

	void AddColorIndicator(CRGBA* color);
	void AddOption(int value, std::string name);
	void SetCurrentOption(int value);
	Option GetCurrentOption();
	bool GetCheckboxValue();

	//

    void Update();
    void Draw();
    void DrawColorIndicatos();

    void OnPress();
};