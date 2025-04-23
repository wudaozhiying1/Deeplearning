import serial
import time
import pyautogui
import re  

port = 'COM4'
baud = 115200


ser = serial.Serial(port, baud, timeout=1)
time.sleep(2)

print("🎧 Volume control (pyautogui + custom thresholds) ready...")


def press_multiple(key, times):
    for _ in range(times):
        pyautogui.press(key)
        time.sleep(0.05)


THRESHOLDS = {
    'move up': 0.70,
    'move down': 0.70,
    'move left': 0.70,
    'move right': 0.40
}


while True:
    line = ser.readline().decode('utf-8').strip()
    if not line:
        continue

    print("Received:", line)

    match = re.match(r"(move (up|down|left|right)):\s*(0\.\d+)", line)
    if match:
        label = match.group(1)
        prob = float(match.group(3))
        threshold = THRESHOLDS.get(label, 1.0)  

        if prob >= threshold:
            if label == "move up":
                press_multiple("volumeup", 5)
                print("🔊 Volume UP ✅")
            elif label == "move down":
                press_multiple("volumedown", 5)
                print("🔉 Volume DOWN ✅")
            elif label == "move left":
                pyautogui.press("volumemute")
                print("🔇 Muted ✅")
            elif label == "move right":

                pyautogui.press("volumemute")
                print("🔈 Unmuted ✅")
        #else:
            ##print(f"⚠️ {label} confidence {prob:.3f} < threshold {threshold}")##

