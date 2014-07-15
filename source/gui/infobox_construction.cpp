// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "infobox_construction.hpp"
#include "core/event.hpp"
#include "game/settings.hpp"
#include "events/showtileinfo.hpp"

using namespace constants;

namespace gui
{

InfoboxConstruction::InfoboxConstruction( Widget* parent, Rect rect, Rect blackArea )
  : InfoboxSimple( parent, rect, blackArea )
{
  Widget::setupUI( GameSettings::rcpath( "/gui/infoboxconstr.gui" ) );
}

InfoboxConstruction::~InfoboxConstruction() {}

bool InfoboxConstruction::onEvent(const NEvent& event)
{
  switch( event.EventType )
  {
  case sEventKeyboard:
    if( event.keyboard.key == KEY_COMMA || event.keyboard.key == KEY_PERIOD )
    {
      _switch( event.keyboard.key );
    }
  break;

  case sEventGui:
    if( event.gui.type == guiButtonClicked && ( event.gui.caller->ID() == KEY_COMMA || event.gui.caller->ID() == KEY_PERIOD ) )
    {
      _switch( event.keyboard.key );
    }
  break;

  default: break;
  }

  return InfoboxSimple::onEvent( event );
}

ConstructionPtr InfoboxConstruction::getConstruction() const { return _construction; }
void InfoboxConstruction::setConstruction(ConstructionPtr construction) { _construction = construction; }

void InfoboxConstruction::_switch(int flag)
{
  if( _construction.isValid() )
  {
    events::GameEventPtr e = events::ShowTileInfo::create( getConstruction()->pos(), flag == KEY_PERIOD
                                                                                       ? events::ShowTileInfo::next
                                                                                       : events::ShowTileInfo::prew );
    deleteLater();
    e->dispatch();
  }
}

}//end namespace gui
