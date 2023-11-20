// Fill out your copyright notice in the Description page of Project Settings.


#include "MOBAGameInstance.h"

#include "GameFramework/GameModeBase.h"

#include "OnlineSessionSettings.h"

#include "MenuSystem/MenuWidget.h"
#include "MenuSystem//MainMenu.h"

#include "ABPlayerState.h"

const static FName SESSION_NAME = NAME_GameSession;
const static FName ROOM_NAME_KEY = TEXT("Room Name");
UMOBAGameInstance::UMOBAGameInstance(const FObjectInitializer& ObjectIn ...)
{
	ConstructorHelpers::FClassFinder<UMenuWidget> mainMenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
	if (mainMenuBPClass.Class == nullptr) return;
	mainMenuClass = mainMenuBPClass.Class;

	ConstructorHelpers::FClassFinder<UMenuWidget> inGameEscmenuBPClass(TEXT("/Game/MenuSystem/WBP_InGameEscMenu"));
	if (inGameEscmenuBPClass.Class == nullptr) return;
	inGameEscMenuClass = inGameEscmenuBPClass.Class;
}

void UMOBAGameInstance::Init()
{
	UGameInstance::Init();

	InitOnlineSubsystem();
}
void UMOBAGameInstance::InitOnlineSubsystem()
{
	IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();
	if (onlineSubsystem == nullptr)return;
	
	UE_LOG(LogTemp, Warning, TEXT("online subsystem name: %s"), *onlineSubsystem->GetSubsystemName().ToString());
	sessionInterface = onlineSubsystem->GetSessionInterface();
	if (sessionInterface.IsValid() == false) return;
	UE_LOG(LogTemp, Warning, TEXT("Get sessionInterface clear"));
	sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMOBAGameInstance::OnCreateSessionComplete);
	sessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMOBAGameInstance::OnDestroySessionComplete);
	sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMOBAGameInstance::OnFindSessionComplete);
	sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMOBAGameInstance::OnJoinSessionComplete);

	if (GEngine == nullptr) return;
	GEngine->OnNetworkFailure().AddUObject(this, &UMOBAGameInstance::OnNetworkFailure);
}

void UMOBAGameInstance::LoadMainMenu()
{
	if (mainMenuClass == nullptr)return;
	mainMenu = CreateWidget<UMainMenu>(this, mainMenuClass);

	mainMenu->Setup();
	mainMenu->SetMenuInterface(this);
}
void UMOBAGameInstance::LoadInGameEscMenu()
{
	if (inGameEscMenuClass == nullptr)return;
	UMenuWidget* inGameEscMenu = CreateWidget<UMenuWidget>(this, inGameEscMenuClass);

	inGameEscMenu->Setup();
	inGameEscMenu->SetMenuInterface(this);
}

