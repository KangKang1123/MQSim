#ifndef SIMULATOR_OBJECT_H
#define SIMULATOR_OBJECT_H

#include<string>
#include "Sim_Event.h"

namespace MQSimEngine
{
	class Sim_Event;
	class Sim_Object
	{
	public:
		Sim_Object(const sim_object_id_type &id)
		{
			_id = id;
			_triggersSetUp = false;
		}

		sim_object_id_type ID()
		{
			return this->_id;
		}

		bool IsTriggersSetUp()
		{
			return _triggersSetUp;
		}

		//The Start function is invoked at the start phase of simulation to perform initialization
		virtual void Start_simulation() = 0;//纯虚函数，该类无法实例化对象，子类必须重写该函数

		//The Validate_simulation_config function is invoked to check if the objected is correctly configured or not.
		virtual void Validate_simulation_config() = 0;

		//The object connects its internal functions to the outside triggering events from other objects
		virtual void Setup_triggers()
		{
			_triggersSetUp = true;
		}
		
		virtual void Execute_simulator_event(Sim_Event*) = 0;//纯虚函数
		 
	private:
		sim_object_id_type _id;
		bool _triggersSetUp;
	};
}

#endif // !SIMULATOR_OBJECT_H
