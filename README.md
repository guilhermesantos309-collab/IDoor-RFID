# Sistema de Controle de Acesso utilizando RFID e Arduino Uno

## 1. Introdução

O presente projeto consiste no desenvolvimento de um sistema automatizado de controle de acesso, utilizando a tecnologia de identificação por radiofrequência (RFID), integrada a uma plataforma de prototipagem eletrônica Arduino Uno.

A proposta consiste em desenvolver uma solução capaz de identificar cartões ou chaveiros RFID, realizar a validação de seus respectivos códigos de identificação (UID) e determinar se o acesso a determinado ambiente deve ser permitido ou negado.

A partir da identificação do cartão, o microcontrolador executa as operações correspondentes, controlando um servo motor responsável pelo acionamento da trava, além de utilizar indicadores luminosos e sonoros para comunicar o resultado da autenticação.

O sistema também incorpora mecanismos de controle de tentativas inválidas, bloqueio temporário e verificação de comunicação com o leitor, buscando demonstrar a aplicação de conceitos de automação, programação embarcada e segurança eletrônica.

## 2. Objetivos

### 2.1 Objetivo geral

Desenvolver um protótipo de controle de acesso eletrônico utilizando Arduino Uno e tecnologia RFID, capaz de identificar usuários por meio de cartões previamente cadastrados e controlar automaticamente a abertura e o fechamento de uma trava.

### 2.2 Objetivos específicos

- Compreender o funcionamento da tecnologia RFID e sua aplicação na identificação eletrônica.
- Implementar a comunicação entre o Arduino Uno e o módulo leitor RFID RC522.
- Desenvolver um mecanismo de identificação e comparação de códigos UID.
- Permitir o cadastramento de múltiplos cartões autorizados.
- Controlar um servo motor para simular o acionamento de uma fechadura.
- Implementar sinalização visual e sonora para indicar os resultados das tentativas de acesso.
- Desenvolver uma máquina de estados para organizar as etapas de funcionamento do sistema.
- Aplicar temporização não bloqueante utilizando a função `millis()`.
- Implementar um mecanismo de bloqueio temporário após tentativas consecutivas inválidas.
- Identificar possíveis falhas de comunicação com o módulo RFID.

## 3. Fundamentação teórica

### 3.1 Tecnologia RFID

RFID (Radio Frequency Identification), ou identificação por radiofrequência, é uma tecnologia utilizada para identificar objetos por meio da comunicação entre dispositivos que utilizam ondas de rádio.

No contexto deste projeto, o sistema é composto por um leitor RFID RC522 e cartões ou chaveiros compatíveis.

Quando um cartão é aproximado do leitor, ocorre uma comunicação que permite a obtenção de seu identificador eletrônico, denominado UID (Unique Identifier).

Esse identificador é utilizado pelo programa para verificar se o cartão pertence à relação de identificadores previamente cadastrados.

### 3.2 Microcontrolador Arduino Uno

O Arduino Uno é a plataforma responsável pelo processamento das informações recebidas pelo leitor RFID e pelo controle dos demais componentes eletrônicos.

Seu microcontrolador executa o programa desenvolvido em C++, realizando a comparação dos identificadores e determinando quais ações devem ser executadas de acordo com o resultado da validação.

### 3.3 Comunicação SPI

A comunicação entre o Arduino Uno e o módulo RC522 ocorre por meio do protocolo SPI (Serial Peripheral Interface).

Esse protocolo permite a troca de informações entre dispositivos eletrônicos utilizando linhas específicas para transmissão, recepção, sincronização e seleção do dispositivo.

No projeto, são utilizados os pinos digitais D10, D11, D12 e D13, além do pino D9 para reset do leitor.

### 3.4 Servo motor

O servo motor é o componente responsável por representar o mecanismo físico de abertura e fechamento da trava.

O posicionamento é realizado por meio de comandos enviados pelo Arduino:

- 0°: posição correspondente à trava fechada.
- 90°: posição correspondente à trava liberada.

O retorno à posição inicial ocorre automaticamente após o intervalo de autorização.

