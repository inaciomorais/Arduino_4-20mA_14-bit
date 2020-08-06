/*

Resistor_250r_Transdutor_4-20mA-master.ino 

Interfaceamento de Transdutor 0(4)-20mA com Resistor 250r e Arduino.

Exemplo de aplicação de Resistor 250r 0.1% em saída de Transdutor 4-20mA (conversão de range para nível de tensão contínua
0(1)-5Vcc apropriado às entradas analógicas do Arduino ou outro sistema microcontrolado). 

O exemplo (Sketch) se aplica a Transdutores de Tensão 0-500Vca e Transdutores de Corrente 0-5Aca, mas pode ser adaptado para
outros tipos de transdutores. O exemplo se aplica a Transdutores com saída 4-20mA, mas pode ser adaptado para transdutores com 
saída 0-20mA.

Obs.: Montar o(s) resistor(es) o mais próximo possível da(s) entrada(s) analógica(s) do sistema microcontrolado.

aimgxp

*/

float ICal=1.000, VCal=1.000; /* Valores multiplicadores para calibração de leituras via firmware. Trabalhar dentro do range
                               * 0,700-1,300 (valores acima ou abaixo desse range indicam transdutor ou aferidor/padrão de 
                               * calibração muito descalibrado). Podem ser implementadas rotinas para calibração posterior in 
                               * loco com edição, salvamento e recuperação de valores em EEPROM. Obs.: Deve-se controlar os 
                               * momentos de leitura e escrita em EEPROM evitando-se ao máximo a possibilidade de desligamento 
                               * do sistema durante o acesso à mesma, pois o completo desligamento do sistema durante 
                               * processos de leitura ou escrita da EEPROM podem ocasionar total perda ou corrompimento dos 
                               * dados salvos. A criação de menus de calibração, leitura e escrita em EEPROM não são objeto 
                               * deste Sketch. Calibrações devem ser feitas com padrões calibrados (no exemplo, para 
                               * calibração de leituras de tensão e corrente altenada, o correto é a utilização de multímetro 
                               * True-RMS calibrado).
                               */
float Iatual, Vatual;
const byte CorrIn=A0, VIn=A1;

void setup()				
{
  pinMode(CorrIn, INPUT);
  pinMode(VIn, INPUT);

  Serial.begin(9600);  //  Inicia comunicação serial com 9600 bauds. Sketch exemplo apresenta comunicação ente Arduino e PC (Monitor Serial).
}

float Tens(byte pin) { // Função para Leitura de Tensão
  int adc0;
  float V_inst, Vact, V_sumf;
  unsigned long V_sum = 0;
  unsigned int i = 0;
  unsigned int samps = 200;   /* Variável que guarda a quantidade de amostras realizadas (amostragem). Oversampling para 
                               * melhora de precisão de ADCs 10 bits pela aplicação de preceitos estatísticos. Valores 
                               * maiores de amostragem garantem uma maior precisão, porém aumentam o tempo de processamento e 
                               * possibilidade de overflows.
                               */
  
  adc0 = analogRead(pin);
  delay(25);
  adc0 = analogRead(pin);     /* Leituras descartadas para eliminar valores parasitas (Ver datasheet ATmega e Arduino 
                               * Reference). "The ATmega datasheet [...] cautions against switching analog pins in close 
                               * temporal proximity to making A/D readings (analogRead) on other analog pins. This can cause 
                               * electrical noise and introduce jitter in the analog system." [Arduino Reference]
                               */

  for(i=0;i<samps;i++) {
    V_sum += analogRead(pin);
  }
    
  V_sumf = ((float)V_sum / (float)samps);
  
  V_inst = (((V_sumf*500.0)-102300.0)/818.4); // 20--4 1023--X | X=204.6 | 1023--var--204.6 500--X--0 | X/500=(var-204.6)/(1023-204.6) | X=((500*var)-(500*204.6))/818.4
                                              // Escalonamento acima referente a sistema microcontrolado com ADC 10 bits máx. 5Vcc e Transdutor 4-20mA 0-500Vca eficaz (RMS)
  Vact = V_inst * VCal;
  
  return Vact;
}

