#include "KnucklesLiveLinkRuntime.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

// Sets default values for this component's properties
UKnucklesLiveLinkRuntime::UKnucklesLiveLinkRuntime()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UKnucklesLiveLinkRuntime::BeginPlay()
{
	Super::BeginPlay();
	
}

bool UKnucklesLiveLinkRuntime::ConnectToKnucklesSource(bool GetKnucklesSkelAnim, bool LeftWithController, bool RightWithController)
{	
	// Initialise
	bSteamVRPresent = false;
	NewSource = MakeShared<FKnucklesLiveLinkSource>();

	if (NewSource)
	{
		// Customize source output
		NewSource->bWithKnucklesAnim = GetKnucklesSkelAnim;
		NewSource->bRangeWithControllerL = LeftWithController;
		NewSource->bRangeWithControllerR = RightWithController;

		if (LiveLinkClient == nullptr)
		{
			IModularFeatures& ModularFeatures = IModularFeatures::Get();
			if (ModularFeatures.IsModularFeatureAvailable(ILiveLinkClient::ModularFeatureName))
			{
				LiveLinkClient = &ModularFeatures.GetModularFeature<ILiveLinkClient>(ILiveLinkClient::ModularFeatureName);
			}
		}

		if (LiveLinkClient)
		{
			// Add Source to Client
			LiveLinkClient->AddSource(NewSource);

			// Get SteamVRSystem
			if (NewSource->SteamVRSystem)
			{
				VRSystem = NewSource->SteamVRSystem;
			}

			// Get Source State
			bSteamVRPresent = NewSource->bSteamVRPresent;
			bLeftKnucklesPresent = NewSource->bLeftKnucklesPresent;
			bRightKnucklesPresent = NewSource->bRightKnucklesPresent;
		}
	}

	return bSteamVRPresent;
}

void UKnucklesLiveLinkRuntime::CleanupKnucklesSource()
{
	if (LiveLinkClient)
	{
		if (NewSource)
		{
			LiveLinkClient->RemoveSource(NewSource);
			NewSource = nullptr;

		}

		LiveLinkClient = nullptr;
	}
}

void UKnucklesLiveLinkRuntime::PlayKnucklesHapticFeedback(bool VibrateLeft, float StartSecondsFromNow, float DurationSeconds, float Frequency, float Amplitude /*= 0.5f*/)
{
	if (bSteamVRPresent && VRSystem)
	{
		if (Amplitude < 0.f)
		{
			Amplitude = 0.f;
		}
		else if (Amplitude > 1.f)
		{
			Amplitude = 1.f;
		}
		
		if (VibrateLeft && bLeftKnucklesPresent)
		{
			// TODO: Implement in VRKnucklesController instead; use errorhandling there and action manifest
			VRInput()->GetActionHandle(TCHAR_TO_UTF8(*FString(TEXT("/actions/main/out/VibrateLeft"))), &vrKnucklesVibrationLeft);
			VRInput()->TriggerHapticVibrationAction(vrKnucklesVibrationLeft, StartSecondsFromNow, 
				DurationSeconds, Frequency, Amplitude, k_ulInvalidInputValueHandle);
		}
		else if (bRightKnucklesPresent)
		{
			// TODO: Implement in VRKnucklesController instead; use errorhandling there and action manifest
			VRInput()->GetActionHandle(TCHAR_TO_UTF8(*FString(TEXT("/actions/main/out/VibrateRight"))), &vrKnucklesVibrationRight);
			VRInput()->TriggerHapticVibrationAction(vrKnucklesVibrationRight, StartSecondsFromNow, 
				DurationSeconds, Frequency, Amplitude, k_ulInvalidInputValueHandle);
		}
	}
}
