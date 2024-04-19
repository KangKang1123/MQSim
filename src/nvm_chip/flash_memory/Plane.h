#ifndef PLANE_H
#define PLANE_H

#include "../NVM_Types.h"
#include "FlashTypes.h"
#include "Block.h"
#include "Flash_Command.h"

namespace NVM
{
	namespace FlashMemory
	{
		class Plane
		{
		public:
			Plane(unsigned int BlocksNoPerPlane, unsigned int PagesNoPerBlock);
			~Plane();
			Block** Blocks;
			unsigned int Healthy_block_no;//健康的块数量
			unsigned long Read_count;                     //how many read count in the process of workload
			unsigned long Progam_count;
			unsigned long Erase_count;
			stream_id_type* Allocated_streams;//指的是什么流？
		};
	}
}
#endif // !PLANE_H
