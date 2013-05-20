/**
 * Copyright (c) 2011 Aldebaran Robotics
 */

#include "lib_1.h"
#include "svm-train.h"
#include "svm-predict.h"
#include <alvalue/alvalue.h>
#include <alcommon/alproxy.h>
#include <alcommon/albroker.h>
#include <qi/log.hpp>
#include <string>

Lib_1::Lib_1(boost::shared_ptr<AL::ALBroker> broker,const std::string& name):
    AL::ALModule(broker, name)
{
  //description du module Bumper pour que ça apparait dans la page web
  setModuleDescription("This module presents h.");
  functionName("SVMcallback", getName(), "");
  BIND_METHOD(Lib_1::SVMcallback);

//  functionName("Lib_1generate_model",getName(),"cmt");
//  BIND_METHOD(Lib_1::Lib_1generate_model);//

//  functionName(Lib_1svm_testing,getName(),"cmt");
//  BIND_METHOD(Lib_1::Lib_1svm_testing);//
}
void Lib_1::init(){

    try{

        fMemoryProxy.subscribeToEvent("SVMEvent", "Lib_1","SVMcallback");
    }
    catch (const AL::ALError& e) {
       qiLogError("module.example") << e.what() << std::endl;
     }

}

int Lib_1::Lib_1generate_model(int argc,char **argv){
generate_model(argc,argv);
return 0;
}
int Lib_1::Lib_1svm_testing(int argc,char **argv){
    svm_testing(argc,argv);

return 0;
}

Lib_1::~Lib_1() {

    fMemoryProxy.unsubscribeToEvent("SVMEvent", "Lib_1");
    qiLogInfo("unsubscribeToEvent") << "unsubscribeToEvent: OK" << std::endl;
}
void Lib_1::SVMcallback(){
    try{
    qiLogInfo("Lib1") << "Lib1:called" << std::endl;
    AL::ALTextToSpeechProxy fTtsProxy = AL::ALTextToSpeechProxy(getParentBroker());
    fTtsProxy.say("appel de Lib SVM");
    char * argv[4]={"svm-predict","/home/nao/data/activities_recognition/stip","/home/nao/data/activities_recognition/training.scale.model","/home/nao/data/activities_recognition/output.predict"};
   /* char **argv=NULL;
    argv =(char **) malloc(4*sizeof(char*));
    for(int i = 0; i<4 ;i++)
    argv[i] = (char *)malloc(256*sizeof(char));

    sprintf(argv[0],"%s","svm-predict");

    sprintf(argv[1],"%s","/home/nao/data/activities_recognition/testing.scale");
    sprintf(argv[2],"%s","/home/nao/data/activities_recognition/training.scale.model");
    sprintf(argv[3],"%s","/home/nao/data/activities_recognition/output.predict");*/
    Lib_1svm_testing(4,argv);
    qiLogInfo("Lib_1svm_testing") << "Lib_1svm_testing:executed OK" << std::endl;
    /* traitement à faire pour déterminer le type de l'activité
      */
    }
    catch (const AL::ALError& e) {
       qiLogError("Lib_1 error") << e.what() << std::endl;
     }
}
         
