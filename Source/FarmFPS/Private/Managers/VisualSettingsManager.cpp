// Fill out your copyright notice in the Description page of Project Settings.

#include "Managers/VisualSettingsManager.h"

// UE
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/GameUserSettings.h"
#include "Layout/SlateRect.h"
#include "Widgets/SWindow.h" // For FSlateApplication if needed, or query screen 

UVisualSettingsManager::UVisualSettingsManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UVisualSettingsManager::BeginPlay()
{
	Super::BeginPlay();

    SetResolution();

    FString SteamDeckEnv = FPlatformMisc::GetEnvironmentVariable(TEXT("SteamDeck"));
    if (SteamDeckEnv.Equals(TEXT("1")))
    {
        //SetVisualSettings(EVisualsValue::Bad);
        SetAutoSettings();
    }
}

void UVisualSettingsManager::SetResolution()
{
    // Retrieve the game user settings instance
    UGameUserSettings* UserSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr;
    if (!UserSettings) return;

    // Set the screen resolution
    UserSettings->SetScreenResolution(UserSettings->GetDesktopResolution());

    // Optionally set fullscreen mode (e.g., EWindowMode::Fullscreen or EWindowMode::WindowedFullscreen)
    UserSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);

    // Apply the settings and save to disk
    UserSettings->ApplyResolutionSettings(false);
    UserSettings->ApplySettings(false);

    // GOT THIS SHIT OFF OF GOOOGLE LOOL
}

void UVisualSettingsManager::SetAutoSettings()
{
    // Get the user settings instance
    UGameUserSettings* UserSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr;

    if (UserSettings)
    {
        // 1. Run the hardware benchmark to evaluate CPU and GPU capabilities
        // The scale can range from 0.0 (low) to higher values depending on hardware
        UserSettings->RunHardwareBenchmark();

        // 2. Apply the recommended scalability settings based on the benchmark
        UserSettings->ApplyHardwareBenchmarkResults();

        // 3. Save the applied settings to disk (GameUserSettings.ini)
        UserSettings->SaveSettings();
    }
}

void UVisualSettingsManager::SetVisualSettings(EVisualsValue visualsValue)
{
    UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
    if (!Settings) return;

    int visualSetting = (int)visualsValue;

    // --- 1. Resolution & Display Mode ---
    SetResolution();

    // --- 2. Frame Rate & VSync ---
    Settings->SetFrameRateLimit(144.0f);
    Settings->SetVSyncEnabled(true);

    // --- 3. Scalability Quality Levels (0 = Low, 3 = Epic, 4 = Cinematic) ---
    Settings->SetViewDistanceQuality(visualSetting);
    Settings->SetAntiAliasingQuality(visualSetting);
    Settings->SetShadowQuality(visualSetting);
    Settings->SetGlobalIlluminationQuality(visualSetting);
    Settings->SetPostProcessingQuality(visualSetting);
    Settings->SetTextureQuality(visualSetting);
    Settings->SetVisualEffectQuality(visualSetting);
    Settings->SetFoliageQuality(visualSetting);
    Settings->SetShadingQuality(visualSetting);
    Settings->SetReflectionQuality(visualSetting);

    // --- 4. Apply and Save ---
    // true = checks for resolution changes and runs a benchmark if necessary
    Settings->ApplySettings(true);
    Settings->SaveSettings();
}

void UVisualSettingsManager::CycleVisualsPreset()
{
    int currentPreset = (int)_currentVisualsPreset;
    currentPreset = (currentPreset + 1) % 4;
    _currentVisualsPreset = (EVisualsValue)currentPreset;

    SetVisualSettings(_currentVisualsPreset);
}
