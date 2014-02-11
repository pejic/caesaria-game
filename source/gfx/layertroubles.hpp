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

#ifndef __CAESARIA_LAYER_TROUBLES_H_INCLUDED__
#define __CAESARIA_LAYER_TROUBLES_H_INCLUDED__

#include "layer.hpp"

class LayerTroubles : public Layer
{
public:
  virtual int getType() const;
  virtual VisibleWalkers getVisibleWalkers() const;
  virtual void drawTile( GfxEngine& engine, Tile& tile, Point offset );

  static LayerPtr create(TilemapCamera& camera, PlayerCityPtr city);
  virtual void handleEvent(NEvent& event);

private:
  LayerTroubles( TilemapCamera& camera, PlayerCityPtr city );

  std::set<int> _flags;
  std::set<int> _walkers;
  int _type;
};

#endif //__CAESARIA_LAYER_TROUBLES_H_INCLUDED__
