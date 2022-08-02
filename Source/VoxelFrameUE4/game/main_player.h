#pragma once

#include "vf_entity.h"
#include "GameContext.h"
#include "paecs/paecs.h"

class AVoxelFrameUE4Character;
class GameContext;

namespace VF
{
	namespace _main_player
	{
		struct PlayerTrans
		{
			FVector pos;
			FRotator rot;
			FVector velo;
			PlayerTrans(FVector pos1, FRotator rot1, FVector velo1)
				:pos(pos1), rot(rot1), velo(velo1) {}
		};

		struct NetSyncData
		{
			//client 2 server
			enum JumpCmd
			{
				No = 0,
				Jump = 1,
				StopJump = 2,
			};
			bool rotated = false;//记录是否旋转
			float move_forward = 0;//记录是否前进后退
			float move_right = 0;//记录是否左右移动
			char jump_cmd = 0;

			//server 2 client
			//double last_sync_time = 0;
		};
		//struct ControlMovement
		//{
		//	//FVector movement_vec;
		//	bool rotate;
		//	std::list<PlayerTrans>* history;//push to begin,read from begin
		//	float last_sync_time;
		//};
		struct AllCompPtr
		{
			AVoxelFrameUE4Character** character;

			//ControlMovement* control_movement;
			//ServerEntityId* server_eid;
		};
		void init(GameContext& context, AVoxelFrameUE4Character* player);
		AllCompPtr get_mainplayer_comps_by_eid(paecs::Scene& ecs, paecs::EntityID& eid);
		void sync_from_server(GameContext& context, const EntityPos& ep);
		namespace _ecs_sys
		{
			//void free_player_raw_ptr(ControlMovement& movement);
			void main_player_update(ContextId& cid, ServerEntityIdCotainer& seid, AVoxelFrameUE4Character*& player, NetSyncData& movement);

		}
		namespace _main_player_update
		{
			void player_movement_sync(ContextId& cid, ServerEntityIdCotainer& seid, AVoxelFrameUE4Character*& player, NetSyncData& movement);
			void checkPlayerRay2Chunk(ContextId& cid, AVoxelFrameUE4Character*& player);
		}
	}
}
