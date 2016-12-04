#include <SoftwareSerial.h#include <XBee.h>
 SoftwareSerial mySerial(4, 10); // 4-RX, 10-TX


 //  NÃO IRÁ COPILAR POIS NÓS NÃO TEMOS O XBEE FISICO



#define START_MSG '*'
#define END_MSG '#'
#define DIV_MSG '|'

#define MSG_DIGITALWRITE 10
#define MSG_ANALOGWRITE 11
#define RSP_COMANDO 12

#define MSG_DIGITALREAD 13
#define MSG_ANALOGREAD 14
#define RSP_LEITURA 15

#define MSG_STATUS 16
#define RSP_STATUS 17

XBee xbee = XBee();

uint8_t payload[] = { 0, 0 ,0 ,0 };

// SH + SL dos dispositivos XBee
XBeeAddress64 XBee1 = XBeeAddress64(0x0013a200, 0x40b97363);
XBeeAddress64 XBee2 = XBeeAddress64(0x0013a200, 0x40b9700a);
XBeeAddress64 XBee3 = XBeeAddress64(0x00000000, 0x0000ffff);// Broadcast
XBeeAddress64 addr64;

ZBTxRequest zbTx;
ZBTxStatusResponse txStatus = ZBTxStatusResponse();
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

  int statusLed = 13;
  int errorLed = 13;  
  int disp_xbee;  
  int APP_command = 0;
  int pin_num = 0;
  int pin_value = 0;  
  char get_char = ' ';  //read serial

void setup() {  

  Serial.flush();//Esvasia o buffer de entrada da porta serial. Isto é, qualquer chamada à Serial.read() ou Serial.avaiable somente retornarão dados recebidos após a última chamada à Serial.flush().
  mySerial.begin(9600);
  mySerial.flush();
  pinMode(statusLed, OUTPUT);
  pinMode(errorLed, OUTPUT);
  Serial.begin(9600);
  xbee.setSerial(Serial);
  
}
    
void loop(){  
  
    if (mySerial.available() > 0){      
        
        get_char = mySerial.read();
        if (get_char != START_MSG) return;       
       
        APP_command = mySerial.parseInt(); // read the command        
        disp_xbee = mySerial.parseInt(); // read the End Device XBee          
        pin_num = mySerial.parseInt(); // read the pin
        pin_value = mySerial.parseInt();  // read the value
      
 
        if (APP_command == MSG_DIGITALWRITE || APP_command == MSG_ANALOGWRITE){
        
                payload[0] = APP_command; 
                payload[1] = disp_xbee;    
                payload[2] = pin_num;
                payload[3] = pin_value;
                
                verificaIdXBee(disp_xbee);
                
                zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
                xbee.send(zbTx);    //flashLed(statusLed, 1, 100);
                
                statusResposta();
                
                xbee.readPacket(300);
                
                if (xbee.getResponse().isAvailable()) {     
                  
                        if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {                           
                              xbee.getResponse().getZBRxResponse(rx);
                              
                              if( (rx.getData()[0]) == RSP_COMANDO ){                                  
                                  mySerial.print(RSP_COMANDO);
                                  mySerial.print(RSP_COMANDO); 
                                  Serial.println(RSP_COMANDO);
                                  Serial.println(RSP_COMANDO);                                    
                              } 
                          }  
                 }
        }      
     
        if (APP_command == MSG_DIGITALREAD) {           
                
                payload[0] = APP_command;                 
                payload[1] = disp_xbee;
                payload[2] = pin_num;
                payload[3] = 0;
                
                verificaIdXBee(disp_xbee);
                
                zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
                xbee.send(zbTx);   // flashLed(statusLed, 1, 100);
                
                statusResposta();
                
                xbee.readPacket(300);
                
                if (xbee.getResponse().isAvailable()) {
                        // got something      
                        if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
                              // got a zb rx packet        
                              // now fill our zb rx class
                              xbee.getResponse().getZBRxResponse(rx);                        
                           
                              if( (rx.getData()[0]) == RSP_LEITURA ){                                              

                                             int valor = rx.getData()[2];  //Extrai o valor solicitado do payload e envia via bluetooth para o APP (2 bytes)                                      
                                             mySerial.print(RSP_LEITURA);                                                    
                                             if (valor>99) valor=99;
                                             if (valor<10){
                                                  mySerial.print("0");
                                             }                                           
                                             mySerial.print(valor);
                                             Serial.println(RSP_LEITURA);
                                             if (valor<10){
                                                  Serial.print("0");
                                             } 
                                             Serial.println(valor);                                                   
                              } 
                          }  
                 }                 
        }        
        if (APP_command == MSG_STATUS ) {           
                
                payload[0] = APP_command;                 
                payload[1] = disp_xbee;
                payload[2] = pin_num;
                payload[3] = 0;
                
                verificaIdXBee(disp_xbee);
                
                zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
                xbee.send(zbTx);   //Envia mensagem para destinatário XBee
                //flashLed(statusLed, 1, 100);//Sinalização
                
                statusResposta();// Recebe o primeiro pacote de resposta indicando a recepção da msg
                
                xbee.readPacket(300); // Recebe o segundo pacote contendo a informação solicitada no payload
                
                if (xbee.getResponse().isAvailable()) {
                        // got something      
                        if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
                              // got a zb rx packet        
                              // now fill our zb rx class
                              xbee.getResponse().getZBRxResponse(rx);                        
                           
                              if( (rx.getData()[0]) == RSP_STATUS ){                                              

                                             int sensor = rx.getData()[2];
                                             mySerial.print(RSP_STATUS);                                        
                                             if (sensor>99)sensor=99;
                                             if (sensor<10){
                                                 mySerial.print("0");                                                
                                             }                                           
                                             mySerial.print(sensor);
                                             Serial.println(RSP_STATUS);
                                             if (sensor<10){
                                                 Serial.print("0");
                                             } 
                                             Serial.println(sensor); 
                              } 
                          }  
                 }                 
        }              
    } 
}

void flashLed(int pin, int times, int wait) {

  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(wait);
    digitalWrite(pin, LOW);

    if (i + 1 < times) {
      delay(wait);
    }
  }
}
void verificaIdXBee(int xb){
         if(xb == 1){
             addr64 = XBee1;
         }else if(xb == 2){
             addr64 = XBee2;
         }else if(xb == 3){
             addr64 = XBee3;
         } 
}

void statusResposta(){ 
  // after sending a tx request, we expect a status response
  // wait up to half second for the status response
  if (xbee.readPacket(300)) {
    // got a response!

    // should be a znet tx status            	
    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
      xbee.getResponse().getZBTxStatusResponse(txStatus);

      // get the delivery status, the fifth byte
      if (txStatus.getDeliveryStatus() == SUCCESS) {
        // success.  time to celebrate
        flashLed(statusLed, 1, 50);
      } else {
        // the remote XBee did not receive our packet. is it powered on?
        flashLed(errorLed, 3, 500);
      }
    }
  } else if (xbee.getResponse().isError()) {
    //nss.print("Error reading packet.  Error code: ");  
    //nss.println(xbee.getResponse().getErrorCode());
  } else {
    // local XBee did not provide a timely TX Status Response -- should not happen
    flashLed(errorLed, 2, 50);
  }
  
}