float Corr(byte pin) { // Função para Leitura de Corrente
  int adc0;
  float I_inst, Iact, I_sumf;
  unsigned int I_sum = 0, i = 0;
  unsigned int samps = 64;  /* Variável que guarda a quantidade de amostras realizadas (amostragem). Oversampling para 
                             * melhora de precisão de ADCs 10 bits pela aplicação de preceitos estatísticos. Valores maiores 
                             * de amostragem garantem uma maior precisão, porém aumentam o tempo de processamento e 
                             * possibilidade de overflows.
                             */
     
  adc0 = analogRead(pin);
  delay(25);
  adc0 = analogRead(pin);    /* Leituras descartadas para eliminar valores parasitas (Ver datasheet ATmega e Arduino 
                              * Reference). "The ATmega datasheet [...] cautions against switching analog pins in close 
                              * temporal proximity to making A/D readings (analogRead) on other analog pins. This can cause 
                              * electrical noise and introduce jitter in the analog system." [Arduino Reference]
                              */

  for(i=0;i<samps;i++) {
    I_sum += analogRead(pin);
  }
    
  I_sumf = ((float)I_sum / (float)samps);
  
  I_inst = (((I_sumf*5.0)-1023.0)/818.4);   // 20--4 1023--X | X=204.6 | 1023--var--204.6 5--X--0 | X/5=(var-204.6)/(1023-204.6) | X=((5*var)-(5*204.6))/818.4
                                            // Escalonamento acima referente a sistema microcontrolado com ADC 10 bits máx. 5Vcc e Transdutor 4-20mA 0-5Aca eficaz (RMS)
  Iact = I_inst * ICal;
  if (Iact < 0.00) Iact = 0.00;
  
  return Iact;
}

void loop()
{
    
  /* Aqui poderiam ser inseridos comandos para acionamento dos circuitos conectados ao transdutor (acionamento com relés, 
   * por exemplo), não esquecendo das devidas configurações de pinos em 'void setup(){}'.
   */
    
  /* Após acionamento dos circuitos conectados nas entradas de leitura do transdutor (cargas de interesse), aplicar o delay 
   * referente ao tempo de aquisição estabelecido no manual do transdutor. Exemplo:
   */

  delay(250);  /* Delay de 250 ms (para tempo de aquisição apresentado no manual do transdutor como '< 250 ms'). Atenção: 
                * Verifique o tempo de aquisição do transdutor aplicado. Aqui ainda deverá ser incluído (somado ao tempo de 
                * aquisição do transdutor) pequeno delay referente à comutação dos relés que acionam o circuito de interesse 
                * conectado ao transdutor, se aplicável (ver tempo de comutação na datasheet do relé e/ou contator aplicado).
                */
               
  Vatual = Tens(VIn);
  Iatual = Corr(CorrIn);                            
 
  Serial.print(Vatual, 2);  /* Envia o valor de Vatual através da porta serial com duas casas decimais. Para abrir o 
                             * Monitor Serial, clique em 'Serial Monitor' no menu 'Tools' da IDE Arduino ou pressione as 
                             * teclas Ctrl+Shift+M com a IDE Arduino aberta.
                             */				
  Serial.println(" Vca");
  
  Serial.print(Iatual, 3);  //  Envia o valor de Iatual através da porta serial com três casas decimais.
  Serial.println(" Aca");     
  
  delay(500); /* Inclui delay limitando quantidade de dados enviados através da porta serial. Delay de 500 ms. Esse
               * delay não é necessário em uma aplicação real, onde a leitura do valor enviado pelo transdutor ocorre em 
               * momentos específicos (aqui a leitura ocorre de forma cíclica para demonstração com auxílio do 
               * Monitor Serial).
               */ 
               
              /* Poderiam ser inseridos aqui comandos para desacionamento dos circuitos conectados às entradas de leitura 
               * do transdutor e demais processamentos. Lembrando ainda que o desacionamento de cargas pode gerar ruídos que 
               * interferem no correto funcionamento do sistema microcontrolado e principalmente em processos de comunicação 
               * associados. Assim, aplicar relés (Módulos de Relés ou Relés Modulares de Interface) com optoisolamento e/ou 
               * supressores de ruído/surtos. Recomenda-se fortemente (semprre) a aplicação de supressores de ruído em 
               * contatores ou minicontatores. Também avaliar aplicação de um delay após desacionamento de cargas indutivas 
               * consideráveis ou (mini)contatores para garantir que o ruído gerado não interfira em processos críticos como 
               * comunicação com Display LCD.
               */
}
