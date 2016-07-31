/* Variáveis portas */
byte pLedVermelho = 8; //Porta Led Vermelho
byte pLedVerde = 7; //Porta Led Verde
byte pLedAzul = 6; //Porta Led Azul
byte pSensor = 2; //Porta Sensor Simples
byte pBotao = 4; //Porta Botão Simples
byte pSirene = 5; //Porta Sirene
int UltSituacaoSensor = 0;
int qtUltSituacaoSensor = 0;
int SituacaoAtualSensor = 0;
int UltSituacaoBotao = 0;
int qtUltSituacaoBotao = 0;
int SituacaoAtualBotao = 0;

int margemPressionamento = 10000;

bool Armado = false;
bool Disparado = false;

unsigned long _millisAlarmeModoSilencioso = 0;
long _timerIntervaloModoSilencioso = 1000;
long _timerToqueModoSilencioso = 50;
bool SireneLigada = false;

void setup() {
  Serial.begin(9600);
  
  pinMode(pLedVerde, OUTPUT);
  pinMode(pLedAzul, OUTPUT);
  pinMode(pLedVermelho, OUTPUT);
  pinMode(pSirene, OUTPUT);
  pinMode(pSensor, INPUT_PULLUP);
  pinMode(pBotao, INPUT_PULLUP);

  digitalWrite(pLedVerde, HIGH); //INFORMA CIRCUITO LIGADO - HIGH Liga o Led, LOW desliga
  digitalWrite(pLedAzul, LOW); //INFORMA ALARME ARMADO - HIGH Liga o Led, LOW desliga
  digitalWrite(pLedVermelho, LOW); //INFORMA ALARME DISPARADO - HIGH Liga o Led, LOW desliga
  digitalWrite(pSirene, LOW); //DESLIGA A SIRENE - HIGH Desliga a sirene (Satura o transistor), LOW Liga a sirene

  qtUltSituacaoBotao = margemPressionamento;
  UltSituacaoBotao = 1;
  SituacaoAtualBotao = 1;
  
  Serial.println("Setup OK");
  ArmarAlarme();
}

void loop() {
  SituacaoAtualSensor = digitalRead(pSensor);
  
  if (UltSituacaoSensor != SituacaoAtualSensor)
  {
    if  (qtUltSituacaoSensor == margemPressionamento) {
      //Sensor foi acionado ou nao
      if (SituacaoAtualSensor == HIGH) //Acionado
      {
        SensorAcionado();  
      }
    }
    qtUltSituacaoSensor = 0;
    UltSituacaoSensor = SituacaoAtualSensor;
  } else if (UltSituacaoSensor == SituacaoAtualSensor && qtUltSituacaoSensor < margemPressionamento)
  {
    qtUltSituacaoSensor = qtUltSituacaoSensor + 1;
  }

  SituacaoAtualBotao = digitalRead(pBotao);
  
  if (UltSituacaoBotao != SituacaoAtualBotao)
  {
    if  (qtUltSituacaoBotao == margemPressionamento) {
      //Botão foi pressionado ou solto
      if (SituacaoAtualBotao == LOW) //Acionado
      {
        ArmarAlarme();  
      }
    }
    qtUltSituacaoBotao = 0;
    UltSituacaoBotao = SituacaoAtualBotao;
  } else if (UltSituacaoBotao == SituacaoAtualBotao && qtUltSituacaoBotao < margemPressionamento)
  {
    qtUltSituacaoBotao = qtUltSituacaoBotao + 1;
  }

  if (Disparado)
  {
    unsigned long _millisAtual = millis();
    if (_millisAtual >= _millisAlarmeModoSilencioso)
    {
      if (SireneLigada)
      {
        //Desliga a sirene
        digitalWrite(pSirene, LOW);
        SireneLigada = false;

        //Prepara próxima entrada
        _millisAlarmeModoSilencioso = _millisAtual + _timerIntervaloModoSilencioso; 
        Serial.println("-- SILENCIO -- ");
      } else 
      {
        //Liga a sirene
        digitalWrite(pSirene, HIGH);
        SireneLigada = true;
        
        //Prepara próxima entrada
        _millisAlarmeModoSilencioso = _millisAtual + _timerToqueModoSilencioso; 
        Serial.println("SIRENE!");
      }
    }
  }
  
}

void ArmarAlarme()
{
  if (!Armado)
  {
    if (!Disparado)
    {
      Armado = true;
      Disparado = false;
      digitalWrite(pLedAzul, HIGH);
      Serial.println("ARMADO");

      //Reseta situação do sensor
      qtUltSituacaoSensor = margemPressionamento;
      UltSituacaoSensor = 0;
      SituacaoAtualSensor = 0;
      
    } else
    {
      Disparado = false;
      //Desliga a sirene
      digitalWrite(pSirene, LOW);
      SireneLigada = false;
      Serial.println("ALARME DESLIGADO");
    }
  } else 
  {
    Armado = false;
    Disparado = false;
    SireneLigada = false;
    digitalWrite(pLedAzul, LOW);
     Serial.println("DESARMADO");
  }
  
   digitalWrite(pLedVermelho, LOW);
}

void SensorAcionado()
{
  if (Armado)
  {
    Serial.println("ALARME!");
    digitalWrite(pLedVermelho, HIGH);
    digitalWrite(pLedAzul, LOW);
    Armado = false;
    Disparado = true;

    //Liga a sirene
    digitalWrite(pSirene, HIGH);
    SireneLigada = true;
        
    //Prepara pausa da sirene
    unsigned long _millisAtual = millis();
    _millisAlarmeModoSilencioso = _millisAtual + _timerToqueModoSilencioso; 
    Serial.println("SIRENE!");
  }
}

