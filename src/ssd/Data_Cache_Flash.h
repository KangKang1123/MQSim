#ifndef DATA_CACHE_FLASH_H
#define DATA_CACHE_FLASH_H

#include <list>
#include <queue>
#include <unordered_map>
#include "../nvm_chip/flash_memory/FlashTypes.h"
#include "SSD_Defs.h"
#include "Data_Cache_Manager_Base.h"
#include "NVM_Transaction_Flash.h"

namespace SSD_Components
{
	enum class Cache_Slot_Status { EMPTY, CLEAN, DIRTY_NO_FLASH_WRITEBACK, DIRTY_FLASH_WRITEBACK };//缓存槽状态，最后两个分别是代表（缓存槽中的数据已经被修改，但尚未写回到后备存储介质中）和（缓存槽中的数据已经被修改，并且需要将这些修改写回到闪存中）
	struct Data_Cache_Slot_Type
	{
		unsigned long long State_bitmap_of_existing_sectors;//已经在缓存中的扇区
		LPA_type LPA;//uint64_t
		data_cache_content_type Content;//uint64_t
		data_timestamp_type Timestamp;//uint64_t
		Cache_Slot_Status Status;
		std::list<std::pair<LPA_type, Data_Cache_Slot_Type*>>::iterator lru_list_ptr;//used for fast implementation of LRU，这是个迭代器
	};

	enum class Data_Cache_Simulation_Event_Type {
		MEMORY_READ_FOR_CACHE_EVICTION_FINISHED,
		MEMORY_WRITE_FOR_CACHE_FINISHED,
		MEMORY_READ_FOR_USERIO_FINISHED,
		MEMORY_WRITE_FOR_USERIO_FINISHED
	};

	struct Memory_Transfer_Info
	{
		unsigned int Size_in_bytes;
		void* Related_request;
		Data_Cache_Simulation_Event_Type next_event_type;
		stream_id_type Stream_id;
	};

	class Data_Cache_Flash
	{
	public:
		Data_Cache_Flash(unsigned int capacity_in_pages = 0);
		~Data_Cache_Flash();
		bool Exists(const stream_id_type streamID, const LPA_type lpn);
		bool Check_free_slot_availability();
		bool Check_free_slot_availability(unsigned int no_of_slots);
		bool Empty();
		bool Full();
		Data_Cache_Slot_Type Get_slot(const stream_id_type stream_id, const LPA_type lpn);
		Data_Cache_Slot_Type Evict_one_dirty_slot();
		Data_Cache_Slot_Type Evict_one_slot_lru();
		void Change_slot_status_to_writeback(const stream_id_type stream_id, const LPA_type lpn);
		void Remove_slot(const stream_id_type stream_id, const LPA_type lpn);
		void Insert_read_data(const stream_id_type stream_id, const LPA_type lpn, const data_cache_content_type content, const data_timestamp_type timestamp, const page_status_type state_bitmap_of_read_sectors);
		void Insert_write_data(const stream_id_type stream_id, const LPA_type lpn, const data_cache_content_type content, const data_timestamp_type timestamp, const page_status_type state_bitmap_of_write_sectors);
		void Update_data(const stream_id_type stream_id, const LPA_type lpn, const data_cache_content_type content, const data_timestamp_type timestamp, const page_status_type state_bitmap_of_write_sectors);
	private:
		std::unordered_map<LPA_type, Data_Cache_Slot_Type*> slots;//缓存槽，缓存的数据存储在这
		std::list<std::pair<LPA_type, Data_Cache_Slot_Type*>> lru_list;//lru链表，缓存中访问过的数据要放在最前面
		unsigned int capacity_in_pages;//容量
	};
}

#endif // !DATA_CACHE_FLASH_H
