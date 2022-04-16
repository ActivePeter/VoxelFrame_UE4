// Fill out your copyright notice in the Description page of Project Settings.


#include "PartServerActor.h"
//#include "net/vf_pack.h"
// Sets default values
APartServerActor::APartServerActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//network_manager = std::make_shared<VF::NetworkManager>();
}


// Called when the game starts or when spawned
void APartServerActor::BeginPlay()
{
	Super::BeginPlay();
	VF_LogWarning("APartServerActor BeginPlay");

	network_manager = std::make_shared<VF::NetworkManager>();
	network_manager->connectServer(network_manager,
		"127.0.0.1"/*FIPv4Address(127, 0, 0, 1)*/, 7776);

	{//首次连接后，发送
		auto cfs = std::make_shared<ClientFirstConfirm>();
		cfs->set_client_type(ClientType_GameServer);
		network_manager->send(VF::PackContainer_make(cfs));
	}
}

void APartServerActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	VF_LogWarning("APartServerActor::EndPlay");
	network_manager->close();
	network_manager = nullptr;
}


// Called every frame
void APartServerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

