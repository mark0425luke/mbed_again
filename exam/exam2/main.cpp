//library
  //med
  #include "mbed.h"

  //accelero & machine learning
  #include "stm32l475e_iot01_accelero.h"
  #include "accelerometer_handler.h"
  #include "hahaconfig.h"
  #include "magic_wand_model_data.h"
  #include "tensorflow/lite/c/common.h"
  #include "tensorflow/lite/micro/kernels/micro_ops.h"
  #include "tensorflow/lite/micro/micro_error_reporter.h"
  #include "tensorflow/lite/micro/micro_interpreter.h"
  #include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
  #include "tensorflow/lite/schema/schema_generated.h"
  #include "tensorflow/lite/version.h"

  //uLCD
  #include "uLCD_4DGL.h"

  //rpc
  #include "mbed_rpc.h"

  //wifi & mqtt
  #include "MQTTNetwork.h"
  #include "MQTTmbed.h"
  #include "MQTTClient.h"

  //math
  #include <math.h>


//interrupt button
InterruptIn btnRecord(USER_BUTTON);


//serial
BufferedSerial pc(USBTX, USBRX);


//uLCD & led
uLCD_4DGL uLCD(D1, D0, D2);
//RpcDigitalOut myled1(LED1,"myled1");
//RpcDigitalOut myled2(LED2,"myled2");
//RpcDigitalOut myled3(LED3,"myled3");
DigitalOut myled3(LED3);
DigitalOut myled1(LED1);

//wifi
//WiFiInterface *wifi;
WiFiInterface *wifi = WiFiInterface::get_default_instance();
int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
NetworkInterface* net = wifi;
MQTTNetwork mqttNetwork(net);
MQTT::Client<MQTTNetwork, Countdown> client(mqttNetwork);

//some variable
volatile int message_num = 0;
volatile int arrivedcount = 0;
volatile bool closed = false;
const char* topic = "Mbed";
Thread mqtt_thread(osPriorityHigh);
EventQueue mqtt_queue;

//mine: variable
#define LABEL_NUM 3
int gesture_data;

#define MODE_CAPTURE 1
#define MODE_RETRIEVE 2
int mode;

int CaptureData[20][3];

#define GESTURE_0 0
#define GESTURE_1 1
#define GESTURE_2 2
float angleDetermine;



#define PI 3.14159
#define GRAVITY 991.0
float angle;

int angleOver;

double x, y;
Thread ThreadAngleSelect;
Thread ThreadAngleDetect;








// Create an area of memory to use for input, output, and intermediate arrays.
// The size of this will depend on the model you're using, and may need to be
// determined by experimentation.
constexpr int kTensorArenaSize = 60 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
// Return the result of the last prediction
int PredictGesture(float* output) {
  // How many times the most recent gesture has been matched in a row
  static int continuous_count = 0;
  // The result of the last prediction
  static int last_predict = -1;

  // Find whichever output has a probability > 0.8 (they sum to 1)
  int this_predict = -1;
  for (int i = 0; i < label_num; i++) {
    if (output[i] > 0.8) this_predict = i;
  }

  // No gesture was detected above the threshold
  if (this_predict == -1) {
    continuous_count = 0;
    last_predict = label_num;
    return label_num;
  }

  if (last_predict == this_predict) {
    continuous_count += 1;
  } else {
    continuous_count = 0;
  }
  last_predict = this_predict;

  // If we haven't yet had enough consecutive matches for this gesture,
  // report a negative result
  if (continuous_count < config.consecutiveInferenceThresholds[this_predict]) {
    return label_num;
  }
  // Otherwise, we've seen a positive result, so clear all our variables
  // and report it
  continuous_count = 0;
  last_predict = -1;

  return this_predict;
}

void messageArrived(MQTT::MessageData& md) {
    MQTT::Message &message = md.message;
    char msg[300];
    sprintf(msg, "Message arrived: QoS%d, retained %d, dup %d, packetID %d\r\n", message.qos, message.retained, message.dup, message.id);
    printf(msg);
    ThisThread::sleep_for(1000ms);
    char payload[300];
    sprintf(payload, "Payload %.*s\r\n", message.payloadlen, (char*)message.payload);
    printf(payload);
    ++arrivedcount;
}

