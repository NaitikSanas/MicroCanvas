#!/usr/bin/env python3
"""
uCanvas Server - UART test client (POC)

Sends CREATE_RECT / SET_COLOR / SET_FILL / SET_VISIBILITY commands and
prints back the server's responses.

Frame format:
  [SOF 0xAA] [LEN_LO] [LEN_HI] [CMD] [PAYLOAD...] [CRC8]
  LEN = len(PAYLOAD), CRC8 over CMD+PAYLOAD (poly 0x07, init 0x00)
"""

import serial
import time
import struct

# ---- Opcodes (mirror uCanvasServer_Protocol.h) ----
SOF = 0xAA

CMD_CREATE_RECT    = 0x10
CMD_CREATE_CIRCLE  = 0x11
CMD_SET_POSITION   = 0x20
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

# Panel types
PANEL_DUMMY  = 0x00
PANEL_ST7789 = 0x01
UCS_PANEL_EK79007 = 0x02

# Render modes
RENDER_AUTO_REFRESH = 0x00

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


def read_frame(ser: serial.Serial, timeout=1.0):
    """Reads one frame from the serial port. Returns (cmd, payload) or None."""
    start = time.time()
    state = "SOF"
    length = 0
    cmd = 0
    payload = bytearray()

    while time.time() - start < timeout:
        b = ser.read(1)
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
                print(f"  [WARN] CRC mismatch (got 0x{byte:02X}, expected 0x{expected:02X})")
                return None
            return cmd, bytes(payload)

    return None


def main():
    port = "COM5"   # adjust as needed (e.g. COM5 on Windows)
    baud = 250000

    ser = serial.Serial(port, baud, timeout=0.5)
    time.sleep(0.2)  # let port settle

    # --- Ping ---
    print("Sending PING...")
    ser.write(build_frame(CMD_PING))
    resp = read_frame(ser)
    print("  Response:", resp)

    

    print("Creating instance 0 (EK79007, 1024x600, offset 0,0)...")
    payload = bytes([0, UCS_PANEL_EK79007]) + struct.pack("<HHHH",1024,600, 0, 0)
    ser.write(build_frame(CMD_NEW_INSTANCE, payload))
    print("  Response:", read_frame(ser))

    print("Attaching render buffer (1024x600)...")
    payload = bytes([0]) + struct.pack("<HH", 1024, 600)
    ser.write(build_frame(CMD_ATTACH_RENDERBUFFER, payload))
    print("  Response:", read_frame(ser))

    

    print("Setting panel refresh delay (2ms)uCanvas_Set_ViewPort_Position..")
    payload = bytes([0]) + struct.pack("<H", 2)
    ser.write(build_frame(CMD_SET_PANEL_REFRESH, payload))
    print("  Response:", read_frame(ser))

    print("Setting viewport position (0,0)...")
    payload = bytes([0]) + struct.pack("<HH", 0, 0)
    ser.write(build_frame(CMD_SET_VIEWPORT_POS, payload))
    print("  Response:", read_frame(ser))

    # # --- Bootstrap: create scene + instance (mirrors example setup) ---
    print("Creating scene...")
    ser.write(build_frame(CMD_NEW_SCENE))
    resp = read_frame(ser)
    print("  Response:", resp)
    scene_id = struct.unpack("<H", resp[1])[0]
    print(f"  Got scene_id: {scene_id}")

    print("Attaching scene to instance...")
    payload = bytes([0]) + struct.pack("<H", scene_id)
    ser.write(build_frame(CMD_ATTACH_SCENE, payload))
    print("  Response:", read_frame(ser))

    print("Setting render mode (AUTO_REFRESH)...")
    payload = bytes([0, RENDER_AUTO_REFRESH])
    ser.write(build_frame(CMD_SET_RENDER_MODE, payload))
    print("  Response:", read_frame(ser))

    print("Attaching renderer (core 1)...")
    payload = bytes([0, 1])
    ser.write(build_frame(CMD_ATTACH_RENDERER, payload))
    print("  Response:", read_frame(ser))

  

    print("Setting active scene...")
    payload = struct.pack("<H", scene_id)
    ser.write(build_frame(CMD_SET_ACTIVE_SCENE, payload))
    print("  Response:", read_frame(ser))

    # --- Create a rectangle: x=10, y=10, h=50, w=80 ---
    print("Creating rectangle...")
    payload = struct.pack("<HHHH", 10, 10, 50, 80)  # x, y, h, w (little-endian u16)
    ser.write(build_frame(CMD_CREATE_RECT, payload))
    resp = read_frame(ser)
    print("  Response:", resp)

    if resp is None or resp[0] != RSP_HANDLE:
        print("Failed to create rectangle, aborting.")
        return

    handle = struct.unpack("<H", resp[1])[0]
    print(f"  Got handle: {handle}")

    # --- Set color: red (255, 0, 0) ---
    print("Setting color to red...")
    payload = struct.pack("<H", handle) + bytes([255, 0, 0])
    ser.write(build_frame(CMD_SET_COLOR, payload))
    resp = read_frame(ser)
    print("  Response:", resp)

    # --- Set fill (1 = FILL, adjust to match your fill_t enum) ---
    print("Setting fill...")
    payload = struct.pack("<H", handle) + bytes([1])
    ser.write(build_frame(CMD_SET_FILL, payload))
    resp = read_frame(ser)
    print("  Response:", resp)

    # --- Set visibility (1 = VISIBLE, adjust to match your enum) ---
    print("Setting visibility...")
    payload = struct.pack("<H", handle) + bytes([1])
    ser.write(build_frame(CMD_SET_VISIBILITY, payload))
    resp = read_frame(ser)
    print("  Response:", resp)

