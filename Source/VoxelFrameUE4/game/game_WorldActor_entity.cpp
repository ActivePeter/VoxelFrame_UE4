//#include "game_WorldActor.h"
//#include "VF_EntityPlayerCpp.h"
//
//
//#define __load(__path__,__cpp_parent__,__enum__)\
//{\
//	UClass* entity = LoadClass<__cpp_parent__>(nullptr,\
//		TEXT(__path__));\
//	if (!entity)\
//	{\
//		VF_LogWarning("entity_load_class_models failed")\
//			return;\
//	}\
//	this->context->entity_manager.add_entity_class(\
//	__enum__, entity);\
//}
//
//void AWorldActor::entity_load_class_models()
//{
//	__load(
//		"Blueprint'/Game/vf_entity/VF_EntityPlayer.VF_EntityPlayer_C'",
//		AVF_EntityPlayerCpp,
//		VF::EntityType::Player
//	)
//		//UClass* entity = LoadClass<AVF_EntityPlayerCpp>(nullptr,
//		//	TEXT("Blueprint'/Game/vf_entity/VF_EntityPlayer.VF_EntityPlayer_C'"));
//		////Blueprint'/Game/vf_entity/VF_EntityPlayer.VF_EntityPlayer'
//		//if (!entity)
//		//{
//		//	VF_LogWarning("entity_load_class_models failed")
//		//		return;
//		//}
//		//this->context->entity_manager.add_entity_class(
//		//	VF::EntityType::Player, entity);
//
//}
