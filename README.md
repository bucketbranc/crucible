> Pryecto final de parcial

## Compilacion
> el programa esta diseñado especificamente para linux con ncurses, sin embargo es posible compliarlo para windows can algunas modificaciones.

### Linux
el proseso aqui es mas facil ya que el programa incluye un `Makefile`
```bash
	make
	make run
```
o alternativamente: 
```bash
	gcc -o crucible main.c -lncurses
	./crucible
```
### Windows
[MinGW](https://www.msys2.org/)[PDcurses](https://pdcurses.org/)
El proceso de compilacion en windows es un tanto mas complejo y requiere de la instalacien de programas externos. Se debe de utilizar PDCurses para esto ya que ncursese no esta disponible enn windows, primero se debera realizar la debida instalacion de Msys2 y Mingw en el sistema, suigiendo las instrucciones del sitio [Instalacion de Msys2](https://www-msys2-org.translate.goog/?_x_tr_sl=en&_x_tr_tl=es). Despues de terminar la instalacion, inicie el programa **MSYS2 MINGW64** y ejecute los siguentes comandos:
```bash
	pacman -Syo
```
despues de que se cierre la ventana, vuelva a abrir el mismo programa y ejecute 
```bash
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-pdcurses
```
En las variables de entorno del sistema `PATH`se debera agregar la siguiente direccion : `C:\msys64\mingw64\bin`, use esta [Guia](https://www.softzone.es/windows/como-se-hace/cambiar-path-variables-entorno/) si es necesario.

Luego para la compilacion utilize 
```bash
gcc main.c -lpdcurses
```