# --- Create a rectangle: x=10, y=10, h=50, w=80 ---
    print("Creating rectangle...")
    payload = struct.pack("<HHHH", 10, 40, 50, 80)  # x, y, h, w (little-endian u16)
    ser.write(build_frame(CMD_CREATE_RECT, payload))
    resp = read_frame(ser)
    print("  Response:", resp)

    if resp is None or resp[0] != RSP_HANDLE:
        print("Failed to create rectangle, aborting.")
        return

    handle2 = struct.unpack("<H", resp[1])[0]
    print(f"  Got handle: {handle}")

    # --- Set color: red (255, 0, 0) ---
    print("Setting color to red...")
    payload = struct.pack("<H", handle2) + bytes([255, 0, 0])
    ser.write(build_frame(CMD_SET_COLOR, payload))
    resp = read_frame(ser)
    print("  Response:", resp)

    # --- Set fill (1 = FILL, adjust to match your fill_t enum) ---
    print("Setting fill...")
    payload = struct.pack("<H", handle2) + bytes([1])
    ser.write(build_frame(CMD_SET_FILL, payload))
    resp = read_frame(ser)
    print("  Response:", resp)

    # --- Set visibility (1 = VISIBLE, adjust to match your enum) ---
    print("Setting visibility...")
    payload = struct.pack("<H", handle2) + bytes([1])
    ser.write(build_frame(CMD_SET_VISIBILITY, payload))
    resp = read_frame(ser)
    print("  Response:", resp)

    # --- Move it around a bit ---
    print("Moving rectangle...")
    for x in range(10, 1024, 1):
        payload = struct.pack("<HHH", handle, x, 10)
        ser.write(build_frame(CMD_SET_POSITION, payload))
        resp = read_frame(ser)
        payload = struct.pack("<HHH", handle2, x, 140)
        ser.write(build_frame(CMD_SET_POSITION, payload))
        resp = read_frame(ser)

        print(f"  x={x} -> Response:", resp)
        # time.sleep(0.001)

    for x in range(1024, 0, 1):
        payload = struct.pack("<HHH", handle, x, 10)
        ser.write(build_frame(CMD_SET_POSITION, payload))
        resp = read_frame(ser)
        payload = struct.pack("<HHH", handle2, x, 140)
        ser.write(build_frame(CMD_SET_POSITION, payload))
        resp = read_frame(ser)

        print(f"  x={x} -> Response:", resp)
        # time.sleep(0.001)

    ser.close()


if __name__ == "__main__":
    main()