#include "analyzer/scratchsensei/scratchsenseisummary.h"

namespace mixxx::scratchsensei {

QByteArray serializeTrackAnalysis(
        const mixxx::track::io::ScratchSenseiTrackAnalysis& analysis) {
    std::string serialized = analysis.SerializeAsString();
    return QByteArray(serialized.data(), static_cast<int>(serialized.size()));
}

bool parseTrackAnalysis(
        const QByteArray& data,
        mixxx::track::io::ScratchSenseiTrackAnalysis* analysis) {
    if (data.isEmpty() || !analysis) {
        return false;
    }
    return analysis->ParseFromArray(data.constData(), data.size());
}

QString analysisVersionForEssentiaVersion(const QString& essentiaVersion) {
    return QString(kAnalysisVersionPrefix) + essentiaVersion;
}

} // namespace mixxx::scratchsensei
