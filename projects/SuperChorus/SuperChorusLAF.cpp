#include "SuperChorusLAF.h"

#include "Utils.h"

SuperChorusLAF::SuperChorusLAF()
{
    setColour(juce::Label::textColourId, colour3);
}

void SuperChorusLAF::drawPluginBackground(juce::Graphics &g, int width, int height)
{
    
    juce::ColourGradient grad = juce::ColourGradient::vertical (
        colour0, 0.0f,
        colour1, (float)height
    );
    g.setGradientFill(grad);
    g.fillAll();
}


void SuperChorusLAF::drawDisplay(
    juce::Graphics& g,
    juce::Rectangle<int> bnds,
    std::vector<float> timePos,
    std::vector<float> stereoPos,
    float drive,
    float bitDepth,
    bool softClip
)
{
    const auto bounds           = bnds.toFloat();
    const auto centreX          = bounds.getX() + bounds.getWidth() * 0.5f;
    const auto centreY          = bounds.getY() + bounds.getHeight();
    const auto radiusX          = bounds.getWidth() * 0.5f;
    const auto radiusY          = bounds.getHeight();
    const auto innerRadiusProp  = 0.2f;
    const auto innerRadiusX     = radiusX * innerRadiusProp;
    const auto innerRadiusY     = radiusY * innerRadiusProp;
    constexpr auto underAngle   = M_PI / 180.f *  10.f;

    constexpr auto startAngle   = 3.f * M_PI * 0.5f + underAngle;
    constexpr auto endAngle     = 2.f * M_PI + M_PI * 0.5f - underAngle;

    // Would be better to split into multiple virtual methods
    // so parts can be customized independently

    // --- Sector ---

    // Background
    juce::Path sectorPath;
    sectorPath.addCentredArc(
        centreX,
        centreY,
        radiusX,
        radiusY,
        0.f,
        startAngle,
        endAngle,
        true
    );
    sectorPath.lineTo(
        centreX + std::cos(underAngle) * innerRadiusX,
        centreY - std::sin(underAngle) * innerRadiusY
    );
    sectorPath.addCentredArc(
        centreX,
        centreY,
        innerRadiusX,
        innerRadiusY,
        0.f,
        endAngle,
        startAngle,
        false
    );
    sectorPath.closeSubPath();
    juce::ColourGradient backgroundGradient(
        colour3,
        centreX,
        centreY,
        colour0.withAlpha(0.f),
        centreX,
        0.f,
        true
    );
    backgroundGradient.addColour(innerRadiusY / radiusY, colour3);
    backgroundGradient.addColour(0.5, colour2);
    g.setGradientFill(backgroundGradient);
    g.fillPath(sectorPath);

    // Guide grid
    constexpr auto numArcs      = 12;
    constexpr auto numLines     = 8;
    constexpr auto thickness    = 1.5f;
    juce::Path gridPath;

    // Arcs
    const auto nArcs = numArcs + 2;
    for (auto i = 0; i < nArcs - 1; ++i)
    {
        // t goes from 0 (innermost) to 1 (outermost), linearly
        const float t = static_cast<float>(i) / static_cast<float>(nArcs - 1);

        // Invert: sample the log curve from the other end
        const float logT = utils::fast_log2(2.f - t);

        // Interpolate proportion between innerRadiusProp and 1.0
        const float radiusProp = innerRadiusProp + logT * (1.f - innerRadiusProp);

        gridPath.addCentredArc(
            centreX,
            centreY,
            radiusProp * radiusX,
            radiusProp * radiusY,
            0.f,
            startAngle,
            endAngle,
            true
        );
    }

    // Lines
    const auto nLines = numLines + 2;
    for (auto i = 1; i < nLines - 1; ++i)
    {
        const float t = nLines == 1
            ? 0.5f
            : static_cast<float>(i) / static_cast<float>(nLines - 1);

        const float angle = startAngle + t * (endAngle - startAngle) - M_PI * 0.5f;

        const float cosA = std::cos(angle);
        const float sinA = std::sin(angle);

        const float outerX = centreX + radiusX * cosA;
        const float outerY = centreY + radiusY * sinA;

        const float innerX = centreX + innerRadiusX * cosA;
        const float innerY = centreY + innerRadiusY * sinA;

        gridPath.startNewSubPath(outerX, outerY);
        gridPath.lineTo(innerX, innerY);
    }

    g.setColour(colour1.withAlpha(0.25f));
    g.strokePath(gridPath, juce::PathStrokeType(thickness));
}

