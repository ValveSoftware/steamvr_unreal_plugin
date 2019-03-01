#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "openvr.h"
#include "KnucklesLiveLinkSource.h"
#include "ILiveLinkClient.h"
#include "KnucklesLiveLinkRuntime.generated.h"

using namespace vr;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KNUCKLESLIVELINK_API UKnucklesLiveLinkRuntime : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UKnucklesLiveLinkRuntime();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable, Category="Knuckles")
	bool ConnectToKnucklesSource(bool GetKnucklesSkelAnim = true, bool LeftWithController = false, bool RightWithController = false);

	UFUNCTION(BlueprintCallable, Category = "Knuckles")
	void CleanupKnucklesSource();

	UFUNCTION(BlueprintCallable, Category = "Knuckles")
	void PlayKnucklesHapticFeedback(bool VibrateLeft, float StartSecondsFromNow, float DurationSeconds = 1.f, 
		float Frequency = 1.f, float Amplitude = 0.5f);

	/* Whether SteamVR is active or not */
	UPROPERTY(BlueprintReadOnly, Category = "VR")
	bool bSteamVRPresent;

	/* Whether Right Knuckles is present or not */
	UPROPERTY(BlueprintReadOnly, Category = "VR")
	bool bRightKnucklesPresent;

	/* Whether Left Knuckles is present or not */
	UPROPERTY(BlueprintReadOnly, Category = "VR")
	bool bLeftKnucklesPresent;

private:
	void GetInputError(EVRInputError InputError, FString InputAction);

	// SteamVR Input System
	IVRSystem* VRSystem = nullptr;
	VRActionHandle_t vrKnucklesVibrationLeft;
	VRActionHandle_t vrKnucklesVibrationRight;

	// Setup LiveLinkClient
	TSharedPtr<FKnucklesLiveLinkSource> NewSource = nullptr;
	ILiveLinkClient* LiveLinkClient;

	// Knuckles Controller IDs
	int KnucklesControllerIdLeft = -1;
	int KnucklesControllerIdRight = -1;
};
