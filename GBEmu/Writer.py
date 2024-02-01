import pyautogui
import time

time.sleep(3)

# loop da 0x00 a 0xFF
for i in range(0x00, 0xFF):
    # scrivi void opcode_{i}();
    # ricorda il newline
    # anche zerofill con 2 cifre
    pyautogui.typewrite(f"void opcode_{i:02X}();\n", interval=0.01)