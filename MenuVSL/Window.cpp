#include "Window.h"

#include "Item.h"
#include "Log.h"
#include "MenuVSL.h"

Window::Window()
{
    auto window = this;

    m_LeftButton = AddButton("<", CRGBA(0, 0, 255));
    m_LeftButton->m_FitInWindow = false;
    m_LeftButton->onClick = [window]() {
        window->m_Page -= 1;
		if (window->m_Page < 0) window->m_Page = 0;

        //menuVSL->debug->m_Visible = true;
        //menuVSL->debug->AddLine("<" + std::to_string(window->m_Page) + "/" + std::to_string(window->GetMaxPages()));
    };

    m_RightButton = AddButton(">", CRGBA(0, 0, 255));
    m_RightButton->m_FitInWindow = false;
    m_RightButton->onClick = [window]() {
        window->m_Page += 1;

		int maxPages = window->GetMaxPages();
		if (window->m_Page >= maxPages-1) window->m_Page = maxPages-1;

        //menuVSL->debug->m_Visible = true;
        //menuVSL->debug->AddLine(">" + std::to_string(window->m_Page) + "/" + std::to_string(window->GetMaxPages()));
    };

    m_BackButton = AddButton("Back", CRGBA(0, 0, 255));
    m_BackButton->m_FitInWindow = false;
    m_BackButton->onClick = [window]() {
        //menuVSL->debug->m_Visible = true;
        //menuVSL->debug->AddLine("Back");

        window->SetToBeRemoved();
    };
}

Window::~Window()
{
    //wait, i just realised that items never get destroyed...
}

void Window::SetPosition(CVector2D position)
{
    m_Position = position;
}

void Window::AddItem(IItem* item)
{
	//item->waitingForTouchRelease = waitingForTouchRelease;

    m_Items.push_back(item);
}

IItem* Window::AddText(std::string text, CRGBA color)
{
	Log::Level(LOG_LEVEL::LOG_BOTH) << "Window: AddText " << text << std::endl;

	Item* item = new Item(eItemType::ITEM_TEXT);
    item->m_Text = text;
    item->m_TextColor = color;
    item->m_CanBeSelected = false;

	AddItem(item);

	return item;
}

IItem* Window::AddButton(std::string text, CRGBA color)
{
	Log::Level(LOG_LEVEL::LOG_BOTH) << "Window: AddButton " << text << std::endl;

	Item* item = new Item(eItemType::ITEM_TEXT);
    item->m_Text = text;
    //item->m_TextColor = color;
    item->m_BackgroundColor = color;

	AddItem(item);

	return item;
}

IItem* Window::AddButton(std::string text)
{
	return AddButton(text, CRGBA(255, 255, 255));
}

IItem* Window::AddCheckbox(std::string text, bool* value)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Window: AddCheckbox " << text << std::endl;

	Item* item = new Item(eItemType::ITEM_CHECKBOX);
    item->m_Text = text;

    item->pCheckBoxBool = value;
	if (value == NULL) item->pCheckBoxBool = &item->tmpBool;

	AddItem(item);

	return item;
}

IItem* Window::AddIntRange(std::string text, int* value, int min, int max, int addBy)
{
	Log::Level(LOG_LEVEL::LOG_BOTH) << "Window: AddIntRange" << std::endl;

	Item* item = new Item(eItemType::ITEM_INT_RANGE);
	item->m_Text = text;
	item->m_IntValueRange.value = value;
    if (value == NULL) item->m_IntValueRange.value = &item->tmpInt;
	item->m_IntValueRange.min = min;
	item->m_IntValueRange.max = max;
	item->m_IntValueRange.addBy = addBy;
    item->m_CanBeSelected = false;

	AddItem(item);

	return item;
}

IItem* Window::AddFloatRange(std::string text, float* value, float min, float max, float addBy)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Window: AddFloatRange" << std::endl;

	Item* item = new Item(eItemType::ITEM_FLOAT_RANGE);
	item->m_Text = text;
	item->m_FloatValueRange.value = value;
    if (value == NULL) item->m_FloatValueRange.value = &item->tmpFloat;
	item->m_FloatValueRange.min = min;
	item->m_FloatValueRange.max = max;
	item->m_FloatValueRange.addBy = addBy;
    item->m_CanBeSelected = false;
    item->m_OptionsTextWidth = 150.0f;

	AddItem(item);

	return item;
}

void Window::SetToBeRemoved()
{
    m_CanBeRemoved = true;
}

bool Window::GetIsActive()
{
    return MenuVSL::Instance->m_ActiveWindow == this;
}

IItem* Window::AddOptions(std::string text)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Window: AddOptions" << std::endl;

	Item* item = new Item(eItemType::ITEM_OPTIONS);
	item->m_Text = text;
	item->m_IntValueRange.value = &item->m_OptionsSelectedIndex;
    item->m_CanBeSelected = false;
    item->m_OptionsTextWidth = 150.0f;

	AddItem(item);

	return item;
}

