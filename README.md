# Off-Road Truck Simulator

A browser-based off-road driving simulator built with Three.js and cannon.js. It runs in any modern browser with nothing to install. A small Node server hosts it from my PC.

> 📸 *Add a screenshot or GIF here: `docs/screenshot.png`*

## What's here

| Path | What it is |
|---|---|
| `game/index.html` | Driving prototype: cannon.js vehicle physics on a box-geometry truck |
| `game/tacoma-model.html` | Model viewer: real-time PBR materials, clearcoat paint, a procedural studio environment, orbit controls and a paint picker |
| `server/server.js` | Zero-dependency Node static server (port 3000) |
| `dual_motor_webserver/` | ESP32 + SimpleFOC firmware that drives two BLDC motors from a web page. It's an early step toward a physical motion rig. |
| `PROJECT.md` | Running dev notes: what works, what broke, and why |

## Run it

```bash
node server/server.js
# then open http://localhost:3000
```

The model viewer needs a truck model saved as `game/toyota_tacoma_trd.glb`. I used a model from Sketchfab, and it isn't included here because it isn't mine to redistribute.

### Motor controller
1. Install **SimpleFOC** from the Arduino Library Manager.
2. Copy `secrets.example.h` to `secrets.h` and add your WiFi details.
3. Set the board to **ESP32 Dev Module** with the partition scheme **Huge APP**, then flash.
4. Open the board's IP in a browser to set speed and voltage limit per motor.

## Things I learned

- **Material matching:** keyword guessing turned the whole truck pink. An exact mesh-to-material lookup table fixed it.
- **Lighting:** a bright studio HDRI washed every paint color out to silver. Switching to a dark procedural environment and keeping `envMapIntensity` ≤ 1.0 fixed it.
- **Glass:** even `transmission: 0.15` made the windows see-through.

## Next

- Swap the box truck in the driving game for the real model
- Add bloom and depth-of-field post-processing
- Add terrain textures and more maps
