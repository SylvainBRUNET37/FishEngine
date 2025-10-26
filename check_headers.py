import os
import subprocess
import tempfile
import glob

# 🔧 À modifier selon ton environnement
PROJECT_INCLUDE_PATHS = [
    r"C:\Users\Sylvain\Documents\Datas\Cours\Sherbrooke\FishEngine\include",   # Dossier où sont les .h publics
]
CL_PATH = r"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\*\bin\Hostx64\x64\cl.exe"

def find_cl():
    import glob
    matches = glob.glob(CL_PATH)
    if not matches:
        raise RuntimeError("❌ cl.exe non trouvé. Vérifie ton chemin Visual Studio.")
    return matches[0]

def check_header(cl, header, includes):
    with tempfile.NamedTemporaryFile(suffix=".cpp", delete=False) as f:
        f.write(f'#include "{header}"\n'.encode())
        f.flush()
        cmd = [cl, f.name, "/nologo", "/c"] + [f'/I{inc}' for inc in includes]
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"⚠️ {header} ne compile pas seul :")
            print(result.stderr.splitlines()[0:5], "\n")
        else:
            print(f"✅ {header} OK")

def main():
    cl = find_cl()
    includes = PROJECT_INCLUDE_PATHS
    for path in PROJECT_INCLUDE_PATHS:
        for header in glob.glob(os.path.join(path, "**", "*.h"), recursive=True):
            check_header(cl, header, includes)

if __name__ == "__main__":
    main()
