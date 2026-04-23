entry:str = "-Wl,-entry=nomain"

flags:list[str] = [
    "-nostdlib",
    "-fno-builtin",
    "-fno-ident",
    "-Wl,--file-alignment,16",
    "-Wl,--section-alignment,16",
    "-ffreestanding",

    "-funsigned-char",
    "",
]

libs:list[str] = [
    "-lkernel32",
]

if __name__ == "__main__":
    import os
    import sys

    argv:list[str] = sys.argv

    asm:bool = False
    if "-S" in argv: #asm
        asm = True
        flags.insert(0, "-S")

    for i in range(4):
        if f"-O{i}" in argv:
            flags.insert(0, f"-O{i}")
            break

    if "--release" in argv:
        flags.insert(0,"-s")

    if "--debug" in argv:
        flags.insert(0,"-g")

    com:str = f"gcc game.c {" ".join([x for x in flags])} {entry} {" ".join([x for x in libs])} -o game.{"s" if asm else "exe"}"
    print(com)
    exit_code:int = os.system(com);
    sys.exit(exit_code);