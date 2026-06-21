# Historia del proyecto — qué se hizo y por qué

Esto es el resumen, en lenguaje normal, de todo lo que se le fue haciendo a este cliente
hasta dejarlo listo para publicar. La idea es que cualquiera que llegue al repo entienda
de dónde salió y qué se tocó, sin necesidad de leer código.

---

## De dónde partimos

El cliente arrancó como un fork de **The Forgotten Client** (un cliente open source de
Tibia hecho por Saiyans King). Sobre esa base se fue armando un cliente custom de
**Tibia 7.72** para el servidor **Project Fibula**.

La versión del protocolo está fija en **772**, así que el cliente solo habla con
servidores 7.72. El servidor al que se conecta (`world.fibula.app`) viene escrito dentro
del propio cliente, por eso el usuario no tiene que configurar nada de red.

---

## Lo que se le agregó (los asistentes)

Con el tiempo se le fueron sumando asistentes para que jugar fuera más cómodo, todos
copiando el mismo patrón: un atajo `Ctrl+Alt+letra` y una ventanita de configuración en
el menú *Options*. Los que quedaron en la versión pública son:

- Mana Trainer, Auto Healer, Auto Eat, Auto Fisher, Auto Runer, Auto Logout y Combo Leader.
- Movimiento con WASD y 36 hotkeys configurables.
- Apilar oro a la mochila con click derecho.

---

## Lo que se quitó (los bots automáticos)

En cierto momento el cliente también tenía tres bots más agresivos: **Cavebot**
(caminar solo por waypoints), **Auto Target** (atacar enemigos solo) y **Auto Loot**
(saquear cadáveres solo).

Se decidió **quitarlos por completo** (no solo el atajo, sino el código entero), porque
no aportaban a la experiencia que se buscaba. Lo único que se conservó de todo eso fue el
detalle cómodo de **apilar el oro con click derecho** a la backpack o a la bag — eso se
quedó porque es útil y no molesta.

---

## La separación entre "cliente de la gente" y "cliente de desarrollo"

El cliente traía además unas herramientas pensadas solo para desarrollar y depurar:

- **UI Inspector** (`Ctrl+Alt+I`) — para inspeccionar la interfaz.
- **Recorder** (`Ctrl+Alt+V`) — para grabar clicks y movimientos.
- **Low Profile** (`Ctrl+Alt+P`) — un modo que esconde paneles.
- Un **Modo Dev** con coordenadas de depuración.

Todo eso **no tiene sentido para quien solo quiere jugar**, así que se separó en dos:

- **Versión pública** (la de este repo): esas herramientas se desactivaron. Se quitó la
  casilla de "Dev Mode" del menú, de modo que un usuario normal no puede activarlas. El
  cliente queda limpio, sin nada de desarrollo a la vista.
- **Versión de desarrollo** (privada): conserva todas esas herramientas para seguir
  trabajando en el cliente.

Así la gente recibe un cliente sencillo y limpio, y el desarrollo sigue teniendo sus
herramientas aparte.

---

## El arreglo del minimapa (el mapa que no se veía)

Había un detalle molesto: **el minimapa salía en negro**. El mundo del juego se veía
perfecto, pero el cuadrito del mapa arriba a la derecha estaba vacío.

La causa: el cliente guardaba y leía los datos del minimapa (los archivos `.map`) en una
carpeta escondida del sistema (`AppData`), pero el minimapa que veníamos preparando estaba
**junto al `.exe`**. Como buscaba en otro lado, nunca lo encontraba y por eso salía negro.

El arreglo fue hacer que el cliente lea y guarde el minimapa **en su propia carpeta**, al
lado del `.exe`. Así el mapa que viene incluido se ve desde el primer momento (plug and
play), y lo que vas explorando se guarda ahí mismo.

---

## Detalles de pulido de la interfaz

- **Hueco en el menú Options:** al quitar el viejo botón de "Auto Loot" había quedado un
  espacio en blanco en el menú. Se reacomodaron los botones para que no quede ese hueco.
- **Lista de atajos desactualizada:** al entrar al juego, el cliente mostraba en el log
  una lista de atajos que todavía mencionaba bots que ya se habían eliminado (Cavebot,
  Auto Target, Auto Loot). Se limpió esa lista para que muestre solo lo que de verdad
  existe.

---

## Cómo quedó para publicar

Al final se armó este repositorio con dos cosas:

1. **El código fuente completo** (`source/`), para que cualquiera pueda verlo, aprender o
   forkearlo. Compila en Windows (Visual Studio) y en Linux (CMake).
2. **Una carpeta lista para jugar** (`Fibula-Client/`), que también se ofrece comprimida en
   la sección de *Releases*: bajar, descomprimir, doble click y a jugar.

Se conservaron los créditos y la licencia original (zlib) del proyecto del que partió,
porque eso es lo correcto y lo que pide esa licencia.
