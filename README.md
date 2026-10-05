# Gen IV Event Distributor

Nintendo DS homebrew for distributing Pokémon Generation IV Mystery Gift events to retail Pokémon games through local wireless communication.

Created by **SooraMaru**.

> ⚠️ **PRE-ALPHA**
>
> This project is currently being tested. Bugs, freezes, and compatibility issues may still occur. Feedback and testing on real hardware are welcome.

[Español](#español)

---

## English

### What is Gen IV Event Distributor?

**Gen IV Event Distributor** turns a Nintendo DS into a wireless Mystery Gift distribution system for Generation IV Pokémon games.

It broadcasts Wonder Cards (`.pcd`) using the Nintendo DS local wireless connection. Another Nintendo DS running a compatible retail Pokémon game can detect and receive the event through the game's normal **Mystery Gift** menu.

The receiving Nintendo DS does **not** need custom firmware or homebrew.

### Supported games

Currently supported:

- Pokémon Diamond
- Pokémon Pearl
- Pokémon HeartGold
- Pokémon SoulSilver

Wonder Cards are separated by game according to compatibility tests performed on real Nintendo DS hardware.

Not every Wonder Card is compatible with every Generation IV game.

### Supported languages

The current event catalog contains:

- English
- Spanish

Events are separated by language to prevent incompatible Wonder Cards from being broadcast using the wrong language settings.

### How to use

1. Launch **Gen IV Event Distributor** on the distributing Nintendo DS.
2. Select the event language.
3. Select the Pokémon game.
4. Select the Wonder Card you want to distribute.
5. Press **A** to start broadcasting.
6. On the receiving Nintendo DS, launch the Pokémon game.
7. Open **Mystery Gift**.
8. Select the option to receive a gift via wireless.
9. Wait for the event to appear and receive it normally.

Press **B** on the distributor to stop broadcasting.

After stopping a distribution, the application waits approximately **3 seconds** before another event can be broadcast. This delay is intentional and improves stability when switching between events.

### Debug mode

The normal bottom screen only displays basic information such as:

- Distribution status
- Active wireless channel

Press **L + R** simultaneously to enable or disable the debug screen.

Debug mode displays additional information about the wireless transmitter and is useful when reporting bugs.

### Building

You need a recent installation of **devkitPro**, including **devkitARM** and the Nintendo DS development libraries.

Build ARM7 first:

```sh
cd arm7
make
```

Then build ARM9:

```sh
cd ../arm9
make
```

The resulting `.nds` file can then be launched through a compatible Nintendo DS homebrew environment.

### Current status

Gen IV Event Distributor is currently a **PRE-ALPHA**.

Wireless event distribution is functional and has been tested on real Nintendo DS hardware, but additional testing is needed.

If you encounter a problem, please include the following information in your report:

- Pokémon game
- Game language
- Wonder Card/event
- Nintendo DS model
- What happened
- Whether the problem occurs consistently or intermittently
- Debug information, if available

### Credits

**SooraMaru**  
Development, integration, interface, and real-hardware testing.

Special thanks to:

- **devkitPro / libnds / Calico** — Nintendo DS homebrew development tools.
- **Eiskasten / wc-beacon** — Open-source work and research related to Generation IV Wonder Card broadcasting.
- **Yuuto** — Research related to the Generation IV wireless distribution protocol.
- Everyone helping test PRE-ALPHA builds on real Nintendo DS hardware.

See `LICENSE-NOTES.md` for additional licensing and attribution information.

### License

Original code and modifications in this repository are distributed under the **GNU General Public License v3.0 or later (GPL-3.0-or-later)**, except for components or portions covered by their respective original licenses.

See the included license files and `LICENSE-NOTES.md` for details.

### Disclaimer

This is an unofficial fan-made homebrew project.

It is not affiliated with, endorsed by, or supported by Nintendo, The Pokémon Company, GAME FREAK, or Creatures Inc.

Pokémon and related trademarks belong to their respective owners.

This project is intended for homebrew development, preservation, research, and interoperability purposes.

---

# Español

## ¿Qué es Gen IV Event Distributor?

**Gen IV Event Distributor** convierte una Nintendo DS en un sistema de distribución inalámbrica de Regalos Misteriosos para los juegos de Pokémon de cuarta generación.

La aplicación transmite Wonder Cards (`.pcd`) mediante la comunicación inalámbrica local de Nintendo DS. Otra Nintendo DS con un juego compatible puede detectar y recibir el evento utilizando el menú normal de **Regalo Misterioso**.

La Nintendo DS que recibe el regalo **no necesita CFW ni homebrew**.

## Juegos compatibles

Actualmente se incluyen eventos para:

- Pokémon Diamante
- Pokémon Perla
- Pokémon HeartGold
- Pokémon SoulSilver

Las Wonder Cards están separadas por juego según pruebas de compatibilidad realizadas en hardware real.

No todas las Wonder Cards son compatibles con todos los juegos de cuarta generación.

## Idiomas compatibles

El catálogo actual contiene eventos en:

- Inglés
- Español

Los eventos están separados por idioma para evitar transmitir una Wonder Card utilizando una configuración de idioma incompatible.

## Cómo utilizarlo

1. Inicia **Gen IV Event Distributor** en la Nintendo DS distribuidora.
2. Selecciona el idioma del evento.
3. Selecciona el juego de Pokémon.
4. Selecciona la Wonder Card que quieres distribuir.
5. Pulsa **A** para comenzar la transmisión.
6. En la Nintendo DS receptora, inicia el juego de Pokémon.
7. Entra en **Regalo Misterioso**.
8. Selecciona la opción para recibir un regalo mediante conexión inalámbrica.
9. Espera a que aparezca el evento y recíbelo normalmente.

Pulsa **B** en la consola distribuidora para detener la transmisión.

Después de detener una distribución, la aplicación espera aproximadamente **3 segundos** antes de permitir transmitir otro evento. Esta espera es intencional y mejora la estabilidad al cambiar de regalo.

## Modo debug

Normalmente la pantalla inferior únicamente muestra información básica:

- Estado de la distribución
- Canal inalámbrico activo

Pulsa **L + R** simultáneamente para activar o desactivar el modo debug.

El modo debug muestra información adicional sobre el transmisor inalámbrico y puede resultar útil al reportar errores.

## Compilación

Es necesaria una instalación reciente de **devkitPro**, incluyendo **devkitARM** y las librerías de desarrollo para Nintendo DS.

Primero compila ARM7:

```sh
cd arm7
make
```

Después compila ARM9:

```sh
cd ../arm9
make
```

El archivo `.nds` resultante puede ejecutarse mediante un entorno homebrew compatible con Nintendo DS.

## Estado actual

Gen IV Event Distributor se encuentra actualmente en estado **PRE-ALPHA**.

La distribución inalámbrica de eventos es funcional y ha sido probada en hardware real, pero todavía es necesario realizar más pruebas.

Si encuentras algún problema, incluye la siguiente información en tu reporte:

- Juego de Pokémon
- Idioma del juego
- Wonder Card/evento
- Modelo de Nintendo DS
- Qué ocurrió
- Si el problema ocurre siempre o solamente algunas veces
- Información del modo debug, si está disponible

## Créditos

**SooraMaru**  
Desarrollo, integración, interfaz y pruebas en hardware real.

Agradecimientos especiales a:

- **devkitPro / libnds / Calico** — Herramientas de desarrollo homebrew para Nintendo DS.
- **Eiskasten / wc-beacon** — Trabajo open source e investigación relacionada con la distribución de Wonder Cards de cuarta generación.
- **Yuuto** — Investigación relacionada con el protocolo inalámbrico de distribución de cuarta generación.
- Todas las personas que están ayudando a probar las versiones PRE-ALPHA en hardware real.

Consulta `LICENSE-NOTES.md` para información adicional sobre licencias y atribuciones.

## Licencia

El código original y las modificaciones de este repositorio se distribuyen bajo la **GNU General Public License v3.0 o posterior (GPL-3.0-or-later)**, excepto aquellos componentes o partes cubiertos por sus respectivas licencias originales.

Consulta los archivos de licencia incluidos y `LICENSE-NOTES.md` para más información.

## Aviso legal

Este es un proyecto homebrew no oficial creado por fans.

No está afiliado, respaldado ni soportado por Nintendo, The Pokémon Company, GAME FREAK o Creatures Inc.

Pokémon y las marcas relacionadas pertenecen a sus respectivos propietarios.

Este proyecto está destinado al desarrollo homebrew, preservación, investigación e interoperabilidad.