void publish_message(MQTT::Client<MQTTNetwork, Countdown>* client) {
    //printf("enter publish message\n");//.......................test

    //mine: publish message
    if( (mode==MODE_GESTURE) && (gesture_data!=3)  ){ //make sure already select gesture
      //mine: angle for later tilt mode
      angleDetermine= (gesture_data==0)?ANGLE_0: 
                      (gesture_data==1)?ANGLE_1:
                      (gesture_data==2)?ANGLE_2:
                      90;

      //angle           
      message_num++;
      MQTT::Message message;
      char buff[100];
      if(gesture_data==0){
        sprintf(buff, "angle selected is %d", ANGLE_0);
      }
      else if(gesture_data==1){
        sprintf(buff, "angle selected is %d", ANGLE_1);
      }
      else if(gesture_data==2){
        sprintf(buff, "angle selected is %d", ANGLE_2);
      }
      else{
        sprintf(buff, "angle not selected");
      }
      
      message.qos = MQTT::QOS0;
      message.retained = false;
      message.dup = false;
      message.payload = (void*) buff;
      message.payloadlen = strlen(buff) + 1;
      int rc = client->publish(topic, message);

      //printf("rc:  %d\r\n", rc);
      //printf("Puslish message: %s\r\n", buff);
    }
    else if( (mode==MODE_TILT) && (tiltMode==TILT_DETECT) ){
      message_num++;
      MQTT::Message message;
      char buff[100];
      sprintf(buff, "%d th angle over", angleOver);
      message.qos = MQTT::QOS0;
      message.retained = false;
      message.dup = false;
      message.payload = (void*) buff;
      message.payloadlen = strlen(buff) + 1;
      int rc = client->publish(topic, message);
    }
    else{}
}

void close_mqtt() {
    closed = true;
}



int idR[32] = {0};
int indexR = 0;
void record(void) {
   BSP_ACCELERO_AccGetXYZ(pDataXYZ);
   printf("%d, %d, %d\n", pDataXYZ[0], pDataXYZ[1], pDataXYZ[2]);
}
void startRecord(void) {
   printf("---start---\n");
   idR[indexR++] = queue.call_every(1ms, record);
   indexR = indexR % 32;
}

void stopRecord(void) {
   printf("---stop---\n");
   for (auto &i : idR)
      queue.cancel(i);
}




//mine: uLCD & LED
class Menu{
    public:
        Menu();
        void show();
};
Menu::Menu(){
}
void Menu::show(){
  if(mode == MODE_GESTURE){
      
      //mode and angle selection
      uLCD.locate(4,4);
      uLCD.printf("MODE_GEST");
      uLCD.locate(4,6);
      uLCD.printf("             ");
      uLCD.locate(4,8);
      if(gesture_data==0){
        uLCD.printf("angle %d ",ANGLE_0);
      }
      else if(gesture_data==1){
        uLCD.printf("angle %d ",ANGLE_1);
      } 
      else if(gesture_data==2){ 
        uLCD.printf("angle %d ",ANGLE_2);
      }
      else  {
        uLCD.printf("angle x        ");
      }
      
    }
    else if(mode == MODE_TILT){
      //LED
      myled3=0;
      if( tiltMode==TILT_DETECT )
        myled1 = 1;
      else
        myled1 = 0;

      //mode and angle detection
      //uLCD.cls();
      //uLCD.reset();
      uLCD.locate(4,4);
      uLCD.printf("MODE_TILT");
      if(tiltMode==TILT_UNINIT){
        uLCD.locate(4,6);
        uLCD.printf("uninitialized");
        uLCD.locate(4,8);
        uLCD.printf("angle : %.2f",angle);
      }
      else{
        uLCD.locate(4,6);
        uLCD.printf("initialized");
        uLCD.locate(4,8);
        uLCD.printf("angle : %.2f",angle);
      }
    }
    else{}
}
Menu menu;




//mine: function definition
void capture(Arguments *in, Reply *out);
RPCFunction rpcCapture(&capture, "capture");
void retrieve(Arguments *in, Reply *out);
RPCFunction rpcRetrieve(&retrieve, "retrieve");
void  LedLcd();
int   AngleSelect();
void  AngleDetect();
void  NetworkSetup();
void Analyze();






