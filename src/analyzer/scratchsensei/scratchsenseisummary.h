#pragma once

#include <QByteArray>
#include <QString>

#include "proto/scratchsensei.pb.h"

namespace mixxx::scratchsensei {

constexpr const char* kSchemaVersion = "scratch-sensei-v1";
constexpr const char* kAnalysisDescription = "Scratch Sensei Track Analysis";
constexpr const char* kAnalysisVersionPrefix = "scratch-sensei-v1:";

QByteArray serializeTrackAnalysis(
        const mixxx::track::io::ScratchSenseiTrackAnalysis& analysis);

bool parseTrackAnalysis(
        const QByteArray& data,
        mixxx::track::io::ScratchSenseiTrackAnalysis* analysis);

QString analysisVersionForEssentiaVersion(const QString& essentiaVersion);

} // namespace mixxx::scratchsensei
