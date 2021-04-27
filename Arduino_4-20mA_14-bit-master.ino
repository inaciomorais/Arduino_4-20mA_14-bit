/*

Arduino_4-20mA_14-bit-master.ino 

Interfaceamento de Transdutor 4-20mA com Resistor 250r e Arduino aplicando técnica de Oversampling e Decimation (14-bit).

Exemplo de aplicação de Resistor 250r 0.1% em saída de Transdutor 4-20mA (conversão de range para nível de tensão contínua
0(1)-5Vcc apropriado às entradas analógicas do Arduino ou outro sistema microcontrolado). 

Realiza leitura analógica 4-20mA e envia valor pela porta serial.

Obs.: Montar o(s) resistor(es) o mais próximo possível da(s) entrada(s) analógica(s) do sistema microcontrolado.

Resistor 250 0.1% Filme metálico 400mW:

https://produto.mercadolivre.com.br/MLB-1620033927-1-un-resistor-filme-metalico-250ohms-01-400mw-_JM?quantity=1

A. Inácio Morais - anderson.morais@protonmail.com - (35) 99161-9878 - 04/2021

*/

const byte iw00=A0;

void setup()				
{
  pinMode(iw00, INPUT);
  
  Serial.begin(9600);  //  Inicia comunicação serial com 9600 bauds. Sketch exemplo apresenta comunicação ente Arduino e PC (Monitor Serial).
}

float analog_14bit(byte pin) {  // Função para leituras em Entradas Analógicas (Oversampling e Decimation)
  int _adc;
  unsigned long _soma = 0;
  float _result;
        
  _adc = analogRead(pin);
  delay(25);
  _adc = analogRead(pin);  /* Leituras descartadas para eliminar valores parasitas (Ver datasheet ATmega e Arduino 
                            * Reference). "The ATmega datasheet [...] cautions against switching analog pins in close 
                            * temporal proximity to making A/D readings (analogRead) on other analog pins. This can cause 
                            * electrical noise and introduce jitter in the analog system." [Arduino Reference]
                            */

  for(unsigned int i=0;i<256;i++) _soma += analogRead(pin);  //Oversampling
    
  _result = ((float)_soma / 16.0);  // Decimation
  
  return _result;
}

void loop() {
  float _cal=1.000;  /* Valor multiplicador para calibração de leituras via firmware. Trabalhar dentro do range
                      * 0,800-1,200 (valores acima ou abaixo desse range indicam transdutor ou aferidor/padrão de 
                      * calibração muito descalibrado). Podem ser implementadas rotinas para calibração posterior in 
                      * loco com edição, salvamento e recuperação de valores em EEPROM. A criação de menus/rotinas 
                      * de calibração, leitura e escrita em EEPROM não são objeto deste Sketch. Calibrações devem 
                      * ser feitas com padrões calibrados.
                      */
  float _leitura;

  delay(250);  /* Principalmente para amostragens que não podem ocorrer indefinidamente (dependentes primeiramente da 
                * comutação de circuitos específicos e com posterior alimentação dos terminais de leitura do transdutor), 
                * aplica-se um delay referente ao tempo de aquisição estabelecido no manual do transdutor e ao tempo de 
                * comutação dos relés/contatores envolvidos. Ex.: Delay de 250 ms para tempo de aquisição apresentado 
                * no manual do transdutor como '< 200 ms' e pequeno delay considerando os tempos de comutação. Atenção: 
                * Sempre verifique o tempo de aquisição do transdutor aplicado.
                */

  _leitura = analog_14bit(iw00);

  // Máx. valor inteiro representado por 14 bits = 16383
  // NORMALIZAÇÃO (Considerando entrada 4-20mA):
  //                    20mA -- 16383 | 4mA -- x
  //                    x = 3276,6

  // ESCALONAMENTO (Considerando unidade/variável de processo como Porcentagem):
  //                    (_leitura - 3276,6) / (16383 - 3276,6) = (y - 0%) / (100% - 0%)
  //                    (_leitura - 3276,6) / 13106,4 = y / 100
  //                    y = ((_leitura - 3276,6) / 13106,4) * 100

  // É possível realizar multiplas chamadas da função 'analog_14bit' após o tempo de aquisição/comutação, salvando os
  // retornos em diferentes variáveis e efetuando média das leituras obtidas para obter-se uma menor variação. Mas, o   
  // aumento na precisão também significa um aumento nos tempos de processamento envolvidos.

  _leitura = (((_leitura - 3276.6) / 13106.4) * 100.0) * _cal;
            
  Serial.print(_leitura, 2);  /* Envia o valor de _leitura através da porta serial com duas casas decimais. Para abrir o 
                               * Monitor Serial, clique em 'Monitor serial' no menu 'Ferramentas' da IDE Arduino ou pressione as 
                               * teclas Ctrl+Shift+M com a IDE Arduino aberta. Usar módulo FTDI232.
                               */        
  Serial.println(" %");  
}