//
int main() {


  //user button
  btnRecord.fall(queue.event(startRecord));
  btnRecord.rise(queue.event(stopRecord));


  //led and lcd thread
  myled1 = 0;
  gesture_data = 3;
  ThreadMenu.start(LedLcd);
  

  //TODO: revise host to your IP
  const char* host = "192.168.43.41";//mine: ip
  printf("Connecting to TCP network...\r\n");

  SocketAddress sockAddr;
  sockAddr.set_ip_address(host);
  sockAddr.set_port(1883);

  printf("address is %s/%d\r\n", (sockAddr.get_ip_address() ? sockAddr.get_ip_address() : "None"),  (sockAddr.get_port() ? sockAddr.get_port() : 0) ); //check setting

  int rc = mqttNetwork.connect(sockAddr);//(host, 1883);
  if (rc != 0) {
          printf("Connection error.");
          return -1;
  }
  printf("Successfully connected!\r\n");

  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  data.MQTTVersion = 3;
  data.clientID.cstring = "Mbed";

  if ((rc = client.connect(data)) != 0){
          printf("Fail to connect MQTT\r\n");
  }
  if (client.subscribe(topic, MQTT::QOS0, messageArrived) != 0){
          printf("Fail to subscribe\r\n");
  }
  mqtt_thread.start(callback(&mqtt_queue, &EventQueue::dispatch_forever));


  //interrupt user_button
  UserBtn.rise(mqtt_queue.event(&publish_message, &client));




  //The mbed RPC classes are now wrapped to create an RPC enabled version - see RpcClasses.h so don't add to base class
  // receive commands, and send back the responses
  char buf[256], outbuf[256];
  FILE *devin = fdopen(&pc, "r");
  FILE *devout = fdopen(&pc, "w");
  while(1) {
      memset(buf, 0, 256);
      for (int i = 0; ; i++) {
          char recv = fgetc(devin);
          if (recv == '\n') {
              printf("\r\n");
              break;
          }
          buf[i] = fputc(recv, devout);
      }
      //Call the static call method on the RPC class
      RPC::call(buf, outbuf);
      printf("%s\r\n", outbuf);
  }
}







//mine: function declaratoin
void capture(Arguments *in, Reply *out){
  //mode
  mode = MODE_CAPTURE;
  
  //thread
  ThreadAngleSelect.start(AngleSelect);
  

  
}

void retrieve(Arguments *in, Reply *out){

  
}

void LedLcd(){
  while(true){
    menu.show();
    ThisThread::sleep_for(1ms);
  }
}

void AngleDetect(){
  
  
  while(true){
    if(mode == ){
      //initialize
      BSP_ACCELERO_Init();
      int16_t pDataXYZ[3] = {0};
      BSP_ACCELERO_AccGetXYZ(pDataXYZ);
      
      
      //calculate 
      
      
      //tiltMode
      if( tiltMode==TILT_UNINIT ){
        if( (pDataXYZ[0]>-10)&&(pDataXYZ[0]<10) &&
            (pDataXYZ[1]>-10)&&(pDataXYZ[1]<10) &&
            (pDataXYZ[2]>GRAVITY-3)&&(pDataXYZ[2]<GRAVITY+3)  ){
          tiltMode = TILT_DETECT;
        }
      }
      else if( tiltMode==TILT_DETECT){
        if( angle > angleDetermine ){
          //mine: queue put in publish_message
          //mqtt_queue.call( publish_message,  );
          //mqtt_queue.event(&publish_message, &client);
          angleOver++;
          mqtt_queue.call(&publish_message, &client);
          //publish_message(client);
        }
      }
      else{}
      
      ThisThread::sleep_for(300ms);
    }
    else{
      ThisThread::sleep_for(500ms);
    }
  }
  
  
}

