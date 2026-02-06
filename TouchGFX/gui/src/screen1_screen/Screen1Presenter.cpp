#include <gui/screen1_screen/Screen1View.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>

Screen1Presenter::Screen1Presenter(Screen1View& v)
    : view(v)
{

}
extern bool ButtonWithLabelState;

void Screen1Presenter::activate()
{
	view.ButtonTextUpdate();
}

void Screen1Presenter::deactivate()
{

}


//*******************************************************************//
void Screen1Presenter::buttonStartUpdate(bool state)
{

}
