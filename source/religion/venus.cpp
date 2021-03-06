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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "city/city.hpp"
#include "venus.hpp"
#include "game/gamedate.hpp"
#include "events/showinfobox.hpp"
#include "objects/extension.hpp"
#include "core/gettext.hpp"
#include "city/statistic.hpp"
#include "objects/house.hpp"
#include "city/sentiment.hpp"
#include "city/wrath_of_venus.hpp"
#include "walker/emigrant.hpp"

using namespace gfx;
using namespace city;
using namespace events;

namespace religion
{

namespace rome
{

void Venus::updateRelation(float income, PlayerCityPtr city)
{
  RomeDivinity::updateRelation( income, city );
}

object::Type Venus::templeType(Divinity::TempleSize size) const
{
    return size == bigTemple
                    ? BIG_TEMPLE_TYPE(venus)
                    : SML_TEMPLE_TYPE(venus);
}

void Venus::_doWrath( PlayerCityPtr city )
{
  events::dispatch<ShowInfobox>( _("##wrath_of_venus_title##"),
                                 _("##wrath_of_venus_description##"),
                                 true,
                                 "god_venus");

  SentimentPtr sentiment = city->statistic().services.find<Sentiment>();

  if( sentiment.isValid() )
  {
    sentiment->addBuff( -75, false, 12 );
  }

  SrvcPtr wrathOfVenus = Srvc::create<WrathOfVenus>( city, DateTime::monthsInYear / 4, 1 );
  wrathOfVenus->attach();
}

void Venus::_doBlessing(PlayerCityPtr city)
{
  events::dispatch<ShowInfobox>( _("##blessing_of_venus_title##"),
                                 _("##blessing_of_venus_description##") );

  // Increase health by 8 in <=20% of houses for 5 month
  HouseList houses = city->statistic().houses.find();

  int rndCount = math::random( houses.size() / 5 );
  if (rndCount > 0)
  {
    HouseList randomHouses = houses.random((size_t) rndCount);
    for( auto house : randomHouses)
    {
      ConstructionParamUpdater::assignTo(house.as<Construction>(),
                                         pr::healthBuff, true, 8, DateTime::weekInMonth * 5);
    }
  }
}

void Venus::_doSmallCurse(PlayerCityPtr city)
{
  int curseType = math::random( 3 );

  GameEventPtr e;
  switch(curseType)
  {
  case 0:
  {
    e = ShowInfobox::create( _("##smcurse_of_venus_title##"),
                             _("##smcurse_of_venus_description##"),
                             ShowInfobox::send2scribe );
    SentimentPtr sentiment = city->statistic().services.find<Sentiment>();
    if( sentiment.isValid() )
    {
      sentiment->addBuff( -math::random( 5 ), true, 12 );
    }
  }
  break;

  case 1:
  {
    e = ShowInfobox::create( _("##smcurse_of_venus_title##"),
                             _("##smcurse1_of_venus_description##"),
                             ShowInfobox::send2scribe );

    // Reduce health by 8 in <=20% of houses for 5 month
    HouseList houses = city->statistic().houses.find();

    HouseList randomHouses = houses.random( houses.size() / 5 );
    for( auto house : randomHouses)
    {
      ConstructionParamUpdater::assignTo( house.as<Construction>(),
                                          pr::healthBuff, true, -8, DateTime::weekInMonth * 5);
    }
  }
  break;

  case 2:
  {
    e = ShowInfobox::create(_("##smcurse_of_venus_title##"),
                            _("##smcurse2_of_venus_description##"),
                            ShowInfobox::send2scribe);

    // People emigrates from <=20% of houses
    HouseList houses = city->statistic().houses.find();

    HouseList list = houses.random(houses.size() / 5);
    for( auto house : list)
    {
      int hbCount = house->habitants().count();
      CitizenGroup homeless = house->removeHabitants(hbCount);
      EmigrantPtr emigrants = Emigrant::send2city(city, homeless, house->tile(), "##emigrant_no_home##");
      if( emigrants.isValid() )
        emigrants->leaveCity(house->tile());
    }

  }
  break;
  }

  e->dispatch();
}

Venus::Venus()
  : RomeDivinity( RomeDivinity::Venus )
{
  _wrathCounter = 0;
}

}//end namespace rome

}//end namespace religion
