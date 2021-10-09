#include <Windows.h>
#include <thread>
#include <string>
#include <mutex>
#include <vector>

#include "../Overgay/Overgay.hpp"
#include "../Renderer/CRenderer.h"

class PepeGui {
private:
	enum Types {
		Slider = 1,
		Checkbox,
		SubMenu

	};
	int ItemsCount = 0;
	int SubMenuCount = 0;
	int BrowserIndex = 0;



	class ParentObject {
	public:
		class CheckBox {
		public:
			const char* label = "";
			bool* vBool;
		};

		class Slider {
		public:
			const char* label = "";
			float* vValue;
			float max, min;
		};


		class cSubMenu {
		public:
			const char* label = "";
			std::vector<std::pair<ParentObject, int>> Ui_Items = {};
		};

		CheckBox CheckBoxObject;
		Slider SliderObject;

		int type;
	};

	typedef std::pair<ParentObject, int>(UI_ItemDef);

	CRenderer* cRenderer;
	std::vector<std::pair<ParentObject, int>> Ui_Items = {};
	std::vector<std::pair<ParentObject::cSubMenu, const char*>> SubMenuObjects = {};

	const char* SubMenuIdentifier = "";
public:

	~PepeGui() {
		DestroyWindow(Vars::OverlayWindow);
	}


	D3DXVECTOR2 ScreenSize = {
		static_cast<float>(GetSystemMetrics(SM_CXSCREEN)),
		static_cast<float>(GetSystemMetrics(SM_CYSCREEN))
	};

	auto BeginSubMenu(const char* label)->void {
		this->SubMenuIdentifier = label;

		ParentObject parentObject{};
		parentObject.type = Types::SubMenu;

		ParentObject::cSubMenu subMenuObject;
		subMenuObject.label = label;
		SubMenuCount += 1;
		SubMenuObjects.push_back(std::make_pair(subMenuObject, label));
		

	}

	auto EndSubMenu()->void {
		this->SubMenuIdentifier = "";
	}

	auto AddCheckBox(bool* vBool, const char* label) {
		ParentObject parentObject{};
		parentObject.type = Types::Checkbox;
		ParentObject::CheckBox checkBoxObj;
		checkBoxObj.label = label;
		checkBoxObj.vBool = vBool;
		ItemsCount += 1;
		parentObject.CheckBoxObject = checkBoxObj;
		if (this->SubMenuIdentifier == "") {
			this->Ui_Items.push_back(std::make_pair(parentObject, ItemsCount));
		}
		else {
			for (int x = 0; x < SubMenuObjects.size(); x++) {
				if (SubMenuObjects.at(x).second == this->SubMenuIdentifier) {
					SubMenuObjects.at(x).first.Ui_Items.push_back(std::make_pair(parentObject, ItemsCount));
				}
			}
		}
	}

	auto AddSlider(float* vValue, float min_val, float max_val, const char* label) {
		ParentObject parentObject{};
		parentObject.type = Types::Slider;

		ParentObject::Slider sliderObject;
		sliderObject.label = label;
		sliderObject.vValue = vValue;
		sliderObject.min = min_val;
		sliderObject.max = max_val;

		parentObject.SliderObject = sliderObject;
		ItemsCount += 1;
		if (this->SubMenuIdentifier == "") {
			this->Ui_Items.push_back(std::make_pair(parentObject, ItemsCount));
		}
		else {
			for (int x = 0; x < SubMenuObjects.size(); x++) {
				if (SubMenuObjects.at(x).second == this->SubMenuIdentifier) {
					SubMenuObjects.at(x).first.Ui_Items.push_back(std::make_pair(parentObject, ItemsCount));
				}
			}
		}
	}


