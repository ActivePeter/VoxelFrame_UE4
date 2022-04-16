//#include "ChunkEvent.h"
#include "PackReceiveEvent.h"
#include "../Chunk.h"
#include "../GameContext.h"
#include "../main_player.h"
#include "game/main_player_Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "net/vf_NetworkManager.h"
#include "pack_receive_distribute/pack_recv_handle_inc.h"

namespace VF
{
	namespace _Event
	{
		void VF::_Event::PackRecieveEvent::run(GameContext& context)
		{
			auto& cp = this->pack_container.proto_pack;

			switch (this->pack_container.pack_id)
			{
				MACRO_pack_recv_distribute;
			}
		}

		void PackRecieveEvent::IVF_Obj_init(ContextId ID)
		{
			IVF_Obj::IVF_Obj_init(ID);
		}

		void PackRecieveEvent::IVF_Obj_begin()
		{
		}
		void PackRecieveEvent::IVF_Obj_end()
		{
		}
	}
}
