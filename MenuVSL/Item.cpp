#include "Item.h"

#include "MenuVSL.h"
#include "Input.h"
#include "Log.h"

#include <string>
#include <sstream>

Item::Item(eItemType type)
{
    m_Type = type;

	if (m_Type == eItemType::ITEM_OPTIONS || m_Type == eItemType::ITEM_INT_RANGE || m_Type == eItemType::ITEM_FLOAT_RANGE)
	{
		m_ButtonLeft = new Item(eItemType::ITEM_BUTTON);

		m_ButtonRight = new Item(eItemType::ITEM_BUTTON);
	}
}

void Item::AddColorIndicator(CRGBA* color)
{
	ColorIndicator colorIndicator = { color };
	m_ColorIndicators.push_back(colorIndicator);
}

void Item::AddOption(int value, std::string name)
{	
	Option option = { value, name };
	m_Options.push_back(option);

	m_IntValueRange.max = m_Options.size() - 1;
}

void Item::SetCurrentOption(int value)
{
	int i = 0;
	for(auto option : m_Options)
	{
		if(option.value == value)
		{
			m_OptionsSelectedIndex = i;
			return;
		}
		i++;
	}
}

Option Item::GetCurrentOption()
{
	return m_Options[m_OptionsSelectedIndex];
}

bool Item::GetCheckboxValue()
{
	return *pCheckBoxBool;
}

void Item::Update()
{
    hasPressedThisFrame = false;

	isPointerOver = Input::IsPointInsideRect(Input::GetTouchPos(), m_Position, m_Size);

	if (isPointerOver && Input::IsTouchPressed())
	{
		if (!isPressed)
		{
			isPressed = true;
			Log::Level(LOG_LEVEL::LOG_BOTH) << "Item is now pressed" << std::endl;
		}
	} else {
		if (isPressed)
		{
			isPressed = false;
			Log::Level(LOG_LEVEL::LOG_BOTH) << "Item is not pressed anymore" << std::endl;
		}
	}

	if(isPointerOver)
	{
		if (Input::hasTouchBeenReleasedThisFrame && !waitingForTouchRelease)
		{
			hasPressedThisFrame = true;

			if (m_Type == eItemType::ITEM_BUTTON || m_Type == eItemType::ITEM_CHECKBOX)
			{	
				//if(onClick) onClick();

				if(m_Type == eItemType::ITEM_CHECKBOX)
				{
					Log::Level(LOG_LEVEL::LOG_BOTH) << "Checkbox value changed" << std::endl;

					*pCheckBoxBool = !*pCheckBoxBool;

					if (onValueChange) onValueChange();
				}
			}
		}
	}

	if(m_ButtonLeft) m_ButtonLeft->Update();
	if(m_ButtonRight) m_ButtonRight->Update();

	//options or int range (change by button press)
	if (m_Type == eItemType::ITEM_OPTIONS || m_Type == eItemType::ITEM_INT_RANGE)
	{
		/*
		if (m_Type == eItemType::ITEM_OPTIONS)
		{
			intValueRange.min = 0;
			intValueRange.max = options.size() - 1;
		}
		*/

		int prevVal = *m_IntValueRange.value;

		if (m_ButtonLeft->isPointerOver)
		{
			if (m_HoldToChange)
			{
				if (Input::isTouchPressed) m_IntValueRange.ChangeValueBy(-m_IntValueRange.addBy);
			}
			else {
				if(Input::hasTouchBeenPressedThisFrame) m_IntValueRange.ChangeValueBy(-m_IntValueRange.addBy);
			}
		}

		if (m_ButtonRight->isPointerOver)
		{
			if (m_HoldToChange)
			{
				if (Input::isTouchPressed) m_IntValueRange.ChangeValueBy(m_IntValueRange.addBy);
			}
			else {
				if (Input::hasTouchBeenPressedThisFrame) m_IntValueRange.ChangeValueBy(m_IntValueRange.addBy);
			}
		}

		if (*m_IntValueRange.value != prevVal)
		{
			if (onValueChange)
			{
				if (m_Type == eItemType::ITEM_OPTIONS)
				{
					Log::Level(LOG_LEVEL::LOG_BOTH) << "Option changed from " << prevVal << " to " << *m_IntValueRange.value << " - " << m_IntValueRange.min << " / " << m_IntValueRange.max << std::endl;
				}

				onValueChange();
			}
		}
	}

	//float range (change by button press)
	if (m_Type == eItemType::ITEM_FLOAT_RANGE)
	{
		float prevVal = *m_FloatValueRange.value;

		if (m_ButtonLeft->isPointerOver)
		{
			if (m_HoldToChange)
			{
				if (Input::isTouchPressed) m_FloatValueRange.ChangeValueBy(-m_FloatValueRange.addBy);
			}
			else {
				if (Input::hasTouchBeenPressedThisFrame) m_FloatValueRange.ChangeValueBy(-m_FloatValueRange.addBy);
			}
		}

		if (m_ButtonRight->isPointerOver)
		{
			if (m_HoldToChange)
			{
				if (Input::isTouchPressed) m_FloatValueRange.ChangeValueBy(m_FloatValueRange.addBy);
			}
			else {
				if (Input::hasTouchBeenPressedThisFrame) m_FloatValueRange.ChangeValueBy(m_FloatValueRange.addBy);
			}
		}

		if (*m_FloatValueRange.value != prevVal)
		{
			if (onValueChange) onValueChange();
		}
	}

	if(onUpdate) onUpdate();
}