	auto HandleInput()->void {
		while (true)
		{
			if (Vars::MenuOpen == false) continue;

			if (GetAsyncKeyState(VK_DOWN) & 1){
				if(BrowserIndex < ItemsCount)
					BrowserIndex += 1;
			}
			else if (GetAsyncKeyState(VK_UP) & 1) {
				if (BrowserIndex >= 1)
					BrowserIndex -= 1;
			} else if (GetAsyncKeyState(VK_RETURN) & 1) {
				for (UI_ItemDef item : Ui_Items) {
					if (item.second == BrowserIndex) {
						if (item.first.type == Types::Checkbox) {
							if (*item.first.CheckBoxObject.vBool == true) {
								*item.first.CheckBoxObject.vBool = false;
							}
							else {
								*item.first.CheckBoxObject.vBool = true;
							}
						}
					}
				}
			} else if (GetAsyncKeyState(VK_LEFT) & 1) {
				for (UI_ItemDef item : Ui_Items) {
					if (item.second == BrowserIndex) {
						if (item.first.type == Types::Slider) {
							ParentObject::Slider slider = item.first.SliderObject;
							if (slider.min < *slider.vValue) {
								*slider.vValue -= 1;
							}
						}
					}
				}
			} else if (GetAsyncKeyState(VK_RIGHT) & 1) {
				for (UI_ItemDef item : Ui_Items) {
					if (item.second == BrowserIndex) {
						if (item.first.type == Types::Slider) {
							ParentObject::Slider slider = item.first.SliderObject;
							if (slider.max > *slider.vValue) {
								*slider.vValue += 1;
							}
						}
					}
				}
			}
			Sleep(100);
		}
	}
	auto Draw()->void {
		// background
		int width = 500, height = 700;
		int padding = 20;
		int menux = ScreenSize.x - (width + padding);
		int menuy = (ScreenSize.y / 2) - height / 2;
		cRenderer->FilledBoxOutlined(menux, menuy, width, height, 1, D3DCOLOR_ARGB(250, 50, 50, 50), D3DCOLOR_ARGB(255, 50, 168, 82));

		int item_padding = 10, indented_item_padding = 3;
		
		for (int x = 0; x < Ui_Items.size(); x++) {
			UI_ItemDef UI_Component = Ui_Items.at(x);
			
			if (UI_Component.first.type == Types::Checkbox) {
				int box_x = menux + 5;
				int box_y = ((menuy)+(x * 25)) + item_padding;

				if (BrowserIndex == UI_Component.second) {
					cRenderer->FilledBoxOutlined(box_x, box_y, width - item_padding * 2, 25, 1, D3DCOLOR_ARGB(250, 40, 40, 40), D3DCOLOR_ARGB(255, 20, 20, 20));
				}
				else {
					cRenderer->FilledBoxOutlined(box_x, box_y, width - item_padding * 2, 25, 1, D3DCOLOR_ARGB(250, 30, 30, 30), D3DCOLOR_ARGB(255, 20, 20, 20));
				}

				if (*UI_Component.first.CheckBoxObject.vBool == true) {
					cRenderer->String(box_x + indented_item_padding, box_y, D3DCOLOR_ARGB(255, 50, 168, 82), true, cRenderer->pMenuFont, UI_Component.first.CheckBoxObject.label);
				}
				else {
					cRenderer->String(box_x + indented_item_padding, box_y , D3DCOLOR_ARGB(255, 255, 255, 255), true, cRenderer->pMenuFont, UI_Component.first.CheckBoxObject.label);
				}
			}
			else if (UI_Component.first.type == Types::Slider) {
				int slider_x = menux + 5;
				int slider_y = (menuy + x * 25) + item_padding * 4;
				int slider_width = width - item_padding * 2;
				int slider_height = 25;
				int outline_size = 1;

				if (BrowserIndex == UI_Component.second) {
					cRenderer->FilledBoxOutlined(slider_x, slider_y, slider_width, slider_height, outline_size, D3DCOLOR_ARGB(240, 40, 40, 40), D3DCOLOR_ARGB(255, 20, 20, 20));
				}
				else {
					cRenderer->FilledBoxOutlined(slider_x, slider_y, slider_width, slider_height, outline_size, D3DCOLOR_ARGB(240, 30, 30, 30), D3DCOLOR_ARGB(255, 20, 20, 20));
				}

				float slider_x_val_based = slider_width * ((*UI_Component.first.SliderObject.vValue - UI_Component.first.SliderObject.min) / UI_Component.first.SliderObject.max);

				cRenderer->String(slider_x, slider_y - cRenderer->pMenuFontSize, D3DCOLOR_ARGB(255, 255, 255, 255), true, cRenderer->pMenuFont, UI_Component.first.SliderObject.label);

				int text_length = cRenderer->StringWidth(cRenderer->pFont, std::to_string(static_cast<int>(*UI_Component.first.SliderObject.vValue)).c_str());
				cRenderer->String((slider_x + slider_x_val_based) - text_length / 2, slider_y + ((25 - cRenderer->pFontSize) / 2), D3DCOLOR_ARGB(255, 255, 255, 255), true, cRenderer->pFont, std::to_string(static_cast<int>(*UI_Component.first.SliderObject.vValue)).c_str());

			}
		}
	}

	auto Init()->void {
		cRenderer = new CRenderer(Vars::p_Device);
	}
};