void UMOBAGameInstance::OnHostButtonClicked(FString roomName)
{
	if (sessionInterface == nullptr) return;
	desiredRoomName = roomName;
	UE_LOG(LogTemp, Warning, TEXT("UMOBAGameInstance::OnHostButtonClicked"));
	auto ExistingSession = sessionInterface->GetNamedSession(SESSION_NAME);
	if (ExistingSession != nullptr)
	{
		DestroySession();
	}
	else
	{
		CreateSession();
	}

}
void UMOBAGameInstance::DestroySession()
{
	if (sessionInterface.IsValid() == false) return;
	UE_LOG(LogTemp, Warning, TEXT("UMOBAGameInstance::DestroySession()"));
	sessionInterface->DestroySession(SESSION_NAME);
}
void UMOBAGameInstance::OnDestroySessionComplete(FName sessionName, bool isSucceed)
{
	UE_LOG(LogTemp, Warning, TEXT("OnDestroySessionComplete %s, %d"),*sessionName.ToString(),isSucceed);

	CreateSession();
}
void UMOBAGameInstance::CreateSession()
{
	if (sessionInterface == nullptr) return;
	UE_LOG(LogTemp, Warning, TEXT("UMOBAGameInstance::CreateSession()"));
	FOnlineSessionSettings sessionSetting;
	if(IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		sessionSetting.bIsLANMatch = true;
	else
		sessionSetting.bIsLANMatch = false;
	sessionSetting.NumPublicConnections = 5 ;
	sessionSetting.bShouldAdvertise = true;
	sessionSetting.bUsesPresence = true;
	sessionSetting.bUseLobbiesIfAvailable = true;
	sessionSetting.Set(ROOM_NAME_KEY, desiredRoomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	UE_LOG(LogTemp, Warning, TEXT("desiredRoomName : %s"), *desiredRoomName);
	sessionInterface->CreateSession(0,SESSION_NAME, sessionSetting);
}
void UMOBAGameInstance::OnCreateSessionComplete(FName sessionName, bool isSucceed)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete name: %s, %d"), *sessionName.ToString(), isSucceed);
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete ID: %s"), *(sessionInterface->GetNamedSession(sessionName)->GetSessionIdStr()));

	UEngine* engine = GetEngine();
	if (engine == nullptr) return;
	engine->AddOnScreenDebugMessage(1, 5, FColor::Green, FString::Printf(TEXT("OnCreateSessionComplete ID: %s"), *(sessionInterface->GetNamedSession(sessionName)->GetSessionIdStr())));


	Host();
}
void UMOBAGameInstance::Host()
{
	UE_LOG(LogTemp, Warning, TEXT(" UMOBAGameInstance::Host()"));
	GetWorld()->ServerTravel("/Game/MOBA/Maps/Lobby?listen");
}


void UMOBAGameInstance::OnJoinMenuButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("UMOBAGameInstance::OnJoinMenuButtonClicked()"));
	FindSession();
}
void UMOBAGameInstance::FindSession()
{
	UE_LOG(LogTemp, Warning, TEXT("UMOBAGameInstance::FindSession()"));
	UEngine* engine = GetEngine();
	if (engine == nullptr) return;
	engine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("UMOBAGameInstance::FindSession()")));

	if (sessionInterface.IsValid() == false) return;
	sessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (sessionSearch.IsValid()== false)return;

	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		sessionSearch->bIsLanQuery = true;
	else
		sessionSearch->bIsLanQuery = false;
	sessionSearch->MaxSearchResults = 100;
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	CheckSearchOptions();

	sessionInterface->FindSessions(0,sessionSearch.ToSharedRef());
}
void UMOBAGameInstance::OnFindSessionComplete(bool isSucceed)
{
	UE_LOG(LogTemp, Warning, TEXT("UMOBAGameInstance::OnFindSessionComplete %d"),isSucceed);

	UEngine* engine = GetEngine();
	if (engine == nullptr) return;
	engine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("UMOBAGameInstance::OnFindSessionComplete %d"), isSucceed));

	if(isSucceed)
		RefrsehServerList();
}
void UMOBAGameInstance::RefrsehServerList()
{
	if (mainMenu == nullptr) return;
	if (sessionSearch.IsValid() == false) return;


	UE_LOG(LogTemp, Warning, TEXT("UMOBAGameInstance::RefrsehServerList(), %d results"), sessionSearch->SearchResults.Num());
	UEngine* engine = GetEngine();
	if (engine == nullptr) return;
	engine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("UMOBAGameInstance::RefrsehServerList(), %d results"), sessionSearch->SearchResults.Num()));

	TArray<FServerData> serverData;
	for (auto& result : sessionSearch->SearchResults)
	{
		
		FServerData data;

		data.hostUserName = result.Session.OwningUserName;
		data.maxPlayerCount = result.Session.SessionSettings.NumPublicConnections;
		data.currentPlayerCount = data.maxPlayerCount - result.Session.NumOpenPublicConnections;//최대 슬롯 - 비어있는 슬롯 수
		FString roomNameValue;
		if (result.Session.SessionSettings.Get(ROOM_NAME_KEY, roomNameValue))
		{
			data.serverName = roomNameValue;
		}
		else
		{
			data.serverName = roomNameValue;
		}

		serverData.Add(data);
	}
	mainMenu->SetServerList(serverData);
}

