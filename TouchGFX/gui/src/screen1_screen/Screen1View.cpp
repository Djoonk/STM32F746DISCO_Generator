#include <gui/screen1_screen/Screen1View.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Color.hpp>
#include <images/BitmapDatabase.hpp>
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

		ButtonWithLabelState = true;
	}else
	{
		buttonWithLabel1.setLabelText(touchgfx::TypedText(T_TXT_START));
		buttonWithLabel1.setBitmaps(
				ButtonWithLabelState ? Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_TINY_ROUND_ACTION_ID) : Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_TINY_ROUND_PRESSED_ID),
				ButtonWithLabelState ? Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_TINY_ROUND_PRESSED_ID) : Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_REGULAR_HEIGHT_50_TINY_ROUND_ACTION_ID)
		);
		ButtonWithLabelState = false;
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





