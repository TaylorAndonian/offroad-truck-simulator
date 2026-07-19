# Toyota Tacoma Off-Road Simulator
## Project Vision & Context for Claude

---

## What This Is

A browser-based Toyota Tacoma TRD Off-Road driving simulator built to be hosted from Taylor's PC and monetized as a web game. The inspiration was a YouTuber who made $19k in a single day from an AI-built video game. The goal is to build something genuinely impressive, host it online, and make money from it.

**The product:** A polished, fun, visually stunning off-road driving simulator centered on the Toyota Tacoma. Players access it through a URL in their browser — no download required.

**The money model:** Host via Cloudflare Tunnel (free, no port forwarding needed) from Taylor's PC. Sell access or cosmetics via itch.io (handles payments, takes 10%) or Stripe. Potentially add Google AdSense for passive ad revenue.

---

## Tech Stack

| Layer | Technology | Why |
|-------|-----------|-----|
| Game engine | Three.js (r128) on WebGL 2.0 | Runs in browser, no download for players, easy to share/go viral |
| Language | Vanilla JavaScript + HTML/CSS | Simple, fast, no build step |
| Hosting | Cloudflare Tunnel → Taylor's PC | Free, no static IP needed, PC stays the server |
| Payments | itch.io or Stripe | itch.io easiest, Stripe for direct integration |
| Version control | Git (already initialized in this folder) | Commit before major changes |

**Why not Unreal/Unity?** Would require users to download a .exe — kills viral potential. HTML loads instantly in any browser on any device.

---

## Current State (as of first session, July 2026)

### What exists: `game/index.html`
A working prototype with:
- Functional Toyota Tacoma model built from Three.js BoxGeometry (boxes, cylinders)
- Army green color scheme, roof rack, fender flares, TRD red hub caps, running boards, mirrors, fog lights, LED DRLs
- Off-road terrain with hills (sine-wave heightmap), rocks, trees, cacti
- Driving physics: WASD controls, wheelbase-based steering, friction, handbrake
- 3 camera modes: chase cam (with mouse look), hood cam, cinematic orbit
- HUD: speed (MPH), RPM bar, gear indicator
- Dust particles on acceleration
- Engine audio via Web Audio API (oscillators)

### What it looks like
Functional and fun, but visually basic. Car is made of geometric primitives — looks like a toy truck. No reflections, no real lighting model, no textures.

---

## The #1 Priority: Make the Car Look REAL

Taylor's words: "I want it to look 100k nicer. Let's put all our energy into making the car look real. I don't care if it takes a month or a year. We're making this game look amazing."

This is the core focus before adding gameplay features, levels, or monetization.

### What "real" means technically

**Right now (bad):**
- `MeshPhongMaterial` — fake, non-physical lighting model
- No reflections on car paint
- No environment mapping
- No post-processing
- Car is boxes

**What it needs (good):**
- `MeshStandardMaterial` — PBR (Physically Based Rendering). Massive visual upgrade.
- **HDRI environment map** — Makes car paint reflect the sky and surroundings. This single change transforms how a car looks.
- **CubeCamera** — Real-time reflections on the car body
- **Post-processing pipeline:** bloom (light glow), SSAO (ambient occlusion/shadows in crevices), depth of field, color grading
- **High-poly car geometry** OR a loaded GLTF model — ideally 50k–200k polygons
- **Normal maps** on surfaces to fake surface detail
- **Better terrain** with real textures (dirt, gravel, rock)

### The biggest single upgrade
Loading a real GLTF/GLB 3D model of the Toyota Tacoma. Three.js has a GLTFLoader that can load production-quality 3D models. If Taylor can source a Tacoma model (free options: Sketchfab, Poly Pizza, BlenderKit), the visual jump would be enormous.

If no model is available, build the most detailed hand-crafted geometry possible with PBR materials.

---

## Folder Structure

```
Toyota Offroad Simulator/
├── PROJECT.md              ← You are here. Read this first every session.
├── game/
│   ├── index.html          ← Main game file (open this in browser to play)
│   ├── src/                ← Future: split JS modules go here
│   └── assets/
│       ├── models/         ← GLTF/GLB 3D models go here
│       ├── textures/       ← HDRI, normal maps, terrain textures go here
│       └── audio/          ← Future: real engine audio samples
├── server/
│   └── server.js           ← Node.js server for hosting (see Hosting section)
├── dev-notes/              ← Design sketches, reference images, ideas
└── tacoma_simulator.html   ← Old file, redirects to game/index.html. Ignore.
```

---

## Hosting From PC

To make the game accessible from the internet:

**Step 1 — Run a local server**
```bash
cd "Toyota Offroad Simulator"
node server/server.js
# Game now at http://localhost:3000
```

**Step 2 — Expose to the internet with Cloudflare Tunnel**
```bash
# Install once: https://developers.cloudflare.com/cloudflare-one/connections/connect-networks/downloads/
cloudflared tunnel --url http://localhost:3000
# Gives you a public URL like: https://random-name.trycloudflare.com
```

That URL is your game. Share it, post it, sell it.

---

## Controls Reference (current)

| Key | Action |
|-----|--------|
| W / ↑ | Accelerate |
| S / ↓ | Brake / Reverse |
| A / ← | Steer Left |
| D / → | Steer Right |
| SPACE | Handbrake |
| R | Reset position |
| C | Cycle camera |
| Click | Enable mouse look |

---

## Decisions Made & Why

- **Single HTML file:** Keep it all in `game/index.html` for now. When it gets complex, split into modules in `src/`.
- **Three.js r128 from CDN:** Loaded from cdnjs. No build step needed.
- **Army green Tacoma:** Chosen for distinctiveness. Can be a color option later.
- **Sine-wave terrain:** Simple, fast, good enough for now. Replace with a proper heightmap texture later.
- **No external assets yet:** Everything is procedural so the game loads instantly with no dependencies.

---

## Next Steps (in priority order)

1. **Visual overhaul on the car** — PBR materials + HDRI environment + post-processing
2. **Source or build a high-poly Tacoma model**
3. **Better terrain** — textures, better heightmap, maybe a real off-road track
4. **Gameplay depth** — objectives, a course, scoring, maybe mud/water
5. **Server setup** — get it live on Cloudflare Tunnel
6. **Monetization** — itch.io listing, pricing, description

---

## For Claude: How to Start Each Session

1. Read this file (`PROJECT.md`) first
2. Read `game/index.html` to see current state
3. Check `dev-notes/` for any new instructions Taylor may have left
4. Ask Taylor what to focus on if unclear
5. Always commit to git before making major changes (or ask Taylor to)

---

