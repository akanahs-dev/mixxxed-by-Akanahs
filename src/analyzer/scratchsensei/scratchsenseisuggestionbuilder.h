#pragma once

#include "proto/scratchsensei.pb.h"
#include "analyzer/scratchsensei/scratchsenseianalysisprovider.h"
#include "track/trackid.h"

namespace mixxx::scratchsensei {

class ScratchSenseiSuggestionBuilder {
  public:
    static mixxx::track::io::ScratchSenseiTrackAnalysis buildAnalysis(
            TrackId trackId,
            const QString& providerVersion,
            const ProviderResult& result,
            double sampleRate);
};

} // namespace mixxx::scratchsensei
