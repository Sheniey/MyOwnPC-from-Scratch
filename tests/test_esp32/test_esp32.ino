
/*
GPIO	  Estado	Comentarios
4-15    [🟢]    Seguro	Pines estándar, sin funciones críticas en boot.
16	    [🟡]    Con cuidado	Puede afectar funciones especiales, evita si puedes.
17-18	  [🔴]    No recomendable	Reservados para USB (D+ y D- alternativos). No usarlos.
19-20	  [🔴]    No recomendable	USB nativo D+ y D-, no usar para I/O.
21-23	  [🟢]    Seguro	Pines estándar.
33-39	  [🟢]    Seguro	Pines estándar, sin problemas para I/O.
40-47	  [🟡]    Con cuidado	Algunos son solo entrada o conectados a PSRAM/Flash.
48	    [🟡]    Con cuidado	Pin LED RGB integrado, no usar si quieres el LED.
0	      [🔴]    No recomendable	Pin BOOT, afecta arranque.
1,3	    [🔴]    No recomendable	UART0 (TX/RX), usado para carga y debug.
45,46	  [🔴]    No recomendable	Pines strapping, afectan modo arranque.
*/

#define LED_BUILTIN 2

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // Configura el pin como salida
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);  // Apaga el LED
  delay(1000);
}