### 3.5 Máquina de estados

Uma máquina de estados é um modelo de organização lógica utilizado para representar diferentes condições de funcionamento de um sistema.

Neste projeto, ela permite separar as operações de espera, autorização, negação e bloqueio.

Essa organização facilita a compreensão do programa, evita a mistura de responsabilidades e permite controlar as transições entre as diferentes etapas de funcionamento.

## 4. Materiais e componentes utilizados

| Componente | Quantidade | Finalidade |
|---|---:|---|
| Arduino Uno | 1 | Processamento e controle |
| Leitor RFID RC522 | 1 | Identificação dos cartões |
| Servo motor | 1 | Acionamento da trava |
| LED verde | 1 | Indicação de acesso autorizado |
| LED vermelho | 1 | Indicação de acesso negado |
| Resistores de 220 Ω | 2 | Limitação da corrente dos LEDs |
| Buzzer | 1 | Sinalização sonora |
| Display LCD I2C | 1 | Exibição de informações |
| Cartões RFID | Variável | Identificação dos usuários |
| Jumpers | Variável | Conexões elétricas |

**Observação:** os componentes apresentados correspondem ao conjunto previsto no esquema elétrico. A utilização efetiva do LCD e do buzzer depende da implementação de suas respectivas rotinas no código.

## 5. Arquitetura e conexões elétricas

A arquitetura do projeto é centralizada no Arduino Uno, que recebe os dados provenientes do leitor RFID e controla os dispositivos de saída.

O leitor RC522 realiza a identificação dos cartões por meio da comunicação SPI. Após a leitura, o microcontrolador processa o UID e executa as ações correspondentes.

### 5.1 Tabela de conexões

| Pino do Arduino | Componente | Função |
|---|---|---|
| D3 | Servo motor | Controle de posição |
| D4 | Buzzer | Sinalização sonora |
| D6 | LED vermelho | Indicação de acesso negado |
| D7 | LED verde | Indicação de acesso autorizado |
| D9 | RC522 (RST) | Reset do leitor |
| D10 | RC522 (SDA/SS) | Seleção do dispositivo SPI |
| D11 | RC522 (MOSI) | Transmissão de dados |
| D12 | RC522 (MISO) | Recepção de dados |
| D13 | RC522 (SCK) | Sincronização da comunicação |
| A4 | LCD I2C (SDA) | Comunicação de dados |
| A5 | LCD I2C (SCL) | Sincronização do display |
| 3,3 V | RC522 | Alimentação do leitor |
| 5 V | Servo e LCD | Alimentação |
| GND | Todos os componentes | Referência elétrica comum |

**Importante:** o módulo RC522 deve ser alimentado com 3,3 V. Também é necessário observar os limites elétricos das entradas e saídas do módulo.

## 6. Desenvolvimento e lógica de funcionamento

O funcionamento do sistema foi estruturado em etapas sequenciais, utilizando uma máquina de estados para controlar as operações.

### 6.1 Inicialização dos componentes

Na inicialização, o programa configura os pinos utilizados, estabelece a comunicação serial, inicializa o protocolo SPI e prepara o módulo RFID.

O servo motor é posicionado inicialmente em 0°, mantendo a trava na condição fechada.

### 6.2 Identificação dos cartões

Quando um cartão é aproximado do leitor, o RC522 realiza a leitura de seu UID.

O identificador é composto por uma sequência de bytes, que são apresentados no Monitor Serial para facilitar a identificação e o cadastramento dos cartões.

O programa compara o UID recebido com os identificadores armazenados na lista de cartões autorizados.

Essa comparação considera tanto os valores quanto a quantidade de bytes do identificador.

### 6.3 Validação de acesso

Após a leitura, o sistema realiza uma verificação:

**Acesso autorizado:**

Quando o UID corresponde a um dos cartões cadastrados, o sistema:

1. Reinicia o contador de tentativas inválidas.
2. Aciona o LED verde.
3. Posiciona o servo motor em 90°.
4. Mantém a trava liberada durante três segundos.
5. Retorna o servo à posição de 0°.
6. Retorna ao estado de espera.

