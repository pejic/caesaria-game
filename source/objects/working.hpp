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

#ifndef __CAESARIA_WORKINGBUILDING_H_INCLUDED__
#define __CAESARIA_WORKINGBUILDING_H_INCLUDED__

#include "objects/building.hpp"
#include "core/signals.hpp"

/** Building where people work */
class WorkingBuilding : public Building
{
public:
  WorkingBuilding(const object::Type type, const Size& size);
  virtual ~WorkingBuilding();

  void setMaximumWorkers(const unsigned int maximumWorkers);
  unsigned int maximumWorkers() const;
  unsigned int numberWorkers() const;
  unsigned int needWorkers() const;
  unsigned int laborAccessPercent() const;
  virtual std::string sound() const;

  void setWorkers( const unsigned int currentWorkers );
  unsigned int addWorkers( const unsigned int workers );
  unsigned int removeWorkers( const unsigned int workers );

  virtual math::Percent productivity() const;
  virtual bool mayWork() const;

  virtual void setActive(bool value);  // if false then this building is stopped
  virtual bool isActive() const;

  virtual void destroy();
  virtual void collapse();
  virtual void burn();
  virtual Variant getProperty(const std::string &name) const;

  virtual void timeStep(const unsigned long time);

  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream);

  template<typename T>
  void addWalker( SmartPtr<T> walker )
  {
    addWalker( ptr_cast<Walker>( walker ) );
  }

  virtual void addWalker( WalkerPtr walker );
  virtual const WalkerList& walkers() const;
  bool haveWalkers() const;
  walker::Type workerType() const;

  virtual std::string errorDesc() const;
  virtual std::string workersProblemDesc() const;
  virtual std::string workersStateDesc() const;
  virtual std::string troubleDesc() const;

  virtual void initialize(const object::Info& mdata);

public signals:
  Signal1<bool>& onActiveChange();

protected:
  void _setError(const std::string& err);
  void _fireWorkers();
  void _setWorkersType( walker::Type type );
  void _setClearAnimationOnStop( bool value );
  void _disaster();

  virtual void _updateAnimation( const unsigned long time );
  virtual void _changeAnimationState( bool enabled );

private:

  class Impl;
  ScopedPtr< Impl > _d;
};

class WorkingBuildingHelper
{
public:
  static std::string productivity2desc( WorkingBuildingPtr w, const std::string& prefix="" );
  static std::string productivity2str( WorkingBuildingPtr w );
};

#endif //__CAESARIA_WORKINGBUILDING_H_INCLUDED__
