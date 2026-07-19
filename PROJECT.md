# Toyota Tacoma Off-Road Simulator
## Project Vision & Context for Claude

---

## What This Is

A browser-based Toyota Tacoma TRD Off-Road driving simulator built to be hosted from Taylor's PC and monetized as a web game. Inspired by a YouTuber who made $19k in a single day from an AI-built video game. The goal is to build something genuinely impressive, host it online, and make money from it.

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

## Folder Structure

```
Toyota Offroad Simulator/
├── PROJECT.md                  ← You are here. Read this first every session.
├── game/
│   ├── index.html              ← Original driving simulator prototype (box geometry). Not currently focus.
│   ├── tacoma-model.html       ← ★ ACTIVE FILE — real GLB model viewer. Work here.
│   ├── toyota_tacoma_trd.glb   ← Real 3D Tacoma model file (from Sketchfab)
│   └── assets/
│       ├── models/             ← Future: additional GLB models
│       ├── textures/           ← Future: terrain textures, normal maps
│       └── audio/              ← Future: real engine audio samples
├── server/
│   └── server.js               ← Node.js static server. Run to serve game at localhost:3000.
├── dev-notes/
│   └── ideas.md                ← Gameplay/monetization ideas scratchpad
└── tacoma_simulator.html       ← Old redirect file. Ignore.
```

---

## Current State (as of Session 2, July 2026)

### What's working: `game/tacoma-model.html`

This is the active file. It's a model viewer for the real Toyota Tacoma GLB.

- Real `toyota_tacoma_trd.glb` model loaded via `THREE.GLTFLoader` + `THREE.DRACOLoader`
- PBR materials applied per mesh using an exact material-name lookup table
- `MeshPhysicalMaterial` with `clearcoat: 1.0` on body paint — real automotive PBR
- Dark automotive studio environment (procedural canvas, no external HDRI)
- ACESFilmic tone mapping + sRGB output encoding
- Custom orbit controls (mouse drag, scroll zoom, double-click reset, auto-spin)
- 6 paint color swatches: Army Green, Super White, Midnight Black, Magnetic Gray, Barcelona Red, Solar Octane
- Dark navy background (`0x080c14`)
- Press **M** to toggle a material debug panel listing all mesh names
- `window._matPaint` exposes the body paint material for the color picker

### What the GLB contains (all mesh names discovered)

| Mesh | Material name | What it is |
|------|--------------|------------|
| `body_body_0` | `body` | Main paint panels |
| `body_plastic_0` | `plastic` | Fender flares, bumpers, trim |
| `body_dark_0` | `dark` | Grille, interior gaps |
| `body_glass_0` | `glass` | All window glass |
| `body_rear_lights_0` | `rear_lights` | Tail lights |
| `body_brake_lights_0` | `brake_lights` | Brake lights |
| `body_reverse_lights_0` | `reverse_lights` | Reverse lights |
| `body_turnsignal_lights_0` | `turnsignal_lights` | Amber turn signals |
| `body_mirrors_0` | `mirrors` | Side mirrors |
| `body_headlights_0` | `headlights` | Headlight reflector housing |
| `body_headlights_glass_0` | `headlights_glass` | Headlight lens |
| `body_front_lights_0` | `front_lights` | Front LED DRLs |
| `fl/fr/rl/rr_tire_Tires_0` | `Tires` | All 4 tires |
| `fl/fr/rl/rr_rim_rim_main_0` | `rim_main` | Wheel rim body |
| `fl/fr/rl/rr_rim_rim_color_0` | `rim_color` | Rim accent face |

### Key material values (latest as of session 2)

```javascript
// Body paint — user-switchable
const matBody = new THREE.MeshPhysicalMaterial({
  color: 0x4B5E3D,        // Army Green default
  metalness: 0.0,
  roughness: 0.22,
  clearcoat: 1.0,
  clearcoatRoughness: 0.08,
  envMap,
  envMapIntensity: 0.85,  // KEEP LOW — too high drowns the paint color in reflections
});
window._matPaint = matBody;

// Glass — fully opaque dark tint (no transmission)
'glass': new THREE.MeshPhysicalMaterial({
  color: 0x080c10, metalness: 0.08, roughness: 0.0,
  transmission: 0.0, transparent: false,
  envMap, envMapIntensity: 1.4, side: THREE.DoubleSide,
})

// Tires — matte black rubber
'Tires': new THREE.MeshStandardMaterial({
  color: 0x141414, roughness: 0.94, metalness: 0.0,
})

// Wheels (rim body) — dark gunmetal
'rim_main': new THREE.MeshStandardMaterial({
  color: 0x222220, metalness: 0.80, roughness: 0.40,
  envMap, envMapIntensity: 1.2,
})
```