**Acesso negado:**

Quando o identificador não corresponde a nenhum cartão cadastrado, o sistema:

1. Aciona o LED vermelho.
2. Incrementa o contador de tentativas inválidas.
3. Mantém o alerta durante dois segundos.
4. Verifica se o limite de tentativas foi atingido.

### 6.4 Bloqueio temporário

Como mecanismo adicional de segurança, foi implementado um bloqueio temporário após três tentativas inválidas consecutivas.

Durante esse período, o LED vermelho permanece piscando e o sistema não realiza novas leituras de cartões.

Após cinco segundos, o contador é reiniciado e o sistema retorna ao funcionamento normal.

### 6.5 Verificação de falhas

O programa também verifica a resposta do leitor RFID por meio da leitura de seu registrador de versão.

Caso sejam identificados valores que indiquem ausência de comunicação ou falha de resposta, o sistema sinaliza a ocorrência por meio do LED vermelho.

## 7. Cadastro de cartões

Uma das funcionalidades implementadas é a possibilidade de definir manualmente os identificadores dos cartões autorizados.

O cadastramento é realizado diretamente no código-fonte, por meio de uma estrutura que armazena o UID e seu respectivo tamanho.

Exemplo de implementação:

```cpp
struct Cartao {
    byte uid[10];
    byte tamanho;
};

Cartao cartoesAutorizados[] = {

    {{0x11, 0x22, 0x33, 0x44}, 4},

    {{0xAA, 0xBB, 0xCC, 0xDD}, 4},

    {{0x12, 0x34, 0x56, 0x78}, 4}

};
```

Cada registro representa um cartão autorizado.

Para adicionar novos usuários, basta inserir outro identificador na lista, respeitando a quantidade de bytes correspondente ao UID do cartão.

A quantidade de cartões cadastrados é calculada automaticamente pelo programa, dispensando a atualização manual de um contador.

## 8. Organização do programa

O código foi dividido em estruturas e funções que facilitam sua manutenção e compreensão.

| Estrutura ou função | Responsabilidade |
|---|---|
| `setup()` | Inicialização dos componentes |
| `loop()` | Execução contínua do sistema |
| `compararUID()` | Verificação dos cartões cadastrados |
| `mostrarUID()` | Exibição do identificador do cartão |
| `Cartao` | Armazenamento dos dados dos cartões |
| `Estados` | Definição das condições de funcionamento |
| `millis()` | Controle dos intervalos de tempo |

Essa organização permite separar as operações de leitura, validação, controle de acesso e temporização.

## 9. Máquina de estados

O sistema possui quatro estados principais:

| Estado | Descrição |
|---|---|
| `ESTADO_AGUARDANDO` | Aguarda a aproximação de um cartão |
| `ESTADO_AUTORIZADO` | Executa a liberação temporária da trava |
| `ESTADO_NEGADO` | Sinaliza uma tentativa inválida |
| `ESTADO_BLOQUEADO` | Suspende temporariamente novas tentativas |

As transições entre os estados são determinadas pelos resultados da leitura RFID e pelo tempo decorrido em cada operação.

O uso desse modelo permite estruturar o funcionamento do programa sem depender de pausas bloqueantes.

## 10. Temporização e controle de execução

O projeto utiliza a função `millis()` para controlar os intervalos de tempo.

Diferentemente da função `delay()`, que interrompe a execução do programa durante determinado período, `millis()` permite verificar quanto tempo passou desde o início da execução sem interromper completamente o ciclo principal.

Os intervalos definidos são:

| Operação | Duração |
|---|---:|
| Liberação da trava | 3 segundos |
| Sinalização de acesso negado | 2 segundos |
| Bloqueio temporário | 5 segundos |
| Tentativas inválidas permitidas antes do bloqueio | 3 |

Essa abordagem permite uma execução mais organizada e facilita a integração de novas funcionalidades.

## 11. Simulação e testes

