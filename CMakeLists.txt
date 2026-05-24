cmake_minimum_required(VERSION 3.25)

# Windows static runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# MacOS unversal binary
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")

# MacOS minimum deployment target
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")

# Main project
project(MRTA VERSION 0.1.0 LANGUAGES C CXX)

# Get 'add_plugin' function and add JUCE to project
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(add_plugin)


# General variables
# You should change this to something more interesting
set(company_name "Modern Real-Time Audio")
set(company_copyright "Modern Real-Time Audio Oy")
set(company_code "Mrta")
set(company_bundle_id "com.ModernRealTimeAudio")


## Plugin projects

# DSP directory helper variable
set(dsp_source ${CMAKE_CURRENT_SOURCE_DIR}/projects/DSP)

# GUI directory helper variable
set(gui_source ${CMAKE_CURRENT_SOURCE_DIR}/projects/GUI)

# MRTA framework helper variable
set(mrta_source ${CMAKE_CURRENT_SOURCE_DIR}/mrta_utils)

# Example project
set(mfrtaa_source ${CMAKE_CURRENT_SOURCE_DIR}/projects/MyFirstRealTimeAudioApp)

add_plugin(mfrtaa
    VERSION 0.1.0
    PLUGIN_NAME "My First RT Audio App"
    PROD_NAME MyFirstRTAudioApp
    PROD_CODE Rtap
    SYNTH FALSE
    SOURCES
        ${mfrtaa_source}/PluginEditor.cpp
        ${mfrtaa_source}/PluginProcessor.cpp
    INCLUDE_DIRS
        ${mfrtaa_source})

# ParametricEQ project
set(parameq_source ${CMAKE_CURRENT_SOURCE_DIR}/projects/ParametricEQ)

add_plugin(parameq
    VERSION 0.1.0
    PLUGIN_NAME "Parametric EQ"
    PROD_NAME ParametricEQ
    PROD_CODE Pmeq
    SYNTH FALSE
    SOURCES
        ${parameq_source}/ParametricEQEditor.cpp
        ${parameq_source}/ParametricEQProcessor.cpp
        ${dsp_source}/Biquad.cpp
        ${dsp_source}/ParametricEqualizer.cpp
        ${gui_source}/MrtaLAF.cpp
    INCLUDE_DIRS
        ${parameq_source}
        ${gui_source}
        ${dsp_source})

# Cabinet Simulator project
set(cabsim_source ${CMAKE_CURRENT_SOURCE_DIR}/projects/CabinetSimulator)

add_plugin(cabsim
    VERSION 0.1.0
    PLUGIN_NAME "Cabinet Simulator"
    PROD_NAME CabinetSimulator
    PROD_CODE Csim
    SYNTH FALSE
    SOURCES
        ${cabsim_source}/CabinetSimulatorEditor.cpp
        ${cabsim_source}/CabinetSimulatorProcessor.cpp
        ${cabsim_source}/Cab2x12.c
        ${cabsim_source}/Cab4x12.c
        ${dsp_source}/FIR.cpp
    INCLUDE_DIRS
        ${cabsim_source}
        ${dsp_source})

# ring mod project
set(ringmod_source ${CMAKE_CURRENT_SOURCE_DIR}/projects/RingModulator)

add_plugin(ringmod
    VERSION 0.1.0
    PLUGIN_NAME "Ring Modulator"
    PROD_NAME RingModulator
    PROD_CODE Rmod
    SYNTH FALSE
    SOURCES
        ${ringmod_source}/RingModulatorEditor.cpp
        ${ringmod_source}/RingModulatorProcessor.cpp
        ${dsp_source}/RingMod.cpp
    INCLUDE_DIRS
        ${dsp_source}
        ${ringmod_source})

# flanger project
set(flanger_source ${CMAKE_CURRENT_SOURCE_DIR}/projects/Flanger)

