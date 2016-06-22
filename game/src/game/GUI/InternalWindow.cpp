//********************************************************************************************
//*
//*    This file is part of Egoboo.
//*
//*    Egoboo is free software: you can redistribute it and/or modify it
//*    under the terms of the GNU General Public License as published by
//*    the Free Software Foundation, either version 3 of the License, or
//*    (at your option) any later version.
//*
//*    Egoboo is distributed in the hope that it will be useful, but
//*    WITHOUT ANY WARRANTY; without even the implied warranty of
//*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//*    General Public License for more details.
//*
//*    You should have received a copy of the GNU General Public License
//*    along with Egoboo.  If not, see <http://www.gnu.org/licenses/>.
//*
//********************************************************************************************

/// @file game/GUI/InternalWindow.cpp
/// @details InternalWindow
/// @author Johan Jansen

#include "game/GUI/InternalWindow.hpp"
#include "game/GUI/Image.hpp"

static constexpr int BORDER_PIXELS = 5;

InternalWindow::TitleBar::TitleBar(const std::string &title) :
    _titleBarTexture("mp_data/titlebar"),
    _titleSkull("mp_data/gui-skull"),
    _font(_gameEngine->getUIManager()->getFont(UIManager::FONT_GAME)),
    _title(title),
    _textWidth(0),
    _textHeight(0)
{
    //Make title upper case
    std::transform(_title.begin(), _title.end(), _title.begin(), ::toupper);

    //Set size depending on title string
    _font->getTextSize(_title, &_textWidth, &_textHeight);
    _textWidth = std::max<int>(32, _textWidth);
    _textHeight = std::max<int>(32, _textHeight);
    setSize(_textWidth + 20, _textHeight+5);
}

void InternalWindow::TitleBar::draw()
{
    //Background
    _gameEngine->getUIManager()->drawImage(_titleBarTexture.get(), Point2f(getX()-BORDER_PIXELS*2, getY()), Vector2f(getWidth()+BORDER_PIXELS*4, getHeight()));

    //Title String
    _font->drawText(_title, getX() + getWidth()/2 - _textWidth/2, getY() + 12, Ego::Colour4f(0.28f, 0.16f, 0.07f, 1.0f));

    //Draw the skull icon on top
    const int skullWidth = _titleSkull.get_ptr()->getWidth()/2;
    const int skullHeight = _titleSkull.get_ptr()->getHeight()/2;
    _gameEngine->getUIManager()->drawImage(_titleSkull.get(), Point2f(getX()+getWidth()/2 - skullWidth/2, getY() - skullHeight/2), Vector2f(skullWidth, skullHeight));
}

InternalWindow::InternalWindow(const std::string &title) :
    _titleBar(new TitleBar(title)),
    _closeButton(std::make_shared<Image>("mp_data/gui-button_close")),
    _background("mp_data/guiwindow"),
    _mouseOver(false),
    _mouseOverCloseButton(false),
    _isDragging(false),
    _mouseDragOffset(0.0f, 0.0f),
    _transparency(0.33f),
    _firstDraw(true)
{
    //Add the close button
    //_closeButton->setOnClickFunction([this]{
    //    this->destroy();
    //});

    //Set initial window position, do after all components have been initialized
    setPosition(20, 20);

    //Set default color
    _closeButton->setTint(Ego::Math::Colour4f(0.8f, 0.8f, 0.8f, 1.0f));
}

void InternalWindow::drawContainer()
{
    //Draw background first
    _gameEngine->getUIManager()->drawImage(_background.get(), Point2f(getX()-BORDER_PIXELS, getY()-BORDER_PIXELS), Vector2f(getWidth()+BORDER_PIXELS*2, getHeight()+BORDER_PIXELS*2), Ego::Colour4f(1.0f, 1.0f, 1.0f, 0.9f));

    //Draw window title
    _titleBar->draw();

    //Draw the close button
    _closeButton->draw();
}