void UMOBAGameInstance::DevMode(bool check)
{
}



void UMOBAGameInstance::StartSession()
{
	if (sessionInterface.IsValid() == false) return;
	sessionInterface->StartSession(SESSION_NAME);
}



void UMOBAGameInstance::OnJoinButtonClicked(uint32 idx)
{
	serverIndex = idx;
	JoinSession(idx);
}
void UMOBAGameInstance::JoinSession(uint32 idx)
{
	if (sessionInterface.IsValid() == false) return;
	if (sessionSearch.IsValid() == false) return;

	if (mainMenu != nullptr)
	{
		mainMenu->Teardown();
	}

	auto ExistingSession = sessionInterface->GetNamedSession(SESSION_NAME);

	sessionInterface->JoinSession(0, SESSION_NAME, sessionSearch->SearchResults[idx]);
}
void UMOBAGameInstance::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	if (sessionInterface.IsValid() == false) return;

	FString address;
	if (!sessionInterface->GetResolvedConnectString(sessionName, address)) {
		UE_LOG(LogTemp, Warning, TEXT("Could not get connect string."));
		return;
	}
	Join(address);
}
void UMOBAGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	GoToMainMenuLevel();
}
void UMOBAGameInstance::Join(const FString& address)
{
	UEngine* engine = GetEngine();
	if (engine == nullptr) return;

	engine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Joining %s"), *address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel(address, ETravelType::TRAVEL_Absolute);
}


void UMOBAGameInstance::GoToMainMenuLevel()
{
	
	APlayerController* playerController = GetFirstLocalPlayerController();
	if (playerController == nullptr) return;

	playerController->ClientTravel("/Game/MenuSystem/MainMenu", ETravelType::TRAVEL_Absolute);
}

void UMOBAGameInstance::DrawLogLine()
{
	UE_LOG(LogTemp, Warning, TEXT("========================================================="));
}

void UMOBAGameInstance::CheckStat()
{
	int ttd = 10;
	UEngine* engine = GetEngine();
	if (engine == nullptr) return;
	if (sessionInterface == nullptr)
	{
		engine->AddOnScreenDebugMessage(-1, ttd, FColor::Green,
			FString::Printf(TEXT("sessionInterface nullptr")));
		return;
	}
	int32 numSessions = sessionInterface->GetNumSessions();
	engine->AddOnScreenDebugMessage(-1, ttd, FColor::Green,
		FString::Printf(TEXT("Num of Sessions: %d"), numSessions));
	auto namedOnlineSession = sessionInterface->GetNamedSession(SESSION_NAME);
	if (namedOnlineSession == nullptr)
	{
		engine->AddOnScreenDebugMessage(-1, ttd, FColor::Green,
			FString::Printf(TEXT("namedOnlineSession nullptr")));
		return;
	}
	auto sessionInfo = namedOnlineSession->SessionInfo;
	if (sessionInfo.IsValid() == false)
	{
		engine->AddOnScreenDebugMessage(-1, ttd, FColor::Green,
			FString::Printf(TEXT("sessionInfo invalid")));
		return;
	}
	auto sessionID = sessionInfo->GetSessionId().ToString();
	engine->AddOnScreenDebugMessage(-1, ttd, FColor::Green,
		FString::Printf(TEXT("Now Existing Session ID %d"), *sessionID));

}

