#!/usr/bin/env python3
"""
Demo for uCanvas_GL_Client

Run from the repo root: `python examples/uCanvas_GL_Client_demo.py`
Adjust `PORT` to your serial device.
"""

from uCanvasClientTest.uCanvas_GL_Client import UCanvasGLClient
import math
import struct
import time

PORT = "COM5"  # adjust as needed


def main():
    with UCanvasGLClient(PORT) as cli:
        print("Ping ->", cli.ping())

        print("Creating scene...")
        scene_id = cli.new_scene()
        print("  scene_id:", scene_id)

        print("Creating instance 0 (EK79007, 1024x600)...")
        cli.new_instance(0, 0x02, 1024, 600, 0, 0)
        print("Attaching renderbuffer...")
        cli.attach_renderbuffer(0, 1024, 600)
        print("Setting panel refresh to 2ms")
        cli.set_panel_refresh(0, 2)
        print("Setting viewport to 0,0")
        cli.set_viewport_pos(0, 0, 0)

        print("Attaching scene to instance...")
        cli.attach_scene(0, scene_id)

        print("Setting render mode AUTO_REFRESH")
        cli.set_render_mode(0, 0)

        print("Attaching renderer (core 1)")
        cli.attach_renderer(0, 1)

        print("Setting active scene")
        cli.set_active_scene(scene_id)

        print("Creating space floor...")
        plane_handle = cli.create_rect(0, 540, 1024, 60)
        print("  floor handle:", plane_handle)

        print("Creating sun...")
        sun_handle = cli.create_circle(840, 140, 60)
        print("  sun handle:", sun_handle)

        print("Creating planet...")
        planet_handle = cli.create_circle(560, 320, 34)
        print("  planet handle:", planet_handle)

        print("Creating moon...")
        moon_handle = cli.create_circle(620, 320, 14)
        print("  moon handle:", moon_handle)

        print("Creating ring shadow...")
        ring_handle = cli.create_rect(520, 316, 128, 8)
        print("  ring handle:", ring_handle)

        if plane_handle is None or sun_handle is None or planet_handle is None or moon_handle is None or ring_handle is None:
            print("Failed to create one or more shapes")
            return

        cli.set_color(plane_handle, 16, 24, 36)
        cli.set_fill(plane_handle, 1)
        cli.set_visibility(plane_handle, 1)

        cli.set_color(sun_handle, 255, 220, 100)
        cli.set_fill(sun_handle, 1)
        cli.set_visibility(sun_handle, 1)

        cli.set_color(planet_handle, 80, 160, 240)
        cli.set_fill(planet_handle, 1)
        cli.set_visibility(planet_handle, 1)

        cli.set_color(moon_handle, 220, 220, 220)
        cli.set_fill(moon_handle, 1)
        cli.set_visibility(moon_handle, 1)

        cli.set_color(ring_handle, 84, 108, 142)
        cli.set_fill(ring_handle, 1)
        cli.set_visibility(ring_handle, 1)

        print("Animating solar system...")
        origin_x = 840
        origin_y = 140
        orbit_radius = 200
        moon_radius = 56

        for frame in range(360):
            sun_glow = max(180, min(255, 220 + int(30 * math.sin(frame * 0.12))))
            cli.set_color(sun_handle, 255, sun_glow, 96)

            theta = frame * 0.02
            planet_x = int(origin_x + orbit_radius * math.cos(theta))
            planet_y = int(origin_y + orbit_radius * math.sin(theta))
            cli.set_position(planet_handle, planet_x, planet_y)

            ring_x = planet_x - 64
            ring_y = planet_y + 22
            cli.set_position(ring_handle, ring_x, ring_y)

            moon_theta = theta * 4.2
            moon_x = int(planet_x + moon_radius * math.cos(moon_theta))
            moon_y = int(planet_y + moon_radius * math.sin(moon_theta))
            cli.set_position(moon_handle, moon_x, moon_y)

            if frame % 90 == 0:
                planet_fill = 1 if (frame // 90) % 2 == 0 else 0
                cli.set_fill(planet_handle, planet_fill)

            if frame % 120 == 0:
                ring_visible = 1 if (frame // 120) % 2 == 0 else 0
                cli.set_visibility(ring_handle, ring_visible)

            # time.sleep(0.016)


if __name__ == "__main__":
    main()
