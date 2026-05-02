# Godot Visual Stream (GVS)

<p align="center">
  <img src="https://img.shields.io/badge/status-in%20development-orange" alt="Status: In Development">
  <img src="https://img.shields.io/badge/godot-4.x-blue" alt="Godot 4.x">
  <img src="https://img.shields.io/badge/language-C%2B%2B-brightgreen" alt="C++">
  <img src="https://img.shields.io/badge/license-MIT-lightgrey" alt="MIT License">
</p>

<p align="center">
  A node-based visual particle system editor for Godot Engine 4.x, built as a native C++ module.
</p>

> **Work in progress.** Core functionality is working but the project is still under active development — many features are incomplete or missing. Expect breaking changes.

---

## What is GVS?

**Godot Visual Stream** is an editor plugin integrated directly into Godot Engine 4.x that lets you create and edit particle systems through a visual node-based interface — similar to Niagara (Unreal Engine) or the Unity VFX Graph.

Particle data is stored in `.gvs` files (a custom JSON format) and can be instantiated at runtime as `GVSParticleSystem` nodes inside any 3D scene, which internally configure and drive Godot's native `GPUParticles3D`.

<img width="1598" height="923" alt="GSV windows" src="https://github.com/user-attachments/assets/f54d9f56-1e9c-4bc9-9eab-c069128ae849" />

---

## Features

- **Visual node canvas** — 2D canvas with zoom, pan, drag and multi-section emitter nodes (Spawn / Update / Render)
- **Module system** — modules are the only source of truth; no modules means no particles
- **Built-in modules:**
	- `SpawnRate` — particles per second
	- `Lifetime` — particle lifetime
	- `InitialVelocity` — direction (Vector3) + min/max speed
	- `Position` — position offset (Vector3)
	- `Facing` — billboard mode (Camera Plane / Camera Position / Velocity)
	- `RenderMaterial` — assign a `ParticleProcessMaterial` or `ShaderMaterial`
- **Typed variable system** — define `Float`, `Int`, `Bool`, `Color` and `Vector3` variables and bind them to module properties via drag & drop
- **Live sync** — changing a variable value instantly propagates to all bound modules and refreshes the 3D preview
- **Embedded 3D preview** — orbital camera, XZ grid with colored axes, togglable with a button overlay
- **Auto-save** — any change is persisted to the `.gvs` file automatically
- **Runtime node** — add `GVSParticleSystem` to any scene and it instantiates the full particle setup from the `.gvs` resource

---

## Architecture

GVS is implemented as a **native C++ module** compiled directly into the Godot Engine source tree — not a GDScript addon or GDExtension. This gives access to internal editor APIs unavailable to external extensions.

```
editor/gvs/
├── gvs_editor_plugin        → Plugin entry point
├── gvs_editor_window        → Main window, panel coordination, variable sync
├── gvs_context_menu_plugin  → FileSystem right-click menu
├── gvs_format               → JSON serialization / deserialization
├── gvs_resource             → Core resource (nodes + variables)
├── nodes/
│   ├── gvs_emitter_node     → Emitter data + create_gpu_particles()
│   ├── gvs_variable         → Typed user variable
│   └── gvs_particle_system  → Runtime Node3D for use in scenes
├── modules/
│   ├── gvs_module           → Abstract base class + factory
│   ├── gvs_module_spawn_rate
│   ├── gvs_module_lifetime
│   ├── gvs_module_initial_velocity
│   ├── gvs_module_position
│   ├── gvs_module_facing
│   └── gvs_module_render_material
└── panels/
    ├── gvs_emitters_panel   → Interactive 2D canvas
    ├── gvs_inspector_panel  → Dynamic property inspector + drop zones
    ├── gvs_variables_panel  → Variable list with drag & drop
    ├── gvs_particle_preview → Embedded 3D preview viewport
    └── gvs_timeline_panel   → Timeline (in development)
```

---

## .gvs File Format

Particle systems are stored as readable JSON:

```json
{
  "version": 1,
  "nodes": [
    {
      "id": 1,
      "title": "Fire Emitter",
      "canvas_pos": { "x": 100.0, "y": 50.0 },
      "spawn_modules": [
        { "type": "lifetime",   "enabled": true, "lifetime": 3.0 },
        { "type": "spawn_rate", "enabled": true, "rate": 200.0 },
        {
          "type": "initial_velocity", "enabled": true,
          "direction": { "x": 0.0, "y": 1.0, "z": 0.0 },
          "velocity_min": 2.0, "velocity_max": 8.0
        }
      ],
      "update_modules": [],
      "render_modules": [
        { "type": "facing", "enabled": true, "facing_mode": 0 }
      ]
    }
  ],
  "variables": [
    { "name": "speed", "type": 0, "value": 5.0 },
    { "name": "spawn_offset", "type": 4, "value": { "x": 0.0, "y": 1.0, "z": 0.0 } }
  ]
}
```

---

## Building

GVS lives inside the Godot Engine source tree. To compile it:

1. Clone this repository (a fork of Godot 4.x with the GVS module included)
2. Follow the [official Godot compilation guide](https://docs.godotengine.org/en/latest/engine_details/development/compiling) for your platform
3. The `editor/gvs/` module is picked up automatically by SCons

```bash
# Example — Windows with MSVC
scons platform=windows
```

---

## Usage

1. **Create a particle system:** right-click in the FileSystem panel → Create → GVS File...
2. **Open the editor:** double-click any `.gvs` file
3. **Add an emitter node:** right-click on the canvas → Add Emitter
4. **Add modules:** click the `+` button on any section (Spawn / Update / Render)
5. **Define variables:** use the Variables panel on the left, drag them onto module properties
6. **Preview:** the 3D viewport on the top-left updates in real time
7. **Add to scene:** click "Add to Scene" in the toolbar to instantiate a `GVSParticleSystem` node

---

## Roadmap

- [ ] Timeline panel (keyframe animation of module properties)
- [ ] Additional modules: initial color, size, gravity, turbulence, emission shapes
- [ ] Variables exposed as `GVSParticleSystem` node parameters (set from code)
- [ ] Node-to-node connections (events, property inheritance)
- [ ] Export to standalone `GPUParticles3D`

---

## Based on

This repository is a fork of [Godot Engine](https://github.com/godotengine/godot), licensed under the [MIT License](LICENSE.txt). GVS code is located entirely under `editor/gvs/` and is also MIT licensed.
