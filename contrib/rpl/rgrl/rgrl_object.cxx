#include "rgrl_object.h"
#include "rgrl_command.h"
#include "rgrl_event.h"

rgrl_object::
rgrl_object()
  : debug_flag_( 0 ),
    warning_( true ),
    observer_count_( 0 )
{}

rgrl_object::
~rgrl_object()
{
}

void
rgrl_object::
set_debug_flag(  unsigned int debugFlag ) const
{
  debug_flag_ = debugFlag;
}

unsigned int
rgrl_object::
debug_flag() const
{
  return debug_flag_;
}

void
rgrl_object::
set_warning( bool flag ) const
{
  warning_ = flag;
}

bool
rgrl_object::
warning() const
{
  return warning_;
}

unsigned int
rgrl_object::
add_observer( rgrl_event_sptr event, rgrl_command_sptr cmd )
{
  rgrl_object_observer new_observer( cmd, event );
  observers_[observer_count_] = new_observer;
  observer_count_++;
  return observer_count_ - 1;
}

rgrl_command_sptr
rgrl_object::
get_command(unsigned int tag)
{
  observer_map::iterator i =  observers_.find( tag );

  if ( i == observers_.end() )
    return 0;

  return i->second.command_;
}

void
rgrl_object::
invoke_event( const rgrl_event & event)
{
  typedef observer_map::iterator obs_itr;
  obs_itr i = observers_.begin();
  for ( ; i != observers_.end(); ++i) {
    rgrl_event_sptr e =  i->second.event_;
    if (e->is_same_type(&event)) {
      i->second.command_->execute(this, event);
    }
  }
}

void
rgrl_object::
invoke_event( const rgrl_event & event) const
{
  typedef observer_map::const_iterator const_obs_itr;
  const_obs_itr i = observers_.begin();
  for ( ; i != observers_.end(); ++i) {
    const rgrl_event_sptr e =  i->second.event_;
    if (e->is_same_type(&event)) {
      i->second.command_->execute(this, event);
    }
  }
}

void
rgrl_object::
remove_observer(unsigned int tag)
{
  observer_map::iterator i =  observers_.find( tag );

  if ( i == observers_.end() ) return;
  observers_.erase(tag);
}

bool
rgrl_object::
has_observer( const rgrl_event & event ) const
{
  typedef observer_map::const_iterator const_obs_itr;
  const_obs_itr i = observers_.begin();
  for ( ; i != observers_.end(); ++i) {
    const rgrl_event_sptr e = i->second.event_;
    if ( e->is_same_type(&event) ) {
      return true;
    }
  }
  return false;
}