/*
// Mostly AI generated
void SuperChorusLAF::drawDisplay(
    juce::Graphics& g,
    juce::Rectangle<int> bounds,
    std::vector<float> timePos,
    std::vector<float> stereoPos,
    float drive,
    float bitDepth,
    bool softClip
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
        const float t = utils::fast_log2(1.f + juce::jlimit(0.f, 1.f, timePos[i]));
        const float s = juce::jlimit(0.f, 1.f, stereoPos[i]);
        const auto  p = toCartesian(t, s);

        // Two-segment lerp: colour1 → colour4 → colour5
        auto voiceDrive = drive * std::fabs(static_cast<float>(i) / static_cast<float>(numVoices - 1) * 2.f - 1.f);
        const auto dotColour = voiceDrive < 0.5f
            ? colour1.interpolatedWith(colour4, voiceDrive * 2.f)
            : colour4.interpolatedWith(colour6, (voiceDrive - 0.5f) * 2.f);

        if (softClip)
        {
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
        else
        {
            // Hard clip
            const float angle = juce::degreesToRadians(45.0f);
            const auto rotation = juce::AffineTransform::rotation(angle, p.x, p.y);

            // Glow
            {
                juce::Path path;
                path.addRectangle(
                    p.x - glowRadius,
                    p.y - glowRadius,
                    glowRadius * 2.f,
                    glowRadius * 2.f
                );

                g.setColour(dotColour.withAlpha(0.18f));
                g.fillPath(path, rotation);
            }

            // Core dot
            {
                juce::Path path;
                path.addRectangle(
                    p.x - dotRadius,
                    p.y - dotRadius,
                    dotRadius * 2.f,
                    dotRadius * 2.f
                );

                g.setColour(dotColour);
                g.fillPath(path, rotation);
            }

            // Highlight
            {
                juce::Path path;
                path.addRectangle(
                    p.x - dotRadius * 0.35f,
                    p.y - dotRadius * 0.75f,
                    dotRadius * 0.55f,
                    dotRadius * 0.5f
                );

                g.setColour(juce::Colours::white.withAlpha(0.55f));
                g.fillPath(path, rotation);
            }
        }
    }
}
*/

