# Loop Context & Guidelines

This document serves as the central context and guidelines for the Loop project. It outlines the project's purpose, architecture, and rules to ensure focus is maintained throughout development.

## 1. Project Purpose
**Loop** is a modular application for live visual effects and also a video mixer built in openFrameworks. It acts as the "brain" or central control hub for a modular visual performance system.

## 2. Core Architecture
- **Video Routing & Mixing:** Uses Syphon to ingest 4 layers of video. Mixes them using standard blend modes (Alpha, Add, Multiply, Screen, Subtract) into a Master Framebuffer (FBO).

- **Built-in FX Containers:** 
The core engine features built-in container apps (
`LiveDatamosh`, 
`AlchemyReimagined`, 
`GlitchLibrary`, 
`TextGenerator`, 
  - Users can open them as containers inside the host app. And control opacity individually
  - Each app will appear as a unified control panel/menu.
  - Once enabled, the app will show in a live preview window that can be routed and mixed to the final output screen.


and `RuttEtraSynthesizer` [in development]). A fractal generator will also be added in the future.

- **Global Stop Function:** Implement a global stop function (button and/or keybind) to reset the software and stop the engine (gracefully kill all running apps).

osc functionality
