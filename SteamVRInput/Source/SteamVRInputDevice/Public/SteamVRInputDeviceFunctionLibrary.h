#pragma once

#include "Core.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SteamVRInputDeviceFunctionLibrary.generated.h"

#define STEAMVRCONTROLLER_SUPPORTED_PLATFORMS (PLATFORM_MAC || (PLATFORM_LINUX && PLATFORM_CPU_X86_FAMILY && PLATFORM_64BITS) || (PLATFORM_WINDOWS && WINVER > 0x0502))

//#define ACTION_MANIFEST					"steamvr_manifest.json"
#define ACTION_PATH_VIBRATE_LEFT		"/actions/main/out/vibrateleft"
#define ACTION_PATH_VIBRATE_RIGHT		"/actions/main/out/vibrateright"

/*
 * Helper Library for Knuckles Controllers
 */
UCLASS()
class STEAMVRINPUTDEVICE_API USteamVRInputDeviceFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SteamVR")
	static void PlaySteamVR_HapticFeedback(bool VibrateLeft, float StartSecondsFromNow, float DurationSeconds = 1.f,
			float Frequency = 1.f, float Amplitude = 0.5f);
};
