#!/usr/bin/env python3
"""
uCanvas GL Client

Lightweight Python client implementing the uCanvas UART protocol.
Provides a convenience class `UCanvasGLClient` with methods mirroring
the MicroCanvas C API used in examples.
"""

import serial
import time
import struct

# ---- Opcodes (mirror uCanvasServer_Protocol.h) ----
SOF = 0xAA

CMD_CREATE_RECT    = 0x10
CMD_CREATE_CIRCLE  = 0x11
CMD_SET_POSITION   = 0x20
CMD_SET_POSITION_X = 0x21
CMD_SET_POSITION_Y = 0x22
CMD_SET_COLOR      = 0x23
CMD_SET_FILL       = 0x24
CMD_SET_VISIBILITY = 0x25
CMD_PING           = 0x50

CMD_NEW_INSTANCE        = 0x06
CMD_ATTACH_RENDERBUFFER = 0x07
CMD_SET_VIEWPORT_POS    = 0x08
CMD_ATTACH_SCENE        = 0x09
CMD_SET_PANEL_REFRESH   = 0x0A
CMD_SET_RENDER_MODE     = 0x0B
CMD_ATTACH_RENDERER     = 0x0C

CMD_NEW_SCENE       = 0x40
CMD_SET_ACTIVE_SCENE = 0x41

# Responses
RSP_SCENE_ID = 0x84
RSP_ERROR  = 0x81
RSP_HANDLE = 0x82
RSP_PONG   = 0x86


def crc8(data: bytes) -> int:
    crc = 0x00
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 0x80:
                crc = ((crc << 1) ^ 0x07) & 0xFF
            else:
                crc = (crc << 1) & 0xFF
    return crc


def build_frame(cmd: int, payload: bytes = b"") -> bytes:
    length = len(payload)
    body = bytes([cmd]) + payload
    frame = bytes([SOF, length & 0xFF, (length >> 8) & 0xFF]) + body + bytes([crc8(body)])
    return frame


