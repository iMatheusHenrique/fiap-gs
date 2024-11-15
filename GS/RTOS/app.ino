#include <TimerOne.h> 
#include <avr/wdt.h> 
#include <EEPROM.h>

const int led_verde = 6;
const int led_vermelho = 7;

const int contador_tempo_limite = 30; // 5 minutos (300 segundos)
const int eepromAddress = 0;  // endereco da EEPROM para o indicador de falha

volatile bool carregamento_esta_ativo = false;
volatile int contador = 0;


void piscarLedVerde() {
  if (carregamento_esta_ativo) {
    digitalWrite(led_verde, !digitalRead(led_verde));
  }
}

void iniciarWatchdog() {
  wdt_enable(WDTO_8S); // Configura o watchdog para resetar em 8 segundos
}

void verificarWatchdog() {
  wdt_reset();
}

void piscarLedVermelho() {
  for (int i = 0; i < 5; i++) { 
    digitalWrite(led_vermelho, HIGH);
    delay(200);
    digitalWrite(led_vermelho, LOW);
    delay(200);
  }
}

void iniciarCarregamento() {
  carregamento_esta_ativo = true;
  digitalWrite(led_verde, HIGH); 
  digitalWrite(led_vermelho, LOW); 
}

void contarSegundos() {
  contador++;
  Serial.print("Contador: ");
  Serial.println(contador);

  if (contador >= contador_tempo_limite) { 
    Serial.println("contador >= contador_tempo_limite");
    verificarWatchdog();
    contador = 0; 
    piscarLedVerde();
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(led_verde, OUTPUT);
  pinMode(led_vermelho, OUTPUT);
  digitalWrite(led_verde, LOW); 
  digitalWrite(led_vermelho, LOW);

  if (EEPROM.read(eepromAddress) == 1) {
    // indica que o sistema foi reiniciado inesperadamente
    Serial.println("Falha detectada");
    piscarLedVermelho();
  }

  EEPROM.write(eepromAddress, 1);

  iniciarWatchdog();
  iniciarCarregamento();

  Timer1.initialize(1000000);  // 1 segundo (1.000.000 microssegundos)
  Timer1.attachInterrupt(contarSegundos); // Associa a função de interrupção
}

void loop() {
  delay(1000);

  // Se o carregamento estiver inativo e o contador passar dos 5 minutos sem resetar, indica falha
  if (!carregamento_esta_ativo) {
    piscarLedVermelho();
    carregamento_esta_ativo = false;
  }
  EEPROM.write(eepromAddress, 0);

}