// -----------------------------
// ---------- SLIDERS ----------
// -----------------------------
void SuperChorusLAF::drawRotarySlider(
    juce::Graphics &g,
    int x,
    int y,
    int width,
    int height,
    float sliderPosProportional,
    float startAngle,
    float endAngle,
    juce::Slider &slider
)
{
    const auto radius       = (float) juce::jmin (width / 2, height / 2) - 4.0f;
    const auto centreX      = (float) x + (float) width * 0.5f;
    const auto centreY      = (float) y + (float) height * 0.5f;
    const auto thickness    = 2.f;
    const auto pointerAngle = startAngle + sliderPosProportional * (endAngle - startAngle);
    const juce::Point<float> centre( centreX, centreY );

    // Centre
    const auto centreRadius = radius * 0.1f;
    const auto centreRx     = centreX - centreRadius;
    const auto centreRy     = centreY - centreRadius;
    const auto centreRw     = centreRadius * 2.f;

    g.setColour(colour6);
    g.drawEllipse (centreRx, centreRy, centreRw, centreRw, thickness);

    // --- Outline ---
    const auto outRadius    = radius * 0.8f;

    // Arc
    juce::Path path;
    path.addCentredArc(centreX, centreY, outRadius, outRadius, 0.f, startAngle, endAngle, true);

    // Indicator lines
    const auto numIndicators = 7;
    for (auto lineIdx = 0; lineIdx < numIndicators; ++lineIdx)
    {
        auto angle = startAngle + (endAngle - startAngle) / (numIndicators - 1) * lineIdx;
        path.startNewSubPath(   centre.getPointOnCircumference(radius, angle));
        path.lineTo(            centre.getPointOnCircumference(outRadius, angle));
    }

    g.setColour(colour2);
    g.strokePath(
        path,
        juce::PathStrokeType(
            thickness,
            juce::PathStrokeType::JointStyle::curved,
            juce::PathStrokeType::EndCapStyle::rounded
        )
    );

    // --- Path ---
    const auto pathRadius           = radius * 0.6f;
    const auto pathRx               = centreX - pathRadius;
    const auto pathRy               = centreY - pathRadius;
    const auto pathRw               = pathRadius * 2.f;
    const auto pathThicknessProp    = 0.7f;
    const juce::Rectangle<float> pathBounds(pathRx, pathRy, pathRw, pathRw);

    juce::Path pathPath;
    pathPath.addPieSegment(pathBounds, startAngle, pointerAngle, pathThicknessProp);

    juce::ColourGradient grad = juce::ColourGradient::horizontal (
        colour2, pathRx,
        colour4, pathRx + pathRw
    );
    g.setGradientFill(grad);

    //g.setColour(colour4);
    g.fillPath(
        pathPath
    );

    // --- Pointer ---
    // Hand
    const auto pointerStartRadius   = centreRadius;
    const auto pointerLength        = radius * 0.75f;

    juce::Path pointerPath;
    pointerPath.startNewSubPath(centre.getPointOnCircumference(pointerStartRadius + pointerLength, pointerAngle));
    pointerPath.lineTo(centre.getPointOnCircumference(pointerStartRadius, pointerAngle));

    // Arc
    const auto arcAngle     = 0.7853981634; // pi / 4
    const auto arcLength    = outRadius * 0.8f;
    /*
    pointerPath.addCentredArc(
        centreX,
        centreY,
        arcLength,
        arcLength,
        0.f,
        pointerAngle - arcAngle * 0.5f,
        pointerAngle + arcAngle * 0.5f,
        true
    );
    */

    // pointer
    g.setColour (colour6);
    g.strokePath(
        pointerPath,
        juce::PathStrokeType(
            thickness,
            juce::PathStrokeType::JointStyle::curved,
            juce::PathStrokeType::EndCapStyle::rounded
        )
    );
}


juce::Slider::SliderLayout SuperChorusLAF::getSliderLayout (juce::Slider& slider)
{
    juce::Slider::SliderLayout layout;

    // Entire slider bounds
    auto bounds = slider.getLocalBounds();

    // Textbox dimensions
    const int textBoxWidth   = slider.getTextBoxWidth();
    const int textBoxHeight  = slider.getTextBoxHeight();
    const int bottomMargin   = textBoxHeight * 0.5f;

    // Default: slider takes whole area
    layout.sliderBounds = bounds;

    if (slider.getTextBoxPosition() == juce::Slider::TextBoxRight)
    {
        // Place textbox at bottom-right instead of centered-right
        layout.textBoxBounds = juce::Rectangle<int>(
            bounds.getRight() - textBoxWidth,
            bounds.getBottom() - textBoxHeight - bottomMargin,
            textBoxWidth,
            textBoxHeight
        );

        // Reduce slider bounds so it doesn't overlap textbox
        layout.sliderBounds.removeFromRight(textBoxWidth);
    }
    else
    {
        // Fallback to default JUCE behaviour
        return juce::LookAndFeel_V4::getSliderLayout(slider);
    }

    return layout;
}

juce::Label *SuperChorusLAF::createSliderTextBox(juce::Slider &slider)
{
    auto* label = new juce::Label();
    label->setColour(juce::Label::textColourId, colour3);
    label->setColour(juce::Label::backgroundColourId, juce::Colour(0x00000000));
    label->setJustificationType(juce::Justification::left);
    return label;
}


