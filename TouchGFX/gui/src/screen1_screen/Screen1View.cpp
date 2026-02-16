#include "touchgfx/Unicode.hpp"
#include <gui/screen1_screen/Screen1View.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Color.hpp>
#include <images/BitmapDatabase.hpp>
#include "stm32f7xx.h"
#include "signal_gen.h"
#include <stdio.h>   // для vsnprintf
#include <stdarg.h>
#include <stdint.h>


extern uint16_t sine_table[];
extern TIM_HandleTypeDef htim7;
extern bool dacStart;

Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

//**************************************************************************************//

bool ButtonWithLabelState = false;
//buttonWithLabel1.setLabelText(touchgfx::TypedText(T_TXT_START)); // зміна напису кнопки
void Screen1View::ButtonStartPresset()
{
	if(!ButtonWithLabelState)
	{
		buttonWithLabel1.setLabelText(touchgfx::TypedText(T_TXT_STOP));
		buttonWithLabel1.setBitmaps(
				ButtonWithLabelState ? Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_TINY_ROUND_ACTION_ID) : Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_TINY_ROUND_PRESSED_ID),
				ButtonWithLabelState ? Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_TINY_ROUND_PRESSED_ID) : Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_TINY_ROUND_ACTION_ID)
		);
		dacStart = false;
		ButtonWithLabelState = true;
		HAL_TIM_Base_Start(&htim7);
	}else
	{
		buttonWithLabel1.setLabelText(touchgfx::TypedText(T_TXT_START));
		buttonWithLabel1.setBitmaps(
				ButtonWithLabelState ? Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_TINY_ROUND_ACTION_ID) : Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_TINY_ROUND_PRESSED_ID),
				ButtonWithLabelState ? Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_TINY_ROUND_PRESSED_ID) : Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_TINY_ROUND_ACTION_ID)
		);
		dacStart = true;
		ButtonWithLabelState = false;
		HAL_TIM_Base_Stop(&htim7);
	}
	buttonWithLabel1.invalidate();

}

void Screen1View::ButtonTextUpdate()
{
	if(ButtonWithLabelState)
	{
		buttonWithLabel1.setLabelText(touchgfx::TypedText(T_TXT_STOP));

	}else
	{
		buttonWithLabel1.setLabelText(touchgfx::TypedText(T_TXT_START));
	}
	buttonWithLabel1.invalidate();
}


void Screen1View::setSlider1Value(int value)
{

	Unicode::snprintf(textArea6Buffer, TEXTAREA6_SIZE, "%d", value);
	textArea6.invalidate();

	//slider1.invalidate();
}

void Screen1View::setSlider2Value(int value)
{

	Unicode::snprintf(textArea7Buffer, TEXTAREA7_SIZE, "%d", value);
	textArea7.invalidate();
}

void Screen1View::setSlider3Value(int value)
{
	float floatValue = value/10.0f;
	Unicode::snprintfFloat(textArea8Buffer, TEXTAREA7_SIZE, "%.1f", floatValue);
	textArea8.invalidate();
}

void Screen1View::setSlider4Value(int value)
{
	float floatValue = value/10.0f;
	Unicode::snprintfFloat(textArea9Buffer, TEXTAREA7_SIZE, "%.1f", floatValue);
	textArea9.invalidate();
	Generete_SineTable(value);
	SCB_CleanDCache_by_Addr((uint32_t*) sine_table, SINE_SAMPLES * sizeof(uint16_t));
}

