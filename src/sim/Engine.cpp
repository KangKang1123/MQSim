#include <stdexcept>
#include "Engine.h"
#include "../utils/Logical_Address_Partitioning_Unit.h"

namespace MQSimEngine
{
	Engine* Engine::_instance = NULL;

	Engine* Engine::Instance() {
		if (_instance == 0) {
			_instance = new Engine;//懒汉模式
		}
		return _instance;
	}

	void Engine::Reset()
	{
		_EventList->Clear();
		_ObjectList.clear();
		_sim_time = 0;
		stop = false;
		started = false;
		Utils::Logical_Address_Partitioning_Unit::Reset();//逻辑地址分区单元
	}


	//Add an object to the simulator object list，假设传进来SSD_Device*类型，把子类指针赋值给基类指针
	void Engine::AddObject(Sim_Object* obj)
	{
		if (_ObjectList.find(obj->ID()) != _ObjectList.end()) {
			throw std::invalid_argument("Duplicate object key: " + obj->ID());//抛出异常，防止添加重复的对象
		}
		_ObjectList.insert(std::pair<sim_object_id_type, Sim_Object*>(obj->ID(), obj));
	}
	
	Sim_Object* Engine::GetObject(sim_object_id_type object_id)
	{
		auto itr = _ObjectList.find(object_id);
		if (itr == _ObjectList.end()) {
			return NULL;
		}

		return (*itr).second;
	}

	void Engine::RemoveObject(Sim_Object* obj)
	{
		std::unordered_map<sim_object_id_type, Sim_Object*>::iterator it = _ObjectList.find(obj->ID());
		if (it == _ObjectList.end()) {
			throw std::invalid_argument("Removing an unregistered object.");
		}
		_ObjectList.erase(it);
	}

	/// This is the main method of simulator which starts simulation process.
	void Engine::Start_simulation()
	{
		started = true;

		for(std::unordered_map<sim_object_id_type, Sim_Object*>::iterator obj = _ObjectList.begin();
			obj != _ObjectList.end();
			++obj) {
			if (!obj->second->IsTriggersSetUp()) {//没有设置触发器的话就开始设置，第一次执行IsTriggersSetUp()都是false
				obj->second->Setup_triggers();
			}
		}

		for (std::unordered_map<sim_object_id_type, Sim_Object*>::iterator obj = _ObjectList.begin();//验证配置
			obj != _ObjectList.end();
			++obj) {
			obj->second->Validate_simulation_config();
		}
		
		for (std::unordered_map<sim_object_id_type, Sim_Object*>::iterator obj = _ObjectList.begin();//开始模拟（应该是为模拟做准备）
			obj != _ObjectList.end();
			++obj) {
			obj->second->Start_simulation();
		}
		
		Sim_Event* ev = NULL;
		while (true) {
			if (_EventList->Count == 0 || stop) {//树的节点数量为0或者stop为true，则跳出循环
				break;
			}

			EventTreeNode* minNode = _EventList->Get_min_node();
			ev = minNode->FirstSimEvent;

			_sim_time = ev->Fire_time;

			while (ev != NULL) {//对应结点事件执行完后就会挑选最小结点执行
				if(!ev->Ignore) {//Ignore为false，才会执行对应事件
					ev->Target_sim_object->Execute_simulator_event(ev);//正式执行相应事件，会边执行边插入事件
				}
				Sim_Event* consumed_event = ev;
				ev = ev->Next_event;//需要把结点对应的事件链表都执行完
				delete consumed_event;
			}
			_EventList->Remove(minNode);//相应的结点执行完就移除
		}
	}

	void Engine::Stop_simulation()
	{
		stop = true;
	}

	bool Engine::Has_started()
	{
		return started;
	}

	sim_time_type Engine::Time()
	{
		return _sim_time;
	}

	Sim_Event* Engine::Register_sim_event(sim_time_type fireTime, Sim_Object* targetObject, void* parameters, int type)//登记事件
	{//void*是一种通用的指针类型，可以指向任何类型的数据
		Sim_Event* ev = new Sim_Event(fireTime, targetObject, parameters, type);
		DEBUG("RegisterEvent " << fireTime << " " << targetObject)
		_EventList->Insert_sim_event(ev);//红黑树
		return ev;
	}

	void Engine::Ignore_sim_event(Sim_Event* ev)
	{
		ev->Ignore = true;
	}

	bool Engine::Is_integrated_execution_mode()
	{
		return false;
	}
}