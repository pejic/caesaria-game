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

#include "layertroubles.hpp"
#include "constants.hpp"
#include "core/gettext.hpp"
#include "objects/house.hpp"
#include "objects/constants.hpp"
#include "city/statistic.hpp"
#include "objects/house_level.hpp"
#include "good/helper.hpp"
#include "game/resourcegroup.hpp"
#include "core/event.hpp"
#include "objects/watersupply.hpp"
#include "gfx/tilemap_camera.hpp"
#include "objects/factory.hpp"
#include "core/utils.hpp"

using namespace gfx;

namespace citylayer
{

int Troubles::type() const{ return _type;}

void Troubles::drawTile( const RenderInfo& rinfo, Tile& tile)
{
  if( tile.overlay().isNull() )
  {
    drawLandTile( rinfo, tile );
  }
  else
  {
    bool needDrawAnimations = false;
    OverlayPtr overlay = tile.overlay();

    if( _isVisibleObject( overlay->type() ) )
    {
      // Base set of visible objects
      needDrawAnimations = true;
    }
    else
    {
      auto construction = overlay.as<Construction>();
      if( construction.isValid() )
      {
        std::string trouble = construction->troubleDesc();
        needDrawAnimations = !trouble.empty();
      }
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( rinfo, tile );
      registerTileForRendering( tile );
    }
    else
    {
      drawArea( rinfo, overlay->area(), config::layer.ground, config::tile.constr );
    }
  }

  tile.setRendered();
}

void Troubles::onEvent( const NEvent& event)
{
  if( event.EventType == sEventMouse )
  {
    switch( event.mouse.type  )
    {
    case NEvent::Mouse::moved:
    {
      Tile* tile = _camera()->at( event.mouse.pos(), false );  // tile under the cursor (or NULL)
      std::string text = "";

      if( tile != 0 )
      {
        auto constr = tile->overlay<Construction>();
        if( constr.isValid() )
        {
          text = constr->troubleDesc();

          if( text.empty() )
          {
            auto working = constr.as<WorkingBuilding>();
            if( text.empty() && working.isValid() )
            {
              int laborAccess = working->laborAccessPercent();
              if( working->roadside().empty() || laborAccess == 0 )
              {
                text = "##working_have_no_labor_access##";
              }              
              else
              {
                if( laborAccess < 25 ) { text = "##working_have_bad_labor_access##"; }
                else if( laborAccess < 50 ) { text = "##working_have_very_little_labor_access##"; }
                else if( laborAccess < 75 ) { text = "##working_have_some_labor_access##"; }
                else if( laborAccess < 100 ) { text = "##working_have_good_labor_access##"; }
                //else if( laborAccess < 100 ) { text = "##working_have_awsesome_labor_access##"; }
                else { text = ""; } //no problem with labor access
              }
            }
          }
        }
      }

      _setTooltipText( _(text) );
    }
    break;

    default: break;
    }
  }

  Layer::onEvent( event );
}

Troubles::Troubles( Camera& camera, PlayerCityPtr city, int type )
  : Layer( &camera, city ), _type( type )
{
  _initialize();
}

}//end namespace citylayer
