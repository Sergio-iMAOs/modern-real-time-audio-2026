#include "SuperChorusLAF.h"

void SuperChorusLAF::drawPluginBackground(juce::Graphics &g, int width, int height)
{
    /*
    juce::ColourGradient grad = juce::ColourGradient::vertical (
        colour0, 0.0f,
        colour1, (float)height
    );
    g.setGradientFill(grad);
    */
    g.setColour(colour0);
    g.setColour(juce::Colours::darkslateblue);
    g.fillAll();
}

void SuperChorusLAF::drawDisplay(
    juce::Graphics& g,
    juce::Rectangle<int> bounds,
    std::vector<float> timePos,
    std::vector<float> stereoPos,
    float drive,
    float bitDepth
)
{
    const auto numVoices = std::min(timePos.size(), stereoPos.size());

    //--------------------------------------------------------------------------
    // Layout constants
    //--------------------------------------------------------------------------
    constexpr float totalAngleDeg   = 170.f;
    constexpr float marginAngleDeg  = 8.f;   // angular margin at each arc edge
    constexpr float radiusMargin    = 12.f;  // px margin at inner and outer arc edge
    constexpr int   numArcGuides    = 3;     // concentric arc grid lines
    constexpr int   numRadialGuides = 4;     // radial spoke grid lines (excludes edges)

    // The sector is symmetric about the vertical axis, centred on the bottom
    // edge of `bounds`, fanning upward.
    const float cx = bounds.getCentreX();
    const float cy = bounds.getBottom();

    // Radius spans the full height of the display
    const float halfTotalRad  = juce::degreesToRadians(totalAngleDeg * 0.5f);
    const float maxRFromWidth = static_cast<float>(bounds.getWidth()) * 0.5f
                                / std::sin(halfTotalRad);
    const float outerRadius   = std::min(static_cast<float>(bounds.getHeight()), maxRFromWidth);
    const float innerRadius   = outerRadius * 0.22f;

    // Effective drawing radii (inside margins)
    const float rMin = innerRadius + radiusMargin;
    const float rMax = outerRadius - radiusMargin;

    // JUCE angles: 0 = 12-o-clock, clockwise positive (degrees → radians)
    // We want the sector symmetric about 12-o-clock, spanning totalAngleDeg.
    // startAngle is left edge (negative half), endAngle is right edge.
    const float halfTotal   = totalAngleDeg * 0.5f;
    const float startAngleDeg = -halfTotal;   // e.g. -85°  → left
    const float endAngleDeg   =  halfTotal;   // e.g. +85°  → right

    // Effective angular range (inside margins)
    const float effectiveStartDeg = startAngleDeg + marginAngleDeg;
    const float effectiveEndDeg   = endAngleDeg   - marginAngleDeg;
    const float effectiveSpanDeg  = effectiveEndDeg - effectiveStartDeg;

    // Lambda: convert normalised [0,1] voice coordinates to Cartesian
    // JUCE rotatePointAroundOrigin convention: 0 rad = up, clockwise.
    auto toCartesian = [&](float t, float s) -> juce::Point<float>
    {
        // Map stereoPos [0,1] → angle within effective range
        const float angleDeg = effectiveStartDeg + s * effectiveSpanDeg;
        const float angleRad = juce::degreesToRadians(angleDeg);

        // Map timePos [0,1] → radius within effective range
        const float r = rMin + t * (rMax - rMin);

        // JUCE screen coords: up = negative Y, clockwise = positive X
        return { cx + r * std::sin(angleRad),
                 cy - r * std::cos(angleRad) };
    };

    //--------------------------------------------------------------------------
    // 1. Clipping region — sector shape
    //--------------------------------------------------------------------------
    juce::Path sectorClip;
    {
        const float startRad = juce::degreesToRadians(startAngleDeg);
        const float endRad   = juce::degreesToRadians(endAngleDeg);

        // Outer arc
        juce::Path outerArc;
        outerArc.addCentredArc(cx, cy, outerRadius, outerRadius,
                               0.f, startRad, endRad, true);

        // Inner arc (reversed)
        juce::Path innerArc;
        innerArc.addCentredArc(cx, cy, innerRadius, innerRadius,
                               0.f, endRad, startRad, true);

        sectorClip.addCentredArc(cx, cy, outerRadius, outerRadius,
                                 0.f, startRad, endRad, true);
        // Right spoke inward
        const auto outerRight = toCartesian(1.f, 1.f);  // approx, use raw angle
        const float srRad = juce::degreesToRadians(endAngleDeg);
        const float slRad = juce::degreesToRadians(startAngleDeg);

        sectorClip.startNewSubPath(cx + outerRadius * std::sin(srRad),
                                   cy - outerRadius * std::cos(srRad));
        sectorClip.lineTo(cx + innerRadius * std::sin(srRad),
                          cy - innerRadius * std::cos(srRad));
        sectorClip.addCentredArc(cx, cy, innerRadius, innerRadius,
                                 0.f, srRad, slRad, true); // inner arc reversed
        sectorClip.lineTo(cx + outerRadius * std::sin(slRad),
                          cy - outerRadius * std::cos(slRad));
        sectorClip.closeSubPath();
    }

    //--------------------------------------------------------------------------
    // 2. Background fill
    //--------------------------------------------------------------------------
    {
        juce::Path sector;
        const float startRad = juce::degreesToRadians(startAngleDeg);
        const float endRad   = juce::degreesToRadians(endAngleDeg);
        sector.addCentredArc(cx, cy, outerRadius, outerRadius,
                            0.f, startRad, endRad, true);
        sector.lineTo(cx + innerRadius * std::sin(endRad),
                    cy - innerRadius * std::cos(endRad));
        sector.addCentredArc(cx, cy, innerRadius, innerRadius,
                            0.f, endRad, startRad, false);
        sector.closeSubPath();

        g.setColour(juce::Colour(0xff0d1117));
        g.fillPath(sector);
    }

    //--------------------------------------------------------------------------
    // 3. Grid — concentric arc guides (time axis)
    //--------------------------------------------------------------------------
    {
        const float startRad = juce::degreesToRadians(effectiveStartDeg);
        const float endRad   = juce::degreesToRadians(effectiveEndDeg);
        g.setColour(juce::Colour(0xff1e2a38));

        for (int i = 1; i <= numArcGuides; ++i)
        {
            const float t = static_cast<float>(i) / (numArcGuides + 1);
            const float r = rMin + t * (rMax - rMin);
            juce::Path arc;
            arc.addCentredArc(cx, cy, r, r, 0.f, startRad, endRad, true);
            g.strokePath(arc, juce::PathStrokeType(0.8f));
        }
    }

    //--------------------------------------------------------------------------
    // 4. Grid — radial spoke guides (stereo axis)
    //--------------------------------------------------------------------------
    {
        g.setColour(juce::Colour(0xff1e2a38));

        // Edges
        for (float angleDeg : { effectiveStartDeg, effectiveEndDeg })
        {
            const float rad = juce::degreesToRadians(angleDeg);
            g.drawLine(cx + rMin * std::sin(rad), cy - rMin * std::cos(rad),
                       cx + rMax * std::sin(rad), cy - rMax * std::cos(rad), 0.8f);
        }

        // Interior spokes
        for (int i = 1; i <= numRadialGuides; ++i)
        {
            const float s   = static_cast<float>(i) / (numRadialGuides + 1);
            const float deg = effectiveStartDeg + s * effectiveSpanDeg;
            const float rad = juce::degreesToRadians(deg);
            // Centre spoke (stereoPos = 0.5) slightly brighter
            const bool isCentre = (i == (numRadialGuides + 1) / 2 && numRadialGuides % 2 == 0)
                                  || std::fabs(s - 0.5f) < 0.01f;
            g.setColour(isCentre ? juce::Colour(0xff2e4055) : juce::Colour(0xff1e2a38));
            g.drawLine(cx + rMin * std::sin(rad), cy - rMin * std::cos(rad),
                       cx + rMax * std::sin(rad), cy - rMax * std::cos(rad), 0.8f);
        }
    }

    //--------------------------------------------------------------------------
    // 5. Sector border
    //--------------------------------------------------------------------------
    {
        juce::Path border;
        const float startRad = juce::degreesToRadians(startAngleDeg);
        const float endRad   = juce::degreesToRadians(endAngleDeg);
        border.addCentredArc(cx, cy, outerRadius - 1.f, outerRadius - 1.f,
                            0.f, startRad, endRad, true);
        border.lineTo(cx + innerRadius * std::sin(endRad),
                    cy - innerRadius * std::cos(endRad));
        border.addCentredArc(cx, cy, innerRadius, innerRadius,
                            0.f, endRad, startRad, false);  // ← was true, BUG
        border.closeSubPath();
        g.setColour(juce::Colour(0xff2e4a6a));
        g.strokePath(border, juce::PathStrokeType(1.2f));
    }

    //--------------------------------------------------------------------------
    // 6. Voice points
    //--------------------------------------------------------------------------
    constexpr float dotRadius = 5.f;
    constexpr float glowRadius = 10.f;

    for (size_t i = 0; i < numVoices; ++i)
    {
        const float t = juce::jlimit(0.f, 1.f, timePos[i]);
        const float s = juce::jlimit(0.f, 1.f, stereoPos[i]);
        const auto  p = toCartesian(t, s);

        // Two-segment lerp: colour1 → colour4 → colour5
        auto voiceDrive = std::abs((drive / (numVoices - 1) * i) * 2.f - 1.f);
        const auto dotColour = voiceDrive < 0.5f
            ? colour1.interpolatedWith(colour4, voiceDrive * 2.f)
            : colour4.interpolatedWith(colour5, (voiceDrive - 0.5f) * 2.f);

        // Glow
        g.setColour(dotColour.withAlpha(0.18f));
        g.fillEllipse(p.x - glowRadius, p.y - glowRadius,
                      glowRadius * 2.f, glowRadius * 2.f);

        // Core dot
        g.setColour(dotColour);
        g.fillEllipse(p.x - dotRadius, p.y - dotRadius,
                      dotRadius * 2.f, dotRadius * 2.f);

        // Highlight
        g.setColour(juce::Colours::white.withAlpha(0.55f));
        g.fillEllipse(p.x - dotRadius * 0.35f, p.y - dotRadius * 0.75f,
                      dotRadius * 0.55f, dotRadius * 0.5f);
    }
}
