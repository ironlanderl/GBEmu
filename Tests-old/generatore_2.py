import os

os.system("cls")

registers = ["A", "B", "C", "D", "E", "H", "L", "PC", "SP"]
flags = ["ZeroFlag", "NegativeFlag", "HalfCarry", "Carry"]

base_register_string = """
if (gameboy.{register} != std::strtoul(testData["final"]["cpu"]["{register_lower}"].get<std::string>().c_str(), nullptr, 16)) {{
    std::printf("Il registro {register} è diverso. Atteso: {dimensione}, Attuale: {dimensione}\\n", std::strtoul(testData["final"]["cpu"]["{register_lower}"].get<std::string>().c_str(), nullptr, 16), gameboy.{register});
    return false;
}}
"""

base_flag_string = """
if (gameboy.{flag} != (std::strtoul(testData["final"]["cpu"]["f"].get<std::string>().c_str(), nullptr, 16) & {mask}) >> {movement}) {{
    std::printf("Il flag {flag} è diverso. Atteso: %d, Attuale: %d\\n", (std::strtoul(testData["final"]["cpu"]["f"].get<std::string>().c_str(), nullptr, 16) & {mask}) >> {movement}, gameboy.{flag});
    return false;
}}

"""

for register in registers:
    print(base_register_string.format(register=register, register_lower=register.lower(), dimensione="%02X" if register in ["A", "B", "C", "D", "E", "H", "L"] else "%04X"))

for flag in flags:
    print(base_flag_string.format(flag=flag, mask=0b1000_0000 >> flags.index(flag), movement=7 - flags.index(flag)))