void UMOBAGameInstance::CheckDefaultSearchOptions()
{
	sessionSearch = MakeShareable(new FOnlineSessionSearch());

	UE_LOG(LogTemp, Warning, TEXT("CheckDefaultSearchOptions()"));

	bool allowInvites = sessionSearch->GetDefaultSessionSettings()->bAllowInvites;
	UE_LOG(LogTemp, Warning, TEXT("bAllowInvites, %d"), allowInvites);

	bool joinInProgress = sessionSearch->GetDefaultSessionSettings()->bAllowJoinInProgress;
	UE_LOG(LogTemp, Warning, TEXT("bAllowJoinInProgress, %d"), joinInProgress);

	bool joinViaPresence = sessionSearch->GetDefaultSessionSettings()->bAllowJoinViaPresence;
	UE_LOG(LogTemp, Warning, TEXT("bAllowJoinViaPresence, %d"), joinViaPresence);

	bool joinViaPresenceFriendsOnly = sessionSearch->GetDefaultSessionSettings()->bAllowJoinViaPresenceFriendsOnly;
	UE_LOG(LogTemp, Warning, TEXT("bAllowJoinViaPresenceFriendsOnly, %d"), joinViaPresenceFriendsOnly);

	bool antiCheatProtect = sessionSearch->GetDefaultSessionSettings()->bAntiCheatProtected;
	UE_LOG(LogTemp, Warning, TEXT("bAntiCheatProtected, %d"), antiCheatProtect);

	bool isDedicated = sessionSearch->GetDefaultSessionSettings()->bIsDedicated;
	UE_LOG(LogTemp, Warning, TEXT("bIsDedicated, %d"), isDedicated);

	bool isLANMatch = sessionSearch->GetDefaultSessionSettings()->bIsLANMatch;
	UE_LOG(LogTemp, Warning, TEXT("bIsLANMatch, %d"), isLANMatch);

	bool shouldAdvertise = sessionSearch->GetDefaultSessionSettings()->bShouldAdvertise;
	UE_LOG(LogTemp, Warning, TEXT("bShouldAdvertise, %d"), shouldAdvertise);

	bool useLobbiesIfAvailable = sessionSearch->GetDefaultSessionSettings()->bUseLobbiesIfAvailable;
	UE_LOG(LogTemp, Warning, TEXT("bUseLobbiesIfAvailable, %d"), useLobbiesIfAvailable);

	bool useLobbiesVoiceChatIfAvailable = sessionSearch->GetDefaultSessionSettings()->bUseLobbiesVoiceChatIfAvailable;
	UE_LOG(LogTemp, Warning, TEXT("bUseLobbiesVoiceChatIfAvailable, %d"), useLobbiesVoiceChatIfAvailable);

	bool usesPresence = sessionSearch->GetDefaultSessionSettings()->bUsesPresence;
	UE_LOG(LogTemp, Warning, TEXT("bUsesPresence, %d"), usesPresence);

	bool usesStats = sessionSearch->GetDefaultSessionSettings()->bUsesStats;
	UE_LOG(LogTemp, Warning, TEXT("bUsesStats, %d"), usesStats);
}

void UMOBAGameInstance::CheckSearchOptions()
{
	if (sessionSearch.IsValid() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("sessionSearch.IsValid() == false"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("CheckSearchOptions()"));
	bool isDedicatedOnly;
	sessionSearch->QuerySettings.Get(SEARCH_DEDICATED_ONLY, isDedicatedOnly);
	UE_LOG(LogTemp, Warning, TEXT("isDedicatedOnly, %d"), isDedicatedOnly);

	bool isEmptyServersOnly;
	sessionSearch->QuerySettings.Get(SEARCH_EMPTY_SERVERS_ONLY, isEmptyServersOnly);
	UE_LOG(LogTemp, Warning, TEXT("isEmptyServersOnly, %d"), isEmptyServersOnly);

}

void UMOBAGameInstance::CheckRace()
{
	UE_LOG(LogTemp, Warning, TEXT("UMOBAGameInstance::CheckRace %d"), Race);
}





