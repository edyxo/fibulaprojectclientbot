# Fibula Project Client

Cliente custom de **Tibia 7.72** hecho para el servidor **Project Fibula** (`world.fibula.app`).
Escrito en C++ con SDL2. Open source. Si quieres jugar, es bajar y darle doble click — nada más.

> 🌎 *English version: [README.en.md](README.en.md)*

> Este cliente está hecho **específicamente para Project Fibula**. Usa sprites y datos
> propios de ese servidor, así que no está pensado para conectarse a otros servers.

---

## 🎮 Jugar (plug and play)

1. Ve a la pestaña **[Releases](../../releases)** de este repo.
2. Baja el archivo `Fibula-Client.zip`.
3. Descomprímelo donde quieras (Escritorio, una carpeta, lo que sea).
4. Entra a la carpeta y abre **`FibulaClient.exe`**.
5. Escribe tu **cuenta** y **contraseña**, y a jugar.

No hay que configurar IPs ni puertos: el cliente ya viene apuntando solo al servidor de
Fibula. Si no tienes cuenta todavía, créala en el servidor de Project Fibula.

> **Solo Windows (64 bits).** Si Windows SmartScreen te avisa que es de un autor
> desconocido, dale en *Más información → Ejecutar de todas formas* (pasa porque el `.exe`
> no está firmado, es normal en proyectos chicos).

### ¿Qué trae la carpeta?
- `FibulaClient.exe` — el juego.
- `Assets/` — sprites y datos gráficos (Tibia.spr, Tibia.dat, etc.).
- Archivos `.map` — el minimapa ya pre-cargado, para que se vea desde el primer momento.
- `Tibia.cfg` — configuración por defecto.
- `binance-qr.bmp` — el QR de Binance Pay de la ventana de donación.
- `zlib1.dll`, `winmm.dll` — librerías que necesita el `.exe`.

Mantén todos esos archivos **juntos en la misma carpeta**. Si separas el `.exe` de los
demás, no va a funcionar.

---

## ✨ Qué tiene de especial

Además del cliente clásico de 7.72, trae varios asistentes (todos con atajo `Ctrl+Alt+letra`):

- **Mana Trainer** (`Ctrl+Alt+M`) — lanza un hechizo solo cuando te llega cierto maná.
- **Auto Healer** (`Ctrl+Alt+H`) — te cura solo cuando bajas de cierto % de vida.
- **Auto Eat** (`Ctrl+Alt+E`) — come comida sola desde tus mochilas.
- **Auto Fisher** (`Ctrl+Alt+F`) — pesca solo en el agua de al lado.
- **Auto Runer** (`Ctrl+Alt+R`) — hace runas solo cuando tienes maná de sobra.
- **Auto Logout** (`Ctrl+Alt+L`) — te desconecta si entra un GM, se acerca un jugador, te dan PK o por tiempo.
- **Combo Leader** (`Ctrl+Alt+C`) — repite las runas del líder del combo.
- **Movimiento WASD** — W/A/S/D para moverte, Q/E/Z/C en diagonal, `Ctrl+WASD` para voltear.
- **36 hotkeys** — F1-F12, Shift+F1-F12 y Ctrl+F1-F12.
- **Apilar oro con click derecho** — click derecho sobre oro/platino/cristal lo manda a tu backpack o bag abierta.

Todo esto se configura desde el menú **Options** dentro del juego.

> 📖 **¿Cómo se usa cada cosa?** Hay un tutorial completo con capturas en
> **[TUTORIAL.md](TUTORIAL.md)**.

---

## 📸 Capturas

<p align="center">
  <img src="screenshots/01-options-menu.png" alt="Menú Options" width="240">
  <img src="screenshots/06-auto-healer.png" alt="Auto Healer" width="240">
  <img src="screenshots/07-combo-leader.png" alt="Combo Leader" width="240">
</p>
<p align="center">
  <img src="screenshots/03-auto-logout.png" alt="Auto Logout" width="200">
  <img src="screenshots/02-hotkeys.png" alt="Hotkeys" width="260">
  <img src="screenshots/04-auto-runer.png" alt="Auto Runer" width="240">
</p>

Más capturas y la explicación de cada asistente en **[TUTORIAL.md](TUTORIAL.md)**.

---

## 🛠️ Compilar tú mismo (para quien quiera forkear)

El código fuente completo está en la carpeta [`source/`](source/).

### Windows (Visual Studio 2022)
```powershell
MSBuild source\src\visual-studio\visual-studio.sln /p:Configuration=Release /p:Platform=x64
```
Necesitas las *Build Tools de Visual Studio 2022* con el toolset de C++. Las dependencias
(SDL2, libcurl) ya vienen incluidas dentro de `source/src/visual-studio/`.

### Linux (CMake)
El proyecto usa CMake, así que también compila en Linux:
```bash
cd source
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

---

## 📜 Licencia y créditos

Este proyecto es un **fork** basado en **The Forgotten Client** de *Saiyans King*,
publicado bajo la **licencia zlib**. Se conservan los avisos de copyright originales
en el código, tal como exige esa licencia.

Las modificaciones de Fibula Project Client se publican bajo la misma licencia zlib.
Ver [`COPYING.txt`](COPYING.txt).

Los sprites y datos gráficos de Tibia son propiedad de **CipSoft GmbH** y se incluyen
únicamente para poder jugar en Project Fibula.

¿Cómo se hizo este cliente y qué se le cambió? Está todo contado, paso a paso y en
lenguaje normal, en [`HISTORIA-DEL-PROYECTO.md`](HISTORIA-DEL-PROYECTO.md).