void Item::Draw()
{
    auto menuVSL = MenuVSL::Instance;

	auto position = m_Position;
    auto size = m_Size;

	bool isHoveringItem = isPressed;
	if(!m_CanBeSelected) isHoveringItem = false;

    if(isHoveringItem) menuVSL->DrawRect(CRect(position.x, position.y, position.x + size.x, position.y + size.y), CRGBA(255, 255, 255, 255));

	//
	
	DrawColorIndicatos();

	if(m_StringPtrAtRight != NULL)
	{
		auto offset = m_StringAtRightOffset;
		auto stringPosition = position + CVector2D(m_Size.x, m_Size.y/2) + offset;
		stringPosition.y -= MenuVSL::GetFontHeight() / 2;
		
		menuVSL->DrawString(*m_StringPtrAtRight, stringPosition, CRGBA(255, 255, 255), eFontAlignment::ALIGN_RIGHT);
	}

	if(m_StringAtRight.size() > 0)
	{
		auto offset = m_StringAtRightOffset;
		auto stringPosition = position + CVector2D(m_Size.x, m_Size.y/2) + offset;
		stringPosition.y -= MenuVSL::GetFontHeight() / 2;
		
		menuVSL->DrawString(m_StringAtRight, stringPosition, CRGBA(255, 255, 255), eFontAlignment::ALIGN_RIGHT);
	}

	if (m_Type == eItemType::ITEM_CHECKBOX)
	{
		float padding = 8.0f;
		float h = m_Size.y - padding * 2;
		float w = h;

		CVector2D checkBoxPos = position;
		checkBoxPos.x += m_Size.x;
		checkBoxPos.y += padding;

		checkBoxPos.x += -padding - w;

		menuVSL->DrawRect(checkBoxPos, CVector2D(w, h), CRGBA(0, 0, 0));

		float border = 4.0f;

		checkBoxPos.x += border;
		checkBoxPos.y += border;
		w -= border * 2;
		h -= border * 2;

		menuVSL->DrawRect(checkBoxPos, CVector2D(w, h), (*pCheckBoxBool) ? CRGBA(255, 255, 255) : CRGBA(51, 51, 51));
	}

	if (m_Type == eItemType::ITEM_OPTIONS || m_Type == eItemType::ITEM_INT_RANGE || m_Type == eItemType::ITEM_FLOAT_RANGE)
	{
		float padding = 4.0f;
		float h = m_Size.y - padding * 2;
		float w = h;

		auto buttonSize = CVector2D(w, h);
		auto buttonPosition = position + CVector2D(m_Size.x, 0);

		buttonPosition.x -= buttonSize.x;

		m_ButtonRight->m_Position = buttonPosition;
		m_ButtonRight->m_Size = buttonSize;
		m_ButtonRight->m_Text = ">";
		m_ButtonRight->m_DrawTextAtCenter = true;
		m_ButtonRight->Draw();

		buttonPosition.x += -m_OptionsTextWidth - buttonSize.x;

		m_ButtonLeft->m_Position = buttonPosition;
		m_ButtonLeft->m_Size = buttonSize;
		m_ButtonLeft->m_Text = "<";
		m_ButtonLeft->m_DrawTextAtCenter = true;
		m_ButtonLeft->Draw();

		buttonPosition.x += buttonSize.x;
		buttonPosition.x += m_OptionsTextWidth/2;

		buttonPosition.y += buttonSize.y/2;
		buttonPosition.y -= MenuVSL::GetFontHeight() / 2;

		std::stringstream ss;
		if(m_Type == eItemType::ITEM_INT_RANGE)
		{
			ss << std::to_string(*m_IntValueRange.value) << std::endl;
		}
		if(m_Type == eItemType::ITEM_FLOAT_RANGE)
		{
			ss << std::fixed << std::setprecision(2) << (*m_FloatValueRange.value) << std::endl;
		}
		if(m_Type == eItemType::ITEM_OPTIONS)
		{
			auto option = m_Options[m_OptionsSelectedIndex];
			ss << option.name << std::endl;
		}

		menuVSL->DrawString(ss.str(), buttonPosition, CRGBA(255, 255, 255), eFontAlignment::ALIGN_CENTER);
	}

	if(m_Type == eItemType::ITEM_BUTTON)
	{
		menuVSL->DrawRect(m_Position, m_Size, isHoveringItem ? CRGBA(255, 255, 255) : m_BackgroundColor);
	}

	//text
	float fontHeight = MenuVSL::GetFontHeight();

	auto textPosition = position;
	auto align = eFontAlignment::ALIGN_LEFT;

	textPosition.y += size.y / 2 - fontHeight / 2;

	if(m_DrawTextAtCenter)
	{
		textPosition.x += m_Size.x / 2;
		align = eFontAlignment::ALIGN_CENTER;
	} else {
		textPosition.x += 10.0f;
	}

	menuVSL->DrawString(m_Text, textPosition, isHoveringItem ? CRGBA(0, 0, 0) : m_TextColor, align);
}

void Item::DrawColorIndicatos()
{
	float padding = 8.0f;
	float h = m_Size.y - padding * 2;
	float w = h;

	CVector2D checkBoxPos = m_Position;
	checkBoxPos.x += m_Size.x;
	checkBoxPos.y += padding;

	checkBoxPos.x += -padding - w;

	for(auto colorIndicator : m_ColorIndicators)
	{
		menuVSL->DrawRect(checkBoxPos, CVector2D(w, h), *colorIndicator.color);

		checkBoxPos.x -= w + padding;
	}
}

void Item::OnPress()
{
	Log::Level(LOG_LEVEL::LOG_BOTH) << "Item OnPress" << std::endl;

	if(onClick) onClick();

	/*
	if (m_Type == eItemType::ITEM_BUTTON || m_Type == eItemType::ITEM_CHECKBOX)
	{	
		if(onClick) onClick();
	}
	*/
}