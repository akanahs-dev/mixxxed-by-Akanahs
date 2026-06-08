#include "library/scratchsensei/wscratchsenseitimeline.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>

#include "moc_wscratchsenseitimeline.cpp"

WScratchSenseiTimeline::WScratchSenseiTimeline(QWidget* pParent)
        : QWidget(pParent),
          m_hasPayload(false),
          m_hoveredSuggestionIndex(-1),
          m_selectedSuggestionIndex(-1) {
    setMouseTracking(true); // to receive mouseMoveEvent without buttons pressed
    setMinimumHeight(120);
}

void WScratchSenseiTimeline::setAnalysisPayload(const QByteArray& payload) {
    m_hasPayload = mixxx::scratchsensei::parseTrackAnalysis(payload, &m_payload);
    m_selectedSuggestionIndex = -1;
    m_hoveredSuggestionIndex = -1;
    update();
}

void WScratchSenseiTimeline::setHiddenSuggestions(const QSet<QString>& hiddenIds) {
    m_hiddenIds = hiddenIds;
    update();
}

QRect WScratchSenseiTimeline::energyCurveRect() const {
    return rect().adjusted(10, 10, -10, -30);
}

int WScratchSenseiTimeline::suggestionAtPos(const QPoint& pos) const {
    if (!m_hasPayload || m_payload.duration_seconds() <= 0) {
        return -1;
    }
    
    QRect curveRect = energyCurveRect();
    int markerY = curveRect.bottom() + 10;
    
    for (int i = 0; i < m_payload.suggestion_size(); ++i) {
        const auto& suggestion = m_payload.suggestion(i);
        if (m_hiddenIds.contains(QString::fromStdString(suggestion.id()))) {
            continue;
        }
        
        double fraction = suggestion.time_seconds() / m_payload.duration_seconds();
        int x = curveRect.left() + static_cast<int>(fraction * curveRect.width());
        
        QRect markerRect(x - 5, markerY, 10, 10);
        if (markerRect.contains(pos)) {
            return i;
        }
    }
    return -1;
}

void WScratchSenseiTimeline::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    painter.fillRect(rect(), QColor(30, 30, 30));
    
    if (!m_hasPayload) {
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, tr("No reliable energy curve was found for this track."));
        return;
    }
    
    QRect curveRect = energyCurveRect();
    
    // Find energy curve timeline
    const mixxx::track::io::DescriptorTimeline* energyTimeline = nullptr;
    for (int i = 0; i < m_payload.descriptor_timeline_size(); ++i) {
        if (m_payload.descriptor_timeline(i).name() == "energy_norm") {
            energyTimeline = &m_payload.descriptor_timeline(i);
            break;
        }
    }
    
    // Draw energy curve
    if (energyTimeline && energyTimeline->value_size() > 0) {
        QPainterPath path;
        path.moveTo(curveRect.left(), curveRect.bottom());
        
        int numPoints = energyTimeline->value_size();
        for (int i = 0; i < numPoints; ++i) {
            double fraction = static_cast<double>(i) / std::max(1, numPoints - 1);
            double value = energyTimeline->value(i); // 0.0 to 1.0
            
            int x = curveRect.left() + static_cast<int>(fraction * curveRect.width());
            int y = curveRect.bottom() - static_cast<int>(value * curveRect.height());
            
            path.lineTo(x, y);
        }
        
        path.lineTo(curveRect.right(), curveRect.bottom());
        path.closeSubpath();
        
        painter.fillPath(path, QColor(0, 122, 204, 128));
        painter.setPen(QPen(QColor(0, 122, 204), 1));
        painter.drawPath(path);
    }
    
    // Draw suggestions
    int markerY = curveRect.bottom() + 10;
    for (int i = 0; i < m_payload.suggestion_size(); ++i) {
        const auto& suggestion = m_payload.suggestion(i);
        if (m_hiddenIds.contains(QString::fromStdString(suggestion.id()))) {
            continue;
        }
        
        double fraction = suggestion.time_seconds() / m_payload.duration_seconds();
        int x = curveRect.left() + static_cast<int>(fraction * curveRect.width());
        
        QRect markerRect(x - 5, markerY, 10, 10);
        
        QColor color = Qt::gray;
        if (suggestion.confidence_bucket() == "Strong") {
            color = Qt::green;
        } else if (suggestion.confidence_bucket() == "Maybe") {
            color = Qt::yellow;
        } else if (suggestion.confidence_bucket() == "Weak") {
            color = Qt::red;
        }
        
        if (i == m_selectedSuggestionIndex) {
            painter.setPen(QPen(Qt::white, 2));
        } else if (i == m_hoveredSuggestionIndex) {
            painter.setPen(QPen(Qt::white, 1));
        } else {
            painter.setPen(Qt::NoPen);
        }
        
        painter.setBrush(color);
        painter.drawEllipse(markerRect);
    }
}

void WScratchSenseiTimeline::mousePressEvent(QMouseEvent* event) {
    int index = suggestionAtPos(event->pos());
    if (index >= 0) {
        m_selectedSuggestionIndex = index;
        update();
        emit suggestionSelected(m_payload.suggestion(index));
    } else {
        m_selectedSuggestionIndex = -1;
        update();
        mixxx::track::io::Suggestion emptySuggestion;
        emit suggestionSelected(emptySuggestion);
    }
}

void WScratchSenseiTimeline::mouseMoveEvent(QMouseEvent* event) {
    int index = suggestionAtPos(event->pos());
    if (index != m_hoveredSuggestionIndex) {
        m_hoveredSuggestionIndex = index;
        update();
    }
}

void WScratchSenseiTimeline::leaveEvent(QEvent* event) {
    Q_UNUSED(event);
    if (m_hoveredSuggestionIndex != -1) {
        m_hoveredSuggestionIndex = -1;
        update();
    }
}
