# SineSynth

A polyphonic sine wave synthesizer VST3 plugin built with JUCE. Features 16-voice polyphony, ADSR envelope control, and MIDI input.

## Features

- Pure sine wave synthesis
- 16-voice polyphony
- ADSR envelope with rotary knob controls
- MIDI note input with velocity sensitivity

## Build Instructions (macOS)

### Prerequisites

- **CMake** 3.15 or later
- **Xcode Command Line Tools** (or full Xcode)
- **JUCE** framework cloned as a sibling directory

Install CMake if you don't have it:

```bash
brew install cmake
```

### Clone and Set Up

```bash
# Clone the repo and JUCE side by side
git clone git@github.com:hacksdump/VSTi.git
git clone https://github.com/juce-framework/JUCE.git

# Directory structure should look like:
# projects/
# ├── VSTi/      (this repo)
# └── JUCE/
```

### Build

```bash
cd VSTi
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The built plugin will be at `build/SineSynth_artefacts/VST3/SineSynth.vst3`.

The build is configured to auto-copy the plugin to your system VST3 directory (`~/Library/Audio/Plug-Ins/VST3/`).

## Usage

### Ableton Live

1. Build and install the plugin (the auto-copy step handles this).
2. Open Ableton Live → **Preferences** → **Plug-ins**.
3. Enable **Use VST3 Plug-in System Folder**.
4. Click **Rescan**.
5. In a MIDI track, open the **Browser** → **Plug-ins** → find **SineSynth**.
6. Drag it onto the MIDI track.
7. Route a MIDI controller or use Ableton's piano roll to play notes.

### FL Studio

1. Build and install the plugin.
2. Open FL Studio → **Options** → **Manage plugins**.
3. Click **Find plugins** (or **Rescan**) — FL Studio scans `~/Library/Audio/Plug-Ins/VST3/` by default.
4. Open the **Channel Rack** → click **+** → **More plugins** → find **SineSynth**.
5. Play notes via the piano roll or a connected MIDI controller.

### VST Plugin Host (JUCE AudioPluginHost)

1. Build the JUCE AudioPluginHost from `JUCE/extras/AudioPluginHost`.
2. Open AudioPluginHost → **Options** → **Edit the List of Available Plug-ins** → **Scan for new or updated VST3 plug-ins**.
3. Right-click the graph area → **Add filter** → select **SineSynth**.
4. Connect **MIDI Input** → **SineSynth** → **Audio Output**.
5. Send MIDI from a connected controller or the on-screen keyboard.

## Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Attack | 0–5 s | 0 s | Time to reach full volume |
| Decay | 0–5 s | 0 s | Time to fall to sustain level |
| Sustain | 0–1.0 | 1.0 | Held volume level |
| Release | 0–5 s | 0 s | Time to fade after note-off |