add_plugin(flanger
    VERSION 0.1.0
    PLUGIN_NAME "Flanger"
    PROD_NAME Flanger
    PROD_CODE Flng
    SYNTH FALSE
    SOURCES
        ${flanger_source}/FlangerEditor.cpp
        ${flanger_source}/FlangerProcessor.cpp
        ${dsp_source}/DelayLine.cpp
        ${dsp_source}/Flanger.cpp
    INCLUDE_DIRS
        ${dsp_source}
        ${flanger_source})

# delay project
set(delay_source ${CMAKE_CURRENT_SOURCE_DIR}/projects/Delay)

add_plugin(delay
    VERSION 0.1.0
    PLUGIN_NAME "Delay"
    PROD_NAME Delay
    PROD_CODE Dlay
    SYNTH FALSE
    SOURCES
        ${delay_source}/DelayEditor.cpp
        ${delay_source}/DelayProcessor.cpp
        ${dsp_source}/DelayLine.cpp
        ${dsp_source}/Delay.cpp
        ${dsp_source}/Biquad.cpp
        ${dsp_source}/ParametricEqualizer.cpp
        ${dsp_source}/Meter.cpp
        ${gui_source}/MeterComponent.cpp
        ${gui_source}/MrtaLAF.cpp
    INCLUDE_DIRS
        ${dsp_source}
        ${gui_source}
        ${delay_source})

# envelope generator project
set(envgen_source ${CMAKE_CURRENT_SOURCE_DIR}/projects/EnvelopeGenerator)

add_plugin(envgen
    VERSION 0.1.0
    PLUGIN_NAME "Envelope Generator"
    PROD_NAME EnvelopeGenerator
    PROD_CODE Egen
    SYNTH FALSE
    SOURCES
        ${envgen_source}/EnvelopeGeneratorEditor.cpp
        ${envgen_source}/EnvelopeGeneratorProcessor.cpp
        ${dsp_source}/EnvelopeGenerator.cpp
    INCLUDE_DIRS
        ${dsp_source}
        ${envgen_source})

# osc project
set(osc_source ${CMAKE_CURRENT_SOURCE_DIR}/projects/Oscillators)

add_plugin(osc
    VERSION 0.1.0
    PLUGIN_NAME "Oscillators"
    PROD_NAME Oscillators
    PROD_CODE Oscs
    SYNTH FALSE
    SOURCES
        ${osc_source}/OscillatorsEditor.cpp
        ${osc_source}/OscillatorsProcessor.cpp
        ${dsp_source}/Oscillator.cpp
    INCLUDE_DIRS
        ${dsp_source}
        ${osc_source})

# state variable filter project
set(svf_source ${CMAKE_CURRENT_SOURCE_DIR}/projects/StateVariableFilter)

add_plugin(svf
    VERSION 0.1.0
    PLUGIN_NAME "State Variable Filter"
    PROD_NAME StateVariableFilter
    PROD_CODE Svff
    SYNTH FALSE
    SOURCES
        ${svf_source}/SVFEditor.cpp
        ${svf_source}/SVFProcessor.cpp
        ${dsp_source}/StateVariableFilter.cpp
        ${dsp_source}/Oscillator.cpp
    INCLUDE_DIRS
        ${dsp_source}
        ${svf_source})

# midi handling project
set(midi_source ${CMAKE_CURRENT_SOURCE_DIR}/projects/MidiHandler)

add_plugin(midi
    VERSION 0.1.0
    PLUGIN_NAME "Midi Handler"
    PROD_NAME MidiHandler
    PROD_CODE Mdhd
    SYNTH TRUE
    SOURCES
        ${midi_source}/MidiHandlerEditor.cpp
        ${midi_source}/MidiHandlerProcessor.cpp
        ${dsp_source}/SynthVoice.cpp
        ${dsp_source}/Oscillator.cpp
    INCLUDE_DIRS
        ${dsp_source}
        ${midi_source})

# Knob example project
set(knob_source ${CMAKE_CURRENT_SOURCE_DIR}/projects/KnobExample)

juce_add_binary_data(knob_data
    SOURCES ${knob_source}/knob.png)