O projeto utiliza a plataforma Wokwi para a montagem e simulação do circuito eletrônico.

**Plataforma:** [Wokwi](https://wokwi.com/)

Os testes devem verificar os seguintes comportamentos:

| Teste | Procedimento | Resultado esperado |
|---|---|---|
| Inicialização | Iniciar a simulação | Componentes inicializados e trava fechada |
| Cartão autorizado | Aproximar um cartão cadastrado | LED verde aceso e servo em 90° |
| Fechamento automático | Aguardar três segundos | Servo retorna a 0° |
| Cartão não cadastrado | Aproximar um cartão desconhecido | LED vermelho aceso |
| Tentativas inválidas | Realizar três tentativas consecutivas | Bloqueio temporário |
| Fim do bloqueio | Aguardar cinco segundos | Retorno ao estado de espera |
| Leitura do UID | Aproximar um cartão | Identificador apresentado no Monitor Serial |

## 12. Bibliotecas utilizadas

As principais bibliotecas utilizadas na implementação são:

- **SPI.h:** responsável pela comunicação SPI entre o Arduino e o leitor RFID.
- **MFRC522.h:** disponibiliza as funções necessárias para inicializar o RC522 e realizar a leitura dos cartões.
- **Servo.h:** permite controlar a posição angular do servo motor.

A utilização dessas bibliotecas simplifica a comunicação com os componentes e permite concentrar o desenvolvimento na lógica de controle do sistema.

## 13. Resultados esperados

Com a implementação do projeto, espera-se obter um protótipo funcional capaz de realizar a identificação de cartões RFID, verificar sua autorização e controlar uma trava automatizada.

A utilização da máquina de estados permite organizar as diferentes condições de operação, enquanto a temporização por `millis()` possibilita controlar os intervalos de funcionamento sem utilizar pausas bloqueantes.

O sistema também demonstra a aplicação de mecanismos básicos de controle de tentativas e sinalização de falhas.

## 14. Limitações e possibilidades de melhoria

Apesar de atender aos objetivos de um protótipo acadêmico, o projeto apresenta possibilidades de expansão:

- Implementação do cadastro e remoção de cartões pelo Monitor Serial.
- Integração completa do display LCD I2C para apresentar mensagens ao usuário.
- Implementação das sinalizações sonoras utilizando o buzzer.
- Armazenamento dos cartões cadastrados em memória não volátil.
- Criação de um histórico de tentativas de acesso.
- Implementação de mecanismos adicionais de autenticação.
- Desenvolvimento de uma interface para gerenciamento dos usuários.

É importante destacar que a identificação baseada exclusivamente no UID não constitui um mecanismo de autenticação criptográfica. Portanto, o protótipo não deve ser considerado um sistema de segurança física de alta confiabilidade sem mecanismos adicionais de proteção.

## 15. Conclusão

O desenvolvimento deste projeto possibilita aplicar conhecimentos relacionados à programação de microcontroladores, comunicação entre dispositivos eletrônicos, identificação por radiofrequência e automação.

A integração entre o Arduino Uno, o leitor RFID RC522 e o servo motor demonstra como diferentes componentes podem trabalhar em conjunto para executar uma tarefa automatizada de controle de acesso.

Além disso, a implementação de uma máquina de estados, o controle de temporização e o tratamento de tentativas inválidas contribuem para uma estrutura de software mais organizada e compreensível.

Dessa forma, o projeto representa uma aplicação prática dos conceitos estudados em sistemas embarcados, permitindo compreender tanto a integração dos componentes físicos quanto a lógica de programação necessária para seu funcionamento.

---

## Informações do projeto

| Característica | Descrição |
|---|---|
| Nome | Sistema de Controle de Acesso RFID |
| Plataforma | Arduino Uno |
| Linguagem | C++ |
| Tecnologia de identificação | RFID |
| Módulo de leitura | MFRC522 RC522 |
| Comunicação | SPI |
| Simulador | Wokwi |
| Aplicação | Automação e controle de acesso |

**Documentação desenvolvida para fins acadêmicos e educacionais.**