void Window::Update()
{
    auto items = GetItemsInPage();

    for(auto item : items)
    {
        if(!item->m_Visible)
        {
            //to fix a bug where window this its always beeing pressed
            item->hasPressedThisFrame = false;
            item->isPointerOver = false;
            continue;
        }

        ((Item*)item)->Update();
    }
}

void Window::Draw()
{
	auto position = m_Position;
    auto menuVSL = MenuVSL::Instance;

    m_LeftButton->m_Position = CVector2D(position.x - 60, position.y);
    m_LeftButton->m_Size = CVector2D(50, 50);
    m_LeftButton->m_Visible = m_Page > 0;

    m_RightButton->m_Position = CVector2D(position.x + m_Width + 10, position.y);
    m_RightButton->m_Size = CVector2D(50, 50);
    m_RightButton->m_Visible = m_Page < GetMaxPages() - 1;

    m_BackButton->m_Position = CVector2D(position.x + m_Width + 10, position.y + 60);
    m_BackButton->m_Size = CVector2D(150, 50);
    m_BackButton->m_Visible = m_ShowBackButton;

    if(!GetIsActive())
    {
        m_LeftButton->m_Visible = false;
        m_RightButton->m_Visible = false;
        m_BackButton->m_Visible = false;
    }

    //window

    float windowHeight = 110;

    menuVSL->DrawRect(CRect(position.x, position.y, position.x + m_Width, position.y + windowHeight), m_TitleBackgroundColor);

    //change font
    menuVSL->SetFontScale(1.5f * 2, 2.25f * 2);
    MenuVSL::m_FontStyle = eFontStyle::FONT_GOTHIC;

    float fontHeight = MenuVSL::m_FontScale.y * 10.0f;
    CVector2D titlePosition = CVector2D(position.x + m_Width/2, position.y + windowHeight/2 - fontHeight / 2);
    
    menuVSL->DrawString(m_Title, titlePosition, CRGBA(255, 255, 255), eFontAlignment::ALIGN_CENTER);
    
    //reset font
    menuVSL->ResetFontScale();
    MenuVSL::m_FontStyle = MenuVSL::m_DefaultFontStyle;

    position.y += windowHeight;

    // sub title

    float subTitleHeight = 40;
    menuVSL->DrawRect(CRect(position.x, position.y, position.x + m_Width, position.y + subTitleHeight), m_SubTitleBackgroundColor);
    
    fontHeight = MenuVSL::m_FontScale.y * 10.0f;
    CVector2D subTitlePosition = CVector2D(position.x + 5, position.y + subTitleHeight/2 - fontHeight / 2);

    menuVSL->DrawString(m_Title, subTitlePosition, CRGBA(255, 255, 255), eFontAlignment::ALIGN_LEFT);

    position.y += subTitleHeight;

    //

    auto items = GetItemsInPage();

    float totalWindowHeight = 0.0f;
    for(auto item : items)
    {
        if(item->m_FitInWindow) totalWindowHeight += item->m_Size.y;
    }

    menuVSL->DrawRect(CRect(position.x, position.y, position.x + m_Width, position.y + totalWindowHeight), m_ItemBackgroundColor);

    for(auto item : items)
    {
        if(item->m_FitInWindow)
        {
            item->m_Size.x = m_Width;
            item->m_Position = position;

            position.y += item->m_Size.y;
        }

        if(!item->m_Visible) continue;

        ((Item*)item)->Draw();
    }
}

std::vector<IItem*> Window::GetItemsInPage()
{
	std::vector<IItem*> itemsOfAllPages;

    for(auto item : m_Items)
    {
        if(!item->m_FitInWindow) continue;

        itemsOfAllPages.push_back(item);
    }

    //int maxPages = (int)std::ceil((float)items.size() / (float)maxItemsPerPage);
	int startIndex = (m_Page * m_MaxItemsPerPage);
	//int endIndex = startIndex + maxItemsPerPage;

    auto floatingItems = GetFloatingItems();
	std::vector<IItem*> itemsToDraw;

    for(auto item : floatingItems)
    {
        itemsToDraw.push_back(item);
    }

	for (size_t i = 0; i < itemsOfAllPages.size(); i++)
	{
		if ((int)i >= startIndex && (int)i < (startIndex + m_MaxItemsPerPage))
		{
			auto item = itemsOfAllPages[i];

			itemsToDraw.push_back(item);
			//totalHeight += item->m_Size.y;
		}
	}

	return itemsToDraw;
}

std::vector<IItem*> Window::GetFloatingItems()
{
    std::vector<IItem*> floatingItems;

    for (size_t i = 0; i < m_Items.size(); i++)
	{
        auto item = m_Items[i];

        if(item->m_FitInWindow) continue;

        floatingItems.push_back(item);
    }

    return floatingItems;
}

int Window::GetMaxPages()
{
    std::vector<IItem*> items;
    for(auto item : m_Items)
    {
        if(!item->m_FitInWindow) continue;
        items.push_back(item);
    }

	return (int)std::ceil((float)items.size() / (float)m_MaxItemsPerPage);
}