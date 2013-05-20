/**
 * Copyright (c) 2011 Aldebaran Robotics
 */

#include "bumper.h"

#include <alvalue/alvalue.h>
#include <alcommon/alproxy.h>
#include <alcommon/albroker.h>
#include <qi/log.hpp>
#include <althread/alcriticalsection.h>

Bumper::Bumper(boost::shared_ptr<AL::ALBroker> broker,const std::string& name):
    AL::ALModule(broker, name),fCallbackMutex(AL::ALMutex::createALMutex())
{//description du module Bumper
  setModuleDescription("This module presents how to subscribe to a simple event (here RightBumperPressed) and use a callback method.");

  functionName("onRightBumperPressed", getName(), "Method called when the right bumper is pressed. Makes a LED animation.");
  BIND_METHOD(Bumper::onRightBumperPressed)//pour que la méthode soit visible
}

Bumper::~Bumper() {
  fMemoryProxy.unsubscribeToEvent("onRightBumperPressed", "Bumper");
}

void Bumper::init() {
  try {
    /** Create a proxy to ALMemory.
    */
    fMemoryProxy = AL::ALMemoryProxy(getParentBroker());

    fState = fMemoryProxy.getData("FrontTactilTouched");
    /** Subscribe to event LeftBumperPressed
    * Arguments:
    * - name of the event
    * - name of the module to be called for the callback
    * - name of the bound method to be called on event
    */
    fMemoryProxy.subscribeToEvent("FrontTactilTouched", "Bumper",
                                  "onRightBumperPressed");//1: nom de l'évènement 2-nom du module qui va être appelé
  }                                                         //3-la méthode qui va être appelée
  catch (const AL::ALError& e) {
    qiLogError("module.example") << e.what() << std::endl;
  }
}

void Bumper::onRightBumperPressed() {
  qiLogInfo("module.example") << "Executing callback method on right bumper event" << std::endl;
  /**
  * As long as this is defined, the code is thread-safe.
  */
  AL::ALCriticalSection section(fCallbackMutex);

  /**
  * Check that the bumper is pressed.
  */
  fState =  fMemoryProxy.getData("FrontTactilTouched");
  if (fState  > 0.5f) {
    return;
  }
  try {
      fTtsProxy = AL::ALTextToSpeechProxy(getParentBroker());
      fTtsProxy.setLanguage("English");
      fTtsProxy.say("Start recording");
      vrecorderproxy= AL::ALVideoRecorderProxy(getParentBroker());
      vrecorderproxy.startRecording("/home/nao/recordings/cameras", "myvideo");
      sleep(5);
      vrecorderproxy.stopRecording();
      fTtsProxy.say("End recording");
      fMemoryProxy.raiseEvent("STIPEvent",1);
      qiLogInfo("Bumper") << "Bumper :STIPEvent" << std::endl;
      //faut faire unsubscribe pour éviter 2 appels successifs

      //fMemoryProxy.raiseEvent("SVMEvent",1);
      //qiLogInfo("Bumper") << "[Bumper] :SVMEvent" << std::endl;

  }
  catch (const AL::ALError& e) {
    qiLogError("module.example") << e.what() << std::endl;
  }
}