add_plugin(knob
    VERSION 0.1.0
    PLUGIN_NAME "Knob Example"
    PROD_NAME KnobExample
    PROD_CODE Knob
    SYNTH FALSE
    SOURCES
        ${knob_source}/PluginEditor.cpp
        ${knob_source}/PluginProcessor.cpp
        ${mrta_source}/GUI/ImageKnob.cpp
    INCLUDE_DIRS
        ${knob_source}
    LINK_LIBS
        knob_data)

# Add "PhaseVocoder" plugin target
set(pv_source ${CMAKE_CURRENT_SOURCE_DIR}/projects/PhaseVocoder)

add_plugin(PhaseVocoder
    VERSION 0.0.1
    SYNTH FALSE
    PLUGIN_NAME "Phase Vocoder"
    PROD_NAME "PhaseVocoder"
    PROD_CODE "Phvc"
    SYNTH FALSE
    SOURCES
        ${pv_source}/PhaseVocoderProcessor.cpp
        ${pv_source}/PhaseVocoderEditor.cpp
        ${dsp_source}/OverlapSave.cpp
        ${dsp_source}/OverlapAdd.cpp
        ${dsp_source}/Phase.cpp
    INCLUDE_DIRS
        ${dsp_source}
        ${pv_source})

# Add "WaveNet" plugin target
set(wavenet_source ${CMAKE_CURRENT_SOURCE_DIR}/projects/WaveNet)

juce_add_binary_data(wavenet_data
    SOURCES
        ${wavenet_source}/wavenet_weights.bin)

add_plugin(WaveNet
    VERSION 0.0.1
    SYNTH FALSE
    PLUGIN_NAME "WaveNet"
    PROD_NAME "WaveNet"
    PROD_CODE "WaNe"
    SOURCES
        ${wavenet_source}/WaveNetProcessor.cpp
        ${wavenet_source}/WaveNetEditor.cpp
    INCLUDE_DIRS
        ${dsp_source}
        ${wavenet_source}
    LINK_LIBS
        wavenet_data)

# homework 3 plugin template
set(amp_model_source ${CMAKE_CURRENT_SOURCE_DIR}/projects/AmpModel)

add_plugin(amp_model
    VERSION 0.1.0
    PLUGIN_NAME "Amp Model"
    PROD_NAME AmpModel
    PROD_CODE AmpM
    SYNTH FALSE
    SOURCES
        ${amp_model_source}/PluginEditor.cpp
        ${amp_model_source}/PluginProcessor.cpp
        ${amp_model_source}/AmpGruParameters.cpp
    INCLUDE_DIRS
        ${gui_source}
        ${dsp_source}
        ${amp_model_source})

# mono synth project
set(synth ${CMAKE_CURRENT_SOURCE_DIR}/projects/Synth)

add_plugin(synth
    VERSION 0.1.0
    PLUGIN_NAME "Synth"
    PROD_NAME Synth
    PROD_CODE Syth
    SYNTH TRUE
    SOURCES
        ${synth}/SynthEditor.cpp
        ${synth}/SynthProcessor.cpp
        ${dsp_source}/Synth.cpp
        ${dsp_source}/Oscillator.cpp
        ${dsp_source}/EnvelopeGenerator.cpp
        ${dsp_source}/StateVariableFilter.cpp
    INCLUDE_DIRS
        ${gui_source}
        ${dsp_source}
        ${synth})


# Add "PhaseVocoder" plugin target
set(spch_source ${CMAKE_CURRENT_SOURCE_DIR}/projects/SuperChorus)

add_plugin(SuperChorus
    VERSION 0.0.1
    SYNTH FALSE
    PLUGIN_NAME "Super Chorus"
    PROD_NAME "SuperChorus"
    PROD_CODE "SpCh"
    SYNTH FALSE
    SOURCES
        ${spch_source}/SuperChorusProcessor.cpp
        ${spch_source}/SuperChorusLAF.cpp
        ${spch_source}/SuperChorusEditor.cpp
        ${dsp_source}/DelayLine.cpp
        ${dsp_source}/Chorus.cpp
        ${dsp_source}/SuperChorus.cpp
    INCLUDE_DIRS
        ${dsp_source}
        ${spch_source})