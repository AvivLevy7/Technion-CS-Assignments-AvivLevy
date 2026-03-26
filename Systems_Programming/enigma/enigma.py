import json
import sys


class JSONFileError(Exception):
    pass


class Enigma:
    def __init__(self, hash_map, wheels, reflector_map):
        self.hash_map = hash_map
        self.wheels = wheels
        self.reflector_map = reflector_map

    def encrypt(self, message):
        encrypted_message = ""
        counter = 0
        c1 = c3 = None
        w1, w2, w3 = self.wheels[0], self.wheels[1], self.wheels[2]
        for c in message:
            if ord("a") <= ord(c) <= ord("z"):
                i = self.hash_map[c]
                num = ((2 * w1) - w2 + w3) % 26
                if num != 0:
                    i += num
                else:
                    i += 1
                i = i % 26
                for key, value in self.hash_map.items():
                    if value == i:
                        c1 = key
                        break
                c2 = self.reflector_map[c1]
                i = self.hash_map[c2]
                if num != 0:
                    i -= num
                else:
                    i -= 1
                i = i % 26
                for key, value in self.hash_map.items():
                    if value == i:
                        c3 = key
                        break
                encrypted_message += c3
                counter += 1
            else:
                encrypted_message += c
            if w1 >= 8:
                w1 = 1
            else:
                w1 += 1
            if counter % 2 == 0:
                w2 *= 2
            else:
                w2 -= 1
            if counter % 10 == 0:
                w3 = 10
            elif counter % 3 == 0:
                w3 = 5
            else:
                w3 = 0
        return encrypted_message

def load_enigma_from_path(path):
    try:
        with open(path, 'r') as f:
            file_dict = json.load(f)
        enigma = Enigma(file_dict["hash_map"], file_dict["wheels"], file_dict["reflector_map"])
        return enigma
    except (ValueError, IOError):
        raise JSONFileError

try:
    new_path = input_file = output_file = None
    for arg in range(1,len(sys.argv) - 1,2):
        if sys.argv[arg] == '-c':
            new_path = sys.argv[arg + 1]
        elif sys.argv[arg] == '-i':
            input_file = sys.argv[arg + 1]
        elif sys.argv[arg] == '-o':
            output_file = sys.argv[arg + 1]
        else:
            raise ValueError
    if '-i' not in sys.argv or '-c' not in sys.argv:
        raise ValueError
    new_enigma = load_enigma_from_path(new_path)
    with open(input_file, 'r') as f_in:
        if len(sys.argv) == 7:
            with open(output_file, 'w') as f_out:
                for line in f_in:
                    new_message = line
                    new_encrypted_message = new_enigma.encrypt(new_message)
                    f_out.write(new_encrypted_message)
        else:
            for line in f_in:
                new_message = line
                new_encrypted_message = new_enigma.encrypt(new_message)
                sys.stdout.write(new_encrypted_message)
except ValueError:
    sys.stderr.write("Usage: python3 enigma.py -c <config_file> -i <input_file> -o <output_file>\n")
    sys.exit(1)
except (JSONFileError, IOError):
    sys.stderr.write("The enigma script has encountered an error")
    sys.exit(1)