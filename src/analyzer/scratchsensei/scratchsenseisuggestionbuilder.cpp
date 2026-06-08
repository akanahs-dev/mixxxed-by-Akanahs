#include "analyzer/scratchsensei/scratchsenseisuggestionbuilder.h"
#include "analyzer/scratchsensei/scratchsenseisummary.h"
#include <cmath>

namespace mixxx::scratchsensei {

mixxx::track::io::ScratchSenseiTrackAnalysis ScratchSenseiSuggestionBuilder::buildAnalysis(
        TrackId trackId,
        const QString& providerVersion,
        const ProviderResult& result,
        double sampleRate) {
    mixxx::track::io::ScratchSenseiTrackAnalysis analysis;
    analysis.set_schema_version(kSchemaVersion);
    analysis.set_analyzer_version(analysisVersionForEssentiaVersion(providerVersion).toStdString());
    analysis.set_track_id(trackId.toVariant().toInt());
    analysis.set_duration_seconds(result.durationSeconds);

    bool hasEnergy = false;
    bool hasFlux = false;
    bool hasOnsets = false;
    bool hasBeatStrength = false;

    const QVector<double>* energyCurve = nullptr;
    const QVector<double>* fluxCurve = nullptr;
    const QVector<double>* onsetCurve = nullptr;
    const QVector<double>* beatStrengthCurve = nullptr;
    double hopSeconds = 8.0; // Default fallback

    for (const auto& series : result.timelines) {
        auto* tl = analysis.add_descriptor_timeline();
        tl->set_name(series.name.toStdString());
        tl->set_hop_seconds(series.hopSeconds);
        for (double val : series.values) {
            tl->add_value(val);
        }

        if (series.name == "energy_norm") {
            hasEnergy = true;
            energyCurve = &series.values;
            hopSeconds = series.hopSeconds;
        } else if (series.name == "spectral_flux_norm") {
            hasFlux = true;
            fluxCurve = &series.values;
        } else if (series.name == "onset_density_norm") {
            hasOnsets = true;
            onsetCurve = &series.values;
        } else if (series.name == "beat_strength_norm") {
            hasBeatStrength = true;
            beatStrengthCurve = &series.values;
        }
    }

    bool beatgridAvailable = (result.bpm > 0.0 && !result.beatTimesSeconds.isEmpty());
    auto* bg = analysis.mutable_beatgrid_snapshot();
    bg->set_available(beatgridAvailable);
    if (beatgridAvailable) {
        bg->set_bpm(result.bpm);
        bg->set_source("Mixxx/Essentia");
    }

    auto* caps = analysis.mutable_capabilities();
    caps->set_has_energy_curve(hasEnergy);
    caps->set_has_phrase_candidates(hasEnergy && hasOnsets);
    caps->set_has_beatgrid_snapshot(beatgridAvailable);
    caps->set_has_reliable_tempo(beatgridAvailable && result.beatConfidence >= 0.5);
    caps->set_has_suggestion_evidence(true);
    caps->set_analysis_stale(false);

    if (hasEnergy && energyCurve && !energyCurve->isEmpty()) {
        int n = energyCurve->size();
        for (int i = 1; i < n; ++i) {
            double t = i * hopSeconds;
            double energyVal = (*energyCurve)[i];
            double prevEnergyVal = (*energyCurve)[i - 1];
            double deltaE = energyVal - prevEnergyVal;

            double fluxVal = (hasFlux && fluxCurve && fluxCurve->size() > i) ? (*fluxCurve)[i] : 0.0;
            double onsetVal = (hasOnsets && onsetCurve && onsetCurve->size() > i) ? (*onsetCurve)[i] : 0.0;
            double beatStrengthVal = (hasBeatStrength && beatStrengthCurve && beatStrengthCurve->size() > i) ? (*beatStrengthCurve)[i] : 0.0;

            int64_t roundedFrame = (sampleRate > 0.0) ? static_cast<int64_t>(std::round(t * sampleRate))
                                                      : static_cast<int64_t>(std::round(t * 44100.0));

            // Helper lambda to map a suggestion
            auto fillSuggestion = [&](const std::string& type, const std::string& label, double confidence, const std::string& reason, double energyD) {
                auto* sug = analysis.add_suggestion();
                sug->set_type(type);
                sug->set_label(label);
                sug->set_time_seconds(t);
                if (sampleRate > 0.0) {
                    sug->set_frame_position(roundedFrame);
                    sug->set_sample_rate(sampleRate);
                }

                std::string sugId = QString("%1_%2_%3")
                                            .arg(trackId.toVariant().toInt())
                                            .arg(QString::fromStdString(type))
                                            .arg(roundedFrame)
                                            .toStdString();
                sug->set_id(sugId);
                sug->set_confidence(confidence);

                if (confidence >= 0.75) {
                    sug->set_confidence_bucket("Strong");
                } else if (confidence >= 0.45) {
                    sug->set_confidence_bucket("Maybe");
                } else {
                    sug->set_confidence_bucket("Weak");
                }

                auto* ev = sug->mutable_evidence();
                ev->set_energy_delta(energyD);
                ev->set_spectral_flux(fluxVal);
                ev->set_onset_density(onsetVal);
                ev->set_beat_strength(beatStrengthVal);
                ev->set_reason(reason);

                if (beatgridAvailable) {
                    int closestBeatIndex = 0;
                    double minDiff = std::abs(result.beatTimesSeconds[0] - t);
                    for (int b = 1; b < result.beatTimesSeconds.size(); ++b) {
                        double diff = std::abs(result.beatTimesSeconds[b] - t);
                        if (diff < minDiff) {
                            minDiff = diff;
                            closestBeatIndex = b;
                        }
                    }
                    sug->set_beat_index(closestBeatIndex);
                    sug->set_bar_index(closestBeatIndex / 4);
                    sug->set_phrase_index(closestBeatIndex / 32);
                }
            };

            if (deltaE >= 0.3) {
                fillSuggestion("energy_lift", "Possible energy lift", deltaE, "Significant energy increase detected.", deltaE);
            }
            if (deltaE <= -0.3) {
                fillSuggestion("energy_dip", "Possible energy dip", -deltaE, "Significant energy decrease detected.", deltaE);
            }
            if (fluxVal >= 0.7) {
                fillSuggestion("section_change", "Possible section change", fluxVal, "High spectral flux indicates structural boundary.", deltaE);
            }
            if (onsetVal >= 0.7) {
                fillSuggestion("phrase_change", "Possible phrase change", onsetVal, "Peak in onset density suggests phrase boundary.", deltaE);
            }
        }
    }

    return analysis;
}

} // namespace mixxx::scratchsensei
