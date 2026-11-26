#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

// Definição de Pinos baseada na imagem do circuito do Wokwi
#define RST_PIN 9
#define SS_PIN 10
#define LED_VERDE_PIN 7
#define LED_VERMELHO_PIN 6
#define SERVO_PIN 3

MFRC522 mfrc522(SS_PIN, RST_PIN); // Instância do leitor RFID
Servo meuServo;                    // Instância do Servo Motor

// Máquina de Estados de Operação
enum Estados {
  ESTADO_AGUARDANDO,
  ESTADO_AUTORIZADO,
  ESTADO_NEGADO,
  ESTADO_BLOQUEADO // Diferencial: Bloqueio temporário por segurança após tentativas falhas
};

Estados estadoAtual = ESTADO_AGUARDANDO;

// Variáveis de Temporização (Sem uso de delay bloqueante)
unsigned long tempoAnterior = 0;
const unsigned long tempoAberto = 3000;   // Porta aberta por 3 segundos
const unsigned long tempoNegado = 2000;   // Alerta negado por 2 segundos
const unsigned long tempoBloqueio = 5000; // Bloqueio de segurança por 5 segundos

// Controle de Tentativas e Tratamento de Sinais
int tentativasInvalidas = 0;
const int maxTentativas = 3;

// Definição do UID do Cartão/Chaveiro Autorizado
// (Ajuste conforme o cartão que você escolher no Wokwi, ex: Cartão Verde)
byte tagAutorizada[4] = { 0x11, 0x22, 0x33, 0x44 }; 

void setup() {
  Serial.begin(9600);
  SPI.begin();          // Inicializa barramento SPI
  mfrc522.PCD_Init();   // Inicializa leitor MFRC522

  pinMode(LED_VERDE_PIN, OUTPUT);
  pinMode(LED_VERMELHO_PIN, OUTPUT);

  meuServo.attach(SERVO_PIN);
  meuServo.write(0); // Posição inicial: Tranca fechada (0 graus)

  digitalWrite(LED_VERDE_PIN, LOW);
  digitalWrite(LED_VERMELHO_PIN, LOW);

  Serial.println("=========================================");
  Serial.println(" Sistema de Controle de Acesso Iniciado ");
  Serial.println(" Aproximar cartao ou chaveiro...");
  Serial.println("=========================================");
}

void loop() {
  unsigned long tempoAtual = millis();

  // Tratamento de falha: Verifica se o leitor RFID está conectado corretamente
  byte versaoLeitor = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  if (versaoLeitor == 0x00 || versaoLeitor == 0xFF) {
    Serial.println("ERRO CRITICO: Leitor RFID desconectado ou com falha de comunicacao!");
    // Pisca o LED vermelho rapidamente para indicar pane no hardware
    digitalWrite(LED_VERMELHO_PIN, (tempoAtual / 250) % 2); 
    digitalWrite(LED_VERDE_PIN, LOW);
    return; // Interrompe o ciclo atual até que o hardware retorne
  }

  // Execução da Máquina de Estados Principal
  switch (estadoAtual) {
    
    case ESTADO_AGUARDANDO:
      digitalWrite(LED_VERDE_PIN, LOW);
      digitalWrite(LED_VERMELHO_PIN, LOW);

      // Verifica se há um novo cartão presente e realiza a leitura do UID com estabilidade
      if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        
        // Compara o UID lido com o UID autorizado cadastrado
        if (compararUID(mfrc522.uid.uidByte, tagAutorizada)) {
          Serial.println("[AUTORIZADO] Acesso liberado. Bem-vindo!");
          tentativasInvalidas = 0; // Reseta as tentativas falhas após sucesso
          meuServo.write(90);      // Abre o servo motor (90 graus)
          digitalWrite(LED_VERDE_PIN, HIGH);
          tempoAnterior = tempoAtual;
          estadoAtual = ESTADO_AUTORIZADO;
        } else {
          Serial.println("[NEGADO] Identificador nao cadastrado!");
          digitalWrite(LED_VERMELHO_PIN, HIGH);
          tentativasInvalidas++;
          tempoAnterior = tempoAtual;
          
          // Diferencial: Entra em modo de bloqueio se exceder o limite de erros
          if (tentativasInvalidas >= maxTentativas) {
            Serial.println("[SEGURANCA] Muitas falhas consecutivas! Sistema temporariamente bloqueado.");
            estadoAtual = ESTADO_BLOQUEADO;
          } else {
            estadoAtual = ESTADO_NEGADO;
          }
        }
        mfrc522.PICC_HaltA(); // Encerra a comunicação com o cartão atual para evitar leituras repetidas
      }
      break;

    case ESTADO_AUTORIZADO:
      // Mantém a porta aberta e o LED verde ligado sem travar o código (sem delay)
      if (tempoAtual - tempoAnterior >= tempoAberto) {
        meuServo.write(0); // Retorna o servo para a posição trancada (0 graus)
        digitalWrite(LED_VERDE_PIN, LOW);
        estadoAtual = ESTADO_AGUARDANDO;
        Serial.println("[INFO] Porta fechada. Aguardando proximo cartao...");
      }
      break;

    case ESTADO_NEGADO:
      // Mantém o LED vermelho aceso pelo tempo estipulado sem usar delay
      if (tempoAtual - tempoAnterior >= tempoNegado) {
        digitalWrite(LED_VERMELHO_PIN, LOW);
        estadoAtual = ESTADO_AGUARDANDO;
        Serial.println("[INFO] Aguardando proximo cartao...");
      }
      break;

    case ESTADO_BLOQUEADO:
      // Diferencial: Pisca o LED vermelho em alerta contínuo durante o bloqueio de segurança
      digitalWrite(LED_VERMELHO_PIN, (tempoAtual / 500) % 2);
      
      // Libera o bloqueio após o tempo estipulado
      if (tempoAtual - tempoAnterior >= tempoBloqueio) {
        digitalWrite(LED_VERMELHO_PIN, LOW);
        tentativasInvalidas = 0;
        estadoAtual = ESTADO_AGUARDANDO;
        Serial.println("[INFO] Bloqueio encerrado. Sistema normalizado.");
      }
      break;
  }
}

// Função auxiliar para comparar bytes do UID do cartão lido com o cadastrado
bool compararUID(byte *uidLido, byte *uidAutorizado) {
  for (byte i = 0; i < 4; i++) {
    if (uidLido[i] != uidAutorizado[i]) {
      return false;
    }
  }
  return true;
}