class UCanvasGLClient:
    def __init__(self, port: str, baud: int = 250000, timeout: float = 0.5):
        self.port = port
        self.baud = baud
        self.timeout = timeout
        self.ser = serial.Serial(port, baud, timeout=timeout)
        time.sleep(0.05)

    def close(self):
        if self.ser and self.ser.is_open:
            self.ser.close()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()

    def read_frame(self, timeout: float = 1.0):
        start = time.time()
        state = "SOF"
        length = 0
        cmd = 0
        payload = bytearray()

        while time.time() - start < timeout:
            b = self.ser.read(1)
            if not b:
                continue
            byte = b[0]

            if state == "SOF":
                if byte == SOF:
                    state = "LEN_LO"
            elif state == "LEN_LO":
                length = byte
                state = "LEN_HI"
            elif state == "LEN_HI":
                length |= (byte << 8)
                state = "CMD"
            elif state == "CMD":
                cmd = byte
                payload = bytearray()
                state = "CRC" if length == 0 else "PAYLOAD"
            elif state == "PAYLOAD":
                payload.append(byte)
                if len(payload) >= length:
                    state = "CRC"
            elif state == "CRC":
                expected = crc8(bytes([cmd]) + bytes(payload))
                if expected != byte:
                    return None
                return cmd, bytes(payload)

        return None

    def send_and_wait(self, cmd: int, payload: bytes = b"", timeout: float = 1.0):
        frame = build_frame(cmd, payload)
        self.ser.write(frame)
        return self.read_frame(timeout)

    # Convenience API methods
    def ping(self):
        return self.send_and_wait(CMD_PING)

    def new_scene(self):
        resp = self.send_and_wait(CMD_NEW_SCENE)
        if resp and resp[0] == RSP_SCENE_ID and len(resp[1]) >= 2:
            return struct.unpack("<H", resp[1][:2])[0]
        return None

    def set_active_scene(self, scene_id: int):
        payload = struct.pack("<H", scene_id)
        return self.send_and_wait(CMD_SET_ACTIVE_SCENE, payload)

    def new_instance(self, instance_id: int, panel_type: int, width: int, height: int, offx: int = 0, offy: int = 0):
        payload = bytes([instance_id, panel_type]) + struct.pack("<HHHH", width, height, offx, offy)
        return self.send_and_wait(CMD_NEW_INSTANCE, payload)

    def attach_renderbuffer(self, instance_id: int, width: int, height: int):
        payload = bytes([instance_id]) + struct.pack("<HH", width, height)
        return self.send_and_wait(CMD_ATTACH_RENDERBUFFER, payload)

    def set_panel_refresh(self, instance_id: int, delay_ms: int):
        payload = bytes([instance_id]) + struct.pack("<H", delay_ms)
        return self.send_and_wait(CMD_SET_PANEL_REFRESH, payload)

    def set_viewport_pos(self, instance_id: int, x: int, y: int):
        payload = bytes([instance_id]) + struct.pack("<HH", x, y)
        return self.send_and_wait(CMD_SET_VIEWPORT_POS, payload)

    def attach_scene(self, instance_id: int, scene_id: int):
        payload = bytes([instance_id]) + struct.pack("<H", scene_id)
        return self.send_and_wait(CMD_ATTACH_SCENE, payload)

    def set_render_mode(self, instance_id: int, mode: int):
        payload = bytes([instance_id, mode])
        return self.send_and_wait(CMD_SET_RENDER_MODE, payload)

    def attach_renderer(self, instance_id: int, core: int = 1):
        payload = bytes([instance_id, core])
        return self.send_and_wait(CMD_ATTACH_RENDERER, payload)

    def create_rect_raw(self, x: int, y: int, h: int, w: int):
        payload = struct.pack("<HHHH", x, y, h, w)
        return self.send_and_wait(CMD_CREATE_RECT, payload)

    def create_rect(self, x: int, y: int, h: int, w: int):
        """Create a rectangle and return its handle (int) on success, or None."""
        resp = self.create_rect_raw(x, y, h, w)
        if resp is None:
            return None
        cmd, payload = resp
        if cmd != RSP_HANDLE or len(payload) < 2:
            return None
        return struct.unpack("<H", payload[:2])[0]

    def create_circle_raw(self, x: int, y: int, radius: int):
        payload = struct.pack("<HHH", x, y, radius)
        return self.send_and_wait(CMD_CREATE_CIRCLE, payload)

    def create_circle(self, x: int, y: int, radius: int):
        """Create a circle and return its handle (int) on success, or None."""
        resp = self.create_circle_raw(x, y, radius)
        if resp is None:
            return None
        cmd, payload = resp
        if cmd != RSP_HANDLE or len(payload) < 2:
            return None
        return struct.unpack("<H", payload[:2])[0]

    def set_color(self, handle: int, r: int, g: int, b: int):
        payload = struct.pack("<H", handle) + bytes([r, g, b])
        return self.send_and_wait(CMD_SET_COLOR, payload)

    def set_fill(self, handle: int, fill: int):
        payload = struct.pack("<H", handle) + bytes([fill])
        return self.send_and_wait(CMD_SET_FILL, payload)

    def set_visibility(self, handle: int, vis: int):
        payload = struct.pack("<H", handle) + bytes([vis])
        return self.send_and_wait(CMD_SET_VISIBILITY, payload)

    def set_position(self, handle: int, x: int, y: int):
        if not (0 <= x <= 0xFFFF) or not (0 <= y <= 0xFFFF):
            raise ValueError(f"set_position values must be 0..65535, got x={x}, y={y}")
        payload = struct.pack("<HHH", handle, x, y)
        return self.send_and_wait(CMD_SET_POSITION, payload)

    def set_position_x(self, handle: int, x: int):
        if not (0 <= x <= 0xFFFF):
            raise ValueError(f"set_position_x value must be 0..65535, got x={x}")
        payload = struct.pack("<HHH", handle, x)
        return self.send_and_wait(CMD_SET_POSITION_X, payload)
        payload = struct.pack("<HH", handle, x)
        return self.send_and_wait(CMD_SET_POSITION_X, payload)

    def set_position_y(self, handle: int, y: int):
        payload = struct.pack("<HH", handle, y)
        return self.send_and_wait(CMD_SET_POSITION_Y, payload)