int AngleSelect(){
  
  // Whether we should clear the buffer next time we fetch data
  bool should_clear_buffer = false;
  bool got_data = false;

  // The gesture index of the prediction
  int gesture_index;

  // Set up logging.
  static tflite::MicroErrorReporter micro_error_reporter;
  tflite::ErrorReporter* error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  const tflite::Model* model = tflite::GetModel(g_magic_wand_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report(
        "Model provided is schema version %d not equal "
        "to supported version %d.",
        model->version(), TFLITE_SCHEMA_VERSION);
    return -1;
  }

  // Pull in only the operation implementations we need.
  // This relies on a complete list of all the ops needed by this graph.
  // An easier approach is to just use the AllOpsResolver, but this will
  // incur some penalty in code space for op implementations that are not
  // needed by this graph.
  static tflite::MicroOpResolver<6> micro_op_resolver;
  micro_op_resolver.AddBuiltin( tflite::BuiltinOperator_DEPTHWISE_CONV_2D,
                                tflite::ops::micro::Register_DEPTHWISE_CONV_2D());
  micro_op_resolver.AddBuiltin( tflite::BuiltinOperator_MAX_POOL_2D,
                                tflite::ops::micro::Register_MAX_POOL_2D());
  micro_op_resolver.AddBuiltin( tflite::BuiltinOperator_CONV_2D,
                                tflite::ops::micro::Register_CONV_2D());
  micro_op_resolver.AddBuiltin( tflite::BuiltinOperator_FULLY_CONNECTED,
                                tflite::ops::micro::Register_FULLY_CONNECTED());
  micro_op_resolver.AddBuiltin( tflite::BuiltinOperator_SOFTMAX,
                                tflite::ops::micro::Register_SOFTMAX());
  micro_op_resolver.AddBuiltin( tflite::BuiltinOperator_RESHAPE,
                                tflite::ops::micro::Register_RESHAPE(), 1);

  // Build an interpreter to run the model with
  static tflite::MicroInterpreter static_interpreter(model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
  tflite::MicroInterpreter* interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors
  interpreter->AllocateTensors();

  // Obtain pointer to the model's input tensor
  TfLiteTensor* model_input = interpreter->input(0);
  if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1) ||
      (model_input->dims->data[1] != config.seq_length) ||
      (model_input->dims->data[2] != kChannelNumber) ||
      (model_input->type != kTfLiteFloat32)) {
    error_reporter->Report("Bad input tensor parameters in model");
    return -1;
  }

  int input_length = model_input->bytes / sizeof(float);

  TfLiteStatus setup_status = SetupAccelerometer(error_reporter);
  if (setup_status != kTfLiteOk) {
    error_reporter->Report("Set up failed\n");
    return -1;
  }

  error_reporter->Report("Set up successful...\n");

  

  //capture accelero
  myled1 =1;
  for(int i=0;i<20;i++){
    BSP_ACCELERO_Init();
    int16_t pDataXYZ[3] = {0};
    BSP_ACCELERO_AccGetXYZ(pDataXYZ);
    CaptureData[i][0] = pDataXYZ[0];
    CaptureData[i][1] = pDataXYZ[1];
    CaptureData[i][2] = pDataXYZ[2];
    ThisThread::sleep_for(500ms);
  }
  myled1 =0;
  Analyze();


  //gesture
  while (true) {
    if(mode == MODE_CAPTURE)

      

      // Attempt to read new data from the accelerometer
      got_data = ReadAccelerometer(error_reporter, model_input->data.f,
                                  input_length, should_clear_buffer);

      // If there was no new data,
      // don't try to clear the buffer again and wait until next time
      if (!got_data) {
        should_clear_buffer = false;
        continue;
      }

      // Run inference, and report any error
      TfLiteStatus invoke_status = interpreter->Invoke();
      if (invoke_status != kTfLiteOk) {
        error_reporter->Report("Invoke failed on index: %d\n", begin_index);
        continue;
      }

      // Analyze the results to obtain a prediction
      gesture_index = PredictGesture(interpreter->output(0)->data.f);
      
      //mine: get which gesture
      gesture_data =  (gesture_index==0) ? gesture_index+1 :
                      (gesture_index==1) ? gesture_index+1 :
                      (gesture_index==2) ? 0 :
                      gesture_data;


      // Clear the buffer next time we read data
      should_clear_buffer = gesture_index < label_num;

      // Produce an output
      if (gesture_index < label_num) {
        error_reporter->Report(config.output_message[gesture_index]);

      }
    }
    else{
      ThisThread::sleep_for(500ms);
    }
  }
  
}

void Analyze(){

  //count
  int count =0;
  for(those data){
    angle = ( acos(pDataXYZ[2]/GRAVITY)*180.0 )/PI;

    if( (angle > 30) && (angle<60) ){
      count++
    }
  }

  //feature
  if(count<20){//feature 1, 30 ~ 60 , < 20 times
    feature =1;
  }
  else if( (count>20)&&(count<40) ){//feature 2, 30 ~ 60 , 20~40 times
    feature =2;
  }
  else{ //feature 3, 30 ~ 60 , >60 times
    feature =3;
  }
}
