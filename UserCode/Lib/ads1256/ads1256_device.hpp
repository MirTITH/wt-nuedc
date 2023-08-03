#pragma once

#include "ads1256.hpp"
#include "Filters/butterworth.hpp"
#include <atomic>

extern Ads1256 VAds;
extern Ads1256 IAds;

extern std::atomic<float> kIAdsFilterResult;
extern std::atomic<float> kVAdsFilterResult;

void InitAds();
