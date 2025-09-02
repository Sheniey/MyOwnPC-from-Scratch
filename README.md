
> H
> 
> O


---

---

## Descripción:
#### ¿Por qué lo hice?
Porque quise poner a prueba mis habilidades de programación y diseño de sistemas computacionales al crear un proyecto que asombrara a mis amigos y familiares de mis capasidades.

#### ¿Por qué este proyecto?
**"De una Necesidad Nace el Software..." ~ Anonymous**
La idea resulta en llevar a la realidad mi sueño de crear mi propia PC desde cero para enseñar a los de mi entorno cómo funciona una computadora al puro estilo de [Ben Eater](https://www.youtube.com/@BenEater "Canal de YouTube: Ben Eater") pero la diferencia entre el mío y el que hizo Ben Eater radica en que el mío está más familiarizado con las personas que no son tecnicas en el campus, es decir, trate de crear metodos visuales en la PC para digerir más facil su funcionamiento, incluyendo una memoria RAM casera, LEDs por doquier, entre otras cosas. Además, me gustaria explicar a mis familiares y amigos como lo hace [Code Dumped](https://www.youtube.com/@CoreDumpped "Canal de YouTube: Code Dumped") puesto que ese es el principal objetivo, enseñar el funcionamiento de una PC.

#### ¿Cómo lo hice?
El objetivo es replicar lo más posible una PC, asi que voy a poner el componente de las computadoras y su equivalente al mío.
- PCIe          -> Protoboard (por mi lo hubiera hecho en KidCad cada componente, pero es más facil de entender un Protoboard)
- CPU           -> Arduino UNO R4 WiFi
- GPU           -> Raspberry Pi Pico 2
- RAM           -> Memoria Flash MOSFET Casero
- Power Source  -> **{{{{{undefined}}}}}**
- I/O Devices   -> USB/VGA a conexion serial UART
- Monitor       -> Monitor
- Buses         -> Lineas de Protoboard Principal
- Motherboard   -> Conjunto de Protoboards
- BIOS          -> EEPROM en la Protoboard Principal

---

## Apoyos:
> En está seccion no se habla sobre los apoyos que tuve, sino de como podria "yo" apoyar el proyecto dependiendo de mis conocimientos.

#### GoodGuy:
Como un buena persona, simplemente seguir los pasos y contruir mi PC casera en la vida real; de verdad que es una grata sensación ese tipo de apoyos.

#### Ingeniero en Sistemas Computacionales:
Creo que como buen ing. sabras mejorar algunas partes del codigo de Arduino de la CPU y GPU añadiendo soporte y rapides pero manteniendo la idea principal; asi mismo algunos errores y/o problemas. si deseas ayudarme... entonces, no se diga más y sumerjete en "nuestro" codigo. Ey, ey... pero antes de eso visita la sección [Developer](#developer "https://github.com/.../README.md#developer"), el cual es una guia del codigo y unas reglas que me gustarias que siguieras.

#### Ingeniero en Software:
Si tu sabiduria te permite... me podrias ayudar mejorando el compilador que hice en Python de Assembly a Hexadecimal (AMF Ryzen 7U) haciendolo más rapido y añadiendo algunos features al lenguaje Assembly. Pero si eres un senior experimentado, entonces seria un enorme favor que me hicieses un compilador de Assembly en Rust o C/C++ para mayor velocidad ó si quieres elevar la dificultad, un compilador (compilado para que se vea el codigo maquina traspasado, aunque es solo un observación) de Python al codigo maquina pasando por Assembly; lo sé, suena anticlimatico el hecho que Python sea compilado en ves de interpretado.

#### Ingeniero DevOps:
Esta en realidad va para todas las ingenierias que tengan que ver con programar, pero me concentro en esta ing. principalmente porque creo yo hacen más fetching. No es como tal un metodo de apoyo pero tiene que ver... por mi un gusto que hagas fetch de mi proyecto con tal de que me des credito y no modifiques el objetivo de este mismo.

---

## Componentes:

---

## Proceso:

---

## Agradecimientos:
Me gustaria dar algunos agradecimientos/bibliografia de donde saque la información y/o cómo aprendí cómo funciona una PC por dentro. Aquí dejo el nombre de la persona y su/s recurso/s que utilizé.

YouTube Canal: [Ben Eater](https://www.youtube.com/@BenEater "Canal de YouTube: Ben Eater")

---

## Componentes:
- Arduino UNO R4 WiFi
- Raspberry Pi Pico 2
- Arduino Nano

---

🔹 1. Registros Generales (General-Purpose Registers - GPRs)
Registro	Familia	Tamaño	Accesible	Propósito / Comentario
AL	A	8 bit	✅	Parte baja de AX (acumulador)
AH	A	8 bit	✅	Parte alta de AX
AX	A	16 bit	✅	Acumulador (histórico)
EAX	A	32 bit	✅	Acumulador extendido
RAX	A	64 bit	✅	Acumulador en x86-64
BL/BH/BX/EBX/RBX	B	8–64 bit	✅	Registro base
CL/CH/CX/ECX/RCX	C	8–64 bit	✅	Contador (loops, shifts)
DL/DH/DX/EDX/RDX	D	8–64 bit	✅	Datos, I/O, multiplicación
SI/ESI/RSI	SI	16–64 bit	✅	Índice fuente
DI/EDI/RDI	DI	16–64 bit	✅	Índice destino
SP/ESP/RSP	SP	16–64 bit	✅	Stack pointer (pila)
BP/EBP/RBP	BP	16–64 bit	✅	Base pointer (frames de pila)
R8–R15	-	64 bit	✅	Registros adicionales en x86-64

🔹 2. Registros de Segmento (Segment Registers)
Registro	Tamaño	Accesible	Propósito / Comentario
CS	16 bit	🔶 (con jmp far, retf, etc.)	Código: contiene el CPL
DS	16 bit	🔶 (no directamente modificable en 64b)	Datos generales
SS	16 bit	🔶	Segmento de pila
ES	16 bit	🔶	Extra (copias de memoria)
FS	16 bit	🔶	Datos especiales (TLS en Linux/Win)
GS	16 bit	🔶	Igual que FS

🔶 Accesibles a través de instrucciones específicas (mov DS, AX, no con add)

🔹 3. Registros de Control (CR0–CR4, CR8)
Registro	Tamaño	Accesible	Propósito
CR0	32/64	⚠️ (solo en anillo 0)	Activa modo protegido, paging, etc.
CR2	32/64	⚠️	Dirección de la última page fault
CR3	32/64	⚠️	Dirección base de la tabla de páginas (🔑 ver más abajo)
CR4	32/64	⚠️	Extensiones: PAE, SSE, etc.
CR8	64	⚠️ (solo x86-64)	Prioridad de interrupciones

⚠️ Accesibles solo desde modo kernel (CPL = 0), usando instrucciones como mov CR3, RAX

🔹 4. Registros de Estado / Puntero de Instrucción
Registro	Tamaño	Accesible	Propósito
EIP / RIP	32/64	❌ (solo lectura con hacks)	Instruction pointer
EFLAGS / RFLAGS	32/64	⚠️ parcialmente	Flags de control (ZF, SF, IF, IOPL...)

⚠️ Algunas flags pueden modificarse directamente, otras no (como el flag de overflow OF).

🔹 5. Registros del Sistema
Registro	Tamaño	Accesible	Propósito
GDTR	48 bit	⚠️ (LGDT/SGDT)	Dirección base y límite de la GDT
IDTR	48 bit	⚠️ (LIDT/SIDT)	Dirección de la tabla de interrupciones
LDTR	16 bit	⚠️	Selector de la LDT (Local Descriptor Table)
TR	16 bit	⚠️	Selector de la TSS (Task State Segment)

🔹 6. Registros SIMD / Flotantes / Extensiones
Registro	Tamaño	Accesible	Propósito
MMX (MM0–MM7)	64 bit	✅	Multimedia (obsoleto)
XMM0–XMM31	128 bit	✅	SIMD/SSE (enteros y floats)
YMM0–YMM31	256 bit	✅	AVX (extensión SIMD)
ZMM0–ZMM31	512 bit	✅	AVX-512