// -----------------------------
// ---------- BUTTON -----------
// -----------------------------
void SuperChorusLAF::drawButtonBackground(
    juce::Graphics &g,
    juce::Button &button,
    const juce::Colour &backgroundColour,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown
)
{
    auto bounds                     = button.getLocalBounds().toFloat();
    const auto left                 = bounds.getX();
    const auto right                = bounds.getX() + bounds.getWidth();
    const auto top                  = bounds.getY();
    const auto bottom               = bounds.getY() + bounds.getHeight();
    const auto centreX              = left + (right - left) * 0.5f;
    const auto centreY              = top + (bottom - top) * 0.5f;
    const auto inBorderColour       = colour2;
    const auto onColour             = colour4;
    const auto offColour            = colour3;
    auto inCentreColour             = offColour;
    const auto outSideColour        = colour2;
    const auto outCentreColour      = colour3;
    const auto cornerSize           = 15.f;
    const auto displacement         = 3.f;
    const auto outThickness         = 3.f;

    bounds.removeFromBottom(displacement);


    /**
     * TODO
     * There is a gap between inside and outline. Fill it with
     * colour6
     * Seems like the outline is popping out on the top too, but it should
     * only on the bottom. The top should be covered by inside
     */


    // Outline
    bounds = bounds.reduced(outThickness * 0.5f);
    auto outBounds = bounds;
    outBounds.translate(0.f, displacement);

    juce::ColourGradient outGradient (
        outCentreColour,
        left,
        bottom,
        outSideColour,
        left,
        top,
        false
    );
    g.setGradientFill(outGradient);
    g.drawEllipse(outBounds, outThickness);

    // Inside
    if (shouldDrawButtonAsDown)
    {
        bounds.translate(0.f, displacement);
    }

    if (button.getToggleState())
    {
        inCentreColour = onColour;
    }

    juce::ColourGradient inGradient(
        inCentreColour,
        centreX,
        centreY,
        inBorderColour,
        left,
        centreY,
        true
    );
    g.setGradientFill(inGradient);
    g.fillEllipse(bounds);
}


// -----------------------------
// ---------- COMBOBOX --------
// -----------------------------
void SuperChorusLAF::drawComboBox(
    juce::Graphics &g,
    int width,
    int height,
    bool isButtonDown,
    int buttonX,
    int buttonY,
    int buttonW,
    int buttonH,
    juce::ComboBox &cb 
)
{
    auto bounds         = cb.getLocalBounds().toFloat();
    const auto centreX  = bounds.getCentreX();
    const auto centreY  = bounds.getCentreY();
    const auto left     = bounds.getX();
    const auto top      = bounds.getY();
    const auto bottom   = bounds.getBottom();
    const auto cornerSize = 20.f;
    auto inColour       = colour3;

    // Box
    juce::Path boxPath;
    boxPath.addRoundedRectangle(bounds, cornerSize);
    if (isButtonDown)
    {
        inColour = colour4;
    }

    // Button
    const auto buttonMargin = 10.f;
    const auto buttonSpace  = 3.f;
    const auto numLines     = 3;
    const auto thickness    = 1.5f;
    const auto lineX        = buttonX;
    const auto lineWidth    = buttonW - buttonMargin;
    const auto lineYIncr    = buttonSpace + thickness;
    auto lineY              = centreY - (numLines / 2) * lineYIncr;

    juce::Path buttonPath;
    for (auto i = 0; i < numLines; ++i)
    {
        buttonPath.addRoundedRectangle(lineX, lineY - thickness * 0.5f, lineWidth, thickness, thickness);
        lineY += lineYIncr;
    }

    // Substract button from box
    boxPath.addPath(buttonPath);
    boxPath.setUsingNonZeroWinding(false);

    juce::ColourGradient boxGradient(
        inColour,
        centreX,
        centreY,
        colour2,
        left,
        centreY,
        true
    );
    g.setGradientFill(boxGradient);
    g.fillPath(boxPath);
}

void SuperChorusLAF::positionComboBoxText(juce::ComboBox& box, juce::Label& label)
{
    constexpr int textOffsetX = 20;

    label.setBounds(
        1 + textOffsetX,
        1,
        box.getWidth() - 30 - textOffsetX,
        box.getHeight() - 2
    );

    label.setFont(getComboBoxFont(box));
}
