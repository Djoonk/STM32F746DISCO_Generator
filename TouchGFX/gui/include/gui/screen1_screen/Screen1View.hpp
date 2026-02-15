#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void ButtonStartPresset();

    virtual void setSlider1Value(int value);
    virtual void setSlider2Value(int value);
    virtual void setSlider3Value(int value);
    virtual void setSlider4Value(int value);

    virtual void ButtonTextUpdate();

protected:
};

#endif // SCREEN1VIEW_HPP