### Renderer settings (latest)

```javascript
renderer.toneMapping = THREE.ACESFilmicToneMapping;
renderer.toneMappingExposure = 0.80;   // keep this moderate — too high = glary
renderer.outputEncoding = THREE.sRGBEncoding;
```

### Environment map approach

**IMPORTANT: Do NOT use the Poly Haven HDRI (`studio_small_08_1k.hdr`).** It is a blinding white studio that washes out all paint colors, making everything look silver/gray. We removed it.

Instead we use a purpose-built dark procedural canvas environment:
- Near-black base (`#04060e`)
- Narrow top softbox band — creates one clean highlight streak on the hood
- Subtle cool fill from left, warm accent from right
- This keeps the paint color dominant and only adds tasteful highlights

The `loadHDRI()` function now instantly returns the procedural env with no network fetch.

---

## What Needs Work Next

The model viewer (`tacoma-model.html`) is functional but there are known visual quality issues still to improve:

1. **Post-processing pipeline** — bloom on lights, depth of field, subtle vignette. This is the single biggest remaining visual upgrade. Needs `EffectComposer` + `UnrealBloomPass` from Three.js examples.
2. **Interior** — currently the model doesn't have a visible interior mesh. Windows are opaque dark which hides this, but eventually an interior would elevate realism.
3. **Ground/floor** — the floor is a dark circle. Could use a subtle reflection (like a showroom floor) or a terrain texture.
4. **Lighting drama** — the car reads well but lacks the "magazine photo" feel. Try adding a very narrow bright strip light (like a studio softbox) that runs across the hood for that sleek highlight.
5. **Integrate model into driving game** — `game/index.html` has driving physics with a box-geometry car. The eventual goal is to swap that out for the real GLB model.

---

## Known Issues & Lessons Learned

| Problem | Root Cause | Fix Applied |
|---------|-----------|-------------|
| All materials pink/salmon | Material detection used keywords; GLB used different names | Exact name lookup table |
| Background bright blue/white | Poly Haven HDRI overriding scene background appearance | Removed HDRI entirely; use procedural dark env |
| Paint washed out to gray | `envMapIntensity` too high; HDRI was white studio | Lowered to 0.85; switched to dark procedural env |
| Windows see-through | `transmission: 0.15` still passed light through | Set `transmission: 0.0`, `transparent: false` |
| Glary / everything overlit | `toneMappingExposure` too high + bright HDRI | Exposure 0.80; dark env; lower light intensities |

**Critical rule:** `envMapIntensity` on the body paint should stay at or below 1.0. Above that, the HDRI reflections overpower the paint color and everything reads as silver.

---

## Hosting From PC

**Step 1 — Run the local server**
```bash
cd "Toyota Offroad Simulator"
node server/server.js
# Model viewer: http://localhost:3000/tacoma-model.html
# Original game: http://localhost:3000/index.html
```

**Step 2 — Expose to the internet (Cloudflare Tunnel)**
```bash
cloudflared tunnel --url http://localhost:3000
# Get a public URL: https://random-name.trycloudflare.com
```

---

## Model Viewer Controls

| Input | Action |
|-------|--------|
| Drag | Orbit around truck |
| Scroll | Zoom in/out |
| Double-click | Reset camera + resume auto-spin |
| Color swatches | Change paint color |
| M key | Toggle material debug panel |

---

## For Claude: How to Start Each Session

1. Read `PROJECT.md` (this file) first
2. Read `game/tacoma-model.html` — it's the active working file
3. Check `dev-notes/ideas.md` for any notes Taylor left
4. Ask Taylor what to focus on if unclear
5. Commit to git before major changes: `git add -A && git commit -m "description"`

---
