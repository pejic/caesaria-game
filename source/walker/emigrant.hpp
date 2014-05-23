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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_EMIGRANT_H_INCLUDE_
#define __CAESARIA_EMIGRANT_H_INCLUDE_

#include "immigrant.hpp"
#include "core/predefinitions.hpp"

class Emigrant;
typedef SmartPtr< Emigrant > EmigrantPtr;

/** This is an immigrant coming with his stuff */
class Emigrant : public Immigrant
{
public:
  typedef enum { G_EMIGRANT_CART1 = Good::goodCount, G_EMIGRANT_CART2, CT_MAX } CartType;

  static EmigrantPtr create( PlayerCityPtr city);

  virtual void getPictures( gfx::Pictures &oPics);
  virtual void timeStep(const unsigned long time);

  virtual void die();


  virtual ~Emigrant();
protected:
  virtual void _changeDirection();
  virtual void _updateThinks();

protected:
  const gfx::Picture& _cartPicture();

  Emigrant( PlayerCityPtr city );
};

#endif //__CAESARIA_EMIGRANT_H_INCLUDE_