bool InternalWindow::notifyMouseMoved(const Ego::Events::MouseMovedEventArgs& e)
{
    if(_isDragging) {
        setPosition( Ego::Math::constrain<int>(e.getPosition().x()+_mouseDragOffset[0], 0, _gameEngine->getUIManager()->getScreenWidth()-getWidth()), 
                     Ego::Math::constrain<int>(e.getPosition().y()+_mouseDragOffset[1], _titleBar->getHeight()/2, _gameEngine->getUIManager()->getScreenHeight()-getHeight()) );
        return true;
    }
    else {
        _mouseOver = InternalWindow::contains(e.getPosition())
                  || _titleBar->contains(e.getPosition());

        if(_closeButton->contains(e.getPosition())) {
            _closeButton->setTint(Ego::Math::Colour4f::white());
        }
        else {
            _closeButton->setTint(Ego::Math::Colour4f(0.8f, 0.8f, 0.8f, 1.0f));
        }
    }

    return ComponentContainer::notifyMouseMoved(e);
}

bool InternalWindow::notifyMouseClicked(const Ego::Events::MouseClickedEventArgs& e)
{
    if(_mouseOver && e.getButton() == SDL_BUTTON_LEFT)
    {
        if(!_isDragging && _closeButton->contains(e.getPosition())) {
            AudioSystem::get().playSoundFull(AudioSystem::get().getGlobalSound(GSND_BUTTON_CLICK));
            destroy();
            return true;
        }

        // Bring the window in front of all other windows
        bringToFront();

        // Only the top title bar triggers dragging
        if(_titleBar->contains(e.getPosition())) {
            _isDragging = true;
            _mouseDragOffset[0] = getX() - e.getPosition().x();
            _mouseDragOffset[1] = getY() - e.getPosition().y();

            // Move the window immediatly
            return notifyMouseMoved(Ego::Events::MouseMovedEventArgs(e.getPosition()));
        } else {
            _isDragging = false;
        }
    } else if(e.getButton() == SDL_BUTTON_RIGHT) {
        _isDragging = false;
    }

    return ComponentContainer::notifyMouseClicked(e);
}

bool InternalWindow::notifyMouseReleased(const Ego::Events::MouseReleasedEventArgs& e)
{
    _isDragging = false;
    return false;
}

void InternalWindow::draw()
{
    if(_firstDraw) {
        _firstDraw = false;

        //Make sure that all components added to this window are placed relative to 
        //our position so that (0,0) is topleft corner in this InternalWindow
        for(const std::shared_ptr<GUIComponent> &component : ComponentContainer::iterator())
        {
            component->setPosition(component->getX()+getX(), component->getY()+getY());
        }
    }
    drawAll();
}

void InternalWindow::setPosition(const int x, const int y)
{
    //Calculate offsets in position change
    int translateX = x - getX();
    int translateY = y - getY();

    //Shift window position
    GUIComponent::setPosition(x, y);

    //Shift all child components as well
    for(const std::shared_ptr<GUIComponent> &component : ComponentContainer::iterator())
    {
        component->setPosition(component->getX() + translateX, component->getY() + translateY);
    }

    //Finally update titlebar position
    _titleBar->setPosition(x, y - _titleBar->getHeight()/2);
    _closeButton->setPosition(_titleBar->getX() + _titleBar->getWidth() - _closeButton->getWidth(), _titleBar->getY() + _titleBar->getHeight()/2 - _closeButton->getHeight()/2);
}

void InternalWindow::setTransparency(float alpha)
{
    _transparency = Ego::Math::constrain(alpha, 0.0f, 1.0f);
}

void InternalWindow::addComponent(std::shared_ptr<GUIComponent> component)
{
    //Make sure that all components added to this window are placed relative to 
    //our position so that (0,0) is topleft corner in this InternalWindow
    if(!_firstDraw) {
        component->setPosition(component->getX()+getX(), component->getY()+getY());
    }
    ComponentContainer::addComponent(component);
}

void InternalWindow::setSize(const int width, const int height)
{
    //Also update the width of the title bar if our with changes
    _titleBar->setSize(width, _titleBar->getHeight());
    _closeButton->setSize(22, 22);
    _closeButton->setPosition(_titleBar->getX() + _titleBar->getWidth() - _closeButton->getWidth(), _titleBar->getY() + _titleBar->getHeight()/2 - _closeButton->getHeight()/2);

    GUIComponent::setSize(width, height);
}
